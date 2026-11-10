// The shortest way between two poses for a vehicle that turns no tighter than a radius and drives forward and in reverse
// (Reeds and Shepp, "Optimal paths for a car that goes both forwards and backwards", Pacific J. Math. 145, 1990). Pure geometry.
#pragma once

#include <vector>

namespace city {
namespace traffic {

// A rear axle's pose in the XZ plane and whether it is reached backing.
struct AxlePose {
	double x = 0, z = 0, angle = 0;
	bool reverse = false;
};

constexpr int RS_MOST_PIECES = 5;

struct RsPath {
	const char *pieces = ""; // L, R and S, one per length
	int count = 0;
	double lengths[RS_MOST_PIECES] = {}; // signed (negative is backing), on a unit radius
	double total = 0;
};

// The shortest way from `a` to `b`; false when there is none.
bool reeds_shepp(const AxlePose &a, const AxlePose &b, double radius, RsPath &out);
// Its length in metres, a huge number when there is none: what the search uses to guide itself.
double reeds_shepp_length(const AxlePose &a, const AxlePose &b, double radius);
// Poses along `path` from `a`, one every `step` metres.
void sample_reeds_shepp(const RsPath &path, const AxlePose &a, double radius, double step, std::vector<AxlePose> &out);

} // namespace traffic
} // namespace city
