// The levels of a mesh (mesh_levels.h).
#include "render/mesh_levels.h"

#include <godot_cpp/variant/variant.hpp>
#include <meshoptimizer.h>

#include <cstring>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr int64_t LEAST_INDICES = 300; // under this a surface is what it is
constexpr size_t AXES = 3;
} // namespace

Dictionary mesh_levels(const PackedVector3Array &pos, const PackedInt32Array &idx, std::initializer_list<float> errors) {
	Dictionary out;
	if (idx.size() < LEAST_INDICES) return out;
	const unsigned *from = reinterpret_cast<const unsigned *>(idx.ptr());
	const size_t count = size_t(idx.size()), vertices = size_t(pos.size());
	std::vector<float> xyz(vertices * AXES);
	for (size_t i = 0; i < vertices; i++)
		for (size_t a = 0; a < AXES; a++) xyz[i * AXES + a] = pos[int64_t(i)][int(a)];
	std::vector<unsigned> kept(count);
	size_t before = count;
	for (float error : errors) {
		const size_t left = meshopt_simplify(kept.data(), from, count, xyz.data(), vertices, sizeof(float) * AXES, 0, error, meshopt_SimplifyErrorAbsolute | meshopt_SimplifyPrune, nullptr);
		if (left == 0 || left >= before) continue;
		PackedInt32Array level;
		level.resize(int64_t(left));
		std::memcpy(level.ptrw(), kept.data(), left * sizeof(unsigned));
		out[error] = level;
		before = left;
	}
	return out;
}

} // namespace city
