// A node's paving (road_node.h draw_plate), the one owner of its tone: at a point, every arm's carriageway colour weighed by that
// arm's width over the distance to its mouth, to the look's power (streets.json plate_tone_power), mixed in linear light. On an arm's
// mouth the paving is that arm's own asphalt across the whole mouth, so no step shows where a lane ends; off it the wider and nearer
// arm leads, so an avenue's asphalt runs through a junction and a side street's gives way in its own throat. The tone is carried by
// the vertices: the paving is drawn in rings from the node out to its rim, enough of them near for the mesh to follow it. Each vertex
// also knows how far its kerb is, so the grime a lane wears by the kerb goes on round the corner.
#include "render/roads/road_node.h"

#include <cmath>

using namespace godot;

namespace city {

namespace {
const Vector3 UP(0, 1, 0);
constexpr int NEAR_RINGS = 3, FAR_RINGS = 1; // the rings a slice is drawn in: near, the tone bends between the node and the rim
constexpr float ON_MOUTH_M = 1e-3f; // nearer a mouth than this a point is on it
constexpr float NO_SLICE_M = 1e-4f; // a stretch of rim shorter than this has no slice

// One arm's mouth in plan and its carriageway's colour, as the look gives it and in linear light.
struct Mouth {
	V2 a, b;
	float width = 0;
	Color colour, light;
};

V2 plan(const Vector3 &p) { return V2(p.x, p.z); }

// A kerb's line carried on: `p`'s distance in plan to the line through `at` whose way is `along` (unit, flat).
float kerb_off(const Vector3 &p, const Vector3 &at, const Vector3 &along) { return std::fabs((plan(p) - plan(at)).cross(plan(along))); }

Color tone_at(const std::vector<Mouth> &mouths, const Vector3 &p, float power) {
	Color sum(0, 0, 0);
	float total = 0;
	for (const Mouth &m : mouths) {
		const float off = dist_point_segment(plan(p), m.a, m.b);
		if (off < ON_MOUTH_M) return m.colour;
		const float weight = std::pow(m.width / off, power);
		sum += m.light * weight;
		total += weight;
	}
	return (sum / total).linear_to_srgb();
}
} // namespace

PlateSlice kerb_slice(const Vector3 &node, const Vector3 &p, const Vector3 &q) {
	const Vector3 run(q.x - p.x, 0, q.z - p.z);
	return { p, q, run.length() > 0 ? kerb_off(node, p, run.normalized()) : (plan(node) - plan(p)).len(), 0, 0 };
}

PlateSlice mouth_slice(const Vector3 &node, const Vector3 &p, const Vector3 &q, const Vector3 &kerb_at, const Vector3 &along) {
	return { p, q, kerb_off(node, kerb_at, along), kerb_off(p, kerb_at, along), kerb_off(q, kerb_at, along) };
}

void draw_plate(const RoadEmit &e, const Vector3 &node, const std::vector<ArmEnd> &ends, const std::vector<PlateSlice> &rim) {
	std::vector<Mouth> mouths;
	for (const ArmEnd &en : ends) {
		const Color colour = e.L.carriage(*en.prof);
		mouths.push_back({ plan(mouth_point(en, -en.arm.asph_right)), plan(mouth_point(en, en.arm.asph_left)), float(en.arm.asph_left + en.arm.asph_right),
				colour, colour.srgb_to_linear() });
	}
	const auto add = [&](const Vector3 &p, float kerb) {
		return e.mb.add(p, UP, road_color(tone_at(mouths, p, e.L.plate_tone_power), RoadMat::PLATE), Vector2(p.x, p.z), Vector2(kerb, 0));
	};
	const int rings = e.far ? FAR_RINGS : NEAR_RINGS;
	for (const PlateSlice &s : rim) {
		if ((plan(s.p) - plan(s.q)).len() < NO_SLICE_M) continue;
		int in_p = add(node, s.kerb_node), in_q = in_p;
		for (int j = 1; j <= rings; j++) {
			const float t = float(j) / float(rings);
			// the rim's own points as they are, so the paving meets the lanes and the kerb on their very vertices
			const int out_p = add(j == rings ? s.p : node.lerp(s.p, t), s.kerb_node + (s.kerb_p - s.kerb_node) * t);
			const int out_q = add(j == rings ? s.q : node.lerp(s.q, t), s.kerb_node + (s.kerb_q - s.kerb_node) * t);
			e.mb.tri(in_p, out_p, out_q, UP);
			if (j > 1) e.mb.tri(in_p, out_q, in_q, UP);
			in_p = out_p;
			in_q = out_q;
		}
	}
}

} // namespace city
