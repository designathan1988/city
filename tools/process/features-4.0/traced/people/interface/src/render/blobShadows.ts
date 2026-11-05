// What the feature uses of src/render/blobShadows.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/blobShadows.ts:18-18 variable LIFT
/** Above the ground, in the matrix's units: clear of it without seeming to float. */
const LIFT = 0.03;

// src/render/blobShadows.ts:21-51 function blobMesh
/** The discs' instanced mesh, none drawn: `capacity` instances to start (the vehicles write theirs as their own part, `agents.ts`). */
export function blobMesh(name: string, capacity: number, opacity = 0.34): InstancedMesh;
