const el = {
  file: document.querySelector("#czFile"),
  dropzone: document.querySelector(".dropzone"),
  importStatus: document.querySelector("#importStatus"),
  themeToggle: document.querySelector("#themeToggle"),
  validationBox: document.querySelector("#validationBox"),
  patchTypeBox: document.querySelector("#patchTypeBox"),
  summaryBox: document.querySelector("#summaryBox"),
  decodedPatchBox: document.querySelector("#decodedPatchBox"),
  mappedDraftBox: document.querySelector("#mappedDraftBox"),
  confidenceBox: document.querySelector("#confidenceBox"),
  supportedOutputBox: document.querySelector("#supportedOutputBox"),
  decodedBox: document.querySelector("#decodedBox"),
  draftNameBox: document.querySelector("#draftNameBox"),
  waveBox: document.querySelector("#waveBox"),
  perfBox: document.querySelector("#perfBox"),
  czEnvelopeSummaryBox: document.querySelector("#czEnvelopeSummaryBox"),
  ampDraftBox: document.querySelector("#ampDraftBox"),
  amp2DraftBox: document.querySelector("#amp2DraftBox"),
  pdDraftBox: document.querySelector("#pdDraftBox"),
  pd2DraftBox: document.querySelector("#pd2DraftBox"),
  ampMode: document.querySelector("#ampMode"),
  pdMode: document.querySelector("#pdMode"),
  pitchMode: document.querySelector("#pitchMode"),
  pitchDco1Box: document.querySelector("#pitchDco1Box"),
  pitchDco2Box: document.querySelector("#pitchDco2Box"),
  pitchSelectedBox: document.querySelector("#pitchSelectedBox"),
  openDraft: document.querySelector("#openDraft"),
};

let currentDraft = null;
let lastImportBuffer = null;
let lastImportFileName = "";
let editorTab = null;
let ampMappingMode = "dual";
let pdMappingMode = "dual";
let pitchMappingMode = "dual";
let themeMode = loadThemeMode();
const HANDOFF_KEY = "c1zzl3-dual-amplitude-envelopes-import-draft";
const HANDOFF_QUEUE_KEY = "c1zzl3-dual-amplitude-envelopes-import-queue";
const ENVELOPE_LAB_HEARTBEAT_KEY = "c1zzl3-dual-amplitude-envelopes-envelope-lab-heartbeat";
const HANDOFF_CHANNEL_NAME = "c1zzl3-dual-amplitude-envelopes-handoff";
const THEME_KEY = "c1zzl3-theme-mode";
const LOCAL_EDITOR_URL = "../index.html";
const HOSTED_EDITOR_URL = "https://soveda.github.io/CozmikC1zzl3/experiments/dual-amplitude-envelopes/index.html";
const ENVELOPE_LAB_WINDOW_NAME = "c1zzl3-dual-amplitude-envelopes-envelope-lab";
const HANDOFF_ACTIVE_WINDOW_MS = 5000;
const handoffChannel = typeof BroadcastChannel === "function"
  ? new BroadcastChannel(HANDOFF_CHANNEL_NAME)
  : null;
const PITCH_MAPPING_MODES = {
  dual: "DCO1 -> oscillator 1, DCO2 -> oscillator 2",
  merged: "Merged DCO1 + DCO2 average",
  dco1: "DCO1 pitch only",
  dco2: "DCO2 pitch only",
  difference: "DCO1 / DCO2 difference emphasis"
};
const AMP_MAPPING_MODES = {
  dual: "DCA1 -> Amp1, DCA2 -> Amp2",
  merged: "Merged DCA1 + DCA2 copied to both amps",
  line1: "DCA1 copied to both amps",
  line2: "DCA2 copied to both amps"
};
const PD_MAPPING_MODES = {
  dual: "DCW1 -> PD1, DCW2 -> PD2",
  merged: "Merged DCW1 + DCW2 average",
  line1: "DCW1 phase distortion only",
  line2: "DCW2 phase distortion only"
};
const WAVE_FAMILIES = [
  { label: "Saw", value: 0, hint: "lower CC23 range" },
  { label: "Square", value: 1, hint: "lower-mid CC23 range" },
  { label: "Narrow pulse", value: 2, hint: "pulse-focused region" },
  { label: "Double sine", value: 3, hint: "mid CC23 range" },
  { label: "Saw pulse", value: 4, hint: "lower-mid to mid CC23 range" },
  { label: "Resonant saw window", value: 5, hint: "upper CC23 range" },
  { label: "Resonant triangle window", value: 6, hint: "upper CC23 range" },
  { label: "Resonant trapezoid window", value: 7, hint: "upper CC23 range" },
];
const MIN_DECODED_PATCH_BYTES = 48;
const MAX_DECODED_PATCH_BYTES = 512;
const CZ_FULL_PATCH_BYTES = 128;
const CZ_SEND_PATCH_COMMAND = 0x20;
const CZ_REQUEST_PATCH_COMMAND = 0x10;
const CZ_SECTIONS = {
  dca1End: 20,
  dca1: 21,
  dcw1End: 37,
  dcw1: 38,
  dco1PitchEnd: 54,
  dco1Pitch: 55,
  dca2End: 77,
  dca2: 78,
  dcw2End: 94,
  dcw2: 95,
  dco2PitchEnd: 111,
  dco2Pitch: 112,
};

function loadThemeMode() {
  try {
    const saved = localStorage.getItem(THEME_KEY);
    if (saved === "light" || saved === "dark") return saved;
  } catch {
    /* Keep default theme if saved data is unavailable. */
  }
  return "dark";
}

function saveThemeMode() {
  try {
    localStorage.setItem(THEME_KEY, themeMode);
  } catch {
    /* Theme persistence is optional. */
  }
}

function renderThemeMode() {
  document.body.classList.toggle("theme-light", themeMode === "light");
  el.themeToggle.classList.toggle("is-active", themeMode === "light");
  el.themeToggle.textContent = themeMode === "light" ? "Light" : "Dark";
  el.themeToggle.setAttribute("aria-checked", String(themeMode === "light"));
}

function getEditorUrl() {
  const path = window.location.pathname;
  const isDualPitchImportLab = path.includes("/experiments/dual-amplitude-envelopes/import-lab/");
  if (isDualPitchImportLab && window.location.protocol === "file:") return LOCAL_EDITOR_URL;

  const host = window.location.hostname;
  const isLocalDev = host === "localhost" || host === "127.0.0.1" || host === "::1";
  return isLocalDev ? LOCAL_EDITOR_URL : HOSTED_EDITOR_URL;
}

function setStatus(text, tone = "ok") {
  el.importStatus.textContent = text;
  el.importStatus.dataset.tone = tone;
}

function pulseButton(button, label = "Done") {
  if (!button) return;
  button.dataset.activatedLabel = label;
  button.classList.remove("was-activated");
  void button.offsetWidth;
  button.classList.add("was-activated");
  window.setTimeout(() => {
    button.classList.remove("was-activated");
    delete button.dataset.activatedLabel;
  }, 1200);
}

function toHex(bytes, limit = 32) {
  return Array.from(bytes.slice(0, limit), (b) => b.toString(16).padStart(2, "0")).join(" ");
}

function findSysexFrame(data) {
  let start = -1;
  for (let i = 0; i < data.length; i++) {
    if (data[i] === 0xf0) start = i;
    if (data[i] === 0xf7 && start >= 0) return data.slice(start, i + 1);
  }
  return null;
}

function unpackNibbles(payload) {
  const bytes = [];
  for (let i = 0; i + 1 < payload.length; i += 2) {
    bytes.push((payload[i] & 0x0f) | ((payload[i + 1] & 0x0f) << 4));
  }
  return bytes;
}

function isNibblePayload(payload) {
  return Array.from(payload).every((b) => b >= 0 && b <= 15);
}

function commandName(command) {
  if (command === CZ_SEND_PATCH_COMMAND) return "send patch data";
  if (command === CZ_REQUEST_PATCH_COMMAND) return "request patch data";
  return `unknown command 0x${command.toString(16).padStart(2, "0")}`;
}

function locationName(location) {
  if (location === 0x60) return "current sound / edit buffer";
  if (location >= 0x20 && location <= 0x2f) return `internal memory ${location - 0x1f}`;
  if (location >= 0x00 && location <= 0x0f) return `preset memory ${location + 1}`;
  if (location >= 0x40 && location <= 0x4f) return `cartridge memory ${location - 0x3f}`;
  return `location 0x${location.toString(16).padStart(2, "0")}`;
}

function decodedLengthSupported(length) {
  return length >= MIN_DECODED_PATCH_BYTES && length <= MAX_DECODED_PATCH_BYTES;
}

function scoreCzPatchSections(decodedBytes) {
  if (decodedBytes.length < CZ_FULL_PATCH_BYTES) return 0;
  const endOffsets = [
    CZ_SECTIONS.dca1End,
    CZ_SECTIONS.dcw1End,
    CZ_SECTIONS.dco1PitchEnd,
    CZ_SECTIONS.dca2End,
    CZ_SECTIONS.dcw2End,
    CZ_SECTIONS.dco2PitchEnd
  ];
  return endOffsets.reduce((score, offset) => {
    const value = decodedBytes[offset];
    return score + (value >= 0 && value <= 7 ? 1 : 0);
  }, 0);
}

function analyzePayloadCandidate(frame, offset, label) {
  const payload = frame.slice(offset, -1);
  const nibblePayload = isNibblePayload(payload);
  const evenNibbleCount = payload.length % 2 === 0;
  const decodedBytes = nibblePayload ? unpackNibbles(payload) : [];
  const decodedLengthOk = decodedLengthSupported(decodedBytes.length);
  const sectionScore = scoreCzPatchSections(decodedBytes);

  return {
    offset,
    label,
    payload,
    nibblePayload,
    evenNibbleCount,
    decodedBytes,
    decodedLengthOk,
    sectionScore,
    supported: nibblePayload && evenNibbleCount && decodedLengthOk
  };
}

function analyzeCzFrame(frame) {
  const manufacturer = frame[1];
  const familyA = frame[2];
  const familyB = frame[3];
  const channelByte = frame[4];
  const command = frame[5] ?? 0;
  const location = frame[6] ?? 0;
  const looksCasio = manufacturer === 0x44;
  const looksCz =
    looksCasio &&
    familyA === 0x00 &&
    familyB === 0x00 &&
    channelByte >= 0x70 &&
    channelByte <= 0x7f;
  const channel = looksCz ? (channelByte - 0x70) + 1 : null;
  const candidates = [];

  if (frame.length > 7) {
    candidates.push(analyzePayloadCandidate(frame, 7, "short CZ send data after location byte"));
  }
  if (frame.length > 9) {
    candidates.push(analyzePayloadCandidate(frame, 9, "legacy Import Lab offset after two apparent send bytes"));
  }

  const preferred =
    candidates
      .filter((candidate) => command === CZ_SEND_PATCH_COMMAND && candidate.supported)
      .sort((a, b) =>
        (b.sectionScore - a.sectionScore) ||
        (b.decodedBytes.length - a.decodedBytes.length) ||
        (a.offset - b.offset)
      )[0] ||
    candidates.find((candidate) => candidate.supported) ||
    candidates[0] ||
    analyzePayloadCandidate(frame, 7, "fallback data after location byte");

  return {
    manufacturer,
    familyA,
    familyB,
    channelByte,
    channel,
    command,
    location,
    looksCasio,
    looksCz,
    commandSupported: command === CZ_SEND_PATCH_COMMAND,
    candidates,
    preferred,
    headerBytes: Array.from(frame.slice(0, Math.min(frame.length, preferred.offset)))
  };
}

function summarizeDecodedBytes(bytes, czInfo) {
  const header = bytes.slice(0, 16);
  const tail = bytes.slice(-16);
  const lines = [
    `Decoded bytes: ${bytes.length}`,
    `Head: ${toHex(header, 16)}`,
    `Tail: ${toHex(tail, 16)}`,
    `Non-zero count: ${bytes.filter((b) => b !== 0).length}`,
  ];

  if (czInfo) {
    lines.unshift(
      `CZ header bytes: ${toHex(czInfo.headerBytes, czInfo.headerBytes.length)}`,
      `CZ channel: ${czInfo.channel ?? "not detected"}`,
      `CZ command: ${commandName(czInfo.command)}`,
      `CZ location: ${locationName(czInfo.location)}`,
      `Patch data offset: ${czInfo.preferred.offset} (${czInfo.preferred.label})`
    );

    lines.push(
      "",
      "Payload candidates:",
      ...czInfo.candidates.map((candidate) => {
        const status = candidate.supported ? "usable" : "not selected";
        return `- offset ${candidate.offset}: ${candidate.payload.length} payload bytes, ${candidate.decodedBytes.length} decoded bytes, CZ section score ${candidate.sectionScore}/6, ${status}`;
      })
    );
  }

  return lines.join("\n");
}

function clamp(value, min, max) {
  return Math.max(min, Math.min(max, value));
}

function roundStage(level, time) {
  return {
    level: clamp(Math.round(level), 0, 4095),
    time: clamp(Math.round(time), 1, 192000)
  };
}

function mapByteToLevel(byte, bias = 0) {
  return clamp(Math.round(((byte + bias) / 255) * 4095), 0, 4095);
}

function mapByteToTime(byte, minTime, maxTime) {
  const scaled = byte / 255;
  const eased = scaled * scaled;
  return clamp(Math.round(minTime + (maxTime - minTime) * eased), 1, 192000);
}

function buildStagesFromBytes(bytes, startIndex, levelBias = 0, timeMin = 120, timeMax = 12000) {
  const stages = [];
  for (let i = 0; i < 8; i++) {
    const levelByte = bytes[(startIndex + i) % bytes.length] ?? 0;
    const timeByte = bytes[(startIndex + i + 8) % bytes.length] ?? 0;
    stages.push(roundStage(
      mapByteToLevel(levelByte, levelBias),
      mapByteToTime(timeByte, timeMin, timeMax)
    ));
  }
  return stages;
}

function czRateToTime(rate, minTime = 240, maxTime = 48000) {
  const normalized = clamp(rate, 0, 99) / 99;
  const eased = 1 - (normalized * normalized);
  return clamp(Math.round(minTime + (maxTime - minTime) * eased), 1, 192000);
}

function dcaRate(byte) {
  const value = byte & 0x7f;
  if (value === 0) return 0;
  if (value === 0x7f) return 99;
  return clamp(Math.floor((99 * value) / 119) + 1, 0, 99);
}

function dcaLevel(byte) {
  const value = byte & 0x7f;
  if (value === 0) return 0;
  if (value === 0x7f) return 99;
  return clamp(Math.floor((99 * value) / 127) + 1, 0, 99);
}

function dcwRate(byte) {
  const value = byte & 0x7f;
  if (value === 8) return 0;
  if (value === 77) return 99;
  return clamp(Math.floor((99 * (value - 8)) / 119) + 1, 0, 99);
}

function dcoRate(byte) {
  const value = byte & 0x7f;
  if (value === 0) return 0;
  if (value === 0x7f) return 99;
  return clamp(Math.floor((99 * value) / 127) + 1, 0, 99);
}

function dcoLevel(byte) {
  const value = byte & 0x7f;
  if (value <= 0x3f) return value;
  if (value >= 0x44 && value <= 0x67) return value - 4;
  return clamp(Math.round((value / 127) * 99), 0, 99);
}

function parseCzEnvelope(bytes, endOffset, envelopeOffset, rateMapper, levelMapper) {
  const endStep = clamp((bytes[endOffset] ?? 7) + 1, 1, 8);
  const stages = [];
  for (let i = 0; i < 8; i++) {
    const rateByte = bytes[envelopeOffset + i * 2] ?? 0;
    const levelByte = bytes[envelopeOffset + i * 2 + 1] ?? 0;
    stages.push({
      rate: rateMapper(rateByte),
      level: levelMapper(levelByte),
      down: (rateByte & 0x80) !== 0,
      sustain: (levelByte & 0x80) !== 0,
      rawRate: rateByte,
      rawLevel: levelByte
    });
  }
  return { endStep, stages };
}

function mergeCzEnvelopes(a, b) {
  const merged = a.stages.map((stage, index) => {
    const other = b.stages[index] ?? stage;
    return {
      rate: Math.round((stage.rate + other.rate) / 2),
      level: Math.round((stage.level + other.level) / 2),
      sustain: stage.sustain || other.sustain,
      down: stage.down || other.down,
      inactive: index >= Math.max(a.endStep || 8, b.endStep || 8)
    };
  });
  merged.endStep = Math.max(a.endStep || 8, b.endStep || 8);
  return merged;
}

function cloneCzEnvelopeStages(envelope) {
  const cloned = envelope.stages.map((stage, index) => ({
    rate: stage.rate,
    level: stage.level,
    sustain: stage.sustain,
    down: stage.down,
    inactive: index >= (envelope.endStep || 8)
  }));
  cloned.endStep = envelope.endStep || 8;
  return cloned;
}

function differenceCzPitchStages(a, b) {
  const difference = a.stages.map((stage, index) => {
    const other = b.stages[index] ?? stage;
    const difference = stage.level - other.level;
    return {
      rate: Math.round((stage.rate + other.rate) / 2),
      level: clamp(Math.round(49.5 + difference / 2), 0, 99),
      sustain: stage.sustain || other.sustain,
      down: stage.down || other.down,
      inactive: index >= Math.max(a.endStep || 8, b.endStep || 8)
    };
  });
  difference.endStep = Math.max(a.endStep || 8, b.endStep || 8);
  return difference;
}

function selectPitchStages(czPatch, mode) {
  if (mode === "dco1") return cloneCzEnvelopeStages(czPatch.dco1Pitch);
  if (mode === "dco2") return cloneCzEnvelopeStages(czPatch.dco2Pitch);
  if (mode === "difference") return differenceCzPitchStages(czPatch.dco1Pitch, czPatch.dco2Pitch);
  return mergeCzEnvelopes(czPatch.dco1Pitch, czPatch.dco2Pitch);
}

function selectLineEnvelopeStages(line1Envelope, line2Envelope, mode) {
  if (mode === "line1") return cloneCzEnvelopeStages(line1Envelope);
  if (mode === "line2") return cloneCzEnvelopeStages(line2Envelope);
  return mergeCzEnvelopes(line1Envelope, line2Envelope);
}

function selectPdEnvelopePair(czPatch, mode) {
  if (mode === "dual") {
    return {
      pd1: cloneCzEnvelopeStages(czPatch.dcw1),
      pd2: cloneCzEnvelopeStages(czPatch.dcw2)
    };
  }

  const selected = selectLineEnvelopeStages(czPatch.dcw1, czPatch.dcw2, mode);
  return { pd1: selected, pd2: cloneCzEnvelopeStages(selected) };
}

function selectAmpEnvelopePair(czPatch, mode) {
  if (mode === "dual") {
    return {
      amp1: cloneCzEnvelopeStages(czPatch.dca1),
      amp2: cloneCzEnvelopeStages(czPatch.dca2)
    };
  }

  const selected = selectLineEnvelopeStages(czPatch.dca1, czPatch.dca2, mode);
  return { amp1: selected, amp2: cloneCzEnvelopeStages(selected) };
}

function czEnvelopeToC1Stages(stages, timeMin = 240, timeMax = 48000, neutralLevel = 0) {
  return stages.map((stage) => roundStage(
    stage.inactive ? neutralLevel : (stage.level / 99) * 4095,
    stage.inactive ? 1 : czRateToTime(stage.rate, timeMin, timeMax)
  ));
}

function czSustainStage(stages) {
  const endStep = Number.isFinite(stages.endStep) ? stages.endStep : 8;
  const index = stages.findIndex((stage, stageIndex) => (
    stageIndex < endStep && stage.sustain && !stage.inactive
  ));
  if (index >= 0) return index;
  return clamp(Math.round(endStep) - 1, 0, STAGES - 1);
}

function parseCzPatch(decodedBytes) {
  const dca1 = parseCzEnvelope(decodedBytes, CZ_SECTIONS.dca1End, CZ_SECTIONS.dca1, dcaRate, dcaLevel);
  const dca2 = parseCzEnvelope(decodedBytes, CZ_SECTIONS.dca2End, CZ_SECTIONS.dca2, dcaRate, dcaLevel);
  const dcw1 = parseCzEnvelope(decodedBytes, CZ_SECTIONS.dcw1End, CZ_SECTIONS.dcw1, dcwRate, dcaLevel);
  const dcw2 = parseCzEnvelope(decodedBytes, CZ_SECTIONS.dcw2End, CZ_SECTIONS.dcw2, dcwRate, dcaLevel);
  const dco1Pitch = parseCzEnvelope(decodedBytes, CZ_SECTIONS.dco1PitchEnd, CZ_SECTIONS.dco1Pitch, dcoRate, dcoLevel);
  const dco2Pitch = parseCzEnvelope(decodedBytes, CZ_SECTIONS.dco2PitchEnd, CZ_SECTIONS.dco2Pitch, dcoRate, dcoLevel);

  return {
    dca1,
    dca2,
    dcw1,
    dcw2,
    dco1Pitch,
    dco2Pitch,
    ampStages: mergeCzEnvelopes(dca1, dca2),
    pdStages: mergeCzEnvelopes(dcw1, dcw2),
    pitchStages: mergeCzEnvelopes(dco1Pitch, dco2Pitch),
  };
}

function classifyWave(bytes) {
  const avg = bytes.reduce((sum, b) => sum + b, 0) / Math.max(1, bytes.length);
  const peak = Math.max(...bytes);
  const mid = bytes.slice(32, 64).reduce((sum, b) => sum + b, 0) / 32;

  if (peak > 220 && avg > 110) {
    return WAVE_FAMILIES[5];
  }
  if (mid > 95) {
    return WAVE_FAMILIES[3];
  }
  if (peak > 190) {
    return WAVE_FAMILIES[2];
  }
  if (avg > 80) {
    return WAVE_FAMILIES[4];
  }
  return WAVE_FAMILIES[0];
}

function buildDraftPreset(
  decodedBytes,
  patchName,
  pitchMode = pitchMappingMode,
  ampMode = ampMappingMode,
  pdMode = pdMappingMode
) {
  if (!decodedBytes.length) return null;

  const baseName = patchName
    .replace(/[_-]+/g, " ")
    .replace(/\s+/g, " ")
    .trim() || "Imported CZ patch";

  const wave = classifyWave(decodedBytes);
  const czPatch = parseCzPatch(decodedBytes);
  const ampPair = selectAmpEnvelopePair(czPatch, ampMode);
  const pdPair = selectPdEnvelopePair(czPatch, pdMode);
  const ampMergedEnvelope = czEnvelopeToC1Stages(mergeCzEnvelopes(czPatch.dca1, czPatch.dca2), 140, 24000);
  const amp1Envelope = czEnvelopeToC1Stages(ampPair.amp1, 140, 24000);
  const amp2Envelope = czEnvelopeToC1Stages(ampPair.amp2, 140, 24000);
  const dcwEnvelope = czEnvelopeToC1Stages(pdPair.pd1, 120, 30000);
  const dcw2Envelope = czEnvelopeToC1Stages(pdPair.pd2, 120, 30000);
  const pitchStages = selectPitchStages(czPatch, pitchMode);
  const pitchEnvelope = czEnvelopeToC1Stages(pitchStages, 240, 48000, 2048);
  const dco1PitchEnvelope = czEnvelopeToC1Stages(cloneCzEnvelopeStages(czPatch.dco1Pitch), 240, 48000, 2048);
  const dco2PitchEnvelope = czEnvelopeToC1Stages(cloneCzEnvelopeStages(czPatch.dco2Pitch), 240, 48000, 2048);
  const pitch2Envelope = pitchMode === "dual" ? dco2PitchEnvelope : pitchEnvelope;
  const sustain = [
    czSustainStage(ampPair.amp1),
    czSustainStage(pdPair.pd1),
    czSustainStage(pitchMode === "dual" ? cloneCzEnvelopeStages(czPatch.dco1Pitch) : pitchStages),
    czSustainStage(pitchMode === "dual" ? cloneCzEnvelopeStages(czPatch.dco2Pitch) : pitchStages),
    czSustainStage(pdPair.pd2),
    czSustainStage(ampPair.amp2)
  ];
  const pitchAlternatives = {
    merged: czEnvelopeToC1Stages(mergeCzEnvelopes(czPatch.dco1Pitch, czPatch.dco2Pitch), 240, 48000, 2048),
    dco1: dco1PitchEnvelope,
    dco2: dco2PitchEnvelope,
    difference: czEnvelopeToC1Stages(differenceCzPitchStages(czPatch.dco1Pitch, czPatch.dco2Pitch), 240, 48000, 2048)
  };
  const detune = clamp(Math.round((decodedBytes[48] ?? 128) / 255 * 4095), 0, 4095);
  const ring = clamp(Math.round((decodedBytes[49] ?? 0) / 255 * 1200), 0, 4095);
  const noise = clamp(Math.round((decodedBytes[50] ?? 0) / 255 * 700), 0, 4095);
  const pd = clamp(Math.max(...dcwEnvelope.map((stage) => stage.level)), 0, 4095);

  return {
    name: `${baseName} draft`,
    wave,
    amp: amp1Envelope,
    amp2: amp2Envelope,
    pd: dcwEnvelope,
    pd2: dcw2Envelope,
    pitch: pitchMode === "dual" ? dco1PitchEnvelope : pitchEnvelope,
    pitch2: pitch2Envelope,
    sustain,
    sourceEnvelopes: {
      pitch: pitchMode === "dual" ? dco1PitchEnvelope : pitchEnvelope,
      pitch2: pitch2Envelope,
      pitchAlternatives,
      pitchMapping: {
        mode: pitchMode,
        label: PITCH_MAPPING_MODES[pitchMode] || PITCH_MAPPING_MODES.merged
      },
      ampMapping: {
        mode: ampMode,
        label: AMP_MAPPING_MODES[ampMode] || AMP_MAPPING_MODES.merged
      },
      pdMapping: {
        mode: pdMode,
        label: PD_MAPPING_MODES[pdMode] || PD_MAPPING_MODES.merged
      },
      dcw: dcwEnvelope,
      pd2: dcw2Envelope,
      amp: amp1Envelope,
      amp2: amp2Envelope,
      ampMerged: ampMergedEnvelope,
      sustain,
      cz: czPatch
    },
    performance: { pd, detune, waveform: wave.value, ring, noise },
    confidence: "medium"
  };
}

function formatStages(stages) {
  return stages.map((stage, index) => {
    const sourceEnded = stage.time <= 1 && (stage.level === 0 || stage.level === 2048);
    return `${index + 1}. ${stage.level}, ${stage.time}${sourceEnded ? " (after CZ END)" : ""}`;
  }).join("\n");
}

function formatCzEnvelope(envelope) {
  return [
    `End step: ${envelope.endStep}`,
    ...envelope.stages.map((stage, index) => {
      const flags = [
        stage.down ? "down" : "",
        stage.sustain ? "sustain" : ""
      ].filter(Boolean).join(", ");
      return `${index + 1}. rate ${stage.rate}, level ${stage.level}${flags ? ` (${flags})` : ""}`;
    })
  ].join("\n");
}

function formatCzEnvelopeSummary(cz) {
  const rows = [
    ["DCA1 / Amp1", cz.dca1],
    ["DCA2 / Amp2", cz.dca2],
    ["DCW1 / PD1", cz.dcw1],
    ["DCW2 / PD2", cz.dcw2],
    ["DCO1 / Pitch1", cz.dco1Pitch],
    ["DCO2 / Pitch2", cz.dco2Pitch],
  ];

  return rows.map(([label, envelope]) => {
    const sustainSteps = envelope.stages
      .map((stage, index) => stage.sustain ? index + 1 : null)
      .filter(Boolean);
    const activeSustains = sustainSteps.filter((step) => step <= envelope.endStep);
    const ignoredSustains = sustainSteps.filter((step) => step > envelope.endStep);
    const activeText = activeSustains.length ? activeSustains.join(", ") : "-";
    const ignoredText = ignoredSustains.length ? `; ignored after END: ${ignoredSustains.join(", ")}` : "";
    const stagesWithEnd = Object.assign([...envelope.stages], { endStep: envelope.endStep });
    return `${label}: END ${envelope.endStep}, sustain ${activeText}, C1ZZL3 hold ${czSustainStage(stagesWithEnd) + 1}${ignoredText}`;
  }).join("\n");
}

function renderDraft(draft) {
  if (!draft) {
    el.decodedPatchBox.textContent = "No decoded patch yet.";
    el.mappedDraftBox.textContent = "Amplitude, PD, and 8-wave family mapping will appear here.";
    el.confidenceBox.textContent = "High / Medium / Low";
    el.supportedOutputBox.textContent = "Draft preset for Envelope Lab";
    el.draftNameBox.textContent = "No draft created yet.";
    el.waveBox.textContent = "No draft yet.";
    el.perfBox.textContent = "No draft yet.";
    el.czEnvelopeSummaryBox.textContent = "No draft yet.";
    el.ampDraftBox.textContent = "No draft yet.";
    el.amp2DraftBox.textContent = "No draft yet.";
    el.pdDraftBox.textContent = "No draft yet.";
    el.pd2DraftBox.textContent = "No draft yet.";
    el.pitchDco1Box.textContent = "No draft yet.";
    el.pitchDco2Box.textContent = "No draft yet.";
    el.pitchSelectedBox.textContent = "No draft yet.";
    return;
  }

  el.decodedPatchBox.textContent = `${draft.name} decoded and unpacked into a draft preset.`;
  el.mappedDraftBox.textContent = `8-wave family ${draft.wave.label}, DCA1/DCA2 -> C1ZZL3 Amp1/Amp2, ${draft.sourceEnvelopes.pdMapping.label} -> C1ZZL3 phase distortion, and ${draft.sourceEnvelopes.pitchMapping.label} -> C1ZZL3 pitch.`;
  el.confidenceBox.textContent = draft.confidence;
  el.supportedOutputBox.textContent = "Envelope Lab draft handoff";
  el.draftNameBox.textContent = `${draft.name} (${draft.confidence} confidence)`;
  el.waveBox.textContent = `${draft.wave.label} -> ${draft.wave.hint}`;
  el.perfBox.textContent = `PD ${draft.performance.pd}, wave family ${draft.wave.label}, detune ${draft.performance.detune}, ring ${draft.performance.ring}, noise ${draft.performance.noise}`;
  el.czEnvelopeSummaryBox.textContent = formatCzEnvelopeSummary(draft.sourceEnvelopes.cz);
  el.ampDraftBox.textContent = formatStages(draft.amp);
  el.amp2DraftBox.textContent = formatStages(draft.amp2 || draft.sourceEnvelopes.amp2 || draft.amp);
  el.pdDraftBox.textContent = formatStages(draft.pd);
  el.pd2DraftBox.textContent = formatStages(draft.pd2 || draft.sourceEnvelopes.pd2 || draft.pd);
  el.pitchDco1Box.textContent = formatCzEnvelope(draft.sourceEnvelopes.cz.dco1Pitch);
  el.pitchDco2Box.textContent = formatCzEnvelope(draft.sourceEnvelopes.cz.dco2Pitch);
  el.pitchSelectedBox.textContent = formatStages(draft.sourceEnvelopes.pitch);
}

function createHandoffId() {
  if (globalThis.crypto?.randomUUID) return globalThis.crypto.randomUUID();
  return `${Date.now()}-${Math.random().toString(16).slice(2)}`;
}

function handoffDraft(draft) {
  if (!draft) return null;
  const payload = {
    id: createHandoffId(),
    source: "cz-import-lab",
    createdAt: new Date().toISOString(),
    draft
  };
  localStorage.setItem(HANDOFF_KEY, JSON.stringify(payload));
  localStorage.setItem(HANDOFF_QUEUE_KEY, JSON.stringify(payload));
  handoffChannel?.postMessage({ type: "cz-import-handoff", payload });
  return payload;
}

function createEditorUrl() {
  const editorUrl = getEditorUrl();
  return editorUrl;
}

function envelopeLabLooksOpen() {
  try {
    const timestamp = Number(localStorage.getItem(ENVELOPE_LAB_HEARTBEAT_KEY) || 0);
    return Number.isFinite(timestamp) && Date.now() - timestamp < HANDOFF_ACTIVE_WINDOW_MS;
  } catch {
    return false;
  }
}

function openEditorTab(payload) {
  const editorUrl = createEditorUrl();
  const canReuseTab = editorTab && !editorTab.closed;
  if (!canReuseTab && envelopeLabLooksOpen()) return editorUrl;
  const tab = canReuseTab ? editorTab : window.open(editorUrl, ENVELOPE_LAB_WINDOW_NAME);
  if (!tab) return editorUrl;

  editorTab = tab;
  if (canReuseTab) {
    try {
      if (payload) tab.postMessage({ type: "cz-import-handoff", payload }, "*");
      tab.focus();
    } catch {
      /* If messaging fails, the next opened editor load can still use local storage. */
    }
  } else if (payload) {
    setTimeout(() => {
      try {
        tab.postMessage({ type: "cz-import-handoff", payload }, "*");
      } catch {
        /* If messaging fails, the editor can still fall back to local storage. */
      }
    }, 250);
  }
  return editorUrl;
}

function decodePatch(buffer, fileName) {
  const data = new Uint8Array(buffer);
  const frame = findSysexFrame(data);
  if (!frame) {
    return {
      ok: false,
      tone: "bad",
      validation: "No complete SysEx frame found.",
      patchType: "Unsupported file",
      summary: `${fileName} does not contain a complete SysEx message.`,
      decoded: "The file could not be read as SysEx.",
      draft: null
    };
  }

  const czInfo = analyzeCzFrame(frame);
  const manufacturer = czInfo.manufacturer;
  const payload = czInfo.preferred.payload;
  const nibblePayload = czInfo.preferred.nibblePayload;
  const decodedBytes = czInfo.preferred.decodedBytes;
  const evenNibbleCount = czInfo.preferred.evenNibbleCount;
  const decodedLengthOk = czInfo.preferred.decodedLengthOk;
  const supportedCandidate =
    czInfo.looksCz &&
    czInfo.commandSupported &&
    czInfo.preferred.supported;
  const patchName = fileName.replace(/\.(syx|mid|sysex)$/i, "");
  const confidence = supportedCandidate ? "medium" : "low";
  const draft = supportedCandidate ? buildDraftPreset(decodedBytes, patchName, pitchMappingMode) : null;

  const validationReasons = [];
  if (!czInfo.looksCasio) validationReasons.push("manufacturer was not Casio `0x44`");
  if (czInfo.looksCasio && !czInfo.looksCz) validationReasons.push("header did not match the common CZ family form `F0 44 00 00 7n`");
  if (czInfo.looksCz && !czInfo.commandSupported) validationReasons.push(`command was ${commandName(czInfo.command)}, not send patch data`);
  if (!nibblePayload) validationReasons.push("payload was not nibble-packed");
  if (nibblePayload && !evenNibbleCount) validationReasons.push("nibble payload length was uneven");
  if (nibblePayload && !decodedLengthOk) {
    validationReasons.push(`decoded size was ${decodedBytes.length} bytes, outside the expected draft range`);
  }

  return {
    ok: supportedCandidate,
    tone: supportedCandidate ? "ok" : "warn",
    validation: supportedCandidate
      ? `Casio CZ ${commandName(czInfo.command)} frame found (${frame.length} bytes, ${locationName(czInfo.location)}, data offset ${czInfo.preferred.offset}).`
      : `This file does not look like a supported Casio CZ single-patch draft candidate: ${validationReasons.join("; ")}.`,
    patchType: supportedCandidate
      ? "Casio CZ single-patch send frame"
      : "Unsupported or different synth family",
    summary: [
      `File: ${fileName}`,
      `Frame length: ${frame.length} bytes`,
      `Manufacturer: 0x${manufacturer.toString(16).padStart(2, "0")}`,
      `CZ family bytes: 0x${czInfo.familyA.toString(16).padStart(2, "0")} 0x${czInfo.familyB.toString(16).padStart(2, "0")}`,
      `MIDI channel byte: 0x${czInfo.channelByte.toString(16).padStart(2, "0")}${czInfo.channel ? ` (channel ${czInfo.channel})` : ""}`,
      `Command: ${commandName(czInfo.command)}`,
      `Location: ${locationName(czInfo.location)}`,
      `Selected data offset: ${czInfo.preferred.offset}`,
      `CZ section score: ${czInfo.preferred.sectionScore}/6`,
      `Payload bytes: ${payload.length}`,
      `Nibble-packed payload: ${nibblePayload ? "yes" : "no"}`,
      `Even nibble count: ${evenNibbleCount ? "yes" : "no"}`,
      `Decoded bytes: ${decodedBytes.length}`,
      `Draft name: ${patchName}`,
      `Confidence: ${confidence}`
    ].join("\n"),
    decoded: nibblePayload
      ? summarizeDecodedBytes(decodedBytes, czInfo)
      : [
          "Payload was not nibble-packed.",
          `Raw frame head: ${toHex(frame, 24)}`,
          `Raw payload head: ${toHex(payload, 48)}`
        ].join("\n"),
    draft
  };
}

async function handleFile(file) {
  if (!file) return;
  setStatus(`Reading ${file.name}...`, "warn");
  const buffer = await file.arrayBuffer();
  lastImportBuffer = buffer;
  lastImportFileName = file.name;
  const result = decodePatch(buffer, file.name);
  currentDraft = result.draft;
  el.validationBox.textContent = result.validation;
  el.patchTypeBox.textContent = result.patchType;
  el.summaryBox.textContent = result.summary;
  el.decodedBox.textContent = result.decoded;
  renderDraft(currentDraft);
  setStatus(result.ok ? "Patch decoded. Draft preset created." : "Patch did not match a supported CZ import.", result.tone);
}

function rebuildCurrentDraftForMapping() {
  if (!lastImportBuffer || !lastImportFileName) {
    renderDraft(null);
    setStatus("Choose a CZ patch before selecting envelope sources.", "warn");
    return;
  }

  const result = decodePatch(lastImportBuffer, lastImportFileName);
  currentDraft = result.draft;
  el.validationBox.textContent = result.validation;
  el.patchTypeBox.textContent = result.patchType;
  el.summaryBox.textContent = result.summary;
  el.decodedBox.textContent = result.decoded;
  renderDraft(currentDraft);
  setStatus(result.ok ? `Envelope sources set to ${AMP_MAPPING_MODES[ampMappingMode]}, ${PD_MAPPING_MODES[pdMappingMode]}, and ${PITCH_MAPPING_MODES[pitchMappingMode]}.` : "Patch did not match a supported CZ import.", result.tone);
}

el.file.addEventListener("change", () => handleFile(el.file.files?.[0]));
el.ampMode.addEventListener("change", () => {
  ampMappingMode = el.ampMode.value;
  rebuildCurrentDraftForMapping();
});
el.pdMode.addEventListener("change", () => {
  pdMappingMode = el.pdMode.value;
  rebuildCurrentDraftForMapping();
});
el.pitchMode.addEventListener("change", () => {
  pitchMappingMode = el.pitchMode.value;
  rebuildCurrentDraftForMapping();
});
el.themeToggle.addEventListener("click", () => {
  themeMode = themeMode === "light" ? "dark" : "light";
  saveThemeMode();
  renderThemeMode();
  pulseButton(el.themeToggle, themeMode === "light" ? "Light" : "Dark");
});
el.dropzone.addEventListener("dragover", (event) => {
  event.preventDefault();
});
el.dropzone.addEventListener("drop", (event) => {
  event.preventDefault();
  const file = event.dataTransfer.files?.[0];
  if (file) handleFile(file);
});
el.openDraft.addEventListener("click", () => {
  const payload = handoffDraft(currentDraft);
  if (payload) {
    setStatus("Draft handed off to Envelope Lab without adding it to the URL.", "ok");
    openEditorTab(payload);
    pulseButton(el.openDraft, "Opened");
  } else {
    setStatus("Import a file first so there is a draft to open.", "warn");
  }
});
renderThemeMode();
renderDraft(null);
