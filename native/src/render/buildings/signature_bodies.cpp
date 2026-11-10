// The cooked designs of signature buildings as meshes and instances (signature_bodies.h). Layout of a design: tools/assets/cook_signatures.py;
// of the looks: tools/assets/pack_signature_looks.py.
#include "render/buildings/signature_bodies.h"

#include "render/mesh_levels.h"
#include "render/surface_data.h"

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
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

namespace city {

namespace {
constexpr const char *FOLDER = "res://assets/lots/";
constexpr const char *SOLID_SHADER = "res://render/shaders/signature.gdshader", *BLENDED_SHADER = "res://render/shaders/signature_glass.gdshader";
constexpr uint32_t MAGIC_LOOKS = 0x4C534352u, MAGIC_BODY = 0x42534352u; // "RCSL", "RCSB"
constexpr uint32_t FLAG_BLENDED = 1, FLAG_TWO_SIDED = 2, FLAG_WINDOWS = 4, FLAG_PAINTED_WINDOWS = 8, MOST_SURFACES = 64, MOST_LOOKS = 1024;
constexpr int NAME_BYTES = 24, RGBA = 4, NORMAL_BYTES = 4;
constexpr size_t VERTEX_BYTES = 5 * sizeof(float) + NORMAL_BYTES; // position, normal, uv: what each vertex takes in the file
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
	scenario_ = scenario;
	layer_ = layer;
	meshes_.clear();
	meshes_.resize(size_t(b40::SIGNATURE_COUNT) + 1);
	state_.assign(size_t(b40::SIGNATURE_COUNT) + 1, State::NONE);
}

void SignatureBodies::reset() {
	for (auto &e : chunks_)
		for (const RID &inst : e.second) RenderingServer::get_singleton()->free_rid(inst);
	chunks_.clear();
	waiting_.clear();
	shown_.clear();
}

// The looks' records read; the pictures stay packed in the file's bytes, each in its own stream (0.6 MiB for all of them): unpacked
// whole and kept, as the cook's one stream was, they held 33 MiB for the game's life.
bool SignatureBodies::read_table() {
	std::lock_guard<std::mutex> lk(table_mutex_);
	if (table_read_) return table_ok_;
	table_read_ = true;
	looks_file_ = FileAccess::get_file_as_bytes(String(FOLDER) + "materials.sig");
	ByteReader r{ looks_file_.ptr(), size_t(looks_file_.size()) };
	const uint32_t magic = r.get<uint32_t>(), count = r.get<uint32_t>();
	if (!r.ok || magic != MAGIC_LOOKS || count == 0 || count > MOST_LOOKS) {
		UtilityFunctions::push_error("signature designs: materials.sig missing or bad");
		return table_ok_ = false;
	}
	looks_.resize(count);
	for (Look &l : looks_) {
		r.o += NAME_BYTES;
		for (float &f : l.v) f = r.get<float>();
		l.flags = r.get<uint32_t>();
		l.width = r.get<uint32_t>();
		l.height = r.get<uint32_t>();
		l.packed = r.get<uint32_t>();
	}
	for (Look &l : looks_) { // the streams follow the records, in the looks' order
		l.at = r.o;
		if (!r.claim(l.packed, 1)) break;
		r.o += l.packed;
	}
	table_ok_ = r.ok && r.o == r.n;
	if (!table_ok_) UtilityFunctions::push_error("signature designs: materials.sig truncated");
	return table_ok_;
}

// A look's picture with its mipmaps (none when the look has no picture), any thread: unpacked from its own stream.
Ref<Image> SignatureBodies::picture(uint32_t index) const {
	const Look &l = looks_[index];
	if (l.width == 0) return Ref<Image>();
	const int64_t bytes = int64_t(l.width) * l.height * RGBA;
	const PackedByteArray pixels = looks_file_.slice(int64_t(l.at), int64_t(l.at) + int64_t(l.packed)).decompress(bytes, FileAccess::COMPRESSION_DEFLATE);
	if (pixels.size() != bytes) {
		UtilityFunctions::push_error("signature designs: a look's picture does not unpack");
		return Ref<Image>();
	}
	Ref<Image> image = Image::create_from_data(int(l.width), int(l.height), false, Image::FORMAT_RGBA8, pixels);
	image->generate_mipmaps();
	return image;
}

Ref<ShaderMaterial> SignatureBodies::look(uint32_t index, const Ref<Image> &image) {
	Look &l = looks_[index];
	if (l.made.is_valid()) return l.made;
	const float *v = l.v;
	const uint32_t flags = l.flags;
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
	if (image.is_valid()) {
		l.made->set_shader_parameter("picture", ImageTexture::create_from_image(image));
		l.made->set_shader_parameter("pictured", true);
	}
	return l.made;
}

int64_t SignatureBodies::Prepared::bytes() const {
	int64_t n = 0;
	for (const Surface &s : surfaces) n += s.bytes;
	for (const auto &kv : pictures)
		if (kv.second.is_valid()) n += kv.second->get_data_size();
	return n;
}

// The file read, unpacked and made into arrays, levels and the pictures of looks not made yet: what the main thread then only uploads.
std::shared_ptr<SignatureBodies::Prepared> SignatureBodies::prepare(int kind) {
	auto out = std::make_shared<Prepared>();
	out->kind = kind;
	out->file = std::string(FOLDER) + b40::SIGNATURE_KINDS[kind - 1] + ".sig";
	if (!read_table()) return out;
	const PackedByteArray data = unpacked(String(out->file.c_str()), MAGIC_BODY);
	ByteReader r{ data.ptr(), size_t(data.size()) };
	const uint32_t surfaces = r.get<uint32_t>();
	if (!r.ok || surfaces > MOST_SURFACES) return out;
	for (uint32_t s = 0; s < surfaces && r.ok; s++) {
		const uint32_t material = r.get<uint32_t>(), vertices = r.get<uint32_t>(), indices = r.get<uint32_t>();
		if (material >= looks_.size()) return out;
		PackedVector3Array pos, nor;
		PackedVector2Array uv;
		PackedInt32Array idx;
		// the counts are claimed against what is left of the file before anything is sized by them (core/bytes.h)
		if (!r.claim(vertices, VERTEX_BYTES) || !r.claim(indices, sizeof(uint32_t))) return out;
		pos.resize(vertices);
		nor.resize(vertices);
		uv.resize(vertices);
		idx.resize(indices);
		for (uint32_t i = 0; i < vertices; i++) {
			const float x = r.get<float>(), y = r.get<float>(), z = r.get<float>();
			int8_t n[NORMAL_BYTES];
			r.get_bytes(reinterpret_cast<uint8_t *>(n), NORMAL_BYTES);
			const float u = r.get<float>(), v = r.get<float>();
			pos.set(i, Vector3(x, y, z));
			nor.set(i, Vector3(float(n[0]), float(n[1]), float(n[2])) / NORMAL_SCALE);
			uv.set(i, Vector2(u, v));
		}
		for (uint32_t i = 0; i < indices; i++) idx.set(i, int32_t(r.get<uint32_t>()));
		Surface sf;
		sf.lods = mesh_levels(pos, idx, DESIGN_LEVELS_M);
		sf.arrays.resize(Mesh::ARRAY_MAX);
		sf.arrays[Mesh::ARRAY_VERTEX] = pos;
		sf.arrays[Mesh::ARRAY_NORMAL] = nor;
		sf.arrays[Mesh::ARRAY_TEX_UV] = uv;
		sf.arrays[Mesh::ARRAY_INDEX] = idx;
		sf.look = material;
		sf.bytes = int64_t(vertices) * int64_t(2 * sizeof(Vector3) + sizeof(Vector2)) + int64_t(indices) * int64_t(sizeof(int32_t));
		out->surfaces.push_back(std::move(sf));
		if (!out->pictures.count(material)) out->pictures[material] = picture(material);
	}
	out->ok = r.ok;
	return out;
}

// The prepared kind uploaded: its surfaces into one mesh made whole, each with its look (surface_data.h: the engine makes its pipelines
// then, not on the frame the landmark is first drawn). False (said once) for a file that could not be read.
bool SignatureBodies::finish(Prepared &p) {
	if (solid_.is_null()) solid_ = ResourceLoader::get_singleton()->load(SOLID_SHADER);
	if (blended_.is_null()) blended_ = ResourceLoader::get_singleton()->load(BLENDED_SHADER);
	if (!p.ok || solid_.is_null() || blended_.is_null()) {
		state_[size_t(p.kind)] = State::FAILED;
		UtilityFunctions::push_error(String("signature design unreadable: ") + String(p.file.c_str()));
		return false;
	}
	TypedArray<Dictionary> surfaces;
	for (const Surface &sf : p.surfaces) {
		const Dictionary made = surface_data(sf.arrays, sf.lods, 0, look(sf.look, p.pictures[sf.look])->get_rid());
		if (!made.is_empty()) surfaces.push_back(made); // a surface with no vertex is none
	}
	if (surfaces.is_empty()) {
		state_[size_t(p.kind)] = State::FAILED;
		UtilityFunctions::push_error(String("signature design with no surface: ") + String(p.file.c_str()));
		return false;
	}
	meshes_[size_t(p.kind)] = OwnedMesh::of(create_mesh(surfaces)).shared(); // kept while its chunks' instances come and go
	state_[size_t(p.kind)] = State::READY;
	return true;
}

RID SignatureBodies::mesh(int kind) {
	if (kind <= 0 || kind > b40::SIGNATURE_COUNT) return RID();
	if (state_[size_t(kind)] == State::FAILED) return RID();
	if (meshes_[size_t(kind)].valid()) return meshes_[size_t(kind)].rid;
	std::shared_ptr<Prepared> p = prepare(kind); // asked now (a job's may land later: it is then let go)
	return finish(*p) ? meshes_[size_t(kind)].rid : RID();
}

void SignatureBodies::place(int64_t key, const SignaturePlace &p, bool on) {
	RenderingServer *rs = RenderingServer::get_singleton();
	const RID inst = rs->instance_create2(meshes_[size_t(p.kind)].rid, scenario_);
	rs->instance_set_layer_mask(inst, layer_);
	rs->instance_set_transform(inst, p.at);
	if (!on) rs->instance_set_visible(inst, false);
	chunks_[key].push_back(inst);
}

void SignatureBodies::set_chunk(int64_t key, const std::vector<SignaturePlace> &places) {
	drop(key);
	if (places.empty()) return;
	for (const SignaturePlace &p : places) {
		if (p.kind <= 0 || p.kind > b40::SIGNATURE_COUNT) continue;
		State &st = state_[size_t(p.kind)];
		if (st == State::FAILED) continue; // said when it failed
		if (st == State::READY) {
			place(key, p, true);
			continue;
		}
		waiting_[key].push_back(p);
		if (st == State::NONE && pool_) {
			st = State::ASKED;
			const int kind = p.kind;
			pool_->submit(JobKey{ JobLane::SIGNATURES, kind }, 0.f, [this, kind]() { inbox_.push(prepare(kind)); });
		}
	}
	if (!pool_) // no pool (a test without one): made here
		for (const SignaturePlace &p : waiting_[key])
			if (mesh(p.kind).is_valid()) place(key, p, true);
	if (!pool_) waiting_.erase(key);
}

// The places that waited for `kind` placed (or let go, when it failed).
void SignatureBodies::release(int kind) {
	const bool ok = state_[size_t(kind)] == State::READY;
	for (auto w = waiting_.begin(); w != waiting_.end();) {
		std::vector<SignaturePlace> &list = w->second;
		for (auto it = list.begin(); it != list.end();) {
			if (it->kind != kind) {
				++it;
				continue;
			}
			if (ok) {
				auto shown = shown_.find(w->first);
				place(w->first, *it, shown == shown_.end() || shown->second);
			}
			it = list.erase(it);
		}
		w = list.empty() ? waiting_.erase(w) : std::next(w);
	}
}

void SignatureBodies::sync(FrameAdmission &f) {
	std::vector<std::shared_ptr<Prepared>> in;
	inbox_.take(in);
	for (std::shared_ptr<Prepared> &p : in) {
		if (state_[size_t(p->kind)] == State::ASKED) ready_.push_back(std::move(p));
		else release(p->kind); // made meanwhile on the main thread (mesh): this one goes
	}
	size_t kept = 0;
	for (size_t i = 0; i < ready_.size(); i++) {
		Prepared &p = *ready_[i];
		if (!f.due() || !f.upload.take(p.bytes())) {
			ready_[kept++] = std::move(ready_[i]);
			continue;
		}
		finish(p);
		release(p.kind);
	}
	ready_.resize(kept);
}

void SignatureBodies::drop(int64_t key) {
	waiting_.erase(key);
	shown_.erase(key);
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	for (const RID &inst : it->second) RenderingServer::get_singleton()->free_rid(inst);
	chunks_.erase(it);
}

void SignatureBodies::visible(int64_t key, bool on) {
	shown_[key] = on;
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
