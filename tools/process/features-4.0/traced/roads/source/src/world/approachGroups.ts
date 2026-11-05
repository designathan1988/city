// The code of src/world/approachGroups.ts that is the feature's (the rest of the file is shared).

// src/world/approachGroups.ts:17-69 function computeApproachGroups: the feature's own
export function computeApproachGroups(
  legs: readonly { readonly segment: SegmentId; readonly dir: Vec2 }[],
): ApproachGroup[] {
  if (legs.length === 0) return [];

  const tolerance = Math.cos((GROUP_MERGE_DEG * Math.PI) / 180);
  // Geometry, not allocation order, drives clustering.
  const sorted = legs
    .map((leg) => ({ leg, direction: normalize(leg.dir) }))
    .sort(
      (a, b) =>
        angleOf(a.direction) - angleOf(b.direction) ||
        angleOf(a.leg.dir) - angleOf(b.leg.dir) ||
        a.leg.segment - b.leg.segment,
    );

  interface Cluster {
    sum: Vec2;
    mean: Vec2;
    members: SegmentId[];
  }
  const clusters: Cluster[] = [];

  for (const item of sorted) {
    const { leg, direction } = item;
    let best = -1;
    let bestDot = tolerance;
    for (let i = 0; i < clusters.length; i++) {
      const d = dot(direction, (clusters[i] as Cluster).mean);
      if (d > bestDot) {
        bestDot = d;
        best = i;
      }
    }
    if (best < 0) {
      clusters.push({ sum: direction, mean: direction, members: [leg.segment] });
    } else {
      const c = clusters[best] as Cluster;
      c.members.push(leg.segment);
      c.sum = { x: c.sum.x + direction.x, y: c.sum.y + direction.y };
      c.mean = normalize(c.sum);
    }
  }

  return clusters
    .map((c) => ({ meanAngle: angleOf(c.mean), members: c.members }))
    .sort((a, b) => a.meanAngle - b.meanAngle)
    .map((c, id) => ({
      id,
      segments: c.members.slice().sort((x, y) => x - y),
      meanAngle: c.meanAngle,
    }));
}

// src/world/approachGroups.ts:33-37 interface Cluster: the feature's own
  interface Cluster {
    sum: Vec2;
    mean: Vec2;
    members: SegmentId[];
  }
