// Loads the models of the element brush (see element_models.h).
#include "render/cover/element_models.h"

#include <godot_cpp/classes/base_material3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/plane_mesh.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <algorithm>

using namespace godot;

namespace city {

namespace {
// The first mesh under `n`.
Ref<Mesh> first_mesh(Node *n) {
	if (MeshInstance3D *m = Object::cast_to<MeshInstance3D>(n))
		if (m->get_mesh().is_valid()) return m->get_mesh();
	for (int i = 0; i < n->get_child_count(); i++) {
		Ref<Mesh> found = first_mesh(n->get_child(i));
		if (found.is_valid()) return found;
	}
	return Ref<Mesh>();
}

ElementModel fitted(const Ref<Mesh> &mesh) {
	ElementModel m;
	m.mesh = mesh;
	AABB box = mesh->get_aabb();
	float across = std::max(box.size.x, box.size.z);
	m.fit = across > 0 ? 1.f / across : 1.f;
	m.foot = box.position.y;
	return m;
}

Ref<Mesh> scene_mesh(const String &path, PackedStringArray &errors) {
	Ref<PackedScene> scene = ResourceLoader::get_singleton()->load(path);
	if (scene.is_null()) {
		errors.push_back("cover.json elements: não abriu " + path);
		return Ref<Mesh>();
	}
	Node *root = scene->instantiate();
	Ref<Mesh> mesh = first_mesh(root);
	memdelete(root);
	if (mesh.is_null()) errors.push_back("cover.json elements: sem malha em " + path);
	return mesh;
}

// A quad lying on the ground, wearing the texture with its cover kept at any distance.
Ref<Mesh> ground_quad(const String &path, PackedStringArray &errors) {
	Ref<Texture2D> tex = ResourceLoader::get_singleton()->load(path);
	if (tex.is_null()) {
		errors.push_back("cover.json elements: não abriu " + path);
		return Ref<Mesh>();
	}
	Ref<StandardMaterial3D> mat;
	mat.instantiate();
	mat->set_texture(BaseMaterial3D::TEXTURE_ALBEDO, tex);
	mat->set_transparency(BaseMaterial3D::TRANSPARENCY_ALPHA_HASH); // a cut at half cover loses the whole quad in the mips, where the mean alpha is under it
	mat->set_cull_mode(BaseMaterial3D::CULL_DISABLED);
	Ref<PlaneMesh> plane;
	plane.instantiate();
	plane->set_material(mat);
	return plane;
}
} // namespace

bool ElementModels::load(const DataValue &elements, PackedStringArray &errors) {
	const int64_t before = errors.size();
	const DataValue kinds = elements["props"];
	models_.clear();
	first_.clear();
	count_.clear();
	for (int k = 0; k < kinds.size(); k++) {
		const DataValue files = kinds[k];
		first_.push_back(int(models_.size()));
		count_.push_back(files.size());
		for (int i = 0; i < files.size(); i++) {
			const String path = files[i].text();
			Ref<Mesh> mesh = path.ends_with(".png") ? ground_quad(path, errors) : scene_mesh(path, errors);
			if (mesh.is_valid()) models_.push_back(fitted(mesh));
		}
	}
	return errors.size() == before;
}

} // namespace city
