// What the feature uses of src/render/poleLayer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/poleLayer.ts:724-726 interface PolePreviewInput
/** A planned pole run, as the editor would build it: in order, each pole new or already standing. */
export interface PolePreviewInput {
  readonly poles: readonly { readonly x: number; readonly y: number; readonly lamp: boolean; readonly standing: boolean }[];
}
