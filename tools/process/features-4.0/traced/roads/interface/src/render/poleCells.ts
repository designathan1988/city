// What the feature uses of src/render/poleCells.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/poleCells.ts:49-238 class PoleCells
/**
 * THE POLES HELD ROUND THE VIEW (`viewCells.ts`): the document's poles and wires block by block (`BLOCK`), each block made from the
 * document when the view reaches it (a pure function of the block and the document: its poles, the spans strung from them), its
 * matrices and wires let go on the CPU once on the GPU (`mesh/gpuOnly.ts`), dropped when an edit reaches it and made again whole.
 * A block's detail is chosen by its box nearest the eye (more detail than its nearest pole's, never less). DO NOT keep a pole's
 * state, slots and matrices for the whole map: 4,590 poles of the 8,100-building city held 21 MB (2026-10-21).
 */
export class PoleCells implements Utilities { … }

// src/render/poleCells.ts:56-56 field PoleCells.from
private from: { net: Network; groundAt: (x: number, y: number) => number } | null = null;

// src/render/poleCells.ts:85-88 method PoleCells.source
/** The network and the ground the poles are framed on (`poleGroundAt`): a new document drops every block. */
  source(net: Network, groundAt: (x: number, y: number) => number): void;

// src/render/poleCells.ts:176-213 method PoleCells.build
/** A block's stores filled from the document: the poles standing in `rect` (its own, filed as `blockKey` files them) and the spans strung from them. */
  private build(rect: ChangeRect): Block;
