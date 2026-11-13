// What people hold: the meshes, one a thing, and the instances that follow the bodies (held_things.h).
#include "render/agents/held_things.h"

#include "core/color.h"
#include "render/mesh_builder.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

using namespace godot;

namespace city {

namespace {

Vector3 triple(const Variant &v) {
	const Array a = v;
	return Vector3(float(a[0]), float(a[1]), float(a[2]));
}

} // namespace

bool HeldThings::init(const RenderContext &ctx, const Dictionary &held, const char *const *act_names, PackedStringArray &errors) {
	reset();
	material_ = ctx.material("people_held", errors);
	if (!errors.is_empty()) return false;
	const Dictionary kinds = held["things"];
	things_ = std::vector<Thing>(size_t(kinds.size()));
	int n = 0;
	for (int act = 0; act < int(PedAct::COUNT); act++) {
		thing_of_[act] = -1;
		if (!act_names[act] || !kinds.has(act_names[act])) continue;
		const Dictionary kind = kinds[act_names[act]];
		const Vector2 upright(bool(kind["upright"]) ? 1.f : 0.f, 0);
		MeshBuilder b;
		for (const Variant &part : Array(kind["parts"])) {
			const Dictionary d = part;
			const Vector3 at = triple(d["at"]), half = triple(d["size"]) * .5f;
			b.box(at - half, at + half, rgba8_to_color(parse_hex_color(String(d["colour"]).utf8().get_data())), upright, true);
		}
		things_[size_t(n)].mesh = b.bake(material_).shared();
		thing_of_[act] = n++;
	}
	if (n != kinds.size()) {
		errors.push_back("agents.json held: a thing with no act that holds it");
		return false;
	}
	keeper_.clear();
	keeper_.draws(size_t(n));
	return true;
}

void HeldThings::reset() {
	for (Thing &t : things_) t.draw.free();
}

void HeldThings::begin() {
	for (Thing &t : things_) t.rows.reopen();
	keeper_.begin();
}

float *HeldThings::row(uint64_t key, int act) {
	const int k = act >= 0 && act < int(PedAct::COUNT) ? thing_of_[act] : -1;
	return k < 0 ? nullptr : keeper_.place(key, k, things_[size_t(k)].rows);
}

void HeldThings::draw(const RID &scenario, const AABB &box, RenderLayer layer) {
	keeper_.sweep([&](int k) -> InstanceRows & { return things_[size_t(k)].rows; });
	for (Thing &t : things_) t.draw.set(scenario, t.mesh.rid, t.rows, box, true, layer);
}

} // namespace city
