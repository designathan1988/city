// Street furniture meshes (Roadcraft render/propMeshes.ts) built once from data/render/streets.json "props": each prop
// is a list of parts in local space (+z faces the road or the arriving traffic, y up, base at y = 0) and is appended
// into chunk meshes with a transform. UV2.x tags the parts for render/shaders/props.gdshader (PropTag). The signal's
// lenses are not in its mesh: they are a mesh of their own (lenses()), every lens drawn once, lit or not, by
// render/props/signal_lamps.h.
#pragma once

#include "render/mesh_builder.h"

#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city {

enum class PropTag : int { FIXED = 0, TINT = 1, NIGHT_GLOW = 2, LENS }; // props.gdshader reads them as 0..3
enum class PropKind : int { SIGNAL = 0, LAMP, HYDRANT, BIN, BENCH, COUNT };
// The signal's lenses (streets.json props.signal "lens"): the cars' red, amber and green, the walkers' stop and walk.
enum class Lens : int { RED = 0, AMBER, GREEN, STOP, WALK, COUNT };

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
	// Every lens of the signal, in its space: COLOR the lens unlit (its colour times lens_dim), UV2 (PropTag::LENS, which
	// Lens) for render/shaders/props.gdshader.
	const MeshBuilder &lenses() const { return lenses_; }
	// Copies `src` into `dst`, every vertex moved by basis then origin.
	static void append(MeshBuilder &dst, const MeshBuilder &src, const godot::Basis &b, const godot::Vector3 &o);
	// Basis whose +z is `face` (horizontal) and +y up.
	static godot::Basis facing(const godot::Vector3 &face);

private:
	MeshBuilder meshes_[int(PropKind::COUNT)];
	MeshBuilder lenses_;
};

} // namespace city
