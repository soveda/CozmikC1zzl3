#!/usr/bin/env python3
"""Regenerate the PD waveform lookup table from readable source formulas.

This is intended for experimental waveform work, where editing the giant
`pdWaveLUT` blob directly is awkward and error-prone.
"""

from __future__ import annotations

import argparse
import pathlib
import re


ROOT = pathlib.Path(__file__).resolve().parents[1]
LUT_CPP = ROOT / "C1ZZL3_LUT.cpp"
PD_WAVE_SIZE = 4096
WAVE_NAMES = [
    "saw",
    "square",
    "narrow pulse",
    "double sine",
    "saw pulse",
    "resonant saw window",
    "resonant triangle window",
    "resonant trapezoid window",
]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--write",
        action="store_true",
        help="Write the regenerated pdWaveLUT back into C1ZZL3_LUT.cpp.",
    )
    parser.add_argument(
        "--stdout",
        action="store_true",
        help="Print the regenerated pdWaveLUT block to stdout.",
    )
    return parser.parse_args()


def load_text(path: pathlib.Path) -> str:
    return path.read_text()


def parse_sine_lut(text: str) -> list[int]:
    match = re.search(
        r"const int16_t sineLUT\[SINE_SIZE\] = \{(.*?)\};\s*const int16_t phaseWarpLUT",
        text,
        re.S,
    )
    if not match:
        raise RuntimeError("Could not locate sineLUT in C1ZZL3_LUT.cpp")

    values = [int(token) for token in re.findall(r"-?\d+", match.group(1))]
    if len(values) != 1024:
        raise RuntimeError(f"Expected 1024 sine samples, found {len(values)}")
    return values


def clip(value: int) -> int:
    return max(-2048, min(2047, value))


def get_sine(phase: int, sine_lut: list[int]) -> int:
    index = (phase >> 22) & 1023
    frac = (phase >> 12) & 1023
    a = sine_lut[index]
    b = sine_lut[(index + 1) & 1023]
    return a + (((b - a) * frac) >> 10)


def phase12(phase: int) -> int:
    return (phase >> 20) & 4095


def narrow_pulse_wave(phase: int, sine_lut: list[int]) -> int:
    p = phase12(phase)
    if p < 128:
        return 2047
    if 2048 <= p < 2176:
        return -2048
    return get_sine(phase, sine_lut) >> 3


def saw_pulse_wave(phase: int) -> int:
    p = phase12(phase)
    if p < 2304:
        rise = (p * 4095) // 2304
        curve = 4095 - (((4095 - rise) * (4095 - rise)) >> 12)
        return curve - 2048
    return -2048


def resonant_saw_window_wave(phase: int, sine_lut: list[int]) -> int:
    p = phase12(phase)
    envelope = 4095 - p
    overtone = get_sine((phase * 6) & 0xFFFFFFFF, sine_lut)
    body = (((p - 2048) * envelope) >> 13)
    return clip(body + ((overtone * envelope) >> 12))


def resonant_triangle_window_wave(phase: int, sine_lut: list[int]) -> int:
    p = phase12(phase)
    envelope = p << 1 if p < 2048 else (4095 - p) << 1
    triangle = (p << 1) - 2048 if p < 2048 else 6143 - (p << 1)
    overtone = get_sine((phase * 5) & 0xFFFFFFFF, sine_lut)
    body = (triangle * envelope) >> 14
    return clip(body + ((overtone * envelope) >> 12))


def resonant_trapezoid_window_wave(phase: int, sine_lut: list[int]) -> int:
    p = phase12(phase)
    if p < 768:
        envelope = (p * 4095) // 768
    elif p < 3072:
        envelope = 4095
    else:
        envelope = ((4095 - p) * 4095) // 1023

    overtone = (
        get_sine((phase * 7) & 0xFFFFFFFF, sine_lut) +
        (get_sine((phase * 8) & 0xFFFFFFFF, sine_lut) >> 1)
    ) >> 1
    body = envelope >> 4
    return clip(body + ((overtone * envelope) >> 12))


def cz_wave(phase: int, wave: int, sine_lut: list[int]) -> int:
    saw = phase12(phase) - 2048
    square = 2047 if (phase & 0x80000000) else -2048
    if wave == 0:
        return saw
    if wave == 1:
        return square
    if wave == 2:
        return narrow_pulse_wave(phase, sine_lut)
    if wave == 3:
        return get_sine((phase << 1) & 0xFFFFFFFF, sine_lut)
    if wave == 4:
        return saw_pulse_wave(phase)
    if wave == 5:
        return resonant_saw_window_wave(phase, sine_lut)
    if wave == 6:
        return resonant_triangle_window_wave(phase, sine_lut)
    return resonant_trapezoid_window_wave(phase, sine_lut)


def build_pd_wave_lut(sine_lut: list[int]) -> list[list[int]]:
    lut: list[list[int]] = []
    for wave in range(8):
        row = []
        for p in range(PD_WAVE_SIZE):
            phase = (p & 4095) << 20
            row.append(cz_wave(phase, wave, sine_lut))
        lut.append(row)
    return lut


def format_pd_wave_lut(lut: list[list[int]]) -> str:
    lines = ["const int16_t pdWaveLUT[PD_WAVE_COUNT][PD_WAVE_SIZE] = {"]
    for wave_index, row in enumerate(lut):
        lines.append(f"  {{ // wave {wave_index}: {WAVE_NAMES[wave_index]}")
        for offset in range(0, len(row), 16):
            chunk = ", ".join(f"{value:6d}" for value in row[offset:offset + 16])
            suffix = "," if offset + 16 < len(row) else ""
            lines.append(f"    {chunk}{suffix}")
        lines.append("  }," if wave_index < len(lut) - 1 else "  }")
    lines.append("};")
    return "\n".join(lines)


def replace_pd_wave_lut(text: str, replacement: str) -> str:
    pattern = re.compile(
        r"const int16_t pdWaveLUT\[PD_WAVE_COUNT\]\[PD_WAVE_SIZE\] = \{.*?\n\};",
        re.S,
    )
    updated, count = pattern.subn(replacement, text, count=1)
    if count != 1:
        raise RuntimeError("Could not replace pdWaveLUT block in C1ZZL3_LUT.cpp")
    return updated


def main() -> int:
    args = parse_args()
    original = load_text(LUT_CPP)
    sine_lut = parse_sine_lut(original)
    pd_wave_lut = build_pd_wave_lut(sine_lut)
    replacement = format_pd_wave_lut(pd_wave_lut)

    if args.stdout or not args.write:
        print(replacement)

    if args.write:
        LUT_CPP.write_text(replace_pd_wave_lut(original, replacement))

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
