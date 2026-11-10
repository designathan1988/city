// The Interior layer's command (interior_commands.h).
#include "sim/commands/interior_commands.h"

#include "sim/buildings/b40/record.h"

#include <algorithm>

namespace city {

namespace {
// Marks the chunk of the building a view opens, to be drawn again (the buildings are the same: only their look changes).
void mark_view(CityState &s, const InteriorView &v) {
	if (v.on && v.target < int(s.buildings.items.size()) && s.buildings.items[size_t(v.target)].alive) s.buildings.mark_look(v.target);
}
} // namespace

int interior_floors(const CityState &s, int id) {
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[size_t(id)].alive) return 0;
	const Building &rec = s.buildings.items[size_t(id)];
	if (rec.floors == 0) return 0;
	b40::Body body;
	b40::record_mass(s.buildings.designs, rec, body);
	return b40::top_level(body) + 1;
}

InteriorFocus interior_focus(const CityState &s, int id) {
	InteriorFocus f;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[size_t(id)].alive) return f;
	const Building &rec = s.buildings.items[size_t(id)];
	b40::Body body;
	b40::record_mass(s.buildings.designs, rec, body);
	f.floors = rec.floors == 0 ? 0 : b40::top_level(body) + 1;
	if (body.volumes.empty()) {
		f.at = V2(rec.x, rec.z);
		return f;
	}
	float x0 = body.volumes[0].x, x1 = x0, y0 = body.volumes[0].y, y1 = y0;
	for (const b40::Volume &v : body.volumes) {
		x0 = std::min(x0, v.x);
		x1 = std::max(x1, v.x + v.w);
		y0 = std::min(y0, v.y);
		y1 = std::max(y1, v.y + v.d);
	}
	f.at = b40::lot_frame(rec).at((x0 + x1) / 2, (y0 + y1) / 2);
	f.extent = std::max(x1 - x0, y1 - y0);
	return f;
}

const char *set_interior_view(CityState &s, const InteriorRequest &r) {
	InteriorView next;
	next.revision = s.interior.revision + 1;
	const char *problem = "";
	if (r.on) {
		next.on = true;
		next.cut.dir = r.dir & (b40::CUT_DIRECTIONS - 1);
		const int floors = r.target >= 0 ? interior_floors(s, r.target) : 0;
		if (floors == 0) {
			next.on = false;
			problem = "game.toast.insideNone";
		} else {
			next.target = r.target;
			next.cut.level = std::clamp(r.level, 0, floors - 1);
		}
	}
	mark_view(s, s.interior); // the buildings it showed whole before
	mark_view(s, next);
	s.interior = next;
	return problem;
}

} // namespace city
