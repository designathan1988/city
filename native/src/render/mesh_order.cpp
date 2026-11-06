// See mesh_order.h.
#include "render/mesh_order.h"

#include <meshoptimizer.h>

#include <algorithm>
#include <vector>

namespace city {

void order_triangles(int32_t *idx, size_t count, size_t vertices) {
	static_assert(sizeof(int32_t) == sizeof(unsigned), "the simplifier's indices are the same words");
	if (count == 0) return;
	std::vector<unsigned> ordered(count);
	meshopt_optimizeVertexCache(ordered.data(), reinterpret_cast<const unsigned *>(idx), count, vertices);
	std::copy(ordered.begin(), ordered.end(), reinterpret_cast<unsigned *>(idx));
}

} // namespace city
