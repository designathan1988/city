// Every lot's covers against one another (CityProbes::lot_covers): the covers render/buildings/lot_covers.h lays for the render, each
// placed as the building chunks place it (its floor on the ground under its blocks), and wherever a cover lies over one before it in
// their order, it stands over it by at least half the cover step: two covers never cross, never share one depth. And each lot's ground
// read in its own frame along scan lines (GroundCensus): what of it lies under no cover, and the planted or bare bands too thin to
// plant that stand beside a hard cover.
#include "api/city_probes.h"

#include "render/buildings/lot_census.h"
#include "render/buildings/lot_covers.h"
#include "sim/buildings/b40/element_shape.h"
#include "sim/buildings/b40/record.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <algorithm>
#include <cmath>
#include <shared_mutex>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr float INSET = 0.05f; // of the way from a corner to the middle: a sample stays off the edges two pieces share
constexpr float HALF = 0.5f;
constexpr float LEAST_UP = 1e-9f; // a piece whose normal has no more of the vertical than this stands on edge: no height over a point

// A piece and its box on the ground (x, z).
struct Laid {
	b40::CoverPiece c;
	float x0, z0, x1, z1;
};

Laid laid(const b40::CoverPiece &c) {
	Laid l{ c, c.p[0].x, c.p[0].z, c.p[0].x, c.p[0].z };
	for (int i = 1; i < c.count; i++) {
		l.x0 = std::min(l.x0, c.p[i].x);
		l.x1 = std::max(l.x1, c.p[i].x);
		l.z0 = std::min(l.z0, c.p[i].z);
		l.z1 = std::max(l.z1, c.p[i].z);
	}
	return l;
}

// The piece's height over (x, z), on the plane of its first three corners: false where (x, z) is not inside it.
bool height_on(const b40::CoverPiece &c, float x, float z, float &y) {
	float sign = 0;
	for (int i = 0; i < c.count; i++) {
		const Vector3 &a = c.p[i], &b = c.p[(i + 1) % c.count];
		const float cross = (b.x - a.x) * (z - a.z) - (b.z - a.z) * (x - a.x);
		if (cross == 0) continue;
		if (sign == 0) sign = cross;
		else if ((cross > 0) != (sign > 0)) return false;
	}
	const Vector3 n = (c.p[1] - c.p[0]).cross(c.p[2] - c.p[0]);
	if (std::fabs(n.y) < LEAST_UP) return false;
	y = c.p[0].y - (n.x * (x - c.p[0].x) + n.z * (z - c.p[0].z)) / n.y;
	return true;
}

constexpr float SCAN_STEP_M = 0.25f; // the lot's ground is read along lines this far apart, across it and along it
constexpr float THIN_M = 0.6f; // a planted or bare band narrower than this beside a hard cover is a sliver (lot_space.cpp LAWN_LEAST)
constexpr float JOINT_M = 0.01f; // a band thinner than this is two edges meeting
enum Ground : uint8_t { BARE, SOFT, HARD, WALL };

// A lot's ground as its body lays it, in the lot's frame: every cover's outline (sim/buildings/b40/element_shape.h: its rectangle,
// turned with it, or a round cover's ellipse; the later over the earlier) and every ground volume's rectangle.
struct GroundBoxes {
	struct Box {
		b40::Outline shape;
		float x0, y0, x1, y1; // the box that holds it
		Ground is;
	};
	std::vector<Box> boxes; // covers in their order, then the ground volumes
	float x0 = 0, x1 = 0, depth = 0;

	void add(const b40::Outline &o, Ground is) {
		const b40::Footprint f = o.bounds();
		boxes.push_back({ o, f.x0, f.y0, f.x1, f.y1, is });
	}
	GroundBoxes(const b40::Body &b, const b40::Paint &lawn) : x0(-b.plan.W * HALF), x1(b.plan.W * HALF), depth(b.plan.D) {
		std::vector<const b40::LotElement *> covers;
		for (const b40::LotElement &e : b.elements)
			if (e.kind == b40::EL_SURFACE) covers.push_back(&e);
		std::stable_sort(covers.begin(), covers.end(), [](const b40::LotElement *p, const b40::LotElement *q) { return p->h < q->h; });
		for (const b40::LotElement *e : covers) {
			const b40::Paint &p = b.paints[size_t(e->paint)];
			add(b40::outline_of(*e), p.finish == lawn.finish && p.rgb == lawn.rgb ? SOFT : HARD);
		}
		for (const b40::Volume &v : b.volumes)
			if (v.base == 0) add(b40::outline_of(b40::Footprint{ v.x, v.y, v.x + v.w, v.y + v.d }), WALL);
	}
	Ground at(float x, float y) const {
		Ground g = BARE;
		for (const Box &c : boxes)
			if (x > c.x0 && x < c.x1 && y > c.y0 && y < c.y1 && c.shape.holds(x, y)) g = c.is;
		return g;
	}
	// Along one line (across the lot at depth `line`, or along it at `line` across): the bare length, and the thin planted or bare bands
	// that stand between a hard cover and whatever ends them (a hard cover, a wall, the lot's edge); the first such band's middle.
	// `first`: the first band's middle, its width, what stands before it and after it (Ground).
	void scan(bool across, float line, float &bare, int &thin, float (&first)[4]) const {
		std::vector<float> cuts{ across ? x0 : 0.f, across ? x1 : depth };
		const float lo = cuts[0], hi = cuts[1];
		for (const Box &c : boxes) {
			float from, to; // the stretch of the line this one lies over
			if (!c.shape.span(across, line, from, to)) continue;
			cuts.push_back(std::clamp(from, lo, hi));
			cuts.push_back(std::clamp(to, lo, hi));
		}
		std::sort(cuts.begin(), cuts.end());
		float run = 0, from = lo;
		Ground before = WALL; // the lot's edge ends a band as a wall does
		const auto close = [&](Ground after, float end) {
			if (run > JOINT_M && run < THIN_M && (before == HARD || after == HARD) && thin++ == 0) {
				first[0] = (from + end) * HALF;
				first[1] = run;
				first[2] = float(before);
				first[3] = float(after);
			}
			run = 0;
		};
		for (size_t k = 1; k < cuts.size(); k++) {
			const float piece = cuts[k] - cuts[k - 1];
			if (piece <= 0) continue;
			const float mid = (cuts[k] + cuts[k - 1]) * HALF;
			const Ground g = across ? at(mid, line) : at(line, mid);
			if (g == BARE) bare += piece;
			if (g == BARE || g == SOFT) {
				if (run == 0) from = cuts[k - 1];
				run += piece;
				continue;
			}
			close(g, cuts[k - 1]);
			before = g;
		}
		close(WALL, hi);
	}
};

// The census of the lots' ground: lots read, the bare ground in them, the lots with a sliver and the first.
struct GroundCensus {
	int lots = 0, sliver_lots = 0, slivers = 0;
	double bare_m2 = 0;
	Array first, first_boxes; // the first sliver, and its lot's boxes (x0, y0, x1, y1, what it is) in their order
	void count(int id, const b40::Body &b, const b40::Paint &lawn) {
		if (b.plan.W <= 0 || b.plan.D <= 0) return;
		const GroundBoxes g(b, lawn);
		if (g.boxes.empty()) return;
		lots++;
		int thin = 0;
		float bare = 0, band[4] = { 0, 0, 0, 0 };
		const auto tell = [&](float x, float y, const char *way) {
			Array uses = Array::make(int(b.plan.kind), int(b.plan.front), int(b.plan.back), int(b.plan.left.use), int(b.plan.right.use));
			first = Array::make(id, x, y, way, band[1], int(band[2]), int(band[3]), uses);
			for (const GroundBoxes::Box &c : g.boxes) first_boxes.push_back(Array::make(c.x0, c.y0, c.x1, c.y1, int(c.is)));
		};
		for (float y = SCAN_STEP_M * HALF; y < g.depth; y += SCAN_STEP_M) {
			const int had = thin;
			g.scan(true, y, bare, thin, band);
			if (had == 0 && thin > 0 && first.is_empty()) tell(band[0], y, "across");
		}
		bare_m2 += double(bare) * SCAN_STEP_M;
		float unused = 0;
		for (float x = g.x0 + SCAN_STEP_M * HALF; x < g.x1; x += SCAN_STEP_M) {
			const int had = thin;
			g.scan(false, x, unused, thin, band);
			if (had == 0 && thin > 0 && first.is_empty()) tell(x, band[0], "along");
		}
		slivers += thin;
		sliver_lots += thin > 0;
	}
};
} // namespace

Dictionary CityProbes::lot_covers() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const Tuning &tn = s.catalog.tune;
	b40::BuildingLook look;
	look.lot_plate = tn.lot_plate_m;
	look.cover_step = tn.lot_cover_step_m;
	const float least = tn.lot_cover_step_m * HALF;
	int lots = 0, samples = 0, under = 0;
	float worst = 0;
	Array first, short_first;
	int64_t short_covers = 0;
	std::vector<Laid> pieces;
	GroundCensus ground;
	const b40::Paint lawn = s.buildings.designs.color("lawn");
	for (int id = 0; id < int(s.buildings.items.size()); id++) {
		const Building &rec = s.buildings.items[size_t(id)];
		if (!rec.alive) continue;
		const std::shared_ptr<const b40::Body> body = b40::record_mass(s.buildings.designs, rec);
		const b40::LotFrame f = b40::lot_frame(rec);
		const float floor = b40::lot_floor(*body, rec, f, [&s](float x, float z) { return s.terrain.height_at(x, z); });
		b40::Placed pl;
		pl.data = &s.buildings.designs;
		pl.look = &look;
		pl.b = pl.whole = body.get();
		pl.lot = b40::Basis{ Vector3(f.o.x, floor, f.o.y), Vector3(f.e0.x, 0, f.e0.y), Vector3(f.e1.x, 0, f.e1.y), Vector3(0, 1, 0) };
		pl.ground = [&s](const Vector3 &p) { return s.terrain.height_at(p.x, p.z); };
		ground.count(id, *body, lawn);
		Array short_one;
		const int64_t short_here = covers_short(pl, short_one);
		if (short_here > 0 && short_first.is_empty()) short_first = Array::make(id, short_one);
		short_covers += short_here;
		pieces.clear();
		b40::lot_covers(pl, [&pieces](const b40::CoverPiece &c) { pieces.push_back(laid(c)); });
		if (pieces.empty()) continue;
		lots++;
		for (const Laid &later : pieces) {
			Vector3 mid;
			for (int i = 0; i < later.c.count; i++) mid += later.c.p[i];
			mid /= float(later.c.count);
			for (int i = 0; i <= later.c.count; i++) {
				const Vector3 q = i < later.c.count ? later.c.p[i] + (mid - later.c.p[i]) * INSET : mid;
				for (const Laid &earlier : pieces) {
					if (earlier.c.rank >= later.c.rank || q.x < earlier.x0 || q.x > earlier.x1 || q.z < earlier.z0 || q.z > earlier.z1) continue;
					float y;
					if (!height_on(earlier.c, q.x, q.z, y)) continue;
					samples++;
					const float gap = q.y - y;
					if (gap >= least) continue;
					under++;
					if (first.is_empty() || gap < worst) {
						worst = gap;
						first = Array::make(id, q.x, q.z, gap, later.c.element, earlier.c.element);
					}
				}
			}
		}
	}
	Dictionary d;
	d["lots"] = lots; // lots with covers
	d["samples"] = samples; // points of a cover over an earlier cover of its lot
	d["under"] = under; // of them, where the later does not stand over the earlier by half a step (crossed, or at one depth)
	d["worst"] = first; // building, x, z, the later less the earlier (m), the two elements
	d["covers_short"] = short_covers; // covers whose mesh, drawn alone, holds less ground than their outline (render/buildings/lot_census.h)
	d["short_first"] = short_first; // the first: building, then its element, middle, size, the m2 drawn and the outline's
	d["ground_lots"] = ground.lots; // lots whose ground was read, in their own frame
	d["bare_m2"] = ground.bare_m2; // their ground under no cover and no building
	d["sliver_lots"] = ground.sliver_lots; // lots with a planted or bare band under a lawn's least width beside a hard cover
	d["slivers"] = ground.slivers; // such bands, a scan line each
	d["sliver_first"] = ground.first; // the first: building, x and y in the lot's frame, the line's way, the band's width, what stands before and after it (0 bare, 1 planted, 2 hard, 3 a wall or the lot's edge), the lot's kind and its front, back, left and right uses
	d["sliver_boxes"] = ground.first_boxes; // that lot's covers in their order, then its ground volumes: x0, y0, x1, y1, what it is
	return d;
}

} // namespace city
