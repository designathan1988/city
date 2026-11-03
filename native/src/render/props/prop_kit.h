// Street furniture meshes (Roadcraft render/propMeshes.ts) built once from data/render/streets.json "props": each prop
// is a list of parts in local space (+z faces the road or the arriving traffic, y up, base at y = 0) and is appended
// into chunk meshes with a transform. UV2.x tags the parts for render/shaders/props.gdshader (PropTag).
#pragma once

#include "render/mesh_builder.h"

#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city {

enum class PropTag : int { FIXED = 0, TINT = 1, NIGHT_GLOW = 2, LIT = 3 };
enum class PropKind : int { SIGNAL_RED = 0, SIGNAL_GREEN, LAMP, HYDRANT, BIN, BENCH, COUNT };

// A placed prop: which mesh, where, facing.
struct PropPlace {
	PropKind kind = PropKind::LAMP;
	godot::Vector3 origin;
	godot::Basis basis;
};

class PropKit {
public:
	bool build(const godot::Dictionary &props, godot::PackedStringArray &errors);
	const MeshBuilder &mesh(PropKind k) const { return meshes_[int(k)]; }
	// Copies `src` into `dst`, every vertex moved by basis then origin.
	static void append(MeshBuilder &dst, const MeshBuilder &src, const godot::Basis &b, const godot::Vector3 &o);
	// Basis whose +z is `face` (horizontal) and +y up.
	static godot::Basis facing(const godot::Vector3 &face);

private:
	MeshBuilder meshes_[int(PropKind::COUNT)];
};

} // namespace city
