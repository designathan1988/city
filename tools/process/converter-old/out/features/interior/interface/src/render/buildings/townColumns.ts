// What the feature uses of src/render/buildings/townColumns.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/townColumns.ts:12-12 variable KNOWN
/** WHAT THE BUILDINGS LAYER KEEPS OF EACH BUILDING, AS NUMBERS IN COLUMNS BY ITS ID (`layer.ts`): its digests, its place and box, its cell and what it flies; never its record (report "Arquitetura para cidades grandes": the table is the source of truth, a record is decoded while it is read). A record is read once when it comes or changes (`file`), and again only where its cell is made near the camera. */

/** Set once the record's numbers are in its columns (`file`); cleared when the diary names it (`stale`). */
export const KNOWN = 1;

// src/render/buildings/townColumns.ts:14-14 variable FLIES
/** It flies a flag (`buildingMesh.ts` `fliesFlag`): its summary is asked even far (the flags). */
export const FLIES = 2;

// src/render/buildings/townColumns.ts:16-16 variable SIGNATURE
/** A signature building (`signature.ts` `drawsSignature`): its body drawn with parts of its own. */
export const SIGNATURE = 4;

// src/render/buildings/townColumns.ts:18-18 variable SHOWN
/** Shown by the layer (not hidden by the preview nor a ruin): its flags read from what is kept of it. */
export const SHOWN = 8;

// src/render/buildings/townColumns.ts:20-20 variable KEPT
/** What the workers emitted of it is kept (`keptSig`, `keptDigest`). */
export const KEPT = 16;

// src/render/buildings/townColumns.ts:22-22 variable FILED
/** Filed in the town (`TownIndex`), in the cell `filedI`, `filedJ`. */
export const FILED = 32;

// src/render/buildings/townColumns.ts:55-55 field TownColumns.slots
private readonly slots = new IdSlots(1024);

// src/render/buildings/townColumns.ts:58-58 field TownColumns.flags
flags = new Uint8Array(16);

// src/render/buildings/townColumns.ts:60-60 field TownColumns.ids
/** The id held at each slot. */
  ids = new Float64Array(16);

// src/render/buildings/townColumns.ts:62-62 field TownColumns.x
/** The anchor (the cell is worked out of it). */
  x = new Float64Array(16);

// src/render/buildings/townColumns.ts:63-63 field TownColumns.y
y = new Float64Array(16);

// src/render/buildings/townColumns.ts:65-65 field TownColumns.minX
/** Its stored box (`storedBounds`), and its height over its foot. */
  minX = new Float32Array(16);

// src/render/buildings/townColumns.ts:66-66 field TownColumns.minY
minY = new Float32Array(16);

// src/render/buildings/townColumns.ts:67-67 field TownColumns.maxX
maxX = new Float32Array(16);

// src/render/buildings/townColumns.ts:68-68 field TownColumns.maxY
maxY = new Float32Array(16);

// src/render/buildings/townColumns.ts:69-69 field TownColumns.height
height = new Float32Array(16);

// src/render/buildings/townColumns.ts:71-71 field TownColumns.groundMinX
/** The box of the ground it reads (`storedBounds` grown by its module twice: `layerCells.ts` `groundDigest`). */
  groundMinX = new Float64Array(16);

// src/render/buildings/townColumns.ts:72-72 field TownColumns.groundMinY
groundMinY = new Float64Array(16);

// src/render/buildings/townColumns.ts:73-73 field TownColumns.groundMaxX
groundMaxX = new Float64Array(16);

// src/render/buildings/townColumns.ts:74-74 field TownColumns.groundMaxY
groundMaxY = new Float64Array(16);

// src/render/buildings/townColumns.ts:76-76 field TownColumns.sig
/** Its record's digest, with and without its wear. */
  sig = new Float64Array(16);

// src/render/buildings/townColumns.ts:77-77 field TownColumns.massSig
massSig = new Float64Array(16);

// src/render/buildings/townColumns.ts:79-79 field TownColumns.keptSig
/** What the kept summary was made of (`KEPT`): its record's and its ground's digests. */
  keptSig = new Float64Array(16);

// src/render/buildings/townColumns.ts:80-80 field TownColumns.keptDigest
keptDigest = new Float64Array(16);

// src/render/buildings/townColumns.ts:82-82 field TownColumns.filedI
/** The cell it is filed in (`FILED`): where it stood when last filed, whatever its record says now. */
  filedI = new Int32Array(16);

// src/render/buildings/townColumns.ts:83-83 field TownColumns.filedJ
filedJ = new Int32Array(16);

// src/render/buildings/townColumns.ts:161-163 method TownColumns.forEach
/** Every id held, with its slot. */
  forEach(visit: (id: number, slot: number) => void): void;
