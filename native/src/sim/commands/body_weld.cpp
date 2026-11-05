// Two buildings made one (model_ops.h): the blocks of the one absorbed go over to the other's lot frame as they stand, each with the
// walls and roof it had. A block at another bearing keeps its plan as an outline; parts keep their boxes, turned by quarters.
#include "sim/buildings/b40/plan_area.h"
#include "sim/commands/model_ops.h"

#include <algorithm>
#include <cmath>

namespace city {

using namespace b40;

namespace {
constexpr float QUARTER_RAD = 1.5707963f, SAME_BEARING_RAD = 1e-3f;
constexpr int SIDES = 4;

struct Move {
	const LotFrame &from, &to;
	Point2 at(float x, float y) const {
		V2 rel = from.at(x, y) - to.o;
		return Point2{ rel.x * to.e0.x + rel.y * to.e0.y, rel.x * to.e1.x + rel.y * to.e1.y };
	}
};

// `other`'s paints in `into`, each added once.
struct PaintMap {
	Body &into;
	const Body &other;
	std::vector<int> index;
	int of(int i) {
		if (i < 0) return -1;
		int &n = index[size_t(i)];
		if (n < 0) n = into.add_paint(other.paints[size_t(i)]);
		return n;
	}
};

void move_plan(Volume &v, const Move &m, bool same_bearing) {
	Ring ring = ring_of(v);
	float x0 = FAR_AWAY, y0 = FAR_AWAY, x1 = -FAR_AWAY, y1 = -FAR_AWAY;
	for (Point2 &p : ring) {
		p = m.at(p.x, p.y);
		x0 = std::min(x0, p.x);
		y0 = std::min(y0, p.y);
		x1 = std::max(x1, p.x);
		y1 = std::max(y1, p.y);
	}
	v.x = x0;
	v.y = y0;
	v.w = x1 - x0;
	v.d = y1 - y0;
	if (same_bearing) return;
	v.outline.clear();
	for (const Point2 &p : ring) v.outline.push_back(Point2{ (p.x - x0) / v.w, (p.y - y0) / v.d });
}
} // namespace

void weld_body(Body &into, const LotFrame &at, const Body &other, const LotFrame &of, float turn) {
	const Move move{ of, at };
	const int quarters = int(std::lround(turn / QUARTER_RAD));
	const bool same_bearing = quarters % SIDES == 0 && std::fabs(turn - float(quarters) * QUARTER_RAD) < SAME_BEARING_RAD;
	const int turned = ((quarters % SIDES) + SIDES) % SIDES;
	PaintMap paints{ into, other, std::vector<int>(other.paints.size(), -1) };
	for (Volume v : other.volumes) {
		const int wall = v.wall >= 0 ? v.wall : other.wall, roof = v.roof_paint >= 0 ? v.roof_paint : other.roof;
		for (RoofPart &p : v.gear) {
			Point2 q = move.at(p.x, p.y);
			p.x = q.x;
			p.y = q.y;
			p.turn += turn;
		}
		move_plan(v, move, same_bearing);
		v.wall = paints.of(wall);
		v.roof_paint = paints.of(roof);
		for (auto &e : v.storey_paint) e.second = paints.of(e.second);
		if (turned % 2 == 1 && v.ridge >= 0) v.ridge = 1 - v.ridge;
		if (turned != 0 && v.roof == ROOF_SHED) v.fall = (std::max(0, v.fall) + turned) % SIDES;
		into.volumes.push_back(std::move(v));
	}
	for (LotElement e : other.elements) {
		Point2 q = move.at(e.x, e.y);
		e.x = q.x;
		e.y = q.y;
		e.paint = paints.of(e.paint);
		e.facing = (e.facing + turned) % SIDES;
		if (turned % 2 == 1) std::swap(e.w, e.d);
		into.elements.push_back(e);
	}
	for (Core c : other.cores) {
		Point2 q = move.at(c.x, c.y);
		c.x = q.x;
		c.y = q.y;
		into.cores.push_back(c);
	}
}

} // namespace city
