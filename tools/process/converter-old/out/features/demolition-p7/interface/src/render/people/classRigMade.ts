// What the feature uses of src/render/people/classRigMade.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/classRigMade.ts:9-16 interface ClassRigMade
/** A PROCEDURAL CLASS'S RIG MADE ON THE PEOPLE'S WORKER AND PUT TOGETHER ON THE PAGE (`proceduralCrowd.ts` buildClass, `citizenBake.worker.ts` klass) */

/** A class's rig as the page puts it together: its body as a cooked person, what its shape and posture are, its height. */
export interface ClassRigMade {
  readonly person: DecodedPerson;
  readonly morphSource: { readonly kind: Int16Array; readonly index: Int32Array };
  readonly deltas: RigDeltas;
  readonly base: MacroParams;
  readonly shape: Float32Array;
  readonly height: number;
}
