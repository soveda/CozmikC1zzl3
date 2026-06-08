#include "ComputerCard.h"
#include "C1ZZL3_LUT.h"

class C1ZZL3 : public ComputerCard
{
public:

    C1ZZL3()
    {
        phase1 = 0;
        phase2 = 0;

        noise = 1;

        family = 0;
    }

    // =========================================================
    // AUDIO CALLBACK
    // =========================================================
    void ProcessSample() override
    {
        int32_t in1 = AudioIn1();
        int32_t in2 = AudioIn2();

        int32_t cv1 = CVIn1();
        int32_t cv2 = CVIn2();

        bool sync = PulseIn2RisingEdge();

        Switch mode = SwitchVal();

        int32_t main = KnobVal(Knob::Main);
        int32_t x    = KnobVal(Knob::X);
        int32_t y    = KnobVal(Knob::Y);

        // =========================
        // PD SYNTH MODE (MID + DOWN)
        // =========================
        if (mode != Switch::Up)
        {
            bool alt = (mode == Switch::Down);

            // -------------------------
            // PITCH (stable scaling)
            // -------------------------
            int32_t freq = baseFreq(main + (in1 << 2) + (cv1 << 2));

            int32_t pd1 = x + (in2 << 1) + (cv2 << 1);
            int32_t pd2 = alt ? (pd1 >> 1) : 0;

            family = 0

            // -------------------------
            // OSCILLATORS
            // -------------------------
            int32_t osc1 = oscPD(phase1, freq, pd1);
            int32_t osc2 = oscPD(phase2, freq, pd2);

            // -------------------------
            // ALT MODE MODS
            // -------------------------
            if (alt)
            {
                int32_t detune = (main - 2048) >> 3;

                osc2 = oscPD(phase2, freq + detune, pd1 >> 1);

                int32_t ring = (osc1 * osc2) >> 11;
                osc1 = mix(osc1, ring, x >> 4);

                int32_t noiseSig = ((int32_t)fastNoise() - 128) << 4;
                osc1 = mix(osc1, noiseSig, y >> 4);
            }

            if (sync)
                phase1 = phase2;

            AudioOut1(clip(osc1));
            AudioOut2(clip(osc2));

            CVOut1(osc1 >> 2);
            CVOut2(osc2 >> 2);
        }

        // =========================
        // TURING MODE
        // =========================
        else
        {
            stepTuring(y);

            int32_t cvA = ((turing & 0xFFFF) - 32768) >> 4;
            int32_t cvB = smooth(cvA);

            CVOut1(cvA);
            CVOut2(cvB);

            AudioOut1((turing & 1) ? 1200 : -1200);
            AudioOut2((turing & 2) ? 600 : -600);

            updateLEDs();
        }

        // LED feedback
        LedBrightness(0, family << 9);
        LedBrightness(1, main);
        LedBrightness(2, x);
        LedBrightness(3, y);
    }

private:

    // =========================================================
    // CZ PHASE DISTORTION ENGINE (OPTIMISED VERSION)
    // =========================================================

    inline int32_t getSine(uint32_t phase)
    {
        return sineLUT[(phase >> 22) & 1023];
    }

    // ---------------------------------------------------------
    // CZ-style smooth breakpoint warp
    // ---------------------------------------------------------
    inline uint32_t phaseWarp(uint32_t phase, uint32_t amount)
    {
        uint32_t p = phase >> 20; // 0..4095

        // smooth nonlinear curvature instead of hard split
        // (key CZ evolution improvement)
        uint32_t center = 2048;

        int32_t dist = (int32_t)p - (int32_t)center;

        // amount controls curvature strength
        dist = (dist * (4096 - (amount >> 1))) >> 12;

        return (uint32_t)(center + dist) & 4095;
    }

    // ---------------------------------------------------------
    // OSCILLATOR
    // ---------------------------------------------------------
    inline int32_t oscPD(uint32_t &phase, int32_t freq, int32_t pd)
    {
        phase += (uint32_t)freq << 4;

        uint32_t warped = phaseWarp(phase, pd);

        return sinLUT(warped);
    }

    // =========================================================
    // TURING MACHINE
    // =========================================================
    void stepTuring(int32_t knob)
    {
        bool flip = (fastNoise() < (uint8_t)(knob >> 4));

        bool bit = turing & 1;
        if (flip) bit ^= 1;

        turing = (turing >> 1) | (bit << 15);
    }

    int32_t smooth(int32_t x)
    {
        turingSmooth = (turingSmooth * 15 + x) >> 4;
        return turingSmooth;
    }

    void updateLEDs()
    {
        LedBrightness(0, turing & 1 ? 4095 : 0);
        LedBrightness(1, turing & 2 ? 4095 : 0);
        LedBrightness(2, turing & 4 ? 4095 : 0);
        LedBrightness(3, PulseIn1());
        LedBrightness(4, family << 9);
        LedBrightness(5, 0);
    }

    // =========================================================
    // UTILS
    // =========================================================

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

    // simple frequency mapping (linear placeholder)
    int32_t baseFreq(int32_t x)
    {
        return 200 + ((x * 800) >> 12);
    }

private:

    uint32_t phase1 = 0;
    uint32_t phase2 = 0;

    uint32_t turing = 0xACE1u;
    int32_t turingSmooth = 0;

    uint8_t noise = 1;

    uint32_t family = 0;
};

// =========================================================
// ENTRY
// =========================================================
C1ZZL3 card;

int main()
{
    card.EnableNormalisationProbe();
    card.Run();
}
