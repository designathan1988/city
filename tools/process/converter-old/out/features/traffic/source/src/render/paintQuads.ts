// The code of src/render/paintQuads.ts that is the feature's (the rest of the file is shared).

// src/render/paintQuads.ts:101-101 slice of src/render/paintQuads.ts#PAINT_COLOURS (shared): the feature's statements; "…" marks code left out
export const PAINT_COLOURS: readonly string[] = Object.freeze([...new Set([...ROAD_TYPES.map((rt) => rt.line), LANE_LINE, ...STYLE_PAINT, ...PARKING_PAINT, STOP_PAINT, ZEBRA_PAINT])]);
