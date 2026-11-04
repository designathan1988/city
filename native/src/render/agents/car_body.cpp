// Lofted passenger-car body (car_shape.h): stations along the length (uniform, plus one at each edge of a detail: the
// screens and their frames, the side glass, the door seams and the B pillar, steps round the wheel arches and the rounded
// ends), each a half ring; every band painted, glazed, the black of the frames and pillars or dark underneath and, in the
// overhangs, the bumpers' valance. Glass is only what lies inside its frame: the windscreen and the rear screen on the top
// of the section, the side glass on its side, pillars between them. Normals are smooth across the bands of one region and
// hard between regions. Fine (the cabin level) or coarse (fewer stations and rows, no seams, no frames, no mirrors nor
// handles). The ends, parts and cabin: car_parts.cpp.
#include "render/agents/car_shape.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <array>
#include <cmath>

using namespace godot;

namespace city {

namespace {

constexpr float DEGENERATE_AREA = 1e-10f; // a band with no area: not drawn
constexpr float BED_LIP_M = 0.04f; // a pick-up's bed cover stops this short of the cab
constexpr int FINE_ROWS[RING] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
constexpr int COARSE_ROWS[] = { 0, 1, 3, 4, RING_BELT, RING_PANE_TOP, 12, RING_ROOF, RING - 1 };

enum class Region : int { UNDER, PAINT, ROOF, GLASS, FRAME };

struct Loft {
	const CarStyle *s = nullptr;
	float seam = 0, pillar = 0, frame = 0; // shares of L
	bool fine = false;
};

bool near_edge(float xm, float edge, float half) { return std::fabs(xm - edge) < half; }

bool near_door(const Loft &lf, float xm) {
	for (float e : lf.s->doors)
		if (near_edge(xm, e, lf.seam * 0.5f)) return true;
	return false;
}

bool near_b_pillar(const Loft &lf, float xm) {
	const std::vector<float> &d = lf.s->doors;
	for (size_t i = 1; i + 1 < d.size(); i++)
		if (near_edge(xm, d[i], lf.pillar * 0.5f)) return true;
	return false;
}

bool on_screen(const CarStyle &s, float xm) { return xm < s.screen[0] && xm > s.screen[1]; }
bool on_rear(const CarStyle &s, float xm) { return s.rear_glass && xm < s.rear[0] && xm > s.rear[1]; }
bool on_roof(const CarStyle &s, float xm) { return xm <= s.screen[1] && (!s.rear_glass || xm >= s.rear[0]) && xm > s.bed_from; }

// From the belt to the top of the side glass: the glass in its frame, the B pillar, the A pillar's foot, paint elsewhere.
Region side_region(const Loft &lf, int k, float xm) {
	const CarStyle &s = *lf.s;
	if (xm < s.bed_from) return Region::PAINT;
	if (xm < s.glass_front && xm > s.glass_back) {
		if (near_b_pillar(lf, xm)) return Region::FRAME;
		const bool framed = k == RING_BELT || k == RING_FRAME_TOP || xm > s.glass_front - lf.frame || xm < s.glass_back + lf.frame;
		return lf.fine && framed ? Region::FRAME : Region::GLASS;
	}
	return xm >= s.glass_front && xm < s.screen[0] + s.pillar_foot / s.L ? Region::FRAME : Region::PAINT;
}

// The top of the section: the screens in their frames, the roof, the bonnet, the boot, the bed's cover.
Region top_region(const Loft &lf, int k, float xm) {
	const CarStyle &s = *lf.s;
	const bool edge_row = k == RING_ROOF;
	if (on_screen(s, xm)) return lf.fine && (xm > s.screen[0] - lf.frame || xm < s.screen[1] + lf.frame || edge_row) ? Region::FRAME : Region::GLASS;
	if (on_rear(s, xm)) return lf.fine && (xm > s.rear[0] - lf.frame || xm < s.rear[1] + lf.frame || edge_row) ? Region::FRAME : Region::GLASS;
	if (xm < s.bed_from - BED_LIP_M / s.L) return Region::FRAME;
	return on_roof(s, xm) ? Region::ROOF : Region::PAINT;
}

// The region of band k (row k -> the next row) over a gap whose middle is xm.
Region region_of(const Loft &lf, int k, float xm) {
	const CarStyle &s = *lf.s;
	if (k < RING_SILL || (k == RING_SILL && car_overhang(s, xm))) return Region::UNDER;
	if (k < RING_BELT) return lf.fine && k > RING_SILL && near_door(lf, xm) ? Region::FRAME : Region::PAINT;
	if (k < RING_PANE_TOP) return side_region(lf, k, xm);
	if (k < RING_ROOF) { // the roof's rounded edge: pillars over the screens
		if (on_screen(s, xm) || on_rear(s, xm)) return Region::FRAME;
		return on_roof(s, xm) ? Region::ROOF : Region::PAINT;
	}
	return top_region(lf, k, xm);
}

AgentTag tag_of(Region r) {
	switch (r) {
		case Region::PAINT: return AgentTag::PAINT;
		case Region::ROOF: return AgentTag::ROOF;
		case Region::GLASS: return AgentTag::GLASS;
		case Region::FRAME: return AgentTag::GLOSS;
		default: return AgentTag::FIXED;
	}
}

Color colour_of(Region r, const AgentTrim &trim) {
	switch (r) {
		case Region::UNDER: return trim.under;
		case Region::GLASS: return trim.glass;
		case Region::FRAME: return trim.blackout;
		default: return AGENT_WHITE;
	}
}

bool same_surface(Region a, Region b) {
	const auto painted = [](Region r) { return r == Region::PAINT || r == Region::ROOF; };
	return a == b || (painted(a) && painted(b));
}

std::vector<float> stations(const Loft &lf, const Dictionary &body) {
	const CarStyle &s = *lf.s;
	const int k = lf.fine ? 0 : 1, uniform = int(Array(body["stations"])[k]), arch = int(Array(body["arch_steps"])[k]);
	const int ends = int(Array(body["end_steps"])[k]), rolls = int(Array(body["end_roll_steps"])[k]);
	std::vector<float> xs;
	for (int i = 0; i <= uniform; i++) xs.push_back(-0.5f + float(i) / uniform);
	for (float f : { s.screen[0], s.screen[1], s.glass_front, s.glass_back }) xs.push_back(f);
	for (float f : s.rear)
		if (s.rear_glass) xs.push_back(f);
	if (s.bed_from > -0.5f) xs.insert(xs.end(), { s.bed_from, s.bed_from - BED_LIP_M / s.L });
	if (lf.fine) {
		for (float e : s.doors) xs.insert(xs.end(), { e - lf.seam * 0.5f, e + lf.seam * 0.5f, e - lf.pillar * 0.5f, e + lf.pillar * 0.5f });
		xs.insert(xs.end(), { s.screen[0] - lf.frame, s.screen[1] + lf.frame, s.glass_front - lf.frame, s.glass_back + lf.frame, s.screen[0] + s.pillar_foot / s.L });
		if (s.rear_glass) xs.insert(xs.end(), { s.rear[0] - lf.frame, s.rear[1] + lf.frame });
	}
	const float ar = (s.r + CAR_ARCH_PAD_M) / s.L;
	for (float a : s.axles)
		for (int j = -arch; j <= arch; j++) xs.push_back(a + ar * j / arch);
	for (int j = 1; j <= ends; j++) { // the plan's rounding at each end
		xs.push_back(0.5f - s.nose_r / s.L * j / (ends + 1));
		xs.push_back(-0.5f + s.tail_r / s.L * j / (ends + 1));
	}
	for (int j = 1; j <= rolls; j++) { // the ends' roll
		xs.push_back(0.5f - s.roll[0] / s.L * j / rolls);
		xs.push_back(-0.5f + s.roll[0] / s.L * j / rolls);
	}
	std::sort(xs.begin(), xs.end());
	const float eps = float(body["station_eps_m"]) / s.L;
	std::vector<float> out;
	for (float x : xs)
		if (x >= -0.5f && x <= 0.5f && (out.empty() || x - out.back() > eps)) out.push_back(x);
	return out;
}

struct Rows {
	const int *at = nullptr;
	int n = 0;
};

// One gap's bands, smooth inside a surface: a corner's normal is the cross of the tangent along the ring (one row each way,
// inside the band's surface) and the tangent along the length (one station each way).
template <typename At>
void gap(MeshBuilder &mb, const Loft &lf, const Rows &rows, const std::vector<float> &xs, size_t i, const At &at, const AgentTrim &trim) {
	const size_t n = xs.size();
	const float xm = (xs[i] + xs[i + 1]) * 0.5f;
	Region reg[RING];
	for (int r = 0; r + 1 < rows.n; r++) reg[r] = region_of(lf, rows.at[r], xm);
	for (int r = 0; r + 1 < rows.n; r++) {
		const int k = rows.at[r], k1 = rows.at[r + 1];
		const Vector3 face = (at(i, k1) - at(i, k) + at(i + 1, k1) - at(i + 1, k)).cross(at(i + 1, k) - at(i, k) + at(i + 1, k1) - at(i, k1));
		if (face.length_squared() < DEGENERATE_AREA) continue;
		const int prev = r > 0 && same_surface(reg[r - 1], reg[r]) ? rows.at[r - 1] : k, next = r + 2 < rows.n && same_surface(reg[r + 1], reg[r]) ? rows.at[r + 2] : k1;
		Vector3 pos[4], nrm[4];
		const size_t ii[4] = { i, i + 1, i + 1, i };
		const int kk[4] = { k, k, k1, k1 };
		for (int c = 0; c < 4; c++) {
			const size_t a = ii[c] > 0 ? ii[c] - 1 : ii[c], b = ii[c] + 1 < n ? ii[c] + 1 : ii[c];
			const Vector3 along_ring = kk[c] == k ? at(ii[c], k1) - at(ii[c], prev) : at(ii[c], next) - at(ii[c], k);
			const Vector3 nn = along_ring.cross(at(b, kk[c]) - at(a, kk[c]));
			pos[c] = at(ii[c], kk[c]);
			nrm[c] = nn.length_squared() > DEGENERATE_AREA ? nn.normalized() : face.normalized();
		}
		const Color col = colour_of(reg[r], trim);
		const Vector2 t2 = agent_tag(tag_of(reg[r]));
		for (int side : { -1, 1 }) {
			const Vector3 mirror(float(side), 1, 1);
			int v[4];
			for (int c = 0; c < 4; c++) v[c] = mb.add(pos[c] * mirror, nrm[c] * mirror, col, Vector2(), t2);
			mb.tri(v[0], v[1], v[2], face * mirror);
			mb.tri(v[0], v[2], v[3], face * mirror);
		}
	}
}

void loft(MeshBuilder &mb, const Loft &lf, const std::vector<float> &xs, const AgentTrim &trim) {
	const CarStyle &s = *lf.s;
	const size_t n = xs.size();
	std::vector<std::array<Vector2, RING>> rings(n);
	for (size_t i = 0; i < n; i++) car_ring(s, xs[i], rings[i].data());
	auto at = [&](size_t i, int k) { return Vector3(rings[i][size_t(k)].x, rings[i][size_t(k)].y, xs[i] * s.L); };
	const Rows rows = lf.fine ? Rows{ FINE_ROWS, RING } : Rows{ COARSE_ROWS, int(sizeof(COARSE_ROWS) / sizeof(int)) };
	for (size_t i = 0; i + 1 < n; i++) gap(mb, lf, rows, xs, i, at, trim);
}

} // namespace

int build_car(MeshBuilder &mb, const Dictionary &car, const Dictionary &body, const Dictionary &inside, const CabinLook *look, const AgentTrim &trim) {
	const CarStyle s = read_car_style(car, body);
	const int passengers = look ? car_cabin(mb, s, inside, *look) : 0;
	Loft lf;
	lf.s = &s;
	lf.fine = look != nullptr; // the cabin level: every detail
	lf.seam = float(body["door_seam_m"]) / s.L;
	lf.pillar = float(body["b_pillar_m"]) / s.L;
	lf.frame = s.frame / s.L;
	const std::vector<float> xs = stations(lf, body);
	loft(mb, lf, xs, trim);
	car_cap(mb, s, xs.back(), 1.f, trim);
	car_cap(mb, s, xs.front(), -1.f, trim);
	car_details(mb, s, body, lf.fine, trim);
	const int sides = int(Array(body["wheel_sides"])[lf.fine ? 0 : 1]);
	for (int k = 0; k < 2; k++)
		for (int side : { -1, 1 }) {
			const Vector3 hub(side * s.track[k] * 0.5f, s.r, s.axles[k] * s.L);
			if (lf.fine) agent_wheel_near(mb, hub, s.r, s.tw, sides, side, trim);
			else agent_wheel(mb, hub, s.r, s.tw, sides, side, trim);
		}
	return passengers;
}

} // namespace city
