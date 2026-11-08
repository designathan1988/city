// The cut of a body (cut.h).
#include "sim/buildings/interior/cut.h"
#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
} // namespace

int top_level(const Body &b) {
	int top = 0;
	for (const Volume &v : b.volumes) top = std::max(top, v.top() - 1);
	return top;
}

Point2 cut_direction(int dir) {
	const float a = float(dir & (CUT_DIRECTIONS - 1)) * TAU_F / float(CUT_DIRECTIONS);
	return { std::cos(a), std::sin(a) };
}

int cut_step(float x, float z) {
	const float turns = std::atan2(z, x) / TAU_F * float(CUT_DIRECTIONS);
	return int(std::lround(turns)) & (CUT_DIRECTIONS - 1);
}

Body cut_open(const Body &b, int level, Point2 view) {
	Body out = b;
	const int at = std::clamp(level, 0, top_level(b));
	out.volumes.clear();
	int entrance = -1;
	for (size_t i = 0; i < b.volumes.size(); i++) {
		if (b.volumes[i].base > at) continue;
		if (int(i) == b.entrance_volume) entrance = int(out.volumes.size());
		out.volumes.push_back(b.volumes[i]);
		Volume &v = out.volumes.back();
		v.storeys = std::min(v.storeys, at - v.base + 1);
		if (v.fill.size() > size_t(v.storeys)) v.fill.resize(size_t(v.storeys));
	}
	out.entrance_volume = std::max(entrance, 0);
	out.cut_level = at;
	out.cut_view = view;
	return out;
}

} // namespace city::b40
