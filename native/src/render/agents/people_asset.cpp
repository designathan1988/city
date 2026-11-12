// Reads project/assets/people/people.bin (layout in tools/assets/cook_people.py) into ArrayMeshes and the palette texture, and atlas.bin
// into the array of atlases. The file is checked end to end: a short or inconsistent file is refused with a message.
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
#include <godot_cpp/variant/typed_array.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

using namespace godot;

namespace city {

namespace {
constexpr uint32_t MAGIC = 0x50504352u; // "RCPP"
constexpr uint32_t FACE_TEXELS = 8 * 13, FACE_TEXEL_BYTES = 8, MAX_FACE_LINES = 16384; // people_face.gdshaderinc FACE_SPAN, FACE_CHANNELS; half floats
constexpr float FACE_ROW_STEP = 4.f; // the same: UV2.x is the region plus this many a row
constexpr uint32_t VERSION = 8, REGION_SHIFT = 6, ROUGHNESS_MASK = 63, FLAG_FEMALE = 1, FLAG_SENIOR = 2, FLAG_CHILD = 4;
constexpr uint32_t MAX_VARIATIONS = 256, MAX_LODS = 4, MAX_CLIPS = 64, CLIP_NAME_BYTES = 16, MAX_VERTICES = 65535, BONE_TEXELS = 3, RGBA = 4;
constexpr int BYTE_MAX = 255, BYTE_BITS = 8, CHANNELS = 4;
constexpr float SRGB_GAMMA = 2.2f;
constexpr float BYTE_STEPS = 256.f, FIXED_MOST = 65535.f, PALETTE_SPAN = 3.f; // cook_people.py fixed_point, PALETTE_SPAN
constexpr int TEXELS_PER_VALUE = 2; // the palette comes as it is drawn (cook_people.py fixed_point, decoded in people.gdshader)
constexpr float CARD_HALF = 0.5f;
constexpr size_t VERTEX_BYTES = 8 * sizeof(float) + 2 * sizeof(uint32_t) + 2 * CHANNELS * sizeof(uint8_t);
constexpr uint32_t MAX_BONES = uint32_t(1) << BYTE_BITS; // cooked bone indices are uint8_t
constexpr uint64_t DXT5_BLOCK_SIDE = 4, DXT5_BLOCK_BYTES = 16;

// Float textures are not sampled reliably in the Compatibility renderer (Godot may fall back to 8 bits and clamp), so
struct LodSize {
	uint32_t vertices = 0, indices = 0;
};

float linear(uint32_t c, int shift) { return std::pow(float((c >> shift) & BYTE_MAX) / float(BYTE_MAX), SRGB_GAMMA); }

Ref<ArrayMesh> read_mesh(ByteReader &r, const LodSize &sz, PersonVariation &variation, uint32_t bone_count) {
	if (!r.require_items(sz.vertices, VERTEX_BYTES, MAX_VERTICES)) return Ref<ArrayMesh>();
	const size_t vertices_bytes = size_t(sz.vertices) * VERTEX_BYTES;
	if (sz.indices % 3 || sz.indices > (r.remaining() - vertices_bytes) / sizeof(uint16_t)) { r.ok = false; return Ref<ArrayMesh>(); }
	PackedVector3Array pos, nor;
	PackedColorArray col;
	PackedVector2Array uv, uv2;
	PackedInt32Array bones;
	PackedFloat32Array weights;
	PackedInt32Array idx;
	pos.resize(sz.vertices);
	nor.resize(sz.vertices);
	col.resize(sz.vertices);
	uv.resize(sz.vertices);
	uv2.resize(sz.vertices);
	bones.resize(int64_t(sz.vertices) * CHANNELS);
	weights.resize(int64_t(sz.vertices) * CHANNELS);
	variation.bone_bounds.resize(bone_count);
	constexpr float WEIGHT_TOP = 65535.f; // RenderingServer packs skin weights to uint16 UNORM
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
		nor.set(i, Vector3(n[0], n[1], n[2]));
		col.set(i, Color(linear(c, 0), linear(c, BYTE_BITS), linear(c, 2 * BYTE_BITS), 1));
		// the skin in the engine's own skin attributes (BONE_INDICES / BONE_WEIGHTS in the shader), the weights made to
		// sum to 1 exactly (the cooked bytes may miss it by a unit)
		int sum = 0;
		for (int k = 0; k < CHANNELS; k++) sum += w8[k];
		const Vector3 point(p[0], p[1], p[2]);
		if (!std::isfinite(p[0]) || !std::isfinite(p[1]) || !std::isfinite(p[2])) { r.ok = false; return Ref<ArrayMesh>(); }
		uint32_t packed_sum = 0;
		for (int k = 0; k < CHANNELS; k++) {
			const float weight = sum > 0 ? float(w8[k]) / float(sum) : (k == 0 ? 1.f : 0.f);
			bones.set(int64_t(i) * CHANNELS + k, int32_t(b8[k]));
			weights.set(int64_t(i) * CHANNELS + k, weight);
			packed_sum += uint16_t(std::clamp(weight * WEIGHT_TOP, 0.f, WEIGHT_TOP));
			if (weight == 0) continue;
			if (b8[k] >= bone_count) { r.ok = false; return Ref<ArrayMesh>(); }
			PersonBoneBounds &box = variation.bone_bounds[size_t(std::min(int(b8[k]), PERSON_LAST_SHADER_BONE))];
			if (!box.used) { box.low = box.high = point; box.used = true; }
			else { box.low = box.low.min(point); box.high = box.high.max(point); }
		}
		const double packed_weight = double(packed_sum) / double(WEIGHT_TOP);
		variation.weight_sum_low = std::min(variation.weight_sum_low, packed_weight);
		variation.weight_sum_high = std::max(variation.weight_sum_high, packed_weight);
		const uint32_t look = c >> (3 * BYTE_BITS);
		uv.set(i, Vector2(t[0], t[1]));
		uv2.set(i, Vector2(float(look >> REGION_SHIFT) + FACE_ROW_STEP * float(face), float(look & ROUGHNESS_MASK) / float(ROUGHNESS_MASK)));
	}
	idx.resize(sz.indices);
	for (uint32_t i = 0; i < sz.indices; i++) {
		const uint16_t index = r.get<uint16_t>();
		if (index >= sz.vertices) { r.ok = false; return Ref<ArrayMesh>(); }
		idx.set(i, int32_t(index));
	}
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
	if (layers != count || side == 0 || side > uint32_t(std::numeric_limits<int>::max()) || !r.require_items(layers, each) || size_t(each) * layers != r.remaining()) return Ref<Texture2DArray>();
	uint64_t expected = 0;
	for (uint64_t width = side;; width = std::max(uint64_t(1), width / 2)) {
		const uint64_t blocks = (width + DXT5_BLOCK_SIDE - 1) / DXT5_BLOCK_SIDE;
		expected += blocks * blocks * DXT5_BLOCK_BYTES;
		if (width == 1) break;
	}
	if (each != expected) return Ref<Texture2DArray>();
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
	const size_t joint_bytes = variations.size() * JOINT_FLOATS * sizeof(float);
	if (bones > MAX_BONES || !r.require(size_t(bones) + JOINT_COUNT + joint_bytes)) { r.ok = false; return; }
	bone_part.resize(bones);
	joint_bone.resize(size_t(JOINT_COUNT));
	if (!r.ok) return;
	r.get_bytes(bone_part.data(), bone_part.size());
	r.get_bytes(joint_bone.data(), joint_bone.size());
	for (PersonVariation &pv : variations)
		for (float &f : pv.joints) f = r.get<float>();
}

void PeopleAsset::clip_rows(int variation, int clip_index, int frame, float *rows) const {
	palette_rows(variation, clips[size_t(clip_index)].row + frame, rows);
}

void PeopleAsset::palette_rows(int variation, int frame, float *rows) const {
	const size_t row = size_t(variation) * size_t(block_rows) + size_t(frame);
	const size_t values = bone_part.size() * BONE_TEXELS, line = values * TEXELS_PER_VALUE * RGBA;
	const uint8_t *from = palette_bytes.ptr() + row * line;
	for (size_t t = 0; t < values; t++)
		for (size_t c = 0; c < RGBA; c++) { // cook_people.py fixed_point: high bytes in a texel, low bytes in the next
			const float fixed = float(from[t * TEXELS_PER_VALUE * RGBA + c]) * BYTE_STEPS + float(from[(t * TEXELS_PER_VALUE + 1) * RGBA + c]);
			rows[t * RGBA + c] = fixed / FIXED_MOST * 2.f * PALETTE_SPAN - PALETTE_SPAN;
		}
}

void PeopleAsset::prepare_animation_bounds(const Ref<Image> &face_image) {
	// Every face channel is clamped to [0,1] by face_move. Sum componentwise maxima across the 13 channels;
	// sharing this outward envelope across face rows and variations can only enlarge the bounds.
	Vector3 face_channel[13] = {};
	bool face_finite = true;
	for (int y = 0; y < face_image->get_height(); y++)
		for (int x = 0; x < face_image->get_width(); x++) {
			const Color c = face_image->get_pixel(x, y);
			face_finite = face_finite && std::isfinite(c.r) && std::isfinite(c.g) && std::isfinite(c.b);
			face_channel[x % 13] = face_channel[x % 13].max(Vector3(std::fabs(c.r), std::fabs(c.g), std::fabs(c.b)));
		}
	Vector3 face_extent;
	for (const Vector3 &channel : face_channel) face_extent += channel;
	std::vector<float> rows(bone_part.size() * PALM_FLOATS);
	for (size_t v = 0; v < variations.size(); v++) {
		PersonVariation &pv = variations[v];
		Vector3 low, high;
		bool used = false, finite = face_finite;
		double largest_terms = 1;
		for (int frame = 0; frame < block_rows; frame++) {
			palette_rows(int(v), frame, rows.data());
			for (size_t bone = 0; bone < pv.bone_bounds.size(); bone++) {
				if (bone == size_t(hand_bone) || bone == size_t(head_bone)) {
					PersonBoneMotion &motion = bone == size_t(hand_bone) ? pv.hand_motion : pv.head_motion;
					for (int k = 0; k < PALM_FLOATS; k++) {
						const float value = rows[bone * PALM_FLOATS + size_t(k)];
						motion.low[size_t(k)] = motion.used ? std::min(motion.low[size_t(k)], value) : value;
						motion.high[size_t(k)] = motion.used ? std::max(motion.high[size_t(k)], value) : value;
					}
					motion.used = true;
				}
				const PersonBoneBounds &box = pv.bone_bounds[bone];
				if (!box.used) continue;
				const Vector3 center = (box.low + box.high) * .5f, half = (box.high - box.low) * .5f + face_extent;
				Vector3 posed, extent;
				for (int axis = 0; axis < 3; axis++) {
					const float *r = rows.data() + bone * PALM_FLOATS + size_t(axis) * RGBA;
					posed[axis] = r[0] * center.x + r[1] * center.y + r[2] * center.z + r[3];
					extent[axis] = std::fabs(r[0]) * half.x + std::fabs(r[1]) * half.y + std::fabs(r[2]) * half.z;
					const double terms = std::fabs(double(r[3])) + std::fabs(double(r[0])) * (std::fabs(double(center.x)) + half.x) +
							std::fabs(double(r[1])) * (std::fabs(double(center.y)) + half.y) + std::fabs(double(r[2])) * (std::fabs(double(center.z)) + half.z);
					const double decode_terms = PALETTE_SPAN * (std::fabs(double(center.x)) + half.x + std::fabs(double(center.y)) + half.y + std::fabs(double(center.z)) + half.z + 1);
					largest_terms = std::max({ largest_terms, terms, decode_terms });
					finite = finite && std::isfinite(posed[axis]) && std::isfinite(extent[axis]) && std::isfinite(terms);
				}
				low = used ? low.min(posed - extent) : posed - extent;
				high = used ? high.max(posed + extent) : posed + extent;
				used = true;
			}
		}
		// Frame/clip interpolation and skin weights form convex combinations. Include the engine's UNORM weight sum,
		// normalized weights and arithmetic in fixed-point decode, morphing, blending and accumulation.
		const double error = largest_terms * 128 * std::numeric_limits<float>::epsilon();
		for (int axis = 0; axis < 3; axis++) {
			const double lo = low[axis], hi = high[axis];
			low[axis] = float(std::min({ lo * pv.weight_sum_low, lo * pv.weight_sum_high, hi * pv.weight_sum_low, hi * pv.weight_sum_high }) - error);
			high[axis] = float(std::max({ lo * pv.weight_sum_low, lo * pv.weight_sum_high, hi * pv.weight_sum_low, hi * pv.weight_sum_high }) + error);
		}
		pv.animated_bounds = AABB(low, high - low);
		pv.animated_bounds_ready = used && finite;
	}
}

AABB PeopleAsset::attachment_bounds(size_t variation, bool head, const AABB &rest) const {
	const PersonVariation &pv = variations[variation];
	const PersonBoneMotion &motion = head ? pv.head_motion : pv.hand_motion;
	if (!motion.used || !pv.animated_bounds_ready) return AABB(Vector3(-200000, -20000, -200000), Vector3(400000, 40000, 400000));
	const Vector3 low = rest.position, high = rest.get_end();
	Vector3 lo, hi;
	for (int axis = 0; axis < 3; axis++) {
		double a = motion.low[size_t(axis) * RGBA + 3], b = motion.high[size_t(axis) * RGBA + 3];
		double terms = std::max(std::fabs(a), std::fabs(b));
		for (int k = 0; k < 3; k++) {
			const size_t i = size_t(axis) * RGBA + size_t(k);
			const double m0 = motion.low[i], m1 = motion.high[i];
			const double bottom = std::min({ m0 * low[k], m0 * high[k], m1 * low[k], m1 * high[k] });
			const double top = std::max({ m0 * low[k], m0 * high[k], m1 * low[k], m1 * high[k] });
			a += bottom; b += top;
			terms += std::max(std::fabs(bottom), std::fabs(top));
		}
		// Fixed-point decode error depends on the palette span even when a decoded coefficient is nearly zero.
		const double decode_terms = PALETTE_SPAN * (std::max(std::fabs(double(low.x)), std::fabs(double(high.x))) +
				std::max(std::fabs(double(low.y)), std::fabs(double(high.y))) + std::max(std::fabs(double(low.z)), std::fabs(double(high.z))) + 1);
		const double error = (std::max(terms, decode_terms) + 1) * 128 * std::numeric_limits<float>::epsilon();
		lo[axis] = float(a - error); hi[axis] = float(b + error);
	}
	return AABB(lo, hi - lo);
}

bool PeopleAsset::load(const String &path, const String &atlas_path, PackedStringArray &errors) {
	PackedByteArray raw = FileAccess::get_file_as_bytes(path);
	ByteReader r{ raw.ptr(), size_t(raw.size()) };
	uint32_t magic = r.get<uint32_t>(), version = r.get<uint32_t>(), count = r.get<uint32_t>(), bones = r.get<uint32_t>();
	uint32_t lod_count = r.get<uint32_t>(), pal_w = r.get<uint32_t>();
	const uint32_t rows = r.get<uint32_t>();
	const uint32_t clip_count = r.get<uint32_t>();
	hand_bone = int(r.get<uint32_t>());
	head_bone = int(r.get<uint32_t>());
	const uint64_t height = uint64_t(rows) * count;
	bool sound = r.ok && magic == MAGIC && version == VERSION && count > 0 && count <= MAX_VARIATIONS && lod_count > 0 && lod_count <= MAX_LODS &&
			bones > 0 && bones <= MAX_BONES && pal_w == bones * BONE_TEXELS && rows > 0 && height <= uint64_t(std::numeric_limits<int>::max()) &&
			clip_count > 0 && clip_count <= MAX_CLIPS;
	block_rows = sound ? int(rows) : 0;
	const uint32_t pal_h = sound ? uint32_t(height) : 0;
	constexpr size_t CLIP_BYTES = CLIP_NAME_BYTES + 2 * sizeof(uint32_t) + 2 * sizeof(float);
	sound = sound && r.require_items(clip_count, CLIP_BYTES, MAX_CLIPS);
	clips.assign(sound ? clip_count : 0, PersonClip());
	for (PersonClip &c : clips) {
		char name[CLIP_NAME_BYTES + 1] = {};
		r.get_bytes(reinterpret_cast<uint8_t *>(name), CLIP_NAME_BYTES);
		c.name = name;
		const uint32_t row = r.get<uint32_t>(), frames = r.get<uint32_t>();
		c.row = row <= uint32_t(block_rows) ? int(row) : -1;
		c.frames = frames <= uint32_t(block_rows) ? int(frames) : 0;
		for (float &s : c.seconds) s = r.get<float>();
		sound = sound && c.row >= 0 && c.frames > 0 && c.frames <= block_rows - c.row;
	}
	if (!r.ok || !sound || clip("idle") < 0 || hand_bone < 0 || uint32_t(hand_bone) >= bones || head_bone < 0 || uint32_t(head_bone) >= bones) {
		errors.push_back("pessoas: arquivo ausente ou inválido: " + path);
		return false;
	}
	read_card(r);
	lods = int(lod_count);
	constexpr size_t VARIATION_FIXED_BYTES = 2 * sizeof(uint32_t) + (7 + 2 * PALM_FLOATS) * sizeof(float);
	const size_t variation_bytes = VARIATION_FIXED_BYTES + lod_count * 2 * sizeof(uint32_t) + clips.size() * 7 * sizeof(float);
	if (card_frames <= 0 || !r.require_items(count, variation_bytes, MAX_VARIATIONS)) {
		errors.push_back("pessoas: metadados truncados ou inválidos: " + path);
		return false;
	}
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
		else variations[i / lod_count].lods.push_back(read_mesh(r, sizes[i], variations[i / lod_count], bones));
	}
	PackedByteArray texels;
	const size_t palette_line = size_t(pal_w) * RGBA * TEXELS_PER_VALUE;
	if (r.require_items(pal_h, palette_line)) {
		texels.resize(int64_t(palette_line) * pal_h);
		r.get_bytes(texels.ptrw(), size_t(texels.size()));
	}
	const uint32_t face_lines = r.get<uint32_t>();
	if (face_lines == 0 || !r.require_items(face_lines, FACE_TEXELS * FACE_TEXEL_BYTES, MAX_FACE_LINES)) r.ok = false;
	PackedByteArray face_bytes;
	if (r.ok) {
		face_bytes.resize(int64_t(face_lines) * FACE_TEXELS * FACE_TEXEL_BYTES);
		r.get_bytes(face_bytes.ptrw(), size_t(face_bytes.size()));
	}
	read_ragdoll(r, bones);
	if (!r.ok || r.o != r.n) {
		errors.push_back("pessoas: arquivo truncado ou com sobra: " + path);
		return false;
	}
	palette_bytes = texels;
	palette = ImageTexture::create_from_image(Image::create_from_data(int(pal_w) * TEXELS_PER_VALUE, int(pal_h), false, Image::FORMAT_RGBA8, texels));
	const Ref<Image> face_image = Image::create_from_data(int(FACE_TEXELS), int(face_lines), false, Image::FORMAT_RGBAH, face_bytes);
	prepare_animation_bounds(face_image);
	faces = ImageTexture::create_from_image(face_image);
	card = card_mesh();
	atlas = read_atlas(atlas_path, count);
	if (atlas.is_null()) {
		errors.push_back("pessoas: atlas ausente ou inválido: " + atlas_path);
		return false;
	}
	return true;
}

} // namespace city
