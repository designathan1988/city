// What the probes read of the agents drawn on the last frame (city_core_probe agent_cost): per vehicle model and level its
// triangles and instances, the draws and triangles of the frame, the cabins, and how the people stand in their slots.
#include "render/agents/agent_renderer.h"

#include <godot_cpp/variant/array.hpp>

using namespace godot;

namespace city {

Dictionary AgentRenderer::cost() const {
	static const char *const NAMES[AGENT_MODELS] = { "sedan", "hatch", "suv", "van", "pickup", "moto", "bus", "truck", "bicycle", "wagon" };
	static const char *const LEVEL_NAMES[DRAWS] = { "cabin", "body", "far", "mid" };
	Dictionary out;
	out["cabins"] = cabins_drawn();
	out["people_near"] = people_.shown(0);
	// people at each slot on the last frame (the mesh levels, the mesh casting nothing, the card), and the two edges in metres
	Array at_slot;
	for (int l = 0; l < people_.levels(); l++) at_slot.push_back(people_.shown(l));
	out["people_slots"] = at_slot;
	out["people_shadow_to_m"] = people_.blob_from_m();
	out["people_card_from_m"] = people_.card_from_m(people_.tallest_m());
	// the walkers of the last frame in pixels: the tallest card and the smallest mesh, the card's pixels and the hold of the edge
	out["people_card_px_most"] = people_.card_px_most();
	out["people_mesh_px_least"] = people_.mesh_px_least();
	out["people_card_px"] = people_.card_below_px();
	out["people_level_hold"] = people_.level_hold();
	out["rows_lost"] = rows_lost_;
	int draws = 0, tris = 0;
	for (int i = 0; i < AGENT_MODELS; i++)
		for (int level = 0; level < DRAWS; level++) {
			Dictionary d;
			d["body_tris"] = body_tris_[i][level];
			d["glass_tris"] = glass_tris_[i][level];
			d["instances"] = shown_[i][level];
			out[String(NAMES[i]) + "_" + LEVEL_NAMES[level]] = d;
			if (shown_[i][level] > 0) draws += glass_tris_[i][level] > 0 ? 2 : 1;
			tris += shown_[i][level] * (body_tris_[i][level] + glass_tris_[i][level]);
		}
	out["draws"] = draws;
	out["triangles"] = tris; // drawn last frame, in the main pass (shadow passes again for the casters)
	return out;
}

int AgentRenderer::cabins_drawn() const {
	int n = 0;
	for (int i = 0; i < VEHICLE_TYPES; i++) n += shown_[i][CABIN_LEVEL] + shown_[i][MID_DRAW];
	return n;
}

} // namespace city
