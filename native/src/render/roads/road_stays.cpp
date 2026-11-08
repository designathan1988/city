// A long bridge held from towers (not in the 4.0: its bridges are decks on piers): the street's whole length shared in spans, an H tower in the middle of each and a fan of cables down
// to both edges of the deck. A road comes in pieces, so everything is counted along the street: each piece draws the towers and the cables that fall on it.
#include "render/roads/road_mesh.h"

#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t CABLE_RGB = 0xd8dcdeU;
constexpr float HALF = 0.5f;
const Vector3 UP(0, 1, 0);

struct Span {
	double start = 0, every = 0;
	int spans = 0;
};

// Along the street, or none: a street of bridge too short for a tower has none.
bool span_of(const RoadEmit &e, int sid, Span &out) {
	const Tuning &tn = e.s.catalog.tune;
	if (structure_of(e.s.roads.segs[size_t(sid)].flags) != Structure::BRIDGE) return false;
	const StreetRun &run = e.s.roads.street_run(sid); // the street's one definition (RoadNetwork)
	const double length = run.total;
	if (length < tn.structure_span_from_m) return false;
	out.start = run.start;
	out.spans = std::max(1, int(std::round(length / tn.structure_span_every_m)));
	out.every = length / out.spans;
	return true;
}

void box(MeshBuilder &mb, const Vector3 &foot, const Vector3 &fwd, const Vector3 &left, float along, float across, float tall, const Color &col) {
	const Vector3 a = fwd * (along * HALF), b = left * (across * HALF), h = UP * tall;
	const Vector3 p[4] = { foot - a - b, foot + a - b, foot + a + b, foot - a + b }, out[4] = { -left, fwd, left, -fwd };
	for (int i = 0; i < 4; i++) structure_face(mb, p[i], p[(i + 1) % 4], p[(i + 1) % 4] + h, p[i] + h, col, out[i]);
	structure_face(mb, p[0] + h, p[1] + h, p[2] + h, p[3] + h, col, UP);
}

void tower(const RoadEmit &e, const SegFrame &f, float casing, const Color &concrete) {
	const StayShape &K = e.L.shapes.road_stays;
	const Tuning &tn = e.s.catalog.tune;
	const float top = f.c.y + float(tn.structure_tower_over_m), leg = float(tn.structure_tower_leg_m), out = casing + K.leg_out_m;
	for (const float side : { -1.f, 1.f }) {
		Vector3 foot = f.c + f.left * (out * side);
		foot.y = e.s.terrain.height_at(foot.x, foot.z) - K.foot_sink_m;
		box(e.mb, foot, f.fwd, f.left, leg, leg, top - foot.y, concrete);
	}
	for (const float y : { f.c.y + K.strut_over_m, top - K.strut_h_m }) box(e.mb, Vector3(f.c.x, y, f.c.z), f.fwd, f.left, leg, out * 2, K.strut_h_m, concrete);
}

// A cable as two thin strips across each other: it reads from any side and takes no volume.
void cable(const StayShape &K, MeshBuilder &mb, const Vector3 &a, const Vector3 &b, const Vector3 &left, const Color &col) {
	const Vector3 up = UP * (K.strand_m * HALF), side = left * (K.strand_m * HALF);
	const Vector3 along = (b - a).normalized(), out = along.cross(left).normalized();
	structure_face(mb, a - side, b - side, b + side, a + side, col, out);
	structure_face(mb, a + side, b + side, b - side, a - side, col, -out);
	structure_face(mb, a - up, b - up, b + up, a + up, col, left);
	structure_face(mb, a + up, b + up, b - up, a - up, col, -left);
}
} // namespace

bool stayed(const RoadEmit &e, int sid) {
	Span span;
	return span_of(e, sid, span);
}

void emit_stays(const RoadEmit &e, int sid, double v0, double v1, float casing) {
	const StayShape &K = e.L.shapes.road_stays;
	Span span;
	if (!span_of(e, sid, span)) return;
	const Tuning &tn = e.s.catalog.tune;
	const Color concrete = structure_concrete(), steel = rgb8_to_color(CABLE_RGB);
	for (int k = 0; k < span.spans; k++) {
		const double at = (double(k) + HALF) * span.every - span.start;
		if (at >= v0 && at < v1) tower(e, seg_frame(e.s, sid, at), casing, concrete);
	}
	const double step = tn.structure_cable_step_m, reach = span.every * HALF;
	for (double s = std::ceil((span.start + v0) / step) * step - span.start; s < v1; s += step) {
		const double along = span.start + s, nearest = (std::floor(along / span.every) + HALF) * span.every, off = nearest - along;
		if (std::fabs(off) < tn.structure_cable_first_m) continue;
		const SegFrame f = seg_frame(e.s, sid, s);
		const float rise = float(tn.structure_tower_over_m) * (K.fan_low + K.fan_high * float(std::fabs(off) / reach));
		for (const float side : { -1.f, 1.f }) {
			const Vector3 edge = f.left * ((casing - K.anchor_in_m) * side), mast = f.left * ((casing + K.leg_out_m) * side);
			cable(K, e.mb, f.c + edge, f.c + f.fwd * float(off) + mast + UP * rise, f.left, steel);
		}
	}
}

} // namespace city
