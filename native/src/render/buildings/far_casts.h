// What the buildings' far level throws in the sun: a caster a chunk of the superchunk, each block's sides and roof alone (far_mass.cpp
// emit_cast), positions welded, in the shadow material (the engine's own depth shader). The far mesh itself casts nothing: thrown whole,
// every cascade that touched a superchunk drew its square kilometre through the far shader's vertex(), the buildings of the chunks drawn
// in their own detail included. A caster has its chunk's box, so a cascade draws the chunks it covers, and is drawn only while its chunk
// is not drawn in detail (a detailed chunk throws its own hulls: cast_hull.h).
#pragma once

#include "core/grid.h"
#include "render/buildings/facade.h"
#include "render/mesh_draw.h"

#include <array>
#include <cstdint>

namespace city {

constexpr size_t SUPER_CHUNKS = size_t(SUPERCHUNK / CHUNK) * size_t(SUPERCHUNK / CHUNK); // of a superchunk, row by row

// A superchunk job's casters: a shell a chunk while the job builds, then their meshes (bake, on the bake job's thread).
struct FarCastsBuilt {
	std::array<b40::Shell, SUPER_CHUNKS> shells;
	std::array<OwnedMesh, SUPER_CHUNKS> meshes;
	std::array<godot::AABB, SUPER_CHUNKS> boxes;
	int64_t triangles = 0; // held by the meshes
	float error_m = 0; // how far a caster may stand off its walls
	godot::RID material; // the shadow caster's
	void made_with(float error, const godot::RID &caster) {
		error_m = error;
		material = caster;
	}
	void add(int chunk, const b40::Placed &pl) { b40::emit_cast(shells[size_t(chunk)], pl); }
	int64_t emitted() const; // the triangles laid, before they are welded and simplified
	int64_t bytes() const; // of the positions and indices as laid: what the frame's upload is charged (welded at the bake, they send no more)
	void bake();
};

// A superchunk's casters on the scene.
struct FarCasts {
	std::array<ShadowTwin, SUPER_CHUNKS> twins;
	std::array<OwnedMesh, SUPER_CHUNKS> meshes;
	int64_t triangles = 0;
	void commit(FarCastsBuilt &b, const godot::RID &scenario, RenderLayer layer);
	void show(int detailed) const; // `detailed`: a bit a chunk drawn in its own detail
	void free();
};

} // namespace city
