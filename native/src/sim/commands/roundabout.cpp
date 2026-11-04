// Roundabouts (Roadcraft roads/construction/roundabout.ts): on vacant ground a ring with four approaches; over roads
// (Shift, or a click on a junction) the roads inside the circle are cut and their ends become the ring's entries.
// The ring is arcs of at most max_arc_deg between entry nodes, one-way anticlockwise with the ring profile; the ground
// under it is a flat disc at the centre's height.
#include "core/units.h"
#include "sim/commands/commands.h"
#include "sim/commands/road_edit.h"

#include "sim/terrain/terrain_gen.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr int REFUSE_SAMPLES = 16, WATER_SAMPLES = 48, CROSS_SAMPLES = 32, BISECT_STEPS = 24, EMPTY_RING_NODES = 6;
constexpr int VACANT_ARMS = 4;
constexpr double WATER_CLEAR_M = 0.3, HALF = 0.5;

struct Hit {
	int seg = -1;
	Quad q;
	std::vector<double> crossings;
};
struct Entry {
	double angle = 0;
	int node = -1;
};

int ring_profile(const Catalog &c) {
	for (size_t i = 0; i < c.profiles.size(); i++)
		if (c.profiles[i].ring) return int(i);
	return -1;
}

double ring_angle(double x, double z, double cx, double cz) { return std::atan2(-(z - cz), x - cx); }

struct Builder {
	CityState &s;
	double cx, cz, radius, y;
	int ring;
	std::vector<Entry> entries;
	std::vector<int> approaches;

	int entry_node(double x, double z) {
		double a = ring_angle(x, z, cx, cz);
		for (const Entry &e : entries) {
			double d = std::fabs(e.angle - a);
			if (d > PI_D) d = 2 * PI_D - d;
			if (d * radius < s.catalog.tune.roundabout_entry_merge_m) return e.node;
		}
		int n = j_add_node(s, x, z, y);
		entries.push_back({ a, n });
		return n;
	}
	bool inside(const Quad &q, double t) const {
		double x, z;
		q.at(t, x, z);
		return std::hypot(x - cx, z - cz) < radius;
	}
	void cut(const Hit &h);
	void vacant();
	std::vector<int> ring_segments();
};

// Keeps the parts of a crossed road outside the circle; their inner ends become entries.
void Builder::cut(const Hit &h) {
	RoadNetwork &net = s.roads;
	if (!net.segs[h.seg].alive) return;
	RoadSegment rec = net.segs[h.seg];
	j_remove_segment(s, h.seg);
	std::vector<double> pts{ 0 };
	pts.insert(pts.end(), h.crossings.begin(), h.crossings.end());
	pts.push_back(1);
	for (size_t k = 0; k + 1 < pts.size(); k++) {
		double t0 = pts[k], t1 = pts[k + 1];
		if (inside(h.q, (t0 + t1) * HALF)) continue;
		Quad sub = sub_quad(h.q, t0, t1);
		int na = t0 == 0 ? rec.a : entry_node(sub.x0, sub.z0);
		int nb = t1 == 1 ? rec.b : entry_node(sub.x1, sub.z1);
		sub.x0 = net.nodes[na].x; sub.z0 = net.nodes[na].z; sub.x1 = net.nodes[nb].x; sub.z1 = net.nodes[nb].z;
		if (na != nb) approaches.push_back(j_add_segment(s, na, nb, sub, rec.profile, rec.zl, rec.zr, rec.flags));
	}
	j_remove_node_if_orphan(s, rec.a);
	j_remove_node_if_orphan(s, rec.b);
}

// Vacant ground: four two-way local streets of approach_m out from the ring, their far ends plain dead ends.
void Builder::vacant() {
	double len = s.catalog.tune.roundabout_approach_m;
	for (int k = 0; k < VACANT_ARMS; k++) {
		double ang = k * PI_D / 2, ux = std::cos(ang), uz = -std::sin(ang);
		int e = j_add_node(s, cx + ux * radius, cz + uz * radius, y);
		entries.push_back({ ang, e });
		double fx = cx + ux * (radius + len), fz = cz + uz * (radius + len);
		int far = j_add_node(s, fx, fz, s.terrain.base_height_at(float(fx), float(fz)));
		approaches.push_back(j_add_segment(s, far, e, Quad::straight(fx, fz, cx + ux * radius, cz + uz * radius), 0, 0, 0, 0));
	}
}

// The ring: entry nodes plus extra nodes so no arc spans more than max_arc_deg; each arc a quadratic whose control
// point lies on the bisector at radius / cos(span / 2).
std::vector<int> Builder::ring_segments() {
	std::sort(entries.begin(), entries.end(), [](const Entry &a, const Entry &b) { return a.angle < b.angle; });
	double max_arc = s.catalog.tune.roundabout_max_arc_deg * DEG_TO_RAD;
	std::vector<Entry> filled;
	auto add = [&](double a) { filled.push_back({ a, j_add_node(s, cx + std::cos(a) * radius, cz - std::sin(a) * radius, y) }); };
	if (entries.empty())
		for (int i = 0; i < EMPTY_RING_NODES; i++) add(-PI_D + 2 * PI_D * i / EMPTY_RING_NODES);
	for (size_t i = 0; i < entries.size(); i++) {
		const Entry &a = entries[i], &b = entries[(i + 1) % entries.size()];
		filled.push_back(a);
		double span = b.angle - a.angle;
		if (span <= 0) span += 2 * PI_D;
		int extra = int(std::ceil(span / max_arc)) - 1;
		for (int k = 1; k <= extra; k++) add(a.angle + span * k / (extra + 1));
	}
	std::vector<int> out;
	for (size_t i = 0; i < filled.size(); i++) {
		const Entry &a = filled[i], &b = filled[(i + 1) % filled.size()];
		double span = b.angle - a.angle;
		if (span <= 0) span += 2 * PI_D;
		double mid = a.angle + span / 2, r = radius / std::cos(span / 2);
		const RoadNode &na = s.roads.nodes[a.node], &nb = s.roads.nodes[b.node];
		Quad q{ na.x, na.z, cx + std::cos(mid) * r, cz - std::sin(mid) * r, nb.x, nb.z };
		out.push_back(j_add_segment(s, a.node, b.node, q, ring, 0, 0, 0));
	}
	return out;
}
} // namespace

std::string roundabout_refusal(const CityState &s, double cx, double cz, double radius, bool cut) {
	int ring = ring_profile(s.catalog);
	const Tuning &tn = s.catalog.tune;
	if (ring < 0 || radius < tn.roundabout_radius_min_m || radius > tn.roundabout_radius_max_m) return "tool.failed";
	if (!s.terrain.in_bounds(float(cx), float(cz), float(radius + tn.roundabout_approach_m))) return "road.move.outside";
	double half = s.catalog.profile(ring).right, reach = radius + half;
	bool ring_there = false, road = false;
	s.roads.for_segments_near(cx - reach, cz - reach, cx + reach, cz + reach, [&](int sg) {
		bool is_ring = s.roads.profile_of(sg).ring;
		for (int k = 0; k <= REFUSE_SAMPLES; k++) {
			double x, z;
			s.roads.segs[sg].q.at(double(k) / REFUSE_SAMPLES, x, z);
			double d = std::hypot(x - cx, z - cz);
			if (is_ring && d < radius) ring_there = true;
			if (!is_ring && d < reach) road = true;
		}
	});
	if (ring_there) return "road.ring.exists";
	if (road && !cut) return "road.ring.cuts";
	for (double r : { radius - half, radius, radius + half })
		for (int k = 0; k < WATER_SAMPLES; k++) {
			double a = 2 * PI_D * k / WATER_SAMPLES;
			float x = float(cx + std::cos(a) * r), z = float(cz + std::sin(a) * r);
			float ground = s.terrain.base_height_at(x, z);
			if (ground < TerrainGen::OCEAN_Y + WATER_CLEAR_M || s.terrain.water_at(x, z) > ground) return "road.ring.water";
		}
	return "";
}

EditResult build_roundabout(CityState &s, double cx, double cz, double radius, bool cut) {
	EditResult r;
	r.error = roundabout_refusal(s, cx, cz, radius, cut);
	if (!r.error.empty()) return r;
	int ring = ring_profile(s.catalog);
	Builder b{ s, cx, cz, radius, double(s.terrain.base_height_at(float(cx), float(cz))), ring, {}, {} };
	std::vector<Hit> hits;
	s.roads.for_segments_near(cx - radius, cz - radius, cx + radius, cz + radius, [&](int sg) {
		Hit h{ sg, s.roads.segs[sg].q, {} };
		for (int k = 0; k < CROSS_SAMPLES; k++) {
			double a = double(k) / CROSS_SAMPLES, bb = double(k + 1) / CROSS_SAMPLES;
			if (b.inside(h.q, a) == b.inside(h.q, bb)) continue;
			double lo = a, hi = bb;
			for (int it = 0; it < BISECT_STEPS; it++) {
				double m = (lo + hi) / 2;
				(b.inside(h.q, m) == b.inside(h.q, a) ? lo : hi) = m;
			}
			h.crossings.push_back((lo + hi) / 2);
		}
		if (b.inside(h.q, 0) || b.inside(h.q, 1) || !h.crossings.empty()) hits.push_back(h);
	});
	for (const Hit &h : hits) b.cut(h);
	if (b.entries.empty()) b.vacant();
	std::vector<int> ring_segs = b.ring_segments();
	double price = 0;
	for (int sg : ring_segs) price += segment_value(s, sg);
	for (int sg : b.approaches)
		if (s.roads.segs[sg].alive && !(s.roads.segs[sg].flags & SEG_FREE)) price += segment_value(s, sg);
	if (!s.unlimited_money && price > s.money) {
		r.error = "ui.insp.noMoney";
		r.money = price;
		return r; // the caller rolls the journal back
	}
	const Tuning &tn = s.catalog.tune;
	float disc = float(radius + s.catalog.profile(ring).right);
	j_add_modifier(s, { float(cx), float(b.y), float(cz) }, disc, float(tn.roundabout_disc_pad_m));
	for (int sg : ring_segs) r.buildings_removed += clear_under(s, sg);
	for (int sg : b.approaches)
		if (s.roads.segs[sg].alive) {
			r.buildings_removed += clear_under(s, sg);
			conform_under(s, sg);
		}
	j_money(s, -std::round(price));
	double reach = disc + tn.roundabout_approach_m;
	finish_edit(s, cx - reach, cz - reach, cx + reach, cz + reach);
	r.ok = true;
	r.segments = int(ring_segs.size() + b.approaches.size());
	r.money = s.unlimited_money ? 0 : std::round(price);
	return r;
}

} // namespace city
