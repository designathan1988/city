// The code of src/render/roadSurfacePass.ts that is the feature's (the rest of the file is shared).

// src/render/roadSurfacePass.ts:23-23 slice of src/render/roadSurfacePass.ts#GROUND_MAX_EDGE (shared): the feature's statements; "…" marks code left out
export const GROUND_MAX_EDGE = TERRAIN_CELL / 2;

// src/render/roadSurfacePass.ts:25-25 slice of src/render/roadSurfacePass.ts#RAISED_MAX_EDGE (shared): the feature's statements; "…" marks code left out
export const RAISED_MAX_EDGE = TERRAIN_CELL;

// src/render/roadSurfacePass.ts:27-27 slice of src/render/roadSurfacePass.ts#followsGround (shared): the feature's statements; "…" marks code left out
const followsGround = (net: Network): boolean => net.doc.terrainRelief !== RELIEF_FLAT || net.doc.terrainStamps.length > 0;

// src/render/roadSurfacePass.ts:95-113 slice of src/render/roadSurfacePass.ts#passDesc (shared): the feature's statements; "…" marks code left out
export function passDesc(net: Network, pass: SurfacePass, raised: boolean, look: PassLook, paint: readonly string[]): PassDesc {
  // … 1 lines
  return {
    // … 6 lines
    maxEdge: raised || !followsGround(net) ? RAISED_MAX_EDGE : GROUND_MAX_EDGE,
  // … 7 lines
  };
}
