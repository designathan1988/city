// One piece of furniture of a building cut open: the parts of its model (furniture_look.h) put in the piece's own frame and
// scaled to its size. Boxes are instanced boxes (the facade parts' way: a MultiMesh a thickness band); the round parts
// (a lamp's shade, a plant's pot, a leg) are fine geometry in the near shell, with few sides.
#include "render/buildings/interior_emit.h"

#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr int CYL_SIDES = 10, BALL_SEGMENTS = 8, BALL_RINGS = 4;
constexpr float TURN = 6.2831853f, HALF_TURN = 3.1415927f;
constexpr float FURNITURE_LIFT = 0.05f; // a piece stands this far over the floor (its slab, FLOOR_LIFT, is under it)
constexpr float CEILING_GAP = 0.06f; // a ceiling lamp hangs from the ceiling's underside: the slab's thickness

// A part's centre and the way its boxes turn, for copy `i` of `p` and turn `s` of its spin.
struct Piece {
	Shell &shell, &parts;
	const InteriorLook &look;
	const Placed &pl;
	float sx, sy, sz; // the placed size over the model's
	void box(const Basis &g, const FurniturePart &p, const float size[3], const float at[3], const Paint &paint) const;
	void cylinder(const Basis &g, const FurniturePart &p, const float size[3], const float at[3], const Paint &paint) const;
	void ball(const Basis &g, const FurniturePart &p, const float size[3], const float at[3], const Paint &paint) const;
};

void Piece::box(const Basis &g, const FurniturePart &, const float size[3], const float at[3], const Paint &paint) const {
	const float w = size[0] * sx, h = size[1] * sy, d = size[2] * sz;
	const float cx = at[0] * sx, by = at[1] * sy, cz = at[2] * sz;
	parts.box(g, cx - w / 2, cx + w / 2, by, by + h, cz - d / 2, cz + d / 2, paint_lin(paint), paint.finish);
}

void Piece::cylinder(const Basis &g, const FurniturePart &, const float size[3], const float at[3], const Paint &paint) const {
	const float radial = (sx + sz) / 2, r0 = size[0] * radial, r1 = size[2] * radial, h = size[1] * sy;
	const float cx = at[0] * sx, by = at[1] * sy, cz = at[2] * sz;
	const Lin lin = paint_lin(paint);
	Vector3 ring0[CYL_SIDES], ring1[CYL_SIDES];
	for (int i = 0; i < CYL_SIDES; i++) {
		const float a = TURN * float(i) / float(CYL_SIDES), c = std::cos(a), s = std::sin(a);
		ring0[i] = g.at(cx + r0 * c, by, cz + r0 * s);
		ring1[i] = g.at(cx + r1 * c, by + h, cz + r1 * s);
	}
	for (int i = 0; i < CYL_SIDES; i++) {
		const int j = (i + 1) % CYL_SIDES;
		const Vector3 out = (ring0[i] + ring0[j]) * 0.5f - g.at(cx, by, cz);
		shell.quad(ring0[i], ring0[j], ring1[j], ring1[i], out, lin, paint.finish);
		if (r1 > 0) shell.tri(g.at(cx, by + h, cz), ring1[i], ring1[j], g.e1, lin, paint.finish);
	}
}

void Piece::ball(const Basis &g, const FurniturePart &, const float size[3], const float at[3], const Paint &paint) const {
	const float r = size[0] * (sx + sy + sz) / 3;
	const Lin lin = paint_lin(paint);
	const Vector3 centre = g.at(at[0] * sx, at[1] * sy, at[2] * sz);
	auto point = [&](int ring, int seg) {
		const float lat = HALF_TURN * (float(ring) / float(BALL_RINGS) - 0.5f), lon = TURN * float(seg) / float(BALL_SEGMENTS);
		return g.at(at[0] * sx + r * std::cos(lat) * std::cos(lon), at[1] * sy + r * std::sin(lat), at[2] * sz + r * std::cos(lat) * std::sin(lon));
	};
	for (int ring = 0; ring < BALL_RINGS; ring++)
		for (int seg = 0; seg < BALL_SEGMENTS; seg++) {
			const int next = (seg + 1) % BALL_SEGMENTS;
			const Vector3 a = point(ring, seg), b = point(ring, next), c = point(ring + 1, next), d = point(ring + 1, seg);
			shell.quad(a, b, c, d, (a + b + c + d) * 0.25f - centre, lin, paint.finish);
		}
}
} // namespace

void emit_furniture(Shell &shell, Shell &parts, const Placed &pl, const Furniture &f, float z) {
	const InteriorLook &look = pl.look->interior;
	const float *nominal = pl.data->interior.size[f.kind]; // across, deep, high
	if (nominal[0] <= 0 || nominal[1] <= 0 || nominal[2] <= 0) return;
	Piece piece{ shell, parts, look, pl, f.w / nominal[0], f.h / nominal[2], f.d / nominal[1] };
	const float base = f.kind == FK_CEILING_LAMP ? z + pl.b->level_h(pl.b->cut_level) - CEILING_GAP - f.h : z + FURNITURE_LIFT;
	// The piece faces (sin a, -cos a) in the lot's plan and its right is (cos a, sin a): a = 0 faces the street.
	const float c = std::cos(f.angle), s = std::sin(f.angle);
	const Vector3 right = pl.lot.e0 * c + pl.lot.e1 * s, front = pl.lot.e0 * s - pl.lot.e1 * c;
	const Vector3 origin = pl.lot.at(f.x, f.y, base);
	for (const FurniturePart &p : look.models[f.kind]) {
		for (int copy = 0; copy < p.copies; copy++) {
			float size[3], at[3];
			for (int k = 0; k < 3; k++) {
				size[k] = p.size[k] + p.grow[k] * float(copy);
				at[k] = p.at[k] + p.step[k] * float(copy);
			}
			const Paint &paint = look.palette[p.colours[size_t(copy) % p.colours.size()]];
			for (int turn = 0; turn < p.spin; turn++) {
				const float a = TURN * float(turn) / float(p.spin), ca = std::cos(a), sa = std::sin(a);
				const Basis g{ origin, right * ca - front * sa, pl.lot.e2, right * sa + front * ca };
				if (p.shape == PART_BOX) piece.box(g, p, size, at, paint);
				else if (p.shape == PART_CYL) piece.cylinder(g, p, size, at, paint);
				else piece.ball(g, p, size, at, paint);
			}
		}
	}
}

} // namespace city::b40
