// A vehicle burnt out (Roadcraft agents.ts carcass): its own body with the glass gone, its panels buckled and its roof sagged, drawn faceted. The waves are
// the 4.0's, each model by its own seed: 45 cm dents with 18 cm ones over them, the top sagging by the square of its height.
#include "render/agents/agent_meshes.h"

#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <cmath>

using namespace godot;

namespace city {

static constexpr float FLATTEST_HALF_M = 1e-3f;

namespace {
constexpr float DENT_WIDE_M = 0.45f, DENT_FINE_M = 0.18f, DENT_WIDE_DEEP_M = 0.04f, DENT_FINE_DEEP_M = 0.015f, SAG_M = 0.12f;
constexpr float DENT_X = 0.5f, DENT_Z = 0.6f, Z_PACE = 1.3f, Y_PACE = 0.7f;
constexpr int CORNERS = 3;
constexpr float WRECK_HASH_A = 97.f, SEED_B = 61.f, SEED_C = 13.f;

float wave(const Vector3 &p, float k, uint32_t seed) {
	return std::sin(p.x * k + float(seed % uint32_t(WRECK_HASH_A))) * std::cos(p.z * k * Z_PACE + float(seed % uint32_t(SEED_B))) + std::sin(p.y * k * Y_PACE + float(seed % uint32_t(SEED_C)));
}
} // namespace

AgentWreck agent_wreck(const MeshBuilder &mb, uint32_t seed) {
	AABB box;
	bool first = true;
	for (int64_t i = 0; i < mb.v.size(); i++) {
		if (first) box = AABB(mb.v[i], Vector3());
		else box.expand_to(mb.v[i]);
		first = false;
	}
	const Vector3 middle = box.get_center();
	const float half = std::max(box.size.y * 0.5f, FLATTEST_HALF_M);
	const auto buckled = [&](const Vector3 &at) {
		const Vector3 p = at - middle;
		const float top = std::max(0.f, p.y / half), dent = wave(p, 1.f / DENT_WIDE_M, seed) * DENT_WIDE_DEEP_M + wave(p, 1.f / DENT_FINE_M, seed) * DENT_FINE_DEEP_M;
		return Vector3(p.x + dent * DENT_X, p.y - top * top * SAG_M + dent, p.z + dent * DENT_Z);
	};
	MeshBuilder out;
	const float glass = agent_tag(AgentTag::GLASS).x;
	for (int64_t t = 0; t + CORNERS <= mb.idx.size(); t += CORNERS) {
		if (mb.uv2[mb.idx[t]].x == glass) continue; // burnt out
		const Vector3 a = buckled(mb.v[mb.idx[t]]), b = buckled(mb.v[mb.idx[t + 1]]), c = buckled(mb.v[mb.idx[t + 2]]);
		const Vector3 face = (b - a).cross(c - a);
		if (face.length_squared() <= 0) continue;
		const Vector3 n = face.normalized();
		const int i0 = out.add(a, n, AGENT_WHITE), i1 = out.add(b, n, AGENT_WHITE), i2 = out.add(c, n, AGENT_WHITE);
		out.tri(i0, i1, i2, n);
	}
	AgentWreck w;
	w.size = box.size;
	w.mesh.instantiate();
	out.commit_to(w.mesh->get_rid());
	return w;
}

} // namespace city
