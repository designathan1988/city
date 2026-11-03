// Lofted passenger-car body (Roadcraft render/carBody.ts, reduced): stations along the length, each a half ring from
// the floor to the roof crown shaped by the style's roof line, belt line, plan radii and wheel arches; bands are
// painted, glazed (windscreen, side and rear windows) or dark underneath. Lamps, grille, plates, four wheels and the
// cabin behind the glass (agent_cabin.cpp), between the windscreen and rear screen bases, under the belt and roof lines.
#include "render/agents/agent_meshes.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {

constexpr int RING = 7; // points per half ring, floor centre to roof centre
constexpr float BOTTOM_TAPER = 0.9f, ROOF_TAPER = 0.88f, MIN_GLASS_M = 0.12f;
constexpr float ARCH_PAD_M = 0.07f, ARCH_CENTRE = 0.96f, ARCH_STEPS = 3;
constexpr float CAR_LAMP_W = 0.28f, LAMP_H = 0.08f, LAMP_X = 0.65f, TAIL_W = 0.26f, TAIL_H = 0.1f, TAIL_X = 0.7f;
constexpr float PART_DEPTH_M = 0.03f, PART_PROUD_M = 0.01f, GRILLE_X = 0.6f, GRILLE_LOW_M = 0.1f, GRILLE_H = 0.15f;
constexpr float PLATE_W = 0.52f, PLATE_H = 0.11f, PLATE_Y_M = 0.22f;

struct Style {
	float L = 0, W = 0, H = 0, r = 0, clear = 0;
	std::vector<Vector2> top;
	float screen[2] = {}, rear[2] = {}, side[2] = {}, belt[2] = {}, nose_r = 0, tail_r = 0;
	float sill = 0, tumble = 0, glass_top = 0, crown = 0, axle = 0;
};

void pair(const Dictionary &d, const char *k, float out[2]) {
	Array a = d[k];
	out[0] = float(a[0]);
	out[1] = float(a[1]);
}

Style read_style(const Dictionary &d, const Dictionary &body) {
	Style s;
	s.L = float(d["length"]); s.W = float(d["width"]); s.H = float(d["height"]); s.r = float(d["wheel"]); s.clear = float(d["clearance"]);
	Array top = d["top"];
	for (int i = 0; i < top.size(); i++) { Array p = top[i]; s.top.push_back(Vector2(float(p[0]), float(p[1]))); }
	pair(d, "screen", s.screen); pair(d, "rear", s.rear); pair(d, "side_glass", s.side); pair(d, "belt", s.belt);
	s.nose_r = float(d["nose_r"]); s.tail_r = float(d["tail_r"]);
	s.sill = float(body["sill_m"]); s.tumble = float(body["tumble"]); s.glass_top = float(body["glass_top_m"]);
	s.crown = float(body["crown_m"]); s.axle = float(body["axle"]);
	return s;
}

float top_at(const Style &s, float xf) {
	for (size_t i = 0; i + 1 < s.top.size(); i++) {
		const Vector2 &a = s.top[i], &b = s.top[i + 1];
		if (xf <= a.x && xf >= b.x) return s.H * (a.x == b.x ? a.y : a.y + (b.y - a.y) * (a.x - xf) / (a.x - b.x));
	}
	return s.H * (xf > 0 ? s.top.front().y : s.top.back().y);
}

float half_w(const Style &s, float xf) {
	float hw = s.W * 0.5f, df = (0.5f - xf) * s.L, db = (xf + 0.5f) * s.L;
	auto round = [&](float d, float r) { float k = r - d; return hw - r + std::sqrt(std::max(0.f, r * r - k * k)); };
	if (df < s.nose_r) return round(df, s.nose_r);
	if (db < s.tail_r) return round(db, s.tail_r);
	return hw;
}

float bottom_at(const Style &s, float xf) {
	float z = xf * s.L, ar = s.r + ARCH_PAD_M, y = s.clear;
	for (int k : { -1, 1 }) {
		float dz = z - k * s.axle * s.L;
		if (std::fabs(dz) < ar) y = std::max(y, s.r * ARCH_CENTRE + std::sqrt(ar * ar - dz * dz));
	}
	return y;
}

void ring(const Style &s, float xf, Vector2 out[RING]) {
	float top = top_at(s, xf), bot = std::min(bottom_at(s, xf), top), hw = half_w(s, xf);
	float belt = xf >= s.screen[0] ? s.belt[0] * s.H : s.H * (s.belt[1] + (s.belt[0] - s.belt[1]) * (xf + 0.5f) / (s.screen[0] + 0.5f));
	float y2 = std::min(bot + s.sill, top), y3 = clampf(belt, y2, top), y4 = std::max(y3, top - s.glass_top);
	float w4 = hw - s.tumble * (y4 - y3);
	const Vector2 pts[RING] = { Vector2(0, bot), Vector2(hw * BOTTOM_TAPER, bot), Vector2(hw, y2), Vector2(hw, y3), Vector2(w4, y4),
		Vector2(w4 * ROOF_TAPER, top), Vector2(0, top + s.crown) };
	for (int i = 0; i < RING; i++) out[i] = pts[i];
}

bool within(float x, const float r[2]) { return x <= std::max(r[0], r[1]) && x >= std::min(r[0], r[1]); }

// Tag of band k (ring[k] -> ring[k+1]) between two stations.
AgentTag band(const Style &s, int k, float xm, float glass_h) {
	if (k == 0) return AgentTag::FIXED;
	bool screen = within(xm, s.screen) || within(xm, s.rear);
	if (k == 3) return (within(xm, s.side) && glass_h > MIN_GLASS_M) || (screen && glass_h > MIN_GLASS_M) ? AgentTag::GLASS : AgentTag::PAINT;
	if (k >= 4 && screen) return AgentTag::GLASS;
	return AgentTag::PAINT;
}

std::vector<float> stations(const Style &s, int uniform, float eps) {
	std::vector<float> xs;
	for (int i = 0; i <= uniform; i++) xs.push_back(-0.5f + float(i) / uniform);
	for (float f : { s.screen[0], s.screen[1], s.rear[0], s.rear[1], s.side[0], s.side[1] })
		if (f > -0.5f && f < 0.5f) xs.push_back(f);
	float ar = (s.r + ARCH_PAD_M) / s.L;
	for (int k : { -1, 1 })
		for (int j = -int(ARCH_STEPS); j <= int(ARCH_STEPS); j++) xs.push_back(k * s.axle + ar * j / ARCH_STEPS);
	std::sort(xs.begin(), xs.end());
	std::vector<float> out;
	for (float x : xs)
		if (out.empty() || x - out.back() > eps) out.push_back(x);
	return out;
}

Color colour_of(AgentTag t, const AgentTrim &trim) {
	return t == AgentTag::FIXED ? trim.under : (t == AgentTag::GLASS ? trim.glass : AGENT_WHITE);
}

void loft(MeshBuilder &mb, const Style &s, const std::vector<float> &xs, const AgentTrim &trim) {
	for (size_t i = 0; i + 1 < xs.size(); i++) {
		Vector2 r0[RING], r1[RING];
		ring(s, xs[i], r0);
		ring(s, xs[i + 1], r1);
		float z0 = xs[i] * s.L, z1 = xs[i + 1] * s.L, xm = (xs[i] + xs[i + 1]) * 0.5f;
		float gh = std::min(r0[4].y - r0[3].y, r1[4].y - r1[3].y);
		for (int k = 0; k + 1 < RING; k++) {
			AgentTag t = band(s, k, xm, gh);
			for (int side : { -1, 1 }) {
				Vector3 a(side * r0[k].x, r0[k].y, z0), b(side * r1[k].x, r1[k].y, z1), c(side * r1[k + 1].x, r1[k + 1].y, z1),
						d(side * r0[k + 1].x, r0[k + 1].y, z0);
				Vector2 m = (r0[k] + r0[k + 1]) * 0.5f;
				Vector3 out = k == 0 ? Vector3(0, -1, 0) : Vector3(side * m.x, m.y - s.H * 0.5f, 0);
				mb.quad(a, b, c, d, colour_of(t, trim), out, nullptr, agent_tag(t));
			}
		}
	}
}

void cap(MeshBuilder &mb, const Style &s, float xf, float dir, const AgentTrim &trim) {
	Vector2 r[RING];
	ring(s, xf, r);
	float z = xf * s.L, cy = (r[0].y + r[RING - 1].y) * 0.5f;
	Vector3 n(0, 0, dir);
	for (int k = 0; k + 1 < RING; k++)
		for (int side : { -1, 1 }) {
			AgentTag t = k == 0 ? AgentTag::FIXED : AgentTag::PAINT;
			Color c = colour_of(t, trim);
			int i0 = mb.add(Vector3(0, cy, z), n, c, Vector2(), agent_tag(t));
			int i1 = mb.add(Vector3(side * r[k].x, r[k].y, z), n, c, Vector2(), agent_tag(t));
			int i2 = mb.add(Vector3(side * r[k + 1].x, r[k + 1].y, z), n, c, Vector2(), agent_tag(t));
			mb.tri(i0, i1, i2, n);
		}
}

void details(MeshBuilder &mb, const Style &s, float lamp_y, float tail_y, const AgentTrim &trim) {
	float zf = s.L * 0.5f, zb = -zf, hf = half_w(s, 0.5f), hb = half_w(s, -0.5f), bot = s.clear;
	float yl = bot + (top_at(s, 0.5f) - bot) * lamp_y, yt = bot + (top_at(s, -0.5f) - bot) * tail_y;
	for (int side : { -1, 1 }) {
		float xl = side * hf * LAMP_X, xt = side * hb * TAIL_X;
		agent_box(mb, Vector3(xl - CAR_LAMP_W * 0.5f, yl - LAMP_H * 0.5f, zf - PART_DEPTH_M), Vector3(xl + CAR_LAMP_W * 0.5f, yl + LAMP_H * 0.5f, zf + PART_PROUD_M),
				trim.headlamp, AgentTag::HEAD);
		agent_box(mb, Vector3(xt - TAIL_W * 0.5f, yt - TAIL_H * 0.5f, zb - PART_PROUD_M), Vector3(xt + TAIL_W * 0.5f, yt + TAIL_H * 0.5f, zb + PART_DEPTH_M),
				trim.tail, AgentTag::TAIL);
	}
	float gy = bot + GRILLE_LOW_M;
	agent_box(mb, Vector3(-hf * GRILLE_X, gy, zf - PART_DEPTH_M), Vector3(hf * GRILLE_X, gy + GRILLE_H, zf + PART_PROUD_M), trim.grille, AgentTag::FIXED);
	for (float z : { zf, zb })
		agent_box(mb, Vector3(-PLATE_W * 0.5f, bot + PLATE_Y_M - PLATE_H, z - PART_DEPTH_M), Vector3(PLATE_W * 0.5f, bot + PLATE_Y_M, z + PART_PROUD_M),
				trim.plate, AgentTag::FIXED);
}

// The inside of the body: from the floor over the sill to the lowest belt line and under the roof between the screens.
Cabin cabin_of(const Style &s) {
	Cabin c;
	c.floor = s.clear + s.sill;
	c.belt = std::min(s.belt[0], s.belt[1]) * s.H;
	c.z_front = std::max(s.screen[0], s.screen[1]) * s.L;
	c.z_back = std::min(s.rear[0], s.rear[1]) * s.L;
	// under the roof between the screens' top edges (at their bases the line is the bonnet's and the boot's)
	c.roof = std::min(top_at(s, std::min(s.screen[0], s.screen[1])), top_at(s, std::max(s.rear[0], s.rear[1]))) - s.glass_top;
	c.half_w = std::min(half_w(s, c.z_front / s.L), half_w(s, c.z_back / s.L));
	return c;
}

} // namespace

void build_car(MeshBuilder &mb, const Dictionary &car, const Dictionary &body, const CabinLook &cabin, const AgentTrim &trim) {
	Style s = read_style(car, body);
	agent_cabin(mb, cabin_of(s), cabin, trim);
	std::vector<float> xs = stations(s, int(body["stations"]), float(body["station_eps_m"]));
	loft(mb, s, xs, trim);
	cap(mb, s, xs.back(), 1.f, trim);
	cap(mb, s, xs.front(), -1.f, trim);
	details(mb, s, float(body["lamp_y"]), float(body["tail_y"]), trim);
	float inset = float(body["wheel_inset_m"]), ww = float(body["wheel_w"]);
	int sides = int(body["wheel_sides"]);
	for (int k : { -1, 1 })
		for (int side : { -1, 1 })
			agent_wheel(mb, Vector3(side * (s.W * 0.5f - inset), s.r, k * s.axle * s.L), s.r, ww, sides, side, trim);
}

} // namespace city
