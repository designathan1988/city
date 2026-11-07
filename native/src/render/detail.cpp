// The one owner of level of detail (render/detail.h): distances to boxes.
#include "render/detail.h"

#include <algorithm>
#include <cmath>

namespace city {

double DetailView::distance_to(double minx, double miny, double minz, double maxx, double maxy, double maxz) const {
	const double dx = std::max({ minx - double(eye.x), 0.0, double(eye.x) - maxx });
	const double dy = std::max({ miny - double(eye.y), 0.0, double(eye.y) - maxy });
	const double dz = std::max({ minz - double(eye.z), 0.0, double(eye.z) - maxz });
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

double DetailView::distance_to(const godot::AABB &box) const {
	const godot::Vector3 lo = box.position, hi = box.position + box.size;
	return distance_to(lo.x, lo.y, lo.z, hi.x, hi.y, hi.z);
}

} // namespace city
