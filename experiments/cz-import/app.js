const el = {
  file: document.querySelector("#czFile"),
  dropzone: document.querySelector(".dropzone"),
  importStatus: document.querySelector("#importStatus"),
  validationBox: document.querySelector("#validationBox"),
  patchTypeBox: document.querySelector("#patchTypeBox"),
  summaryBox: document.querySelector("#summaryBox"),
  decodedBox: document.querySelector("#decodedBox"),
  draftNameBox: document.querySelector("#draftNameBox"),
  waveBox: document.querySelector("#waveBox"),
  perfBox: document.querySelector("#perfBox"),
  ampDraftBox: document.querySelector("#ampDraftBox"),
  pdDraftBox: document.querySelector("#pdDraftBox"),
  createDraft: document.querySelector("#createDraft"),
  openDraft: document.querySelector("#openDraft"),
  saveDraft: document.querySelector("#saveDraft"),
};

let currentDraft = null;
const HANDOFF_KEY = "c1zzl3-cz-import-draft";
const LOCAL_EDITOR_URL = "../../web-midi/editor/index.html";
const HOSTED_EDITOR_URL = "https://soveda.github.io/CozmikC1zzl3/web-midi/editor/index.html";

function getEditorUrl() {
  return window.location.hostname.endsWith("github.io") ? HOSTED_EDITOR_URL : LOCAL_EDITOR_URL;
}

function setStatus(text, tone = "ok") {
  el.importStatus.textContent = text;
  el.importStatus.dataset.tone = tone;
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

function summarizeDecodedBytes(bytes) {
  const header = bytes.slice(0, 16);
  const tail = bytes.slice(-16);
  return [
    `Decoded bytes: ${bytes.length}`,
    `Head: ${toHex(header, 16)}`,
    `Tail: ${toHex(tail, 16)}`,
    `Non-zero count: ${bytes.filter((b) => b !== 0).length}`,
  ].join("\n");
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

function classifyWave(bytes) {
  const avg = bytes.reduce((sum, b) => sum + b, 0) / Math.max(1, bytes.length);
  const peak = Math.max(...bytes);
  const mid = bytes.slice(32, 64).reduce((sum, b) => sum + b, 0) / 32;

  if (peak > 220 && avg > 110) {
    return { label: "Bright resonant", value: "upper CC23 range" };
  }
  if (mid > 95) {
    return { label: "Smooth harmonic", value: "mid CC23 range" };
  }
  if (peak > 190) {
    return { label: "Pulse / square-leaning", value: "lower-mid CC23 range" };
  }
  return { label: "Saw-like", value: "lower CC23 range" };
}

function buildDraftPreset(decodedBytes, patchName) {
  if (!decodedBytes.length) return null;

  const baseName = patchName
    .replace(/[_-]+/g, " ")
    .replace(/\s+/g, " ")
    .trim() || "Imported CZ patch";

  const wave = classifyWave(decodedBytes);
  const amp = buildStagesFromBytes(decodedBytes, 0, 0, 140, 14000);
  const pd = buildStagesFromBytes(decodedBytes, 16, 0, 120, 18000);
  const detune = clamp(Math.round((decodedBytes[32] ?? 128) / 255 * 4095), 0, 4095);
  const ring = clamp(Math.round((decodedBytes[33] ?? 0) / 255 * 1200), 0, 4095);
  const noise = clamp(Math.round((decodedBytes[34] ?? 0) / 255 * 700), 0, 4095);

  return {
    name: `${baseName} draft`,
    wave,
    amp,
    pd,
    performance: { detune, ring, noise },
    confidence: "medium"
  };
}

function formatStages(stages) {
  return stages.map((stage, index) => `${index + 1}. ${stage.level}, ${stage.time}`).join("\n");
}

function renderDraft(draft) {
  if (!draft) {
    el.draftNameBox.textContent = "No draft created yet.";
    el.waveBox.textContent = "No draft yet.";
    el.perfBox.textContent = "No draft yet.";
    el.ampDraftBox.textContent = "No draft yet.";
    el.pdDraftBox.textContent = "No draft yet.";
    return;
  }

  el.draftNameBox.textContent = `${draft.name} (${draft.confidence} confidence)`;
  el.waveBox.textContent = `${draft.wave.label} -> ${draft.wave.value}`;
  el.perfBox.textContent = `Detune ${draft.performance.detune}, ring ${draft.performance.ring}, noise ${draft.performance.noise}`;
  el.ampDraftBox.textContent = formatStages(draft.amp);
  el.pdDraftBox.textContent = formatStages(draft.pd);
}

function handoffDraft(draft) {
  if (!draft) return false;
  localStorage.setItem(HANDOFF_KEY, JSON.stringify({
    source: "cz-import-lab",
    createdAt: new Date().toISOString(),
    draft
  }));
  return true;
}

function openEditorTab() {
  const editorUrl = getEditorUrl();
  window.open(editorUrl, "_blank", "noopener,noreferrer");
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

  const manufacturer = frame[1];
  const deviceId = frame[2];
  const modelBytes = Array.from(frame.slice(3, 7));
  const payload = frame.slice(7, -1);
  const nibblePayload = Array.from(payload).every((b) => b >= 0 && b <= 15);
  const decodedBytes = nibblePayload ? unpackNibbles(payload) : [];
  const looksCasio = manufacturer === 0x44;
  const patchName = fileName.replace(/\.(syx|mid|sysex)$/i, "");
  const confidence = looksCasio ? "medium" : "low";
  const draft = looksCasio ? buildDraftPreset(decodedBytes, patchName) : null;

  return {
    ok: looksCasio,
    tone: looksCasio ? "ok" : "warn",
    validation: looksCasio
      ? `Casio SysEx frame found (${frame.length} bytes, device id ${deviceId}).`
      : `SysEx frame found, but manufacturer byte was 0x${manufacturer.toString(16).padStart(2, "0")}.`,
    patchType: looksCasio
      ? "Casio CZ single-patch candidate"
      : "Unsupported or different synth family",
    summary: [
      `File: ${fileName}`,
      `Frame length: ${frame.length} bytes`,
      `Manufacturer: 0x${manufacturer.toString(16).padStart(2, "0")}`,
      `Device ID: ${deviceId}`,
      `Model bytes: ${modelBytes.map((b) => `0x${b.toString(16).padStart(2, "0")}`).join(" ")}`,
      `Payload bytes: ${payload.length}`,
      `Nibble-packed payload: ${nibblePayload ? "yes" : "no"}`,
      `Decoded bytes: ${decodedBytes.length}`,
      `Draft name: ${patchName}`,
      `Confidence: ${confidence}`
    ].join("\n"),
    decoded: nibblePayload
      ? summarizeDecodedBytes(decodedBytes)
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
  const result = decodePatch(buffer, file.name);
  currentDraft = result.draft;
  el.validationBox.textContent = result.validation;
  el.patchTypeBox.textContent = result.patchType;
  el.summaryBox.textContent = result.summary;
  el.decodedBox.textContent = result.decoded;
  renderDraft(currentDraft);
  setStatus(result.ok ? "Patch decoded. Draft preset created." : "Patch did not match a supported CZ import.", result.tone);
}

el.file.addEventListener("change", () => handleFile(el.file.files?.[0]));
el.dropzone.addEventListener("dragover", (event) => {
  event.preventDefault();
});
el.dropzone.addEventListener("drop", (event) => {
  event.preventDefault();
  const file = event.dataTransfer.files?.[0];
  if (file) handleFile(file);
});
el.createDraft.addEventListener("click", () => {
  if (currentDraft) {
    setStatus("Draft preset is already populated from the imported CZ file.", "ok");
  } else {
    setStatus("Import a file first so there is a draft to create.", "warn");
  }
});
el.openDraft.addEventListener("click", () => {
  if (handoffDraft(currentDraft)) {
    setStatus("Draft handed off to Envelope Lab.", "ok");
    openEditorTab();
  } else {
    setStatus("Import a file first so there is a draft to open.", "warn");
  }
});
el.saveDraft.addEventListener("click", () => {
  setStatus("Saving to card will come after the draft handoff flow is connected.", "warn");
});
