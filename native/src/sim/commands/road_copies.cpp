// Edits of one road the inspector offers (Roadcraft main.ts onReverseDirection, editor/commit.ts duplicateSegment) and the map's
// paint standard (world/doc.ts setMarkingStyle): a one-way turned round, a copy alongside, every road painted again.
#include "sim/commands/commands.h"
#include "sim/commands/edit_journal.h"
#include "sim/commands/road_edit.h"

#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
bool alive_segment(const CityState &s, int seg) {
	return seg >= 0 && seg < int(s.roads.segs.size()) && s.roads.segs[size_t(seg)].alive;
}
} // namespace

EditResult reverse_road(CityState &s, int seg) {
	EditResult r;
	if (!alive_segment(s, seg) || !s.roads.profile_of(seg).one_way) {
		r.error = "tools.hint.upgradePick";
		return r;
	}
	const RoadSegment rec = s.roads.segs[size_t(seg)];
	j_remove_segment(s, seg);
	const Quad q{ rec.q.x1, rec.q.z1, rec.q.cx, rec.q.cz, rec.q.x0, rec.q.z0 };
	j_add_segment(s, rec.b, rec.a, q, rec.profile, rec.zr, rec.zl, rec.flags);
	finish_edit(s, rec.minx, rec.minz, rec.maxx, rec.maxz);
	r.ok = true;
	r.segments = 1;
	return r;
}

// The copy stands clear of the road by its width and a gap, on the left of a to b; it is built as the player would, so it is
// planned, charged and refused the same way.
EditResult duplicate_road(CityState &s, int seg) {
	EditResult r;
	if (!alive_segment(s, seg)) {
		r.error = "tools.hint.upgradePick";
		return r;
	}
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	const Profile &p = s.roads.profile_of(seg);
	const double dx = g.q.x1 - g.q.x0, dz = g.q.z1 - g.q.z0, len = std::hypot(dx, dz);
	if (len <= 0) {
		r.error = "tools.hint.upgradePick";
		return r;
	}
	const Tuning &t = s.catalog.tune;
	const double offset = std::max(double(t.edit_duplicate_min_m), double(p.right + p.left) + t.edit_duplicate_gap_m);
	const double nx = -dz / len * offset, nz = dx / len * offset;
	Anchor a, b;
	a.x = g.q.x0 + nx;
	a.z = g.q.z0 + nz;
	b.x = g.q.x1 + nx;
	b.z = g.q.z1 + nz;
	return build_road(s, a, true, g.q.cx + nx, g.q.cz + nz, b, g.profile);
}

EditResult set_marking_style(CityState &s, int style) {
	EditResult r;
	j_set_marking(s, style);
	double minx = 0, minz = 0, maxx = 0, maxz = 0; // the network's box: finish_edit walks the cells of what it is given
	bool any = false;
	for (int seg = 0; seg < int(s.roads.segs.size()); seg++) {
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		if (!g.alive) continue;
		s.roads.mark_segment(seg);
		minx = any ? std::min(minx, double(g.minx)) : g.minx;
		minz = any ? std::min(minz, double(g.minz)) : g.minz;
		maxx = any ? std::max(maxx, double(g.maxx)) : g.maxx;
		maxz = any ? std::max(maxz, double(g.maxz)) : g.maxz;
		any = true;
	}
	if (any) finish_edit(s, minx, minz, maxx, maxz);
	r.ok = true;
	return r;
}

} // namespace city
