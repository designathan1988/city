// What the feature uses of src/jobs/jobScope.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/jobs/jobScope.ts:8-10 type JobService
/** The services the job workers run, each a module with `serve(scope)`. */
export type JobService = 'growth' | 'bays' | 'conflicts' | 'cells' | 'fracture' | 'citizenBake' | 'ecology' | 'flatGround' | 'groundShape'
  | 'garmentPadding' | 'peopleLod' | 'roadTiles' | 'roadCoarse' | 'terrainLight' | 'landStroke' | 'derivedWriter' | 'save' | 'roadStations'
  | 'cityGen' | 'roadFar' | 'networkBake';
