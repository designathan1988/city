// The lamps of the floor a building is cut open at (interior_lamps.h).
#include "render/buildings/interior_lamps.h"

#include "render/buildings/building_bodies.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/interior/interior.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float LAMP_DROP_M = 0.6f; // under the ceiling: the lit shade of a hanging lamp
constexpr float LAMP_RISE = 0.9f; // of a lamp's height: the lit shade of a floor or table lamp
} // namespace

std::vector<Vector3> interior_lamps(const CityState &s, int id, int level) {
	std::vector<Vector3> out;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[size_t(id)].alive) return out;
	const Building &rec = s.buildings.items[size_t(id)];
	if (rec.floors == 0) return out;
	const std::shared_ptr<const b40::Body> body_held = b40::record_mass(s.buildings.designs, rec);
	const b40::Body &body = *body_held;
	level = std::clamp(level, 0, b40::top_level(body));
	const b40::LotFrame f = b40::lot_frame(rec);
	const float floor = b40::lot_floor(body, rec, f, [&](float x, float z) { return s.terrain.height_at(x, z); });
	b40::Interior inside(s.buildings.designs, body, b40::purpose_of(body, rec.fn, rec.seed));
	for (const b40::Furniture &p : inside.at(level).furniture) {
		if (p.kind != b40::FK_CEILING_LAMP && p.kind != b40::FK_FLOOR_LAMP && p.kind != b40::FK_TABLE_LAMP) continue;
		const V2 w = f.at(p.x, p.y);
		const float up = p.kind == b40::FK_CEILING_LAMP ? body.level_h(level) - LAMP_DROP_M : p.h * LAMP_RISE;
		out.push_back(Vector3(w.x, floor + b40::lift_at(body, level, p.x, p.y) + body.level_z(level) + up, w.y));
	}
	return out;
}

} // namespace city
