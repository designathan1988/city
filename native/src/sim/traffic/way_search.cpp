// The search (way_search.h). Nodes are poses on a grid of place and heading; a step is an exact arc of the rear axle at one of five
// curvatures, either way, the coarse grain's or, where that one meets something, the fine grain's.
// The cost still to go is the larger of the paper's two heuristics: round the obstacles for the axle alone, and the Reeds-Shepp
// length near the goal. Near it a Reeds-Shepp shot is tried, the paper's analytic expansion.
#include "sim/traffic/way_search.h"

#include "sim/traffic/way_body.h"

#include "core/laps.h"

#include <algorithm>
#include <cmath>
#include <deque>
#include <functional>
#include <limits>
#include <queue>

namespace city {
namespace traffic {

namespace {
constexpr double PI = 3.14159265358979323846, TWO_PI = 2 * PI;
// The search's two grains (graduated fidelity, way_search.h): its cells this much across and this many headings round, and a step's
// arc a little longer than a cell's diagonal so a step always leaves its cell. Where the coarse step meets something the fine one is
// tried (a lorry boxed in by a dock's walls moves on at the fine grain, where the coarse one had stopped).
struct Grain {
	double cell = 0;
	int headings = 0;
	double step = 0;
};
constexpr int COARSE = 0, FINE = 1, GRAIN_COUNT = 2;
constexpr int COARSE_HEADINGS = 72; // a heading every 5 degrees
constexpr int FINE_HEADINGS = 144; // every 2.5
constexpr Grain GRAINS[GRAIN_COUNT] = { { 0.5, COARSE_HEADINGS, 0.8 }, { 0.25, FINE_HEADINGS, 0.4 } };
constexpr int COARSE_MOST = 80000, FINE_MOST = 1500000; // the poses a search may expand: at the coarse grain, at any finer
constexpr int SUBSTEPS = 2; // poses tested along a step
constexpr double REVERSE_COST = 1.6, SWITCH_COST = 15, STEER_COST = 0.6, STEER_CHANGE_COST = 2; // per metre; a stop and a start; the wheel
constexpr double H_WEIGHT = 1.4; // a little over one finds a way sooner, a little longer than the shortest
constexpr double SHOT_REACH = 30, SHOT_NEAR = 8; // shots from every pose this near, from one in SHOT_EVERY as far as the reach
constexpr int SHOT_EVERY = 8;
constexpr double SHOT_STEP = 0.25;
constexpr double CURVATURES[] = { -1, -0.5, 0, 0.5, 1 }; // shares of the tightest
constexpr double NO_CURVE = 1e-9, HALF = 0.5;
constexpr int EXPANDED_WORK = 8, SHOT_WORK = 8; // a pose expanded, a Reeds-Shepp shot: units of work, measured against a grid's cells

double wrap(double a) {
	double r = std::fmod(a, TWO_PI);
	if (r > PI) r -= TWO_PI;
	if (r < -PI) r += TWO_PI;
	return r;
}

struct Node {
	AxlePose pose;
	double g = 0, kappa = 0;
	int parent = -1;
	bool closed = false;
	int grain = COARSE; // the grain of the step that reached it: the table it is kept in
};

// The node standing for each place and heading: a page of headings for each place touched, in blocks that never move. A search runs
// on the simulation's tick, so nothing here grows by copying all it holds (a hash table rehashing took 78 ms of one tick).
class PoseTable {
public:
	void begin(size_t places, int headings) {
		page_of_.assign(places, -1);
		headings_ = headings;
	}
	int &at(size_t place, int heading) {
		int &page = page_of_[place];
		if (page < 0) {
			page = pages_++;
			if (size_t(page / BLOCK_PAGES) == blocks_.size()) {
				const size_t ints = size_t(BLOCK_PAGES) * headings_;
				blocks_.push_back(std::make_unique<int[]>(ints));
				std::fill_n(blocks_.back().get(), ints, -1);
			}
		}
		return blocks_[size_t(page / BLOCK_PAGES)][size_t(page % BLOCK_PAGES) * headings_ + heading];
	}

private:
	static constexpr int BLOCK_PAGES = 256;
	std::vector<int> page_of_;
	std::vector<std::unique_ptr<int[]>> blocks_;
	int headings_ = 0, pages_ = 0;
};

} // namespace

class WaySearch::Run {
public:
	Run(const Field &f, const Steered &b, const AxlePose &start, const WayGoal &goal, std::shared_ptr<Holonomic> map, WayGrain grain);
	bool advance(int &work);
	bool found = false;
	std::vector<AxlePose> way;
	SearchCost cost;

private:
	using Item = std::pair<double, int>;
	const Field &f_;
	const Steered &b_;
	const WayGoal goal_;
	const AxlePose start_;
	const WayGrain grain_;
	const int most_;
	const int map_grain_; // the grain of its distance map: the coarse one for a coarse search, the fine one for the others
	Discs discs_;
	std::shared_ptr<Holonomic> h_;
	std::deque<Node> nodes_;
	PoseTable tables_[GRAIN_COUNT];
	int nx_[GRAIN_COUNT] = {}, nz_[GRAIN_COUNT] = {};
	std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open_;
	int expanded_ = 0;
	bool over_ = false, begun_ = false;
	bool place_of(const AxlePose &p, int grain, size_t &place) const;
	int &slot_of(int grain, size_t place, const AxlePose &p);
	double h_at(size_t map_place, const AxlePose &p) const;
	void push(const Node &n);
	bool near_ends(const AxlePose &p) const;
	void expand(int id);
	bool arc(const AxlePose &from, double kappa, double dir, double step, AxlePose &to) const;
	bool shot(const AxlePose &from, std::vector<AxlePose> &out) const;
	void trace(int id);
	void close_on(int id, int &work);
};

// The cell of the rear axle at a grain, false off the grid: the body answers for its middle, so in the open the axle can stand past
// the edge.
bool WaySearch::Run::place_of(const AxlePose &p, int grain, size_t &place) const {
	const double cell = GRAINS[grain].cell;
	const int i = int(std::floor((p.x - f_.x0) / cell)), j = int(std::floor((p.z - f_.z0) / cell));
	if (i < 0 || j < 0 || i >= nx_[grain] || j >= nz_[grain]) return false;
	place = size_t(j) * nx_[grain] + i;
	return true;
}

int &WaySearch::Run::slot_of(int grain, size_t place, const AxlePose &p) {
	const int headings = GRAINS[grain].headings;
	int k = int(std::lround(wrap(p.angle) / TWO_PI * headings)) % headings;
	if (k < 0) k += headings;
	return tables_[grain].at(place, k);
}

// `map_place`: the pose's cell at the grain of the distance map.
double WaySearch::Run::h_at(size_t map_place, const AxlePose &p) const {
	const double far = std::hypot(goal_.pose.x - p.x, goal_.pose.z - p.z);
	double best = std::max(h_->dist[map_place], far);
	if (far <= SHOT_REACH) best = std::max(best, reeds_shepp_length(p, goal_.pose, b_.radius));
	return best;
}

void WaySearch::Run::push(const Node &n) {
	size_t place = 0, mapped = 0;
	if (!place_of(n.pose, n.grain, place) || !place_of(n.pose, map_grain_, mapped)) return;
	int &slot = slot_of(n.grain, place, n.pose);
	if (slot >= 0) {
		Node &old = nodes_[size_t(slot)];
		if (old.closed || old.g <= n.g) return;
		old = n;
		open_.push({ n.g + H_WEIGHT * h_at(mapped, n.pose), slot });
		return;
	}
	slot = int(nodes_.size());
	nodes_.push_back(n);
	open_.push({ n.g + H_WEIGHT * h_at(mapped, n.pose), slot });
}

// The exact arc of the rear axle over a step of `step` metres, `to` its end; false when the body meets something on the way.
bool WaySearch::Run::arc(const AxlePose &from, double kappa, double dir, double step, AxlePose &to) const {
	double x = from.x, z = from.z, a = from.angle;
	const double ds = step / SUBSTEPS * dir;
	for (int k = 0; k < SUBSTEPS; k++) {
		const double a1 = a + kappa * ds;
		if (std::fabs(kappa) < NO_CURVE) {
			x += std::cos(a) * ds;
			z += std::sin(a) * ds;
		} else {
			x += (std::sin(a1) - std::sin(a)) / kappa;
			z -= (std::cos(a1) - std::cos(a)) / kappa;
		}
		a = a1;
		if (!discs_clear(f_, discs_, x, z, std::cos(a), std::sin(a))) return false;
	}
	to = { x, z, a, dir < 0 };
	return true;
}

// Near the start or the goal, where a lorry is fitted to a dock or a lane, the fine grain (high fidelity near the ends, Pivtoraiko
// and Kelly; González-Sieira et al.: "using high fidelity near the goal facilitates approaching to this point"); elsewhere each way and
// curvature the longest step that does not meet anything (their Alg. 3): the coarse one, else the fine one. A pose is kept at the
// fine grain when a fine step reached it or it stands near an end, and each step leaves the cell of its grain.
bool WaySearch::Run::near_ends(const AxlePose &p) const {
	return std::hypot(p.x - goal_.pose.x, p.z - goal_.pose.z) <= SHOT_NEAR || std::hypot(p.x - start_.x, p.z - start_.z) <= SHOT_NEAR;
}

void WaySearch::Run::expand(int id) {
	const Node n = nodes_[size_t(id)];
	const bool near = grain_ == WayGrain::FINE || (grain_ == WayGrain::GRADUATED && near_ends(n.pose));
	for (double dir : { 1.0, -1.0 })
		for (double share : CURVATURES) {
			const double kappa = share / b_.radius;
			AxlePose to;
			int stepped = COARSE;
			if (near || !arc(n.pose, kappa, dir, GRAINS[COARSE].step, to)) {
				if (grain_ == WayGrain::COARSE || !arc(n.pose, kappa, dir, GRAINS[FINE].step, to)) continue;
				stepped = FINE;
			}
			const int lands = stepped == FINE || (grain_ == WayGrain::GRADUATED && near_ends(to)) ? FINE : COARSE;
			const double step = GRAINS[stepped].step;
			double g = n.g + step * (to.reverse ? REVERSE_COST : 1) + step * STEER_COST * std::fabs(share) + STEER_CHANGE_COST * std::fabs(kappa - n.kappa) * b_.radius;
			if (n.parent >= 0 && to.reverse != n.pose.reverse) g += SWITCH_COST;
			push({ to, g, kappa, id, false, lands });
		}
}

bool WaySearch::Run::shot(const AxlePose &from, std::vector<AxlePose> &out) const {
	RsPath path;
	if (!reeds_shepp(from, goal_.pose, b_.radius, path)) return false;
	bool last_back = false;
	for (int i = 0; i < path.count; i++)
		if (std::fabs(path.lengths[i]) > NO_CURVE) last_back = path.lengths[i] < 0;
	if (last_back != goal_.reversing) return false;
	sample_reeds_shepp(path, from, b_.radius, SHOT_STEP, out);
	for (const AxlePose &p : out)
		if (!discs_clear(f_, discs_, p.x, p.z, std::cos(p.angle), std::sin(p.angle))) return false;
	return true;
}

void WaySearch::Run::trace(int id) {
	for (int at = id; at >= 0; at = nodes_[size_t(at)].parent) way.push_back(nodes_[size_t(at)].pose);
	std::reverse(way.begin(), way.end());
	if (way.size() > 1) way[0].reverse = way[1].reverse; // the start is driven the way of the first step
}

WaySearch::Run::Run(const Field &f, const Steered &b, const AxlePose &start, const WayGoal &goal, std::shared_ptr<Holonomic> map, WayGrain grain) :
		f_(f), b_(b), goal_(goal), start_(start), grain_(grain), most_(grain == WayGrain::COARSE ? COARSE_MOST : FINE_MOST), map_grain_(grain == WayGrain::COARSE ? COARSE : FINE), discs_(discs_of(b)),
		h_(std::move(map)) {
	over_ = !body_clear(f, b, start) || !body_clear(f, b, goal.pose);
	if (over_) return;
	if (!h_->begun) h_->begin(f, goal.pose, b.width * HALF, GRAINS[map_grain_].cell);
	for (int k = 0; k < GRAIN_COUNT; k++) {
		nx_[k] = int(std::ceil(f.nx * f.cell / GRAINS[k].cell)); // as Holonomic::begin sizes its map
		nz_[k] = int(std::ceil(f.nz * f.cell / GRAINS[k].cell));
		tables_[k].begin(size_t(nx_[k]) * nz_[k], GRAINS[k].headings);
	}
	std::vector<Item> room;
	room.reserve(size_t(most_));
	open_ = std::priority_queue<Item, std::vector<Item>, std::greater<Item>>(std::greater<Item>(), std::move(room));
}

// A pose taken off the queue: at the goal, or a shot from it that lands, ends the search.
void WaySearch::Run::close_on(int id, int &work) {
	nodes_[size_t(id)].closed = true;
	expanded_++;
	work -= EXPANDED_WORK;
	const AxlePose p = nodes_[size_t(id)].pose;
	const double far = std::hypot(p.x - goal_.pose.x, p.z - goal_.pose.z);
	if (far <= goal_.reach && std::fabs(wrap(p.angle - goal_.pose.angle)) <= goal_.turn && (nodes_[size_t(id)].parent < 0 || p.reverse == goal_.reversing)) {
		trace(id);
		found = over_ = true;
		return;
	}
	if (far <= SHOT_NEAR || (far <= SHOT_REACH && expanded_ % SHOT_EVERY == 0)) {
		work -= SHOT_WORK;
		std::vector<AxlePose> tail;
		if (shot(p, tail)) {
			trace(id);
			way.insert(way.end(), tail.begin(), tail.end());
			found = over_ = true;
			return;
		}
	}
	expand(id);
}

bool WaySearch::Run::advance(int &work) {
	Laps took;
	if (!over_ && !begun_) {
		const int had = work;
		h_->advance(f_, work); // nothing left when another search to the goal's cell worked it out
		cost.holonomic += had - work;
		cost.holonomic_ms += took.lap();
		if (!h_->open.empty()) return false;
		begun_ = true;
		push({ start_, 0, 0, -1, false, grain_ == WayGrain::COARSE ? COARSE : FINE }); // at a finer grain the start is fine: it is an end
	}
	while (!over_ && work > 0) {
		if (open_.empty() || expanded_ >= most_) { over_ = true; break; }
		const int id = open_.top().second;
		open_.pop();
		if (!nodes_[size_t(id)].closed) close_on(id, work);
	}
	cost.expanded = expanded_;
	cost.expand_ms += took.lap();
	return over_;
}

int64_t holonomic_key(const Field &f, const AxlePose &goal, WayGrain grain) {
	const int g = grain == WayGrain::COARSE ? COARSE : FINE;
	const double cell = GRAINS[g].cell;
	const int64_t nx = int64_t(std::ceil(f.nx * f.cell / cell));
	return (int64_t(std::floor((goal.z - f.z0) / cell)) * nx + int64_t(std::floor((goal.x - f.x0) / cell))) * GRAIN_COUNT + g;
}

WaySearch::WaySearch(const Field &f, const Steered &b, const AxlePose &start, const WayGoal &goal, std::shared_ptr<Holonomic> map, WayGrain grain) :
		run_(std::make_unique<Run>(f, b, start, goal, std::move(map), grain)) {}
WaySearch::~WaySearch() = default;
bool WaySearch::advance(int &work) { return run_->advance(work); }
bool WaySearch::found() const { return run_->found; }
const std::vector<AxlePose> &WaySearch::way() const { return run_->way; }
const SearchCost &WaySearch::cost() const { return run_->cost; }

} // namespace traffic
} // namespace city
