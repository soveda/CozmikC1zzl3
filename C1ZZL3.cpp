//
//  C1ZZL3.cpp
//
//  Fixed + Optimised Phase Distortion Synth for Workshop Computer
//

#include "ComputerCard.h"
#include <math.h>

class C1ZZL3 : public ComputerCard
{
public:
    C1ZZL3()
    {
        phase1 = 0;
        phase2 = 0;

        env1 = env2 = 0;

        turing = 0xACE1u;
        turingLen = 16;
        turingProb = 0;
        turingClockDiv = 0;

        family = 0;
        noise = 1;

        // Build sine LUT once at boot
        for (int i = 0; i < LUT_SIZE; i++)
        {
            float p = (float)i / (float)LUT_SIZE;
            sineLUT[i] = (int16_t)(sinf(p * 6.28318530718f) * 2047.0f);
        }
    }

    // ============================================================
    // AUDIO CALLBACK
    // ============================================================
    void ProcessSample() override
    {
        // INPUTS
        int32_t in1 = AudioIn1();
        int32_t in2 = AudioIn2();

        int32_t cv1 = CVIn1();
        int32_t cv2 = CVIn2();

        bool sync = PulseIn2RisingEdge();

        Switch mode = SwitchVal();

        // KNOBS
        int32_t mainKnob = KnobVal(Knob::Main);
        int32_t xKnob    = KnobVal(Knob::X);
        int32_t yKnob    = KnobVal(Knob::Y);

        // ============================================================
        // PD SYNTH MODE (Middle + Down)
        // ============================================================
        if (mode != Switch::Up)
        {
            bool alt = (mode == Switch::Down);

            int32_t pitch = mainKnob + (in1 >> 1) + (cv1 >> 1);
            int32_t pdAmt = xKnob    + (in2 >> 1) + (cv2 >> 1);
            int32_t famCV = yKnob;

            family = (famCV >> 9) & 7;

            int32_t osc1 = oscPD(phase1, pitch, pdAmt);
            int32_t osc2 = oscWT(phase2, pitch + ((mainKnob - 2048) >> 4));

            if (alt)
            {
                int32_t detune   = (mainKnob - 2048) >> 4;
                int32_t ringAmt  = xKnob;
                int32_t noiseAmt = yKnob;

                osc2 = oscWT(phase2, pitch + detune);

                int32_t ring = (osc1 * osc2) >> 12;
                osc1 = mix(osc1, ring, ringAmt >> 4);

                int32_t noiseSig = ((int32_t)fastNoise() - 128) << 4;
                osc1 = mix(osc1, noiseSig, noiseAmt >> 4);
            }

            if (sync)
                phase1 = phase2 & 0xFFFFFFFF;

            AudioOut1(clip(osc1));
            AudioOut2(clip(osc2));

            CVOut1(osc1 >> 2);
            CVOut2(osc2 >> 2);
        }

        // ============================================================
        // TURING MODE (Up)
        // ============================================================
        else
        {
            bool extClock = PulseIn1RisingEdge();

            turingClockDiv++;
            if (turingClockDiv > (yKnob >> 7))
            {
                turingClockDiv = 0;
                if (extClock) stepTuring();
            }

            turingProb = mainKnob >> 5;
            turingLen  = 4 + (xKnob >> 8);
            if (turingLen > 32) turingLen = 32;

            int32_t cvA = ((turing & 0xFFFF) - 32768) >> 4;
            int32_t cvB = smooth(cvA);

            CVOut1(cvA);
            CVOut2(cvB);

            int32_t click = (turing & 1) ? 1200 : -1200;
            AudioOut1(click);
            AudioOut2(click >> 1);

            updateTuringLEDs();
        }

        // ============================================================
        // VISUALS
        // ============================================================
        LedBrightness(0, family << 9);
        LedBrightness(1, mainKnob);
        LedBrightness(2, xKnob);
        LedBrightness(3, yKnob);
        LedBrightness(4, env1);
        LedBrightness(5, env2);
    }

private:

    // ============================================================
    // CONSTANTS
    // ============================================================
    static constexpr int LUT_SIZE = 2048;
    int16_t sineLUT[LUT_SIZE];

    // ============================================================
    // PHASE DISTORTION ENGINE
    // ============================================================
    inline uint16_t phaseDistort(uint16_t phase, uint16_t amount)
    {
        int32_t p = (int32_t)phase - 2048;
        int32_t a = (int32_t)amount - 2048;

        p += (p * a) >> 12;

        if (p > 2048)  p = 2048 + ((p - 2048) >> 1);
        if (p < -2048) p = -2048 + ((p + 2048) >> 1);

        return (uint16_t)(p + 2048);
    }

    inline int32_t fastSin(uint16_t phase)
    {
        return sineLUT[phase & (LUT_SIZE - 1)];
    }

    inline int32_t oscPD(uint32_t &phase, int32_t pitch, int32_t pd)
    {
        phase += (pitch << 5);

        uint16_t base = phase >> 20;
        uint16_t warped = phaseDistort(base, pd);

        return fastSin(warped);
    }

    inline int32_t oscWT(uint32_t &phase, int32_t pitch)
    {
        phase += (pitch << 5);

        uint16_t p = phase >> 20;
        uint16_t x = p & 1023;

        int32_t v = (x < 512) ? x : (1024 - x);

        return (v - 256) << 3;
    }

    // ============================================================
    // TURING MACHINE
    // ============================================================
    void stepTuring()
    {
        bool bit = (turing & 1);

        if ((fastNoise() & 255) < turingProb)
            bit = !bit;

        turing >>= 1;
        turing |= (bit << 15);
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

    // ============================================================
    // UTILS
    // ============================================================
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
    uint32_t phase1, phase2;

    int32_t env1, env2;

    uint32_t turing;
    uint32_t turingLen;
    uint32_t turingProb;
    uint32_t turingClockDiv;

    int32_t turingSmooth = 0;

    uint32_t family;

    uint32_t noise;
};

// NOTE: No main() here (Workshop Computer handles runtime)

// Optional external instance if needed by build system
C1ZZL3 card;
