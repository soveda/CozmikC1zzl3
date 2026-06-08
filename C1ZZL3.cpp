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

        Switch mode = SwitchVal();

        int32_t main = KnobVal(Knob::Main);
        int32_t x    = KnobVal(Knob::X);
        int32_t y    = KnobVal(Knob::Y);

        if (lastMode == Switch::Up && mode == Switch::Down)
            tapTuringClock();
        lastMode = mode;

        // =========================
        // PD SYNTH MODE (MID + DOWN)
        // =========================
        if (mode != Switch::Up)
        {
            bool alt = (mode == Switch::Down);

            if (!alt)
                pitchControl = main;
            else
                updateAltControls(main, x, y);

            // -------------------------
            // PITCH (stable scaling)
            // -------------------------
            int32_t freq = smoothPitch(baseFreq(pitchControl + (in1 << 1)));

            int32_t pd = clamp12(x + (in2 << 1));
            int32_t wave = clamp12(y + (cv1 << 1));

            int32_t ring = alt ? clamp12(osc2Ring + (cv2 > 0 ? cv2 << 1 : 0)) : 0;
            int32_t noiseAmt = alt ? clamp12(osc2Noise + (cv2 < 0 ? (-cv2) << 1 : 0)) : 0;

            if (PulseIn2RisingEdge())
                syncOscillators();

            // -------------------------
            // OSCILLATORS
            // -------------------------
            int32_t osc1 =
                oscCZ(phase1, freq, pd, wave);

            int32_t freq2 =
                applyDetune(freq, osc2Detune);

            int32_t osc2 =
                oscCZ(phase2, freq2, pd, wave);

            osc2 = (osc2 * osc2Level) >> 12;

            int32_t ringSig = (osc1 * osc2) >> 11;
            osc1 = mix(osc1, ringSig, ring);

            int32_t noiseSig = ((int32_t)fastNoise() - 128) << 4;
            osc1 = mix(osc1, noiseSig, noiseAmt);

            AudioOut1(clip(osc1));
            AudioOut2(clip(osc2));

            CVOut1(turingCv);
            CVOut2(turingModCv);
            PulseOut1(turingPulse);
            PulseOut2(turingPulse);

            updateSynthLEDs(alt, pd, wave);
        }

        // =========================
        // TURING MODE
        // =========================
        else
        {
            turingLength = 2 + ((x * 14) >> 12);
            if (turingLength > 16) turingLength = 16;

            bool externalClock = PulseIn1RisingEdge();
            bool clocked = false;

            if (externalClock)
            {
                externalClockAge = 0;
                clocked = true;
            }
            else
            {
                if (externalClockAge < 96000u)
                    externalClockAge++;
                else
                    clocked = internalTuringClock(y);
            }

            if (clocked)
                stepTuring(main);

            CVOut1(turingCv);
            CVOut2(turingModCv);

            AudioOut1(turingPulse ? 1200 : -1200);
            AudioOut2((turing & 2) ? 600 : -600);

            PulseOut1(turingPulse);
            PulseOut2(turingPulse);

            updateTuringLEDs();
        }
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
        int32_t pd,
        int32_t wave)
    {
        phase += (uint32_t)freq;

        int32_t pdCurve = responseCurve(pd);

        int32_t sine = getSine(phase);
        int32_t target = morphWave(phase, wave);

        return mix(sine, target, pdCurve);
    }

    inline int32_t morphWave(uint32_t phase, int32_t wave)
    {
        uint32_t scaled = ((uint32_t)wave * 7u);
        uint32_t index = scaled >> 12;
        uint32_t frac = smoothStep12(scaled & 4095);

        int32_t a = czWave(phase, index);
        int32_t b = czWave(phase, index < 7 ? index + 1 : 7);

        return a + (((b - a) * (int32_t)frac) >> 12);
    }

    inline int32_t czWave(uint32_t phase, uint32_t wave)
    {
        int32_t saw = ((int32_t)(phase >> 20) & 4095) - 2048;
        int32_t square = (phase & 0x80000000u) ? 2047 : -2048;
        int32_t pulse = narrowPulseWave(phase);
        int32_t doubleSine = getSine(phase << 1);

        switch (wave)
        {
            case 0: return saw;
            case 1: return square;
            case 2: return pulse;
            case 3: return doubleSine;
            case 4: return mix(saw, pulse, 3072);
            case 5: return pluckedResonantWave(phase, 5);
            case 6: return resonantWave(phase, 4, 2);
            default: return resonantWave(phase, 7, 3);
        }
    }

    inline int32_t narrowPulseWave(uint32_t phase)
    {
        uint32_t p = (phase >> 20) & 4095;

        if (p < 128)
            return 2047;

        if (p >= 2048 && p < 2176)
            return -2048;

        return getSine(phase) >> 3;
    }

    inline int32_t pluckedResonantWave(uint32_t phase, uint32_t harmonic)
    {
        uint32_t p = (phase >> 20) & 4095;
        int32_t envelope = (int32_t)(4095 - p);

        envelope = (envelope * envelope) >> 12;
        envelope = (envelope * envelope) >> 12;

        int32_t overtone = getSine(phase * harmonic);
        int32_t click = p < 96 ? 2047 : 0;

        return clip(((overtone * envelope) >> 12) + click - 256);
    }

    inline int32_t resonantWave(uint32_t phase, uint32_t harmonic, uint32_t decay)
    {
        uint32_t p = (phase >> 20) & 4095;
        int32_t envelope = (int32_t)(4095 - p);

        for (uint32_t i = 0; i < decay; ++i)
            envelope = (envelope * envelope) >> 12;

        int32_t overtone = getSine(phase * harmonic);
        int32_t body = getSine(phase) >> (decay + 1);

        return clip(body + ((overtone * envelope) >> 12));
    }

    // =========================================================
    // TURING MACHINE
    // =========================================================
    void stepTuring(int32_t knob)
    {
        uint32_t mask = (1u << turingLength) - 1u;
        uint8_t chance = turingMutationChance(knob);

        bool bit = turing & 1;
        if (fastNoise() < chance) bit ^= 1;

        turing = ((turing >> 1) | ((uint32_t)bit << (turingLength - 1))) & mask;

        turingPulse = bit;
        turingPulseAge = 0;

        int32_t unipolar = (int32_t)((turing * 4095u) / mask);
        turingCv = unipolar - 2048;
        turingModCv = smooth(turingCv);
    }

    uint8_t turingMutationChance(int32_t knob)
    {
        int32_t distance = knob - 2048;
        if (distance < 0) distance = -distance;

        int32_t chance = 255 - (distance >> 3);
        if (chance < 0) chance = 0;
        if (chance > 255) chance = 255;

        return (uint8_t)chance;
    }

    bool internalTuringClock(int32_t speed)
    {
        int32_t moved = speed - lastClockSpeed;
        if (moved < 0) moved = -moved;
        if (moved > 64)
            useTappedClock = false;
        lastClockSpeed = speed;

        uint32_t period = 48000u - (((uint32_t)speed * 47400u) >> 12);
        if (period < 600u) period = 600u;

        if (!useTappedClock)
            turingClockPeriod = period;

        if (++turingClock >= turingClockPeriod)
        {
            turingClock = 0;
            return true;
        }

        if (turingPulse && ++turingPulseAge > 1200)
            turingPulse = false;

        return false;
    }

    void tapTuringClock()
    {
        if (turingClock > 2400u && turingClock < 96000u)
        {
            tappedClockPeriod = turingClock;
            turingClockPeriod = tappedClockPeriod;
            useTappedClock = true;
        }

        turingClock = 0;
    }

    int32_t smooth(int32_t x)
    {
        turingSmooth = (turingSmooth * 15 + x) >> 4;
        return turingSmooth;
    }

    void updateTuringLEDs()
    {
        LedBrightness(0, turing & 1 ? 4095 : 0);
        LedBrightness(1, turing & 2 ? 4095 : 0);
        LedBrightness(2, turing & 4 ? 4095 : 0);
        LedBrightness(3, PulseIn1());
        LedBrightness(4, (turingLength * 4095) >> 4);
        LedBrightness(5, turingPulse ? 4095 : 0);
    }

    void updateSynthLEDs(bool alt, int32_t pd, int32_t wave)
    {
        LedBrightness(0, pd);
        LedBrightness(1, wave);
        LedBrightness(2, osc2Level);
        LedBrightness(3, osc2Ring);
        LedBrightness(4, osc2Noise);
        LedBrightness(5, alt ? 4095 : 0);
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

    int32_t clamp12(int32_t x)
    {
        if (x < 0) return 0;
        if (x > 4095) return 4095;
        return x;
    }

    void updateAltControls(int32_t main, int32_t x, int32_t y)
    {
        osc2Detune = main - 2048;
        if (osc2Detune > -32 && osc2Detune < 32)
            osc2Detune = 0;

        osc2Level = osc2Detune < 0 ? -osc2Detune : osc2Detune;
        osc2Level <<= 1;
        if (osc2Level > 4095) osc2Level = 4095;

        osc2Ring = x;
        osc2Noise = y;
    }

    int32_t applyDetune(int32_t freq, int32_t detune)
    {
        int32_t bend = detune;
        int32_t sign = 1;

        if (bend < 0)
        {
            sign = -1;
            bend = -bend;
        }

        // Small movements give fine beating; the far ends reach wide offsets.
        int32_t fine = (bend * bend) >> 11;
        int32_t offset = (freq * fine) >> 12;

        if (sign < 0)
            return freq - offset;

        return freq + offset;
    }

    void syncOscillators()
    {
        phase1 = 0;
        phase2 = 0;
    }

    uint8_t fastNoise()
    {
        noise = noise * 1664525u + 1013904223u;
        return noise >> 24;
    }

    // Simple audible pitch mapping.
    //
    // This deliberately uses a smooth test range rather than octave
    // zones. It is not calibrated 1V/oct yet, but it feels continuous
    // on the hardware.
    int32_t baseFreq(int32_t x)
    {
        if (x < 0) x = 0;
        if (x > 4095) x = 4095;

        int32_t coarse = (x * 70000000) >> 12;
        int32_t fine = ((x * x) >> 12) * 30000;

        return 6000000 + coarse + fine;
    }

    int32_t smoothPitch(int32_t target)
    {
        if (smoothedFreq == 0)
            smoothedFreq = target;

        smoothedFreq += (target - smoothedFreq) >> 7;

        return smoothedFreq;
    }

    int32_t responseCurve(int32_t x)
    {
        uint32_t ux = (uint32_t)clamp12(x);

        return (ux * ux) >> 12;
    }

    uint32_t smoothStep12(uint32_t x)
    {
        uint32_t x2 = (x * x) >> 12;
        uint32_t x3 = (x2 * x) >> 12;

        return (3u * x2) - (2u * x3);
    }
private:

    uint32_t phase1 = 0;
    uint32_t phase2 = 0;

    uint32_t turing = 0xACE1u;
    int32_t turingSmooth = 0;
    int32_t turingCv = 0;
    int32_t turingModCv = 0;
    uint32_t turingClock = 0;
    uint32_t turingClockPeriod = 12000;
    uint32_t tappedClockPeriod = 12000;
    int32_t lastClockSpeed = 2048;
    uint32_t turingPulseAge = 0;
    uint32_t turingLength = 16;
    uint32_t externalClockAge = 96000u;
    bool turingPulse = false;
    bool useTappedClock = false;

    uint32_t noise = 1;

    int32_t pitchControl = 2048;
    int32_t smoothedFreq = 0;
    int32_t osc2Detune = 0;
    int32_t osc2Level = 0;
    int32_t osc2Ring = 0;
    int32_t osc2Noise = 0;
    Switch lastMode = Switch::Middle;
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
