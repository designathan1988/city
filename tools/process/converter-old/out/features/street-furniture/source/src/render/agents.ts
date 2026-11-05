// The code of src/render/agents.ts that is the feature's (the rest of the file is shared).

// src/render/agents.ts:261-267 slice of src/render/agents.ts#LAMP_GAIN (shared): the feature's statements; "…" marks code left out
const LAMP_GAIN: ReadonlyMap<number, number> = new Map([
  [HEADLAMP, nightGain(HEADLAMP, NIGHT_LUMINANCE.headlamp)],
  [TAILLAMP, nightGain(TAILLAMP, NIGHT_LUMINANCE.tailLamp)],
  [BRAKELAMP, nightGain(BRAKELAMP, NIGHT_LUMINANCE.brakeLamp)],
  [INDICATOR, nightGain(INDICATOR, NIGHT_LUMINANCE.indicator)],
  [DESTINATION, nightGain(DESTINATION, NIGHT_LUMINANCE.destination)],
]);
