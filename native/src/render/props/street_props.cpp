// The street furniture as it is drawn, from the simulation's places (sim/roads/street_furniture.h, sim/street), see
// street_props.h. A pole is one instance of the pole mesh with the parts it shows in INSTANCE_CUSTOM.a (prop_kit.h
// PolePart); a wire is one instance whose transform carries its chord and INSTANCE_CUSTOM its sag and phase (props.gdshader).
#include "render/props/street_props.h"
#include "core/shader_contracts/streets.h"

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
// A span's phase in the wind and the kinds of its two poles share INSTANCE_CUSTOM.y: the phase (under a turn) plus this times
// kind_a + 2 kind_b, which props.gdshader takes apart to pick each end's seats.
constexpr int KIND_B_WEIGHT = 2;
constexpr int JOINT_ARMS = 2; // a street going on through a node
const Vector3 UP(0, 1, 0);

Vector3 world_of(const CityState &s, const StreetLook &L, const FurnitureSpot &p, Basis *basis) {
	const SegFrame f = seg_frame(s, p.seg, p.v);
	if (basis) *basis = PropKit::facing(f.left * p.face_left + f.fwd * p.face_fwd);
	return f.c + f.left * p.u + UP * L.h_sidewalk;
}

void push_part(PackedFloat32Array &buf, const Basis &b, const Vector3 &o, int mask) {
	mm_push_plain(buf, b.get_column(0), b.get_column(1), b.get_column(2), o, 0, 0, 0, float(mask));
}

// The way an arm points, as the span's instance carries it.
float heading(const Vector3 &arm) { return std::atan2(arm.z, arm.x); }

// A span between two arms (each end its pole's place and the arm the span hangs from): one instance, because the renderer
// keeps three copies of every instance's floats on the CPU, and its mesh holds every wire (prop_kit.cpp wire_mesh). x along
// the chord, y up, z across; props.gdshader hangs each wire from the two arms and bends it from t = 0 to 1.
// The arms are the poles' arms turned toward the street (street_arm), along which each wire's seat lies, and the kinds say which
// seats (prop_kit.cpp wire_mesh).
void push_span(StreetInstances &inst, const WireLook &w, const Vector3 &a, const Vector3 &arm_a, const Vector3 &b, const Vector3 &arm_b, float phase,
		PoleKind kind_a, PoleKind kind_b) {
	const Vector3 d = b - a;
	const float span = Vector2(d.x, d.z).length();
	if (span < w.min_span || span > w.max_span) return;
	const Vector3 across = Vector3(-d.z, 0, d.x).normalized();
	const float kinds = PROP_SPAN_KIND_STRIDE * float(int(kind_a) + KIND_B_WEIGHT * int(kind_b));
	mm_push_plain(inst.wires, d, UP, across, a, std::min(w.max_sag, span * w.sag), std::fmod(phase, TAU_F) + kinds, heading(arm_a), heading(arm_b));
}

Vector3 flat(ArmDir a) { return Vector3(a.x, 0, a.z); }

// What the light of a lamp asks of it: how far out its lens hangs and how tall it stands.
struct LampLens {
	float reach = 0, height = 0;
};

// A lamp's light: its lens for the lights of render/props/lamp_lights.h, and the lit air under it.
void push_light(StreetInstances &inst, const FurnitureLook &F, const Vector3 &foot, const Vector3 &toward, const LampLens &lens) {
	const Vector3 head = foot + toward * lens.reach;
	inst.lamps.push_back(head + UP * lens.height);
	if (F.beams) // lit air: rain and storm, and there is no weather yet
		mm_push_plain(inst.light, Vector3(F.beam_radius, 0, 0), UP * lens.height, Vector3(0, 0, F.beam_radius), head, 0, 0, 0, 0);
}

struct PoleFrame {
	Vector3 foot, kerb;
	PoleArms arms;
	int seg = -1; // the street it stands on, and where along it
	double v = 0;
};

PoleFrame frame_of(const CityState &s, const StreetLook &L, int id) {
	const UtilityPole &p = s.street.poles.items[size_t(id)];
	const PoleStand st = pole_stand(s.street, s.roads, s.catalog.furniture, id);
	PoleFrame f;
	f.foot = Vector3(p.x, street_ground(s, L, p.x, p.z), p.z);
	f.seg = st.seg;
	f.v = st.v;
	f.arms = st.arms;
	f.kerb = flat(st.toward);
	return f;
}

// A pole the player built, with its arms, and the spans it starts.
void place_pole(const CityState &s, const StreetLook &L, const FurnitureLook &F, int id, StreetInstances &inst) {
	const UtilityPole &p = s.street.poles.items[size_t(id)];
	const PoleFrame f = frame_of(s, L, id);
	const bool rural = p.kind == PoleKind::RURAL;
	const ArmDir toward{ f.kerb.x, f.kerb.z };
	int mask = int(PolePart::MAST);
	if (p.lamp) mask |= int(PolePart::LAMP);
	if (id % CAN_PERIOD == CAN_PHASE) mask |= int(rural ? PolePart::CAN_RURAL : PolePart::CAN);
	push_part(inst.poles, PropKit::facing(f.kerb), f.foot, mask);
	for (const ArmDir &a : f.arms.arms) push_part(inst.poles, PropKit::facing(flat(street_arm(a, toward))), f.foot, int(rural ? PolePart::ARM_RURAL : PolePart::ARM));
	if (p.lamp) push_light(inst, F, f.foot, f.kerb, { F.lamp_reach, F.lamp_height });
	for (size_t i = 0; i < p.spans.size(); i++) {
		const UtilitySpan &sp = s.street.spans.items[size_t(p.spans[i])];
		if (sp.a != id) continue;
		const PoleFrame o = frame_of(s, L, sp.b);
		const std::vector<int> &theirs = s.street.poles.items[size_t(sp.b)].spans;
		const size_t at = size_t(std::find(theirs.begin(), theirs.end(), p.spans[i]) - theirs.begin());
		const ArmDir mine = street_arm(f.arms.by_span[i], toward), other = street_arm(o.arms.by_span[at], ArmDir{ o.kerb.x, o.kerb.z });
		push_span(inst, F.wire, f.foot, flat(mine), o.foot, flat(other), float(p.spans[i]) * PHASE_STEP, p.kind, s.street.poles.items[size_t(sp.b)].kind);
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
// powerLine.ts join), by the lower segment of the two. Both poles' arms reach over the same street, as the wires' seats want.
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
		push_span(inst, F.wire, a.origin, a.basis.get_column(2), b.origin, b.basis.get_column(2), float(sid) * PHASE_STEP, PoleKind::URBAN, PoleKind::URBAN);
	}
}

// A row pole: one instance, its arm across the street; a pole of the row on the other side is a lighting column.
void place_row_pole(const FurnitureLook &F, const FurnitureSpot &p, int sid, const PropPlace &at, StreetInstances &inst) {
	const uint32_t h = hash_combine(uint32_t(sid), uint32_t(p.station));
	const bool column = p.row != 0 || p.kind == FurnitureKind::COLUMN;
	int mask = column ? int(PolePart::COLUMN) : int(PolePart::MAST) | int(PolePart::ARM) | (p.lit ? int(PolePart::LAMP) : 0);
	if (!column && h % uint32_t(CAN_PERIOD) == uint32_t(CAN_PHASE)) mask |= int(PolePart::CAN);
	push_part(inst.poles, at.basis, at.origin, mask);
	if (p.lit) push_light(inst, F, at.origin, at.basis.get_column(2), { column ? F.column_reach : F.lamp_reach, column ? F.column_height : F.lamp_height });
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
			place_row_pole(F, p, sid, place, inst);
			if (p.kind != FurnitureKind::LAMP || p.row != 0) continue;
			row_side = p.side;
			if (wired && p.station == last_station + 1) {
				const Vector3 arm = place.basis.get_column(2);
				push_span(inst, F.wire, last.origin, last.basis.get_column(2), place.origin, arm, float(sid + p.station) * PHASE_STEP, PoleKind::URBAN, PoleKind::URBAN);
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

void place_pole_preview(const CityState &s, const StreetLook &L, const FurnitureLook &F, const std::vector<PreviewPole> &poles, StreetInstances &inst) {
	const float reach = s.catalog.furniture.items.stand_reach_m;
	std::vector<Vector3> foot(poles.size()), arm(poles.size());
	for (size_t i = 0; i < poles.size(); i++) {
		const Vector3 &at = poles[i].at;
		foot[i] = Vector3(at.x, street_ground(s, L, at.x, at.z), at.z);
		const Vector3 next = i + 1 < poles.size() ? poles[i + 1].at : at, prev = i > 0 ? poles[i - 1].at : at;
		const Vector3 along = (next - prev).normalized();
		const ArmDir square{ along.z, -along.x };
		const FootwayHit h = footway_at(s.roads, at.x, at.z, reach);
		const ArmDir kerb{ h.kx, h.kz };
		const ArmDir toward = pole_toward(h.ok ? &kerb : nullptr, square);
		arm[i] = flat(street_arm(square, toward));
		if (poles[i].standing) continue;
		const bool rural = poles[i].kind == PoleKind::RURAL;
		push_part(inst.poles, PropKit::facing(flat(toward)), foot[i], int(PolePart::MAST) | (poles[i].lamp ? int(PolePart::LAMP) : 0));
		push_part(inst.poles, PropKit::facing(arm[i]), foot[i], int(rural ? PolePart::ARM_RURAL : PolePart::ARM));
	}
	for (size_t i = 0; i + 1 < poles.size(); i++) push_span(inst, F.wire, foot[i], arm[i], foot[i + 1], arm[i + 1], 0, poles[i].kind, poles[i + 1].kind);
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

bool FurnitureLook::load(const DataValue &t, PackedStringArray &errors) {
	const int64_t before = errors.size();
	const DataValue w = t["wires"], l = t["lamp_light"];
	wire.sag = w["sag"].f(); wire.max_sag = w["max_sag_m"].f(); wire.min_span = w["min_span_m"].f(); wire.max_span = w["max_span_m"].f();
	wire.near_end = w["tube_end_m"].f(); wire.far_end = w["far_end_m"].f();
	beams = l["beams"].flag(); beam_radius = l["beam_radius_m"].f(); lamp_height = l["lamp_height_m"].f(); lamp_reach = l["lamp_reach_m"].f();
	column_height = l["column_height_m"].f(); column_reach = l["column_reach_m"].f();
	return errors.size() == before;
}

} // namespace city
