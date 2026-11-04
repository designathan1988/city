// Street furniture meshes built once from data/render/streets.json "props": each prop
// is a list of parts in local space (+z faces the road or the arriving traffic, y up, base at y = 0) and is appended
// into chunk meshes with a transform, or drawn as instances (the poles, wires, barrier boxes and lamp light). UV2.x tags the
// parts for render/shaders/props.gdshader (PropTag). The signal's lenses are not in its mesh: they are a mesh of their own
// (lenses()), every lens drawn once, lit or not, by render/props/signal_lamps.h.
#pragma once

#include "render/mesh_builder.h"

#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city {

enum class PropTag : int { FIXED = 0, TINT = 1, NIGHT_GLOW = 2, LENS, WIRE, WOOD, PLATE }; // props.gdshader reads them
enum class PropKind : int { SIGNAL = 0, HYDRANT, BIN, BENCH, POSTBOX, PHONE, DRAIN, TREE_PIT, MEADOW, COUNT };
// The signal's lenses (streets.json props.signal "lens"): the cars' red, amber and green, the walkers' stop and walk.
enum class Lens : int { RED = 0, AMBER, GREEN, STOP, WALK, COUNT };
// The parts of the pole mesh (streets.json props.pole "part"): an instance shows those of INSTANCE_CUSTOM.a, its UV2.y bits.
enum class PolePart : int { MAST = 1, ARM = MAST << 1, CAN = ARM << 1, LAMP = CAN << 1, COLUMN = LAMP << 1 };

// A placed prop: which mesh, where, facing.
struct PropPlace {
	PropKind kind = PropKind::SIGNAL;
	godot::Vector3 origin;
	godot::Basis basis;
};

class PropKit {
public:
	bool build(const godot::Dictionary &props, const godot::Dictionary &wires, godot::PackedStringArray &errors);
	const MeshBuilder &mesh(PropKind k) const { return meshes_[int(k)]; }
	// Every lens of the signal, in its space: COLOR the lens unlit (its colour times lens_dim), UV2 (PropTag::LENS, which
	// Lens) for render/shaders/props.gdshader.
	const MeshBuilder &lenses() const { return lenses_; }
	const MeshBuilder &pole() const { return pole_; }
	// A wire from t = 0 to 1 along x, as a tube round it (near) or a coarser one (far): props.gdshader bends it.
	const MeshBuilder &wire(bool near) const { return near ? wire_near_ : wire_far_; }
	const MeshBuilder &unit_box() const { return box_; }
	// The pool of light (a disc of radius 1 at y = 0) and the beam (a cone from there to its apex at y = 1): lamp_glow.gdshader.
	const MeshBuilder &light() const { return light_; }
	// Copies `src` into `dst`, every vertex moved by basis then origin.
	static void append(MeshBuilder &dst, const MeshBuilder &src, const godot::Basis &b, const godot::Vector3 &o);
	// Basis whose +z is `face` (horizontal) and +y up.
	static godot::Basis facing(const godot::Vector3 &face);

private:
	MeshBuilder meshes_[int(PropKind::COUNT)];
	MeshBuilder lenses_, pole_, wire_near_, wire_far_, box_, light_;
};

} // namespace city
