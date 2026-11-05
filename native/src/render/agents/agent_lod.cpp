// See agent_lod.h. meshoptimizer: https://github.com/zeux/meshoptimizer (MIT), "Simplification" in its README.
#include "render/agents/agent_lod.h"

#include <meshoptimizer.h>

#include <array>
#include <cstdint>
#include <map>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr int CORNERS = 3, AXES = 3;
constexpr float PIVOTED = 0.5f; // uv2.y above it: uv is a pivot or a seat (agents.gdshader vertex()), the same for all of a part
constexpr unsigned NO_INDEX = 0xFFFFFFFFu;
using PartKey = std::array<float, 8>;

PartKey part_of(const MeshBuilder &m, int vertex) {
	const Vector2 t2 = m.uv2[vertex];
	const Vector2 pivot = t2.y > PIVOTED ? m.uv[vertex] : Vector2();
	const Color col = m.c[vertex];
	return { t2.x, t2.y, pivot.x, pivot.y, col.r, col.g, col.b, col.a };
}
} // namespace

MeshBuilder simplify_parts(const MeshBuilder &in, float error_m) {
	const size_t vertices = size_t(in.v.size());
	std::vector<float> positions(vertices * AXES);
	for (size_t i = 0; i < vertices; i++) {
		const Vector3 &p = in.v[int64_t(i)];
		positions[i * AXES] = p.x;
		positions[i * AXES + 1] = p.y;
		positions[i * AXES + 2] = p.z;
	}
	std::map<PartKey, std::vector<unsigned>> parts;
	for (int64_t t = 0; t + CORNERS <= in.idx.size(); t += CORNERS) {
		std::vector<unsigned> &part = parts[part_of(in, in.idx[t])];
		for (int k = 0; k < CORNERS; k++) part.push_back(unsigned(in.idx[t + k]));
	}
	// Borders stay: where two parts meet (a door against a wing) neither may open a gap.
	const unsigned options = meshopt_SimplifyLockBorder | meshopt_SimplifySparse | meshopt_SimplifyErrorAbsolute;
	MeshBuilder out;
	std::vector<unsigned> moved(vertices, NO_INDEX), kept;
	for (const auto &[key, part] : parts) {
		kept.resize(part.size());
		kept.resize(meshopt_simplify(kept.data(), part.data(), part.size(), positions.data(), vertices, sizeof(float) * AXES, 0, error_m, options, nullptr));
		for (unsigned v : kept) {
			if (moved[v] == NO_INDEX) {
				const int64_t i = int64_t(v);
				moved[v] = unsigned(out.add(in.v[i], in.n[i], in.c[i], in.uv[i], in.uv2[i]));
			}
			out.idx.push_back(int32_t(moved[v]));
		}
	}
	return out;
}

} // namespace city
