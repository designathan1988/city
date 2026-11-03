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
constexpr uint32_t VERSION = 1, FLAG_FEMALE = 1, FLAG_SENIOR = 2;
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
	PackedVector2Array uv, uv2;
	PackedByteArray bones, weights;
	PackedInt32Array idx;
	pos.resize(sz.vertices);
	nor.resize(sz.vertices);
	col.resize(sz.vertices);
	uv.resize(sz.vertices);
	uv2.resize(sz.vertices);
	bones.resize(int64_t(sz.vertices) * CHANNELS);
	weights.resize(int64_t(sz.vertices) * CHANNELS);
	for (uint32_t i = 0; i < sz.vertices; i++) {
		float p[3], n[3];
		for (float &v : p) v = r.get<float>();
		for (float &v : n) v = r.get<float>();
		uint32_t c = r.get<uint32_t>();
		r.get_bytes(bones.ptrw() + i * CHANNELS, CHANNELS);
		r.get_bytes(weights.ptrw() + i * CHANNELS, CHANNELS);
		float detail = r.get<float>();
		pos.set(i, Vector3(p[0], p[1], p[2]));
		nor.set(i, Vector3(n[0], n[1], n[2]));
		// the first three weights in UV and COLOR.a (the fourth is what they leave of 1): standard attributes every
		// renderer carries (the Compatibility renderer dropped CUSTOM1's g, b and a)
		const uint8_t *w = weights.ptr() + i * CHANNELS;
		uv.set(i, Vector2(float(w[0]) / BYTE_MAX, float(w[1]) / BYTE_MAX));
		col.set(i, Color(linear(c, 0), linear(c, BYTE_BITS), linear(c, 2 * BYTE_BITS), float(w[2]) / BYTE_MAX));
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
	arr[Mesh::ARRAY_TEX_UV] = uv;
	arr[Mesh::ARRAY_TEX_UV2] = uv2;
	arr[Mesh::ARRAY_CUSTOM0] = bones;
	arr[Mesh::ARRAY_INDEX] = idx;
	uint64_t flags = uint64_t(Mesh::ARRAY_CUSTOM_RGBA8_UNORM) << Mesh::ARRAY_FORMAT_CUSTOM0_SHIFT;
	Ref<ArrayMesh> m;
	m.instantiate();
	m->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arr, Array(), Dictionary(), BitField<Mesh::ArrayFormat>(flags));
	return m;
}
} // namespace

bool PeopleAsset::load(const String &path, PackedStringArray &errors) {
	PackedByteArray raw = FileAccess::get_file_as_bytes(path);
	ByteReader r{ raw.ptr(), size_t(raw.size()) };
	uint32_t magic = r.get<uint32_t>(), version = r.get<uint32_t>(), count = r.get<uint32_t>(), bones = r.get<uint32_t>();
	uint32_t lod_count = r.get<uint32_t>(), pal_w = r.get<uint32_t>(), pal_h = r.get<uint32_t>();
	if (!r.ok || magic != MAGIC || version != VERSION || count == 0 || count > MAX_VARIATIONS || lod_count == 0 || lod_count > MAX_LODS ||
			pal_w != bones * BONE_TEXELS) {
		errors.push_back("pessoas: arquivo ausente ou inválido: " + path);
		return false;
	}
	lods = int(lod_count);
	variations.assign(count, PersonVariation());
	std::vector<LodSize> sizes(size_t(count) * lod_count);
	for (uint32_t v = 0; v < count; v++) {
		PersonVariation &pv = variations[v];
		pv.palette_row = int(r.get<uint32_t>());
		pv.frames = int(r.get<uint32_t>());
		pv.fps = r.get<float>();
		pv.walk_advance = r.get<float>();
		pv.height = r.get<float>();
		uint32_t flags = r.get<uint32_t>();
		pv.female = flags & FLAG_FEMALE;
		pv.senior = flags & FLAG_SENIOR;
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
