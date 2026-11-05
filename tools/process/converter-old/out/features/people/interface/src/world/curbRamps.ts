// What the feature uses of src/world/curbRamps.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/curbRamps.ts:43-47 function rampsOfNode
/** The dropped kerbs of one node, as it stands (`rampState` brought up to the network first) */
export function rampsOfNode(net: Network, node: NodeId): readonly CurbRamp[];

// src/world/curbRamps.ts:174-181 function walkingRise
/** How high the walking surface stands at (x, y), as a share of the footway's rise: a read of the ramps as kept (asked by every walker drawn every frame), brought up to the network by its own rebuild (`followRamps`); made here only the first time a network's are asked */
export function walkingRise(net: Network, x: number, y: number, onFootway: boolean, structure = 'ground'): number;
