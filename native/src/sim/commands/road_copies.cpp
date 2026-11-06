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
	const std::vector<int> pieces = s.roads.pieces_of(seg); // the whole road turns: half of one running against the other would stop it
	RoadBox box;
	for (int piece : pieces) {
		const RoadSegment rec = s.roads.segs[size_t(piece)];
		box.add(rec);
		j_remove_segment(s, piece);
		const Quad q{ rec.q.x1, rec.q.z1, rec.q.cx, rec.q.cz, rec.q.x0, rec.q.z0 };
		j_add_segment(s, rec.b, rec.a, q, rec.profile, rec.zr, rec.zl, flags_reversed(rec.flags));
	}
	finish_edit(s, box.minx, box.minz, box.maxx, box.maxz);
	r.ok = true;
	r.segments = int(pieces.size());
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
	const Profile &p = s.roads.profile_of(seg);
	const int profile = s.roads.segs[size_t(seg)].profile;
	const Tuning &t = s.catalog.tune;
	const double offset = std::max(double(t.edit_duplicate_min_m), double(p.right + p.left) + t.edit_duplicate_gap_m);
	const std::vector<int> pieces = s.roads.pieces_of(seg);
	Quad line = s.roads.segs[size_t(pieces.front())].q; // the road as it was drawn: its pieces are cuts of one curve
	for (size_t k = 1; k < pieces.size(); k++) {
		const Quad &next = s.roads.segs[size_t(pieces[k])].q;
		Quad joined;
		line = joined_quad(line, next, joined) ? joined : Quad::straight(line.x0, line.z0, next.x1, next.z1);
	}
	const double dx = line.x1 - line.x0, dz = line.z1 - line.z0, len = std::hypot(dx, dz);
	if (len <= 0) {
		r.error = "tools.hint.upgradePick";
		return r;
	}
	const double nx = -dz / len * offset, nz = dx / len * offset;
	Anchor a, b;
	a.x = line.x0 + nx;
	a.z = line.z0 + nz;
	b.x = line.x1 + nx;
	b.z = line.z1 + nz;
	return build_road(s, a, true, line.cx + nx, line.cz + nz, b, profile);
}

EditResult set_marking_style(CityState &s, int style) {
	EditResult r;
	r.ok = true;
	if (style == s.marking_style.load()) return r; // nothing to paint again: the city is not changed (a session read on opening)
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
	return r;
}

} // namespace city
