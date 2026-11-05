// The HSL nudge of roof and second-skin colours: three draws, hue, saturation, lightness.
#include "sim/buildings/b40/paint.h"

namespace city::b40 {

namespace {
constexpr double HUE_STEP = 0.03, SAT_STEP = 0.1, LIGHT_STEP = 0.08, LIGHT_MIN = 0.05, LIGHT_MAX = 0.96;
constexpr double SIXTH = 1.0 / 6, THIRD = 1.0 / 3, HALF = 0.5, TWO_THIRDS = 2.0 / 3, HUE_SECTORS = 6;

double hue_of(double r, double g, double b, double mx, double d) {
	double h = mx == r ? (g - b) / d + (g < b ? HUE_SECTORS : 0) : mx == g ? (b - r) / d + 2 : (r - g) / d + 4;
	return h / HUE_SECTORS;
}
} // namespace

uint32_t nudge(uint32_t c, Xoshiro128 &rng, float amount) {
	double r = ch(c, CH_R) / CH_MAX, g = ch(c, CH_G) / CH_MAX, b = ch(c, 0) / CH_MAX;
	double mx = std::max({ r, g, b }), mn = std::min({ r, g, b });
	double h = 0, s = 0, l = (mx + mn) / 2;
	if (mx != mn) {
		double d = mx - mn;
		s = l > HALF ? d / (2 - mx - mn) : d / (mx + mn);
		h = hue_of(r, g, b, mx, d);
	}
	double h2 = std::fmod(h + (rng.f() - HALF) * HUE_STEP * amount + 1, 1.0);
	double s2 = std::clamp(s + (rng.f() - HALF) * SAT_STEP * amount, 0.0, 1.0);
	double l2 = std::clamp(l + (rng.f() - HALF) * LIGHT_STEP * amount, LIGHT_MIN, LIGHT_MAX);
	double q = l2 < HALF ? l2 * (1 + s2) : l2 + s2 - l2 * s2, p = 2 * l2 - q;
	auto chan = [&](double t) {
		double u = std::fmod(t + 1, 1.0);
		double v = u < SIXTH ? p + (q - p) * HUE_SECTORS * u : u < HALF ? q : u < TWO_THIRDS ? p + (q - p) * (TWO_THIRDS - u) * HUE_SECTORS : p;
		return float(std::clamp(v, 0.0, 1.0) * CH_MAX);
	};
	return pack(chan(h2 + THIRD), chan(h2), chan(h2 - THIRD));
}

} // namespace city::b40
