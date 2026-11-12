// See mounds.h. The hill is a grid with a flattened bell for its shape, broken by three octaves of a lattice noise: a heap of rubble slumps at
// its angle of repose with lumps in it, and its edge dies into the ground.
#include "render/effects/mounds.h"

#include "core/random.h"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <godot_cpp/core/math.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

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

// CPU counterpart of lattice_noise.gdshaderinc, solely for an outward geometric error bound.
uint32_t shader_pcg(uint32_t v) {
	const uint32_t s = v * 747796405u + 2891336453u;
	const uint32_t w = ((s >> ((s >> 28u) + 4u)) ^ s) * 277803737u;
	return (w >> 22u) ^ w;
}
float shader_lattice(int x, int z) {
	const uint32_t h = (uint32_t(x) + 32768u) * 73856093u ^ (uint32_t(z) + 32768u) * 19349663u;
	return float(shader_pcg(h)) / 4294967295.f;
}
float shader_noise(float x, float z) {
	const int i = int(std::floor(x)), j = int(std::floor(z));
	const float fx = x - std::floor(x), fz = z - std::floor(z), sx = fx * fx * (3 - 2 * fx), sz = fz * fz * (3 - 2 * fz);
	const float a = shader_lattice(i, j) + (shader_lattice(i + 1, j) - shader_lattice(i, j)) * sx;
	const float b = shader_lattice(i, j + 1) + (shader_lattice(i + 1, j + 1) - shader_lattice(i, j + 1)) * sx;
	return a + (b - a) * sz;
}

Ref<ArrayMesh> hill_mesh(int stride, std::vector<Vector3> *positions = nullptr) {
	PackedVector3Array v, n;
	PackedInt32Array idx;
	const int cells = RELIEF_CELLS / stride;
	const float step = 1.f / float(RELIEF_CELLS);
	for (int j = 0; j <= cells; j++)
		for (int i = 0; i <= cells; i++) {
			const float x = float(i * stride) * step - 0.5f, z = float(j * stride) * step - 0.5f;
			v.push_back(Vector3(x, hill(x, z), z));
			n.push_back(Vector3(hill(x - step, z) - hill(x + step, z), 2 * step, hill(x, z - step) - hill(x, z + step)).normalized());
		}
	for (int j = 0; j < cells; j++)
		for (int i = 0; i < cells; i++) {
			const int a = j * (cells + 1) + i, b = a + 1, c = a + cells + 1, d = c + 1;
			for (int k : { a, b, c, b, d, c }) idx.push_back(k);
		}
	if (positions) { positions->clear(); positions->reserve(size_t(v.size())); for (int64_t i = 0; i < v.size(); i++) positions->push_back(v[i]); }
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

void Mounds::init(const RID &scenario, const RID &material, int capacity, float sector_m, float error_px, float fade_s) {
	release();
	material_ = material;
	capacity_ = capacity;
	error_px_ = std::isfinite(error_px) ? std::max(0.f, error_px) : 0;
	fade_s_ = std::isfinite(fade_s) ? std::max(0.f, fade_s) : 0;
	for (int level = 0; level < LEVELS; level++) {
		meshes_[size_t(level)] = hill_mesh(1 << level, level == 0 ? &fine_positions_ : nullptr);
		main_[level].configure(scenario, meshes_[size_t(level)]->get_rid(), material, false, LAYER_BASE, std::max(1.f, sector_m));
	}
	relief_scratch_.resize(fine_positions_.size());
	layer_.init(scenario, meshes_[0], material, capacity, true, AABB(Vector3(-WORLD_HALF_M, -WORLD_UP_M, -WORLD_HALF_M), Vector3(2 * WORLD_HALF_M, 2 * WORLD_UP_M, 2 * WORLD_HALF_M)), std::max(1.f, sector_m), true);
	// hill <= 1.28; shader slump <= 1.45, chunk <= 1.28, fall easing in [0,1].
	layer_.envelope(AABB(Vector3(-.5f, 0, -.5f), Vector3(1, 1.28f * 1.45f * 1.28f, 1)));
	layer_.count(0);
}

Mounds::Detail Mounds::prepare_detail(const float *row) {
	Detail detail;
	const AABB local(Vector3(-.5f, 0, -.5f), Vector3(1, 1.28f * 1.45f * 1.28f, 1));
	detail.bounds = instance_bounds(row, local);
	bool finite = true;
	for (size_t i = 0; i < fine_positions_.size(); i++) {
		const Vector3 &p = fine_positions_[i];
		const float x = row[0] * p.x + row[2] * p.z + row[3] + row[17] * 91.f;
		const float z = row[8] * p.x + row[10] * p.z + row[11] + row[17] * 91.f;
		if (!std::isfinite(x) || !std::isfinite(z) || std::fabs(x) > 1.e9f || std::fabs(z) > 1.e9f) { finite = false; break; }
		const float slump = .35f + 1.1f * shader_noise(x * .09f, z * .09f);
		const float chunk = .72f + .56f * shader_noise(x * .62f, z * .62f);
		relief_scratch_[i] = double(row[5]) * p.y * slump * chunk;
		finite = finite && std::isfinite(relief_scratch_[i]);
	}
	if (!finite) {
		for (int level = 1; level < LEVELS; level++) detail.error[size_t(level)] = std::numeric_limits<double>::infinity();
		return detail;
	}
	const double epsilon = std::numeric_limits<float>::epsilon();
	const double terms = std::max(std::fabs(double(row[3])) + .5 * (std::fabs(double(row[0])) + std::fabs(double(row[2]))),
			std::fabs(double(row[11])) + .5 * (std::fabs(double(row[8])) + std::fabs(double(row[10])))) + std::fabs(double(row[17])) * 91;
	const float half_seed = Math::half_to_float(Math::make_half_float(row[17]));
	if (!std::isfinite(half_seed)) {
		for (int level = 1; level < LEVELS; level++) detail.error[size_t(level)] = std::numeric_limits<double>::infinity();
		return detail;
	}
	// Compatibility's half custom seed can shift the noise coordinates; cover it together with the float backend.
	const double coordinate_error = 16 * epsilon * (terms + 1) + 91 * std::fabs(double(row[17]) - half_seed);
	// Smoothstep derivative <=1.5 per coordinate. Product-rule bound for slump*chunk, using their complete ranges.
	const double factor_gradient = 3 * (1.28 * 1.1 * .09 + 1.45 * .56 * .62);
	const double arithmetic = std::fabs(double(row[5])) * 1.28 * (factor_gradient * coordinate_error + 128 * epsilon);
	// Float grid coordinates differ slightly from an exact nested grid. Bound the corresponding surface displacement:
	// hill's L1 gradient <30, multiplier <=1.856; the shader's world-space multiplier gradient is bounded above.
	const double columns = std::fabs(double(row[0])) + std::fabs(double(row[2])) + std::fabs(double(row[8])) + std::fabs(double(row[10]));
	const double grid_error = std::fabs(double(row[5])) * (30 * 1.856 + 1.28 * factor_gradient * columns) * 16 * epsilon;
	auto sample = [&](int x, int z) { return relief_scratch_[size_t(z) * (RELIEF_CELLS + 1) + size_t(x)]; };
	for (int level = 1; level < LEVELS; level++) {
		const int stride = 1 << level;
		double error = 0;
		for (int j = 0; j <= RELIEF_CELLS; j++)
			for (int i = 0; i <= RELIEF_CELLS; i++) {
				const int x0 = std::min(i / stride, RELIEF_CELLS / stride - 1) * stride;
				const int z0 = std::min(j / stride, RELIEF_CELLS / stride - 1) * stride;
				const double x = double(i - x0) / stride, z = double(j - z0) / stride;
				const double a = sample(x0, z0), b = sample(x0 + stride, z0), c = sample(x0, z0 + stride), d = sample(x0 + stride, z0 + stride);
				const double coarse = x + z <= 1 ? a + (b - a) * x + (c - a) * z : d + (c - d) * (1 - x) + (b - d) * (1 - z);
				error = std::max(error, std::fabs(sample(i, j) - coarse));
			}
		// Both piecewise-linear surfaces share this refinement; their difference reaches extrema at refinement vertices.
		// Include CPU/GPU float differences on both surfaces, coordinate refinement error and the full-height fall endpoint.
		detail.error[size_t(level)] = std::max(detail.error[size_t(level - 1)], error + 2 * arithmetic + grid_error);
	}
	return detail;
}

void Mounds::reset() {
	shown_ = 0;
	owners_.clear();
	slots_.clear();
	detail_.clear();
	layer_.count(0);
	for (SpatialInstanceDraw &draw : main_) draw.reset();
	presentation_ = transitioning_ = false;
	dirty_ = true;
}

void Mounds::release() {
	layer_.release();
	for (SpatialInstanceDraw &draw : main_) draw.reset();
	for (auto &mesh : meshes_) mesh.unref();
	fine_positions_.clear(); relief_scratch_.clear();
	owners_.clear(); slots_.clear(); detail_.clear(); shown_ = 0;
	presentation_ = transitioning_ = false;
}

void Mounds::add(uint32_t owner, const Vector3 &at, float rot, float w, float d, float h, const Color &wall, float seed, double born) {
	if (holds(owner) || shown_ >= capacity_) return;
	const int slot = shown_++;
	slots_[owner] = slot;
	owners_.push_back(owner);
	const float c = std::cos(rot), s = std::sin(rot);
	const uint32_t packed = uint32_t(wall.r * float(BYTE)) | uint32_t(wall.g * float(BYTE)) << GREEN_SHIFT | uint32_t(wall.b * float(BYTE)) << BLUE_SHIFT;
	layer_.put(slot, Vector3(c * w, 0, -s * w), Vector3(0, h, 0), Vector3(s * d, 0, c * d), at, float(born), seed, float(packed), 0);
	detail_.push_back(prepare_detail(layer_.buf.ptr() + int64_t(slot) * MM_STRIDE));
	dirty_ = true;
}

void Mounds::remove(uint32_t owner) {
	const auto found = slots_.find(owner);
	if (found == slots_.end()) return;
	const int at = found->second;
	float *rows = layer_.buf.ptrw();
	std::memmove(rows + int64_t(at) * MM_STRIDE, rows + int64_t(at + 1) * MM_STRIDE, size_t(shown_ - at - 1) * MM_STRIDE * sizeof(float));
	layer_.shift_bounds(at, shown_);
	owners_.erase(owners_.begin() + at);
	detail_.erase(detail_.begin() + at);
	slots_.erase(found);
	shown_--;
	for (int i = at; i < shown_; i++) slots_[owners_[size_t(i)]] = i;
	dirty_ = true;
}

void Mounds::update(double now) {
	RenderingServer::get_singleton()->material_set_param(material_, "now", float(now));
	if (dirty_) {
		layer_.count(shown_);
		layer_.shown(shown_ > 0);
		layer_.upload(); // full-detail casters; their selection never depends on the main camera's projected error
	}
	if (!dirty_ && presentation_ && !transitioning_ && view_.same_as(shown_view_)) {
		for (SpatialInstanceDraw &draw : main_) draw.settle();
		return;
	}
	for (SpatialInstanceDraw &draw : main_) draw.begin();
	transitioning_ = false;
	const float *rows = layer_.buf.ptr();
	for (int i = 0; i < shown_; i++) {
		Detail &detail = detail_[size_t(i)];
		double scale = 0;
		for (int level = 1; level < LEVELS; level++)
			if (detail.error[size_t(level)] > 0) { const double error = detail.error[size_t(level)]; scale = std::isfinite(error) ? view_.vertical_error(detail.bounds, error) / error : std::numeric_limits<double>::infinity(); break; }
		auto pixels = [&](int level) { return detail.error[size_t(level)] == 0 ? 0.0 : detail.error[size_t(level)] * scale; };
		int best = 0;
		if (view_.ready && error_px_ > 0)
			for (int level = 1; level < LEVELS; level++) if (pixels(level) <= double(error_px_) * .75) best = level;
		const bool valid = view_.ready && error_px_ > 0;
		if (!valid || !std::isfinite(pixels(detail.level)) || pixels(detail.level) > error_px_ || (detail.target >= 0 && (!std::isfinite(pixels(detail.target)) || pixels(detail.target) > error_px_))) {
			// A camera jump may invalidate a fading coarse surface. Keep the error cap by presenting the valid finer level immediately.
			detail.level = best; detail.target = -1;
		} else if (detail.target >= 0 && now >= detail.since + fade_s_) {
			detail.level = detail.target; detail.target = -1;
		}
		if (detail.target < 0 && (best > detail.level || (best < detail.level && pixels(detail.level) > double(error_px_) * .9))) {
			if (fade_s_ > 0) { detail.target = best; detail.since = now; }
			else detail.level = best;
		}
		float row[MM_STRIDE];
		std::memcpy(row, rows + int64_t(i) * MM_STRIDE, sizeof(row));
		const uint64_t owner = owners_[size_t(i)];
		if (detail.target >= 0) {
			transitioning_ = true;
			const float share = float(std::clamp((now - detail.since) / double(fade_s_), 0.0, 1.0));
			row[19] = 1.f + share;
			main_[detail.level].put(owner, row, detail.bounds);
			row[19] = -(1.f + share);
			main_[detail.target].put(owner, row, detail.bounds);
		} else {
			row[19] = 0;
			main_[detail.level].put(owner, row, detail.bounds);
		}
	}
	for (SpatialInstanceDraw &draw : main_) draw.finish();
	shown_view_ = view_;
	presentation_ = true;
	dirty_ = false;
}

} // namespace city
