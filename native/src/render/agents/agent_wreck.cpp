// A vehicle burnt out (Roadcraft agents.ts carcass): its own body with the glass gone, its panels buckled and its roof sagged, drawn faceted. The waves are
// the 4.0's, each model by its own seed: 45 cm dents with 18 cm ones over them, the top sagging by the square of its height.
#include "render/agents/agent_meshes.h"

#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <cmath>

using namespace godot;

namespace city {

static constexpr float FLATTEST_HALF_M = 1e-3f; // a body's half height divides: never under this

namespace {
constexpr int CORNERS = 3; // a triangle's
constexpr float WRECK_HASH_A = 97.f, SEED_B = 61.f, SEED_C = 13.f; // primes the seed is taken modulo: a phase a wave

float wave(const WreckShape &K, const Vector3 &p, float k, uint32_t seed) {
	return std::sin(p.x * k + float(seed % uint32_t(WRECK_HASH_A))) * std::cos(p.z * k * K.z_pace + float(seed % uint32_t(SEED_B))) + std::sin(p.y * k * K.y_pace + float(seed % uint32_t(SEED_C)));
}
} // namespace

AgentWreck agent_wreck(const AgentShapes &shapes, const MeshBuilder &mb, uint32_t seed) {
	const WreckShape &K = shapes.agent_wreck;
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
		const float top = std::max(0.f, p.y / half), dent = wave(K, p, 1.f / K.dent_wide_m, seed) * K.dent_wide_deep_m + wave(K, p, 1.f / K.dent_fine_m, seed) * K.dent_fine_deep_m;
		return Vector3(p.x + dent * K.dent_x, p.y - top * top * K.sag_m + dent, p.z + dent * K.dent_z);
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
