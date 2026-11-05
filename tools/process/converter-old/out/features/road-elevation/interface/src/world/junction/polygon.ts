// What the feature uses of src/world/junction/polygon.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/junction/polygon.ts:26-34 function nodeCentre
/** The node itself. `leg.origin` is back-projected so every leg agrees on it. */
export function nodeCentre(legs: readonly Leg[]): Vec2;

// src/world/junction/polygon.ts:63-76 function legTongue
/** Rectangle covering a leg's carriageway from the node out to its mouth. */
export function legTongue(leg: Leg, trim: number): Ring | null;

// src/world/junction/polygon.ts:82-106 function nodeCap
/** A rounded cap over the node, for junctions whose legs all leave on one side. */
export function nodeCap(legs: readonly Leg[], centre: Vec2): Ring | null;
