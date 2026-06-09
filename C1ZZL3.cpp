#include "ComputerCard.h"
#include "C1ZZL3_LUT.h"
#include "hardware/sync.h"

class C1ZZL3 : public ComputerCard
{
public:

    C1ZZL3()
    {
        phase1 = 0;
        phase2 = 0;

        noise = 1;

        loadPerformanceState();
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

        Switch previousMode = lastMode;

        if (previousMode == Switch::Up && mode == Switch::Down)
            tapTuringClock();
        lastMode = mode;

        // =========================
        // PD SYNTH MODE (MID + DOWN)
        // =========================
        if (mode != Switch::Up)
        {
            bool alt = (mode == Switch::Down);

            if (!alt)
            {
                pitchControl = main;
                pdControl = x;
                waveControl = y;
            }
            else
            {
                if (previousMode != Switch::Down)
                {
                    saveHoldCanSave = (previousMode == Switch::Middle);
                    resetAltPickup(main, x, y);
                }

                updateAltControls(main, x, y);
            }

            // -------------------------
            // PITCH (stable scaling)
            // -------------------------
            int32_t freq = smoothPitch(baseFreq(pitchControl + (in1 << 1)));

            int32_t pd = clamp12(pdControl + (in2 << 1));
            int32_t wave = clamp12(waveControl + (cv1 << 1));

            int32_t ring =
                clamp12(osc2Ring + (alt && cv2 > 0 ? cv2 << 1 : 0));
            int32_t noiseAmt =
                clamp12(osc2Noise + (alt && cv2 < 0 ? (-cv2) << 1 : 0));

            bool pulse2Trigger = PulseIn2RisingEdge();
            if (pulse2Trigger)
            {
                syncOscillators();
                triggerEnvelope();
            }

            int32_t envelopeLevel = updateEnvelope();
            pd = applyEnvelopeToPd(pd, envelopeLevel);

            // -------------------------
            // OSCILLATORS
            // -------------------------
            int32_t osc1 =
                oscCZ(phase1, freq, pd, wave, noiseAmt);

            int32_t freq2 =
                applyDetune(freq, osc2Detune);

            int32_t osc2 =
                oscCZ(phase2, freq2, pd, wave, noiseAmt);

            int32_t osc2Raw = osc2;
            osc2 = (osc2Raw * osc2Level) >> 12;

            int32_t ringDrive = osc2Level;
            if (ringDrive < 2048)
                ringDrive = 2048;
            int32_t ringCarrier = (osc2Raw * ringDrive) >> 12;
            int32_t ringSig = clip((osc1 * ringCarrier) >> 10);
            int32_t ringMix = (ring * 3840) >> 12;
            osc1 = mix(osc1, ringSig, ringMix);

            int32_t ampScale = envelopeAmpScale(envelopeLevel);
            osc1 = (osc1 * ampScale) >> 12;
            osc2 = (osc2 * ampScale) >> 12;

            AudioOut1(clip(osc1));
            AudioOut2(clip(osc2));

            CVOut1(turingCv);
            CVOut2(turingModCv);
            PulseOut1(turingPulse);
            PulseOut2(turingAltPulse);

            updateSynthLEDs(alt, pd, wave);
            updateSaveGesture(alt);
        }

        // =========================
        // TURING MODE
        // =========================
        else
        {
            resetSaveGesture();

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

            updateTuringPulseAge();

            CVOut1(turingCv);
            CVOut2(turingModCv);

            AudioOut1(turingPulse ? 1200 : -1200);
            AudioOut2((turing & 2) ? 600 : -600);

            PulseOut1(turingPulse);
            PulseOut2(turingAltPulse);

            updateTuringLEDs();
        }
    }

private:
    enum class EnvelopePreset : uint8_t
    {
        Off,
        Pluck,
        Swell,
        Bell
    };

    struct EnvelopeShape
    {
        uint32_t attackSamples;
        uint32_t decaySamples;
        int32_t ampDepth;
        int32_t pdDepth;
    };

    static constexpr int32_t TuringCvScale = 3072;
    static constexpr int32_t TuringCvOffset = 512;
    static constexpr EnvelopePreset ActiveEnvelopePreset = EnvelopePreset::Off;
    static constexpr uint32_t SaveMagic = 0x43315A33u; // C1Z3
    static constexpr uint16_t SaveVersion = 1;
    static constexpr uint32_t SaveFlashOffset =
        (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE) &
        ~(FLASH_SECTOR_SIZE - 1u);
    static constexpr uint32_t SaveHoldSamples = 384000u;
    static constexpr uint32_t SaveConfirmSamples = 48000u;

    struct SavedPerformanceState
    {
        uint32_t magic;
        uint16_t version;
        uint16_t size;
        int32_t osc2Detune;
        int32_t osc2Level;
        int32_t osc2Ring;
        int32_t osc2Noise;
        uint32_t checksum;
    };

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
        int32_t wave,
        int32_t noiseAmt)
    {
        phase += (uint32_t)freq;

        uint32_t renderPhase = phase;
        int32_t noisyPd = pd;

        if (noiseAmt > 0)
            applyCZNoise(renderPhase, noisyPd, noiseAmt);

        int32_t pdCurve = responseCurve(noisyPd);

        int32_t sine = getSine(renderPhase);
        int32_t target = morphWave(renderPhase, wave);

        return mix(sine, target, pdCurve);
    }

    inline void applyCZNoise(uint32_t& phase, int32_t& pd, int32_t amount)
    {
        if (++noiseHoldCounter >= 512)
        {
            noiseHoldCounter = 0;
            heldPdNoise = ((int32_t)fastNoise() - 128);
            heldPhaseNoise = ((int32_t)fastNoise() - 128);
        }

        int32_t noiseCurve = responseCurve(amount) >> 1;
        int32_t pdJitter = (heldPdNoise * noiseCurve) >> 10;
        int32_t phaseJitter = heldPhaseNoise * (noiseCurve >> 6);

        pd = clamp12(pd + pdJitter);
        phase += (uint32_t)phaseJitter;
    }

    EnvelopeShape envelopeShape()
    {
        switch (ActiveEnvelopePreset)
        {
            case EnvelopePreset::Pluck:
                return {480u, 12000u, 3072, 1024};
            case EnvelopePreset::Swell:
                return {24000u, 48000u, 2048, 1536};
            case EnvelopePreset::Bell:
                return {240u, 36000u, 3072, 2048};
            case EnvelopePreset::Off:
            default:
                return {1u, 1u, 0, 0};
        }
    }

    void triggerEnvelope()
    {
        if (ActiveEnvelopePreset == EnvelopePreset::Off)
            return;

        envelopeStage = 1;
        envelopeSample = 0;
        envelopeLevel = 0;
    }

    int32_t updateEnvelope()
    {
        if (ActiveEnvelopePreset == EnvelopePreset::Off || envelopeStage == 0)
            return 0;

        EnvelopeShape shape = envelopeShape();

        if (envelopeStage == 1)
        {
            envelopeSample++;
            envelopeLevel = (int32_t)((envelopeSample * 4095u) / shape.attackSamples);

            if (envelopeSample >= shape.attackSamples)
            {
                envelopeStage = 2;
                envelopeSample = 0;
                envelopeLevel = 4095;
            }
        }
        else
        {
            envelopeSample++;

            if (envelopeSample >= shape.decaySamples)
            {
                envelopeStage = 0;
                envelopeSample = 0;
                envelopeLevel = 0;
            }
            else
            {
                envelopeLevel = 4095 -
                    (int32_t)((envelopeSample * 4095u) / shape.decaySamples);
            }
        }

        return clamp12(envelopeLevel);
    }

    int32_t applyEnvelopeToPd(int32_t pd, int32_t level)
    {
        EnvelopeShape shape = envelopeShape();
        return clamp12(pd + ((level * shape.pdDepth) >> 12));
    }

    int32_t envelopeAmpScale(int32_t level)
    {
        EnvelopeShape shape = envelopeShape();
        int32_t reduction = ((4095 - level) * shape.ampDepth) >> 12;
        return clamp12(4095 - reduction);
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
        turingAltPulse = (turing >> 1) & 1u;
        turingPulseAge = 0;

        int32_t unipolar = (int32_t)((turing * 4095u) / mask);
        turingCv = clip((((unipolar - 2048) * TuringCvScale) >> 12) + TuringCvOffset);
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

        uint32_t inverseSpeed = 4095u - (uint32_t)clamp12(speed);
        uint32_t period =
            3000u +
            (((inverseSpeed * inverseSpeed) >> 12) * 57000u >> 12);
        if (period < 3000u) period = 3000u;

        if (!useTappedClock)
            turingClockPeriod = period;

        if (++turingClock >= turingClockPeriod)
        {
            turingClock = 0;
            return true;
        }

        return false;
    }

    void updateTuringPulseAge()
    {
        if ((turingPulse || turingAltPulse) && ++turingPulseAge > 1200)
        {
            turingPulse = false;
            turingAltPulse = false;
        }
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

    void updateSaveGesture(bool alt)
    {
        if (!alt)
        {
            resetSaveGesture();
            updateSaveFeedback();
            return;
        }

        if (saveHoldCanSave)
        {
            if (saveHoldSamples < SaveHoldSamples)
                saveHoldSamples++;

            if (saveHoldSamples >= SaveHoldSamples && !saveCompletedThisHold)
            {
                savePerformanceStateIfChanged();
                saveCompletedThisHold = true;
                saveConfirmSamples = SaveConfirmSamples;
            }
        }

        updateSaveFeedback();
    }

    void updateSaveFeedback()
    {
        if (saveConfirmSamples == 0)
            return;

        saveConfirmSamples--;

        bool on = (saveConfirmSamples & 4096u) != 0;
        for (uint32_t i = 0; i < 6; ++i)
            LedBrightness(i, on ? 4095 : 0);
    }

    void resetSaveGesture()
    {
        saveHoldSamples = 0;
        saveCompletedThisHold = false;
        saveHoldCanSave = false;
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
        if (altMainPickedUp ||
            pickupAltControl(main, altMainEntry, clamp12(osc2Detune + 2048), altMainPickedUp))
        {
            osc2Detune = main - 2048;
            if (osc2Detune > -32 && osc2Detune < 32)
                osc2Detune = 0;

            osc2Level = osc2Detune < 0 ? -osc2Detune : osc2Detune;
            osc2Level <<= 1;
            if (osc2Level > 4095) osc2Level = 4095;
        }

        if (altXPickedUp ||
            pickupAltControl(x, altXEntry, osc2Ring, altXPickedUp))
            osc2Ring = x;

        if (altYPickedUp ||
            pickupAltControl(y, altYEntry, osc2Noise, altYPickedUp))
            osc2Noise = y;
    }

    void resetAltPickup(int32_t main, int32_t x, int32_t y)
    {
        altMainPickedUp = false;
        altXPickedUp = false;
        altYPickedUp = false;
        altMainEntry = main;
        altXEntry = x;
        altYEntry = y;
    }

    bool pickupAltControl(int32_t knob, int32_t entry, int32_t target, bool& pickedUp)
    {
        int32_t moved = knob - entry;
        if (moved < 0)
            moved = -moved;

        int32_t distance = knob - target;
        if (distance < 0)
            distance = -distance;

        if (moved >= 64 && distance <= 96)
            pickedUp = true;

        return pickedUp;
    }

    SavedPerformanceState currentPerformanceState()
    {
        SavedPerformanceState state;
        state.magic = SaveMagic;
        state.version = SaveVersion;
        state.size = sizeof(SavedPerformanceState);
        state.osc2Detune = osc2Detune;
        state.osc2Level = osc2Level;
        state.osc2Ring = osc2Ring;
        state.osc2Noise = osc2Noise;
        state.checksum = 0;
        state.checksum = checksumState(state);

        return state;
    }

    uint32_t checksumState(const SavedPerformanceState& state)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&state);
        uint32_t checksum = 2166136261u;

        for (uint32_t i = 0; i < sizeof(SavedPerformanceState) - sizeof(uint32_t); ++i)
        {
            checksum ^= bytes[i];
            checksum *= 16777619u;
        }

        return checksum;
    }

    const SavedPerformanceState& flashPerformanceState()
    {
        return *reinterpret_cast<const SavedPerformanceState*>(
            XIP_BASE + SaveFlashOffset);
    }

    bool isValidSavedState(const SavedPerformanceState& state)
    {
        return
            state.magic == SaveMagic &&
            state.version == SaveVersion &&
            state.size == sizeof(SavedPerformanceState) &&
            state.checksum == checksumState(state);
    }

    bool savedStateMatches(const SavedPerformanceState& a, const SavedPerformanceState& b)
    {
        return
            a.osc2Detune == b.osc2Detune &&
            a.osc2Level == b.osc2Level &&
            a.osc2Ring == b.osc2Ring &&
            a.osc2Noise == b.osc2Noise;
    }

    void loadPerformanceState()
    {
        const SavedPerformanceState& state = flashPerformanceState();
        if (!isValidSavedState(state))
            return;

        osc2Detune = state.osc2Detune;
        osc2Level = clamp12(state.osc2Level);
        osc2Ring = clamp12(state.osc2Ring);
        osc2Noise = clamp12(state.osc2Noise);
    }

    void savePerformanceStateIfChanged()
    {
        SavedPerformanceState state = currentPerformanceState();
        const SavedPerformanceState& saved = flashPerformanceState();

        if (isValidSavedState(saved) && savedStateMatches(state, saved))
            return;

        uint8_t page[FLASH_PAGE_SIZE];
        for (uint32_t i = 0; i < FLASH_PAGE_SIZE; ++i)
            page[i] = 0xFF;

        const uint8_t* stateBytes = reinterpret_cast<const uint8_t*>(&state);
        for (uint32_t i = 0; i < sizeof(SavedPerformanceState); ++i)
            page[i] = stateBytes[i];

        uint32_t interrupts = save_and_disable_interrupts();
        flash_range_erase(SaveFlashOffset, FLASH_SECTOR_SIZE);
        flash_range_program(SaveFlashOffset, page, FLASH_PAGE_SIZE);
        restore_interrupts(interrupts);
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

        int32_t delta = target - smoothedFreq;
        int32_t step = delta >> 9;

        if (step == 0 && delta != 0)
            step = delta > 0 ? 1 : -1;

        smoothedFreq += step;

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
    bool turingAltPulse = false;
    bool useTappedClock = false;

    uint32_t noise = 1;
    uint32_t noiseHoldCounter = 0;
    int32_t heldPdNoise = 0;
    int32_t heldPhaseNoise = 0;
    uint32_t envelopeSample = 0;
    int32_t envelopeLevel = 0;
    uint8_t envelopeStage = 0;

    int32_t pitchControl = 2048;
    int32_t pdControl = 0;
    int32_t waveControl = 0;
    int32_t smoothedFreq = 0;
    int32_t osc2Detune = 0;
    int32_t osc2Level = 0;
    int32_t osc2Ring = 0;
    int32_t osc2Noise = 0;
    int32_t altMainEntry = 2048;
    int32_t altXEntry = 0;
    int32_t altYEntry = 0;
    bool altMainPickedUp = false;
    bool altXPickedUp = false;
    bool altYPickedUp = false;
    uint32_t saveHoldSamples = 0;
    uint32_t saveConfirmSamples = 0;
    bool saveHoldCanSave = false;
    bool saveCompletedThisHold = false;
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
