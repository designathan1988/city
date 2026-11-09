// The three.js runtime of the frame-replay port: each captured three.js draw rebuilt alone in a SubViewport (its translated
// shader, its mesh and instances, its textures as three's GPU held them) and read back for scripts/replay-compare.mjs.
#include "three_runtime.h"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/environment.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/multi_mesh.hpp>
#include <godot_cpp/classes/multi_mesh_instance3d.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/rd_texture_format.hpp>
#include <godot_cpp/classes/rd_texture_view.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/texture2drd.hpp>
#include <godot_cpp/classes/texture2d_array_rd.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/classes/world_environment.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <map>

using namespace godot;

extern const three::DrawFn three_gen_draws[];
extern const int three_gen_count;

namespace three {

static PackedByteArray bytes_of(const std::string &file) {
	return FileAccess::get_file_as_bytes(String("res://data/") + file.c_str());
}
static PackedFloat32Array floats_of(const std::string &file) { return bytes_of(file).to_float32_array(); }
static PackedInt32Array ints_of(const std::string &file) { return bytes_of(file).to_int32_array(); }

// a texture as three's GPU held it, through the RenderingDevice (any level, layer and format); cached by its files
static std::map<std::string, Ref<godot::Texture>> texture_cache;
// the RenderingDevice textures made here: freed by release(), with the cache, before the engine stops (a static
// destroyed at the library's unload would outlive the RenderingServer)
static std::vector<RID> texture_rids;

static Ref<godot::Texture> texture_of(const Texture &t) {
	std::string key;
	for (const auto &l : t.levels) for (const auto &f : l.files) key += f + "|";
	auto hit = texture_cache.find(key);
	if (hit != texture_cache.end()) return hit->second;
	RenderingDevice *rd = RenderingServer::get_singleton()->get_rendering_device();
	Ref<RDTextureFormat> fmt;
	fmt.instantiate();
	int bpp = 16;
	switch (t.kind) {
		case Texture::UNORM:
			// UNORM storage shareable as its sRGB view: Godot views a Texture2DRD through its Image format (UNORM) and
			// samples the sRGB view for a source_color uniform (decoded by the hardware, like WebGL's SRGB8_ALPHA8)
			fmt->set_format(RenderingDevice::DATA_FORMAT_R8G8B8A8_UNORM);
			if (t.srgb) {
				fmt->add_shareable_format(RenderingDevice::DATA_FORMAT_R8G8B8A8_UNORM);
				fmt->add_shareable_format(RenderingDevice::DATA_FORMAT_R8G8B8A8_SRGB);
			}
			bpp = 4;
			break;
		case Texture::UINT16:
			fmt->set_format(RenderingDevice::DATA_FORMAT_R16G16B16A16_UINT); // the widest integer format Godot 4.7 maps
			bpp = 8;
			break;
		case Texture::FLOAT:
			fmt->set_format(RenderingDevice::DATA_FORMAT_R32G32B32A32_SFLOAT);
			break;
	}
	fmt->set_width(t.levels[0].width);
	fmt->set_height(t.levels[0].height);
	fmt->set_mipmaps(int(t.levels.size()));
	fmt->set_array_layers(t.layers);
	fmt->set_texture_type(t.array ? RenderingDevice::TEXTURE_TYPE_2D_ARRAY : RenderingDevice::TEXTURE_TYPE_2D);
	fmt->set_usage_bits(RenderingDevice::TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice::TEXTURE_USAGE_CAN_UPDATE_BIT);
	TypedArray<PackedByteArray> data;
	for (int layer = 0; layer < t.layers; layer++) {
		PackedByteArray all;
		for (const auto &l : t.levels) {
			PackedByteArray b = bytes_of(l.files[size_t(layer)]);
			b.resize(int64_t(l.width) * l.height * bpp);
			all.append_array(b);
		}
		data.push_back(all);
	}
	Ref<RDTextureView> view;
	view.instantiate();
	RID rid = rd->texture_create(fmt, view, data);
	if (rid.is_valid()) texture_rids.push_back(rid);
	Ref<godot::Texture> out;
	if (!rid.is_valid()) {
		UtilityFunctions::push_error("REPLAY texture not created");
	} else if (t.array) {
		Ref<Texture2DArrayRD> a;
		a.instantiate();
		a->set_texture_rd_rid(rid);
		out = a;
	} else {
		Ref<Texture2DRD> a;
		a.instantiate();
		a->set_texture_rd_rid(rid);
		out = a;
	}
	texture_cache[key] = out;
	return out;
}

static const AABB EVERYWHERE(Vector3(-1e6, -1e6, -1e6), Vector3(2e6, 2e6, 2e6));

void release() {
	texture_cache.clear(); // the Texture2DRD wrappers first (they free through the RenderingServer)
	RenderingDevice *rd = RenderingServer::get_singleton()->get_rendering_device();
	for (const RID &rid : texture_rids) if (rd->texture_is_valid(rid)) rd->free_rid(rid);
	texture_rids.clear();
}

// The draw's material: its translated shader, its uniforms and its textures (an element of an array uniform in its slot).
static Ref<ShaderMaterial> material_of(const Draw &d) {
	Ref<Shader> shader;
	shader.instantiate();
	shader->set_code(FileAccess::get_file_as_string(String(d.shader.c_str())));
	Ref<ShaderMaterial> mat;
	mat.instantiate();
	mat->set_shader(shader);
	for (const auto &p : d.params) mat->set_shader_parameter(p.first, p.second);
	std::map<std::string, Array> groups;
	for (const auto &b : d.textures) {
		Ref<godot::Texture> tex = texture_of(b.texture);
		if (b.element >= 0) {
			Array &g = groups[std::string(String(b.uniform).utf8().get_data())];
			while (g.size() <= b.element) g.push_back(Variant());
			g[b.element] = tex;
		} else {
			mat->set_shader_parameter(b.uniform, tex);
		}
	}
	for (auto &g : groups) mat->set_shader_parameter(StringName(g.first.c_str()), g.second);
	return mat;
}

// The draw's mesh arrays as the translation placed each attribute, and the format flags of its custom slots.
static Array arrays_of(const Draw &d, int64_t &flags) {
	Array arrays;
	arrays.resize(Mesh::ARRAY_MAX);
	const int n = d.vertex_count;
	PackedVector3Array pos;
	pos.resize(n);
	if (!d.vertex.empty()) {
		PackedFloat32Array p = floats_of(d.vertex);
		for (int i = 0; i < n; i++) pos.set(i, Vector3(p[i * 3], p[i * 3 + 1], p[i * 3 + 2]));
	}
	arrays[Mesh::ARRAY_VERTEX] = pos;
	if (!d.normal.empty()) {
		PackedFloat32Array q = floats_of(d.normal);
		PackedVector3Array nor;
		nor.resize(n);
		for (int i = 0; i < n; i++) nor.set(i, Vector3(q[i * 3], q[i * 3 + 1], q[i * 3 + 2]));
		arrays[Mesh::ARRAY_NORMAL] = nor;
	}
	if (!d.tangent.empty()) arrays[Mesh::ARRAY_TANGENT] = floats_of(d.tangent);
	if (!d.uv.empty()) {
		PackedFloat32Array u = floats_of(d.uv);
		PackedVector2Array uv;
		uv.resize(n);
		for (int i = 0; i < n; i++) uv.set(i, Vector2(u[i * 2], u[i * 2 + 1]));
		arrays[Mesh::ARRAY_TEX_UV] = uv;
	}
	if (!d.uv2.empty()) {
		PackedFloat32Array u = floats_of(d.uv2);
		PackedVector2Array uv;
		uv.resize(n);
		for (int i = 0; i < n; i++) uv.set(i, Vector2(u[i * 2], u[i * 2 + 1]));
		arrays[Mesh::ARRAY_TEX_UV2] = uv;
	}
	if (!d.color.empty()) {
		PackedFloat32Array c = floats_of(d.color);
		PackedColorArray col;
		col.resize(n);
		for (int i = 0; i < n; i++) col.set(i, Color(c[i * 4], c[i * 4 + 1], c[i * 4 + 2], c[i * 4 + 3]));
		arrays[Mesh::ARRAY_COLOR] = col;
	}
	flags = 0;
	const int64_t shifts[4] = { Mesh::ARRAY_FORMAT_CUSTOM0_SHIFT, Mesh::ARRAY_FORMAT_CUSTOM1_SHIFT, Mesh::ARRAY_FORMAT_CUSTOM2_SHIFT, Mesh::ARRAY_FORMAT_CUSTOM3_SHIFT };
	for (int k = 0; k < 4; k++) {
		if (d.customs[k].empty()) continue;
		arrays[Mesh::ARRAY_CUSTOM0 + k] = floats_of(d.customs[k]);
		flags |= int64_t(Mesh::ARRAY_CUSTOM_RGBA_FLOAT) << shifts[k];
	}
	if (!d.bones.empty()) {
		arrays[Mesh::ARRAY_BONES] = ints_of(d.bones);
		// the translation reads three's skinWeight from a CUSTOM slot (exact); Godot still requires weights with bones
		if (!d.weights.empty()) arrays[Mesh::ARRAY_WEIGHTS] = floats_of(d.weights);
		else { PackedFloat32Array z; z.resize(ints_of(d.bones).size()); arrays[Mesh::ARRAY_WEIGHTS] = z; }
	}
	// the indices as the generator wrote them (triangles reversed already: Godot's front faces are clockwise)
	arrays[Mesh::ARRAY_INDEX] = ints_of(d.indices);
	return arrays;
}

// The node that draws the mesh: one MultiMesh instance per three.js instance (from the draw or its file), else the mesh.
static Node *node_of(const Draw &d, const Ref<ArrayMesh> &mesh) {
	std::vector<Draw::Instance> from_file;
	if (!d.instances_file.empty()) {
		PackedFloat32Array v = floats_of(d.instances_file);
		for (int64_t o = 0; o + 24 <= v.size(); o += 24) {
			const float *m = v.ptr() + o;
			from_file.push_back({ Transform3D(Basis(Vector3(m[0], m[1], m[2]), Vector3(m[4], m[5], m[6]), Vector3(m[8], m[9], m[10])), Vector3(m[12], m[13], m[14])), Color(m[16], m[17], m[18], m[19]), Color(m[20], m[21], m[22], m[23]) });
		}
	}
	const std::vector<Draw::Instance> &instances = d.instances_file.empty() ? d.instances : from_file;
	if (d.instanced || !d.instances_file.empty()) {
		Ref<MultiMesh> mm;
		mm.instantiate();
		mm->set_transform_format(MultiMesh::TRANSFORM_3D);
		mm->set_use_colors(true);
		mm->set_use_custom_data(true);
		mm->set_mesh(mesh);
		mm->set_instance_count(int(instances.size()));
		for (size_t i = 0; i < instances.size(); i++) {
			mm->set_instance_transform(int(i), instances[i].transform);
			mm->set_instance_color(int(i), instances[i].color);
			mm->set_instance_custom_data(int(i), instances[i].custom);
		}
		mm->set_custom_aabb(EVERYWHERE);
		MultiMeshInstance3D *mmi = memnew(MultiMeshInstance3D);
		mmi->set_multimesh(mm);
		return mmi;
	}
	MeshInstance3D *mi = memnew(MeshInstance3D);
	mi->set_mesh(mesh);
	return mi;
}

Node *build(const Draw &d) {
	int64_t flags = 0;
	const Array arrays = arrays_of(d, flags);
	Ref<ArrayMesh> mesh;
	mesh.instantiate();
	mesh->add_surface_from_arrays(d.primitive, arrays, TypedArray<Array>(), Dictionary(), BitField<Mesh::ArrayFormat>(flags));
	mesh->surface_set_material(0, material_of(d));
	// the positions are three's (uniforms), not Godot's camera: never culled
	mesh->set_custom_aabb(EVERYWHERE);
	return node_of(d, mesh);
}

} // namespace three

void ThreeReplay::_bind_methods() {
	ClassDB::bind_method(D_METHOD("on_post_draw"), &ThreeReplay::on_post_draw);
	ClassDB::bind_method(D_METHOD("start_next"), &ThreeReplay::start_next);
}

void ThreeReplay::_ready() {
	UtilityFunctions::print("LIVE PID ", OS::get_singleton()->get_process_id());
	RenderingServer::get_singleton()->set_default_clear_color(Color(0, 0, 0, 0));
	DirAccess::make_dir_recursive_absolute("res://out_cpp");
	RenderingServer::get_singleton()->connect("frame_post_draw", Callable(this, "on_post_draw"));
	start_next();
}

void ThreeReplay::start_next() {
	if (next >= three_gen_count) {
		// the last viewport is freed at the end of this frame: the textures are released two drawn frames later
		releasing = true;
		frames = 0;
		return;
	}
	three::Draw d;
	three_gen_draws[next++](d);
	current = d.id.empty() ? std::to_string(d.index) : d.id;
	current_bits = d.bits;
	UtilityFunctions::print("REPLAY BEGIN ", String(current.c_str()));
	viewport = memnew(SubViewport);
	viewport->set_size(Vector2i(d.width, d.height));
	viewport->set_use_own_world_3d(true);
	viewport->set_transparent_background(true);
	viewport->set_use_hdr_2d(true);
	viewport->set_msaa_3d(Viewport::MSAA_DISABLED);
	viewport->set_screen_space_aa(Viewport::SCREEN_SPACE_AA_DISABLED);
	viewport->set_use_taa(false);
	viewport->set_anisotropic_filtering_level(Viewport::AnisotropicFiltering(d.anisotropy));
	viewport->set_update_mode(SubViewport::UPDATE_ALWAYS);
	add_child(viewport);
	Ref<Environment> env;
	env.instantiate();
	env->set_background(Environment::BG_CLEAR_COLOR);
	env->set_tonemapper(Environment::TONE_MAPPER_LINEAR);
	env->set_ambient_source(Environment::AMBIENT_SOURCE_DISABLED);
	WorldEnvironment *we = memnew(WorldEnvironment);
	we->set_environment(env);
	viewport->add_child(we);
	Camera3D *cam = memnew(Camera3D);
	viewport->add_child(cam);
	cam->make_current();
	viewport->add_child(three::build(d));
	frames = 0;
}

void ThreeReplay::on_post_draw() {
	if (frames < 0) return;
	if (releasing) {
		if (++frames < 2) return;
		frames = -1;
		three::release();
		UtilityFunctions::print("REPLAY DONE");
		get_tree()->quit();
		return;
	}
	if (++frames < 3) return; // as scripts/replay.gd: three frames drawn
	frames = -1;
	finish_current();
	call_deferred("start_next");
}

void ThreeReplay::finish_current() {
	Ref<Image> out = viewport->get_texture()->get_image();
	// a component pass of a float target wrote bytes k/255: kept as floats; any other draw as RGBA8
	out->convert(current_bits ? Image::FORMAT_RGBAF : Image::FORMAT_RGBA8);
	Ref<FileAccess> f = FileAccess::open(String("res://out_cpp/") + String(current.c_str()) + (current_bits ? ".f32" : ".rgba"), FileAccess::WRITE);
	f->store_buffer(out->get_data());
	f->close();
	viewport->queue_free();
	viewport = nullptr;
	UtilityFunctions::print("REPLAY END ", String(current.c_str()));
}
