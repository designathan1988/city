// The code of src/render/streetFurnitureLayer.ts that is the feature's (the rest of the file is shared).

// src/render/streetFurnitureLayer.ts:167-231 slice of src/render/streetFurnitureLayer.ts#furniturePlace (shared): the feature's statements; "…" marks code left out
function furniturePlace(net: Network, elevation: RoadElevation, placed: LandscapeItem, lens: LampLens): FurniturePlace | undefined {
  // … 7 lines
  const base = elevation.onSegment(item.segment, item.x, item.y) + footwayRiseAt(net, item.x, item.y);
  // … 3 lines
  const at = { x: item.x, y: item.y, z: base, sx: 1, sy: 1, sz: 1 };
  switch (item.kind) {
    case 'lamp': {
      // … 6 lines
      put(MODEL.pool, { x: item.x - item.outward.x * centre, y: item.y - item.outward.y * centre, z: base + m(0.012),
        yaw: inward, sx: across, sy: 1, sz: LAMP_HEIGHT * POOL_ALONG });
      // … 1 lines
      put(MODEL.beam, { x: item.x - item.outward.x * lens.reach, y: item.y - item.outward.y * lens.reach, z: base + lens.height,
        yaw: 0, sx: LAMP_BEAM_RADIUS, sy: lens.height + FOOTWAY_RISE, sz: LAMP_BEAM_RADIUS });
    // … 1 lines
    }
    // … 16 lines
    case 'streetTree': {
      // … 4 lines
      const tree = { x: item.x, y: item.y, z: base + m(0.03), yaw: rng.float() * Math.PI * 2,
        sx: 0.85 + rng.float() * 0.2, sy: 1, sz: 0.85 + rng.float() * 0.2, tint: foliageTint(rng), h: STREET_TREE_MIN + item.seed * STREET_TREE_RANGE };
    // … 2 lines
    }
    case 'shrub': {
      // … 3 lines
      const shrub = { x: item.x, y: item.y, z: base - m(0.05), yaw: rng.float() * Math.PI * 2, sx: 1.05, sy: 1, sz: 1.05, tint: foliageTint(rng), h };
    // … 2 lines
    }
  }
// … 1 lines
}
