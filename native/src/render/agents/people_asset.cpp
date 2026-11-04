// Reads project/assets/people/people.bin (layout in tools/assets/cook_people.py) into ArrayMeshes and the palette
// texture. The file is checked end to end: a short or inconsistent file is refused with a message.
#include "render/agents/people_asset.h"

#include "core/bytes.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_color_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t MAGIC = 0x50504352u; // "RCPP"
constexpr uint32_t VERSION = 2, FLAG_FEMALE = 1, FLAG_SENIOR = 2, FLAG_CHILD = 4;
constexpr uint32_t MAX_VARIATIONS = 256, MAX_LODS = 4, MAX_VERTICES = 65535, BONE_TEXELS = 3, RGBA = 4;
constexpr int BYTE_MAX = 255, BYTE_BITS = 8, CHANNELS = 4;
constexpr float SRGB_GAMMA = 2.2f, PALETTE_SPAN = 2.f; // palette values lie in [-1.1, 1.0]
constexpr int TEXELS_PER_VALUE = 2;
constexpr uint32_t FIXED_MAX = 65535;

// Float textures are not sampled reliably in the Compatibility renderer (Godot may fall back to 8 bits and clamp), so
// each palette value goes out as 16-bit fixed point over [-PALETTE_SPAN, PALETTE_SPAN]: texel 2x holds the high
// bytes of texel x's four channels, texel 2x + 1 the low bytes (decoded in people.gdshader).
PackedByteArray fixed_point(const PackedByteArray &floats) {
	const float *f = reinterpret_cast<const float *>(floats.ptr());
	int64_t n = floats.size() / int64_t(sizeof(float)), texels = n / RGBA;
	PackedByteArray out;
	out.resize(n * TEXELS_PER_VALUE);
	uint8_t *o = out.ptrw();
	for (int64_t t = 0; t < texels; t++)
		for (int c = 0; c < int(RGBA); c++) {
			float v = std::clamp(f[t * RGBA + c], -PALETTE_SPAN, PALETTE_SPAN);
			uint32_t q = uint32_t(std::lround((v + PALETTE_SPAN) / (2 * PALETTE_SPAN) * float(FIXED_MAX)));
			o[(t * TEXELS_PER_VALUE) * RGBA + c] = uint8_t(q >> BYTE_BITS);
			o[(t * TEXELS_PER_VALUE + 1) * RGBA + c] = uint8_t(q & BYTE_MAX);
		}
	return out;
}

struct LodSize {
	uint32_t vertices = 0, indices = 0;
};

float linear(uint32_t c, int shift) { return std::pow(float((c >> shift) & BYTE_MAX) / float(BYTE_MAX), SRGB_GAMMA); }

Ref<ArrayMesh> read_mesh(ByteReader &r, const LodSize &sz) {
	PackedVector3Array pos, nor;
	PackedColorArray col;
	PackedVector2Array uv2;
	PackedInt32Array bones;
	PackedFloat32Array weights;
	PackedInt32Array idx;
	pos.resize(sz.vertices);
	nor.resize(sz.vertices);
	col.resize(sz.vertices);
	uv2.resize(sz.vertices);
	bones.resize(int64_t(sz.vertices) * CHANNELS);
	weights.resize(int64_t(sz.vertices) * CHANNELS);
	uint8_t b8[CHANNELS], w8[CHANNELS];
	for (uint32_t i = 0; i < sz.vertices; i++) {
		float p[3], n[3];
		for (float &v : p) v = r.get<float>();
		for (float &v : n) v = r.get<float>();
		uint32_t c = r.get<uint32_t>();
		r.get_bytes(b8, CHANNELS);
		r.get_bytes(w8, CHANNELS);
		float detail = r.get<float>();
		pos.set(i, Vector3(p[0], p[1], p[2]));
		nor.set(i, Vector3(n[0], n[1], n[2]));
		col.set(i, Color(linear(c, 0), linear(c, BYTE_BITS), linear(c, 2 * BYTE_BITS), 1));
		// the skin in the engine's own skin attributes (BONE_INDICES / BONE_WEIGHTS in the shader), the weights made to
		// sum to 1 exactly (the cooked bytes may miss it by a unit)
		int sum = 0;
		for (int k = 0; k < CHANNELS; k++) sum += w8[k];
		for (int k = 0; k < CHANNELS; k++) {
			bones.set(int64_t(i) * CHANNELS + k, int32_t(b8[k]));
			weights.set(int64_t(i) * CHANNELS + k, sum > 0 ? float(w8[k]) / float(sum) : (k == 0 ? 1.f : 0.f));
		}
		uv2.set(i, Vector2(float(c >> (3 * BYTE_BITS)), detail));
	}
	idx.resize(sz.indices);
	for (uint32_t i = 0; i < sz.indices; i++) idx.set(i, int32_t(r.get<uint16_t>()));
	if (!r.ok) return Ref<ArrayMesh>();
	Array arr;
	arr.resize(Mesh::ARRAY_MAX);
	arr[Mesh::ARRAY_VERTEX] = pos;
	arr[Mesh::ARRAY_NORMAL] = nor;
	arr[Mesh::ARRAY_COLOR] = col;
	arr[Mesh::ARRAY_TEX_UV2] = uv2;
	arr[Mesh::ARRAY_BONES] = bones;
	arr[Mesh::ARRAY_WEIGHTS] = weights;
	arr[Mesh::ARRAY_INDEX] = idx;
	// no skeleton is bound (the MultiMesh is skinned by people.gdshader from the palette), so the bone attributes stay as
	// data. Not CUSTOM0/1: Godot 4.7.2's Compatibility renderer gives an RGBA8 custom attribute one component
	// (drivers/gles3/storage/mesh_storage.cpp: size = fmtsize / sizeof(float)), so g, b, a read 0, 0, 1
	Ref<ArrayMesh> m;
	m.instantiate();
	m->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arr);
	return m;
}
} // namespace

bool PeopleAsset::load(const String &path, PackedStringArray &errors) {
	PackedByteArray raw = FileAccess::get_file_as_bytes(path);
	ByteReader r{ raw.ptr(), size_t(raw.size()) };
	uint32_t magic = r.get<uint32_t>(), version = r.get<uint32_t>(), count = r.get<uint32_t>(), bones = r.get<uint32_t>();
	uint32_t lod_count = r.get<uint32_t>(), pal_w = r.get<uint32_t>();
	block_rows = int(r.get<uint32_t>());
	idle_row = int(r.get<uint32_t>());
	idle_frames = int(r.get<uint32_t>());
	pedal_row = int(r.get<uint32_t>());
	pedal_frames = int(r.get<uint32_t>());
	const uint32_t pal_h = uint32_t(block_rows) * count;
	if (!r.ok || magic != MAGIC || version != VERSION || count == 0 || count > MAX_VARIATIONS || lod_count == 0 || lod_count > MAX_LODS ||
			pal_w != bones * BONE_TEXELS || idle_row <= 0 || idle_frames <= 0 || pedal_row < idle_row + idle_frames || pedal_row + pedal_frames > block_rows) {
		errors.push_back("pessoas: arquivo ausente ou inválido: " + path);
		return false;
	}
	lods = int(lod_count);
	variations.assign(count, PersonVariation());
	std::vector<LodSize> sizes(size_t(count) * lod_count);
	for (uint32_t v = 0; v < count; v++) {
		PersonVariation &pv = variations[v];
		pv.walk_frames = int(r.get<uint32_t>());
		pv.idle_s = r.get<float>();
		pv.walk_advance = r.get<float>();
		pv.height = r.get<float>();
		uint32_t flags = r.get<uint32_t>();
		pv.female = flags & FLAG_FEMALE;
		pv.senior = flags & FLAG_SENIOR;
		pv.child = flags & FLAG_CHILD;
		for (uint32_t l = 0; l < lod_count; l++) sizes[v * lod_count + l] = { r.get<uint32_t>(), r.get<uint32_t>() };
	}
	for (uint32_t v = 0; v < count && r.ok; v++)
		for (uint32_t l = 0; l < lod_count && r.ok; l++) {
			const LodSize &sz = sizes[v * lod_count + l];
			if (sz.vertices > MAX_VERTICES) r.ok = false;
			else variations[v].lods.push_back(read_mesh(r, sz));
		}
	PackedByteArray texels;
	texels.resize(int64_t(pal_w) * pal_h * RGBA * sizeof(float));
	if (r.ok) r.get_bytes(texels.ptrw(), size_t(texels.size()));
	if (!r.ok || r.o != r.n) {
		errors.push_back("pessoas: arquivo truncado ou com sobra: " + path);
		return false;
	}
	palette = ImageTexture::create_from_image(Image::create_from_data(int(pal_w) * TEXELS_PER_VALUE, int(pal_h), false, Image::FORMAT_RGBA8,
			fixed_point(texels)));
	return true;
}

} // namespace city
