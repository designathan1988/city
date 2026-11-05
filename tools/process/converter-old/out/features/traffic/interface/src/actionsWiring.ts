// What the feature uses of src/actionsWiring.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/actionsWiring.ts:28-28 variable VEHICLE_HALF_SPAN
/** The farthest a vehicle's box reaches from its centre on the ground (a rider's shoulders over a two-wheeler's frame, `shootVehicle`). */
const VEHICLE_HALF_SPAN = Math.max(...ARCHETYPES.map((a) => Math.hypot(a.length / 2, Math.max(a.width / 2, m(0.35)))));
