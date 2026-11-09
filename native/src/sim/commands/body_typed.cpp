// A block's size, place and first level as typed in the Build drawer's Selection panel (the Roadcraft's setVolumeSize, setBlockOffset
// and setBlockBase), each about the block's own middle or the block it stands on (body_ops.h).
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/commands/body_ops.h"

#include <algorithm>
#include <cmath>
#include <set>

namespace city {

using namespace b40;

namespace {
constexpr const char *NO_BLOCK = "tool.failed";
// A size this near the one it has is the same; a shift this small is none.
constexpr float SIZE_SLACK_M = 1e-3f, NO_SHIFT = 1e-6f;
} // namespace

// A typed width or depth, about the block's middle: what stands on its roof keeps its share of the plan, and the walls count their bays again.
std::string op_block_size(Body &b, const Tuning &t, int vol, int axis, float metres) {
	if (vol < 0 || vol >= int(b.volumes.size()) || (axis != AXIS_X && axis != AXIS_Y)) return NO_BLOCK;
	Volume &v = b.volumes[size_t(vol)];
	const float size = std::clamp(metres, float(t.build_min_side_m), float(t.build_max_side_m));
	float &at = axis == AXIS_X ? v.x : v.y, &was = axis == AXIS_X ? v.w : v.d;
	if (std::fabs(size - was) < SIZE_SLACK_M) return "";
	const float middle = at + was / 2, k = size / was;
	for (RoofPart &p : v.gear) (axis == AXIS_X ? p.x : p.y) = middle + ((axis == AXIS_X ? p.x : p.y) - middle) * k;
	at = middle - size / 2;
	was = size;
	v.reliefs.clear();
	for (FaceGeometry &f : v.faces) f.bays = -1;
	return "";
}

// The block's middle put at a typed distance from the middle of the block it stands on, with what stands on it.
std::string op_block_offset(Body &b, int vol, int axis, float metres) {
	if (vol < 0 || vol >= int(b.volumes.size()) || (axis != AXIS_X && axis != AXIS_Y)) return NO_BLOCK;
	const Volume &v = b.volumes[size_t(vol)];
	const Volume *r = reference_of(b, v);
	if (r == nullptr) return "building.problem.noReference";
	const float shift = axis == AXIS_X ? r->x + r->w / 2 + metres - v.x - v.w / 2 : r->y + r->d / 2 + metres - v.y - v.d / 2;
	if (std::fabs(shift) < NO_SHIFT) return "";
	for (int i : riding_on(b, vol)) shift_block(b.volumes[size_t(i)], axis == AXIS_X ? shift : 0, axis == AXIS_Y ? shift : 0);
	return "";
}

// The level the block starts on, what stands on it carried up or down with it; the body's check refuses a block left in the air.
std::string op_block_base(Body &b, const Tuning &t, int vol, int level) {
	if (vol < 0 || vol >= int(b.volumes.size()) || level < 0) return NO_BLOCK;
	const int shift = level - b.volumes[size_t(vol)].base;
	if (shift == 0) return "";
	const std::set<int> moving = riding_on(b, vol);
	for (int i : moving)
		if (b.volumes[size_t(i)].top() + shift > t.edit_max_floors) return "building.problem.size";
	for (int i : moving) b.volumes[size_t(i)].base += shift;
	return "";
}

} // namespace city
