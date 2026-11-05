// What the feature uses of src/sim/ambient/ambientIndex.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/ambient/ambientIndex.ts:350-350 field StreetLength.whole
/** The street length of the whole map (`world/networkLength.ts`). */
  private readonly whole = new NetworkLength();

// src/sim/ambient/ambientIndex.ts:352-352 field StreetLength.pieces
/** Each ribbon's straight pieces as the walk reads them: middle x, middle y, length. Ribbons are never changed in place. */
  private readonly pieces = new WeakMap<SegmentRibbon, Float64Array>();

// src/sim/ambient/ambientIndex.ts:354-365 method StreetLength.around
around(net: Network, x: number, y: number, r: number): number;

// src/sim/ambient/ambientIndex.ts:368-370 method StreetLength.wholeMap
/** DO NOT sum every ribbon of the map when the network moves: every road drawn walked the town's roads. */
  private wholeMap(net: Network): number;

// src/sim/ambient/ambientIndex.ts:373-388 method StreetLength.add
/** The pieces of `ribbon` whose middle is within `r` of (x, y), added to `total` in order. */
  private add(total: number, ribbon: SegmentRibbon, x: number, y: number, r: number): number;
