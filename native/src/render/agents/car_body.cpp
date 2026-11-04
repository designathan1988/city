// Lofted passenger-car body (car_shape.h): stations along the length (uniform, plus one at each edge of a detail: the
// screens, the side glass, the door seams and the B pillar, steps round the wheel arches and the rounded ends), each a
// half ring; every band painted, glazed, the black frame of the glass (pillars, B pillar, door seams) or dark underneath
// and, in the overhangs, the bumpers' valance. Normals are smooth across the bands of one region (from the tangents along
// the ring and along the length) and hard between regions. Fine (the cabin level) or coarse (fewer stations, no seams,
// no mirrors nor handles). The ends, parts and cabin: car_parts.cpp.
#include "render/agents/car_shape.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <array>
#include <cmath>

using namespace godot;

namespace city {

namespace {

constexpr float MIN_GLASS_M = 0.12f;
constexpr float DEGENERATE_AREA = 1e-10f; // a band with no area: not drawn

enum class Region : int { UNDER, PAINT, GLASS, FRAME };

struct Loft {
	const CarStyle *s = nullptr;
	float doors[3] = {}, seam = 0, pillar = 0; // shares of L
	bool fine = false;
};

bool near_edge(float xm, float edge, float half) { return std::fabs(xm - edge) < half; }

// The region of band k (ring[k] -> ring[k+1]) over a gap whose middle is xm and whose side glass is glass_h high.
Region region_of(const Loft &lf, int k, float xm, float glass_h) {
	const CarStyle &s = *lf.s;
	if (k < RING_SILL || (k == RING_SILL && car_overhang(s, xm))) return Region::UNDER;
	if (k < RING_BELT) {
		for (float e : lf.doors)
			if (lf.fine && near_edge(xm, e, lf.seam * 0.5f)) return Region::FRAME;
		return Region::PAINT;
	}
	const bool screen = car_within(xm, s.screen) || (s.rear_glass && car_within(xm, s.rear));
	if (k > RING_BELT) return screen ? Region::GLASS : Region::PAINT;
	if (glass_h <= MIN_GLASS_M) return Region::PAINT;
	if (screen) return Region::GLASS;
	if (car_within(xm, s.side)) return near_edge(xm, lf.doors[1], lf.pillar * 0.5f) ? Region::FRAME : Region::GLASS;
	const float front = std::max(s.screen[0], s.screen[1]), back = s.rear_glass ? std::min(s.rear[0], s.rear[1]) : std::min(s.side[0], s.side[1]);
	return xm < front && xm > back ? Region::FRAME : Region::PAINT; // the pillars between the glass
}

AgentTag tag_of(Region r) { return r == Region::PAINT ? AgentTag::PAINT : (r == Region::GLASS ? AgentTag::GLASS : AgentTag::FIXED); }

Color colour_of(Region r, const AgentTrim &trim) {
	switch (r) {
		case Region::UNDER: return trim.under;
		case Region::GLASS: return trim.glass;
		case Region::FRAME: return trim.blackout;
		default: return AGENT_WHITE;
	}
}

std::vector<float> stations(const Loft &lf, const Dictionary &body) {
	const CarStyle &s = *lf.s;
	const int k = lf.fine ? 0 : 1, uniform = int(Array(body["stations"])[k]), arch = int(Array(body["arch_steps"])[k]);
	const int ends = int(Array(body["end_steps"])[k]);
	std::vector<float> xs;
	for (int i = 0; i <= uniform; i++) xs.push_back(-0.5f + float(i) / uniform);
	for (float f : { s.screen[0], s.screen[1], s.side[0], s.side[1] }) xs.push_back(f);
	for (float f : s.rear)
		if (s.rear_glass) xs.push_back(f);
	if (lf.fine) {
		for (float e : lf.doors) xs.insert(xs.end(), { e - lf.seam * 0.5f, e + lf.seam * 0.5f });
		xs.insert(xs.end(), { lf.doors[1] - lf.pillar * 0.5f, lf.doors[1] + lf.pillar * 0.5f });
	}
	const float ar = (s.r + CAR_ARCH_PAD_M) / s.L;
	for (int side : { -1, 1 })
		for (int j = -arch; j <= arch; j++) xs.push_back(side * s.axle + ar * j / arch);
	for (int j = 1; j <= ends; j++) { // the plan's rounding at each end
		xs.push_back(0.5f - s.nose_r / s.L * j / (ends + 1));
		xs.push_back(-0.5f + s.tail_r / s.L * j / (ends + 1));
	}
	std::sort(xs.begin(), xs.end());
	const float eps = float(body["station_eps_m"]) / s.L;
	std::vector<float> out;
	for (float x : xs)
		if (x >= -0.5f && x <= 0.5f && (out.empty() || x - out.back() > eps)) out.push_back(x);
	return out;
}

// The bands of the half rings, smooth inside a region: a corner's normal is the cross of the tangent along the ring (one
// point each way, inside the band's region) and the tangent along the length (one station each way).
void loft(MeshBuilder &mb, const Loft &lf, const std::vector<float> &xs, const AgentTrim &trim) {
	const CarStyle &s = *lf.s;
	const size_t n = xs.size();
	std::vector<std::array<Vector2, RING>> rings(n);
	for (size_t i = 0; i < n; i++) car_ring(s, xs[i], rings[i].data());
	auto at = [&](size_t i, int k) { return Vector3(rings[i][size_t(k)].x, rings[i][size_t(k)].y, xs[i] * s.L); };
	for (size_t i = 0; i + 1 < n; i++) {
		const float xm = (xs[i] + xs[i + 1]) * 0.5f;
		const float gh = std::min(rings[i][RING_PANE_TOP].y - rings[i][RING_BELT].y, rings[i + 1][RING_PANE_TOP].y - rings[i + 1][RING_BELT].y);
		Region reg[RING - 1];
		for (int k = 0; k + 1 < RING; k++) reg[k] = region_of(lf, k, xm, gh);
		for (int k = 0; k + 1 < RING; k++) {
			const Vector3 face = (at(i, k + 1) - at(i, k) + at(i + 1, k + 1) - at(i + 1, k)).cross(at(i + 1, k) - at(i, k) + at(i + 1, k + 1) - at(i, k + 1));
			if (face.length_squared() < DEGENERATE_AREA) continue;
			const int prev = k > 0 && reg[k - 1] == reg[k] ? k - 1 : k, next = k + 2 < RING && reg[k + 1] == reg[k] ? k + 2 : k + 1;
			Vector3 pos[4], nrm[4];
			const size_t ii[4] = { i, i + 1, i + 1, i };
			const int kk[4] = { k, k, k + 1, k + 1 };
			for (int c = 0; c < 4; c++) {
				const size_t a = ii[c] > 0 ? ii[c] - 1 : ii[c], b = ii[c] + 1 < n ? ii[c] + 1 : ii[c];
				const Vector3 along_ring = kk[c] == k ? at(ii[c], k + 1) - at(ii[c], prev) : at(ii[c], next) - at(ii[c], k);
				const Vector3 nn = along_ring.cross(at(b, kk[c]) - at(a, kk[c]));
				pos[c] = at(ii[c], kk[c]);
				nrm[c] = nn.length_squared() > DEGENERATE_AREA ? nn.normalized() : face.normalized();
			}
			const Color col = colour_of(reg[k], trim);
			const Vector2 t2 = agent_tag(tag_of(reg[k]));
			for (int side : { -1, 1 }) {
				const Vector3 mirror(float(side), 1, 1);
				int v[4];
				for (int c = 0; c < 4; c++) v[c] = mb.add(pos[c] * mirror, nrm[c] * mirror, col, Vector2(), t2);
				mb.tri(v[0], v[1], v[2], face * mirror);
				mb.tri(v[0], v[2], v[3], face * mirror);
			}
		}
	}
}

} // namespace

int build_car(MeshBuilder &mb, const Dictionary &car, const Dictionary &body, const Dictionary &inside, const CabinLook *look, const AgentTrim &trim) {
	const CarStyle s = read_car_style(car, body);
	const int passengers = look ? car_cabin(mb, s, inside, *look) : 0;
	Loft lf;
	lf.s = &s;
	lf.fine = look != nullptr; // the cabin level: every detail
	car_door_edges(s, lf.doors);
	lf.seam = float(body["door_seam_m"]) / s.L;
	lf.pillar = float(body["b_pillar_m"]) / s.L;
	const std::vector<float> xs = stations(lf, body);
	loft(mb, lf, xs, trim);
	const float bumper = float(body["bumper_top_m"]);
	car_cap(mb, s, xs.back(), 1.f, bumper, trim);
	car_cap(mb, s, xs.front(), -1.f, bumper, trim);
	car_details(mb, s, body, lf.fine, trim);
	const float inset = float(body["wheel_inset_m"]), ww = float(body["wheel_w"]);
	const int sides = int(body["wheel_sides"]);
	for (int k : { -1, 1 })
		for (int side : { -1, 1 })
			agent_wheel(mb, Vector3(side * (s.W * 0.5f - inset), s.r, k * s.axle * s.L), s.r, ww, sides, side, trim);
	return passengers;
}

} // namespace city
