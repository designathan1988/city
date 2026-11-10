// See mounds.h. The hill is a grid with a flattened bell for its shape, broken by three octaves of a lattice noise: a heap of rubble slumps at
// its angle of repose with lumps in it, and its edge dies into the ground.
#include "render/effects/mounds.h"
#include "render/surface_data.h"

#include "core/random.h"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
// The relief's lattice: its salt, a hash's low sixteen bits taken over their top; a packed colour's channels.
constexpr uint32_t LATTICE_SEED = 7741;
constexpr uint32_t BYTE = 255, GREEN_SHIFT = 8, BLUE_SHIFT = 16;
constexpr uint32_t HASH_BITS = 0xFFFFu;
constexpr float HASH_TOP = 65535.f;
// rubble.gdshader stretches the hill's height by smoothstep(age) * slump * chunk, each noise 0..1: at most these
constexpr float RUBBLE_SLUMP_MOST = 0.35f + 1.1f, RUBBLE_CHUNK_MOST = 0.72f + 0.56f;

float lattice(int i, int j) { return float(hash_u32(uint32_t(i) * SPATIAL_PRIME_X ^ uint32_t(j) * SPATIAL_PRIME_Y ^ LATTICE_SEED) & HASH_BITS) / HASH_TOP; }

float value_noise(float x, float z) {
	const int i = int(std::floor(x)), j = int(std::floor(z));
	const float fx = x - float(i), fz = z - float(j), sx = fx * fx * (3 - 2 * fx), sz = fz * fz * (3 - 2 * fz);
	const float a = lattice(i, j) + (lattice(i + 1, j) - lattice(i, j)) * sx, b = lattice(i, j + 1) + (lattice(i + 1, j + 1) - lattice(i, j + 1)) * sx;
	return a + (b - a) * sz;
}

// Height of the unit hill at (x, z), both -0.5..0.5.
float hill(const MoundsShape &K, float x, float z) {
	const float r = std::min(1.f, std::pow(std::pow(std::fabs(x) * 2, 3.f) + std::pow(std::fabs(z) * 2, 3.f), 1.f / 3.f)); // a rounded square, as the footprint
	const float bell = std::pow(std::max(0.f, 1 - std::pow(r, K.bell_power)), K.bell_shoulder);
	const float lumps = value_noise(x * K.lump_wide, z * K.lump_wide) * K.coarse_share + value_noise(x * K.lump_mid, z * K.lump_mid) * K.mid_share + value_noise(x * K.lump_fine, z * K.lump_fine) * K.fine_share;
	return bell * (1 - K.lump_share + K.lump_share * 2 * lumps);
}

Array hill_mesh(const MoundsShape &K) {
	PackedVector3Array v, n;
	PackedInt32Array idx;
	const float step = 1.f / float(K.relief_cells);
	for (int j = 0; j <= K.relief_cells; j++)
		for (int i = 0; i <= K.relief_cells; i++) {
			const float x = float(i) * step - 0.5f, z = float(j) * step - 0.5f;
			v.push_back(Vector3(x, hill(K, x, z), z));
			n.push_back(Vector3(hill(K, x - step, z) - hill(K, x + step, z), 2 * step, hill(K, x, z - step) - hill(K, x, z + step)).normalized());
		}
	for (int j = 0; j < K.relief_cells; j++)
		for (int i = 0; i < K.relief_cells; i++) {
			const int a = j * (K.relief_cells + 1) + i, b = a + 1, c = a + K.relief_cells + 1, d = c + 1;
			for (int k : { a, b, c, b, d, c }) idx.push_back(k);
		}
	Array arrays;
	arrays.resize(Mesh::ARRAY_MAX);
	arrays[Mesh::ARRAY_VERTEX] = v;
	arrays[Mesh::ARRAY_NORMAL] = n;
	arrays[Mesh::ARRAY_INDEX] = idx;
	return arrays;
}
} // namespace

float Mounds::hill_at(const MoundsShape &K, float x, float z) { return hill(K, x, z); }

void Mounds::init(const RID &scenario, const RID &material, int capacity, const MoundsShape &K) {
	material_ = material;
	capacity_ = capacity;
	const Array hill = hill_mesh(K);
	AABB local = arrays_box(hill); // as the shader may stretch it: a height times 0 .. the most
	const float most = RUBBLE_SLUMP_MOST * RUBBLE_CHUNK_MOST, low = std::min(0.f, local.position.y * most), high = std::max(0.f, local.get_end().y * most);
	local.position.y = low;
	local.size.y = high - low;
	layer_.init_shaped(scenario, OwnedMesh::of(mesh_of_arrays(hill, material)), capacity, true, local, 0.f);
	layer_.count(0);
}

void Mounds::reset() {
	shown_ = 0;
	layer_.clear_bounds(); // the heaps laid again add theirs
	dirty_ = true;
}

void Mounds::release() { layer_.release(); }

void Mounds::add(const Vector3 &at, float rot, float w, float d, float h, const Color &wall, float seed, double born) {
	if (shown_ >= capacity_) return;
	const float c = std::cos(rot), s = std::sin(rot);
	const uint32_t packed = uint32_t(wall.r * float(BYTE)) | uint32_t(wall.g * float(BYTE)) << GREEN_SHIFT | uint32_t(wall.b * float(BYTE)) << BLUE_SHIFT;
	layer_.put(shown_++, Vector3(c * w, 0, -s * w), Vector3(0, h, 0), Vector3(s * d, 0, c * d), at, float(born), seed, float(packed), 0);
	dirty_ = true;
}

void Mounds::update(double now) {
	if (shown_ > 0) RenderingServer::get_singleton()->material_set_param(material_, "now", float(now)); // a mound settles by `now`: told while one stands
	if (!dirty_) return;
	layer_.count(shown_);
	layer_.shown(shown_ > 0);
	layer_.upload();
	dirty_ = false;
}

} // namespace city
