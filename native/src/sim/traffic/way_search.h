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

// What the steps of a yard's planning cost, in units of work. A unit is the time of writing CELLS_PER_WORK cells of a grid: 0.048 µs
// measured on 2026-11-05 on a bench of the planner since removed (git keeps it before commit 18782f55), a row of a yard's street cells. Each
// step is charged what it measured there in that unit, so a tick's budget bounds its time; the work is still counted, never timed: the same map gives the same plan on the
// same tick. Nothing a step does is left uncharged, and no step is bigger than a few of them: a yard's grid, its clearance and a
// distance map are laid a row at a time.
constexpr int CELLS_PER_WORK = 64;
constexpr int WRITE8_WORK_SHARE = 4; // a cell first written as a double (the clearance's start, a new distance map): 2.8 ns a cell
constexpr int LINE_WORK_SHARE = 5; // a line of the distance transform, per CELLS_PER_WORK cells: 0.225 µs
constexpr int EDGES_PER_WORK = 3; // the edges of a ring a row of its fill tests (RingFill): at most 0.0154 µs an edge, the bench's
                                  // 1,577,932 edges given every row's whole time (24.3 ms in 394,483 rows)
constexpr int HOLONOMIC_CELL_WORK = 2; // a cell of a distance map settled: 0.085 µs

// What is solid on a grid, world XZ: 1 for a cell a body may not touch. `clear`: metres from each cell's centre to the nearest solid.
// Laid a row at a time (begin, then lay_row until whole), as the planner's slices pay for every row they write.
struct Field {
	double x0 = 0, z0 = 0, cell = 0;
	int nx = 0, nz = 0;
	std::vector<uint8_t> blocked;
	std::vector<float> clear;
	void begin(double x, double z, double size, int cells_x, int cells_z); // its frame and its room, no cell yet
	bool lay_row(uint8_t value); // one more row of `value`; true once whole
	bool whole() const { return blocked.size() == size_t(nx) * nz; }
	float clear_at(double x, double z) const; // less than nothing off the grid
	bool solid_at(double x, double z) const; // off the grid is solid
};

// A polygon set into a grid a row at a time, the cells whose centre lies inside it: each row costs the edges it tests and the cells it
// writes (a carriageway's ring has thousands of edges: what it costs is rows times edges, not the cells of its box).
class RingFill {
public:
	RingFill(const Field &f, const std::vector<V2> *ring, uint8_t value);
	bool advance(Field &f, int &work); // true once every row is set

private:
	const std::vector<V2> *ring_;
	uint8_t value_;
	int row_ = 0, last_ = -1;
	std::vector<double> xs_;
};

// The clearance of what is blocked, a line of the grid at a time: an exact distance transform (Felzenszwalb and Huttenlocher, 2012).
// Its squared distances and the clearance are laid a row at a time too, before the columns.
class ClearanceFill {
public:
	explicit ClearanceFill(Field &f);
	bool advance(int &work); // true once `f.clear` is whole

private:
	Field &f_;
	int stage_ = 0, line_ = 0; // 0 laying out the squared distances, 1 the columns, 2 the rows
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

// The grain a search runs at. COARSE: half a metre and 5 degrees, the 4.0's and Dolgov et al.'s; it finds most yards' ways fast, and
// keeps one pose to a cell, so a tight yard can be lost. GRADUATED (Pivtoraiko and Kelly 2008, as González-Sieira et al. 2019 adapt it
// to the obstacles): the fine grain near the start and the goal, where a lorry is fitted to a dock or a lane, and elsewhere each
// motion the longest that does not meet anything, the coarse step or, where that one touches, the fine one; it finds a tight yard's
// way with some half the poses of FINE (the yard planner's bench). FINE: a quarter of a metre and 2.5 degrees everywhere.
enum class WayGrain { COARSE, GRADUATED, FINE };

// The rear axle's shortest distance to the goal from every search cell, round what is solid, a cell at a time: Dolgov et al.'s
// holonomic-with-obstacles heuristic, dynamic programming in 2D from the goal. It depends on the field and the goal's cell only, so
// every search to that cell at the same grain on the same field reads one (holonomic_key): the searches to a dock from each of its
// gates, and to a gate from each dock. A search reads the map at its own grain: the coarse one at the coarse cell, the others at the fine.
struct Holonomic {
	using Item = std::pair<double, int>;
	int nx = 0, nz = 0;
	double cell = 0, half = 0;
	bool begun = false, seeded = false;
	int goal_cell = -1; // -1: the goal is off the grid, and every cell stays out of reach
	std::vector<double> dist;
	std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open;
	void begin(const Field &f, const AxlePose &goal, double body_half, double size); // its size and its goal, no cell yet
	void advance(const Field &f, int &work); // its cells laid, then settled
	bool done() const { return seeded && open.empty(); }
};

// The key of the distance map a search at `grain` to `goal` on `f` reads: the grain of its map and its goal's cell there.
int64_t holonomic_key(const Field &f, const AxlePose &goal, WayGrain grain);

struct WayGoal {
	AxlePose pose;
	bool reversing = false; // reached backing, else driving forward
	double reach = 0, turn = 0; // how near its place and heading a pose must come
};

// What a search spent: the cells of its distance map settled, the poses it expanded, and the time of each. Probes, never read by
// the simulation (the work handed out is what paces it).
// `looks`, `rs`, `queued`, `pages`: the clearance cells its body tests read, the Reeds-Shepp lengths it worked out, the poses it
// queued, the pages of headings its tables of poses took (one for every place it first stood at).
struct SearchCost {
	int64_t holonomic = 0, expanded = 0, looks = 0, rs = 0, queued = 0, pages = 0;
	double holonomic_ms = 0, expand_ms = 0;
	void add(const SearchCost &o) {
		holonomic += o.holonomic;
		expanded += o.expanded;
		looks += o.looks;
		rs += o.rs;
		queued += o.queued;
		pages += o.pages;
		holonomic_ms += o.holonomic_ms;
		expand_ms += o.expand_ms;
	}
};


// The memory a yard's searches use one after another, the planner's: their tables of poses, their poses and their queue. A search's
// first writes to megabytes of new memory were most of what starting one cost (the planner's bench: 0.17 ms, up to 2.1 ms).
class WaySpace {
public:
	WaySpace();
	~WaySpace();
	struct Room;
	std::unique_ptr<Room> room;
};

// The poses from `start` to near the goal, none when there is no way or none within the search's limit. The field, the body and the
// goal's distance map (`map`: empty, or another search's to the same holonomic_key) and the memory (`space`, one search at a time)
// are read as it goes, so they outlive it.
class WaySearch {
public:
	WaySearch(const Field &f, const Steered &b, const AxlePose &start, const WayGoal &goal, std::shared_ptr<Holonomic> map, WayGrain grain,
			WaySpace &space);
	~WaySearch();
	bool advance(int &work); // spends `work`; true once it is over
	bool found() const;
	const std::vector<AxlePose> &way() const;
	const SearchCost &cost() const;

private:
	class Run;
	std::unique_ptr<Run> run_;
};

} // namespace traffic
} // namespace city
