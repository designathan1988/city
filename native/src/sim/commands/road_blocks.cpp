// Several blocks at once (Roadcraft editor/blocks.ts): a grid of streets laid in one gesture, each line a road of its own that joins what is
// already there, the whole grid one step of undo.
#include "sim/commands/road_blocks.h"

#include <cmath>

namespace city {

namespace {
constexpr double JOIN_PX = 4; // a line's end takes a junction or road this near, as a pointer would
} // namespace

std::vector<BlockLine> block_lines(double x, double z, const BlockGrid &g) {
	const double step = std::round(g.block_m), w = g.cols * step, h = g.rows * step;
	const double c = std::cos(g.angle), s = std::sin(g.angle);
	const auto at = [&](double u, double v) { return StrokePoint{ x + u * c - v * s, z + u * s + v * c }; };
	std::vector<BlockLine> out;
	for (int j = 0; j <= g.rows; j++) out.push_back({ at(-w / 2, -h / 2 + j * step), at(w / 2, -h / 2 + j * step) });
	for (int i = 0; i <= g.cols; i++) out.push_back({ at(-w / 2 + i * step, -h / 2), at(-w / 2 + i * step, h / 2) });
	return out;
}

EditResult lay_blocks(CityState &s, double x, double z, const BlockGrid &g, int profile) {
	EditResult all;
	all.error = "hint.road.invalid";
	for (const BlockLine &l : block_lines(x, z, g)) {
		const Anchor a = find_anchor(s, l.a.x, l.a.z, JOIN_PX), b = find_anchor(s, l.b.x, l.b.z, JOIN_PX);
		if (!plan_road(s, a, false, 0, 0, b, profile).ok) continue; // a line the land refuses: the others stand
		const EditResult r = build_road(s, a, false, 0, 0, b, profile);
		if (!r.ok) return r; // no money left half way: the caller rolls the whole grid back
		all.ok = true;
		all.error.clear();
		all.money += r.money;
		all.segments += r.segments;
		all.buildings_removed += r.buildings_removed;
	}
	return all;
}

} // namespace city
