// The cross-arms of a pole, see pole_arms.h. Left of a direction (x, z) is (z, -x), as in sim/roads/geometry.h.
#include "sim/street/pole_arms.h"

#include "core/units.h"
#include "sim/catalog/furniture_types.h"
#include "sim/roads/road_network.h"
#include "sim/street/footway.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float UNIT_EPS = 1e-9f;

ArmDir unit(float x, float z) {
	const float l = std::hypot(x, z);
	return l > UNIT_EPS ? ArmDir{ x / l, z / l } : ArmDir{ 0, 1 };
}
ArmDir square_to(const ArmDir &d) { return { d.z, -d.x }; }

struct Line {
	int span = -1;
	size_t at = 0; // its place in the pole's list
	ArmDir d; // leaving the pole
};

// The arm turned to lie left of the span's own a -> b.
ArmDir oriented(const StreetWorks &w, const ArmDir &arm, int span) {
	const UtilitySpan &s = w.spans.items[size_t(span)];
	const UtilityPole &a = w.poles.items[size_t(s.a)], &b = w.poles.items[size_t(s.b)];
	const float ax = b.x - a.x, az = b.z - a.z;
	return az * arm.x - ax * arm.z >= 0 ? arm : ArmDir{ -arm.x, -arm.z };
}
} // namespace

PoleArms pole_arms(const StreetWorks &w, const PoleDef &def, int pole, ArmDir idle) {
	PoleArms out;
	const UtilityPole &p = w.poles.items[size_t(pole)];
	std::vector<Line> lines;
	for (size_t i = 0; i < p.spans.size(); i++) {
		const UtilitySpan &s = w.spans.items[size_t(p.spans[i])];
		const UtilityPole &o = w.poles.items[size_t(s.a == pole ? s.b : s.a)];
		lines.push_back({ p.spans[i], i, unit(o.x - p.x, o.z - p.z) });
	}
	out.by_span.resize(p.spans.size());
	if (lines.empty()) {
		out.arms.push_back(unit(idle.x, idle.z));
		return out;
	}
	if (lines.size() == 1) {
		const ArmDir arm = square_to(lines[0].d);
		out.arms.push_back(arm);
		out.by_span[0] = oriented(w, arm, lines[0].span);
		return out;
	}
	if (lines.size() == 2) {
		const Line &a = lines[0], &b = lines[1];
		const float cos_turn = std::clamp(-(a.d.x * b.d.x + a.d.z * b.d.z), -1.f, 1.f); // 1 on a straight run
		if (std::acos(cos_turn) < def.corner_deflection_deg * DEG_TO_RAD) {
			const ArmDir arm = square_to(unit(a.d.x - b.d.x, a.d.z - b.d.z));
			out.arms.push_back(arm);
			out.by_span[0] = oriented(w, arm, a.span);
			out.by_span[1] = oriented(w, arm, b.span);
			return out;
		}
	}
	for (const Line &l : lines) {
		const ArmDir arm = square_to(l.d);
		out.by_span[l.at] = oriented(w, arm, l.span);
		const bool shared = std::any_of(out.arms.begin(), out.arms.end(), [&](const ArmDir &o) { return std::fabs(o.x * arm.z - o.z * arm.x) < def.same_arm_sin; });
		if (!shared) out.arms.push_back(arm);
	}
	return out;
}

ArmDir pole_toward(const ArmDir *kerb, ArmDir arm) {
	if (kerb) return *kerb;
	const bool back = std::abs(arm.x) >= std::abs(arm.z) ? arm.x < 0 : arm.z < 0;
	return back ? ArmDir{ -arm.x, -arm.z } : arm;
}

ArmDir street_arm(ArmDir arm, ArmDir toward) {
	return arm.x * toward.x + arm.z * toward.z >= 0 ? arm : ArmDir{ -arm.x, -arm.z };
}

PoleStand pole_stand(const StreetWorks &w, const RoadNetwork &roads, const StreetFurnitureDef &def, int id) {
	const UtilityPole &p = w.poles.items[size_t(id)];
	const FootwayHit h = footway_at(roads, p.x, p.z, def.items.stand_reach_m);
	PoleStand out;
	ArmDir idle;
	if (h.ok) {
		idle = { h.kx, h.kz };
		out.seg = h.seg;
		out.v = h.v;
	}
	out.arms = pole_arms(w, def.poles, id, idle);
	out.toward = pole_toward(h.ok ? &idle : nullptr, out.arms.arms[0]);
	return out;
}

} // namespace city
