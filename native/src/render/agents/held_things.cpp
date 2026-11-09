// What people hold: the meshes, one a thing, and the instances that follow the bodies (held_things.h).
#include "render/agents/held_things.h"

#include "core/color.h"
#include "render/mesh_builder.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

using namespace godot;

namespace city {

namespace {

} // namespace

bool HeldThings::init(const RenderContext &ctx, const DataValue &held, const char *const *act_names, PackedStringArray &errors) {
	reset();
	material_ = ctx.material("people_held", errors);
	if (!errors.is_empty()) return false;
	const DataValue kinds = held["things"];
	const int count = int(kinds.keys().size());
	things_ = std::vector<Thing>(size_t(count));
	int n = 0;
	for (int act = 0; act < int(PedAct::COUNT); act++) {
		thing_of_[act] = -1;
		if (!act_names[act] || !kinds.has(act_names[act])) continue;
		const DataValue kind = kinds[act_names[act]];
		const Vector2 upright(kind["upright"].flag() ? 1.f : 0.f, 0);
		MeshBuilder b;
		const DataValue parts = kind["parts"];
		for (int i = 0; i < parts.size(); i++) {
			const DataValue d = parts[i];
			const Vector3 at = d["at"].v3(), half = d["size"].v3() * .5f;
			b.box(at - half, at + half, rgba8_to_color(d["colour"].colour()), upright, true);
		}
		things_[size_t(n)].mesh = b.bake(material_);
		thing_of_[act] = n++;
	}
	if (n != count) {
		kinds.fail("a thing no act holds");
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
