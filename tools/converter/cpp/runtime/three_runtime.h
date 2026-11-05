// The C++ side of the three.js → Godot port: what a generated scene (scripts/cpp-scene.mjs) is made of. A draw is
// the data three gave the GPU (its mesh arrays as the translation placed them, its instances, its textures in the
// formats three's GPU held, its uniforms as typed values) and the translated shader; the runtime builds the Godot
// objects for it (ArrayMesh, MultiMesh, RenderingDevice textures, ShaderMaterial) and, for the replay, draws each
// one alone in a SubViewport and writes what Godot drew.
#pragma once

#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/texture.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace three {

/** A texture as three's GPU held it: every level, every layer, in the kind the capture read. */
struct Texture {
	enum Kind { UNORM, FLOAT, UINT16 };
	Kind kind = UNORM;
	bool srgb = false;
	bool array = false;
	int layers = 1;
	struct Level {
		int width = 0, height = 0;
		std::vector<std::string> files; // one per layer, under res://data/
	};
	std::vector<Level> levels;
};

/** One draw: what three gave the GPU for it. */
struct Draw {
	int index = -1;
	std::string id; // the draw, or <draw>c<k>: one component pass of a float target
	bool bits = false; // a component pass: its image kept as floats (the bytes it wrote)
	std::string shader;
	int width = 0, height = 0;
	int anisotropy = 0; // Viewport::AnisotropicFiltering: three's texture.anisotropy, per viewport in Godot 4.7
	godot::Mesh::PrimitiveType primitive = godot::Mesh::PRIMITIVE_TRIANGLES;
	int vertex_count = 0;
	std::string vertex, normal, tangent, uv, uv2, color, bones, weights, indices; // files ("" when absent)
	std::string customs[4];
	struct Instance {
		godot::Transform3D transform;
		godot::Color color, custom;
	};
	bool instanced = false;
	std::vector<Instance> instances;
	// or, for many, a file of them: per instance the 16 floats of the matrix (columns), the colour, the custom data
	std::string instances_file;
	std::vector<std::pair<godot::StringName, godot::Variant>> params;
	struct Bound {
		godot::StringName uniform;
		int element = -1; // an element of a sampler array, or -1
		Texture texture;
	};
	std::vector<Bound> textures;

	void param(const char *name, const godot::Variant &value) { params.emplace_back(godot::StringName(name), value); }
	void instance(const godot::Transform3D &t, const godot::Color &c, const godot::Color &custom) {
		instanced = true;
		instances.push_back({ t, c, custom });
	}
};

using DrawFn = void (*)(Draw &);

/** Builds the Godot objects of a draw (the mesh or multimesh instance carrying its material). */
godot::Node *build(const Draw &d);

/** Frees the textures the runtime made (wrappers, then RenderingDevice RIDs): before the engine stops. */
void release();

} // namespace three

/** The replay: each generated draw alone in a SubViewport (HDR 2D: the values the shader wrote, unencoded), read
 *  back as RGBA8 into res://out_cpp/<draw>.rgba. Prints LIVE PID and REPLAY BEGIN/END <draw> for scripts/live-run.mjs. */
class ThreeReplay : public godot::Node {
	GDCLASS(ThreeReplay, godot::Node)

	int next = 0;
	int frames = -1;
	godot::SubViewport *viewport = nullptr;
	std::string current;
	bool current_bits = false;
	bool releasing = false;

	void start_next();
	void finish_current();
	void on_post_draw();

protected:
	static void _bind_methods();

public:
	void _ready() override;
};
