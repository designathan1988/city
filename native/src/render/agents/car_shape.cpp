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

constexpr float EPS_M = 1e-6f, HALF_PI = 1.5707963f; // a length this short is none; a quarter turn to eight digits
constexpr float FC_LIMIT = 3.f; // Fritsch-Carlson: slopes scaled back inside the circle of radius 3 keep a piece monotone

void pair(const DataValue &d, const char *k, float out[2]) {
	const DataValue a = d[k];
	a.list(2);
	out[0] = a[0].f();
	out[1] = a[1].f();
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
	const CarShapeShape &K = s.shapes->car_shape;
	float z = xf * s.L, ar = s.r + s.shapes->agent_parts.arch_pad_m, y = car_low_at(s, xf);
	for (float a : s.axles) {
		float dz = z - a * s.L;
		if (std::fabs(dz) < ar) y = std::max(y, s.r * K.arch_centre + std::sqrt(ar * ar - dz * dz));
	}
	return y;
}

} // namespace

CarStyle read_car_style(const DataValue &d, const DataValue &body, const AgentShapes &shapes) {
	CarStyle s;
	s.shapes = &shapes;
	s.L = d["length"].f(); s.W = d["width"].f(); s.H = d["height"].f(); s.clear = d["clearance"].f();
	const Vector2 tyre = agent_tyre(d["tyre"]), track = agent_v2(d["track_m"]);
	s.r = tyre.x;
	s.tw = tyre.y;
	// the wheelbase centred when the sheet gives no overhang; the tyres flush with the sides when it gives no track
	const float wb = d["wheelbase_m"].f(), front = d["front_overhang_m"].f() > 0 ? d["front_overhang_m"].f() : (s.L - wb) * 0.5f;
	s.axles[0] = (s.L * 0.5f - front) / s.L;
	s.axles[1] = s.axles[0] - wb / s.L;
	s.track[0] = track.x > 0 ? track.x : s.W - s.tw;
	s.track[1] = track.y > 0 ? track.y : s.W - s.tw;
	const DataValue top = d["top"]; // front to back
	for (int64_t i = top.size() - 1; i >= 0; i--) {
		const DataValue p = top[i];
		s.top_x.push_back(p[0].f());
		s.top_y.push_back(p[1].f());
	}
	s.top_m = monotone_slopes(s.top_x, s.top_y);
	pair(d, "screen", s.screen); pair(d, "belt", s.belt);
	s.rear_glass = d["rear"].size() == 2; // [from, to], or [] for a car with no rear glass
	if (s.rear_glass) pair(d, "rear", s.rear);
	const DataValue doors = d["doors"];
	for (int i = 0; i < doors.size(); i++) s.doors.push_back(doors[i].f());
	s.nose_r = d["nose_r"].f(); s.tail_r = d["tail_r"].f();
	s.lean = d["tumble_m"].f() / std::max(EPS_M, s.H * (1 - s.belt[0]));
	s.lift[0] = d["nose_lift_m"].f(); s.lift[1] = d["tail_lift_m"].f(); s.tail_lamp_h = d["tail_lamp_h_m"].f(); s.bed_from = d["bed_from"].f();
	pair(body, "tuck_m", s.tuck); pair(body, "crown_m", s.crown);
	const DataValue roll = body["end_roll_m"];
	for (int i = 0; i < 3; i++) s.roll[i] = roll[i].f();
	s.sill = body["sill_m"].f(); s.bottom_round = body["bottom_round_m"].f(); s.bulge = body["bulge"].f();
	s.frame = body["glass_frame_m"].f(); s.pillar_foot = body["pillar_foot_m"].f(); s.roof_r = body["roof_round_m"].f();
	// the side glass from behind the A pillar to the C pillar, or just past the last door
	float quarter[2];
	pair(body, "c_pillar_m", quarter);
	const float last = s.doors.back();
	s.glass_front = s.screen[0] - body["a_foot_m"].f() / s.L;
	s.glass_back = d["quarter"].flag() && s.rear_glass ? std::max(s.rear[0] + quarter[0] / s.L, last - quarter[1] / s.L) : last + body["last_door_m"].f() / s.L;
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

float car_low_at(const CarStyle &s, float xf) {
	const float nose = std::clamp(((xf - 0.5f) * s.L + s.tuck[0]) / s.tuck[0], 0.f, 1.f), tail = std::clamp(((-0.5f - xf) * s.L + s.tuck[1]) / s.tuck[1], 0.f, 1.f);
	return s.clear + s.lift[0] * nose * nose + s.lift[1] * tail * tail;
}

void car_ring(const CarStyle &s, float xf, Vector2 out[RING]) {
	const CarShapeShape &K = s.shapes->car_shape;
	const float top = car_top_at(s, xf), hw = car_half_w(s, xf), yt = top - s.roof_r;
	const float belt = xf >= s.screen[0] ? s.belt[0] * s.H : s.H * (s.belt[1] + (s.belt[0] - s.belt[1]) * (xf + 0.5f) / (s.screen[0] + 0.5f));
	const float ys = std::min(belt, yt - K.roof_gap_m), bot = std::min(bottom_at(s, xf), ys - K.belt_room_m);
	const float mid = (bot + ys) * 0.5f, half = std::max(EPS_M, (ys - bot) * 0.5f);
	auto flank = [&](float y) { const float t = (y - mid) / half; return hw * (1 - s.bulge * t * t); };
	auto glass = [&](float y) { return flank(ys) - (y - ys) * s.lean; };
	const float rb = std::min(s.bottom_round, ys - bot), zb = hw - K.bottom_in_m, low = bot + rb;
	const float y2 = std::clamp(bot + s.sill, low + K.sill_room_m, std::max(low + K.sill_room_m, ys - K.belt_room_m));
	const float f = std::min(s.frame, (yt - ys) * 0.5f), zt = glass(yt), rr = std::min(s.roof_r, zt * K.roof_edge_share);
	int n = 0;
	out[n++] = Vector2(0, bot);
	out[n++] = Vector2(zb - rb, bot);
	out[n++] = Vector2(zb - rb + (flank(low) - zb + rb) * std::sin(HALF_PI * 0.5f), bot + rb * (1 - std::cos(HALF_PI * 0.5f)));
	out[n++] = Vector2(flank(low), low);
	for (float t : { 0.f, K.flank_low, K.flank_high, 1.f }) {
		const float y = y2 + (ys - y2) * t;
		out[n++] = Vector2(flank(y), y);
	}
	for (float y : { ys + f, yt - f, yt }) out[n++] = Vector2(glass(y), y);
	for (int k = 1; k <= 3; k++) {
		const float a = HALF_PI * float(k) / 4;
		out[n++] = Vector2(zt - rr + std::cos(a) * rr, yt + std::sin(a) * s.roof_r);
	}
	out[n++] = Vector2(zt - rr, top);
	const float crown = xf > s.screen[0] || xf < s.bed_from ? s.crown[0] : s.crown[1];
	for (float c : K.crown_rows) out[n++] = Vector2((zt - rr) * c, top + crown * (1 - c * c));
	const float edge = (0.5f - std::fabs(xf)) * s.L;
	if (edge >= s.roll[0]) return;
	const float u = 1 - edge / s.roll[0], k = 1 - (1 - std::sqrt(std::max(0.f, 1 - u * u))) * K.roll_depth, cy = (bot + top) * 0.5f;
	for (int i = 0; i < RING; i++) out[i] = Vector2(out[i].x - (1 - k) * s.roll[1] * (out[i].x / std::max(EPS_M, hw)), cy + (out[i].y - cy) * (1 - (1 - k) * s.roll[2]));
}

bool car_within(float x, const float r[2]) { return x <= std::max(r[0], r[1]) && x >= std::min(r[0], r[1]); }

bool car_overhang(const CarStyle &s, float xf) {
	const float z = xf * s.L, room = s.r + s.shapes->agent_parts.arch_pad_m;
	return z > s.axles[0] * s.L + room || z < s.axles[1] * s.L - room;
}

} // namespace city
