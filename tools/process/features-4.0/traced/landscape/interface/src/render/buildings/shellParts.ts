// What the feature uses of src/render/buildings/shellParts.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/shellParts.ts:11-11 type ShellPartKind
/** THE SHELL'S PARTS: THE LOTS' AND ELEMENTS' BOXES, DRAWN INSTANCED WITH THE SHELL'S OWN LOOK */
// A gate's bars, a fence's posts and rails, a bin, a crate: a box each, once laid in the shell face by face (24 vertices,
// ~1 KB), now one instance of a unit model (88 bytes) drawn by the shell's material (`finishes.ts`), which lays on it what
// the shell laid on each vertex (`buildingMesh.ts` `Shell.face`): the planar uv of its finish, the tone and the grime.

/** The kinds drawn by the shell's material: a box (sides and top), a fence's rail (sides, top and bottom), a paving (a box whose sides stand in),
 *  a box whose top is covered (its four sides: a wall's body under its coping, a post under its head, a bar between a gate's rails). */
export type ShellPartKind = 'lotBox' | 'lotRail' | 'lotSlab' | 'lotSides';
