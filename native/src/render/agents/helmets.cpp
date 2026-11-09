// A motorcyclist's helmet: its mesh, colours and instances (helmets.h).
#include "render/agents/helmets.h"
#include "core/shader_contracts/people.h"

#include "core/color.h"
#include "render/mesh_builder.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

using namespace godot;

namespace city {

namespace {
constexpr float SHELL_RADIUS = 0.5f; // the fit in people.bin is the shell's whole width
}

bool Helmets::init(const RenderContext &ctx, const DataValue &helmet, PackedStringArray &errors) {
	reset();
	material_ = ctx.material("people_helmet", errors);
	if (!errors.is_empty()) return false;
	const DataValue colours = helmet["colours"];
	const int count = colours.size();
	if (count == 0 || count > HELMET_COLOURS) {
		colours.fail(String("between 1 and ") + String::num_int64(HELMET_COLOURS) + String(" colours, what the shader holds"));
		return false;
	}
	PackedVector3Array linear;
	linear.resize(HELMET_COLOURS);
	for (int i = 0; i < count; i++) {
		const Color c = rgba8_to_color(colours[i].colour()).srgb_to_linear();
		linear.set(i, Vector3(c.r, c.g, c.b));
	}
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "helmet_colours", linear);
	rs->material_set_param(material_, "colour_count", count);
	rs->material_set_param(material_, "helmet_roughness", helmet["roughness"].f());
	MeshBuilder b;
	b.blob(Vector3(), Vector3(SHELL_RADIUS, SHELL_RADIUS, SHELL_RADIUS), Color(1, 1, 1));
	mesh_ = b.bake(material_);
	keeper_.clear();
	keeper_.draws(1);
	return true;
}

void Helmets::reset() {
	draw_.free();
}

void Helmets::begin() {
	rows_.reopen();
	keeper_.begin();
}

void Helmets::draw(const RID &scenario, const AABB &box, RenderLayer layer) {
	keeper_.sweep([&](int) -> InstanceRows & { return rows_; });
	draw_.set(scenario, mesh_.rid, rows_, box, true, layer);
}

} // namespace city
