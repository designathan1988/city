// What the feature uses of src/render/instanceBounds.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/instanceBounds.ts:5-26 function fitInstances
/** `mesh`'s bounding sphere round its first `n` instances (InstancedMesh docs: "You may need to recompute the bounding sphere if an instance is transformed"): the box of their places, widened by the model's sphere at the largest scale any of them has. Conservative: no instance is ever left out. */
/** `grow`: the model's sphere widened by this much (a skinned body's limbs reach past its rest pose). */
export function fitInstances(mesh: InstancedMesh, n: number, grow = 1): void;
