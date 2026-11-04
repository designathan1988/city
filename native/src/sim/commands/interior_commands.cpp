// The Interior layer's command (interior_commands.h).
#include "sim/commands/interior_commands.h"

#include "sim/buildings/b40/record.h"

#include <algorithm>

namespace city {

namespace {
// Marks the chunks of every building a view opens, to be drawn again (the buildings are the same: only their look changes).
void mark_view(CityState &s, const InteriorView &v) {
	if (!v.on) return;
	if (v.target >= 0) {
		if (v.target < int(s.buildings.items.size()) && s.buildings.items[size_t(v.target)].alive) s.buildings.mark_look(v.target);
		return;
	}
	if (v.radius <= 0) return;
	s.buildings.query(v.cx - v.radius, v.cz - v.radius, v.cx + v.radius, v.cz + v.radius, [&](int id) {
		if (v.cuts(id, s.buildings.items[size_t(id)])) s.buildings.mark_look(id);
	});
}
} // namespace

int interior_floors(const CityState &s, int id) {
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[size_t(id)].alive) return 0;
	const Building &rec = s.buildings.items[size_t(id)];
	if (rec.floors == 0) return 0;
	b40::Body body;
	b40::record_body(s.buildings.designs, rec, body);
	return b40::top_level(body) + 1;
}

const char *set_interior_view(CityState &s, const InteriorRequest &r) {
	InteriorView next;
	next.revision = s.interior.revision + 1;
	const char *problem = "";
	if (r.on) {
		next.on = true;
		next.cut.dir = r.dir & (b40::CUT_DIRECTIONS - 1);
		if (r.target >= 0) {
			const int floors = interior_floors(s, r.target);
			if (floors == 0) {
				next.on = false;
				problem = "game.toast.insideNone";
			} else {
				next.target = r.target;
				next.cut.level = std::clamp(r.level, 0, floors - 1);
			}
		} else {
			next.cx = r.cx;
			next.cz = r.cz;
			next.radius = std::clamp(r.radius, 0.f, s.catalog.tune.interior_max_radius_m);
			next.cut.level = std::max(r.level, 0);
			if (next.radius <= 0) next.on = false;
		}
	}
	mark_view(s, s.interior); // the buildings it showed whole before
	mark_view(s, next);
	s.interior = next;
	return problem;
}

} // namespace city
