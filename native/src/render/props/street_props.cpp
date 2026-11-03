// Street furniture placement (Roadcraft jobs/props.ts placeSegment / placeJunction), see street_props.h.
#include "render/props/street_props.h"

#include "render/owned_segments.h"

#include "core/color.h"
#include "core/random.h"
#include "render/roads/road_mesh.h"
#include "sim/roads/junction.h"
#include "sim/traffic/traffic_system.h"

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int FURN_FULL = 0, FURN_NONE = 2, MAX_STRANDS = 4;
constexpr int ZONE_RES = 1, ZONE_IND = 3, ZONE_RES_LOW = 5, ZONE_RES_HIGH = 6;
constexpr uint32_t TREE_SALT = 0x7ee5u, TREE_SPECIES = 2;
constexpr float CANOPY_SCALE_MIN = 0.8f, TREE_SCALE_SPAN = 0.35f;

const Vector3 UP(0, 1, 0);

bool is_res(int z) { return z == ZONE_RES || z == ZONE_RES_LOW || z == ZONE_RES_HIGH; }

// A post on every approach with lanes into signalled node `n` (Roadcraft world/signalPosts.ts): on the footway at the
// approach's right, signal_in from the kerb, signal_back past the crosswalk (at most signal_max_share of the
// segment), its arm over the arriving lanes and its walkers' head across the crosswalk.
void signals(const CityState &s, const StreetLook &L, const FurnitureLook &F, int n, std::vector<SignalHead> &out) {
	for (int sid : s.roads.nodes[n].segs) {
		const RoadSegment &g = s.roads.segs[sid];
		if (!g.alive || g.a == g.b) continue;
		const Profile &p = s.catalog.profile(g.profile);
		const bool at_a = g.a == n; // arriving at a: travelling b to a, the back lanes
		if ((at_a ? p.back : p.fwd).empty() || p.edges.empty()) continue;
		const double inner = g.len - g.retreat_a - g.retreat_b;
		const double back = std::min(double(L.zebra_to + F.signal_back), inner * F.signal_max_share);
		const SegFrame f = seg_frame(s, sid, at_a ? g.retreat_a + back : g.len - g.retreat_b - back);
		// the kerb on the arriving traffic's right: the left edge's inner side at a, the right edge's at b
		const float u = at_a ? p.edges.back().u0 + F.signal_in : p.edges.front().u1 - F.signal_in;
		const Vector3 face = at_a ? f.fwd : -f.fwd; // +z toward the arriving traffic
		out.push_back({ traffic::in_ds(s.roads, sid, n), f.c + f.left * u + UP * L.h_sidewalk, PropKit::facing(face) });
	}
}

// One sagging strand between two attachment points, as a thin vertical and a thin horizontal ribbon.
void strand(MeshBuilder &mb, const WireLook &w, const Vector3 &a, const Vector3 &b) {
	Vector3 d = b - a;
	float span = Vector2(d.x, d.z).length();
	Vector3 side = Vector3(-d.z, 0, d.x).normalized() * (w.thick * 0.5f), lift(0, w.thick * 0.5f, 0);
	const Vector2 t2(float(int(PropTag::FIXED)), 0);
	for (int k = 0; k < w.pieces; k++) {
		float t0 = float(k) / w.pieces, t1 = float(k + 1) / w.pieces;
		Vector3 p0 = a + d * t0 - UP * (4 * t0 * (1 - t0) * w.sag * span), p1 = a + d * t1 - UP * (4 * t1 * (1 - t1) * w.sag * span);
		mb.quad(p0 - lift, p1 - lift, p1 + lift, p0 + lift, w.color, side, nullptr, t2);
		mb.quad(p0 - side, p1 - side, p1 + side, p0 + side, w.color, UP, nullptr, t2);
	}
}

struct SegCtx {
	const CityState &s;
	const StreetLook &L;
	const FurnitureLook &F;
	int sid;
	const Profile &p;
	double v0, v1;
	LampRow row;
	std::vector<double> stations;
	bool clear(double v) const {
		if (v < v0 + F.end_clear || v > v1 - F.end_clear) return false;
		for (double st : stations)
			if (std::fabs(st - v) < F.lamp_clear) return false;
		return true;
	}
	PropPlace at(PropKind k, double v, float u, const Vector3 &face_left) const {
		SegFrame f = seg_frame(s, sid, v);
		Vector3 face = f.left * face_left.x;
		return { k, f.c + f.left * u + UP * L.h_sidewalk, PropKit::facing(face) };
	}
};

void lamps(SegCtx &c, std::vector<PropPlace> &out, MeshBuilder &wires) {
	if (!c.row.any) return;
	for (double v = c.v0 + c.L.lamp.first; v < c.v1 - c.L.lamp.end_clear; v += c.L.lamp.step) c.stations.push_back(v);
	float side = c.row.head_u > c.row.pole_u ? 1.f : -1.f;
	const WireLook &w = c.F.wire;
	for (size_t i = 0; i < c.stations.size(); i++) {
		out.push_back(c.at(PropKind::LAMP, c.stations[i], c.row.pole_u, Vector3(side, 0, 0)));
		if (c.row.twin) out.push_back(c.at(PropKind::LAMP, c.stations[i], c.row.pole_u2, Vector3(-1, 0, 0)));
		if (i == 0) continue;
		const PropPlace &a = out[out.size() - (c.row.twin ? 4 : 2)], &b = out[out.size() - (c.row.twin ? 2 : 1)];
		float span = (b.origin - a.origin).length();
		if (span < w.min_span || span > w.max_span) continue;
		for (int k = 0; k < w.strands && k < MAX_STRANDS; k++)
			strand(wires, w, a.origin + a.basis.xform(Vector3(0, w.y[k], w.z[k])), b.origin + b.basis.xform(Vector3(0, w.y[k], w.z[k])));
	}
}

void furniture(const SegCtx &c, int level, std::vector<PropPlace> &out) {
	const FurnitureLook &F = c.F;
	const RoadSegment &g = c.s.roads.segs[c.sid];
	const StripEdge &right = c.p.edges.front(), &left = c.p.edges.back();
	double span = c.v1 - c.v0;
	bool walk_r = right.strip.kind == StripKind::SIDEWALK && right.u1 - right.u0 >= F.min_walk;
	bool walk_l = left.strip.kind == StripKind::SIDEWALK && left.u1 - left.u0 >= F.min_walk;
	if (walk_r && span > F.hydrant_min_span)
		for (double v = c.v0 + F.hydrant_first; v < c.v1; v += F.hydrant_step)
			if (c.clear(v)) out.push_back(c.at(PropKind::HYDRANT, v, right.u1 - F.kerb_u, Vector3(1, 0, 0)));
	if (level != FURN_FULL) return;
	if (span > F.bin_min_span) {
		if (walk_r && g.zr != ZONE_IND && c.clear(c.v0 + F.bin_first)) out.push_back(c.at(PropKind::BIN, c.v0 + F.bin_first, right.u0 + F.wall_u, Vector3(1, 0, 0)));
		if (walk_l && g.zl != ZONE_IND && c.clear(c.v1 - F.bin_first)) out.push_back(c.at(PropKind::BIN, c.v1 - F.bin_first, left.u1 - F.wall_u, Vector3(-1, 0, 0)));
	}
	double mid = (c.v0 + c.v1) / 2;
	if (walk_r && is_res(g.zr) && span > F.bench_min_span && right.u1 - right.u0 >= F.bench_min_walk && c.clear(mid)) {
		out.push_back(c.at(PropKind::BENCH, mid, right.u0 + F.wall_u, Vector3(1, 0, 0)));
		out.push_back(c.at(PropKind::BIN, mid + F.bin_after_bench, right.u0 + F.wall_u, Vector3(1, 0, 0)));
	}
}

struct SegStart {
	const RoadSegment *g;
	const Profile *p;
	int level;
};

// The segment's furniture level and profile, or false when it has nothing placed on it.
bool seg_start(const CityState &s, int sid, SegStart &out) {
	const RoadSegment &g = s.roads.segs[sid];
	const Profile &p = s.catalog.profile(g.profile);
	int level = (g.flags & SEG_FURNITURE_MASK) >> SEG_FURNITURE_SHIFT;
	if (level == FURN_NONE || p.edges.empty()) return false;
	out = { &g, &p, level };
	return true;
}

// Runs fn(ctx, furniture level) for every zoned segment the chunk owns.
template <typename Fn>
void each_seg_ctx(const CityState &s, const StreetLook &L, const FurnitureLook &F, int64_t key, Fn &&fn) {
	each_owned_segment(s, key, [&](int sid) {
		SegStart st;
		if (!seg_start(s, sid, st)) return;
		SegCtx c{ s, L, F, sid, *st.p, st.g->retreat_a, st.g->len - st.g->retreat_b, lamp_row(*st.p, L.lamp), {} };
		fn(c, st.level);
	});
}

} // namespace

void place_street_props(const CityState &s, const StreetLook &L, const FurnitureLook &F, int64_t key, std::vector<PropPlace> &out,
		MeshBuilder &wires, std::vector<SignalHead> &heads) {
	each_seg_ctx(s, L, F, key, [&](SegCtx &c, int level) {
		lamps(c, out, wires);
		furniture(c, level, out);
	});
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.roads.node_hash.query(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int n) {
		const RoadNode &nd = s.roads.nodes[n];
		if (nd.alive && chunk_of(float(nd.x), float(nd.z)) == key && traffic::control_of(s, n) == Control::SIGNAL) signals(s, L, F, n, heads);
	});
}

void place_street_trees(const CityState &s, const StreetLook &L, const FurnitureLook &F, int64_t key, std::vector<TreeSpot> &out) {
	each_seg_ctx(s, L, F, key, [&](SegCtx &c, int) {
		int sid = c.sid;
		for (double v = c.v0 + L.lamp.first; v < c.v1 - L.lamp.end_clear; v += L.lamp.step) c.stations.push_back(v);
		for (int side = 0; side < 2; side++) {
			const StripEdge &e = side == 0 ? c.p.edges.front() : c.p.edges.back();
			if (e.strip.kind != StripKind::SIDEWALK || e.u1 - e.u0 < F.tree_min_walk) continue;
			float u = side == 0 ? e.u1 - F.tree_kerb_u : e.u0 + F.tree_kerb_u;
			for (double v = c.v0 + F.tree_first; v < c.v1 - F.tree_first; v += F.tree_step) {
				bool near_lamp = false;
				for (double st : c.stations) near_lamp = near_lamp || std::fabs(st - v) < F.tree_clear;
				if (near_lamp) continue;
				SegFrame f = seg_frame(s, sid, v);
				uint32_t h = hash_combine(uint32_t(sid) * TREE_SALT, uint32_t(v * side + v));
				out.push_back({ f.c + f.left * u + UP * L.h_sidewalk, CANOPY_SCALE_MIN + TREE_SCALE_SPAN * hash_float(h),
						TAU_F * hash_float(h + 1), int(hash_u32(h) % TREE_SPECIES) });
			}
		}
	});
}

bool FurnitureLook::load(const Dictionary &t, PackedStringArray &errors) {
	if (!t.has("furniture") || !t.has("wires")) {
		errors.push_back("streets.json: faltam furniture/wires");
		return false;
	}
	Dictionary f = t["furniture"], w = t["wires"];
	min_walk = f["min_walk_m"]; kerb_u = f["kerb_u_m"]; wall_u = f["wall_u_m"]; end_clear = f["end_clear_m"]; lamp_clear = f["lamp_clear_m"];
	hydrant_first = f["hydrant_first_m"]; hydrant_step = f["hydrant_step_m"]; hydrant_min_span = f["hydrant_min_span_m"];
	bin_first = f["bin_first_m"]; bin_min_span = f["bin_min_span_m"]; bench_min_span = f["bench_min_span_m"];
	bench_min_walk = f["bench_min_walk_m"]; bin_after_bench = f["bin_after_bench_m"];
	signal_back = f["signal_back_m"]; signal_in = f["signal_in_m"]; signal_max_share = f["signal_max_share"];
	tree_first = f["tree_first_m"]; tree_step = f["tree_step_m"]; tree_min_walk = f["tree_min_walk_m"]; tree_kerb_u = f["tree_kerb_u_m"];
	tree_clear = f["tree_clear_m"];
	Array ys = w["y_m"], zs = w["z_m"];
	wire.strands = int(std::min<int64_t>(MAX_STRANDS, std::min(ys.size(), zs.size())));
	for (int k = 0; k < wire.strands; k++) {
		wire.y[k] = ys[k];
		wire.z[k] = zs[k];
	}
	wire.pieces = w["pieces"]; wire.thick = w["thick_m"]; wire.sag = w["sag"]; wire.min_span = w["min_span_m"]; wire.max_span = w["max_span_m"];
	wire.color = rgba8_to_color(parse_hex_color(String(w["color"]).utf8().get_data()));
	return true;
}

} // namespace city
