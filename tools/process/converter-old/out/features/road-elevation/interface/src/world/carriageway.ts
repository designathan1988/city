// What the feature uses of src/world/carriageway.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/carriageway.ts:9-20 function onCarriageway
/** DO NOT go back to a grid of every road's centre line made again at each network revision */
export function onCarriageway(net: Network, p: Vec2): boolean;

// src/world/carriageway.ts:23-23 variable ringPoints
/** A junction's footway ring as points, once per ring (a junction solved again is a new one). */
const ringPoints = identityMemo((ring: Ring): readonly Vec2[] => ring.flatten());

// src/world/carriageway.ts:27-43 function onRoadGround
/** Whether the point is on a street's ground: its carriageway, its footways, or a junction's plate (a tunnel's road is under the ground, not on it).
 * What the ecosystem's trees and woods stand off: DO NOT test them by `onCarriageway` alone, a road laid through a wood left the trees on its footways and its junctions (the player, 2026-10-23). */
export function onRoadGround(net: Network, p: Vec2): boolean;
