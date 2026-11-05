// What the feature uses of src/world/buildings/intern.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/intern.ts:117-233 class SharedMembers
/**
 * RECORDS' MEMBERS SHARED AMONG THEM, A BATCH AT A TIME, in place (hash-consing as `intern`, with tables of its own): what records written
 * to a table have alike written once (`recordTable.ts` TableWriter, a member reached twice is one node). DO NOT `intern` records a long
 * synchronous job writes and lets go (a city grown off the page, `editor/preparedCity.ts`): each canonical object gets a `WeakRef`, and a
 * `WeakRef`'s target is held until the job's task ends (ECMA-262 AddToKeptObjects) - every member of every building grown stayed alive
 * for the whole generation. Its objects are not frozen: the records are written once and let go.
 * After each batch only the members met more than once are kept
 * in that batch (a material, a storey, a facade), the next batch's equal members made those very objects - a table written a batch at a
 * time with its shared nodes kept (`TableWriter` keepShared) writes each of them once while it is met. DO NOT keep every member met: a
 * building's own volumes and elements are met once, and keeping them is keeping every record.
 */
export class SharedMembers { … }
