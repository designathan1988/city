// A body's blocks resolved into solid ones (blocks.h).
#include "sim/buildings/b40/blocks.h"

#include "core/polygon_ops.h"
#include "sim/buildings/b40/plan_area.h"

#include <algorithm>
#include <set>
#include <vector>

namespace city::b40 {

namespace {
constexpr float SLIVER_M = 0.75f, TOUCH_AREA_M2 = 1e-3f;

PlanRing plan_of(const Volume &v) {
	PlanRing out;
	for (const Point2 &p : ring_of(v)) out.push_back(V2(p.x, p.y));
	return out;
}

bool levels_meet(const Volume &a, int l0, int l1) { return a.base < l1 && a.top() > l0; }

// The storeys [l0, l1) of a block, with what was set on them; the roof and what stands on it only on the top slice.
Volume slice(const Volume &s, int l0, int l1) {
	Volume v = s;
	v.mode = BLOCK_SOLID;
	v.base = l0;
	v.storeys = l1 - l0;
	v.fill.assign(s.fill.begin() + (l0 - s.base), s.fill.begin() + (l1 - s.base));
	move_storeys(v.bays, KEY_STOREY_SHIFT, s.base - l0, v.storeys);
	move_storeys(v.side_fill, KEY_SIDE_SHIFT, s.base - l0, v.storeys);
	move_storeys(v.storey_paint, KEY_SIDE_SHIFT, s.base - l0, v.storeys);
	v.reliefs.clear();
	for (Relief r : s.reliefs) {
		r.storey0 = std::max(r.storey0 + s.base - l0, 0);
		r.storey1 = std::min(r.storey1 + s.base - l0, v.storeys - 1);
		if (r.storey0 <= r.storey1) v.reliefs.push_back(r);
	}
	if (l1 != s.top()) {
		v.roof = ROOF_FLAT;
		v.crown = CROWN_NONE;
		v.gear.clear();
	}
	return v;
}

// A piece of a cut slice: the block's look on a plan of its own. What was set wall by wall belongs to the block's own walls.
bool cut_piece(const Volume &whole, const PlanRing &piece, Volume &v) {
	if (piece.size() < 3 || int(piece.size()) > MAX_FACES) return false;
	Ring ring;
	for (const V2 &p : piece) ring.push_back({ p.x, p.y });
	v = whole;
	set_block_ring(v, ring);
	if (v.w < SLIVER_M || v.d < SLIVER_M) return false;
	plain_walls(v);
	const Ring plan = ring_of(v);
	v.gear.erase(std::remove_if(v.gear.begin(), v.gear.end(), [&](const RoofPart &p) { return !contains_point(plan, { p.x, p.y }); }), v.gear.end());
	return true;
}

struct Resolver {
	std::vector<const Volume *> solids, cutters, clips;
	std::vector<Volume> out;

	static bool touch(const Volume &a, const Volume &c) { return levels_meet(c, a.base, a.top()) && overlap_area(ring_of(a), ring_of(c)) > TOUCH_AREA_M2; }

	std::vector<const Volume *> touching(const Volume &s, const std::vector<const Volume *> &list) const {
		std::vector<const Volume *> found;
		for (const Volume *o : list)
			if (o != &s && touch(s, *o)) found.push_back(o);
		return found;
	}

	// One range of levels, inside which the same voids and clips apply.
	void range(const Volume &s, int l0, int l1, const std::vector<const Volume *> &voids, const std::vector<const Volume *> &keeps) {
		std::vector<PlanRing> shape{ plan_of(s) }, cut, kept;
		for (const Volume *o : voids)
			if (levels_meet(*o, l0, l1)) cut.push_back(plan_of(*o));
		for (const Volume *o : keeps)
			if (levels_meet(*o, l0, l1)) kept.push_back(plan_of(*o));
		const Volume part = slice(s, l0, l1);
		if (cut.empty() && kept.empty()) { // the range nothing cuts keeps the block's own plan and facade
			out.push_back(part);
			return;
		}
		if (!kept.empty()) shape = plan_intersection(shape, kept);
		if (!cut.empty()) shape = plan_difference(shape, cut);
		for (const PlanRing &piece : shape) {
			Volume v;
			if (cut_piece(part, piece, v)) out.push_back(std::move(v));
		}
	}

	void block(const Volume &s, bool exclusive) {
		std::vector<const Volume *> voids = exclusive ? touching(s, solids) : touching(s, cutters);
		std::vector<const Volume *> keeps;
		if (!exclusive)
			for (const Volume *o : clips)
				if (levels_meet(*o, s.base, s.top())) keeps.push_back(o);
		std::set<int> levels{ s.base, s.top() };
		for (const auto *list : { &voids, &keeps })
			for (const Volume *o : *list) {
				if (o->base > s.base && o->base < s.top()) levels.insert(o->base);
				if (o->top() > s.base && o->top() < s.top()) levels.insert(o->top());
			}
		for (auto it = levels.begin(); std::next(it) != levels.end(); ++it) range(s, *it, *std::next(it), voids, keeps);
	}
};
} // namespace

void plain_walls(Volume &v) {
	v.bays.clear();
	v.side_fill.clear();
	v.storey_paint.clear();
	v.reliefs.clear();
	FaceGeometry face = v.faces[0];
	face.bays = -1;
	for (FaceGeometry &f : v.faces) f = face;
}

bool has_modes(const Body &b) {
	return std::any_of(b.volumes.begin(), b.volumes.end(), [](const Volume &v) { return v.mode != BLOCK_SOLID; });
}

void resolve_blocks(Body &b) {
	if (!has_modes(b)) return;
	Resolver r;
	for (const Volume &v : b.volumes) {
		if (v.mode == BLOCK_SOLID) r.solids.push_back(&v);
		else if (v.mode == BLOCK_INTERSECT) r.clips.push_back(&v);
		else r.cutters.push_back(&v); // an exclusive block cuts the solids where it meets them
	}
	int entrance = 0;
	for (size_t i = 0; i < b.volumes.size(); i++) {
		const Volume &v = b.volumes[i];
		if (int(i) == b.entrance_volume) entrance = int(r.out.size());
		if (v.mode == BLOCK_SOLID) r.block(v, false);
		else if (v.mode == BLOCK_XOR) r.block(v, true); // and stands as a solid where it meets none
	}
	b.entrance_volume = entrance < int(r.out.size()) ? entrance : 0;
	b.volumes = std::move(r.out);
}

} // namespace city::b40
