// The street furniture as it is drawn, from the simulation's places (sim/roads/street_furniture.h, sim/street), see
// street_props.h. A pole is one instance of the pole mesh with the parts it shows in INSTANCE_CUSTOM.a (prop_kit.h
// PolePart); a wire is one instance whose transform carries its chord and INSTANCE_CUSTOM its sag and phase (props.gdshader).
#include "render/props/street_props.h"

#include "render/owned_segments.h"

#include "core/color.h"
#include "core/random.h"
#include "render/roads/road_mesh.h"
#include "sim/roads/street_lines.h"
#include "sim/state/city_cut.h"
#include "sim/street/footway.h"
#include "sim/street/pole_arms.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>

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
constexpr float KIND_STRIDE = 8.f;
constexpr int KIND_B_WEIGHT = 2;
const Vector3 UP(0, 1, 0);

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
	const float kinds = KIND_STRIDE * float(int(kind_a) + KIND_B_WEIGHT * int(kind_b));
	const float sag = std::min(w.max_sag, span * w.sag);
	inst.wire_sag = std::max(inst.wire_sag, sag);
	mm_push_plain(inst.wires, d, UP, across, a, sag, std::fmod(phase, TAU_F) + kinds, heading(arm_a), heading(arm_b));
}

Vector3 flat(ArmDir a) { return Vector3(a.x, 0, a.z); }

// A lamp's light: its lens for the lights of render/props/lamp_lights.h, and the lit air under it. Its pool on the ground is
// render/props/lamp_pools.h's, from the same lens.
void push_light(StreetInstances &inst, const FurnitureLook &F, const Vector3 &foot, const Vector3 &toward, const ArmReach &lens) {
	const Vector3 lens_point = lens_at(foot, toward, lens);
	inst.lamps.push_back(lens_point);
	if (F.beams) // lit air: rain and storm, and there is no weather yet
		mm_push_plain(inst.light, Vector3(F.beam_radius, 0, 0), UP * lens.height, Vector3(0, 0, F.beam_radius), lens_point - UP * lens.height, 0, 0, 0, 0);
}

struct PoleFrame {
	Vector3 foot, kerb;
	PoleArms arms;
	int seg = -1; // the street it stands on, and where along it
	double v = 0;
};

PoleFrame frame_of(const CityView &s,const StreetLook &L, int id) {
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
	f.kerb = flat(pole_toward(h.ok ? &idle : nullptr, f.arms.arms[0]));
	return f;
}

// A pole the player built, with its arms, and the spans it starts.
void place_pole(const CityView &s,const StreetLook &L, const FurnitureLook &F, int id, StreetInstances &inst) {
	const UtilityPole &p = s.street.poles.items[size_t(id)];
	const PoleFrame f = frame_of(s, L, id);
	const bool rural = p.kind == PoleKind::RURAL;
	const ArmDir toward{ f.kerb.x, f.kerb.z };
	int mask = int(PolePart::MAST);
	if (p.lamp) mask |= int(PolePart::LAMP);
	if (id % CAN_PERIOD == CAN_PHASE) mask |= int(rural ? PolePart::CAN_RURAL : PolePart::CAN);
	push_part(inst.poles, PropKit::facing(f.kerb), f.foot, mask);
	for (const ArmDir &a : f.arms.arms) push_part(inst.poles, PropKit::facing(flat(street_arm(a, toward))), f.foot, int(rural ? PolePart::ARM_RURAL : PolePart::ARM));
	if (p.lamp) push_light(inst, F, f.foot, f.kerb, arm_reach(F, false));
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

// The spans node `n`'s lines string beyond any one segment's row (sim/roads/street_lines.h node_line_spans: a joint's, a corner
// pole's, across the junction), drawn by the chunk the node stands in; each pole found where its segment's placement lists it.
void place_node_lines(const CityView &s, const StreetLook &L, const FurnitureLook &F, int n, StreetInstances &inst) {
	std::unordered_map<int, std::vector<FurnitureSpot>> kept;
	const auto spots_of = [&](int seg) -> const std::vector<FurnitureSpot> & {
		auto it = kept.find(seg);
		if (it == kept.end()) {
			it = kept.emplace(seg, std::vector<FurnitureSpot>()).first;
			furniture_of(s, seg, furniture_bit(FurnitureKind::LAMP), it->second);
		}
		return it->second;
	};
	const FirstStation first = [&](int seg, int end) {
		int best = -1;
		for (const FurnitureSpot &p : spots_of(seg))
			if (p.row == 0 && p.corner < 0 && p.station >= 0 && (best < 0 || (end == 0 ? p.station < best : p.station > best))) best = p.station;
		return best;
	};
	const auto place_of = [&](const LinePole &lp, PropPlace &out) {
		for (const FurnitureSpot &p : spots_of(lp.seg)) {
			if (p.row != 0 || p.kind != FurnitureKind::LAMP) continue;
			if (lp.corner >= 0 ? (p.corner == lp.corner && p.node == lp.node) : (p.corner < 0 && p.station == lp.station)) {
				out.origin = spot_world(s, L, p, &out.basis);
				return true;
			}
		}
		return false;
	};
	std::vector<LineSpan> spans;
	node_line_spans(s.roads, n, first, spans);
	for (size_t i = 0; i < spans.size(); i++) {
		PropPlace a, b;
		if (!place_of(spans[i].a, a) || !place_of(spans[i].b, b)) continue; // a pole cut, knocked out or buried takes its wires
		push_span(inst, F.wire, a.origin, a.basis.get_column(2), b.origin, b.basis.get_column(2), float(n + int(i)) * PHASE_STEP, PoleKind::URBAN,
				PoleKind::URBAN);
	}
}

// A row pole: one instance, its arm across the street; a pole of the row on the other side is a lighting column.
void place_row_pole(const FurnitureLook &F, const FurnitureSpot &p, int sid, const PropPlace &at, StreetInstances &inst) {
	const uint32_t h = hash_combine(uint32_t(sid), uint32_t(p.station));
	const bool column = p.row != 0 || p.kind == FurnitureKind::COLUMN;
	int mask = column ? int(PolePart::COLUMN) : int(PolePart::MAST) | int(PolePart::ARM) | (p.lit ? int(PolePart::LAMP) : 0);
	if (!column && h % uint32_t(CAN_PERIOD) == uint32_t(CAN_PHASE)) mask |= int(PolePart::CAN);
	push_part(inst.poles, at.basis, at.origin, mask);
	if (p.lit) push_light(inst, F, at.origin, at.basis.get_column(2), arm_reach(F, column));
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

// A segment's pieces; the main row's poles strung as the layout strung them (FurnitureSpot::strung: the pole each one's wires come from).
void place_segment(const CityView &s,const StreetLook &L, const FurnitureLook &F, int sid, std::vector<PropPlace> &out, StreetInstances &inst) {
	std::vector<FurnitureSpot> spots;
	furniture_of(s, sid, PROP_KINDS, spots);
	std::vector<std::pair<int, PropPlace>> row; // the main row's poles standing, by station
	std::vector<std::pair<int, PropPlace>> strung; // each pole's place and the station its wires come from
	for (const FurnitureSpot &p : spots) {
		PropPlace place;
		place.origin = spot_world(s, L, p, &place.basis);
		if (p.kind == FurnitureKind::LAMP || p.kind == FurnitureKind::COLUMN) {
			place_row_pole(F, p, sid, place, inst);
			if (p.kind != FurnitureKind::LAMP || p.row != 0 || p.corner >= 0) continue; // a corner pole's spans are its node's
			row.emplace_back(p.station, place);
			if (p.strung >= 0) strung.emplace_back(p.strung, place);
			continue;
		}
		place.kind = prop_kind(p.kind);
		out.push_back(place);
	}
	for (const auto &[from, place] : strung)
		for (const auto &[station, before] : row) {
			if (station != from) continue;
			push_span(inst, F.wire, before.origin, before.basis.get_column(2), place.origin, place.basis.get_column(2), float(sid + station) * PHASE_STEP,
					PoleKind::URBAN, PoleKind::URBAN);
			break;
		}
}
} // namespace

// A wire hangs in a parabola `sag_m` deep at its middle: `far_chords` equal chords leave it sag / chords^2 off at the most (the
// parabola over one chord's width), and a tube of `far_sides` sides the radius times 1 - cos(pi / sides) off its round.
float WireLook::chord_miss(float sag_m) const {
	return sag_m / float(far_chords * far_chords) + radius * (1.f - std::cos(PI_F / float(far_sides)));
}

ArmReach arm_reach(const FurnitureLook &F, bool column) { return column ? ArmReach{ F.column_reach, F.column_height } : ArmReach{ F.lamp_reach, F.lamp_height }; }

Vector3 lens_at(const Vector3 &foot, const Vector3 &toward, const ArmReach &lens) { return foot + toward * lens.reach + UP * lens.height; }

Vector3 spot_world(const CityView &s, const StreetLook &L, const FurnitureSpot &p, Basis *basis) {
	const SegFrame f = seg_frame(s, p.seg, p.v);
	if (basis) *basis = PropKit::facing(f.left * p.face_left + f.fwd * p.face_fwd);
	return f.c + f.left * p.u + UP * L.h_sidewalk;
}

PoleFoot pole_foot(const CityView &s, const StreetLook &L, int id) {
	const PoleFrame f = frame_of(s, L, id);
	return PoleFoot{ f.foot, f.kerb, f.seg };
}

float street_ground(const CityView &s,const StreetLook &L, double x, double z) {
	const FootwayHit h = footway_at(s.roads, x, z, s.catalog.furniture.items.stand_reach_m);
	if (!h.ok) return s.terrain.height_at(float(x), float(z));
	double px, pz, y;
	footway_point(s.roads, h.seg, h.v, h.u, px, pz, y);
	return float(y) + L.h_sidewalk;
}

void place_street_props(const CityView &s,const StreetLook &L, const FurnitureLook &F, int64_t key, std::vector<PropPlace> &out, StreetInstances &inst) {
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
		if (!nd.alive || chunk_of(float(nd.x), float(nd.z)) != key) return;
		place_node_lines(s, L, F, n, inst);
		if (!node_signalled(s, n)) return;
		FurnitureSpot post;
		for (int sid : nd.segs) {
			if (!signal_post(s.roads, n, sid, post)) continue;
			SignalHead h;
			h.ds = traffic::in_ds(s.roads, sid, n);
			h.origin = spot_world(s, L, post, &h.basis);
			inst.heads.push_back(h);
		}
	});
}

void place_pole_preview(const CityView &s,const StreetLook &L, const FurnitureLook &F, const std::vector<PreviewPole> &poles, StreetInstances &inst) {
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

void place_street_trees(const CityView &s, const StreetLook &L, int64_t key, std::vector<TreeSpot> &out) {
	std::vector<FurnitureSpot> spots;
	each_owned_segment(s, key, [&](int sid) {
		spots.clear();
		furniture_of(s, sid, furniture_bit(FurnitureKind::TREE) | furniture_bit(FurnitureKind::SHRUB), spots);
		for (const FurnitureSpot &p : spots) {
			uint32_t h = hash_combine(uint32_t(sid) * TREE_SALT, uint32_t(p.v * p.side + p.v));
			const float scale = (CANOPY_SCALE_MIN + TREE_SCALE_SPAN * hash_float(h)) * (p.kind == FurnitureKind::SHRUB ? SHRUB_SCALE : 1.f);
			out.push_back({ spot_world(s, L, p, nullptr), scale, TAU_F * hash_float(h + 1), int(hash_u32(h) % TREE_SPECIES) });
		}
	});
}

bool FurnitureLook::load(const Dictionary &t, PackedStringArray &errors) {
	if (!t.has("wires") || !t.has("lamp_light")) {
		errors.push_back("streets.json: falta wires ou lamp_light");
		return false;
	}
	Dictionary w = t["wires"], l = t["lamp_light"];
	wire.sag = w["sag"]; wire.max_sag = w["max_sag_m"]; wire.min_span = w["min_span_m"]; wire.max_span = w["max_span_m"];
	wire.radius = w["radius_m"]; wire.far_chords = int(w["far_chords"]); wire.far_sides = int(w["far_sides"]);
	if (wire.far_chords < 1 || wire.far_sides < 3) {
		errors.push_back("streets.json wires: far_chords under 1 or far_sides under 3");
		return false;
	}
	beams = l["beams"]; beam_radius = l["beam_radius_m"]; lamp_height = l["lamp_height_m"]; lamp_reach = l["lamp_reach_m"];
	column_height = l["column_height_m"]; column_reach = l["column_reach_m"]; pool_slab = l["pool_slab_m"];
	return true;
}

} // namespace city
