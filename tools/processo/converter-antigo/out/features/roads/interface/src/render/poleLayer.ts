// What the feature uses of src/render/poleLayer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/poleLayer.ts:66-66 function docPoles
/** DO NOT build the whole layer again for a change; DO NOT pace or hold it. */


/** The document's poles as a layer's source. */
export const docPoles = (doc: RoadDoc): PoleSource => …;

// src/render/poleLayer.ts:153-324 class PartStore
/** A part's instances over a block of the layer (`BLOCK`): a mesh with room for `capacity`, the first `count` drawn, and which pole holds each slot. */
export class PartStore { … }

// src/render/poleLayer.ts:327-489 class WireStore
/** One block's spans' wires: `SPAN_WIRES` instances a span in a block of their own, drawn with the template the view asks for. */
export class WireStore { … }

// src/render/poleLayer.ts:503-503 variable BLOCK
/** THE LAYER IN BLOCKS OF THE PLANE: a store a part and one for the wires in each, so a block out of the view is culled by three and each is drawn at the detail its own nearest pole asks. DO NOT keep one store a part for the whole layer: its bounds were the city's, never culled, and its levels chosen by the pole nearest the eye, every wire, insulator and lamp of a large city drawn at that detail at every frame (26,145 wires of 64 triangles, 3 M triangles at a close view, 2026-10-20). DO NOT make them a world cell (256 m) either: forty meshes a part over a city, drawn. */
export const BLOCK = m(768);

// src/render/poleLayer.ts:506-511 interface Block
export interface Block {
  readonly parts: PartStore[];
  readonly wires: WireStore;
  /** The poles placed in it here (none for a block of wires only): where it is nearest the eye. */
  readonly poles: Set<PoleId>;
}

// src/render/poleLayer.ts:513-707 class PoleLayer
export class PoleLayer implements Utilities { … }

// src/render/poleLayer.ts:561-565 method PoleLayer.fill
/** Every pole and span of the source, on an empty layer, in the source's order: the layer a full build makes */
  fill(net: Network, groundAt: (x: number, y: number) => number, source: PoleSource): void;

// src/render/poleLayer.ts:611-626 method PoleLayer.place
/** A pole framed and its parts placed; its way to the kerb kept from before, or undefined to read it (`kerbward`). */
  private place(net: Network, groundAt: (x: number, y: number) => number, source: PoleSource,
    pole: UtilityPole, kept: { x: number; y: number } | null | undefined): void;

// src/render/poleLayer.ts:711-721 function buildUtilities
/** The layer of every pole of a source, built whole (a load, a preview, the shaders' warm-up). */
export function buildUtilities(
  net: Network,
  groundAt: (x: number, y: number) => number,
  kit: SceneryKit,
  source: PoleSource = docPoles(net.doc),
  name = 'utilities',
): PoleLayer;

// src/render/poleLayer.ts:724-726 interface PolePreviewInput
/** A planned pole run, as the editor would build it: in order, each pole new or already standing. */
export interface PolePreviewInput {
  readonly poles: readonly { readonly x: number; readonly y: number; readonly lamp: boolean; readonly standing: boolean }[];
}

// src/render/poleLayer.ts:729-750 function buildPolePreview
/** The run under the pointer, drawn as it will stand */
export function buildPolePreview(
  net: Network,
  groundAt: (x: number, y: number) => number,
  kit: SceneryKit,
  input: PolePreviewInput,
): Utilities;
