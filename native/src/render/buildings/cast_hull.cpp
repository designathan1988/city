// See cast_hull.h. meshoptimizer: https://github.com/zeux/meshoptimizer (MIT), "Simplification" and "Indexing" in its README. The shells keep a vertex per face
// (each its own normal and colour), which to the simplifier are borders it may not move: they are welded by position first.
#include "render/buildings/cast_hull.h"

#include "render/surface_data.h"

#include <meshoptimizer.h>

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <vector>

using namespace godot;

namespace city {

OwnedMesh cast_hull(const b40::Shell &walls, const b40::Shell &panes, float error_m, const RID &material) {
	OwnedMesh out;
	std::vector<Vector3> points;
	std::vector<unsigned> corners;
	points.reserve(walls.pos.size() + panes.pos.size());
	corners.reserve(walls.idx.size() + panes.idx.size());
	for (const b40::Shell *s : { &walls, &panes }) {
		const unsigned first = unsigned(points.size());
		points.insert(points.end(), s->pos.begin(), s->pos.end());
		for (int32_t i : s->idx) corners.push_back(first + unsigned(i));
	}
	if (corners.empty()) return out;
	std::vector<unsigned> remap(points.size());
	const size_t welded = meshopt_generateVertexRemap(remap.data(), corners.data(), corners.size(), points.data(), points.size(), sizeof(Vector3));
	std::vector<Vector3> unique(welded);
	meshopt_remapVertexBuffer(unique.data(), points.data(), points.size(), sizeof(Vector3), remap.data());
	meshopt_remapIndexBuffer(corners.data(), corners.data(), corners.size(), remap.data());
	std::vector<unsigned> kept(corners.size());
	kept.resize(meshopt_simplify(kept.data(), corners.data(), corners.size(), &unique[0].x, welded, sizeof(Vector3), 0, error_m, meshopt_SimplifyErrorAbsolute, nullptr));
	if (kept.empty()) return out;
	std::vector<unsigned> moved(welded, ~0u);
	PackedVector3Array vertices;
	PackedInt32Array indices;
	indices.resize(int64_t(kept.size()));
	int32_t *to = indices.ptrw();
	for (size_t k = 0; k < kept.size(); k++) {
		if (moved[kept[k]] == ~0u) {
			moved[kept[k]] = unsigned(vertices.size());
			vertices.push_back(unique[kept[k]]);
		}
		to[k] = int32_t(moved[kept[k]]);
	}
	Array arrays;
	arrays.resize(RenderingServer::ARRAY_MAX);
	arrays[RenderingServer::ARRAY_VERTEX] = vertices;
	arrays[RenderingServer::ARRAY_INDEX] = indices;
	return OwnedMesh::of(mesh_of_arrays(arrays, material));
}

} // namespace city
