// Node shape shared by drawing, lots and traffic (Roadcraft src/roads/junction.ts): the arm of each segment end,
// each arm's setback (where the segment's mouth is), kerb-return fillets and dead-end turning bulbs.
#pragma once

#include "sim/roads/geometry.h"
#include "sim/roads/profiles.h"

#include <vector>

namespace city {

constexpr double BULB_R = 10, BULB_WALK = 2.5; // turning bulb: asphalt radius and footway width
constexpr double KERB_RADIUS_MAX = 7.2; // largest kerb return radius
constexpr double MOUTH_SHARE = 0.45; // the most of a road one of its mouths may take

Arm arm_of(const Quad &q, const Profile &p, int end);
// Setback of a dead end with a bulb (0 = no bulb: wide road or short segment).
double bulb_retreat(const Arm &arm, double len);
// Setback of each arm (input order); `order` returns the arms sorted anticlockwise.
void junction_retreats(const std::vector<Arm> &arms, const std::vector<double> &lens, std::vector<double> &out,
		std::vector<int> &order, bool bulb = false);

enum class CornerMode : uint8_t { NONE, FILLET, BEVEL, TAPER };
// The corner between arm a and the next arm anticlockwise b, where a's left kerb meets b's right kerb.
struct Corner {
	CornerMode mode = CornerMode::NONE;
	double trim_a = 0, trim_b = 0; // how far along each arm the corner reaches
	double r = 0, cx = 0, cz = 0; // kerb return (fillet only), centre relative to the node
};
Corner corner_fillet(const Arm &a, const Arm &b);

} // namespace city
