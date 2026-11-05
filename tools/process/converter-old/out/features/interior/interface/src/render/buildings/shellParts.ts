// What the feature uses of src/render/buildings/shellParts.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/shellParts.ts:11-11 type ShellPartKind
/** THE SHELL'S PARTS: THE LOTS' AND ELEMENTS' BOXES, DRAWN INSTANCED WITH THE SHELL'S OWN LOOK */
// A gate's bars, a fence's posts and rails, a bin, a crate: a box each, once laid in the shell face by face (24 vertices,
// ~1 KB), now one instance of a unit model (88 bytes) drawn by the shell's material (`finishes.ts`), which lays on it what
// the shell laid on each vertex (`buildingMesh.ts` `Shell.face`): the planar uv of its finish, the tone and the grime.

/** The kinds drawn by the shell's material: a box (sides and top), a fence's rail (sides, top and bottom), a paving (a box whose sides stand in),
 *  a box whose top is covered (its four sides: a wall's body under its coping, a post under its head, a bar between a gate's rails). */
export type ShellPartKind = 'lotBox' | 'lotRail' | 'lotSlab' | 'lotSides';

// src/render/buildings/shellParts.ts:12-12 variable SHELL_PART_KINDS
export const SHELL_PART_KINDS: readonly ShellPartKind[] = Object.freeze(['lotBox', 'lotRail', 'lotSlab', 'lotSides']);

// src/render/buildings/shellParts.ts:13-13 variable SHELL_PARTS
export const SHELL_PARTS: ReadonlySet<PartKind> = Object.freeze(new Set<PartKind>(SHELL_PART_KINDS));

// src/render/buildings/shellParts.ts:16-16 variable LOOK_FLOATS
/** Numbers an instance of a shell part carries besides its matrix and colour: its finish id, its wear and the ground its walls darken towards. */
export const LOOK_FLOATS = 3;

// src/render/buildings/shellParts.ts:19-22 function shellPartError
/** The screen error of a shell part's top-only level, model units, from an instance's scales [x, y, z] at `o`: the smaller side of the largest face its sides lose. A wall (long, tall, thin) leaves out its wall face, so its height; a post (thin across) leaves out a face no wider than the post, so its width. DO NOT use the height alone: a fence's posts are a metre tall and a tenth wide, and stayed at ten triangles each for a face a fraction of a pixel across. */
export function shellPartError(scale: ArrayLike<number>, o: number): number;

// src/render/buildings/shellParts.ts:24-24 variable GROUND_ATTRIBUTE
/** The attribute of the ground a shell part's walls darken towards (`GRIME_REACH`), per instance. */
export const GROUND_ATTRIBUTE = 'shellGround';
