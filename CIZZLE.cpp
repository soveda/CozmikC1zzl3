//
//  CIZZLE.cpp
//
//
//  Created by Adrian Vos on 07/06/2026.
//


#include "ComputerCard.h"

class CIZZLE : public ComputerCard
{
public:
    CIZZLE()
    {
        phase1 = 0;
        phase2 = 0;

        env1 = env2 = 0;

        turing = 0xACE1u;
        turingLen = 16;
        turingProb = 0;
        turingClockDiv = 0;
        turingClock = 0;

        family = 0;
    }

    // =========================
    // MAIN AUDIO CALLBACK
    // =========================
    void ProcessSample() override final
    {
        // -------------------------
        // INPUTS
        // -------------------------
        int32_t in1 = AudioIn1();
        int32_t in2 = AudioIn2();

        int32_t cv1 = CVIn1();
        int32_t cv2 = CVIn2();

        bool sync = PulseIn2RisingEdge();

        // -------------------------
        // MODE
        // -------------------------
        Switch mode = SwitchVal();

        // -------------------------
        // KNOBS
        // -------------------------
        int32_t mainKnob = KnobVal(Knob::Main);
        int32_t xKnob    = KnobVal(Knob::X);
        int32_t yKnob    = KnobVal(Knob::Y);

        // =========================
        // MODE: PD SYNTH (MIDDLE + DOWN)
        // =========================
        if (mode != Switch::Up)
        {
            bool alt = (mode == Switch::Down);

            // ---- Osc1 controls ----
            int32_t pitch = mainKnob + (in1 << 2) + (cv1 << 2);
            int32_t pdAmt = xKnob + (in2 << 1) + (cv2 << 1);
            int32_t famCV = yKnob;

            family = (famCV >> 9) & 7;

            int32_t osc1 = oscPD(phase1, pitch, pdAmt, family);
            int32_t osc2 = oscPD(phase2, pitch, 0, family); // base detuned first

            // ---- ALT MODE (Down) ----
            if (alt)
            {
                int32_t detune = mainKnob - 2048;
                int32_t ringAmt = xKnob;
                int32_t noiseAmt = yKnob;

                osc2 = oscPD(phase2, pitch + (detune >> 3), pdAmt >> 1, family);

                int32_t ring = (osc1 * osc2) >> 12;
                osc1 = mix(osc1, ring, ringAmt >> 6);

                int32_t noise = ((int32_t)fastNoise() - 128) << 4;
                osc1 = mix(osc1, noise, noiseAmt >> 6);
            }

            // Hard sync from Osc2 → Osc1
            if (sync)
                phase1 = phase2 & PHASE_MASK;

            AudioOut1(clip(osc1));
            AudioOut2(clip(osc2));

            // CV outs unused in synth mode → safe modulation mirrors
            CVOut1((osc1 >> 2));
            CVOut2((osc2 >> 2));
        }

        // =========================
        // MODE: TURING MACHINE (UP)
        // =========================
        else
        {
            // ---- clock ----
            bool extClock = PulseIn1RisingEdge();

            turingClockDiv++;
            if (turingClockDiv > (yKnob >> 6))
            {
                turingClockDiv = 0;

                if (extClock)
                    stepTuring();
            }

            // ---- controls ----
            turingProb = mainKnob >> 4;
            turingLen  = 4 + (xKnob >> 8);
            if (turingLen > 32) turingLen = 32;

            // ---- outputs ----
            int32_t cvA = ((turing & 0xFFFF) - 32768) >> 4;
            int32_t cvB = smooth(cvA);

            CVOut1(cvA);
            CVOut2(cvB);

            // audio becomes tiny click generator (useful!)
            int32_t click = (turing & 1) ? 1200 : -1200;
            AudioOut1(click);
            AudioOut2(click >> 1);

            // LEDs show shift register
            updateTuringLEDs();
        }

        // -------------------------
        // GLOBAL VISUAL FEEDBACK
        // -------------------------
        LedBrightness(0, family << 9);
        LedBrightness(1, mainKnob);
        LedBrightness(2, xKnob);
        LedBrightness(3, yKnob);
        LedBrightness(4, env1);
        LedBrightness(5, env2);
    }

private:

    // =========================
    // PHASE DISTORTION ENGINE
    // =========================
    static constexpr int32_t PHASE_BITS = 32;
    static constexpr uint32_t PHASE_MASK = 0xFFFFFFFF;

    // 4096-entry sine LUT (recommended static table)
    // but here we keep placeholder assumption:
    // sinLUT[x] returns -2048..2047

    int32_t sinLUT(uint16_t i)
    {
        // Replace with real LUT in production build
        // (kept simple here for clarity)
        int32_t x = (i & 1023);
        int32_t s = (x < 512) ? x : (1024 - x);
        return (s << 2) - 1024;
    }
    
    inline uint32_t phaseDistort(uint32_t phase, uint32_t amount)
    {
        uint32_t p = phase >> 20; // 0–4095

        // normalize
        int32_t x = (int32_t)p - 2048;

        // amount scaling
        int32_t a = amount - 2048;

        // nonlinear warp (CZ-style curvature)
        x += (x * a) >> 12;

        // fold back
        uint32_t out = (uint32_t)(x + 2048);

        return out & 4095;
    }
    
    inline int32_t oscWT(uint32_t &phase, int32_t pitch)
    {
        phase += (pitch << 5);

        uint16_t idx = phase >> 20; // 12-bit table index

        // simple triangle-sine hybrid approximation (cheap + stable)
        uint16_t x = idx & 1023;

        int32_t v = (x < 512) ? x : (1024 - x);

        return (v - 256) << 3;
    }

    // =========================
    // TURING MACHINE
    // =========================
    void stepTuring()
    {
        bool newBit = (turing & 1);

        if ((fastNoise() & 255) < turingProb)
            newBit = !newBit;

        turing >>= 1;
        turing |= (newBit << 15);
    }

    int32_t smooth(int32_t x)
    {
        turingSmooth = (turingSmooth * 15 + x) >> 4;
        return turingSmooth;
    }

    void updateTuringLEDs()
    {
        LedBrightness(0, (turing & 1) ? 4095 : 0);
        LedBrightness(1, (turing & 2) ? 4095 : 0);
        LedBrightness(2, (turing & 4) ? 4095 : 0);
        LedBrightness(3, PulseIn1());
        LedBrightness(4, turingProb << 4);
        LedBrightness(5, turingLen << 6);
    }

    // =========================
    // UTILS
    // =========================
    int32_t mix(int32_t a, int32_t b, int32_t amt)
    {
        return a + ((b - a) * amt >> 12);
    }

    int32_t clip(int32_t x)
    {
        if (x > 2047) return 2047;
        if (x < -2048) return -2048;
        return x;
    }

    uint8_t fastNoise()
    {
        noise = noise * 1664525 + 1013904223;
        return noise >> 24;
    }

private:
    // oscillators
    uint32_t phase1, phase2;

    // envelopes (reserved for future expansion)
    int32_t env1, env2;

    // turing machine
    uint32_t turing;
    uint32_t turingLen;
    uint32_t turingProb;
    uint32_t turingClockDiv;

    int32_t turingSmooth = 0;

    // waveform family
    uint32_t family;

    // noise
    uint32_t noise = 1;
};

// =========================
// ENTRY POINT
// =========================
CIZZLE card;
Run();
}
