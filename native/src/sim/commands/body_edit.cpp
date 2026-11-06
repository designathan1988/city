// The one way an edit reaches a building's body, and what a body must be to stand (body_ops.h).
#include "sim/buildings/b40/blocks.h"
#include "sim/buildings/b40/body_table.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/buildings/b40/record.h"
#include "sim/commands/body_ops.h"
#include "sim/commands/edit_journal.h"
#include "sim/commands/road_edit.h"
#include "sim/persistence/body_codec.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

using namespace b40;

namespace {
constexpr float SIZE_EPS = 1e-3f;

// A block above the ground bears on the blocks that reach its floor; it may overhang them, up to the share in the tuning.
bool supported(const Tuning &t, const Body &b, const Volume &v) {
	if (v.base == 0 || v.mode == BLOCK_VOID || v.mode == BLOCK_INTERSECT) return true;
	std::vector<Ring> under;
	for (const Volume &o : b.volumes)
		if (&o != &v && o.base < v.base && o.top() >= v.base && o.mode != BLOCK_VOID && o.mode != BLOCK_INTERSECT) under.push_back(ring_of(o));
	return support_share(ring_of(v), under) >= float(t.build_min_bearing);
}

bool within(float x, double lo, double hi) { return x >= float(lo) - SIZE_EPS && x <= float(hi) + SIZE_EPS; }
} // namespace

std::string body_problem(const Tuning &t, const Body &b) {
	if (b.volumes.empty()) return ""; // an open lot, a square or a park: it has no mass to hold up
	if (int(b.volumes.size()) > int(t.build_max_volumes)) return "building.problem.size";
	if (!within(b.ground_h, t.build_min_storey_m, t.build_max_ground_m)) return "building.problem.size";
	if (!within(b.storey_h, t.build_min_storey_m, t.build_max_storey_m)) return "building.problem.size";
	bool on_ground = false;
	for (const Volume &v : b.volumes) {
		on_ground = on_ground || v.base == 0;
		if (!within(v.w, t.build_min_side_m, t.build_max_side_m) || !within(v.d, t.build_min_side_m, t.build_max_side_m))
			return "building.problem.size";
		if (v.base < 0 || v.storeys < 1 || v.top() > int(t.edit_max_floors)) return "building.problem.size";
	}
	if (!on_ground) return "building.problem.footprint";
	for (const Volume &v : b.volumes)
		if (!supported(t, b, v)) return "building.problem.support";
	if (has_modes(b)) { // what the blocks leave must stand
		Body solid = b;
		resolve_blocks(solid);
		if (solid.volumes.empty()) return "building.problem.noMass";
	}
	return "";
}

EditResult edit_body(CityState &s, int id, const BodyOp &op) {
	EditResult r;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) {
		r.error = "tool.failed";
		return r;
	}
	Building rec = s.buildings.items[id];
	Body body;
	record_body(s.buildings.designs, rec, body);
	const uint32_t before = body_digest(body);
	r.error = op(body);
	if (r.error.empty()) r.error = body_problem(s.catalog.tune, body);
	if (!r.error.empty()) return r;
	r.ok = true;
	if (body_digest(body) == before) return r;
	int top = 0;
	for (const Volume &v : body.volumes) top = std::max(top, v.top());
	rec.floors = uint16_t(top);
	body.signature = 0; // an edit is the kit's to draw: a design drawn whole knows nothing of a moved wall
	rec.body = s.buildings.designs.edited->add(std::move(body));
	j_edit_building(s, id, rec);
	float reach = std::max(rec.w, rec.d);
	finish_edit(s, rec.x - reach, rec.z - reach, rec.x + reach, rec.z + reach);
	return r;
}

} // namespace city
