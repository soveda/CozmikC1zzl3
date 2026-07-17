import fs from "node:fs";
import path from "node:path";

const inputDir = process.argv[2] || "/Users/adrianvos/Downloads/cz-pack-1";

function unpackNibbles(payload) {
  const bytes = [];
  for (let i = 0; i + 1 < payload.length; i += 2) {
    bytes.push((payload[i] & 0x0f) | ((payload[i + 1] & 0x0f) << 4));
  }
  return bytes;
}

function findFrame(data) {
  const start = data.indexOf(0xf0);
  const end = data.lastIndexOf(0xf7);
  if (start < 0 || end < start) return null;
  return data.subarray(start, end + 1);
}

function dcoRate(byte) {
  const value = byte & 0x7f;
  if (value === 0) return 0;
  if (value === 0x7f) return 99;
  return Math.min(99, Math.floor((99 * value) / 127) + 1);
}

function dcoLevel(byte) {
  const value = byte & 0x7f;
  if (value <= 0x3f) return value;
  if (value >= 0x44 && value <= 0x67) return value - 4;
  return null;
}

function parsePitchEnvelope(bytes, endOffset, envelopeOffset) {
  const endStep = bytes[endOffset] == null ? null : bytes[endOffset] + 1;
  const stages = [];

  for (let i = 0; i < 8; i++) {
    const rateByte = bytes[envelopeOffset + i * 2];
    const levelByte = bytes[envelopeOffset + i * 2 + 1];
    stages.push({
      rate: rateByte == null ? null : dcoRate(rateByte),
      level: levelByte == null ? null : dcoLevel(levelByte),
      down: rateByte != null && (rateByte & 0x80) !== 0,
      sustain: levelByte != null && (levelByte & 0x80) !== 0,
      rawRate: rateByte,
      rawLevel: levelByte,
    });
  }

  return { endStep, stages };
}

function stageSummary(stage, index) {
  const flags = `${stage.down ? "d" : ""}${stage.sustain ? "S" : ""}`;
  return `${index + 1}:${stage.rate}/${stage.level}${flags}`;
}

function candidateScore(bytes) {
  const endOffsets = [20, 37, 54, 77, 94, 111];
  return endOffsets.reduce((score, offset) => {
    const value = bytes[offset];
    return score + (value >= 0 && value <= 7 ? 1 : 0);
  }, 0);
}

function decodeCandidates(frame) {
  return [7, 9].map((offset) => {
    const bytes = unpackNibbles(frame.subarray(offset, frame.length - 1));
    return {
      offset,
      decodedLength: bytes.length,
      score: candidateScore(bytes),
      bytes,
    };
  });
}

const files = fs.readdirSync(inputDir)
  .filter((file) => file.toLowerCase().endsWith(".syx"))
  .sort((a, b) => a.localeCompare(b));

for (const file of files) {
  const data = fs.readFileSync(path.join(inputDir, file));
  const frame = findFrame(data);
  if (!frame) {
    console.log(`${file}: no complete SysEx frame`);
    continue;
  }

  const candidates = decodeCandidates(frame);
  const selected = candidates
    .toSorted((a, b) => (b.score - a.score) || (b.decodedLength - a.decodedLength))[0];
  const dco1 = parsePitchEnvelope(selected.bytes, 54, 55);
  const dco2 = parsePitchEnvelope(selected.bytes, 111, 112);

  console.log(`\n${file}`);
  console.log(`  selected offset ${selected.offset}, decoded ${selected.decodedLength} bytes, score ${selected.score}/6`);
  console.log(`  DCO1 end ${dco1.endStep}: ${dco1.stages.map(stageSummary).join(" ")}`);
  console.log(`  DCO2 end ${dco2.endStep}: ${dco2.stages.map(stageSummary).join(" ")}`);
}
