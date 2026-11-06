#include "render/agents/helmets.h"

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
constexpr int MOST_COLOURS = 8; // people_helmet.gdshader helmet_colours
}

bool Helmets::init(const RenderContext &ctx, const Dictionary &helmet, PackedStringArray &errors) {
	reset();
	material_ = ctx.material("people_helmet", errors);
	if (!errors.is_empty()) return false;
	const Array colours = helmet["colours"];
	if (colours.is_empty() || colours.size() > MOST_COLOURS) {
		errors.push_back("agents.json held.helmet: cores fora do que o shader guarda");
		return false;
	}
	PackedVector3Array linear;
	linear.resize(MOST_COLOURS);
	for (int i = 0; i < colours.size(); i++) {
		const Color c = rgba8_to_color(parse_hex_color(String(colours[i]).utf8().get_data())).srgb_to_linear();
		linear.set(i, Vector3(c.r, c.g, c.b));
	}
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "helmet_colours", linear);
	rs->material_set_param(material_, "colour_count", int(colours.size()));
	rs->material_set_param(material_, "helmet_roughness", float(helmet["roughness"]));
	MeshBuilder b;
	b.blob(Vector3(), Vector3(SHELL_RADIUS, SHELL_RADIUS, SHELL_RADIUS), Color(1, 1, 1));
	mesh_ = b.bake();
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
	draw_.set(scenario, mesh_.rid, material_, rows_, box, true, layer);
}

} // namespace city
