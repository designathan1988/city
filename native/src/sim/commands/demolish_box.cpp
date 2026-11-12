// Demolish by box (Roadcraft editor/bulldozer.ts, a drag): everything inside the rectangle between the two corners goes,
// in one undo step: the buildings whose centre is in it, the rubble of those a blow brought down, the road segments whose
// middle is in it with the nodes they leave alone (what comes back settled once, road_economy.h).
#include "sim/commands/commands.h"

#include "sim/commands/edit_journal.h"
#include "sim/commands/road_edit.h"
#include "sim/economy/road_economy.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
bool inside(float x, float z, float x0, float z0, float x1, float z1) { return x >= x0 && x <= x1 && z >= z0 && z <= z1; }
} // namespace

EditResult demolish_box(CityState &s, double ax, double az, double bx, double bz) {
	EditResult r;
	const float x0 = float(std::min(ax, bx)), x1 = float(std::max(ax, bx)), z0 = float(std::min(az, bz)), z1 = float(std::max(az, bz));
	std::vector<int> buildings, segments;
	s.buildings.query(x0, z0, x1, z1, [&](int id) {
		const Building &b = s.buildings.items[size_t(id)];
		if (inside(b.x, b.z, x0, z0, x1, z1)) buildings.push_back(id);
	});
	s.roads.for_segments_near(x0, z0, x1, z1, [&](int seg) {
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		if (inside((g.minx + g.maxx) * 0.5f, (g.minz + g.maxz) * 0.5f, x0, z0, x1, z1)) segments.push_back(seg);
	});
	int rubble = 0;
	for (int i = int(s.damage.ruins().size()) - 1; i >= 0; i--) {
		const Ruin &u = s.damage.ruins()[size_t(i)];
		if (!inside(u.x, u.z, x0, z0, x1, z1)) continue;
		j_remove_ruin(s, i);
		rubble++;
	}
	for (int id : buildings) j_remove_building(s, id);
	for (int seg : segments) demolish_segment(s, seg);
	if (buildings.empty() && segments.empty() && rubble == 0) {
		r.error = "help.tool.bulldoze";
		return r;
	}
	settle_road_edit(s, r); // what comes back is always affordable
	r.buildings_removed = int(buildings.size());
	r.segments = int(segments.size());
	finish_edit(s, x0, z0, x1, z1);
	r.ok = true;
	return r;
}

} // namespace city
