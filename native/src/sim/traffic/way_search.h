// A long vehicle's way through a tight yard: the hybrid A* of Dolgov, Thrun, Montemerlo and Diebel ("Practical search techniques
// in path planning for autonomous driving", 2008), on a grid of what is solid and its Euclidean clearance. Pure geometry: it runs
// on a worker with its own copies, so it may take as long as a dock's ways take.
#pragma once

#include "core/math.h"
#include "sim/traffic/reeds_shepp.h"

#include <cstdint>
#include <vector>

namespace city {
namespace traffic {

// What is solid on a grid, world XZ: 1 for a cell a body may not touch. `clear`: metres from each cell's centre to the nearest solid.
struct Field {
	double x0 = 0, z0 = 0, cell = 0;
	int nx = 0, nz = 0;
	std::vector<uint8_t> blocked;
	std::vector<float> clear;
	void resize(double x, double z, double size, int cells_x, int cells_z, uint8_t value);
	float clear_at(double x, double z) const; // less than nothing off the grid
	bool solid_at(double x, double z) const; // off the grid is solid
};

// Sets every cell whose centre lies inside the polygon.
void fill_ring(Field &f, const std::vector<V2> &ring, uint8_t value);
// The clearance of what is blocked: an exact distance transform (Felzenszwalb and Huttenlocher, 2012).
void fill_clearance(Field &f);

// A vehicle as the planner sees it: its rear axle is what moves.
struct Steered {
	double length = 0, width = 0;
	double rear = 0; // how far the body's centre is ahead of the rear axle
	double radius = 0; // the least radius the rear axle turns at
	double margin = 0; // kept clear of anything solid, besides the body itself
};

// Whether the body's rectangle at the pose covers no solid cell, `margin` grown.
bool body_fits(const Field &f, const Steered &b, const AxlePose &p, double margin);
// Whether the body's discs at the pose touch nothing: the quick test of the search.
bool body_clear(const Field &f, const Steered &b, const AxlePose &p);

struct WayGoal {
	AxlePose pose;
	bool reversing = false; // reached backing, else driving forward
	double reach = 0, turn = 0; // how near its place and heading a pose must come
};

// The poses from `start` to near the goal; false when there is no way, or none within the search's limit. `fine`: at half the grain,
// for the tight yard the usual search loses; some eight times the work.
bool find_way(const Field &f, const Steered &b, const AxlePose &start, const WayGoal &goal, std::vector<AxlePose> &way, bool fine = false);

} // namespace traffic
} // namespace city
