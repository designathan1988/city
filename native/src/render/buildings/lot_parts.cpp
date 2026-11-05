// Parts the player sets on a lot (Build > Parts) that no generated lot holds: flights of steps, ramps, rocks, a paved parking bay and
// a clock on the wall. A flight runs from the wall out along the part's facing, its top at the wall.
#include "render/buildings/facade.h"

#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr uint32_t CLOCK_FACE = 0xf2efe6, CLOCK_HANDS = 0x23262a, BAY_LINE = 0xe9e6dc;
constexpr float ROCK_SHADE = 0.86f, DIAL_OUT = 0.02f, DIAL_MARGIN = 0.12f, HAND_W = 0.05f, LINE_W = 0.1f, LINE_UP = 0.01f, CAR_BAY_M = 2.5f, LONG_HAND = 0.8f, SHORT_HAND = 0.6f;
// A rock is a few boxes of a share of the part's size: offset across, offset along, width, height.
constexpr float ROCKS[3][4] = { { -0.2f, -0.1f, 0.6f, 1.f }, { 0.25f, 0.15f, 0.45f, 0.7f }, { -0.05f, 0.3f, 0.35f, 0.45f } };

struct Run {
	float ox, oy; // the foot of the flight
	float dx, dy; // from the foot towards the wall
	float length, width;
};

// The flight's run in the lot frame: its facing looks away from the wall, so the foot is that way.
Run run_of(const LotElement &e) {
	switch (e.facing) {
		case FACE_STREET: return { e.x, e.y - e.d / 2, 0, 1, e.d, e.w };
		case FACE_BACK: return { e.x, e.y + e.d / 2, 0, -1, e.d, e.w };
		case FACE_RIGHT: return { e.x + e.w / 2, e.y, -1, 0, e.w, e.d };
		default: return { e.x - e.w / 2, e.y, 1, 0, e.w, e.d };
	}
}

void stair(Shell &shell, const Placed &pl, const LotElement &e, float g, const Lin &lin, int8_t fin) {
	Run r = run_of(e);
	int steps = std::max(1, int(std::ceil(e.h / RISER_M)));
	float tread = r.length / float(steps), half = r.width / 2, side_x = r.dy, side_y = -r.dx;
	for (int i = 0; i < steps; i++) {
		float a = float(i) * tread, b = r.length;
		float ax = r.ox + r.dx * a, ay = r.oy + r.dy * a, bx = r.ox + r.dx * b, by = r.oy + r.dy * b;
		float x0 = std::min({ ax - side_x * half, ax + side_x * half, bx - side_x * half, bx + side_x * half });
		float x1 = std::max({ ax - side_x * half, ax + side_x * half, bx - side_x * half, bx + side_x * half });
		float y0 = std::min({ ay - side_y * half, ay + side_y * half, by - side_y * half, by + side_y * half });
		float y1 = std::max({ ay - side_y * half, ay + side_y * half, by - side_y * half, by + side_y * half });
		shell.box(pl.lot, x0, x1, y0, y1, g + e.h * float(i) / float(steps), g + e.h * float(i + 1) / float(steps), lin, fin);
	}
}

void ramp(Shell &shell, const Placed &pl, const LotElement &e, float g, const Lin &lin, int8_t fin) {
	Run r = run_of(e);
	float half = r.width / 2, sx = r.dy * half, sy = -r.dx * half;
	float tx = r.ox + r.dx * r.length, ty = r.oy + r.dy * r.length;
	Vector3 foot_a = pl.lot.at(r.ox - sx, r.oy - sy, g), foot_b = pl.lot.at(r.ox + sx, r.oy + sy, g);
	Vector3 top_a = pl.lot.at(tx - sx, ty - sy, g + e.h), top_b = pl.lot.at(tx + sx, ty + sy, g + e.h);
	Vector3 low_a = pl.lot.at(tx - sx, ty - sy, g), low_b = pl.lot.at(tx + sx, ty + sy, g);
	Vector3 up = (top_b - foot_a).cross(top_a - foot_b).normalized();
	if (up.dot(pl.lot.e2) < 0) up = -up;
	shell.quad(foot_a, foot_b, top_b, top_a, up, lin, fin);
	Vector3 side = (foot_b - foot_a).normalized();
	shell.tri(foot_a, top_a, low_a, -side, lin, fin);
	shell.tri(foot_b, low_b, top_b, side, lin, fin);
}

void rocks(Shell &shell, const Placed &pl, const LotElement &e, float g, const Lin &lin) {
	for (const auto &k : ROCKS) {
		float x = e.x + k[0] * e.w, y = e.y + k[1] * e.d, hw = k[2] * e.w / 2, hd = k[2] * e.d / 2;
		shell.box(pl.lot, x - hw, x + hw, y - hd, y + hd, g, g + k[3] * e.h, lin * ROCK_SHADE, FIN_STONE, SKIP_E2_NEG);
	}
}

void parking(Shell &shell, const Placed &pl, const LotElement &e, float g, const Lin &lin) {
	float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, y0 = e.y - e.d / 2, y1 = e.y + e.d / 2;
	shell.rect(pl.lot, x0, x1, y0, y1, g + e.h, lin, FIN_CONCRETE);
	Lin line = lin_of(BAY_LINE);
	bool across_x = e.facing == FACE_STREET || e.facing == FACE_BACK;
	float length = across_x ? e.w : e.d;
	int bays = std::max(1, int(length / CAR_BAY_M));
	for (int i = 0; i <= bays; i++) {
		float a = length * float(i) / float(bays) - length / 2;
		if (across_x) shell.rect(pl.lot, e.x + a - LINE_W / 2, e.x + a + LINE_W / 2, y0, y1, g + e.h + LINE_UP, line, FIN_NONE);
		else shell.rect(pl.lot, x0, x1, e.y + a - LINE_W / 2, e.y + a + LINE_W / 2, g + e.h + LINE_UP, line, FIN_NONE);
	}
}

// A square dial on the wall, its hands at ten past ten, the way a clock is shown.
void clock(Shell &shell, const Placed &pl, const LotElement &e, const Lin &lin, int8_t fin) {
	float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, y0 = e.y - e.d / 2, y1 = e.y + e.d / 2;
	shell.box(pl.lot, x0, x1, y0, y1, e.z, e.z + e.h, lin, fin);
	bool front = e.facing == FACE_STREET || e.facing == FACE_BACK;
	float sign = e.facing == FACE_STREET || e.facing == FACE_LEFT ? -1.f : 1.f, m = e.h * DIAL_MARGIN, cz = e.z + e.h / 2;
	float at = (front ? e.y + sign * e.d / 2 : e.x + sign * e.w / 2) + sign * DIAL_OUT, half = (front ? e.w : e.d) / 2 - m;
	float cx = front ? e.x : e.y;
	auto pane = [&](float a0, float a1, float z0, float z1, float out, const Lin &colour) {
		float p = at + sign * out;
		Vector3 n = front ? pl.lot.e1 * sign : pl.lot.e0 * sign;
		Vector3 c0 = front ? pl.lot.at(a0, p, z0) : pl.lot.at(p, a0, z0), c1 = front ? pl.lot.at(a1, p, z0) : pl.lot.at(p, a1, z0);
		Vector3 c2 = front ? pl.lot.at(a1, p, z1) : pl.lot.at(p, a1, z1), c3 = front ? pl.lot.at(a0, p, z1) : pl.lot.at(p, a0, z1);
		shell.quad(c0, c1, c2, c3, n, colour, FIN_NONE);
	};
	pane(cx - half, cx + half, e.z + m, e.z + e.h - m, 0, lin_of(CLOCK_FACE));
	Lin hands = lin_of(CLOCK_HANDS);
	pane(cx - HAND_W, cx + HAND_W, cz, cz + half * LONG_HAND, DIAL_OUT, hands);
	pane(cx, cx + half * SHORT_HAND, cz - HAND_W, cz + HAND_W, DIAL_OUT, hands);
}
} // namespace

bool emit_built_part(Shell &shell, const Placed &pl, const LotElement &e, float g, const Lin &lin, int8_t fin) {
	switch (e.kind) {
		case EL_STAIR: stair(shell, pl, e, g, lin, fin); return true;
		case EL_RAMP: ramp(shell, pl, e, g, lin, fin); return true;
		case EL_ROCKS: rocks(shell, pl, e, g, lin); return true;
		case EL_PARKING: parking(shell, pl, e, g, lin); return true;
		case EL_CLOCK: clock(shell, pl, e, lin, fin); return true;
		default: return false;
	}
}

} // namespace city::b40
