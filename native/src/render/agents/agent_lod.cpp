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
constexpr unsigned NO_INDEX = 0xFFFFFFFFu; // a vertex not yet moved: no index has every bit set
constexpr int PART_FIELDS = 8; // tag, kind, the pivot's two and the colour's four
using PartKey = std::array<float, PART_FIELDS>;

PartKey part_of(const MeshBuilder &m, int vertex) {
	const Vector2 t2 = m.uv2[vertex];
	const Vector2 pivot = t2.y > PIVOTED ? m.uv[vertex] : Vector2();
	const Color col = m.c[vertex];
	return { t2.x, t2.y, pivot.x, pivot.y, col.r, col.g, col.b, col.a };
}

// Reduces lists of `in`'s triangles into `out`, each vertex carried over once.
class Reducer {
public:
	Reducer(const MeshBuilder &in, MeshBuilder &out, float error_m) : in_(in), out_(out), error_m_(error_m), moved_(size_t(in.v.size()), NO_INDEX) {
		positions_.resize(moved_.size() * AXES);
		for (size_t i = 0; i < moved_.size(); i++) {
			const Vector3 &p = in.v[int64_t(i)];
			positions_[i * AXES] = p.x;
			positions_[i * AXES + 1] = p.y;
			positions_[i * AXES + 2] = p.z;
		}
	}
	void reduce(const std::vector<unsigned> &triangles, unsigned options) {
		std::vector<unsigned> kept(triangles.size());
		kept.resize(meshopt_simplify(kept.data(), triangles.data(), triangles.size(), positions_.data(), moved_.size(), sizeof(float) * AXES, 0, error_m_,
				options | meshopt_SimplifyErrorAbsolute, nullptr));
		for (unsigned v : kept) {
			if (moved_[v] == NO_INDEX) {
				const int64_t i = int64_t(v);
				moved_[v] = unsigned(out_.add(in_.v[i], in_.n[i], in_.c[i], in_.uv[i], in_.uv2[i]));
			}
			out_.idx.push_back(int32_t(moved_[v]));
		}
	}

private:
	const MeshBuilder &in_;
	MeshBuilder &out_;
	float error_m_;
	std::vector<unsigned> moved_;
	std::vector<float> positions_;
};
} // namespace

MeshBuilder simplify_parts(const MeshBuilder &in, float error_m) {
	std::map<PartKey, std::vector<unsigned>> parts;
	for (int64_t t = 0; t + CORNERS <= in.idx.size(); t += CORNERS) {
		std::vector<unsigned> &part = parts[part_of(in, in.idx[t])];
		for (int k = 0; k < CORNERS; k++) part.push_back(unsigned(in.idx[t + k]));
	}
	// Borders stay: where two parts meet (a door against a wing) neither may open a gap.
	MeshBuilder out;
	Reducer reducer(in, out, error_m);
	for (const auto &[key, part] : parts) reducer.reduce(part, meshopt_SimplifyLockBorder | meshopt_SimplifySparse);
	return out;
}

MeshBuilder simplify_whole(const MeshBuilder &in, float error_m) {
	MeshBuilder out;
	Reducer reducer(in, out, error_m);
	reducer.reduce(std::vector<unsigned>(in.idx.ptr(), in.idx.ptr() + in.idx.size()), 0);
	return out;
}

} // namespace city
