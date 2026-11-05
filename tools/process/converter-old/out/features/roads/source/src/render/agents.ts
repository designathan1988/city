// The code of src/render/agents.ts that is the feature's (the rest of the file is shared).

// src/render/agents.ts:168-198 slice of src/render/agents.ts#AgentMeshes (shared): the feature's statements; "…" marks code left out
export interface AgentMeshes {
  // … 7 lines
  sync(world: SimClient, net: Network, alpha: number, detailed: boolean, zoom?: number, options?: AgentRenderOptions): void;
// … 21 lines
}

// src/render/agents.ts:2153-2573 slice of src/render/agents.ts#createAgentMeshes.sync (shared): the feature's statements; "…" marks code left out
    sync(world, net, alpha, detailed, zoom = Number.POSITIVE_INFINITY, options = {}) {
      // … 1 lines
      if (offRoadSweep?.journal !== net.doc.changes) offRoadSweep = new ChangeSweep(net.doc.changes, OFF_ROAD_GROUND);
      // … 322 lines
      if (band >= 1) {
        // … 3 lines
        for (const ped of world.pedViews) {
          // … 17 lines
          const rise = open ? m(0.04) : footwayRiseAt(net, pose.p.x, pose.p.y) * walkingRise(net, pose.p.x, pose.p.y, ped.ground !== 'crossing',
            segment === undefined ? 'ground' : net.doc.segment(segment)?.structure ?? 'ground');
          const deck = (open && roughGroundAt ? land(pose.p.x, pose.p.y) : below) + rise;
          // … 1 lines
          if (procedural && ped.id !== PLAYER_ID) {
            // … 4 lines
            if (ped.bleeding || ped.lost?.length) procBleed?.(ped.id, pose.p.x, pose.p.y, deck);
            // … 2 lines
            procDraw(ped.id, pose.p.x, pose.p.y, pose.angle, deck, gait, ped.walking || gait > m(0.15), gaitDt, shown,
              ped.lost ?? (ped.maimed ? [ped.maimed] : undefined), ped.gesture ? { t: ped.gesture.t, hold: ped.gesture.hold ?? 0 } : undefined, ped.wound, ped.ageClass);
          }
          else {
            // … 1 lines
            const ground = groundGradient(land,
              pose.p.x, pose.p.y, deck - rise);
            pedestrians.draw(ped, pose.p.x, pose.p.y, pose.angle, deck, alpha, ground);
          }
        // … 1 lines
        }
      }
    // … 49 lines
    },
