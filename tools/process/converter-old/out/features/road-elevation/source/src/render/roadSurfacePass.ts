// The code of src/render/roadSurfacePass.ts that is the feature's (the rest of the file is shared).

// src/render/roadSurfacePass.ts:56-67 slice of src/render/roadSurfacePass.ts#passRaised (shared): the feature's statements; "…" marks code left out
export function passRaised(net: Network, pass: SurfacePass, elevation: Pick<RoadReads, 'onSegment'>, terrainAt: (x: number, y: number) => number): boolean {
  if (pass.chain === undefined) return isRaised(roadStructure(pass.structure).id);
  return pass.chain.some((id) => {
    // … 2 lines
    for (let s = 0; s <= line.length; s += Math.max(4, line.length / 24)) {
      // … 1 lines
      if (elevation.onSegment(id, p.x, p.y) - terrainAt(p.x, p.y) > RAISED_LIFT) return true;
    }
  // … 1 lines
  });
}

// src/render/roadSurfacePass.ts:70-92 slice of src/render/roadSurfacePass.ts#passReads (shared): the feature's statements; "…" marks code left out
export function passReads(net: Network, elevation: RoadReads, terrainAt: (x: number, y: number) => number, inputs: SurfaceInputs, passId: string): SurfaceReads {
  // … 12 lines
  return {
    field: elevation,
  // … 6 lines
  };
}

// src/render/roadSurfacePass.ts:95-113 slice of src/render/roadSurfacePass.ts#passDesc (shared): the feature's statements; "…" marks code left out
export function passDesc(net: Network, pass: SurfacePass, raised: boolean, look: PassLook, paint: readonly string[]): PassDesc {
  const structure = roadStructure(pass.structure);
  return {
    // … 1 lines
    structure: structure.id,
  // … 12 lines
  };
}
