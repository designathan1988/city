// The code of src/render/agents.ts that is the feature's (the rest of the file is shared).

// src/render/agents.ts:261-267 slice of src/render/agents.ts#LAMP_GAIN (shared): the feature's statements; "…" marks code left out
const LAMP_GAIN: ReadonlyMap<number, number> = new Map([
  [HEADLAMP, nightGain(HEADLAMP, NIGHT_LUMINANCE.headlamp)],
  [TAILLAMP, nightGain(TAILLAMP, NIGHT_LUMINANCE.tailLamp)],
  [BRAKELAMP, nightGain(BRAKELAMP, NIGHT_LUMINANCE.brakeLamp)],
  [INDICATOR, nightGain(INDICATOR, NIGHT_LUMINANCE.indicator)],
  [DESTINATION, nightGain(DESTINATION, NIGHT_LUMINANCE.destination)],
]);

// src/render/agents.ts:2153-2573 slice of src/render/agents.ts#createAgentMeshes.sync (shared): the feature's statements; "…" marks code left out
    sync(world, net, alpha, detailed, zoom = Number.POSITIVE_INFINITY, options = {}) {
      // … 324 lines
      if (band >= 1) {
        // … 3 lines
        for (const ped of world.pedViews) {
          // … 21 lines
          if (procedural && ped.id !== PLAYER_ID) {
          // … 10 lines
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
