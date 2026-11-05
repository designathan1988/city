// What the feature uses of src/render/buildings/buildingMesh.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/buildingMesh.ts:3716-3729 class ShellWriter
/** DO NOT grow a new shell's arrays for each building of a job from nothing (every one doubling its way up, the half let go, its chunk holding views of arrays twice its size): one shell written again for each, the arrays grown once to the largest, each chunk an exact copy. */
export class ShellWriter { … }
