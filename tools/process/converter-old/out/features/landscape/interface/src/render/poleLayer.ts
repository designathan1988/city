// What the feature uses of src/render/poleLayer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/poleLayer.ts:513-707 class PoleLayer
export class PoleLayer implements Utilities { … }

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
