// Low-level triangle builder for procedural meshes (roads, building masses, data-defined models).
// Godot uses clockwise winding for front faces; tri() orients each triangle toward `out`.
#pragma once

#include "core/math.h"
#include "render/rs_util.h"

#include <vector>

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/packed_color_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace city {

// Box topology shared by every box emitter: corners 0-3 bottom ring, 4-7 top ring; faces with their
// outward normals (the last face, the bottom, is optional).
constexpr int BOX_CORNERS = 8, BOX_FACES = 6;
inline constexpr int BOX_FACE_CORNERS[BOX_FACES][4] = { { 4, 5, 6, 7 }, { 0, 1, 5, 4 }, { 1, 2, 6, 5 }, { 2, 3, 7, 6 }, { 3, 0, 4, 7 }, { 0, 3, 2, 1 } };
inline constexpr int BOX_FACE_NORMALS[BOX_FACES][3] = { { 0, 1, 0 }, { 0, 0, -1 }, { 1, 0, 0 }, { 0, 0, 1 }, { -1, 0, 0 }, { 0, -1, 0 } };

struct MeshBuilder {
	godot::PackedVector3Array v, n;
	godot::PackedColorArray c;
	godot::PackedVector2Array uv, uv2;

	godot::PackedInt32Array idx;
	godot::PackedByteArray tags; // what a shader tells the vertices apart by, CUSTOM0.r: none unless `tag` was asked

	int add(const godot::Vector3 &p, const godot::Vector3 &nn, const godot::Color &col, const godot::Vector2 &t = godot::Vector2(),
			const godot::Vector2 &t2 = godot::Vector2());
	void tri(int a, int b, int c, const godot::Vector3 &out);
	// Quad a-b-c-d (any winding) facing `out`; uv per corner, uv2 shared.
	void quad(const godot::Vector3 &a, const godot::Vector3 &b, const godot::Vector3 &cc, const godot::Vector3 &d, const godot::Color &col,
			const godot::Vector3 &out, const godot::Vector2 uvs[4] = nullptr, const godot::Vector2 &t2 = godot::Vector2());
	void box(const godot::Vector3 &mn, const godot::Vector3 &mx, const godot::Color &col, const godot::Vector2 &t2 = godot::Vector2(), bool bottom = false);
	void cylinder(const godot::Vector3 &base, float r, float h, int seg, const godot::Color &col, bool cap, const godot::Vector2 &t2 = godot::Vector2());
	void cone(const godot::Vector3 &base, float r, float h, int seg, const godot::Color &col, const godot::Vector2 &t2 = godot::Vector2());
	void blob(const godot::Vector3 &center, const godot::Vector3 &radius, const godot::Color &col, const godot::Vector2 &t2 = godot::Vector2());
	void tag(uint8_t value); // the vertices added since the last tag carry `value`
	bool empty() const { return idx.is_empty(); }
	int64_t bytes() const; // of the arrays handed to the RenderingServer (chunk_jobs.h UploadBudget)
	godot::Ref<godot::ArrayMesh> commit() const;
	// Adds the surface to an existing mesh RID (RenderingServer) instead of creating a resource.
	void commit_to(const godot::RID &mesh) const;
	bool levels = false; // the surface goes with fewer triangles for the distance (mesh_levels.h)
	// The surface converted on the calling thread into a mesh of its own (rs_util.h OwnedMesh: a job bakes, the main
	// thread only hands it to an instance); no mesh when the builder is empty.
	OwnedMesh bake() const;
};

godot::Color rgba8_to_color(uint32_t rgba);
godot::Color rgb8_to_color(uint32_t rgb); // a line's colour (no alpha)

godot::AABB mesh_outline(const MeshBuilder &b);

} // namespace city
