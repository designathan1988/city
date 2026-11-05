// What the feature uses of src/world/rebuildWork.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/rebuildWork.ts:3-3 variable networkWork
/** THE WORK OF A REBUILD AFTER A ROAD EDIT, counted per derived product: how many things each one looked at */
/** The network's rebuild (`networkBuild.ts`): records compared for changes (`sources`), junction solves looked up or made */
export const networkWork = { sources: 0, junctions: 0, segments: 0, marks: 0, built: 0, taken: 0 };

// src/world/rebuildWork.ts:5-5 variable rebuildWork
export const rebuildWork = { conflicts: 0, walkways: 0, signals: 0, sidewalks: 0, spans: 0, lanes: 0, routing: 0 };
