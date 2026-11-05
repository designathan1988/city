// What the feature uses of src/render/ragdollJolt.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/ragdollJolt.ts:7-7 type JoltApi
/** The bodies on the ground simulated by Jolt Physics */

type JoltApi = typeof JoltModule;

// src/render/ragdollJolt.ts:10-10 type JBodyID
type JBodyID = InstanceType<JoltApi['BodyID']>;

// src/render/ragdollJolt.ts:58-58 interface JoltStatic
/** A static collider made for a body (its ground, the walls round it), released with it. */
export interface JoltStatic { readonly id: JBodyID }

// src/render/ragdollJolt.ts:293-398 class JoltRagdoll
/** One body's parts in Jolt, and which part carries each of its particles. */
export class JoltRagdoll { … }
