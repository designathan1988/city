// Reads project/assets/people/people.bin (layout in tools/assets/cook_people.py) into ArrayMeshes and the palette texture, and atlas.bin
// into the array of atlases. The file is checked end to end: a short or inconsistent file is refused with a message.
#include "render/agents/people_asset.h"
#include "core/shader_contracts/people.h"

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
#include <godot_cpp/variant/typed_array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t MAGIC = 0x50504352u; // "RCPP"
constexpr uint32_t FACE_TEXELS = PEOPLE_FACE_SPAN * PEOPLE_FACE_CHANNELS, FACE_TEXEL_BYTES = 8, MAX_FACE_LINES = 16384; // half floats
constexpr uint32_t VERSION = 8, REGION_SHIFT = 6, ROUGHNESS_MASK = 63, FLAG_FEMALE = 1, FLAG_SENIOR = 2, FLAG_CHILD = 4;
constexpr uint32_t MAX_VARIATIONS = 256, MAX_LODS = 4, MAX_CLIPS = 64, CLIP_NAME_BYTES = 16, MAX_VERTICES = 65535, RGBA = 4;
constexpr int BYTE_MAX = 255, BYTE_BITS = 8, CHANNELS = 4;
// What one vertex takes in the file (read_mesh): position, normal, colour, bone bytes, weight bytes, uv, face row.
constexpr size_t VERTEX_BYTES = 6 * sizeof(float) + sizeof(uint32_t) + 2 * CHANNELS + 2 * sizeof(float) + sizeof(uint32_t);
constexpr float SRGB_GAMMA = 2.2f;
constexpr float BYTE_STEPS = 256.f, FIXED_MOST = 65535.f; // cook_people.py fixed_point
constexpr float CARD_HALF = 0.5f;

// The vertex and index counts of one level of a person's mesh, as people.bin gives them.
struct LodSize {
	uint32_t vertices = 0, indices = 0;
};

float linear(uint32_t c, int shift) { return std::pow(float((c >> shift) & BYTE_MAX) / float(BYTE_MAX), SRGB_GAMMA); }

// The level's mesh; `reach`: its farthest vertex from the bind origin.
Ref<ArrayMesh> read_mesh(ByteReader &r, const LodSize &sz, float &reach) {
	PackedVector3Array pos, nor;
	PackedColorArray col;
	PackedVector2Array uv, uv2;
	PackedInt32Array bones;
	PackedFloat32Array weights;
	PackedInt32Array idx;
	// every count is claimed against what is left of the file before anything is sized by it (core/bytes.h)
	if (!r.claim(sz.vertices, VERTEX_BYTES)) return Ref<ArrayMesh>();
	pos.resize(sz.vertices);
	nor.resize(sz.vertices);
	col.resize(sz.vertices);
	uv.resize(sz.vertices);
	uv2.resize(sz.vertices);
	bones.resize(int64_t(sz.vertices) * CHANNELS);
	weights.resize(int64_t(sz.vertices) * CHANNELS);
	uint8_t b8[CHANNELS], w8[CHANNELS];
	for (uint32_t i = 0; i < sz.vertices; i++) {
		float p[3], n[3], t[2];
		for (float &v : p) v = r.get<float>();
		for (float &v : n) v = r.get<float>();
		uint32_t c = r.get<uint32_t>();
		r.get_bytes(b8, CHANNELS);
		r.get_bytes(w8, CHANNELS);
		for (float &v : t) v = r.get<float>();
		const uint32_t face = r.get<uint32_t>();
		pos.set(i, Vector3(p[0], p[1], p[2]));
		reach = std::max(reach, Vector3(p[0], p[1], p[2]).length());
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
		const uint32_t look = c >> (3 * BYTE_BITS);
		uv.set(i, Vector2(t[0], t[1]));
		uv2.set(i, Vector2(float(look >> REGION_SHIFT) + PEOPLE_FACE_ROW_STEP * float(face), float(look & ROUGHNESS_MASK) / float(ROUGHNESS_MASK)));
	}
	if (!r.claim(sz.indices, sizeof(uint16_t))) return Ref<ArrayMesh>();
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
// The atlases of atlas.bin (tools/assets/compress_atlas.gd), compressed and with their mipmaps as they are to be drawn: nothing to decode.
Ref<Texture2DArray> read_atlas(const String &path, uint32_t count) {
	const PackedByteArray raw = FileAccess::get_file_as_bytes(path);
	ByteReader r{ raw.ptr(), size_t(raw.size()) };
	const uint32_t layers = r.get<uint32_t>(), side = r.get<uint32_t>(), each = r.get<uint32_t>();
	if (!r.ok || layers != count || side == 0 || size_t(each) * layers + r.o != r.n) return Ref<Texture2DArray>();
	TypedArray<Image> images;
	for (uint32_t i = 0; i < layers; i++) {
		PackedByteArray data;
		data.resize(each);
		r.get_bytes(data.ptrw(), each);
		images.push_back(Image::create_from_data(int(side), int(side), true, Image::FORMAT_DXT5, data));
	}
	Ref<Texture2DArray> out;
	out.instantiate();
	return out->create_from_images(images) == OK ? out : Ref<Texture2DArray>();
}

// One quad, x and y in -0.5..0.5: people_far.gdshader turns it to the eye.
Ref<ArrayMesh> card_mesh() {
	PackedVector3Array pos, nor;
	PackedVector2Array uv;
	PackedInt32Array idx;
	for (const Vector2 &corner : { Vector2(-CARD_HALF, -CARD_HALF), Vector2(CARD_HALF, -CARD_HALF), Vector2(CARD_HALF, CARD_HALF), Vector2(-CARD_HALF, CARD_HALF) }) {
		pos.push_back(Vector3(corner.x, corner.y, 0));
		nor.push_back(Vector3(0, 0, 1));
		uv.push_back(corner);
	}
	for (int i : { 0, 2, 1, 0, 3, 2 }) idx.push_back(i);
	Array arr;
	arr.resize(Mesh::ARRAY_MAX);
	arr[Mesh::ARRAY_VERTEX] = pos;
	arr[Mesh::ARRAY_NORMAL] = nor;
	arr[Mesh::ARRAY_TEX_UV] = uv;
	arr[Mesh::ARRAY_INDEX] = idx;
	Ref<ArrayMesh> m;
	m.instantiate();
	m->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arr);
	return m;
}
void read_fit(ByteReader &r, PersonFit &f) {
	f.pelvis.x = r.get<float>();
	f.pelvis.y = r.get<float>();
	f.pelvis.z = r.get<float>();
	f.above = r.get<float>();
	f.below = r.get<float>();
	f.ahead = r.get<float>();
	f.side = r.get<float>();
}
} // namespace

int PeopleAsset::clip(const char *name) const {
	for (size_t i = 0; i < clips.size(); i++)
		if (clips[i].name == name) return int(i);
	return -1;
}

void PeopleAsset::read_card(ByteReader &r) {
	card_frames = int(r.get<uint32_t>());
	card_strip.r = r.get<float>();
	card_strip.g = r.get<float>();
	card_strip.b = r.get<float>();
	card_strip.a = r.get<float>();
}

void PeopleAsset::read_ragdoll(ByteReader &r, uint32_t bones) {
	if (!r.claim(uint64_t(bones) + uint64_t(JOINT_COUNT), 1)) return;
	bone_part.resize(bones);
	joint_bone.resize(size_t(JOINT_COUNT));
	if (!r.ok) return;
	r.get_bytes(bone_part.data(), bone_part.size());
	r.get_bytes(joint_bone.data(), joint_bone.size());
	for (PersonVariation &pv : variations)
		for (float &f : pv.joints) f = r.get<float>();
}

void PeopleAsset::clip_rows(int variation, int clip_index, int frame, float *rows) const {
	const size_t row = size_t(variation) * size_t(block_rows) + size_t(clips[size_t(clip_index)].row + frame);
	const size_t values = bone_part.size() * PEOPLE_BONE_TEXELS, line = values * PEOPLE_PALETTE_TEXELS * RGBA;
	const uint8_t *from = palette_bytes.ptr() + row * line;
	for (size_t t = 0; t < values; t++)
		for (size_t c = 0; c < RGBA; c++) { // cook_people.py fixed_point: high bytes in a texel, low bytes in the next
			const float fixed = float(from[t * PEOPLE_PALETTE_TEXELS * RGBA + c]) * BYTE_STEPS + float(from[(t * PEOPLE_PALETTE_TEXELS + 1) * RGBA + c]);
			rows[t * RGBA + c] = fixed / FIXED_MOST * 2.f * PEOPLE_PALETTE_SPAN - PEOPLE_PALETTE_SPAN;
		}
}

bool PeopleAsset::load(const String &path, const String &atlas_path, PackedStringArray &errors) {
	PackedByteArray raw = FileAccess::get_file_as_bytes(path);
	ByteReader r{ raw.ptr(), size_t(raw.size()) };
	uint32_t magic = r.get<uint32_t>(), version = r.get<uint32_t>(), count = r.get<uint32_t>(), bones = r.get<uint32_t>();
	uint32_t lod_count = r.get<uint32_t>(), pal_w = r.get<uint32_t>();
	block_rows = int(r.get<uint32_t>());
	const uint32_t clip_count = r.get<uint32_t>();
	hand_bone = int(r.get<uint32_t>());
	head_bone = int(r.get<uint32_t>());
	const uint32_t pal_h = uint32_t(block_rows) * count;
	bool sound = r.ok && magic == MAGIC && version == VERSION && count > 0 && count <= MAX_VARIATIONS && lod_count > 0 && lod_count <= MAX_LODS &&
			bones == uint32_t(PEOPLE_BONES) && pal_w == bones * PEOPLE_BONE_TEXELS && clip_count > 0 && clip_count <= MAX_CLIPS;
	clips.assign(sound ? clip_count : 0, PersonClip());
	for (PersonClip &c : clips) {
		char name[CLIP_NAME_BYTES + 1] = {};
		r.get_bytes(reinterpret_cast<uint8_t *>(name), CLIP_NAME_BYTES);
		c.name = name;
		c.row = int(r.get<uint32_t>());
		c.frames = int(r.get<uint32_t>());
		for (float &s : c.seconds) s = r.get<float>();
		sound = sound && c.frames > 0 && c.row + c.frames <= block_rows;
	}
	if (!r.ok || !sound || clip("idle") < 0 || hand_bone < 0 || uint32_t(hand_bone) >= bones) {
		errors.push_back("pessoas: arquivo ausente ou inválido: " + path);
		return false;
	}
	read_card(r);
	lods = int(lod_count);
	variations.assign(count, PersonVariation());
	std::vector<LodSize> sizes(size_t(count) * lod_count);
	for (uint32_t v = 0; v < count; v++) {
		PersonVariation &pv = variations[v];
		pv.walk_frames = int(r.get<uint32_t>());
		pv.walk_advance = r.get<float>();
		pv.height = r.get<float>();
		uint32_t flags = r.get<uint32_t>();
		pv.female = flags & FLAG_FEMALE;
		pv.senior = flags & FLAG_SENIOR;
		pv.child = flags & FLAG_CHILD;
		pv.alpha_test = r.get<float>();
		for (int k = 0; k < 3; k++) r.get<float>(); // where a helmet sits on this body, for the cook's own use
		pv.mood = r.get<float>();
		for (float &f : pv.palm) f = r.get<float>();
		for (float &f : pv.helmet) f = r.get<float>();
		for (uint32_t l = 0; l < lod_count; l++) sizes[v * lod_count + l] = { r.get<uint32_t>(), r.get<uint32_t>() };
		pv.fits.resize(clips.size());
		for (PersonFit &f : pv.fits) read_fit(r, f);
	}
	for (uint32_t i = 0; i < count * lod_count && r.ok; i++) {
		if (sizes[i].vertices > MAX_VERTICES) r.ok = false;
		else {
			PersonVariation &pv = variations[i / lod_count];
			float reach = 0;
			pv.lods.push_back(read_mesh(r, sizes[i], reach));
			if (i % lod_count == 0) pv.reach = reach;
		}
	}
	PackedByteArray texels;
	r.claim(uint64_t(pal_w) * pal_h * RGBA * PEOPLE_PALETTE_TEXELS, 1);
	if (r.ok) texels.resize(int64_t(pal_w) * pal_h * RGBA * PEOPLE_PALETTE_TEXELS);
	if (r.ok) r.get_bytes(texels.ptrw(), size_t(texels.size()));
	const uint32_t face_lines = r.get<uint32_t>();
	if (face_lines == 0 || face_lines > MAX_FACE_LINES) r.ok = false;
	PackedByteArray face_bytes;
	if (r.ok && r.claim(uint64_t(face_lines) * FACE_TEXELS * FACE_TEXEL_BYTES, 1)) {
		face_bytes.resize(int64_t(face_lines) * FACE_TEXELS * FACE_TEXEL_BYTES);
		r.get_bytes(face_bytes.ptrw(), size_t(face_bytes.size()));
	}
	read_ragdoll(r, bones);
	if (!r.ok || r.o != r.n) {
		errors.push_back("pessoas: arquivo truncado ou com sobra: " + path);
		return false;
	}
	palette_bytes = texels;
	palette = ImageTexture::create_from_image(Image::create_from_data(int(pal_w) * PEOPLE_PALETTE_TEXELS, int(pal_h), false, Image::FORMAT_RGBA8, texels));
	faces = ImageTexture::create_from_image(Image::create_from_data(int(FACE_TEXELS), int(face_lines), false, Image::FORMAT_RGBAH, face_bytes));
	card = card_mesh();
	atlas = read_atlas(atlas_path, count);
	if (atlas.is_null()) {
		errors.push_back("pessoas: atlas ausente ou inválido: " + atlas_path);
		return false;
	}
	return true;
}

} // namespace city
