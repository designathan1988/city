// The body against a yard's field (way_body.h): the search's discs, and the cell-by-cell fit of the ways found.
#include "sim/traffic/way_body.h"

#include <immintrin.h>

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {
namespace traffic {

namespace {
constexpr double HALF = 0.5, ROOT_HALF = 0.70710678118654752;
constexpr double DISC_STEP = 1, DISC_SPREAD = 1.9; // two discs this far apart, in chord lengths, still cover the side between them
constexpr double QUARTER_STEP = 0.25; // a corner of the body is tested in squares about this wide: 5 cm past the flank at most
constexpr double FIT_SQUARE = 0.5; // the squares body_fits reads the clearance in

// The squares of a corner that reach out of the end disc at `end`: `tip` the body's end there, `side` which flank.
void corner_squares(Discs &d, double hw, double end, double tip, double side, int corner) {
	const int n = std::max(1, int(std::ceil(hw / QUARTER_STEP)));
	const double w = hw / n, inward = tip > end ? -1 : 1;
	for (int a = 0; a < n; a++)
		for (int b = 0; b < n; b++) {
			const double u = tip + inward * (a + HALF) * w, v = side * (hw - (b + HALF) * w);
			if (std::hypot(std::fabs(u - end) + w * HALF, std::fabs(v) + w * HALF) <= d.r) continue;
			d.square_u[corner].push_back(u);
			d.square_v[corner].push_back(v);
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
		corner_squares(d, hw, k / 2 ? u1 : u0, k / 2 ? front : -back, k % 2 ? 1 : -1, k);
	}
	d.corner_r = ROOT_HALF * hw + b.margin;
	d.centre = b.rear;
	d.whole = std::hypot(b.length * HALF, hw) + b.margin;
	return d;
}

namespace {
constexpr int LANES = 4;
const float NOTHING_CLEAR = -std::numeric_limits<float>::infinity();

// Four reads of the clearance at once: the hottest loop of the core, the search's. The sum, the floor and the bounds are Field::clear_at's, in the same order, so the
// answers are the same bit for bit; the cells are fetched together (vgatherdps), where one at a time each waits for the last and for the branch on it.
struct Lanes {
	__m256d x0, z0, cell;
	__m128i nx, nz;
	const float *clear;
	explicit Lanes(const Field &f) :
			x0(_mm256_set1_pd(f.x0)), z0(_mm256_set1_pd(f.z0)), cell(_mm256_set1_pd(f.cell)), nx(_mm_set1_epi32(f.nx)), nz(_mm_set1_epi32(f.nz)), clear(f.clear.data()) {}
	__m256d read(__m256d x, __m256d z) const {
		const __m128i i = _mm256_cvttpd_epi32(_mm256_floor_pd(_mm256_div_pd(_mm256_sub_pd(x, x0), cell)));
		const __m128i j = _mm256_cvttpd_epi32(_mm256_floor_pd(_mm256_div_pd(_mm256_sub_pd(z, z0), cell)));
		const __m128i none = _mm_set1_epi32(-1);
		const __m128i inside = _mm_and_si128(_mm_and_si128(_mm_cmpgt_epi32(i, none), _mm_cmpgt_epi32(j, none)), _mm_and_si128(_mm_cmpgt_epi32(nx, i), _mm_cmpgt_epi32(nz, j)));
		const __m128i at = _mm_add_epi32(_mm_mullo_epi32(j, nx), i);
		return _mm256_cvtps_pd(_mm_mask_i32gather_ps(_mm_set1_ps(NOTHING_CLEAR), clear, at, _mm_castsi128_ps(inside), sizeof(float)));
	}
};

// The first of four below `least`, or LANES.
int first_under(__m256d value, double least) {
	const int under = _mm256_movemask_pd(_mm256_cmp_pd(value, _mm256_set1_pd(least), _CMP_LT_OQ));
	return under == 0 ? LANES : __builtin_ctz(unsigned(under));
}
} // namespace

bool discs_clear(const Field &f, const Discs &d, double x, double z, double c, double s, int64_t *looks) {
	int64_t read = 0;
	struct Count { int64_t *to; const int64_t &n; ~Count() { if (to) *to += n; } } count{ looks, read };
	const double slack = f.cell * ROOT_HALF; // the field holds the distance at a cell's centre
	read++;
	if (f.clear_at(x + c * d.centre, z + s * d.centre) - slack >= d.whole) return true;
	const Lanes lanes(f);
	const __m256d vx = _mm256_set1_pd(x), vz = _mm256_set1_pd(z), vc = _mm256_set1_pd(c), vs = _mm256_set1_pd(s), vslack = _mm256_set1_pd(slack);
	const size_t discs = d.at.size();
	size_t k = 0;
	for (; k + LANES <= discs; k += LANES) {
		const __m256d u = _mm256_loadu_pd(d.at.data() + k);
		const __m256d room = _mm256_sub_pd(lanes.read(_mm256_add_pd(vx, _mm256_mul_pd(vc, u)), _mm256_add_pd(vz, _mm256_mul_pd(vs, u))), vslack);
		const int under = first_under(room, d.r);
		read += std::min(under + 1, LANES);
		if (under < LANES) return false;
	}
	for (; k < discs; k++) {
		const double u = d.at[k];
		read++;
		if (f.clear_at(x + c * u, z + s * u) - slack < d.r) return false;
	}
	for (int corner = 0; corner < LANES; corner++) {
		read++;
		if (f.clear_at(x + c * d.cu[corner] - s * d.cv[corner], z + s * d.cu[corner] + c * d.cv[corner]) - slack >= d.corner_r) continue;
		const std::vector<double> &us = d.square_u[corner], &vs_of = d.square_v[corner];
		const size_t squares = us.size();
		size_t q = 0;
		for (; q + LANES <= squares; q += LANES) {
			const __m256d u = _mm256_loadu_pd(us.data() + q), v = _mm256_loadu_pd(vs_of.data() + q);
			const __m256d px = _mm256_sub_pd(_mm256_add_pd(vx, _mm256_mul_pd(vc, u)), _mm256_mul_pd(vs, v));
			const __m256d pz = _mm256_add_pd(_mm256_add_pd(vz, _mm256_mul_pd(vs, u)), _mm256_mul_pd(vc, v));
			const int under = first_under(_mm256_sub_pd(lanes.read(px, pz), vslack), d.square_r);
			read += std::min(under + 1, LANES);
			if (under < LANES) return false;
		}
		for (; q < squares; q++) {
			read++;
			if (f.clear_at(x + c * us[q] - s * vs_of[q], z + s * us[q] + c * vs_of[q]) - slack < d.square_r) return false;
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
