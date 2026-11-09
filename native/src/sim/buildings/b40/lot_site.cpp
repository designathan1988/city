// The site a grown building stands on, read from the terrain (lot_site.h).
#include "sim/buildings/b40/lot_site.h"

#include "sim/buildings/b40/body_table.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f; // a front point stands in the middle of its share of the front
} // namespace

float LotSite::ground_at(float x, float y, float w, float d) const {
	const float fx = std::clamp((x + w / 2) / std::max(w, EPS) * float(SITE_COLS - 1), 0.f, float(SITE_COLS - 1));
	const float fy = std::clamp(y / std::max(d, EPS) * float(SITE_ROWS - 1), 0.f, float(SITE_ROWS - 1));
	const int i = std::min(int(fx), SITE_COLS - 2), j = std::min(int(fy), SITE_ROWS - 2);
	const float tx = fx - float(i), ty = fy - float(j);
	auto at = [&](int a, int b) { return float(ground[size_t(b * SITE_COLS + a)]) * SITE_QUANTUM_M; };
	return lerpf(lerpf(at(i, j), at(i + 1, j), tx), lerpf(at(i, j + 1), at(i + 1, j + 1), tx), ty);
}

void LotSite::seal() {
	sloped = std::any_of(ground.begin(), ground.end(), [](int16_t g) { return g != 0; });
	uint64_t h = 0;
	for (size_t k = 0; k < ground.size(); k++) h = BodyKeyHash::mix(h, uint64_t(uint16_t(ground[k])) | (uint64_t(k) << BodyKeyHash::HALF_BITS));
	for (const V2 &p : ring) {
		uint32_t x = 0, y = 0;
		std::memcpy(&x, &p.x, sizeof x);
		std::memcpy(&y, &p.y, sizeof y);
		h = BodyKeyHash::mix(h, uint64_t(x) | (uint64_t(y) << BodyKeyHash::HALF_BITS));
	}
	hash = h | 1; // never 0: 0 is a record with no site
}

SiteRef read_site(V2 o, V2 e0, V2 e1, float w, float d, const std::function<float(float, float)> &height, float threshold,
		const std::vector<V2> &ring_world, int front_points) {
	auto s = std::make_shared<LotSite>();
	float best = -std::numeric_limits<float>::infinity();
	for (int i = 0; i < front_points; i++) {
		const V2 p = o + e0 * (-w / 2 + w * (float(i) + HALF) / float(front_points));
		best = std::max(best, height(p.x, p.y));
	}
	s->floor = best + threshold;
	constexpr float LIMIT = float(std::numeric_limits<int16_t>::max());
	for (int j = 0; j < SITE_ROWS; j++)
		for (int i = 0; i < SITE_COLS; i++) {
			const V2 p = o + e0 * (-w / 2 + w * float(i) / float(SITE_COLS - 1)) + e1 * (d * float(j) / float(SITE_ROWS - 1));
			const float rel = std::clamp((height(p.x, p.y) - s->floor) / SITE_QUANTUM_M, -LIMIT, LIMIT);
			s->ground[size_t(j * SITE_COLS + i)] = int16_t(std::lround(rel));
		}
	for (const V2 &c : ring_world) {
		const V2 r = c - o;
		s->ring.push_back(V2(r.x * e0.x + r.y * e0.y, r.x * e1.x + r.y * e1.y));
	}
	s->seal();
	return s;
}

} // namespace city::b40
