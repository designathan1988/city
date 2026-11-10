// See mounds.h. The hill is a grid with a flattened bell for its shape, broken by three octaves of a lattice noise: a heap of rubble slumps at
// its angle of repose with lumps in it, and its edge dies into the ground.
#include "render/effects/mounds.h"

#include "core/random.h"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int RELIEF_CELLS = 40;
constexpr float BELL_POWER = 2.2f, BELL_SHOULDER = 1.1f; // how flat its top, how soon it falls away
constexpr float LUMP_SHARE = 0.28f; // what the noise moves of its height
constexpr uint32_t LATTICE_SEED = 7741;
constexpr uint32_t BYTE = 255, GREEN_SHIFT = 8, BLUE_SHIFT = 16;
constexpr float HASH_TOP = 65535.f;
constexpr float LUMP_WIDE = 5, LUMP_MID = 12, LUMP_FINE = 27; // the relief's three scales
constexpr float COARSE_SHARE = 0.55f, MID_SHARE = 0.3f, FINE_SHARE = 0.15f;
// rubble.gdshader stretches the hill's height by smoothstep(age) * slump * chunk, each noise 0..1: at most these
constexpr float RUBBLE_SLUMP_MOST = 0.35f + 1.1f, RUBBLE_CHUNK_MOST = 0.72f + 0.56f;

float lattice(int i, int j) { return float(hash_u32(uint32_t(i) * 73856093u ^ uint32_t(j) * 19349663u ^ LATTICE_SEED) & 0xFFFFu) / HASH_TOP; }

float value_noise(float x, float z) {
	const int i = int(std::floor(x)), j = int(std::floor(z));
	const float fx = x - float(i), fz = z - float(j), sx = fx * fx * (3 - 2 * fx), sz = fz * fz * (3 - 2 * fz);
	const float a = lattice(i, j) + (lattice(i + 1, j) - lattice(i, j)) * sx, b = lattice(i, j + 1) + (lattice(i + 1, j + 1) - lattice(i, j + 1)) * sx;
	return a + (b - a) * sz;
}

// Height of the unit hill at (x, z), both -0.5..0.5.
float hill(float x, float z) {
	const float r = std::min(1.f, std::pow(std::pow(std::fabs(x) * 2, 3.f) + std::pow(std::fabs(z) * 2, 3.f), 1.f / 3.f)); // a rounded square, as the footprint
	const float bell = std::pow(std::max(0.f, 1 - std::pow(r, BELL_POWER)), BELL_SHOULDER);
	const float lumps = value_noise(x * LUMP_WIDE, z * LUMP_WIDE) * COARSE_SHARE + value_noise(x * LUMP_MID, z * LUMP_MID) * MID_SHARE + value_noise(x * LUMP_FINE, z * LUMP_FINE) * FINE_SHARE;
	return bell * (1 - LUMP_SHARE + LUMP_SHARE * 2 * lumps);
}

Ref<ArrayMesh> hill_mesh() {
	PackedVector3Array v, n;
	PackedInt32Array idx;
	const float step = 1.f / float(RELIEF_CELLS);
	for (int j = 0; j <= RELIEF_CELLS; j++)
		for (int i = 0; i <= RELIEF_CELLS; i++) {
			const float x = float(i) * step - 0.5f, z = float(j) * step - 0.5f;
			v.push_back(Vector3(x, hill(x, z), z));
			n.push_back(Vector3(hill(x - step, z) - hill(x + step, z), 2 * step, hill(x, z - step) - hill(x, z + step)).normalized());
		}
	for (int j = 0; j < RELIEF_CELLS; j++)
		for (int i = 0; i < RELIEF_CELLS; i++) {
			const int a = j * (RELIEF_CELLS + 1) + i, b = a + 1, c = a + RELIEF_CELLS + 1, d = c + 1;
			for (int k : { a, b, c, b, d, c }) idx.push_back(k);
		}
	Array arrays;
	arrays.resize(Mesh::ARRAY_MAX);
	arrays[Mesh::ARRAY_VERTEX] = v;
	arrays[Mesh::ARRAY_NORMAL] = n;
	arrays[Mesh::ARRAY_INDEX] = idx;
	Ref<ArrayMesh> mesh;
	mesh.instantiate();
	mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
	return mesh;
}
} // namespace

float Mounds::hill_at(float x, float z) { return hill(x, z); }

void Mounds::init(const RID &scenario, const RID &material, int capacity) {
	material_ = material;
	capacity_ = capacity;
	const Ref<ArrayMesh> hill = hill_mesh();
	AABB local = hill->get_aabb(); // as the shader may stretch it: a height times 0 .. the most
	const float most = RUBBLE_SLUMP_MOST * RUBBLE_CHUNK_MOST, low = std::min(0.f, local.position.y * most), high = std::max(0.f, local.get_end().y * most);
	local.position.y = low;
	local.size.y = high - low;
	layer_.init_shaped(scenario, hill, material, capacity, true, local, 0.f);
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
