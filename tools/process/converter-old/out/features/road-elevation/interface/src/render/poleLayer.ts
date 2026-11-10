// What the feature uses of src/render/poleLayer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/poleLayer.ts:153-324 class PartStore
/** A part's instances over a block of the layer (`BLOCK`): a mesh with room for `capacity`, the first `count` drawn, and which pole holds each slot. */
export class PartStore { … }

// src/render/poleLayer.ts:156-156 field PartStore.count
count = 0;

// src/render/poleLayer.ts:169-169 field PartStore.perInstance
readonly perInstance: number;

// src/render/poleLayer.ts:327-489 class WireStore
/** One block's spans' wires: `SPAN_WIRES` instances a span in a block of their own, drawn with the template the view asks for. */
export class WireStore { … }

// src/render/poleLayer.ts:506-511 interface Block
export interface Block {
  readonly parts: PartStore[];
  readonly wires: WireStore;
  /** The poles placed in it here (none for a block of wires only): where it is nearest the eye. */
  readonly poles: Set<PoleId>;
}

// src/render/poleLayer.ts:513-707 class PoleLayer
export class PoleLayer implements Utilities { … }

// src/render/poleLayer.ts:516-516 field PoleLayer.blocks
private readonly blocks = new Map<string, Block>();

// src/render/poleLayer.ts:697-705 method PoleLayer.dispose
dispose(): void;

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
