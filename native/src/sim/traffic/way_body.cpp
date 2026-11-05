// The body against a yard's field (way_body.h): the search's discs, and the cell-by-cell fit of the ways found.
#include "sim/traffic/way_body.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr double HALF = 0.5, ROOT_HALF = 0.70710678118654752;
constexpr double DISC_STEP = 1, DISC_SPREAD = 1.9; // two discs this far apart, in chord lengths, still cover the side between them
constexpr double QUARTER_STEP = 0.25; // a corner of the body is tested in squares about this wide: 5 cm past the flank at most
constexpr double FIT_SQUARE = 0.5; // the squares body_fits reads the clearance in

// The squares of a corner that reach out of the end disc at `end`: `tip` the body's end there, `side` which flank.
void corner_squares(Discs &d, double hw, double end, double tip, double side, std::vector<BodyPoint> &out) {
	const int n = std::max(1, int(std::ceil(hw / QUARTER_STEP)));
	const double w = hw / n, inward = tip > end ? -1 : 1;
	for (int a = 0; a < n; a++)
		for (int b = 0; b < n; b++) {
			const double u = tip + inward * (a + HALF) * w, v = side * (hw - (b + HALF) * w);
			if (std::hypot(std::fabs(u - end) + w * HALF, std::fabs(v) + w * HALF) > d.r) out.push_back({ u, v });
		}
	d.square_r = w * ROOT_HALF + (d.r - hw);
}

} // namespace

Discs discs_of(const Steered &b) {
	Discs d;
	const double back = b.length * HALF - b.rear, front = b.length * HALF + b.rear, hw = b.width * HALF;
	const double u0 = -back + hw, u1 = front - hw;
	d.r = hw + b.margin;
	const double spacing = std::min(DISC_STEP, DISC_SPREAD * std::sqrt(d.r * d.r - hw * hw));
	const int n = std::max(1, int(std::ceil((u1 - u0) / spacing))) + 1;
	for (int i = 0; i < n; i++) d.at.push_back(u0 + (u1 - u0) * i / std::max(1, n - 1));
	const double us[2] = { -back + hw * HALF, front - hw * HALF }, vs[2] = { -hw * HALF, hw * HALF };
	for (int k = 0; k < 4; k++) {
		d.cu[k] = us[k / 2];
		d.cv[k] = vs[k % 2];
		corner_squares(d, hw, k / 2 ? u1 : u0, k / 2 ? front : -back, k % 2 ? 1 : -1, d.squares[k]);
	}
	d.corner_r = ROOT_HALF * hw + b.margin;
	d.centre = b.rear;
	d.whole = std::hypot(b.length * HALF, hw) + b.margin;
	return d;
}

bool discs_clear(const Field &f, const Discs &d, double x, double z, double c, double s, int64_t *looks) {
	int64_t read = 0;
	struct Count { int64_t *to; const int64_t &n; ~Count() { if (to) *to += n; } } count{ looks, read };
	const double slack = f.cell * ROOT_HALF; // the field holds the distance at a cell's centre
	read++;
	if (f.clear_at(x + c * d.centre, z + s * d.centre) - slack >= d.whole) return true;
	for (double u : d.at) {
		read++;
		if (f.clear_at(x + c * u, z + s * u) - slack < d.r) return false;
	}
	for (int k = 0; k < 4; k++) {
		read++;
		if (f.clear_at(x + c * d.cu[k] - s * d.cv[k], z + s * d.cu[k] + c * d.cv[k]) - slack >= d.corner_r) continue;
		for (const BodyPoint &q : d.squares[k]) {
			read++;
			if (f.clear_at(x + c * q.u - s * q.v, z + s * q.u + c * q.v) - slack < d.square_r) return false;
		}
	}
	return true;
}

bool body_clear(const Field &f, const Steered &b, const AxlePose &p) {
	return discs_clear(f, discs_of(b), p.x, p.z, std::cos(p.angle), std::sin(p.angle));
}

// In squares: one the clearance says is free costs a look; only those near something solid are read cell by cell.
bool body_fits(const Field &f, const Steered &b, const AxlePose &p, double margin) {
	const double c = std::cos(p.angle), s = std::sin(p.angle), step = f.cell * HALF, slack = f.cell * ROOT_HALF;
	const double back = b.length * HALF - b.rear + margin, front = b.length * HALF + b.rear + margin, hw = b.width * HALF + margin;
	const int nu = int(std::ceil((back + front) / FIT_SQUARE)), nv = int(std::ceil(2 * hw / FIT_SQUARE));
	const double du = (back + front) / nu, dv = 2 * hw / nv, room = std::hypot(du, dv) * HALF + slack;
	for (int a = 0; a < nu; a++)
		for (int d = 0; d < nv; d++) {
			const double u0 = -back + a * du, v0 = -hw + d * dv, cu = u0 + du * HALF, cv = v0 + dv * HALF;
			if (f.clear_at(p.x + c * cu - s * cv, p.z + s * cu + c * cv) >= room) continue;
			for (double u = u0; u <= u0 + du; u += step)
				for (double v = v0; v <= v0 + dv; v += step)
					if (f.solid_at(p.x + c * u - s * v, p.z + s * u + c * v)) return false;
		}
	return true;
}

} // namespace traffic
} // namespace city
