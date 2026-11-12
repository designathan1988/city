// CityProbes::square_fit, square_rooms: the squares a generated city's plan sets down (sim/citygen/squares.h), held tree by tree.
// A tree of a square is an element of its lot, and an element's rectangle is the room of what stands there (the trees' renderer draws
// the crown inside it, the camera's clearance reads the crown's reach from it): so every tree's room lies inside the square's own
// ground, no two rooms lie in one another, no room is narrower than the clearance a trunk keeps from the square's lines on either
// side nor wider than the step between two trees, no tree is more slender than the plan's tallest on a whole step, and no trunk
// stands on a walk or on the plaza (each by its outline, element_shape.h); the plaza is one round cover about the walks' crossing,
// and stays so through a save's bytes. square_fit lays a square on a block of lots given by hand, with no city; square_rooms reads the city's own.
#include "api/city_probes.h"

#include "sim/buildings/b40/element_shape.h"
#include "sim/buildings/b40/record.h"
#include "sim/citygen/lot_gen.h"
#include "sim/citygen/squares.h"
#include "sim/persistence/body_codec.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <shared_mutex>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f;
constexpr float ROOM_PITCH_M = 0.25f; // a room is held against its square's ground at points this far apart
constexpr float ROOM_SLACK_M = 0.01f; // two rooms side by side share a line; a room on the lot's line stands on it
constexpr int LOT_FIELDS = 5; // a lot laid by hand: x, z, width, depth, the way it faces

constexpr size_t PLAZA_FROM = 3; // a square's covers in the order it lays them: its lawn, its two walks, then its plaza

struct Tally {
	int squares = 0, trees = 0, out = 0, meet = 0, on_walk = 0;
	int plaza_covers = 0, plazas_round = 0; // the covers laid for the squares' plazas, and the squares whose plaza is one round cover
	Array first_plaza;
	float least = b40::FAR_AWAY, most = 0, slender = 0;
	Array first_out, first_meet, first_walk;
};

// Every tree of a square's body, in the lot's frame: `inside` says whether a point is the square's own ground.
void measure(const b40::Body &body, const std::function<bool(float, float)> &inside, float plaza_m, Tally &t) {
	t.squares++;
	std::vector<const b40::LotElement *> trees, covers;
	for (const b40::LotElement &e : body.elements) {
		if (e.kind == b40::EL_TREE) trees.push_back(&e);
		if (e.kind == b40::EL_SURFACE) covers.push_back(&e);
	}
	// The plaza: one round cover, `plaza_m` across, about the point where the two walks cross.
	if (covers.size() > PLAZA_FROM) {
		const b40::LotElement &p = *covers[PLAZA_FROM];
		const float cx = covers[2]->x, cy = covers[1]->y;
		t.plaza_covers += int(covers.size() - PLAZA_FROM);
		const bool one = covers.size() == PLAZA_FROM + 1 && p.round && std::fabs(p.w - plaza_m) <= ROOM_SLACK_M && std::fabs(p.d - plaza_m) <= ROOM_SLACK_M &&
				std::fabs(p.x - cx) <= ROOM_SLACK_M && std::fabs(p.y - cy) <= ROOM_SLACK_M;
		t.plazas_round += one ? 1 : 0;
		if (!one && t.first_plaza.is_empty()) t.first_plaza = Array::make(int(covers.size() - PLAZA_FROM), p.round, p.x - cx, p.y - cy, p.w, p.d);
	}
	for (size_t i = 0; i < trees.size(); i++) {
		const b40::LotElement &p = *trees[i];
		const float side = std::min(p.w, p.d);
		t.trees++;
		t.least = std::min(t.least, side);
		t.most = std::max(t.most, std::max(p.w, p.d));
		t.slender = std::max(t.slender, p.h / side);
		// its room, point by point, on the square's ground
		const float x0 = p.x - p.w * HALF + ROOM_SLACK_M, x1 = p.x + p.w * HALF - ROOM_SLACK_M, y0 = p.y - p.d * HALF + ROOM_SLACK_M, y1 = p.y + p.d * HALF - ROOM_SLACK_M;
		const int nx = 1 + int(std::ceil((x1 - x0) / ROOM_PITCH_M)), ny = 1 + int(std::ceil((y1 - y0) / ROOM_PITCH_M));
		bool past = false;
		for (int ix = 0; ix <= nx && !past; ix++)
			for (int iy = 0; iy <= ny && !past; iy++) past = !inside(x0 + (x1 - x0) * float(ix) / float(nx), y0 + (y1 - y0) * float(iy) / float(ny));
		if (past) {
			t.out++;
			if (t.first_out.is_empty()) t.first_out = Array::make(p.x, p.y, p.w, p.d);
		}
		// against every other tree's
		for (size_t k = i + 1; k < trees.size(); k++) {
			const b40::LotElement &q = *trees[k];
			if (std::fabs(p.x - q.x) >= (p.w + q.w) * HALF - ROOM_SLACK_M || std::fabs(p.y - q.y) >= (p.d + q.d) * HALF - ROOM_SLACK_M) continue;
			t.meet++;
			if (t.first_meet.is_empty()) t.first_meet = Array::make(p.x, p.y, p.w, q.x, q.y, q.w);
		}
		// its trunk on the lawn: the first cover is the square's lawn, the others its walks
		for (size_t k = 1; k < covers.size(); k++) {
			if (!b40::outline_of(*covers[k]).holds(p.x, p.y)) continue;
			t.on_walk++;
			if (t.first_walk.is_empty()) t.first_walk = Array::make(p.x, p.y, covers[k]->x, covers[k]->y, covers[k]->w, covers[k]->d);
			break;
		}
	}
}

Dictionary answer(const Tally &t, const CityPlanDef &plan) {
	Dictionary r;
	r["squares"] = t.squares;
	r["trees"] = t.trees;
	r["out"] = t.out; // trees whose room passes the square's own ground
	r["first_out"] = t.first_out; // the room's middle in the lot's frame, its width and depth
	r["meet"] = t.meet; // pairs of trees whose rooms lie in one another
	r["first_meet"] = t.first_meet; // each one's middle and width
	r["on_walk"] = t.on_walk; // trunks standing on a walk
	r["first_walk"] = t.first_walk; // the trunk, the walk's middle, width and depth
	r["plaza_covers"] = t.plaza_covers; // the covers laid for the plazas: one a square
	r["plazas_round"] = t.plazas_round; // the squares whose plaza is one round cover of the plan's size about the walks' crossing
	r["first_plaza"] = t.first_plaza; // of the first that is not: its covers, whether the first is round, its middle off the crossing, its size
	r["plaza_m"] = 2 * plan.square_plaza_m;
	r["least_room_m"] = t.trees > 0 ? t.least : 0.f; // the narrowest room's side
	r["most_room_m"] = t.most;
	r["slender"] = t.slender; // the most a tree's height is of its room's side
	r["edge_m"] = plan.square_tree_edge_m; // the plan's numbers the rooms are held against
	r["step_m"] = plan.square_tree_grid_m;
	r["low_m"] = plan.square_tree_low_m;
	r["high_m"] = plan.square_tree_high_m;
	return r;
}
} // namespace

// A square on a block of lots laid by hand, each a row (x, z, width, depth, the way it faces in radians), with the draws of `seed`.
Dictionary CityProbes::square_fit(const Array &lots, int seed) const {
	std::vector<Lot> block;
	for (const Variant &row : lots) {
		const Array a = row;
		if (a.size() < LOT_FIELDS) continue;
		Lot l;
		l.x = double(a[0]);
		l.z = double(a[1]);
		l.w = double(a[2]);
		l.d = double(a[3]);
		l.rot = double(a[4]);
		block.push_back(l);
	}
	const CityPlanDef &plan = state_->catalog.growth.plan;
	b40::Body laid_body, body;
	Tally t;
	// The square is measured as a save keeps it (a square's body is stored with its record): written to bytes and read back.
	ByteWriter bytes;
	const bool laid = square_on(plan, state_->buildings.designs, block, uint32_t(seed), laid_body) && (write_body(bytes, laid_body), true);
	ByteReader kept{ bytes.buf.data(), bytes.buf.size() };
	if (laid && read_body(kept, body)) {
		// The square's frame is its first lot's, its front the box's nearest line to that lot's street: a point of the body in the
		// world, then in each lot's own frame.
		const Lot &first = block[0];
		const float sn = std::sin(float(first.rot)), cs = std::cos(float(first.rot));
		const V2 along(cs, -sn), in(-sn, -cs);
		float a0 = b40::FAR_AWAY, a1 = -b40::FAR_AWAY, b0 = b40::FAR_AWAY;
		const auto corners = [](const Lot &l, const std::function<void(V2)> &fn) {
			const float s = std::sin(float(l.rot)), c = std::cos(float(l.rot)), hw = float(l.w) * HALF, hd = float(l.d) * HALF;
			for (float u : { -1.f, 1.f })
				for (float v : { -1.f, 1.f }) fn(V2(float(l.x), float(l.z)) + V2(c, -s) * (hw * u) + V2(s, c) * (hd * v));
		};
		for (const Lot &l : block)
			corners(l, [&](V2 p) {
				a0 = std::min(a0, p.dot(along));
				a1 = std::max(a1, p.dot(along));
				b0 = std::min(b0, p.dot(in));
			});
		const V2 front = along * ((a0 + a1) * HALF) + in * b0;
		measure(body, [&](float x, float y) {
			const V2 p = front + along * x + in * y;
			return std::any_of(block.begin(), block.end(), [&](const Lot &l) {
				const float s = std::sin(float(l.rot)), c = std::cos(float(l.rot));
				const V2 rel = p - V2(float(l.x), float(l.z));
				return std::fabs(rel.dot(V2(c, -s))) <= float(l.w) * HALF + ROOM_SLACK_M && std::fabs(rel.dot(V2(s, c))) <= float(l.d) * HALF + ROOM_SLACK_M;
			});
		}, float(2 * plan.square_plaza_m), t);
	}
	Dictionary r = answer(t, plan);
	r["laid"] = t.squares > 0; // laid, and whole after its bytes
	r["w"] = body.plan.W;
	r["d"] = body.plan.D;
	return r;
}

// Every square of the city: each tree's room against its record's rectangle.
Dictionary CityProbes::square_rooms() const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Tally t;
	for (const Building &rec : state_->buildings.items) {
		if (!rec.alive || rec.fn != FN_SQUARE) continue;
		const std::shared_ptr<const b40::Body> body = b40::record_body(state_->buildings.designs, rec);
		measure(*body, [&](float x, float y) { return std::fabs(x) <= rec.w * HALF + ROOM_SLACK_M && y >= -ROOM_SLACK_M && y <= rec.d + ROOM_SLACK_M; },
				float(2 * state_->catalog.growth.plan.square_plaza_m), t);
	}
	return answer(t, state_->catalog.growth.plan);
}

} // namespace city
