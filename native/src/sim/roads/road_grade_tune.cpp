// The figures a road grade solve is tuned by (Roadcraft 4.0 roads/tuning.ts ROAD_TUNING and world/structures.ts
// ROAD_STRUCTURES), read from the tuning (data/catalog/tuning.json, grade): the grades of a road at grade, a ramp, a deck
// and a tunnel, the ground clearance, and what each structure is.
#include "sim/roads/road_grade_internal.h"

#include "sim/catalog/catalog.h"

namespace city {

GradeTune grade_tune(const Tuning &tn) {
	GradeTune t;
	t.ground = tn.grade_ground;
	t.ramp = tn.grade_ramp;
	t.deck = tn.grade_deck;
	t.deck_tie = tn.grade_deck_tie;
	t.tunnel = tn.grade_tunnel;
	t.clearance = tn.grade_clearance_m;
	for (size_t s = 0; s < STRUCTURE_COUNT; s++) t.structure[s] = structure_spec(Structure(s), tn);
	return t;
}

StructureSpec structure_spec(Structure s, const Tuning &tn) {
	switch (s) {
		case Structure::ELEVATED: return { tn.grade_clear_elevated_m, tn.grade_deck_elevated_m, true };
		case Structure::BRIDGE: return { tn.grade_clear_bridge_m, tn.grade_deck_bridge_m, true };
		case Structure::TUNNEL:
			// TUNNEL_DEPTH: headroom, arch and the depth past them, under the lowest ground.
			return { -(tn.grade_tunnel_headroom_m + tn.grade_tunnel_arch_m + tn.grade_tunnel_floor_m), tn.grade_deck_tunnel_m, false };
		case Structure::GROUND: break;
	}
	return { 0.f, tn.grade_deck_ground_m, false };
}

} // namespace city
