// What the feature uses of src/render/roadsRead.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadsRead.ts:2-12 class RoadsRead
/** THE ROADS THE NEXT SOLVE READS THE GROUND UNDER, AS LAST WORKED OUT (`elevation.ts` roadsToRead), and what for (`key`: what they were worked out of). DO NOT work them out again every frame the solve waits on the stations' job (`renderer.ts` buildWorld returns and is asked again each frame): each time joined every change of the network since the last solve into new sets, 365 MB of a 95 s opening on the page (allocation profile ap1, 2026-10-17). No key (a moved ground's region, asked once): never kept. */
export class RoadsRead<T> { … }
