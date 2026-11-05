// What the feature uses of src/editor/chunkedSave.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/chunkedSave.ts:28-28 variable SAVE_TILE
/** THE MAP SAVED BY TILES: each record in the square of the map it stands in, and a save writes only the squares the diary says changed */

/** THE SQUARES AS BINARY REGIONS (manifest 6, 2026-10-21): each square one frame (`saveBinary.ts`) - the buildings their record table (`encodeRecords`), the other layers their records' JSON, the order as doubles - compressed by the save worker; the saves by tiles in JSON (manifest 5) and before them (manifest 3) still open, and are written as regions by the next save */

/** DO NOT walk the map to find what changed, nor write what did not: an edit in one tile writes that tile, whatever the map's size */

/** The side of the squares the map is saved by: the world's tile (CLAUDE.md, "One grid"). */
export const SAVE_TILE = m(1_024);

// src/editor/chunkedSave.ts:30-30 variable DENSE_SQUARE
/** Buildings and lots by the world's cell: a dense tile holds thousands of buildings of ~7 KB, one edit would write megabytes. */
const DENSE_SQUARE = CELL;

// src/editor/chunkedSave.ts:33-33 type KeyedName
/** The keyed layers (records with ids, any order) and the ordered lists (the order is meaning: stamps and dabs are laid in turn). */
type KeyedName = 'nodes' | 'segments' | 'poles' | 'spans' | 'landscape' | 'barriers' | 'buildings' | 'lots';

// src/editor/chunkedSave.ts:36-36 variable KEYED_NAMES
const KEYED_NAMES: readonly KeyedName[] = ['nodes', 'segments', 'poles', 'spans', 'landscape', 'barriers', 'buildings', 'lots'];

// src/editor/chunkedSave.ts:40-40 interface Place
interface Place { readonly x: number; readonly y: number }

// src/editor/chunkedSave.ts:41-41 interface Keyed
interface Keyed { readonly id: number }

// src/editor/chunkedSave.ts:44-55 interface KeyedSpec
/** A keyed layer: its records by id, the point that files each one (one square per record, so it is stored once) and its form in the file. */
interface KeyedSpec<T extends Keyed = Keyed> {
  readonly side: number;
  readonly kinds: readonly ChangeKind[];
  get(doc: RoadDoc, id: number): T | undefined;
  all(doc: RoadDoc): Iterable<T>;
  at(doc: RoadDoc, record: T): Place | undefined;
  saved(record: T): unknown;
  /** The record's place in its store's order, where the order means something (the lots: the first one holds a point). */
  order?(doc: RoadDoc, id: number): number;
  /** Records of other layers whose square or file form follows this one's (a node's roads: their curves are fitted to it). */
  reaches?(doc: RoadDoc, record: T): Iterable<readonly [KeyedName, number]>;
}

// src/editor/chunkedSave.ts:56-56 function keyed
const keyed = <T extends Keyed>(spec: KeyedSpec<T>): KeyedSpec => …;

// src/editor/chunkedSave.ts:57-57 function nodeOf
const nodeOf = (doc: RoadDoc, id: number): RoadNode | undefined => …;

// src/editor/chunkedSave.ts:60-94 variable KEYED
/** DO NOT file a road in every tile it crosses: a road is filed by its end `a`, a span by its pole `a`, a barrier by its first point. */
const KEYED: Readonly<Record<KeyedName, KeyedSpec>> = {
  nodes: keyed<RoadNode>({
    side: SAVE_TILE, kinds: ['roads', 'traffic'], get: nodeOf, all: (doc) => doc.nodes.values(), at: (_doc, n) => n, saved: savedNode,
    reaches: (_doc, n) => n.incident.map((s) => ['segments', s] as const),
  }),
  segments: keyed<RoadSegment>({
    side: SAVE_TILE, kinds: ['roads', 'traffic'], get: (doc, id) => (doc.segments as ReadonlyMap<number, RoadSegment>).get(id),
    all: (doc) => doc.segments.values(), at: (doc, s) => nodeOf(doc, s.a), saved: savedSegment,
  }),
  poles: keyed<UtilityPole>({
    side: SAVE_TILE, kinds: ['utilities'], get: (doc, id) => doc.poles.get(id as PoleId), all: (doc) => doc.poles.values(), at: (_doc, p) => p,
    saved: savedPole, reaches: (doc, p) => doc.spansAt(p.id).map((s) => ['spans', s.id] as const),
  }),
  spans: keyed<UtilitySpan>({
    side: SAVE_TILE, kinds: ['utilities'], get: (doc, id) => doc.poleSpans.get(id as SpanId), all: (doc) => doc.poleSpans.values(),
    at: (doc, s) => doc.poles.get(s.a), saved: savedSpan,
  }),
  landscape: keyed<LandscapeItem>({
    side: SAVE_TILE, kinds: ['landscape'], get: (doc, id) => doc.landscape.get(id), all: (doc) => doc.landscape.values(), at: (_doc, item) => item,
    saved: savedLandscape,
  }),
  barriers: keyed<Barrier>({
    side: SAVE_TILE, kinds: ['barriers'], get: (doc, id) => doc.barriers.get(id), all: (doc) => doc.barriers.values(), at: (_doc, b) => b.points[0],
    saved: savedBarrier,
  }),
  // `JSON.stringify` of a record is the text of its copy (`cloneBuilding` is a JSON round trip): the record itself is handed on.
  buildings: keyed<Building>({
    side: DENSE_SQUARE, kinds: ['buildings'], get: (doc, id) => doc.buildings.get(id as BuildingId), all: (doc) => doc.buildings.all(),
    at: (_doc, b) => b, saved: (b) => b,
  }),
  lots: keyed<Lot>({
    side: DENSE_SQUARE, kinds: ['lots'], get: (doc, id) => doc.lotStore.get(id), all: (doc) => doc.lotStore.all(), at: (_doc, lot) => lot.corners[0],
    saved: (lot) => lot, order: (doc, id) => doc.lotStore.orderOf(id) ?? 0,
  }),
};

// src/editor/chunkedSave.ts:476-564 method TiledSaves.write
private async write(doc: RoadDoc, settings: unknown, savedAt: number): Promise<boolean>;
