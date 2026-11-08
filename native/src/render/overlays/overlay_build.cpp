// CPU meshes of the overlay layers, one per 256 m chunk: the lots coloured by their zone (the streets' zoned lots and the player's,
// each drawn whole by the chunk its middle stands in) with, while the Zones tool is in hand, their outlines, their fronts and the
// lots the streets propose; and the congestion ribbons along each carriageway (see overlay_renderer.h).
#include "render/overlays/overlay_renderer.h"

#include "render/mesh_builder.h"
#include "render/owned_segments.h"
#include "sim/buildings/growth.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr double MIN_DERIV = 1e-6; // a degenerate curve keeps a finite normal
constexpr float COLOUR_RANGE_MIN = 1e-3f; // free_share == jam_share still divides
constexpr float HALF_JAM = 0.5f;
constexpr double SHORTEST_EDGE_M = 1e-6; // an edge this short has no direction

Color zone_color(const CityState &s, int code, float alpha) {
	Color c = rgba8_to_color(s.catalog.zone(code).color);
	c.a = alpha;
	return c;
}

// Whether a building stands on a ring (its middle on it): the lot is built on.
bool built_on(const CityState &s, const std::vector<LotPoint> &ring) {
	float x0 = INFINITY, z0 = INFINITY, x1 = -INFINITY, z1 = -INFINITY;
	for (const LotPoint &p : ring) {
		x0 = std::min(x0, float(p.x));
		z0 = std::min(z0, float(p.z));
		x1 = std::max(x1, float(p.x));
		z1 = std::max(z1, float(p.z));
	}
	bool hit = false;
	s.buildings.query(x0, z0, x1, z1, [&](int id) {
		const Building &b = s.buildings.items[size_t(id)];
		hit = hit || inside_lot({ b.x, b.z }, ring);
	});
	return hit;
}

// Whether a point lies on a zoned lot of the streets' the player does not hold (a proposal there is no land to take).
bool on_street_lot(const CityState &s, const LotPoint &p) {
	return any_street_lot(s.lots, s.lot_book, p, [&](const Lot &l) { return on_lot(l, p); });
}
} // namespace

// A lot coloured on the ground: cut to the cells of the relief's own grid (lot_cover), each point laid on
// the relief: the terrain never rises through it.
void OverlayRenderer::lot_cover(Ribbon &out, const CityState &s, const std::vector<LotPoint> &ring, const Color &col) const {
	const double corner = s.terrain.x_of(0);
	for (const LotPoint &p : city::lot_cover(ring, s.terrain.spacing, { corner, corner })) {
		out.v.push_back(Vector3(float(p.x), s.terrain.height_at(float(p.x), float(p.z)) + lift_, float(p.z)));
		out.c.push_back(col);
	}
}

// A stroke `width` wide along a->b on the ground, a quad each step of the relief's grid (it follows the terrain's mesh).
void OverlayRenderer::edge(Ribbon &out, const CityState &s, const LotPoint &a, const LotPoint &b, float width, const Color &col) const {
	const double dx = b.x - a.x, dz = b.z - a.z, len = std::hypot(dx, dz);
	if (len < SHORTEST_EDGE_M) return;
	const double nx = -dz / len * width / 2, nz = dx / len * width / 2;
	const int n = std::max(1, int(std::ceil(len / s.terrain.spacing)));
	Vector3 prev0, prev1;
	for (int i = 0; i <= n; i++) {
		const double t = double(i) / n, x = a.x + dx * t, z = a.z + dz * t;
		const float y = s.terrain.height_at(float(x), float(z)) + lift_;
		const Vector3 p0(float(x + nx), y, float(z + nz)), p1(float(x - nx), y, float(z - nz));
		if (i > 0)
			for (const Vector3 &v : { prev0, prev1, p1, prev0, p1, p0 }) {
				out.v.push_back(v);
				out.c.push_back(col);
			}
		prev0 = p0;
		prev1 = p1;
	}
}

// One lot as the layer shows it: its zone's colour (fainter once built on, and with another tool in hand only
// the unbuilt zoned lots), and with the Zones tool its outline, and its front with an arrow from inside out through its middle.
void OverlayRenderer::lot_shape(Ribbon &out, const CityState &s, const std::vector<LotPoint> &ring, int zone, bool built, bool tool) const {
	if (ring.size() < 3) return;
	if (zone != 0 && (tool || !built)) lot_cover(out, s, ring, zone_color(s, zone, !tool ? zone_alpha_ : built ? look_.alpha_built : look_.alpha_tool));
	if (!tool) return;
	for (size_t k = 1; k < ring.size(); k++) edge(out, s, ring[k], ring[(k + 1) % ring.size()], look_.line_m, look_.line);
	const LotPoint &a = ring[0], &b = ring[1];
	edge(out, s, a, b, look_.front_m, look_.front);
	const double len = std::hypot(b.x - a.x, b.z - a.z);
	if (len < look_.arrow_least_m) return;
	LotPoint centre{ 0, 0 };
	for (const LotPoint &p : ring) {
		centre.x += p.x / double(ring.size());
		centre.z += p.z / double(ring.size());
	}
	const LotPoint mid{ (a.x + b.x) / 2, (a.z + b.z) / 2 };
	double nx = -(b.z - a.z) / len, nz = (b.x - a.x) / len; // the front's normal, out of the lot
	if ((mid.x - centre.x) * nx + (mid.z - centre.z) * nz < 0) {
		nx = -nx;
		nz = -nz;
	}
	const double size = std::min({ double(look_.arrow_most_m), len * look_.arrow_of_front,
			std::max(double(look_.arrow_least_m), std::hypot(mid.x - centre.x, mid.z - centre.z) * look_.arrow_of_depth) });
	const LotPoint tail{ mid.x - nx * size, mid.z - nz * size }, tip{ mid.x - nx * size * look_.arrow_tip, mid.z - nz * size * look_.arrow_tip };
	const double head = size * look_.arrow_head, spread = head * look_.arrow_spread;
	edge(out, s, tail, tip, look_.arrow_m, look_.front);
	edge(out, s, tip, { tip.x - nx * head - nz * spread, tip.z - nz * head + nx * spread }, look_.arrow_m, look_.front);
	edge(out, s, tip, { tip.x - nx * head + nz * spread, tip.z - nz * head - nx * spread }, look_.arrow_m, look_.front);
}

// A ribbon along segment `seg` between offsets u0 and u1 (left of a -> b positive), on the road or on the ground.
void OverlayRenderer::strip(Ribbon &out, const CityState &s, int seg, float u0, float u1, const Color &col, bool on_road) const {
	const RoadSegment &g = s.roads.segs[seg];
	int n = std::max(1, int(std::ceil(g.len / sample_)));
	Vector3 prev0, prev1;
	for (int i = 0; i <= n; i++) {
		double t = double(i) / n, x, z, dx, dz;
		g.q.at(t, x, z);
		g.q.deriv(t, dx, dz);
		double l = std::max(MIN_DERIV, std::hypot(dx, dz)), nx = dz / l, nz = -dx / l;
		auto at = [&](float u) {
			float px = float(x + nx * u), pz = float(z + nz * u);
			float y = on_road ? float(s.roads.seg_y(seg, g.len * t)) : s.terrain.height_at(px, pz);
			return Vector3(px, y + lift_, pz);
		};
		Vector3 p0 = at(u0), p1 = at(u1);
		if (i > 0) {
			for (const Vector3 &v : { prev0, prev1, p1, prev0, p1, p0 }) {
				out.v.push_back(v);
				out.c.push_back(col);
			}
		}
		prev0 = p0;
		prev1 = p1;
	}
}

// The chunk's lots: the streets' zoned lots whose middle is in it (the player's are drawn from their own ring), the player's lots
// whose stand is in it, and, with the Zones tool in hand, the lots the streets propose there on land nobody holds.
OverlayRenderer::Ribbon OverlayRenderer::build_zone_cpu(const CityState &s, int64_t key) const {
	Ribbon r;
	const bool tool = every_zone_.load();
	if (const std::vector<Lot> *lots = s.lots.lots_of(key))
		for (const Lot &l : *lots)
			if (l.zone != 0 && s.lot_book.at(l.x, l.z) == 0) lot_shape(r, s, lot_ring(l), l.zone, lot_occupied(s, l), tool);
	const double cx = double(key_x(key)) * CHUNK, cz = double(key_y(key)) * CHUNK;
	for (int i : s.lot_book.near(cx, cz, cx + CHUNK, cz + CHUNK)) {
		const LotStand &st = s.lot_book.stand(i);
		if (chunk_of(float(st.x), float(st.z)) != key) continue;
		const PlayerLot &lot = s.lot_book.all()[size_t(i)];
		if (lot.zone != 0 || tool) lot_shape(r, s, lot.corners, lot.zone, built_on(s, lot.corners), tool);
	}
	if (!tool) return r;
	for (const Lot &l : *LotCache::proposed(s, key, zone_works_.load())) {
		if (s.lot_book.at(l.x, l.z) != 0 || on_street_lot(s, { l.x, l.z })) continue;
		const std::vector<LotPoint> ring = lot_ring(l);
		for (size_t k = 0; k < ring.size(); k++) edge(r, s, ring[k], ring[(k + 1) % ring.size()], look_.proposal_m, look_.proposal);
	}
	return r;
}

Color OverlayRenderer::congestion_colour(const PoseFrame &f, int sg) const {
	float share = size_t(sg) < f.seg_speed_share.size() ? f.seg_speed_share[size_t(sg)] : 1.f;
	float k = std::clamp((share - jam_share_) / std::max(COLOUR_RANGE_MIN, free_share_ - jam_share_), 0.f, 1.f);
	Color col = k > HALF_JAM ? jam_colors_[1].lerp(jam_colors_[0], (k - HALF_JAM) * 2) : jam_colors_[2].lerp(jam_colors_[1], k * 2);
	col.a = congestion_alpha_;
	return col;
}

// The chunk's carriageways laid as ribbons (what update_congestion colours): each one's vertices and how many.
OverlayRenderer::Lanes OverlayRenderer::lay_congestion(const CityState &s, int64_t key) const {
	Lanes l;
	Ribbon r;
	each_owned_segment(s, key, [&](int sg) {
		const RoadSegment &g = s.roads.segs[size_t(sg)];
		if (!g.alive) return;
		const Profile &p = s.roads.profile_of(sg);
		if (p.rail) return;
		const int64_t before = r.v.size();
		strip(r, s, sg, -p.asph_right, p.asph_left, Color(), true);
		l.strips.push_back({ sg, int(r.v.size() - before) });
	});
	l.v = r.v;
	return l;
}

} // namespace city
