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
            
            if (pd1 < 0) pd1 = 0;
            if (pd1 > 4095) pd1 = 4095;

            if (pd2 < 0) pd2 = 0;
            if (pd2 > 4095) pd2 = 4095;

            family = 0;

            // -------------------------
            // OSCILLATORS
            // -------------------------
            int32_t osc1 =
                oscCZ(phase1, freq, pd1);

            int32_t freq2 = freq;
            if (alt)
                freq2 += (main - 2048) >> 3;

            int32_t osc2 =
                oscCZ(phase2, freq2, pd2);
            
            // -------------------------
            // ALT MODE MODS
            // -------------------------
            if (alt)
            {
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
    // CZ OSCILLATOR
    // =========================================================

    inline int32_t getSine(uint32_t phase)
    {
        // 1024-entry full-cycle sine table.
        // Top 10 bits select the table index.
        // Next 10 bits interpolate between adjacent samples.
        uint32_t index = (phase >> 22) & 1023;
        uint32_t frac  = (phase >> 12) & 1023;

        int32_t a = sineLUT[index];
        int32_t b = sineLUT[(index + 1) & 1023];

        return a + (((b - a) * (int32_t)frac) >> 10);
    }
    
    inline uint16_t getCosWarp(uint32_t index)
    {
        return phaseWarpLUT[index & 1023];
    }
    
    
    // ---------------------------------------------------------
    // CZ-style smooth breakpoint warp
    // ---------------------------------------------------------
    inline uint32_t czPhaseWarp(uint32_t phase, uint32_t amount)
    {
        uint32_t p = (phase >> 20) & 4095;

        int32_t bp =
            2048 +
            (((int32_t)amount - 2048) >> 1);

        if (bp < 256) bp = 256;
        if (bp > 3840) bp = 3840;

        if (p < (uint32_t)bp)
        {
            uint32_t t = (p << 10) / bp;

            uint32_t c = getCosWarp(t);

            return (c * bp) >> 11;
        }
        else
        {
            uint32_t t =
                ((p - bp) << 10) /
                (4095 - bp);

            uint32_t c = getCosWarp(t);

            return bp +
                (((2047 - c) *
                 (4095 - bp)) >> 11);
        }
    }

    // ---------------------------------------------------------
    // OSCILLATOR
    // ---------------------------------------------------------
    inline int32_t oscCZ(
        uint32_t& phase,
        int32_t freq,
        int32_t pd)
    {
        phase += (uint32_t)freq;

        uint32_t warped =
            czPhaseWarp(phase, pd);

        return getSine(warped << 20);
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
        noise = noise * 1664525u + 1013904223u;
        return noise >> 24;
    }

    // simple frequency mapping (linear placeholder)
    int32_t baseFreq(int32_t x)
    {
        if (x < 0) x = 0;
        if (x > 4095) x = 4095;

        // Temporary audible test range.
        // Not 1V/oct yet, just enough to prove the oscillator.
        return 20000 + ((x * 500000) >> 12);
    }
private:

    uint32_t phase1 = 0;
    uint32_t phase2 = 0;

    uint32_t turing = 0xACE1u;
    int32_t turingSmooth = 0;

    uint32_t noise = 1;

    uint32_t family = 0;
};

// =========================================================
// ENTRY
// =========================================================
C1ZZL3 card;

//int main()
//{
//    card.EnableNormalisationProbe();
//    card.Run();
//}
int main()
{
    card.Run();
}
