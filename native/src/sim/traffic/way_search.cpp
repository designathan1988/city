// The search (way_search.h). Nodes are poses on a grid of place and heading; a step is an exact arc of the rear axle at one of five
// curvatures, either way. The cost still to go is the larger of the paper's two heuristics: round the obstacles for the axle alone,
// and the Reeds-Shepp length near the goal. Near it a Reeds-Shepp shot is tried, the paper's analytic expansion.
#include "sim/traffic/way_search.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <queue>
#include <unordered_map>

namespace city {
namespace traffic {

namespace {
constexpr double PI = 3.14159265358979323846, TWO_PI = 2 * PI, ROOT_HALF = 0.70710678118654752, ROOT_TWO = 1.41421356237309505;
// The search's own cells, this much across, and this many headings round; a step's arc, a little longer than a cell's diagonal so a
// step always leaves its cell; the poses it may expand. A cell keeps one pose, so a tight yard can be lost at the coarser grain.
struct Grain {
	double cell = 0;
	int headings = 0;
	double step = 0;
	int most = 0;
};
constexpr Grain GRAINS[] = { { 0.5, 72, 0.8, 80000 }, { 0.25, 144, 0.4, 1500000 } };
constexpr int SUBSTEPS = 2; // poses tested along a step
constexpr double REVERSE_COST = 1.6, SWITCH_COST = 15, STEER_COST = 0.6, STEER_CHANGE_COST = 2; // per metre; a stop and a start; the wheel
constexpr double H_WEIGHT = 1.4; // a little over one finds a way sooner, a little longer than the shortest
constexpr double SHOT_REACH = 30, SHOT_NEAR = 8; // shots from every pose this near, from one in SHOT_EVERY as far as the reach
constexpr int SHOT_EVERY = 8;
constexpr double SHOT_STEP = 0.25;
constexpr double DISC_STEP = 1, DISC_SPREAD = 1.9; // two discs this far apart, in chord lengths, still cover the side between them
constexpr double QUARTER_STEP = 0.25; // a corner of the body is tested in squares about this wide: 5 cm past the flank at most
constexpr double CURVATURES[] = { -1, -0.5, 0, 0.5, 1 }; // shares of the tightest
constexpr double NO_CURVE = 1e-9, HALF = 0.5;

double wrap(double a) {
	double r = std::fmod(a, TWO_PI);
	if (r > PI) r -= TWO_PI;
	if (r < -PI) r += TWO_PI;
	return r;
}

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

// The squares of a corner that reach out of the end disc at `end`: `tip` the body's end there, `side` which flank.
void corner_squares(Discs &d, double hw, double end, double tip, double side, std::vector<BodyPoint> &out) {
	const int n = std::max(1, int(std::ceil(hw / QUARTER_STEP)));
	const double w = hw / n, inward = tip > end ? -1 : 1;
	for (int a = 0; a < n; a++)
		for (int b = 0; b < n; b++) {
			const double u = tip + inward * (a + HALF) * w, v = side * (hw - (b + HALF) * w);
			if (std::hypot(std::fabs(u - end) + w * HALF, std::fabs(v) + w * HALF) > d.r) out.push_back({ u, v });
		}
	d.square_r = w * ROOT_HALF + (d.r - hw);
}

Discs discs_of(const Steered &b) {
	Discs d;
	const double back = b.length * HALF - b.rear, front = b.length * HALF + b.rear, hw = b.width * HALF;
	const double u0 = -back + hw, u1 = front - hw;
	d.r = hw + b.margin;
	const double spacing = std::min(DISC_STEP, DISC_SPREAD * std::sqrt(d.r * d.r - hw * hw));
	const int n = std::max(1, int(std::ceil((u1 - u0) / spacing))) + 1;
	for (int i = 0; i < n; i++) d.at.push_back(u0 + (u1 - u0) * i / std::max(1, n - 1));
	const double us[2] = { -back + hw * HALF, front - hw * HALF }, vs[2] = { -hw * HALF, hw * HALF };
	for (int k = 0; k < 4; k++) {
		d.cu[k] = us[k / 2];
		d.cv[k] = vs[k % 2];
		corner_squares(d, hw, k / 2 ? u1 : u0, k / 2 ? front : -back, k % 2 ? 1 : -1, d.squares[k]);
	}
	d.corner_r = ROOT_HALF * hw + b.margin;
	d.centre = b.rear;
	d.whole = std::hypot(b.length * HALF, hw) + b.margin;
	return d;
}

bool discs_clear(const Field &f, const Discs &d, double x, double z, double c, double s) {
	const double slack = f.cell * ROOT_HALF; // the field holds the distance at a cell's centre
	if (f.clear_at(x + c * d.centre, z + s * d.centre) - slack >= d.whole) return true;
	for (double u : d.at)
		if (f.clear_at(x + c * u, z + s * u) - slack < d.r) return false;
	for (int k = 0; k < 4; k++) {
		if (f.clear_at(x + c * d.cu[k] - s * d.cv[k], z + s * d.cu[k] + c * d.cv[k]) - slack >= d.corner_r) continue;
		for (const BodyPoint &q : d.squares[k])
			if (f.clear_at(x + c * q.u - s * q.v, z + s * q.u + c * q.v) - slack < d.square_r) return false;
	}
	return true;
}

// The rear axle's shortest distance to the goal from every search cell.
struct Holonomic {
	int nx = 0, nz = 0;
	std::vector<double> dist;
};

Holonomic holonomic(const Field &f, const AxlePose &goal, double half, double cell) {
	Holonomic h;
	h.nx = int(std::ceil(f.nx * f.cell / cell));
	h.nz = int(std::ceil(f.nz * f.cell / cell));
	h.dist.assign(size_t(h.nx) * h.nz, std::numeric_limits<double>::infinity());
	const int gi = int(std::floor((goal.x - f.x0) / cell)), gj = int(std::floor((goal.z - f.z0) / cell));
	if (gi < 0 || gj < 0 || gi >= h.nx || gj >= h.nz) return h;
	using Item = std::pair<double, int>;
	std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open;
	h.dist[size_t(gj) * h.nx + gi] = 0;
	open.push({ 0, gj * h.nx + gi });
	while (!open.empty()) {
		const auto [d, c] = open.top();
		open.pop();
		if (d > h.dist[size_t(c)]) continue;
		const int i = c % h.nx, j = c / h.nx;
		for (int dj = -1; dj <= 1; dj++)
			for (int di = -1; di <= 1; di++) {
				const int a = i + di, b = j + dj;
				if ((!di && !dj) || a < 0 || b < 0 || a >= h.nx || b >= h.nz) continue;
				if (f.clear_at(f.x0 + (a + HALF) * cell, f.z0 + (b + HALF) * cell) < half) continue;
				const double nd = d + (di && dj ? ROOT_TWO : 1) * cell;
				if (nd < h.dist[size_t(b) * h.nx + a]) {
					h.dist[size_t(b) * h.nx + a] = nd;
					open.push({ nd, b * h.nx + a });
				}
			}
	}
	return h;
}

struct Node {
	AxlePose pose;
	double g = 0, kappa = 0;
	int parent = -1;
	bool closed = false;
};

class Search {
public:
	Search(const Field &f, const Steered &b, const WayGoal &goal, const Grain &grain) : f_(f), b_(b), goal_(goal), grain_(grain), discs_(discs_of(b)), h_(holonomic(f, goal.pose, b.width * HALF, grain.cell)) {}
	bool run(const AxlePose &start, std::vector<AxlePose> &way);

private:
	using Item = std::pair<double, int>;
	const Field &f_;
	const Steered &b_;
	const WayGoal &goal_;
	const Grain &grain_;
	Discs discs_;
	Holonomic h_;
	std::vector<Node> nodes_;
	std::unordered_map<int64_t, int> by_key_;
	std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open_;
	int64_t key_of(const AxlePose &p) const;
	double h_at(const AxlePose &p) const;
	void push(const Node &n);
	void expand(int id);
	bool arc(const AxlePose &from, double kappa, double dir, AxlePose &to) const;
	bool shot(const AxlePose &from, std::vector<AxlePose> &out) const;
	void trace(int id, std::vector<AxlePose> &way) const;
};

int64_t Search::key_of(const AxlePose &p) const {
	const int nx = int(std::ceil(f_.nx * f_.cell / grain_.cell));
	const int64_t i = int64_t(std::floor((p.x - f_.x0) / grain_.cell)), j = int64_t(std::floor((p.z - f_.z0) / grain_.cell));
	int k = int(std::lround(wrap(p.angle) / TWO_PI * grain_.headings)) % grain_.headings;
	if (k < 0) k += grain_.headings;
	return (j * nx + i) * grain_.headings + k;
}

double Search::h_at(const AxlePose &p) const {
	const int i = int(std::floor((p.x - f_.x0) / grain_.cell)), j = int(std::floor((p.z - f_.z0) / grain_.cell));
	if (i < 0 || j < 0 || i >= h_.nx || j >= h_.nz) return std::numeric_limits<double>::infinity();
	const double far = std::hypot(goal_.pose.x - p.x, goal_.pose.z - p.z);
	double best = std::max(h_.dist[size_t(j) * h_.nx + i], far);
	if (far <= SHOT_REACH) best = std::max(best, reeds_shepp_length(p, goal_.pose, b_.radius));
	return best;
}

void Search::push(const Node &n) {
	const int64_t key = key_of(n.pose);
	const auto hit = by_key_.find(key);
	if (hit != by_key_.end()) {
		Node &old = nodes_[size_t(hit->second)];
		if (old.closed || old.g <= n.g) return;
		old = n;
		open_.push({ n.g + H_WEIGHT * h_at(n.pose), hit->second });
		return;
	}
	const int id = int(nodes_.size());
	nodes_.push_back(n);
	by_key_[key] = id;
	open_.push({ n.g + H_WEIGHT * h_at(n.pose), id });
}

// The exact arc of the rear axle over a step, `to` its end; false when the body meets something on the way.
bool Search::arc(const AxlePose &from, double kappa, double dir, AxlePose &to) const {
	double x = from.x, z = from.z, a = from.angle;
	const double ds = grain_.step / SUBSTEPS * dir;
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

void Search::expand(int id) {
	const Node n = nodes_[size_t(id)];
	for (double dir : { 1.0, -1.0 })
		for (double share : CURVATURES) {
			const double kappa = share / b_.radius;
			AxlePose to;
			if (!arc(n.pose, kappa, dir, to)) continue;
			double g = n.g + grain_.step * (to.reverse ? REVERSE_COST : 1) + grain_.step * STEER_COST * std::fabs(share) + STEER_CHANGE_COST * std::fabs(kappa - n.kappa) * b_.radius;
			if (n.parent >= 0 && to.reverse != n.pose.reverse) g += SWITCH_COST;
			push({ to, g, kappa, id, false });
		}
}

bool Search::shot(const AxlePose &from, std::vector<AxlePose> &out) const {
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

void Search::trace(int id, std::vector<AxlePose> &way) const {
	for (int at = id; at >= 0; at = nodes_[size_t(at)].parent) way.push_back(nodes_[size_t(at)].pose);
	std::reverse(way.begin(), way.end());
	if (way.size() > 1) way[0].reverse = way[1].reverse; // the start is driven the way of the first step
}

bool Search::run(const AxlePose &start, std::vector<AxlePose> &way) {
	push({ start, 0, 0, -1, false });
	for (int expanded = 0; !open_.empty() && expanded < grain_.most;) {
		const int id = open_.top().second;
		open_.pop();
		if (nodes_[size_t(id)].closed) continue;
		nodes_[size_t(id)].closed = true;
		expanded++;
		const AxlePose p = nodes_[size_t(id)].pose;
		const double far = std::hypot(p.x - goal_.pose.x, p.z - goal_.pose.z);
		if (far <= goal_.reach && std::fabs(wrap(p.angle - goal_.pose.angle)) <= goal_.turn && (nodes_[size_t(id)].parent < 0 || p.reverse == goal_.reversing)) {
			trace(id, way);
			return true;
		}
		if (far <= SHOT_NEAR || (far <= SHOT_REACH && expanded % SHOT_EVERY == 0)) {
			std::vector<AxlePose> tail;
			if (shot(p, tail)) {
				trace(id, way);
				way.insert(way.end(), tail.begin(), tail.end());
				return true;
			}
		}
		expand(id);
	}
	return false;
}
} // namespace

bool body_clear(const Field &f, const Steered &b, const AxlePose &p) {
	return discs_clear(f, discs_of(b), p.x, p.z, std::cos(p.angle), std::sin(p.angle));
}

bool body_fits(const Field &f, const Steered &b, const AxlePose &p, double margin) {
	const double c = std::cos(p.angle), s = std::sin(p.angle), step = f.cell * HALF;
	const double back = b.length * HALF - b.rear + margin, front = b.length * HALF + b.rear + margin, hw = b.width * HALF + margin;
	for (double u = -back; u <= front; u += step)
		for (double v = -hw; v <= hw; v += step)
			if (f.solid_at(p.x + c * u - s * v, p.z + s * u + c * v)) return false;
	return true;
}

bool find_way(const Field &f, const Steered &b, const AxlePose &start, const WayGoal &goal, std::vector<AxlePose> &way, bool fine) {
	if (!body_clear(f, b, start) || !body_clear(f, b, goal.pose)) return false;
	Search search(f, b, goal, GRAINS[fine ? 1 : 0]);
	return search.run(start, way);
}

} // namespace traffic
} // namespace city
