// What the feature uses of src/render/densityCars.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/densityCars.ts:37-37 variable LINK_POINTS
/** THE FAR TRAFFIC: the meso model's cars on every road outside the micro window, as Cidade's density cars, a draw a tile */
/** DO NOT write every far car again at each meso step nor draw the tiles off the view: each link's cars belong to the tile of its middle, a mesh a tile culled by its sphere, and only the tiles where a link's numbers changed are written and sent.
 *  DO NOT walk every link the worker holds each step: it sends only the links that changed (`MesoDelta`), and only those are looked at here. */
// Each car an instance (its link's slot, place, lane, speed); the vertex shader places it on the link's road in a float texture (`texelFetch`):
// the page's roads by their centreline, the far tiles' straight through the start, middle and end their link tables give (`meso/tileLinks.ts`).

/** Points of a road's centreline in the texture, evenly spaced along it (a bend between two of them is a straight chord). */
const LINK_POINTS = 8;

// src/render/densityCars.ts:112-117 interface DensityCars
export interface DensityCars {
  readonly object: Group;
  /** Each frame: the window told the worker, the roads' lines the network or the meso changed written, the cars of a new meso step written. */
  update(net: Network, sim: SimClient, focusX: number, focusY: number): void;
  dispose(): void;
}

// src/render/densityCars.ts:120-121 function farLateral
/** A far road's car across it, to the right of its travel (`laneOffset` without the road's profile: a two-way road's lanes right of its centre, a one-way's about it). */
const farLateral = (lane: number, lanes: number, oneWay: boolean): number => …;
