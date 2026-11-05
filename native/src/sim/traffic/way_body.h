// The body as the search tests it against a yard's field (way_search.cpp): discs along its axis and at its corners, read against the
// field's clearance. Internal to the yard's ways; body_clear and body_fits are way_search.h's.
#pragma once

#include "sim/traffic/way_search.h"

#include <cstdint>
#include <vector>

namespace city {
namespace traffic {

struct BodyPoint {
	double u = 0, v = 0;
};

// Discs along the body's axis as wide as it and a little: they hold the flanks. The four corners the end discs leave out: one wide
// disc each, which answers in the open but stands a fifth of the half width past the flank, so a lorry beside a kerb would not fit
// its own lane; when it touches, the small squares of that corner say.
struct Discs {
	std::vector<double> at;
	double r = 0, corner_r = 0, centre = 0, whole = 0, square_r = 0;
	double cu[4] = {}, cv[4] = {};
	std::vector<BodyPoint> squares[4];
};

Discs discs_of(const Steered &b);
// Whether the body's discs at (x, z), its heading's cosine and sine, touch nothing; `looks`, when given, counts the cells of the
// clearance it read (one in the open, a disc's or a corner's each nearer something solid).
bool discs_clear(const Field &f, const Discs &d, double x, double z, double c, double s, int64_t *looks = nullptr);

} // namespace traffic
} // namespace city
