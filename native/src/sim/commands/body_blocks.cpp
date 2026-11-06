// The blocks a rectangle dragged against a block makes (the 4.0's addPlanMass and planAction): a wing on the ground, a block on the roof, a void cut out; and how a block
// counts in the body (setBlockMode).
#include "sim/buildings/b40/plan_area.h"
#include "sim/commands/body_ops.h"

#include "core/polygon_ops.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace city {

using namespace b40;

namespace {
constexpr const char *NO_BLOCK = "tool.failed";
constexpr float TOUCH_AREA_M2 = 1e-3f;
constexpr int ROOF_MASS_STOREYS = 2;

float on_grid(const Tuning &t, float x) { return std::round(x / float(t.build_grid_m)) * float(t.build_grid_m); }

// The biggest piece of `box` standing on the block: a mass on a roof is kept to the roof.
Ring on_roof(const Ring &box, const Volume &v) {
	PlanRing a, c;
	for (const Point2 &p : box) a.push_back(V2(p.x, p.y));
	for (const Point2 &p : ring_of(v)) c.push_back(V2(p.x, p.y));
	Ring best;
	float most = 0;
	for (const PlanRing &piece : plan_intersection({ a }, { c })) {
		Ring ring;
		for (const V2 &p : piece) ring.push_back({ p.x, p.y });
		const float area = ring_area(ring);
		if (area > most) {
			most = area;
			best = std::move(ring);
		}
	}
	return best;
}
} // namespace

std::string op_add_mass(Body &b, const Tuning &t, int vol, Point2 from, Point2 to, int action) {
	if (vol < 0 || vol >= int(b.volumes.size()) || action < MASS_GROUND || action > MASS_CUT) return NO_BLOCK;
	if (int(b.volumes.size()) >= int(t.build_max_volumes)) return "building.problem.size";
	Volume &source = b.volumes[size_t(vol)];
	const float x0 = on_grid(t, std::min(from.x, to.x)), x1 = on_grid(t, std::max(from.x, to.x)), y0 = on_grid(t, std::min(from.y, to.y)), y1 = on_grid(t, std::max(from.y, to.y));
	Ring ring{ { x0, y0 }, { x1, y0 }, { x1, y1 }, { x0, y1 } };
	if (x1 <= x0 || y1 <= y0) return "building.problem.size";
	if (action == MASS_ROOF) ring = on_roof(ring, source);
	if (ring.size() < 3) return "building.problem.outline";
	if (action == MASS_CUT && overlap_area(ring, ring_of(source)) <= TOUCH_AREA_M2) return "building.problem.cut";
	Volume mass = block_like(source, action == MASS_ROOF ? std::min(ROOF_MASS_STOREYS, std::max(1, int(t.edit_max_floors) - source.top())) : source.storeys);
	for (FaceGeometry &f : mass.faces) f.bays = -1; // its openings' proportions, but as many bays as its own walls hold
	set_block_ring(mass, ring);
	if (action == MASS_GROUND) {
		mass.base = 0;
		std::fill(mass.fill.begin(), mass.fill.end(), source.fill.front());
	} else if (action == MASS_ROOF) {
		mass.base = source.top();
		mass.roof = ROOF_FLAT;
		std::fill(mass.fill.begin(), mass.fill.end(), next_fill(source));
		clear_roof_under(source, ring_of(mass));
		source.roof = ROOF_TERRACE;
	} else {
		mass.mode = BLOCK_VOID;
		mass.roof = ROOF_FLAT;
	}
	b.volumes.push_back(std::move(mass));
	return "";
}

std::string op_set_block_mode(Body &b, int vol, int mode) {
	if (vol < 0 || vol >= int(b.volumes.size()) || mode < BLOCK_SOLID || mode > BLOCK_XOR) return NO_BLOCK;
	b.volumes[size_t(vol)].mode = BlockMode(mode);
	const bool mass = std::any_of(b.volumes.begin(), b.volumes.end(), [](const Volume &v) { return v.mode == BLOCK_SOLID || v.mode == BLOCK_XOR; });
	return mass ? "" : "building.problem.noMass";
}

} // namespace city
