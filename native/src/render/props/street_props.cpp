// The street furniture as it is drawn, from the simulation's places (sim/roads/street_furniture.h), see street_props.h.
#include "render/props/street_props.h"

#include "render/owned_segments.h"

#include "core/color.h"
#include "core/random.h"
#include "render/roads/road_mesh.h"
#include "sim/traffic/traffic_system.h"

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int MAX_STRANDS = 4;
constexpr uint32_t TREE_SALT = 0x7ee5u, TREE_SPECIES = 2;
constexpr float CANOPY_SCALE_MIN = 0.8f, TREE_SCALE_SPAN = 0.35f;
constexpr uint32_t PROP_KINDS = furniture_bit(FurnitureKind::LAMP) | furniture_bit(FurnitureKind::HYDRANT) |
		furniture_bit(FurnitureKind::BIN) | furniture_bit(FurnitureKind::BENCH);

const Vector3 UP(0, 1, 0);

PropKind prop_kind(FurnitureKind k) {
	switch (k) {
		case FurnitureKind::SIGNAL: return PropKind::SIGNAL;
		case FurnitureKind::HYDRANT: return PropKind::HYDRANT;
		case FurnitureKind::BIN: return PropKind::BIN;
		case FurnitureKind::BENCH: return PropKind::BENCH;
		default: return PropKind::LAMP;
	}
}

// The world place of a piece on the sidewalk top and, when asked, its turn: +z toward the road or the arriving traffic.
Vector3 world_of(const CityState &s, const StreetLook &L, const FurnitureSpot &p, Basis *basis) {
	const SegFrame f = seg_frame(s, p.seg, p.v);
	if (basis) *basis = PropKit::facing(f.left * p.face_left + f.fwd * p.face_fwd);
	return f.c + f.left * p.u + UP * L.h_sidewalk;
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

// The wires between two consecutive poles of a lamp row, when their span suits a wire.
void wire(MeshBuilder &mb, const WireLook &w, const PropPlace &a, const PropPlace &b) {
	const float span = (b.origin - a.origin).length();
	if (span < w.min_span || span > w.max_span) return;
	for (int k = 0; k < w.strands && k < MAX_STRANDS; k++)
		strand(mb, w, a.origin + a.basis.xform(Vector3(0, w.y[k], w.z[k])), b.origin + b.basis.xform(Vector3(0, w.y[k], w.z[k])));
}

} // namespace

void place_street_props(const CityState &s, const StreetLook &L, const FurnitureLook &F, int64_t key, std::vector<PropPlace> &out,
		MeshBuilder &wires, std::vector<SignalHead> &heads) {
	std::vector<FurnitureSpot> spots;
	each_owned_segment(s, key, [&](int sid) {
		spots.clear();
		traffic::street_furniture_of(s, sid, PROP_KINDS, spots);
		int pole = -1; // the last pole of the segment's main lamp row: the wires run from it to the next
		for (const FurnitureSpot &p : spots) {
			PropPlace place;
			place.kind = prop_kind(p.kind);
			place.origin = world_of(s, L, p, &place.basis);
			out.push_back(place);
			if (p.kind != FurnitureKind::LAMP || p.row != 0) continue;
			if (pole >= 0) wire(wires, F.wire, out[size_t(pole)], out.back());
			pole = int(out.size()) - 1;
		}
	});
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.roads.node_hash.query(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int n) {
		const RoadNode &nd = s.roads.nodes[n];
		if (!nd.alive || chunk_of(float(nd.x), float(nd.z)) != key || traffic::control_of(s, n) != Control::SIGNAL) return;
		FurnitureSpot post;
		for (int sid : nd.segs) {
			if (!signal_post(s.roads, n, sid, post)) continue;
			SignalHead h;
			h.ds = traffic::in_ds(s.roads, sid, n);
			h.origin = world_of(s, L, post, &h.basis);
			heads.push_back(h);
		}
	});
}

void place_street_trees(const CityState &s, const StreetLook &L, int64_t key, std::vector<TreeSpot> &out) {
	std::vector<FurnitureSpot> spots;
	each_owned_segment(s, key, [&](int sid) {
		spots.clear();
		traffic::street_furniture_of(s, sid, furniture_bit(FurnitureKind::TREE), spots);
		for (const FurnitureSpot &p : spots) {
			uint32_t h = hash_combine(uint32_t(sid) * TREE_SALT, uint32_t(p.v * p.side + p.v));
			out.push_back({ world_of(s, L, p, nullptr), CANOPY_SCALE_MIN + TREE_SCALE_SPAN * hash_float(h), TAU_F * hash_float(h + 1),
					int(hash_u32(h) % TREE_SPECIES) });
		}
	});
}

bool FurnitureLook::load(const Dictionary &t, PackedStringArray &errors) {
	if (!t.has("wires")) {
		errors.push_back("streets.json: falta wires");
		return false;
	}
	Dictionary w = t["wires"];
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
