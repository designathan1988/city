// The streams of a mesh surface (surface_streams.h), byte for byte as Godot 4.7's RenderingServer::_surface_set_data writes them.
#include "render/surface_streams.h"

#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_color_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <algorithm>
#include <cstring>
#include <vector>

using namespace godot;

namespace city::surface {

namespace {

using RS = RenderingServer;

// The engine's own numbers (rendering_server.cpp, 4.7-stable).
constexpr float U16_TOP = 65535.f; // a unit share stored in sixteen bits
constexpr float CHANNEL_BYTE_TOP = 255.f; // a colour channel in eight
constexpr float SMALL_SIDE = 0.001f; // SMALL_VEC3: an AABB side is never under this
constexpr int SHORT_INDEX_MOST = 1 << 16; // up to this many vertices the indices are sixteen bits
constexpr int BONE_GROUP = 4; // RS::ARRAY_WEIGHTS_SIZE: bones a vertex, or twice that with ARRAY_FLAG_USE_8_BONE_WEIGHTS
constexpr float HALF = 0.5f;
constexpr float ANGLE_LOW_MOST = 0.49999f, ANGLE_HIGH_LEAST = 0.500008f; // _get_axis_angle: the angle's two halves keep apart
constexpr int VERTEX_FLOATS = 3, COMPRESSED_VERTEX_FLOATS = 2; // a position: three floats, or four sixteen-bit values
constexpr int RG_HALF = 4, RGBA_HALF = 8; // bytes of the half-float custom formats

uint16_t u16(float share) {
	return uint16_t(std::clamp(share * U16_TOP, 0.f, U16_TOP));
}

uint64_t custom_format(uint64_t format, int i) {
	return (format >> (RS::ARRAY_FORMAT_CUSTOM_BASE + RS::ARRAY_FORMAT_CUSTOM_BITS * (i - RS::ARRAY_CUSTOM0))) & RS::ARRAY_FORMAT_CUSTOM_MASK;
}

int custom_bytes(uint64_t type) {
	switch (type) {
		case RS::ARRAY_CUSTOM_RGBA8_UNORM:
		case RS::ARRAY_CUSTOM_RGBA8_SNORM:
		case RS::ARRAY_CUSTOM_RG_HALF:
		case RS::ARRAY_CUSTOM_R_FLOAT: return RG_HALF;
		case RS::ARRAY_CUSTOM_RGBA_HALF:
		case RS::ARRAY_CUSTOM_RG_FLOAT: return RGBA_HALF;
		case RS::ARRAY_CUSTOM_RGB_FLOAT: return int(sizeof(float)) * VERTEX_FLOATS;
		case RS::ARRAY_CUSTOM_RGBA_FLOAT: return int(sizeof(float)) * BONE_GROUP;
		default: return 0;
	}
}

int bone_group(uint64_t format) {
	return (format & RS::ARRAY_FLAG_USE_8_BONE_WEIGHTS) ? 2 * BONE_GROUP : BONE_GROUP;
}

// _get_axis_angle: a normal and tangent as the rotation of the tangent frame, its angle carrying the binormal's sign.
void axis_angle(const Vector3 &normal_in, const Vector4 &tangent_in, float &angle, Vector3 &axis) {
	const Vector3 normal = normal_in.normalized(), tangent = Vector3(tangent_in.x, tangent_in.y, tangent_in.z).normalized();
	const Vector3 binormal = normal.cross(tangent).normalized();
	Basis tbn;
	tbn.rows[0] = tangent;
	tbn.rows[1] = binormal;
	tbn.rows[2] = normal;
	real_t a = 0;
	tbn.get_axis_angle(axis, a);
	angle = float(a);
	if (tangent_in.w < 0) angle = std::clamp((1.f - angle / float(Math::PI)) * HALF, 0.f, ANGLE_LOW_MOST);
	else angle = std::clamp((angle / float(Math::PI)) * HALF + HALF, ANGLE_HIGH_LEAST, 1.f);
}

// The tangent the engine makes up for a normal with none (_surface_set_data, no tangent array).
Vector4 made_tangent(const Vector3 &n) {
	const Vector3 t = Vector3(n.z, -n.x, n.y).cross(n.normalized()).normalized();
	return Vector4(t.x, t.y, t.z, 1);
}

void put_u16x2(uint8_t *at, const Vector2 &v) {
	const uint16_t w[2] = { u16(v.x), u16(v.y) };
	std::memcpy(at, w, sizeof(w));
}

// A position: packed into the box (with the tangent frame's angle), or as three floats.
void put_position(uint8_t *at, const Vector3 &p, const AABB &box, bool packed, float angle) {
	if (packed) {
		const Vector3 s = (p - box.position) / box.size;
		const uint16_t w[4] = { u16(s.x), u16(s.y), u16(s.z), u16(angle) };
		std::memcpy(at, w, sizeof(w));
		return;
	}
	const float f[VERTEX_FLOATS] = { p.x, p.y, p.z };
	std::memcpy(at, f, sizeof(f));
}

// An index array into `out` as the engine stores it for `vertex_count` vertices.
void put_indices(const PackedInt32Array &idx, int count, bool short_index, uint8_t *out) {
	for (int i = 0; i < count; i++) {
		if (short_index) {
			const uint16_t v = uint16_t(idx[i]);
			std::memcpy(out + size_t(i) * 2, &v, 2);
		} else {
			const uint32_t v = uint32_t(idx[i]);
			std::memcpy(out + size_t(i) * 4, &v, 4);
		}
	}
}

} // namespace

uint64_t format_of(const Array &arrays, uint64_t compress_format, int vertex_count, int &index_count) {
	uint64_t format = 0;
	index_count = 0;
	for (int i = 0; i < RS::ARRAY_MAX; i++) {
		if (arrays[i].get_type() == Variant::NIL) continue;
		format |= 1ULL << i;
		if (i == RS::ARRAY_NORMAL && arrays[RS::ARRAY_TANGENT].get_type() == Variant::NIL) format |= 1ULL << RS::ARRAY_TANGENT;
		if (i == RS::ARRAY_BONES && int(PackedInt32Array(arrays[i]).size()) / (BONE_GROUP * 2) == vertex_count) format |= RS::ARRAY_FLAG_USE_8_BONE_WEIGHTS;
		if (i == RS::ARRAY_INDEX) index_count = int(PackedInt32Array(arrays[i]).size());
	}
	for (int c = 0; c < RS::ARRAY_CUSTOM_COUNT; c++)
		if (format & (1ULL << (RS::ARRAY_CUSTOM0 + c)))
			format |= (uint64_t(RS::ARRAY_FORMAT_CUSTOM_MASK) << (RS::ARRAY_FORMAT_CUSTOM_BASE + c * RS::ARRAY_FORMAT_CUSTOM_BITS)) & compress_format;
	const uint64_t mask = (1ULL << RS::ARRAY_MAX) - 1ULL;
	format |= ~mask & compress_format;
	format &= ~(uint64_t(RS::ARRAY_FLAG_FORMAT_VERSION_MASK) << RS::ARRAY_FLAG_FORMAT_VERSION_SHIFT);
	format |= uint64_t(RS::ARRAY_FLAG_FORMAT_CURRENT_VERSION) & (uint64_t(RS::ARRAY_FLAG_FORMAT_VERSION_MASK) << RS::ARRAY_FLAG_FORMAT_VERSION_SHIFT);
	return format;
}

Layout layout_of(uint64_t format, int vertex_count) {
	Layout l;
	uint32_t *accum = nullptr;
	const bool packed = format & RS::ARRAY_FLAG_COMPRESS_ATTRIBUTES;
	for (int i = 0; i < RS::ARRAY_MAX; i++) {
		if (i == RS::ARRAY_VERTEX) accum = &l.vertex;
		else if (i == RS::ARRAY_NORMAL) accum = &l.normal;
		else if (i == RS::ARRAY_COLOR) accum = &l.attrib;
		else if (i == RS::ARRAY_BONES) accum = &l.skin;
		if (!(format & (1ULL << i))) continue;
		uint32_t size = 0;
		switch (i) {
			case RS::ARRAY_VERTEX: size = uint32_t(sizeof(float)) * (packed ? COMPRESSED_VERTEX_FLOATS : VERTEX_FLOATS); break;
			case RS::ARRAY_NORMAL: size = sizeof(uint16_t) * 2; break;
			case RS::ARRAY_TANGENT: size = packed ? 0 : sizeof(uint16_t) * 2; break;
			case RS::ARRAY_COLOR: size = BONE_GROUP; break;
			case RS::ARRAY_TEX_UV:
			case RS::ARRAY_TEX_UV2: size = packed ? sizeof(uint16_t) * 2 : sizeof(float) * 2; break;
			case RS::ARRAY_WEIGHTS:
			case RS::ARRAY_BONES: size = uint32_t(sizeof(uint16_t)) * uint32_t(bone_group(format)); break;
			case RS::ARRAY_INDEX: l.offset[i] = vertex_count <= SHORT_INDEX_MOST && vertex_count > 0 ? 2 : 4; continue;
			default: size = uint32_t(custom_bytes(custom_format(format, i))); break; // the four custom arrays
		}
		l.offset[i] = *accum;
		if (i == RS::ARRAY_NORMAL || i == RS::ARRAY_TANGENT) l.offset[i] += l.vertex * uint32_t(vertex_count);
		*accum += size;
	}
	return l;
}

AABB box_of(const Array &arrays) {
	const PackedVector3Array p = arrays[RS::ARRAY_VERTEX];
	Vector3 lo = p[0], hi = p[0];
	for (int64_t i = 1; i < p.size(); i++) {
		lo = lo.min(p[i]);
		hi = hi.max(p[i]);
	}
	return AABB(lo, (hi - lo).max(Vector3(SMALL_SIDE, SMALL_SIDE, SMALL_SIDE)));
}

Vector4 uv_scale_of(const Array &arrays, uint64_t format, int n) {
	if (!(format & RS::ARRAY_FLAG_COMPRESS_ATTRIBUTES) || !(format & (RS::ARRAY_FORMAT_TEX_UV | RS::ARRAY_FORMAT_TEX_UV2))) return Vector4();
	Vector2 top, low, top2, low2;
	const PackedVector2Array uv = format & RS::ARRAY_FORMAT_TEX_UV ? PackedVector2Array(arrays[RS::ARRAY_TEX_UV]) : PackedVector2Array();
	const PackedVector2Array uv2 = format & RS::ARRAY_FORMAT_TEX_UV2 ? PackedVector2Array(arrays[RS::ARRAY_TEX_UV2]) : PackedVector2Array();
	for (int i = 0; i < n; i++) {
		if (i < uv.size()) {
			top = top.max(uv[i]);
			low = low.min(uv[i]);
		}
		if (i < uv2.size()) {
			top2 = top2.max(uv2[i]);
			low2 = low2.min(uv2[i]);
		}
	}
	const bool unit = low.x >= 0 && low2.x >= 0 && top.x <= 1 && top2.x <= 1 && low.y >= 0 && low2.y >= 0 && top.y <= 1 && top2.y <= 1;
	top = top.abs().max(low.abs());
	top2 = top2.abs().max(low2.abs());
	return unit ? Vector4() : Vector4(top.x, top.y, top2.x, top2.y) * 2;
}

bool write_vertices(const Array &arrays, uint64_t format, const Layout &l, const AABB &box, int n, uint8_t *vw) {
	const PackedVector3Array p = arrays[RS::ARRAY_VERTEX];
	const bool packed = format & RS::ARRAY_FLAG_COMPRESS_ATTRIBUTES;
	if (!(format & RS::ARRAY_FORMAT_NORMAL)) {
		for (int i = 0; i < n; i++) put_position(vw + l.offset[RS::ARRAY_VERTEX] + size_t(i) * l.vertex, p[i], box, packed, 0);
		return true;
	}
	const PackedVector3Array normals = arrays[RS::ARRAY_NORMAL];
	const PackedFloat32Array tangents = arrays[RS::ARRAY_TANGENT].get_type() == Variant::NIL ? PackedFloat32Array() : PackedFloat32Array(arrays[RS::ARRAY_TANGENT]);
	if (normals.size() != n || (!tangents.is_empty() && tangents.size() != int64_t(n) * BONE_GROUP)) return false;
	for (int i = 0; i < n; i++) {
		const int t = i * BONE_GROUP;
		const Vector4 tangent = tangents.is_empty() ? made_tangent(normals[i]) : Vector4(tangents[t], tangents[t + 1], tangents[t + 2], tangents[t + 3]);
		uint8_t *normal_at = vw + l.offset[RS::ARRAY_NORMAL] + size_t(i) * l.normal;
		if (packed) {
			float angle = 0;
			Vector3 axis;
			axis_angle(normals[i], tangent, angle, axis);
			put_u16x2(normal_at, axis.octahedron_encode());
			put_position(vw + l.offset[RS::ARRAY_VERTEX] + size_t(i) * l.vertex, p[i], box, true, angle);
			continue;
		}
		put_u16x2(normal_at, normals[i].octahedron_encode());
		const Vector2 e = Vector3(tangent.x, tangent.y, tangent.z).octahedron_tangent_encode(tangent.w);
		uint16_t w[2] = { u16(e.x), u16(e.y) };
		if (w[0] == 0 && w[1] == uint16_t(U16_TOP)) w[0] = uint16_t(U16_TOP); // the engine's guard against a degenerate sign
		std::memcpy(vw + l.offset[RS::ARRAY_TANGENT] + size_t(i) * l.normal, w, sizeof(w));
		put_position(vw + l.offset[RS::ARRAY_VERTEX] + size_t(i) * l.vertex, p[i], box, false, 0);
	}
	return true;
}

bool write_attributes(const Array &arrays, uint64_t format, const Layout &l, const Vector4 &uv_scale, int n, uint8_t *aw) {
	const bool packed = format & RS::ARRAY_FLAG_COMPRESS_ATTRIBUTES;
	if (format & RS::ARRAY_FORMAT_COLOR) {
		const PackedColorArray colours = arrays[RS::ARRAY_COLOR];
		if (colours.size() != n) return false;
		for (int i = 0; i < n; i++) {
			const Color &c = colours[i];
			const uint8_t b[4] = { uint8_t(std::clamp(c.r * CHANNEL_BYTE_TOP, 0.f, CHANNEL_BYTE_TOP)), uint8_t(std::clamp(c.g * CHANNEL_BYTE_TOP, 0.f, CHANNEL_BYTE_TOP)),
				uint8_t(std::clamp(c.b * CHANNEL_BYTE_TOP, 0.f, CHANNEL_BYTE_TOP)), uint8_t(std::clamp(c.a * CHANNEL_BYTE_TOP, 0.f, CHANNEL_BYTE_TOP)) };
			std::memcpy(aw + l.offset[RS::ARRAY_COLOR] + size_t(i) * l.attrib, b, sizeof(b));
		}
	}
	for (int k : { int(RS::ARRAY_TEX_UV), int(RS::ARRAY_TEX_UV2) }) {
		if (!(format & (1ULL << k))) continue;
		const PackedVector2Array uv = arrays[k];
		if (uv.size() != n) return false;
		const Vector2 scale = k == RS::ARRAY_TEX_UV ? Vector2(uv_scale.x, uv_scale.y) : Vector2(uv_scale.z, uv_scale.w);
		for (int i = 0; i < n; i++) {
			uint8_t *at = aw + l.offset[k] + size_t(i) * l.attrib;
			if (packed) {
				put_u16x2(at, uv_scale.is_zero_approx() ? uv[i] : uv[i] / scale + Vector2(HALF, HALF));
			} else {
				const float f[2] = { uv[i].x, uv[i].y };
				std::memcpy(at, f, sizeof(f));
			}
		}
	}
	for (int k = RS::ARRAY_CUSTOM0; k <= RS::ARRAY_CUSTOM3; k++) {
		if (!(format & (1ULL << k))) continue;
		const uint64_t type = custom_format(format, k);
		const int bytes = custom_bytes(type);
		const bool floats = type >= RS::ARRAY_CUSTOM_R_FLOAT;
		const PackedFloat32Array f = floats ? PackedFloat32Array(arrays[k]) : PackedFloat32Array();
		const PackedByteArray b = floats ? PackedByteArray() : PackedByteArray(arrays[k]);
		const uint8_t *src = floats ? reinterpret_cast<const uint8_t *>(f.ptr()) : b.ptr();
		const int64_t given = floats ? f.size() * int64_t(sizeof(float)) : b.size();
		if (given != int64_t(n) * bytes) return false;
		for (int i = 0; i < n; i++) std::memcpy(aw + l.offset[k] + size_t(i) * l.attrib, src + size_t(i) * size_t(bytes), size_t(bytes));
	}
	return true;
}

bool write_skin(const Array &arrays, uint64_t format, const Layout &l, int n, uint8_t *sw, Array &bone_boxes) {
	const int group = bone_group(format);
	if (format & RS::ARRAY_FORMAT_WEIGHTS) {
		const PackedFloat32Array w = arrays[RS::ARRAY_WEIGHTS];
		if (w.size() != int64_t(n) * group) return false;
		for (int i = 0; i < n; i++)
			for (int j = 0; j < group; j++) {
				const uint16_t v = u16(w[i * group + j]);
				std::memcpy(sw + l.offset[RS::ARRAY_WEIGHTS] + size_t(i) * l.skin + size_t(j) * sizeof(uint16_t), &v, sizeof(v));
			}
	}
	if (!(format & RS::ARRAY_FORMAT_BONES)) return true;
	const PackedInt32Array b = arrays[RS::ARRAY_BONES];
	const PackedFloat32Array w = arrays[RS::ARRAY_WEIGHTS];
	const PackedVector3Array p = arrays[RS::ARRAY_VERTEX];
	if (b.size() != int64_t(n) * group) return false;
	int most = 0;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < group; j++) {
			const uint16_t v = uint16_t(b[i * group + j]);
			most = std::max(most, int(v));
			std::memcpy(sw + l.offset[RS::ARRAY_BONES] + size_t(i) * l.skin + size_t(j) * sizeof(uint16_t), &v, sizeof(v));
		}
	// each bone's box round the vertices it weighs on (a negative size: a bone that weighs on none)
	std::vector<AABB> boxes(size_t(most) + 1, AABB(Vector3(), Vector3(-1, -1, -1)));
	bool any = false;
	if (w.size() == b.size())
		for (int i = 0; i < n; i++)
			for (int j = 0; j < group; j++) {
				if (w[i * group + j] == 0) continue;
				AABB &bb = boxes[size_t(b[i * group + j])];
				if (bb.size.x < 0) bb = AABB(p[i], Vector3(SMALL_SIDE, SMALL_SIDE, SMALL_SIDE));
				else bb.expand_to(p[i]);
				any = true;
			}
	if (any)
		for (const AABB &bb : boxes) bone_boxes.push_back(bb);
	return true;
}

void write_indices(const Array &arrays, const Layout &l, int index_count, uint8_t *iw) {
	if (index_count > 0) put_indices(arrays[RS::ARRAY_INDEX], index_count, l.offset[RS::ARRAY_INDEX] == 2, iw);
}

Array levels_of(const Dictionary &lods, int index_count, int vertex_count) {
	Array levels;
	if (index_count == 0 || lods.is_empty()) return levels;
	Array keys = lods.keys();
	keys.sort();
	const bool short_index = vertex_count <= SHORT_INDEX_MOST;
	for (int k = 0; k < keys.size(); k++) {
		const float edge = float(keys[k]);
		const PackedInt32Array li = lods[keys[k]];
		if (edge <= 0 || li.is_empty() || li.size() >= index_count) continue;
		PackedByteArray data;
		data.resize(li.size() * (short_index ? 2 : 4));
		put_indices(li, int(li.size()), short_index, data.ptrw());
		Dictionary lod;
		lod["edge_length"] = edge;
		lod["index_data"] = data;
		levels.push_back(lod);
	}
	return levels;
}

} // namespace city::surface
