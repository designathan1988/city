// Looks of bodies (see likeness.h). The weights are the 4.0's: a different form counts most, then a whole colour away, then the height.
#include "sim/buildings/likeness.h"

#include "core/random.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr float FORM_APART = 1.5f, ROOF_APART = 0.7f, STOREYS_APART = 0.8f, STOREYS_SPAN = 2.f;
constexpr float COLOUR_SPAN = 90.f; // of the three 8-bit channels, the distance that is a whole colour away
constexpr float DISTANCE_WEIGHT = 2.5f; // what a neighbour LOOK_REACH_M off adds to how unlike it is
constexpr float FRONT_ROW_M = 1.f, WHOLE_SHARE = 0.8f, COLUMNS = 2.f, HALF = 0.5f;
constexpr float NOTHING_WIDE = 1e-6f;
constexpr uint32_t RED_SHIFT = 16, GREEN_SHIFT = 8, CHANNEL = 255;
constexpr uint32_t FORM_SALT = 0xf0a3U, ROOF_SALT = 0x400fU, FOLD_SHIFT = 32;

// A volume where it stands in the body, as the eye reads it: its column, front or back, whether it spans the body, its floors and roof.
uint32_t token(const b40::Volume &v, float x0, float y0, float width) {
	uint32_t t = uint32_t(std::lround((v.x + v.w * HALF - x0) / width * COLUMNS));
	t = hash_combine(t, v.y - y0 < FRONT_ROW_M ? 1U : 0U);
	t = hash_combine(t, v.w > width * WHOLE_SHARE ? 1U : 0U);
	t = hash_combine(t, uint32_t(v.base));
	t = hash_combine(t, uint32_t(v.storeys));
	return hash_combine(t, uint32_t(v.roof));
}

float channel_gap(uint32_t a, uint32_t b, uint32_t shift) { return float(int((a >> shift) & CHANNEL) - int((b >> shift) & CHANNEL)); }
} // namespace

BodyLook body_look(const b40::Body &b, int fn) {
	BodyLook look;
	look.form = hash_combine(FORM_SALT, hash_combine(uint32_t(fn), uint32_t(b.use)));
	if (b.volumes.empty()) return look;
	float x0 = b.volumes[0].x, x1 = x0, y0 = b.volumes[0].y;
	const b40::Volume *main = &b.volumes[0];
	for (const b40::Volume &v : b.volumes) {
		x0 = std::min(x0, v.x);
		x1 = std::max(x1, v.x + v.w);
		y0 = std::min(y0, v.y);
		look.storeys = std::max(look.storeys, v.top());
		if (v.w * v.d > main->w * main->d) main = &v;
	}
	const float width = std::max(NOTHING_WIDE, x1 - x0);
	std::vector<uint32_t> tokens;
	tokens.reserve(b.volumes.size());
	for (const b40::Volume &v : b.volumes) tokens.push_back(token(v, x0, y0, width));
	std::sort(tokens.begin(), tokens.end());
	uint32_t form = uint32_t(look.form);
	for (uint32_t t : tokens) form = hash_combine(form, t);
	look.form = (look.form << FOLD_SHIFT) | form;
	const int wall = main->wall >= 0 ? main->wall : b.wall, roof = main->roof_paint >= 0 ? main->roof_paint : b.roof;
	look.colour = wall >= 0 ? b.paints[size_t(wall)].rgb : 0;
	uint32_t top = hash_combine(ROOF_SALT, uint32_t(main->roof));
	if (roof >= 0) top = hash_combine(hash_combine(top, uint32_t(b.paints[size_t(roof)].finish)), b.paints[size_t(roof)].rgb);
	look.roof = top;
	return look;
}

float look_distance(const BodyLook &a, const BodyLook &b) {
	const float colour = std::min(1.f, std::hypot(channel_gap(a.colour, b.colour, RED_SHIFT), channel_gap(a.colour, b.colour, GREEN_SHIFT), channel_gap(a.colour, b.colour, 0)) / COLOUR_SPAN);
	return (a.form == b.form ? 0.f : FORM_APART) + colour + (a.roof == b.roof ? 0.f : ROOF_APART) +
			std::min(1.f, float(std::abs(a.storeys - b.storeys)) / STOREYS_SPAN) * STOREYS_APART;
}

float unlikeness(const BodyLook &mine, const std::vector<NearLook> &near) {
	float least = std::numeric_limits<float>::max();
	for (const NearLook &n : near) least = std::min(least, look_distance(mine, n.look) + DISTANCE_WEIGHT * n.distance / LOOK_REACH_M);
	return least;
}

} // namespace city
