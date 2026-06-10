# C1ZZL3 Experimental Web MIDI SysEx Protocol

This protocol is experimental and is only implemented by the firmware in
`experimental/web-midi/firmware/`.

The production firmware in the repo root does not use this protocol.

## Transport

- USB MIDI device mode.
- Browser/editor sends Web MIDI SysEx to the card.
- Current firmware command is RAM-only preview. Custom envelope data is not
  persisted to flash.

## Frame

All bytes inside the SysEx frame must be 7-bit clean.

```text
F0 7D 43 31 5A 33 01 pp nn... aa... dd... F7
```

| Bytes | Meaning |
| --- | --- |
| `F0` | SysEx start |
| `7D` | non-commercial/manufacturer development ID |
| `43 31 5A 33` | C1ZZL3 card ID, ASCII `C1Z3` |
| `01` | preview command |
| `pp` | source preset index from the editor, currently informational |
| `nn...` | 16-byte ASCII preset name, space-padded |
| `aa...` | eight amplitude stages |
| `dd...` | eight phase-distortion stages |
| `F7` | SysEx end |

Payload length after the command byte is 97 bytes:

```text
1 preset index
16 name bytes
40 amplitude stage bytes
40 phase-distortion stage bytes
```

The firmware expects 103 bytes between `F0` and `F7`:

```text
1 manufacturer byte
4 card ID bytes
1 command byte
97 payload bytes
```

## Stage Encoding

Each envelope lane has exactly eight stages. Each stage is encoded as:

```text
level_lsb7 level_msb7 time_lsb7 time_mid7 time_msb7
```

`level` is a 14-bit little-endian 7-bit packed integer:

```text
level = level_lsb7 | (level_msb7 << 7)
```

Valid firmware range:

```text
0..4095
```

`time` is a 21-bit little-endian 7-bit packed integer:

```text
time = time_lsb7 | (time_mid7 << 7) | (time_msb7 << 14)
```

Firmware treats `time == 0` as `1` sample.

## Firmware Response

There is no MIDI acknowledgement yet.

LED feedback is used during hardware testing:

- Valid C1ZZL3 preview frame: all six LEDs flash together briefly.
- Rejected/ignored frame: even and odd LEDs alternate briefly.

## Current Limitations

- No SysEx readback.
- No flash persistence for custom envelope data.
- No USB MIDI host mode.
- No MIDI note or pitchbend control of the oscillator.
