// One piece of furniture of a building cut open: the parts of its model (furniture_look.h) put in the piece's own frame and
// scaled to its size. Hard boxes are instanced boxes (the facade parts' way: a MultiMesh a thickness band); boxes with rounded
// edges (furniture.json "edges" and a part's "edge") and the round parts (a lamp's shade, a plant's pot, a leg) are fine
// geometry in the near shell, with few sides. A row of copies that varies (books) draws each copy's width, height and colour
// from the piece's own seed, so two shelves differ and one shelf stays the same from frame to frame.
#include "render/buildings/interior_emit.h"
#include "core/math.h"
#include "core/random.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>

using namespace godot;

namespace city::b40 {

namespace {
constexpr int CYL_SIDES = 10, BALL_SEGMENTS = 8, BALL_RINGS = 4; // a piece's cylinders and balls, as fine as a room seen through a window needs
constexpr float FURNITURE_LIFT = 0.05f; // a piece stands this far over the floor (its slab, FLOOR_LIFT, is under it)
constexpr float CEILING_GAP = 0.06f; // a ceiling lamp hangs from the ceiling's underside: the slab's thickness
constexpr int ROUND_STEPS = 2; // facets of a rounded edge's quarter turn: a bevel that reads as round at a room's distance
constexpr int QUADRANTS = 4; // a rounded box's corners around the up axis
constexpr int ROUND_LONS = QUADRANTS * (ROUND_STEPS + 1), ROUND_RINGS = 2 * (ROUND_STEPS + 1); // the corner quarters' lines, both ends kept

// The cosine and sine of each facet's angle over a quarter turn, the ends exact (so the flat faces' points meet exactly).
struct QuarterTurn {
	float c[ROUND_STEPS + 1], s[ROUND_STEPS + 1];
	QuarterTurn() {
		for (int k = 0; k <= ROUND_STEPS; k++) {
			const float a = PI_F * 0.5f * float(k) / float(ROUND_STEPS);
			c[k] = k == ROUND_STEPS ? 0.f : std::cos(a);
			s[k] = k == ROUND_STEPS ? 1.f : std::sin(a);
		}
	}
};
const QuarterTurn QUARTER;

// A box with its edges rounded by `r` (no more than half its thinnest side), in frame `g`: centre c[3], half sides h[3].
// Each corner is an eighth of a ball of radius r about the inner box's corner; the quads between the eighths are the flat
// faces and the bands of the edges, and the two caps close the top and the bottom.
void rounded_box(Shell &shell, const Basis &g, const float c[3], const float h[3], float r, const Lin &lin, int8_t fin) {
	const float in[3] = { h[0] - r, h[1] - r, h[2] - r };
	Vector3 grid[ROUND_RINGS][ROUND_LONS];
	for (int ring = 0; ring < ROUND_RINGS; ring++) {
		const bool upper = ring > ROUND_STEPS;
		const int k = upper ? ring - (ROUND_STEPS + 1) : ROUND_STEPS - ring; // the latitude's step from the equator
		const float up = upper ? 1.f : -1.f, flat = QUARTER.c[k], rise = up * QUARTER.s[k];
		for (int q = 0; q < QUADRANTS; q++)
			for (int j = 0; j <= ROUND_STEPS; j++) {
				// the quarter's direction turned by q quarters: (c, s), (-s, c), (-c, -s), (s, -c)
				const float qc = QUARTER.c[j], qs = QUARTER.s[j];
				const float dx = q == 0 ? qc : q == 1 ? -qs : q == 2 ? -qc : qs;
				const float dz = q == 0 ? qs : q == 1 ? qc : q == 2 ? -qs : -qc;
				const float sx = q == 0 || q == 3 ? 1.f : -1.f, sz = q < 2 ? 1.f : -1.f;
				grid[ring][q * (ROUND_STEPS + 1) + j] =
						g.at(c[0] + sx * in[0] + r * flat * dx, c[1] + up * in[1] + r * rise, c[2] + sz * in[2] + r * flat * dz);
			}
	}
	const Vector3 centre = g.at(c[0], c[1], c[2]);
	for (int ring = 0; ring + 1 < ROUND_RINGS; ring++)
		for (int i = 0; i < ROUND_LONS; i++) {
			const int j = (i + 1) % ROUND_LONS;
			const Vector3 &a = grid[ring][i], &b = grid[ring][j], &d = grid[ring + 1][i], &e = grid[ring + 1][j];
			shell.quad(a, b, e, d, (a + b + d + e) * 0.25f - centre, lin, fin);
		}
	for (int cap = 0; cap < 2; cap++) {
		const int ring = cap == 0 ? 0 : ROUND_RINGS - 1;
		const Vector3 &p0 = grid[ring][0], &p1 = grid[ring][ROUND_STEPS + 1], &p2 = grid[ring][2 * (ROUND_STEPS + 1)],
					  &p3 = grid[ring][3 * (ROUND_STEPS + 1)];
		shell.quad(p0, p1, p2, p3, cap == 0 ? -g.e1 : g.e1, lin, fin);
	}
}

uint32_t float_bits(float v) {
	uint32_t bits = 0;
	std::memcpy(&bits, &v, sizeof bits);
	return bits;
}

// A part's centre and the way its boxes turn, for copy `i` of `p` and turn `s` of its spin.
struct Piece {
	Shell &shell, &parts;
	const InteriorLook &look;
	const Placed &pl;
	float sx, sy, sz; // the placed size over the model's
	float edge; // the kind's rounding, metres
	void box(const Basis &g, const FurniturePart &p, const float size[3], const float at[3], const Paint &paint) const;
	void cylinder(const Basis &g, const FurniturePart &p, const float size[3], const float at[3], const Paint &paint) const;
	void ball(const Basis &g, const FurniturePart &p, const float size[3], const float at[3], const Paint &paint) const;
};

void Piece::box(const Basis &g, const FurniturePart &p, const float size[3], const float at[3], const Paint &paint) const {
	const float w = size[0] * sx, h = size[1] * sy, d = size[2] * sz;
	const float cx = at[0] * sx, by = at[1] * sy, cz = at[2] * sz;
	const float r = std::min(p.edge < 0 ? edge : p.edge, std::min({ w, h, d }) * 0.5f);
	if (r <= 0) {
		parts.box(g, cx - w / 2, cx + w / 2, by, by + h, cz - d / 2, cz + d / 2, paint_lin(paint), paint.finish);
		return;
	}
	const float centre[3] = { cx, by + h / 2, cz }, half[3] = { w / 2, h / 2, d / 2 };
	rounded_box(shell, g, centre, half, r, paint_lin(paint), paint.finish);
}

// The copies of a varied row: each one's share of the row (the shares sum to the copies, so the row keeps its length) and
// where its slot starts, its height's share and its palette index, drawn from `seed`.
struct RowDraw {
	std::vector<float> share, tall, start;
	std::vector<uint16_t> colour;
	void draw(const FurniturePart &p, uint32_t seed) {
		const int n = p.copies;
		share.resize(size_t(n));
		tall.resize(size_t(n));
		start.resize(size_t(n));
		colour.resize(size_t(n));
		float sum = 0;
		for (int i = 0; i < n; i++) {
			// three draws in a chain, one for each thing a copy varies by
			const uint32_t width_draw = hash_combine(seed, uint32_t(i)), tall_draw = hash_u32(width_draw), colour_draw = hash_u32(tall_draw);
			share[i] = 1.f + p.vary[0] * (2.f * hash_float(width_draw) - 1.f);
			tall[i] = 1.f - p.vary[1] * hash_float(tall_draw);
			colour[i] = p.colours[size_t(hash_u32(colour_draw)) % p.colours.size()];
			sum += share[i];
		}
		float at = 0;
		for (int i = 0; i < n; i++) {
			share[i] *= float(n) / sum;
			start[i] = at;
			at += share[i];
		}
	}
};

void Piece::cylinder(const Basis &g, const FurniturePart &, const float size[3], const float at[3], const Paint &paint) const {
	const float radial = (sx + sz) / 2, r0 = size[0] * radial, r1 = size[2] * radial, h = size[1] * sy;
	const float cx = at[0] * sx, by = at[1] * sy, cz = at[2] * sz;
	const Lin lin = paint_lin(paint);
	Vector3 ring0[CYL_SIDES], ring1[CYL_SIDES];
	for (int i = 0; i < CYL_SIDES; i++) {
		const float a = TAU_F * float(i) / float(CYL_SIDES), c = std::cos(a), s = std::sin(a);
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
		const float lat = PI_F * (float(ring) / float(BALL_RINGS) - 0.5f), lon = TAU_F * float(seg) / float(BALL_SEGMENTS);
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
	Piece piece{ shell, parts, look, pl, f.w / nominal[0], f.h / nominal[2], f.d / nominal[1], look.edges[f.kind] };
	const uint32_t seed = hash_combine(hash_combine(pl.id, float_bits(f.x)), float_bits(f.y)); // the piece's own: where it stands
	const float base = f.kind == FK_CEILING_LAMP ? z + pl.b->level_h(pl.b->cut_level) - CEILING_GAP - f.h : z + FURNITURE_LIFT;
	// The piece faces (sin a, -cos a) in the lot's plan and its right is (cos a, sin a): a = 0 faces the street.
	const float c = std::cos(f.angle), s = std::sin(f.angle);
	const Vector3 right = pl.lot.e0 * c + pl.lot.e1 * s, front = pl.lot.e0 * s - pl.lot.e1 * c;
	const Vector3 origin = pl.lot.at(f.x, f.y, base);
	const auto &model = look.models[f.kind];
	RowDraw row;
	for (size_t index = 0; index < model.size(); index++) {
		const FurniturePart &p = model[index];
		const bool varied = p.vary[0] > 0 || p.vary[1] > 0;
		if (varied) row.draw(p, hash_combine(seed, uint32_t(index)));
		for (int copy = 0; copy < p.copies; copy++) {
			float size[3], at[3];
			// a varied copy stands in the middle of its share of the row: its slot starts `start` steps along and is `share` long
			const float along = varied ? row.start[copy] + row.share[copy] * 0.5f - 0.5f : float(copy);
			for (int k = 0; k < 3; k++) {
				size[k] = p.size[k] + p.grow[k] * float(copy);
				at[k] = p.at[k] + p.step[k] * along;
				if (varied && p.step[k] != 0) size[k] *= row.share[copy];
			}
			if (varied) size[1] *= row.tall[copy];
			const Paint &paint = look.palette[varied ? row.colour[copy] : p.colours[size_t(copy) % p.colours.size()]];
			for (int turn = 0; turn < p.spin; turn++) {
				const float a = TAU_F * float(turn) / float(p.spin), ca = std::cos(a), sa = std::sin(a);
				const Basis g{ origin, right * ca - front * sa, pl.lot.e2, right * sa + front * ca };
				if (p.shape == PART_BOX) piece.box(g, p, size, at, paint);
				else if (p.shape == PART_CYL) piece.cylinder(g, p, size, at, paint);
				else piece.ball(g, p, size, at, paint);
			}
		}
	}
}

} // namespace city::b40
