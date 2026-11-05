// The Landscape drawer as script API: the sky, the painted ground, the planted trees, the map's biome and the relief's
// map actions. Commands hold map_mutex exclusively (run_edit); `landscape` is what the sky's view and the panels read.
#include "api/city_core.h"

#include "api/edit_result.h"

#include <godot_cpp/variant/array.hpp>

using namespace godot;

namespace city {

namespace {
PlacedCloud cloud_of(const Dictionary &d) {
	PlacedCloud c;
	c.x = float(double(d.get("x", 0.0)));
	c.z = float(double(d.get("z", 0.0)));
	c.height = float(double(d.get("height", 0.0)));
	c.size = float(double(d.get("size", 0.0)));
	c.density = float(double(d.get("density", 0.0)));
	c.yaw = float(double(d.get("yaw", 0.0)));
	return c;
}

Dictionary cloud_dict(const PlacedCloud &c) {
	Dictionary d;
	d["id"] = c.id;
	d["x"] = c.x;
	d["z"] = c.z;
	d["height"] = c.height;
	d["size"] = c.size;
	d["density"] = c.density;
	d["yaw"] = c.yaw;
	return d;
}
} // namespace

Dictionary CityCore::set_weather(const Dictionary &change) {
	return run_edit(*state_, "cmd.weather", [&](CityState &s) {
		Weather w = s.nature.weather;
#define CITY_WEATHER_SET(name) if (change.has(#name)) w.name = float(double(change[#name]));
		CITY_WEATHER_FIELDS(CITY_WEATHER_SET)
#undef CITY_WEATHER_SET
		return city::set_weather(s, w);
	});
}

Dictionary CityCore::add_cloud(const Dictionary &c) {
	return run_edit(*state_, "cmd.cloud.add", [&](CityState &s) { return city::add_cloud(s, cloud_of(c)); });
}

Dictionary CityCore::edit_cloud(int id, const Dictionary &c) {
	return run_edit(*state_, "cmd.cloud.edit", [&](CityState &s) { return city::edit_cloud(s, id, cloud_of(c)); });
}

Dictionary CityCore::remove_cloud(int id) {
	return run_edit(*state_, "cmd.cloud.remove", [&](CityState &s) { return city::remove_cloud(s, id); });
}

Dictionary CityCore::scatter_clouds(int count, const Dictionary &like, float variation) {
	return run_edit(*state_, "cmd.cloud.scatter", [&](CityState &s) { return scatter_sky(s, count, cloud_of(like), variation); });
}

Dictionary CityCore::clear_clouds() {
	return run_edit(*state_, "cmd.cloud.clear", [&](CityState &s) { return city::clear_clouds(s); });
}

int CityCore::cloud_at(const Vector3 &origin, const Vector3 &direction, float drift_x, float drift_z) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	ViewRay ray{ origin.x, origin.y, origin.z, direction.x, direction.y, direction.z };
	return cloud_under(s.nature.clouds, ray, drift_x, drift_z, s.catalog.nature.clouds);
}

Dictionary CityCore::fog_brush(const Vector3 &p, float radius, float strength, float height, float speed, bool erase) {
	return run_edit(*state_, "cmd.fog", [&](CityState &s) {
		FogDab d;
		d.x = p.x; d.z = p.z; d.radius = radius; d.strength = strength; d.height = height; d.speed = speed;
		d.erase = erase ? 1 : 0;
		return fog_dab(s, d);
	});
}

Dictionary CityCore::clear_fog() {
	return run_edit(*state_, "cmd.fog.clear", [](CityState &s) { return city::clear_fog(s); });
}

Dictionary CityCore::paint_brush(const Vector3 &p, float radius, float strength, float hardness, int kind) {
	return run_edit(*state_, "cmd.place.paintGround", [&](CityState &s) {
		PaintDab d;
		d.x = p.x; d.z = p.z; d.radius = radius; d.strength = strength; d.hardness = hardness; d.kind = kind;
		return paint_dab(s, d);
	});
}

Dictionary CityCore::clear_gullies() {
	return run_edit(*state_, "cmd.gully.clear", [](CityState &s) { return city::clear_gullies(s); });
}

Dictionary CityCore::tree_brush(const Vector3 &p, float radius, int mode, int kind, const Dictionary &brush) {
	return run_edit(*state_, "cmd.place.tree", [&](CityState &s) {
		TreeBrush b;
		b.density = float(double(brush.get("density", 0.0)));
		b.height = float(double(brush.get("height", 0.0)));
		b.variation = float(double(brush.get("variation", 0.0)));
		b.spacing = float(double(brush.get("spacing", 0.0)));
		return tree_dab(s, p.x, p.z, radius, mode, kind, b);
	});
}

Dictionary CityCore::element_brush(const Vector3 &p, float radius, int kind, const Dictionary &brush, bool erase) {
	return run_edit(*state_, "cmd.place.element", [&](CityState &s) {
		ElementBrush b;
		b.density = float(double(brush.get("density", 0.0)));
		b.size = float(double(brush.get("size", 0.0)));
		b.variation = float(double(brush.get("variation", 0.0)));
		b.spacing = float(double(brush.get("spacing", 0.0)));
		b.strength = float(double(brush.get("strength", 0.0)));
		b.intensity = float(double(brush.get("intensity", 0.0)));
		return element_dab(s, p.x, p.z, radius, kind, b, erase);
	});
}

Dictionary CityCore::clear_elements() {
	return run_edit(*state_, "cmd.elements.clear", [](CityState &s) { return city::clear_elements(s); });
}

// The effects standing within `reach`, for render/element_effects.gd: kind, place, spread and how much each gives off.
Array CityCore::effects_near(const Vector3 &p, float reach, int first_effect) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Array out;
	state_->nature.elements.for_near(p.x, p.z, reach, [&](const ElementItem &e) {
		if (e.kind < first_effect) return;
		Dictionary d;
		d["kind"] = e.kind - first_effect;
		d["position"] = Vector3(e.x, state_->terrain.height_at(e.x, e.z), e.z);
		d["size"] = e.size;
		d["intensity"] = e.intensity;
		out.push_back(d);
	});
	return out;
}

Dictionary CityCore::clear_trees() {
	return run_edit(*state_, "cmd.trees.clear", [](CityState &s) { return city::clear_trees(s); });
}

Dictionary CityCore::set_biome(int region) {
	return run_edit(*state_, "cmd.biome", [&](CityState &s) { return city::set_biome(s, region); });
}

Dictionary CityCore::clear_relief() {
	return run_edit(*state_, "cmd.place.flattenMap", [](CityState &s) { return city::clear_relief(s); });
}

Dictionary CityCore::landscape() const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const NatureState &n = state_->nature;
	Dictionary d, w;
#define CITY_WEATHER_GET(name) w[#name] = n.weather.name;
	CITY_WEATHER_FIELDS(CITY_WEATHER_GET)
#undef CITY_WEATHER_GET
	Array clouds;
	for (const PlacedCloud &c : n.clouds) clouds.push_back(cloud_dict(c));
	d["weather"] = w;
	d["clouds"] = clouds;
	d["biome"] = n.nature.region;
	d["revision"] = int64_t(n.sky_revision);
	d["fog_dabs"] = int64_t(n.fog.size());
	d["planted"] = n.planted.live_trees();
	d["clearings"] = n.planted.live_clearings();
	d["paint_tiles"] = int64_t(n.ground.all().size());
	d["elements"] = int64_t(n.elements.count());
	return d;
}

int64_t CityCore::sky_revision() const { return int64_t(state_->nature.sky_revision); }

Dictionary CityCore::fog_field() const { return render_.fog().view(); }

int64_t CityCore::fog_revision() const { return int64_t(render_.fog().revision()); }

} // namespace city
