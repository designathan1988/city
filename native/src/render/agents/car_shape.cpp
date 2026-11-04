// The shape of a passenger-car style (car_shape.h): dimensions and lines read from agents.json, the roof line by
// monotone cubic Hermite interpolation (Fritsch, F. N. & Carlson, R. E., "Monotone Piecewise Cubic Interpolation", SIAM
// J. Numer. Anal. 17(2), 1980), the plan's rounded ends, the arches and the half ring of a station.
#include "render/agents/car_shape.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {

constexpr float BOTTOM_TAPER = 0.9f, ROOF_TAPER = 0.88f, ARCH_CENTRE = 0.96f;
constexpr float FC_LIMIT = 3.f; // Fritsch-Carlson: slopes scaled back inside the circle of radius 3 keep a piece monotone
constexpr float QUARTER = 0.7071068f; // cos 45°: the middle point of a quarter ellipse

void pair(const Dictionary &d, const char *k, float out[2]) {
	Array a = d[k];
	out[0] = float(a[0]);
	out[1] = float(a[1]);
}

// The slopes at the points (Fritsch & Carlson's steps 1-5): the secants' mean where they agree in sign, zero at a
// turn or a flat, then scaled so no piece overshoots.
std::vector<float> monotone_slopes(const std::vector<float> &x, const std::vector<float> &y) {
	const size_t n = x.size();
	std::vector<float> d(n > 1 ? n - 1 : 0), m(n, 0.f);
	for (size_t i = 0; i + 1 < n; i++) d[i] = x[i + 1] > x[i] ? (y[i + 1] - y[i]) / (x[i + 1] - x[i]) : 0.f;
	if (n < 2) return m;
	m[0] = d[0];
	m[n - 1] = d[n - 2];
	for (size_t i = 1; i + 1 < n; i++) m[i] = d[i - 1] * d[i] > 0 ? (d[i - 1] + d[i]) * 0.5f : 0.f;
	for (size_t i = 0; i + 1 < n; i++) {
		if (d[i] == 0.f) {
			m[i] = m[i + 1] = 0.f;
			continue;
		}
		const float a = m[i] / d[i], b = m[i + 1] / d[i], h = a * a + b * b;
		if (h > FC_LIMIT * FC_LIMIT) {
			const float t = FC_LIMIT / std::sqrt(h);
			m[i] = t * a * d[i];
			m[i + 1] = t * b * d[i];
		}
	}
	return m;
}

float bottom_at(const CarStyle &s, float xf) {
	float z = xf * s.L, ar = s.r + CAR_ARCH_PAD_M, y = s.clear;
	for (int k : { -1, 1 }) {
		float dz = z - k * s.axle * s.L;
		if (std::fabs(dz) < ar) y = std::max(y, s.r * ARCH_CENTRE + std::sqrt(ar * ar - dz * dz));
	}
	return y;
}

} // namespace

CarStyle read_car_style(const Dictionary &d, const Dictionary &body) {
	CarStyle s;
	s.L = float(d["length"]); s.W = float(d["width"]); s.H = float(d["height"]); s.r = float(d["wheel"]); s.clear = float(d["clearance"]);
	const Array top = d["top"]; // front to back
	for (int64_t i = top.size() - 1; i >= 0; i--) {
		const Array p = top[i];
		s.top_x.push_back(float(p[0]));
		s.top_y.push_back(float(p[1]));
	}
	s.top_m = monotone_slopes(s.top_x, s.top_y);
	pair(d, "screen", s.screen); pair(d, "side_glass", s.side); pair(d, "belt", s.belt);
	s.rear_glass = Array(d["rear"]).size() == 2;
	if (s.rear_glass) pair(d, "rear", s.rear);
	s.nose_r = float(d["nose_r"]); s.tail_r = float(d["tail_r"]);
	s.sill = float(body["sill_m"]); s.tumble = float(body["tumble"]); s.glass_top = float(body["glass_top_m"]);
	s.crown = float(body["crown_m"]); s.axle = float(body["axle"]); s.bottom_round = float(body["bottom_round_m"]); s.bulge = float(body["bulge"]);
	return s;
}

float car_top_at(const CarStyle &s, float xf) {
	const std::vector<float> &x = s.top_x, &y = s.top_y;
	if (x.empty()) return s.H;
	if (xf <= x.front()) return s.H * y.front();
	if (xf >= x.back()) return s.H * y.back();
	const size_t i = size_t(std::upper_bound(x.begin(), x.end(), xf) - x.begin()) - 1;
	const float h = x[i + 1] - x[i], t = h > 0 ? (xf - x[i]) / h : 0.f, t2 = t * t, t3 = t2 * t;
	const float v = (2 * t3 - 3 * t2 + 1) * y[i] + (t3 - 2 * t2 + t) * h * s.top_m[i] + (-2 * t3 + 3 * t2) * y[i + 1] + (t3 - t2) * h * s.top_m[i + 1];
	return s.H * v;
}

float car_half_w(const CarStyle &s, float xf) {
	float hw = s.W * 0.5f, df = (0.5f - xf) * s.L, db = (xf + 0.5f) * s.L;
	auto round = [&](float d, float r) { float k = r - d; return hw - r + std::sqrt(std::max(0.f, r * r - k * k)); };
	if (df < s.nose_r) return round(df, s.nose_r);
	if (db < s.tail_r) return round(db, s.tail_r);
	return hw;
}

void car_ring(const CarStyle &s, float xf, Vector2 out[RING]) {
	const float top = car_top_at(s, xf), bot = std::min(bottom_at(s, xf), top), hw = car_half_w(s, xf);
	const float belt = xf >= s.screen[0] ? s.belt[0] * s.H : s.H * (s.belt[1] + (s.belt[0] - s.belt[1]) * (xf + 0.5f) / (s.screen[0] + 0.5f));
	const float y2 = std::min(bot + s.sill, top), y3 = clampf(belt, y2, top), y4 = std::max(y3, top - s.glass_top);
	const float w4 = hw - s.tumble * (y4 - y3), hb = hw * BOTTOM_TAPER;
	const float rb = std::min({ s.bottom_round, y2 - bot, hb }); // the bottom corner, a quarter circle
	const float rx = w4 * (1 - ROOF_TAPER), ry = top - y4;          // the roof edge, a quarter ellipse
	const Vector2 pts[RING] = {
		Vector2(0, bot), Vector2(hb - rb, bot), Vector2(hb - rb + rb * QUARTER, bot + rb - rb * QUARTER), Vector2(hb, bot + rb),
		Vector2(hw, y2), Vector2(hw * (1 + s.bulge), (y2 + y3) * 0.5f), Vector2(hw, y3),
		Vector2(w4, y4), Vector2(w4 - rx + rx * QUARTER, y4 + ry * QUARTER), Vector2(w4 - rx, top), Vector2(0, top + s.crown)
	};
	for (int i = 0; i < RING; i++) out[i] = pts[i];
}

bool car_within(float x, const float r[2]) { return x <= std::max(r[0], r[1]) && x >= std::min(r[0], r[1]); }

bool car_overhang(const CarStyle &s, float xf) { return std::fabs(xf) * s.L > s.axle * s.L + s.r + CAR_ARCH_PAD_M; }

void car_door_edges(const CarStyle &s, float out[3]) {
	out[0] = std::max(s.screen[0], s.screen[1]); // the A pillar's foot
	out[1] = (s.side[0] + s.side[1]) * 0.5f;
	out[2] = std::min(s.side[0], s.side[1]);
}

} // namespace city
