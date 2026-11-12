// What people hold: the meshes, one a thing, and the instances that follow the bodies (held_things.h).
#include "render/agents/held_things.h"
#include "render/agents/people_asset.h"

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

bool HeldThings::init(const RenderContext &ctx, const Dictionary &held, const char *const *act_names, const PeopleAsset &asset, PackedStringArray &errors) {
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
		Thing &thing = things_[size_t(n)];
		thing.mesh = b.bake();
		thing.draw = std::make_unique<SpatialInstanceDraw>();
		thing.draw->configure(ctx.scenario, thing.mesh.rid, material_, true, LAYER_PEOPLE, float(ctx.catalog->tune.render_instance_sector_m));
		const Vector3 fist = triple(held["fist"]);
		const AABB rest = mesh_outline(b).merge(AABB(fist, Vector3()));
		const float extra = rest.position.abs().max(rest.get_end().abs()).length();
		// Both branches of people_held: palm-space growth around the fist, or an upright mesh added after posing it.
		for (size_t v = 0; v < asset.variations.size(); v++)
			thing.envelopes.push_back(asset.attachment_bounds(v, false, instance_bounds(asset.variations[v].palm, rest)).grow(extra));
		thing_of_[act] = n++;
	}
	if (n != kinds.size()) {
		errors.push_back("agents.json held: uma coisa sem ato que a segure");
		return false;
	}
	keeper_.clear();
	keeper_.draws(size_t(n));
	return true;
}

void HeldThings::reset() {
	for (Thing &t : things_) {
		if (t.draw) t.draw->reset();
		t.rows.restart();
	}
	keeper_.clear();
	keeper_.draws(things_.size());
}

void HeldThings::begin() {
	for (Thing &t : things_) t.rows.reopen();
	keeper_.begin();
}

float *HeldThings::row(uint64_t key, int act) {
	const int k = act >= 0 && act < int(PedAct::COUNT) ? thing_of_[act] : -1;
	return k < 0 ? nullptr : keeper_.place(key, k, things_[size_t(k)].rows);
}

void HeldThings::draw() {
	keeper_.sweep([&](int k) -> InstanceRows & { return things_[size_t(k)].rows; });
	for (size_t k = 0; k < things_.size(); k++) {
		Thing &t = things_[k];
		if (!t.draw) continue;
		t.draw->begin();
		const auto &keys = keeper_.owners(k);
		for (int i = 0; i < t.rows.count(); i++) {
			const float *row = t.rows.data() + int64_t(i) * MM_STRIDE;
			const int variation = int(row[16] + .5f);
			t.draw->put(keys[size_t(i)], row, instance_bounds(row, t.envelopes[size_t(variation)]));
		}
		t.draw->finish();
	}
}

} // namespace city
