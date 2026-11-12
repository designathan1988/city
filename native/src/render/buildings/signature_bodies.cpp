// The cooked designs of signature buildings as meshes and instances (signature_bodies.h). Layout: tools/assets/cook_signatures.py.
#include "render/buildings/signature_bodies.h"

#include "render/mesh_levels.h"

#include "core/bytes.h"
#include "sim/buildings/b40/signature_kinds.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <limits>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr const char *FOLDER = "res://assets/lots/";
constexpr const char *SOLID_SHADER = "res://render/shaders/signature.gdshader", *BLENDED_SHADER = "res://render/shaders/signature_glass.gdshader";
constexpr uint32_t MAGIC_LOOKS = 0x4D534352u, MAGIC_BODY = 0x42534352u; // "RCSM", "RCSB"
constexpr uint32_t FLAG_BLENDED = 1, FLAG_TWO_SIDED = 2, FLAG_WINDOWS = 4, FLAG_PAINTED_WINDOWS = 8, MOST_SURFACES = 64, MOST_LOOKS = 1024;
constexpr int NAME_BYTES = 24, RECORD_FLOATS = 10, RGBA = 4, NORMAL_BYTES = 4;
constexpr int RED = 0, ROUGHNESS = 3, METALLIC = 4, GLOW = 5, OPACITY = 8, ALPHA_CUT = 9; // a record's floats
constexpr float NORMAL_SCALE = 127.f;

// A file: u32 magic, u32 size unpacked, then the packed bytes.
PackedByteArray unpacked(const String &path, uint32_t magic) {
	const PackedByteArray raw = FileAccess::get_file_as_bytes(path);
	ByteReader r{ raw.ptr(), size_t(raw.size()) };
	const uint32_t found = r.get<uint32_t>(), size = r.get<uint32_t>();
	if (!r.ok || found != magic) return PackedByteArray();
	return raw.slice(int64_t(r.o)).decompress(size, FileAccess::COMPRESSION_DEFLATE);
}
} // namespace

void SignatureBodies::init(const RID &scenario, uint32_t layer) {
	reset();
	table_attempted_ = table_ready_ = false;
	table_.resize(0); looks_.clear(); solid_.unref(); blended_.unref();
	mesh_attempted_.assign(size_t(b40::SIGNATURE_COUNT) + 1, false);
	scenario_ = scenario;
	layer_ = layer;
	meshes_.assign(size_t(b40::SIGNATURE_COUNT) + 1, Ref<ArrayMesh>());
}

void SignatureBodies::reset() {
	for (auto &e : chunks_)
		for (const RID &inst : e.second) RenderingServer::get_singleton()->free_rid(inst);
	chunks_.clear();
}

bool SignatureBodies::read_table() {
	if (table_attempted_) return table_ready_;
	table_attempted_ = true;
	const PackedByteArray table = unpacked(String(FOLDER) + "materials.sig", MAGIC_LOOKS);
	ByteReader r{ table.ptr(), size_t(table.size()) };
	const uint32_t count = r.get<uint32_t>();
	constexpr size_t RECORD_BYTES = NAME_BYTES + (RECORD_FLOATS + 3) * sizeof(uint32_t);
	if (!count || !r.require_items(count, RECORD_BYTES, MOST_LOOKS)) return false;
	std::vector<Look> parsed(count);
	for (Look &l : parsed) {
		if (!r.require(RECORD_BYTES)) return false;
		l.at = r.o;
		r.o += NAME_BYTES;
		for (int i = 0; i < RECORD_FLOATS; i++) r.get<float>();
		r.get<uint32_t>();
		const uint32_t w = r.get<uint32_t>(), h = r.get<uint32_t>();
		if ((w == 0) != (h == 0) || w > uint32_t(std::numeric_limits<int>::max()) || h > uint32_t(std::numeric_limits<int>::max()) ||
				uint64_t(w) * h > r.remaining() / RGBA) return false;
		r.o += size_t(w) * h * RGBA;
	}
	if (!r.ok || r.o != r.n) return false;
	const Ref<Shader> solid = ResourceLoader::get_singleton()->load(SOLID_SHADER), blended = ResourceLoader::get_singleton()->load(BLENDED_SHADER);
	if (solid.is_null() || blended.is_null()) return false;
	table_ = table;
	looks_ = std::move(parsed);
	solid_ = solid; blended_ = blended;
	table_ready_ = true;
	return true;
}

Ref<ShaderMaterial> SignatureBodies::look(uint32_t index) {
	Look &l = looks_[index];
	if (l.made.is_valid()) return l.made;
	ByteReader r{ table_.ptr(), size_t(table_.size()) };
	r.o = l.at + NAME_BYTES;
	float v[RECORD_FLOATS];
	for (float &f : v) f = r.get<float>();
	const uint32_t flags = r.get<uint32_t>(), w = r.get<uint32_t>(), h = r.get<uint32_t>();
	l.made.instantiate();
	l.made->set_shader(flags & FLAG_BLENDED ? blended_ : solid_);
	l.made->set_shader_parameter("albedo", Color(v[RED], v[RED + 1], v[RED + 2], v[OPACITY]));
	l.made->set_shader_parameter("roughness", v[ROUGHNESS]);
	l.made->set_shader_parameter("metallic", v[METALLIC]);
	l.made->set_shader_parameter("emission", Color(v[GLOW], v[GLOW + 1], v[GLOW + 2]));
	l.made->set_shader_parameter("alpha_cut", v[ALPHA_CUT]);
	l.made->set_shader_parameter("two_sided", (flags & FLAG_TWO_SIDED) != 0);
	l.made->set_shader_parameter("windows", (flags & FLAG_WINDOWS) != 0);
	l.made->set_shader_parameter("painted_windows", (flags & FLAG_PAINTED_WINDOWS) != 0);
	if (w > 0) {
		Ref<Image> image = Image::create_from_data(int(w), int(h), false, Image::FORMAT_RGBA8, table_.slice(int64_t(r.o), int64_t(r.o) + int64_t(w) * h * RGBA));
		image->generate_mipmaps();
		l.made->set_shader_parameter("picture", ImageTexture::create_from_image(image));
		l.made->set_shader_parameter("pictured", true);
	}
	return l.made;
}

Ref<ArrayMesh> SignatureBodies::mesh(int kind) {
	if (kind <= 0 || kind > b40::SIGNATURE_COUNT || size_t(kind) >= meshes_.size() || !read_table()) return Ref<ArrayMesh>();
	if (mesh_attempted_[size_t(kind)]) return meshes_[size_t(kind)];
	mesh_attempted_[size_t(kind)] = true;
	const PackedByteArray data = unpacked(String(FOLDER) + b40::SIGNATURE_KINDS[kind - 1] + ".sig", MAGIC_BODY);
	ByteReader r{ data.ptr(), size_t(data.size()) };
	const uint32_t surfaces = r.get<uint32_t>();
	if (!r.ok || surfaces > MOST_SURFACES) return Ref<ArrayMesh>();
	Ref<ArrayMesh> m;
	m.instantiate();
	for (uint32_t s = 0; s < surfaces && r.ok; s++) {
		const uint32_t material = r.get<uint32_t>(), vertices = r.get<uint32_t>(), indices = r.get<uint32_t>();
		constexpr size_t VERTEX_BYTES = 5 * sizeof(float) + NORMAL_BYTES;
		if (material >= looks_.size() || indices % 3 || vertices > uint32_t(std::numeric_limits<int32_t>::max()) ||
				!r.require_items(vertices, VERTEX_BYTES) || size_t(indices) > (r.remaining() - size_t(vertices) * VERTEX_BYTES) / sizeof(uint32_t)) return Ref<ArrayMesh>();
		PackedVector3Array pos, nor;
		PackedVector2Array uv;
		PackedInt32Array idx;
		pos.resize(vertices);
		nor.resize(vertices);
		uv.resize(vertices);
		idx.resize(indices);
		for (uint32_t i = 0; i < vertices; i++) {
			const float x = r.get<float>(), y = r.get<float>(), z = r.get<float>();
			int8_t n[NORMAL_BYTES];
			r.get_bytes(reinterpret_cast<uint8_t *>(n), NORMAL_BYTES);
			const float u = r.get<float>(), v = r.get<float>();
			if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z) || !std::isfinite(u) || !std::isfinite(v)) return Ref<ArrayMesh>();
			pos.set(i, Vector3(x, y, z));
			nor.set(i, Vector3(float(n[0]), float(n[1]), float(n[2])) / NORMAL_SCALE);
			uv.set(i, Vector2(u, v));
		}
		for (uint32_t i = 0; i < indices; i++) {
			const uint32_t at = r.get<uint32_t>();
			if (at >= vertices) return Ref<ArrayMesh>();
			idx.set(i, int32_t(at));
		}
		const Dictionary lods = mesh_levels(pos, idx, DESIGN_LEVELS_M);
		Array arr;
		arr.resize(Mesh::ARRAY_MAX);
		arr[Mesh::ARRAY_VERTEX] = pos;
		arr[Mesh::ARRAY_NORMAL] = nor;
		arr[Mesh::ARRAY_TEX_UV] = uv;
		arr[Mesh::ARRAY_INDEX] = idx;
		m->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arr, Array(), lods);
		m->surface_set_material(int(s), look(material));
	}
	if (!r.ok || r.o != r.n) return Ref<ArrayMesh>();
	meshes_[size_t(kind)] = m;
	return m;
}

void SignatureBodies::set_chunk(int64_t key, const std::vector<SignaturePlace> &places) {
	drop(key);
	if (places.empty()) return;
	RenderingServer *rs = RenderingServer::get_singleton();
	std::vector<RID> &list = chunks_[key];
	for (const SignaturePlace &p : places) {
		const Ref<ArrayMesh> m = mesh(p.kind);
		if (m.is_null()) continue;
		const RID inst = rs->instance_create2(m->get_rid(), scenario_);
		rs->instance_set_layer_mask(inst, layer_);
		rs->instance_set_transform(inst, p.at);
		list.push_back(inst);
	}
}

void SignatureBodies::drop(int64_t key) {
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	for (const RID &inst : it->second) RenderingServer::get_singleton()->free_rid(inst);
	chunks_.erase(it);
}

void SignatureBodies::visible(int64_t key, bool on) {
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	for (const RID &inst : it->second) RenderingServer::get_singleton()->instance_set_visible(inst, on);
}

int SignatureBodies::drawn() const {
	int n = 0;
	for (const auto &e : chunks_) n += int(e.second.size());
	return n;
}

} // namespace city
