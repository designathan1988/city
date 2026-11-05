// What the feature uses of src/world/junction/bulb.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/junction/bulb.ts:24-24 interface Outline
/** A bulb's outline at some lateral reach, in its leg's frame at the node. */
interface Outline { readonly ring: Ring; readonly mouth: number }

// src/world/junction/bulb.ts:27-55 function outline
/** The outline `grow` outside the carriageway's */
function outline(c: Vec2, dir: Vec2, nrm: Vec2, hwL: number, hwR: number, grow: number): Outline | null;
