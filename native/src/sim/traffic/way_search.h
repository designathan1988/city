// A long vehicle's way through a tight yard: the hybrid A* of Dolgov, Thrun, Montemerlo and Diebel ("Practical search techniques
// in path planning for autonomous driving", 2008), on a grid of what is solid and its Euclidean clearance. Pure geometry, done a
// little at a time: whoever runs it hands out work, counted and never timed, so a map gives the same ways on the same tick.
#pragma once

#include "core/math.h"
#include "sim/traffic/reeds_shepp.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <queue>
#include <utility>
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
// The cells of a grid a unit of work pays for; a search's pose expanded is worth a few of those units (way_search.cpp).
constexpr int CELLS_PER_WORK = 64;

// The clearance of what is blocked, a line of the grid at a time: an exact distance transform (Felzenszwalb and Huttenlocher, 2012).
class ClearanceFill {
public:
	explicit ClearanceFill(Field &f);
	bool advance(int &work); // true once `f.clear` is whole

private:
	Field &f_;
	int stage_ = 0, line_ = 0;
	std::vector<double> sq_, in_, out_, edge_;
	std::vector<int> v_;
};

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

// The rear axle's shortest distance to the goal from every search cell, round what is solid, a cell at a time.
struct Holonomic {
	using Item = std::pair<double, int>;
	int nx = 0, nz = 0;
	double cell = 0, half = 0;
	std::vector<double> dist;
	std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open;
	void begin(const Field &f, const AxlePose &goal, double body_half, double size);
	void advance(const Field &f, int &work);
};

struct WayGoal {
	AxlePose pose;
	bool reversing = false; // reached backing, else driving forward
	double reach = 0, turn = 0; // how near its place and heading a pose must come
};

// The poses from `start` to near the goal, none when there is no way or none within the search's limit. The field and the body are
// read as it goes, so they outlive it. `fine`: at half the grain, for the tight yard the usual search loses; some eight times the work.
class WaySearch {
public:
	WaySearch(const Field &f, const Steered &b, const AxlePose &start, const WayGoal &goal, bool fine = false);
	~WaySearch();
	bool advance(int &work); // spends `work`; true once it is over
	bool found() const;
	const std::vector<AxlePose> &way() const;

private:
	class Run;
	std::unique_ptr<Run> run_;
};

} // namespace traffic
} // namespace city
