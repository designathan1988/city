// The parts of a road that are not its surface, chunk by chunk (Roadcraft 4.0 render/structures.ts, its behaviour): the deck under a
// raised road, its abutments, its bents (a crossbeam on a column at each edge, sized by the height they stand) and its parapets, and
// a tunnel's portals with their wing walls stepping down the hillside. A road is raised by its structure or by how far it stands over
// the ground, so a ramp finds its own abutment where it leaves the solid approach.
#include "render/roads/road_mesh.h"

#include "sim/roads/road_grade.h"

#include <algorithm>
#include <cmath>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr float BOX_ROUGHNESS = 1.f, METAL = 0.02f; // the material's UV: a kind of box's roughness and metal
constexpr uint32_t CONCRETE_RGB = 0x9fa4a2U; // the 4.0's piers and mouths
constexpr uint32_t DECK_RGB = 0xa7a6a1U; // the 4.0's deck's tone
constexpr int SCAN_MOST = 32; // a long road is read in at most this many steps
constexpr int PIER_SIDES = 16; // a column's facets round
constexpr float GROWN = 0.001f; // a parapet of less than this share of its height is none
constexpr float HALF = 0.5f;
constexpr int BOX_FACES = 6; // a box's

const Vector3 UP(0, 1, 0);

constexpr float LEAST_SPAN = 1e-6f; // a step's span and a pier's rise never quite nothing

float smooth(float a, float b, float v) {
	const float u = std::min(1.f, std::max(0.f, (v - a) / std::max(LEAST_SPAN, b - a)));
	return u * u * (3.f - 2.f * u);
}

// A box of length `along` on the road's way, width `across` and height `tall`, with `o` the middle of its foot.
void slab(MeshBuilder &mb, const Vector3 &o, const Vector3 &fwd, const Vector3 &left, float along, float across, float tall, const Color &col) {
	const Vector3 a = fwd * (along * HALF), b = left * (across * HALF), h = UP * tall;
	const Vector3 p[4] = { o - a - b, o + a - b, o + a + b, o - a + b };
	const Vector3 out[4] = { -left, fwd, left, -fwd };
	for (int i = 0; i < BOX_FACES - 2; i++) {
		const int j = (i + 1) % 4;
		structure_face(mb, p[i], p[j], p[j] + h, p[i] + h, col, out[i]);
	}
	structure_face(mb, p[0] + h, p[1] + h, p[2] + h, p[3] + h, col, UP);
	structure_face(mb, p[0], p[1], p[2], p[3], col, -UP);
}

// A column: `width` the radius of its head, its foot flared, open under the ground.
void column(const StructureShape &K, MeshBuilder &mb, const Vector3 &foot, float top, float width, const Color &col) {
	const Vector2 t(BOX_ROUGHNESS, METAL);
	std::vector<int> rim;
	for (int i = 0; i <= PIER_SIDES; i++) {
		const float a = TAU_F * float(i) / float(PIER_SIDES);
		const Vector3 n(std::cos(a), 0, std::sin(a));
		rim.push_back(mb.add(foot + n * (width * K.pier_flare), n, col, t));
		rim.push_back(mb.add(Vector3(foot.x, top, foot.z) + n * width, n, col, t));
	}
	for (int i = 0; i < PIER_SIDES; i++) {
		const Vector3 n(std::cos(TAU_F * (float(i) + HALF) / float(PIER_SIDES)), 0, std::sin(TAU_F * (float(i) + HALF) / float(PIER_SIDES)));
		const int a = rim[size_t(i) * 2], b = rim[size_t(i) * 2 + 1], c = rim[size_t(i) * 2 + 2], d = rim[size_t(i) * 2 + 3];
		mb.tri(a, c, d, n);
		mb.tri(a, d, b, n);
	}
}


// Stands on a road that runs on the ground there: a column's foot keeps off it, not off a deck over it.
bool on_ground_road(const CityState &s, int self, float x, float z, float clearance, float raised) {
	bool hit = false;
	s.roads.for_segments_near(x - clearance, z - clearance, x + clearance, z + clearance, [&](int o) {
		if (hit || o == self) return;
		double t, d;
		closest_t(s.roads.segs[size_t(o)].q, x, z, t, d);
		if (d >= casing_half(s.roads.profile_of(o)) + clearance) return;
		hit = s.roads.seg_y(o, arc_at(s.roads.segs[size_t(o)].table, t)) - s.terrain.height_at(x, z) < raised;
	});
	return hit;
}

struct Deck {
	const RoadEmit &e;
	int sid;
	const RoadSegment &g;
	const Profile &p;
	const StructureSpec spec;
	const Tuning &tn;
	const StructureShape &K = e.L.shapes.road_structures; // streets.json shapes
	const Color concrete = structure_concrete(), deck_tone = rgb8_to_color(DECK_RGB);

	float soffit_at(const SegFrame &f) const { return f.c.y - spec.deck - K.soffit_band_m; }
	float ground(const Vector3 &q) const { return e.s.terrain.height_at(q.x, q.z); }
	float lift_at(double at) const {
		const SegFrame f = seg_frame(e.s, sid, at);
		return soffit_at(f) - ground(f.c);
	}

	// The deck's edge band, its sides and its soffit, from mouth to mouth.
	void slabs(double v0, double v1) const {
		const float casing = casing_half(p), half = p.half();
		const int n = std::max(1, int(std::ceil((v1 - v0) / tn.structure_parapet_step_m)));
		SegFrame a = seg_frame(e.s, sid, v0);
		for (int k = 1; k <= n; k++) {
			const SegFrame b = seg_frame(e.s, sid, v0 + (v1 - v0) * k / n);
			const Vector3 da(0, -spec.deck - K.soffit_band_m, 0), lift(0, e.L.h_sidewalk, 0);
			const Vector3 under(0, K.deck_top_under_m, 0);
			for (const float side : { 1.f, -1.f }) {
				const Vector3 oa = a.left * (side * casing), ob = b.left * (side * casing), ia = a.left * (side * half), ib = b.left * (side * half);
				structure_face(e.mb, a.c + ia + lift, b.c + ib + lift, b.c + ob + lift, a.c + oa + lift, deck_tone, UP);
				structure_face(e.mb, a.c + oa + lift, b.c + ob + lift, b.c + ob + da, a.c + oa + da, deck_tone, a.left * side);
			}
			structure_face(e.mb, a.c + a.left * casing + da, b.c + b.left * casing + da, b.c - b.left * casing + da, a.c - a.left * casing + da, concrete, -UP);
			// The structure's own top, just under the road: the road's surface throws no shadow, and the sun sees the soffit from behind.
			structure_face(e.mb, a.c - a.left * half - under, b.c - b.left * half - under, b.c + b.left * half - under, a.c + a.left * half - under, concrete, UP);
			a = b;
		}
	}

	// Where a ramp leaves its solid approach: an abutment there, and no bent near it.
	std::vector<double> abutments(double v0, double v1) const {
		std::vector<double> at;
		float before = lift_at(v0);
		for (double s = v0 + K.scan_m; s <= v1; s += K.scan_m) {
			const float here = lift_at(s);
			if ((before < tn.structure_abutment_lift_m) != (here < tn.structure_abutment_lift_m)) {
				const double mid = s - K.scan_m * HALF;
				const SegFrame f = seg_frame(e.s, sid, mid);
				const float soffit = soffit_at(f), foot = std::min(ground(f.c), soffit) - K.abutment_sink_m;
				slab(e.mb, Vector3(f.c.x, foot, f.c.z), f.fwd, f.left, tn.structure_abutment_depth_m, casing_half(p) * 2, soffit - foot, concrete);
				at.push_back(mid);
			}
			before = here;
		}
		return at;
	}

	// One bent: a crossbeam under the soffit on a column at each deck edge, none where the column would stand on a road.
	void bent(const SegFrame &f, float radius) const {
		const float half = p.half(), beam = soffit_at(f) - tn.structure_bearing_m - tn.structure_cap_height_m;
		const float spread = std::max(0.f, std::min(half * tn.structure_bent_spread, half - radius - 1.f));
		std::vector<Vector3> standing;
		float tallest = 0;
		for (const float side : { -1.f, 1.f }) {
			Vector3 q = f.c + f.left * (spread * side);
			q.y = ground(q);
			if (beam - q.y < tn.structure_min_support_m) continue;
			standing.push_back(q);
			tallest = std::max(tallest, beam - q.y);
			if (spread <= 0) break;
		}
		if (standing.empty()) return;
		const float width = std::min(radius * tn.structure_pier_max_width, std::max(radius, tallest / tn.structure_pier_slenderness));
		int built = 0;
		for (const Vector3 &q : standing) {
			if (on_ground_road(e.s, sid, q.x, q.z, width * K.pier_flare, tn.structure_raised_m)) continue;
			column(K, e.mb, q - UP * K.foot_sink_m, beam, width, concrete);
			built++;
		}
		if (built == 0) return;
		slab(e.mb, Vector3(f.c.x, beam, f.c.z), f.fwd, f.left, width * tn.structure_cap_depth, spread * 2 + width + tn.structure_cap_overhang_m * 2, tn.structure_cap_height_m, concrete);
	}

	// The bents at the road's own spacing, counted along the whole street a piece belongs to, so a road laid in pieces keeps its rhythm.
	void bents(double v0, double v1, const std::vector<double> &near_abutment) const {
		const bool bridge = structure_of(g.flags) == Structure::BRIDGE;
		const float spacing = bridge ? tn.structure_pier_spacing_bridge_m : tn.structure_pier_spacing_elevated_m;
		const float radius = bridge ? tn.structure_pier_radius_bridge_m : tn.structure_pier_radius_elevated_m;
		const double start = e.s.roads.street_run(sid).start, first = std::ceil((start + v0) / spacing - K.bent_first) + K.bent_first;
		for (double k = first; k * spacing - start < v1 - double(spacing) * K.bent_last; k += 1) {
			const double at = k * spacing - start;
			const bool clear = std::none_of(near_abutment.begin(), near_abutment.end(), [&](double a) { return std::fabs(a - at) < tn.structure_pier_abutment_gap_m; });
			if (clear) bent(seg_frame(e.s, sid, at), radius);
		}
	}

	// The barrier along one edge: its traffic face leans back, and it grows out of the road as the deck leaves the ground.
	void parapet(double v0, double v1, float side) const {
		const int n = std::max(1, int(std::ceil((v1 - v0) / tn.structure_parapet_step_m)));
		const float edge = casing_half(p) - K.deck_clear_in_m;
		struct Section { Vector3 out, in; float foot, top, grow; Vector3 fwd; };
		std::vector<Section> sec;
		for (int k = 0; k <= n; k++) {
			const SegFrame f = seg_frame(e.s, sid, v0 + (v1 - v0) * k / n);
			const Vector3 o = f.left * side, at = f.c + o * edge;
			const float grow = smooth(tn.structure_parapet_from_m, tn.structure_parapet_full_m, f.c.y - ground(at));
			sec.push_back({ at, -o, f.c.y - K.guard_base_under_m, f.c.y + K.soffit_band_m + tn.structure_parapet_height_m * grow, grow, f.fwd });
		}
		const float foot_w = tn.structure_parapet_foot_m, top_w = tn.structure_parapet_top_m;
		auto point = [&](const Section &q, float in, float y) { return Vector3(q.out.x + q.in.x * in, y, q.out.z + q.in.z * in); };
		bool open = false;
		for (size_t k = 0; k + 1 < sec.size(); k++) {
			const Section &a = sec[k], &b = sec[k + 1];
			if (a.grow <= GROWN && b.grow <= GROWN) {
				open = false;
				continue;
			}
			if (!open) structure_face(e.mb, point(a, 0, a.foot), point(a, foot_w, a.foot), point(a, top_w, a.top), point(a, 0, a.top), deck_tone, -a.fwd);
			open = true;
			const Vector3 outward = -(a.in + b.in) * HALF;
			structure_face(e.mb, point(a, 0, a.foot), point(b, 0, b.foot), point(b, 0, b.top), point(a, 0, a.top), deck_tone, outward);
			structure_face(e.mb, point(a, 0, a.top), point(b, 0, b.top), point(b, top_w, b.top), point(a, top_w, a.top), deck_tone, UP);
			const float lean = (foot_w - top_w) / std::max(K.least_lean_m, a.top - a.foot);
			structure_face(e.mb, point(a, top_w, a.top), point(b, top_w, b.top), point(b, foot_w, b.foot), point(a, foot_w, a.foot), deck_tone, (-outward + UP * lean).normalized());
			if (k + 2 == sec.size() && b.grow > GROWN) structure_face(e.mb, point(b, 0, b.foot), point(b, foot_w, b.foot), point(b, top_w, b.top), point(b, 0, b.top), deck_tone, b.fwd);
		}
	}
};

// One mouth of a bore: the jambs and wing walls either side, the lintel over the opening and its coping.
void portal(const RoadEmit &e, const SegFrame &f, const Profile &p, const Tuning &tn) {
	const StructureShape &K = e.L.shapes.road_structures;
	const Color concrete = structure_concrete();
	const float road = f.c.y, half = p.half(), crown = road + tn.grade_tunnel_headroom_m + tn.grade_tunnel_arch_m;
	const float top = std::max(crown + K.crown_over_m, e.s.terrain.height_at(f.c.x, f.c.z) + K.ground_over_m);
	const float thick = tn.structure_portal_thickness_m, reach = thick * HALF + K.portal_reach_m;
	const float wide = K.jamb_m + tn.structure_portal_wing_m;
	const int slices = std::max(2, int(std::ceil(wide / tn.structure_portal_slice_m)));
	const float w = wide / float(slices);
	for (const float side : { -1.f, 1.f })
		for (int k = 0; k < slices; k++) {
			const float at = half + w * (float(k) + HALF);
			const Vector3 q = f.c + f.left * (at * side), ahead = q + f.fwd * reach, back = q - f.fwd * reach;
			const float g0 = e.s.terrain.height_at(q.x, q.z), g1 = e.s.terrain.height_at(ahead.x, ahead.z), g2 = e.s.terrain.height_at(back.x, back.z);
			const float ground = std::max({ g0, g1, g2 }), low = std::min(g1, g2);
			const bool jamb = at - half < K.jamb_m;
			if (!jamb && ground < road + K.wing_over_road_m) continue; // over ground lower than the road it holds nothing back
			const float slice_top = jamb ? std::max(crown + K.crown_over_m, std::min(top, ground + tn.structure_portal_coping_m)) : std::min(top, ground + tn.structure_portal_coping_m);
			const float foot = std::min(road, low) - K.abutment_sink_m;
			if (slice_top - foot < K.wing_least_m) continue;
			slab(e.mb, Vector3(q.x, foot, q.z), f.fwd, f.left, thick, w + K.slice_lap_m, slice_top - foot, concrete);
		}
	const float across = (half + K.jamb_m) * 2;
	slab(e.mb, Vector3(f.c.x, crown, f.c.z), f.fwd, f.left, thick, across, std::max(K.lintel_least_m, top - crown), concrete);
	slab(e.mb, Vector3(f.c.x, top, f.c.z), f.fwd, f.left, thick + K.headwall_out_m, across + K.headwall_out_m, K.headwall_h_m, concrete);
}

// Where the ground first closes over the road, along it: a portal each, but for one by a junction already buried.
void portals(const RoadEmit &e, int sid, const Tuning &tn) {
	const StructureShape &K = e.L.shapes.road_structures;
	const RoadSegment &g = e.s.roads.segs[size_t(sid)];
	const float step = std::min(K.portal_step_most_m, std::max(K.portal_step_least_m, float(g.len) * K.portal_step_share));
	auto cover = [&](double at) {
		const SegFrame f = seg_frame(e.s, sid, at);
		return e.s.terrain.height_at(f.c.x, f.c.z) - f.c.y;
	};
	auto buried = [&](int n) { return e.s.roads.nodes[size_t(n)].segs.size() > 1 && e.s.roads.nodes[size_t(n)].lift < -tn.grade_tunnel_headroom_m; };
	float before = cover(0);
	for (double s = step; s <= g.len; s += step) {
		const float here = cover(s);
		const bool crossed = (before < tn.structure_portal_cover_m) != (here < tn.structure_portal_cover_m);
		before = here;
		if (!crossed || (s < K.near_end_m && buried(g.a)) || (g.len - s < K.near_end_m && buried(g.b))) continue;
		portal(e, seg_frame(e.s, sid, s - step * HALF), e.s.roads.profile_of(sid), tn);
	}
}
} // namespace

Color structure_concrete() { return rgb8_to_color(CONCRETE_RGB); }

void structure_face(MeshBuilder &mb, const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d, const Color &col, const Vector3 &out) {
	const Vector2 uv[4] = { Vector2(BOX_ROUGHNESS, METAL), Vector2(BOX_ROUGHNESS, METAL), Vector2(BOX_ROUGHNESS, METAL), Vector2(BOX_ROUGHNESS, METAL) };
	mb.quad(a, b, c, d, col, out, uv);
}

void emit_structures(const RoadEmit &e, int sid) {
	const StructureShape &K = e.L.shapes.road_structures;
	const RoadSegment &g = e.s.roads.segs[size_t(sid)];
	const Profile &p = e.s.catalog.profile(g.profile);
	const Tuning &tn = e.s.catalog.tune;
	if (g.len <= 0 || p.edges.empty()) return;
	const Structure st = structure_of(g.flags);
	const float bore = tn.grade_tunnel_headroom_m + tn.grade_tunnel_arch_m + tn.structure_bore_over_m;
	bool raised = is_raised(st), sunk = is_sunken(st);
	const double scan = std::max(double(K.scan_road_m), g.len / SCAN_MOST);
	for (double s = 0; s <= g.len && !(raised && sunk); s += scan) {
		const SegFrame f = seg_frame(e.s, sid, s);
		const float over = f.c.y - e.s.terrain.height_at(f.c.x, f.c.z);
		raised = raised || over > tn.structure_raised_m;
		sunk = sunk || over < -bore;
	}
	if (raised) {
		const Deck deck{ e, sid, g, p, structure_spec(is_raised(st) ? st : Structure::ELEVATED, tn), tn };
		const double v0 = g.retreat_a, v1 = g.len - g.retreat_b;
		if (v1 > v0) {
			deck.slabs(v0, v1);
			const std::vector<double> near_abutment = deck.abutments(v0, v1);
			if (stayed(e, sid)) emit_stays(e, sid, v0, v1, casing_half(p));
			else deck.bents(v0, v1, near_abutment);
			deck.parapet(v0, v1, 1.f);
			deck.parapet(v0, v1, -1.f);
		}
	}
	if (sunk) portals(e, sid, tn);
	emit_retaining_walls(e, sid);
}

} // namespace city
