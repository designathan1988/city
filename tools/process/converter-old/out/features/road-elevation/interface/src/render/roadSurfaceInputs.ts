// What the feature uses of src/render/roadSurfaceInputs.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadSurfaceInputs.ts:42-770 class SurfaceInputs
/** DO NOT gather a pass's inputs by walking the whole map and filtering by the pass */

/** The inputs of every pass, kept between builds and brought up to the network by its change (`refresh`). */
export class SurfaceInputs { … }

// src/render/roadSurfaceInputs.ts:451-531 method SurfaceInputs.placeNode
/** A node's inputs made again and filed: its plates, taper, island, dropped kerbs and junction paint. */
  private placeNode(net: Network, node: NodeId, silent = false): void;

// src/render/roadSurfaceInputs.ts:534-549 method SurfaceInputs.ownerOf
/** The pass a junction's plates are drawn in: that of its lowest road that is not a tunnel */
  private ownerOf(net: Network, node: NodeId): string | null;

// src/render/roadSurfaceInputs.ts:651-657 method SurfaceInputs.passList
/** The passes of the surfaces: every structural level, then each alignment raised by hand, by its lowest road. */
  passList(): SurfacePass[];
