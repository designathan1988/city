// The street furniture as it is drawn, from the simulation's places (sim/roads/street_furniture.h, sim/street), see
// street_props.h. A pole is one instance of the pole mesh with the parts it shows in INSTANCE_CUSTOM.a (prop_kit.h
// PolePart); a wire is one instance whose transform carries its chord and INSTANCE_CUSTOM its sag and phase (props.gdshader).
#include "render/props/street_props.h"

#include "render/owned_segments.h"

#include "core/color.h"
#include "core/random.h"
#include "render/roads/road_mesh.h"
#include "sim/street/pole_arms.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t TREE_SALT = 0x7ee5u, TREE_SPECIES = 2;
constexpr float CANOPY_SCALE_MIN = 0.8f, TREE_SCALE_SPAN = 0.35f, SHRUB_SCALE = 0.18f;
constexpr uint32_t PROP_KINDS = furniture_bit(FurnitureKind::LAMP) | furniture_bit(FurnitureKind::HYDRANT) | furniture_bit(FurnitureKind::BIN) |
		furniture_bit(FurnitureKind::BENCH) | furniture_bit(FurnitureKind::COLUMN) | furniture_bit(FurnitureKind::POSTBOX) |
		furniture_bit(FurnitureKind::PHONE) | furniture_bit(FurnitureKind::DRAIN) | furniture_bit(FurnitureKind::TREE);
constexpr int CAN_PERIOD = 5, CAN_PHASE = 3; // a transformer on one pole in five
constexpr float PHASE_STEP = 2.399f; // the golden angle: neighbouring spans swing out of step
constexpr int LIGHT_POOL = 1, LIGHT_BEAM = 2; // parts of the lamp light mesh
constexpr int JOINT_ARMS = 2; // a street going on through a node
const Vector3 UP(0, 1, 0);
const Color WHITE(1, 1, 1, 1);

Vector3 world_of(const CityState &s, const StreetLook &L, const FurnitureSpot &p, Basis *basis) {
	const SegFrame f = seg_frame(s, p.seg, p.v);
	if (basis) *basis = PropKit::facing(f.left * p.face_left + f.fwd * p.face_fwd);
	return f.c + f.left * p.u + UP * L.h_sidewalk;
}

void push_part(PackedFloat32Array &buf, const Basis &b, const Vector3 &o, int mask) {
	mm_push(buf, b.get_column(0), b.get_column(1), b.get_column(2), o, WHITE, 0, 0, 0, float(mask));
}

// One wire between two points: x along its chord, y up, z across (props.gdshader bends it from t = 0 to 1).
void push_wire(PackedFloat32Array &buf, const WireLook &w, const Vector3 &a, const Vector3 &b, float phase) {
	const Vector3 d = b - a;
	const float span = Vector2(d.x, d.z).length();
	const Vector3 across = Vector3(-d.z, 0, d.x).normalized();
	mm_push(buf, d, UP, across, a, w.color, std::min(w.max_sag, span * w.sag), phase, 0, 0);
}

// Every wire of a span between two arms (each end its pole's place and the arm the span hangs from).
void push_span(StreetInstances &inst, const WireLook &w, const Vector3 &a, const Vector3 &arm_a, const Vector3 &b, const Vector3 &arm_b, float phase) {
	const float span = Vector2(b.x - a.x, b.z - a.z).length();
	if (span < w.min_span || span > w.max_span) return;
	for (float y : w.y)
		for (float z : w.z) push_wire(inst.wires, w, a + UP * y + arm_a * z, b + UP * y + arm_b * z, phase);
}

// What a lamp's pool lies on: how far it reaches across (the street's width, sim/roads lamp_row) and the rise per metre
// along `along` of the road under it.
struct PoolBed {
	float across = 0, rise = 0;
};

PoolBed pool_bed(const CityState &s, int seg, double v, float half, const Vector3 &along) {
	const LampDef &d = s.catalog.furniture.lamps;
	if (seg < 0) return { d.pool_least_m, 0 };
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	const double a = std::max(0.0, v - half), b = std::min(g.len, v + half);
	const float grade = b > a ? float((s.roads.seg_y(seg, b) - s.roads.seg_y(seg, a)) / (b - a)) : 0.f;
	return { std::max(lamp_row(s.roads, seg).across, d.pool_least_m), grade * along.dot(seg_frame(s, seg, v).fwd) };
}

// A lamp's light, on segment `seg` at arc `v` (-1: off the streets): one long disc over the road and the footway (a road
// luminaire's patch, not a spotlight's), added to whatever lies under it, so a junction or a corner is lit like the rest.
void push_light(StreetInstances &inst, const CityState &s, const FurnitureLook &F, int seg, double v, const Vector3 &foot, const Vector3 &toward, float reach, float height) {
	const Vector3 head = foot + toward * reach, along = toward.cross(UP);
	const float half = height * F.pool_along;
	const PoolBed bed = pool_bed(s, seg, v, half, along);
	const Vector3 middle = foot + toward * std::max(reach, bed.across - s.catalog.furniture.lamps.pool_back_m) + UP * F.pool_lift;
	mm_push(inst.light, (along + UP * bed.rise) * half, UP * height, toward * bed.across, middle, WHITE, 0, 0, 0, float(LIGHT_POOL));
	if (F.beams) // lit air: rain and storm, and there is no weather yet
		mm_push(inst.light, Vector3(F.beam_radius, 0, 0), UP * height, Vector3(0, 0, F.beam_radius), head, WHITE, 0, 0, 0, float(LIGHT_BEAM));
}

struct PoleFrame {
	Vector3 foot, kerb;
	PoleArms arms;
	int seg = -1; // the street it stands on, and where along it
	double v = 0;
};

PoleFrame frame_of(const CityState &s, const StreetLook &L, int id) {
	const UtilityPole &p = s.street.poles.items[size_t(id)];
	const FootwayHit h = footway_at(s.roads, p.x, p.z, s.catalog.furniture.items.stand_reach_m);
	PoleFrame f;
	f.foot = Vector3(p.x, street_ground(s, L, p.x, p.z), p.z);
	ArmDir idle;
	if (h.ok) {
		idle = { h.kx, h.kz };
		f.seg = h.seg;
		f.v = h.v;
	}
	f.arms = pole_arms(s.street, s.catalog.furniture.poles, id, idle);
	f.kerb = h.ok ? Vector3(h.kx, 0, h.kz) : Vector3(f.arms.arms[0].x, 0, f.arms.arms[0].z);
	return f;
}

// A pole the player built, with its arms, and the spans it starts.
void place_pole(const CityState &s, const StreetLook &L, const FurnitureLook &F, int id, StreetInstances &inst) {
	const UtilityPole &p = s.street.poles.items[size_t(id)];
	const PoleFrame f = frame_of(s, L, id);
	int mask = int(PolePart::MAST);
	if (p.lamp) mask |= int(PolePart::LAMP);
	if (id % CAN_PERIOD == CAN_PHASE) mask |= int(PolePart::CAN);
	push_part(inst.poles, PropKit::facing(f.kerb), f.foot, mask);
	for (const ArmDir &a : f.arms.arms) push_part(inst.poles, PropKit::facing(Vector3(a.x, 0, a.z)), f.foot, int(PolePart::ARM));
	if (p.lamp) push_light(inst, s, F, f.seg, f.v, f.foot, f.kerb, F.lamp_reach, F.lamp_height);
	for (size_t i = 0; i < p.spans.size(); i++) {
		const UtilitySpan &sp = s.street.spans.items[size_t(p.spans[i])];
		if (sp.a != id) continue;
		const PoleFrame o = frame_of(s, L, sp.b);
		const std::vector<int> &theirs = s.street.poles.items[size_t(sp.b)].spans;
		const size_t at = size_t(std::find(theirs.begin(), theirs.end(), p.spans[i]) - theirs.begin());
		const ArmDir mine = f.arms.by_span[i], other = o.arms.by_span[at];
		push_span(inst, F.wire, f.foot, Vector3(mine.x, 0, mine.z), o.foot, Vector3(other.x, 0, other.z), float(p.spans[i]) * PHASE_STEP);
	}
}

// The first or last pole of `seg`'s main row at node `n`, when it stands on `side`.
bool row_end(const CityState &s, const StreetLook &L, int seg, int n, int side, PropPlace &out) {
	const LampRow row = lamp_row(s.roads, seg);
	if (!row.any || row.stations == 0 || row.side != side) return false;
	std::vector<FurnitureSpot> spots;
	traffic::street_furniture_of(s, seg, furniture_bit(FurnitureKind::LAMP), spots);
	const int station = s.roads.segs[size_t(seg)].a == n ? 0 : row.stations - 1;
	for (const FurnitureSpot &p : spots) {
		if (p.row != 0 || p.station != station) continue;
		out.origin = world_of(s, L, p, &out.basis);
		return true;
	}
	return false;
}

// A street going on through its node keeps its line: the last pole of `sid` strung to the first of the next (Roadcraft
// powerLine.ts join), by the lower segment of the two.
void join_rows(const CityState &s, const StreetLook &L, const FurnitureLook &F, int sid, int row_side, StreetInstances &inst) {
	const RoadSegment &g = s.roads.segs[size_t(sid)];
	for (int n : { g.a, g.b }) {
		const std::vector<int> &arms = s.roads.nodes[size_t(n)].segs;
		if (int(arms.size()) != JOINT_ARMS) continue;
		const int other = arms[0] == sid ? arms[1] : arms[0];
		if (other <= sid) continue;
		const bool same_way = (g.b == n) == (s.roads.segs[size_t(other)].a == n); // the sides keep their names through the node
		PropPlace a, b;
		if (!row_end(s, L, sid, n, row_side, a) || !row_end(s, L, other, n, same_way ? row_side : 1 - row_side, b)) continue;
		push_span(inst, F.wire, a.origin, a.basis.get_column(2), b.origin, b.basis.get_column(2) * (same_way ? 1.f : -1.f), float(sid) * PHASE_STEP);
	}
}

// A row pole: one instance, its arm across the street; a pole of the row on the other side is a lighting column.
void place_row_pole(const CityState &s, const FurnitureLook &F, const FurnitureSpot &p, int sid, const PropPlace &at, StreetInstances &inst) {
	const uint32_t h = hash_combine(uint32_t(sid), uint32_t(p.station));
	const bool column = p.row != 0 || p.kind == FurnitureKind::COLUMN;
	int mask = column ? int(PolePart::COLUMN) : int(PolePart::MAST) | int(PolePart::ARM) | (p.lit ? int(PolePart::LAMP) : 0);
	if (!column && h % uint32_t(CAN_PERIOD) == uint32_t(CAN_PHASE)) mask |= int(PolePart::CAN);
	push_part(inst.poles, at.basis, at.origin, mask);
	if (p.lit) push_light(inst, s, F, sid, p.v, at.origin, at.basis.get_column(2), column ? F.column_reach : F.lamp_reach, column ? F.column_height : F.lamp_height);
}

PropKind prop_kind(FurnitureKind k) {
	switch (k) {
		case FurnitureKind::HYDRANT: return PropKind::HYDRANT;
		case FurnitureKind::BIN: return PropKind::BIN;
		case FurnitureKind::BENCH: return PropKind::BENCH;
		case FurnitureKind::POSTBOX: return PropKind::POSTBOX;
		case FurnitureKind::PHONE: return PropKind::PHONE;
		case FurnitureKind::DRAIN: return PropKind::DRAIN;
		case FurnitureKind::TREE: return PropKind::TREE_PIT;
		default: return PropKind::SIGNAL;
	}
}

void place_segment(const CityState &s, const StreetLook &L, const FurnitureLook &F, int sid, std::vector<PropPlace> &out, StreetInstances &inst) {
	std::vector<FurnitureSpot> spots;
	traffic::street_furniture_of(s, sid, PROP_KINDS, spots);
	bool wired = false;
	PropPlace last;
	int last_station = -1, row_side = 0;
	for (const FurnitureSpot &p : spots) {
		PropPlace place;
		place.origin = world_of(s, L, p, &place.basis);
		if (p.kind == FurnitureKind::LAMP || p.kind == FurnitureKind::COLUMN) {
			place_row_pole(s, F, p, sid, place, inst);
			if (p.kind != FurnitureKind::LAMP || p.row != 0) continue;
			row_side = p.side;
			if (wired && p.station == last_station + 1) {
				const Vector3 arm = place.basis.get_column(2);
				push_span(inst, F.wire, last.origin, last.basis.get_column(2), place.origin, arm, float(sid + p.station) * PHASE_STEP);
			}
			last = place;
			last_station = p.station;
			wired = true;
			continue;
		}
		place.kind = prop_kind(p.kind);
		out.push_back(place);
	}
	if (wired) join_rows(s, L, F, sid, row_side, inst);
}
} // namespace

float street_ground(const CityState &s, const StreetLook &L, double x, double z) {
	const FootwayHit h = footway_at(s.roads, x, z, s.catalog.furniture.items.stand_reach_m);
	if (!h.ok) return s.terrain.height_at(float(x), float(z));
	double px, pz, y;
	footway_point(s.roads, h.seg, h.v, h.u, px, pz, y);
	return float(y) + L.h_sidewalk;
}

void place_street_props(const CityState &s, const StreetLook &L, const FurnitureLook &F, int64_t key, std::vector<PropPlace> &out, StreetInstances &inst) {
	each_owned_segment(s, key, [&](int sid) { place_segment(s, L, F, sid, out, inst); });
	const float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.street.poles_in(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int id) {
		const UtilityPole &p = s.street.poles.items[size_t(id)];
		if (chunk_of(p.x, p.z) == key) place_pole(s, L, F, id, inst);
	});
	s.street.items_in(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int id) {
		const StreetItem &it = s.street.items.items[size_t(id)];
		if (it.kind != ItemKind::MEADOW || chunk_of(it.x, it.z) != key) return;
		PropPlace place;
		place.kind = PropKind::MEADOW;
		place.origin = Vector3(it.x, street_ground(s, L, it.x, it.z), it.z);
		place.basis = PropKit::facing(Vector3(std::sin(float(id)), 0, std::cos(float(id))));
		out.push_back(place);
	});
	s.roads.node_hash.query(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int n) {
		const RoadNode &nd = s.roads.nodes[n];
		if (!nd.alive || chunk_of(float(nd.x), float(nd.z)) != key || traffic::control_of(s, n) != Control::SIGNAL) return;
		FurnitureSpot post;
		for (int sid : nd.segs) {
			if (!signal_post(s.roads, n, sid, post)) continue;
			SignalHead h;
			h.ds = traffic::in_ds(s.roads, sid, n);
			h.origin = world_of(s, L, post, &h.basis);
			inst.heads.push_back(h);
		}
	});
}

void place_pole_preview(const CityState &s, const StreetLook &L, const FurnitureLook &F, const std::vector<Vector3> &at, const std::vector<bool> &lamp,
		const std::vector<bool> &standing, StreetInstances &inst) {
	for (size_t i = 0; i < at.size(); i++) {
		const Vector3 foot(at[i].x, street_ground(s, L, at[i].x, at[i].z), at[i].z);
		const Vector3 next = i + 1 < at.size() ? at[i + 1] : at[i], prev = i > 0 ? at[i - 1] : at[i];
		const Vector3 along = (next - prev).normalized(), arm(along.z, 0, -along.x);
		if (!standing[i]) push_part(inst.poles, PropKit::facing(arm), foot, int(PolePart::MAST) | int(PolePart::ARM) | (lamp[i] ? int(PolePart::LAMP) : 0));
		if (i + 1 == at.size()) break;
		const Vector3 far(next.x, street_ground(s, L, next.x, next.z), next.z);
		push_span(inst, F.wire, foot, arm, far, arm, 0);
	}
}

void place_street_trees(const CityState &s, const StreetLook &L, int64_t key, std::vector<TreeSpot> &out) {
	std::vector<FurnitureSpot> spots;
	each_owned_segment(s, key, [&](int sid) {
		spots.clear();
		traffic::street_furniture_of(s, sid, furniture_bit(FurnitureKind::TREE) | furniture_bit(FurnitureKind::SHRUB), spots);
		for (const FurnitureSpot &p : spots) {
			uint32_t h = hash_combine(uint32_t(sid) * TREE_SALT, uint32_t(p.v * p.side + p.v));
			const float scale = (CANOPY_SCALE_MIN + TREE_SCALE_SPAN * hash_float(h)) * (p.kind == FurnitureKind::SHRUB ? SHRUB_SCALE : 1.f);
			out.push_back({ world_of(s, L, p, nullptr), scale, TAU_F * hash_float(h + 1), int(hash_u32(h) % TREE_SPECIES) });
		}
	});
}

bool FurnitureLook::load(const Dictionary &t, PackedStringArray &errors) {
	if (!t.has("wires") || !t.has("lamp_light")) {
		errors.push_back("streets.json: falta wires ou lamp_light");
		return false;
	}
	Dictionary w = t["wires"], l = t["lamp_light"];
	Array ys = w["y_m"], zs = w["z_m"];
	for (int k = 0; k < ys.size(); k++) wire.y.push_back(ys[k]);
	for (int k = 0; k < zs.size(); k++) wire.z.push_back(zs[k]);
	wire.radius = w["radius_m"]; wire.sag = w["sag"]; wire.max_sag = w["max_sag_m"]; wire.min_span = w["min_span_m"]; wire.max_span = w["max_span_m"];
	wire.near_end = w["tube_end_m"]; wire.far_end = w["far_end_m"];
	wire.color = rgba8_to_color(parse_hex_color(String(w["color"]).utf8().get_data()));
	pool_along = l["pool_along"]; pool_lift = l["pool_lift_m"]; beams = l["beams"]; beam_radius = l["beam_radius_m"]; lamp_height = l["lamp_height_m"]; lamp_reach = l["lamp_reach_m"];
	column_height = l["column_height_m"]; column_reach = l["column_reach_m"];
	return true;
}

} // namespace city
