// The Landscape drawer as script API: the sky, the painted ground, the planted trees, the map's biome and the relief's
// map actions. Commands hold map_mutex exclusively (run_edit); `landscape` is what the sky's view and the panels read.
#include "api/city_landscape.h"

#include "api/edit_result.h"
#include "sim/state/city_state.h"
#include "render/render_world.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/core/class_db.hpp>

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

Dictionary CityLandscape::set_weather(const Dictionary &change) {
	return run_edit(*state_, "cmd.weather", [&](CityState &s) {
		Weather w = s.nature.weather;
#define CITY_WEATHER_SET(name) if (change.has(#name)) w.name = float(double(change[#name]));
		CITY_WEATHER_FIELDS(CITY_WEATHER_SET)
#undef CITY_WEATHER_SET
		return city::set_weather(s, w);
	});
}

Dictionary CityLandscape::add_cloud(const Dictionary &c) {
	return run_edit(*state_, "cmd.cloud.add", [&](CityState &s) { return city::add_cloud(s, cloud_of(c)); });
}

Dictionary CityLandscape::edit_cloud(int id, const Dictionary &c) {
	return run_edit(*state_, "cmd.cloud.edit", [&](CityState &s) { return city::edit_cloud(s, id, cloud_of(c)); });
}

Dictionary CityLandscape::remove_cloud(int id) {
	return run_edit(*state_, "cmd.cloud.remove", [&](CityState &s) { return city::remove_cloud(s, id); });
}

Dictionary CityLandscape::scatter_clouds(int count, const Dictionary &like, float variation) {
	return run_edit(*state_, "cmd.cloud.scatter", [&](CityState &s) { return scatter_sky(s, count, cloud_of(like), variation); });
}

Dictionary CityLandscape::clear_clouds() {
	return run_edit(*state_, "cmd.cloud.clear", [&](CityState &s) { return city::clear_clouds(s); });
}

int CityLandscape::cloud_at(const Vector3 &origin, const Vector3 &direction, float drift_x, float drift_z) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	ViewRay ray{ origin.x, origin.y, origin.z, direction.x, direction.y, direction.z };
	return cloud_under(s.nature.clouds, ray, drift_x, drift_z, s.catalog.nature.clouds);
}

Dictionary CityLandscape::fog_brush(const Vector3 &p, float radius, float strength, float height, float speed, bool erase) {
	return run_edit(*state_, "cmd.fog", [&](CityState &s) {
		FogDab d;
		d.x = p.x; d.z = p.z; d.radius = radius; d.strength = strength; d.height = height; d.speed = speed;
		d.erase = erase ? 1 : 0;
		return fog_dab(s, d);
	});
}

Dictionary CityLandscape::clear_fog() {
	return run_edit(*state_, "cmd.fog.clear", [](CityState &s) { return city::clear_fog(s); });
}

Dictionary CityLandscape::paint_brush(const Vector3 &p, float radius, float strength, float hardness, int kind) {
	return run_edit(*state_, "cmd.place.paintGround", [&](CityState &s) {
		PaintDab d;
		d.x = p.x; d.z = p.z; d.radius = radius; d.strength = strength; d.hardness = hardness; d.kind = kind;
		return paint_dab(s, d);
	});
}

Dictionary CityLandscape::clear_gullies() {
	return run_edit(*state_, "cmd.gully.clear", [](CityState &s) { return city::clear_gullies(s); });
}

Dictionary CityLandscape::tree_brush(const Vector3 &p, float radius, int mode, int kind, const Dictionary &brush) {
	return run_edit(*state_, "cmd.place.tree", [&](CityState &s) {
		TreeBrush b;
		b.density = float(double(brush.get("density", 0.0)));
		b.height = float(double(brush.get("height", 0.0)));
		b.variation = float(double(brush.get("variation", 0.0)));
		b.spacing = float(double(brush.get("spacing", 0.0)));
		return tree_dab(s, p.x, p.z, radius, mode, kind, b);
	});
}

Dictionary CityLandscape::element_brush(const Vector3 &p, float radius, int kind, const Dictionary &brush, bool erase) {
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

Dictionary CityLandscape::clear_elements() {
	return run_edit(*state_, "cmd.elements.clear", [](CityState &s) { return city::clear_elements(s); });
}

// The effects standing within `reach`, for render/element_effects.gd: kind, place, spread and how much each gives off.
Array CityLandscape::effects_near(const Vector3 &p, float reach, int first_effect) const {
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

Dictionary CityLandscape::clear_trees() {
	return run_edit(*state_, "cmd.trees.clear", [](CityState &s) { return city::clear_trees(s); });
}

Dictionary CityLandscape::set_biome(int region) {
	return run_edit(*state_, "cmd.biome", [&](CityState &s) { return city::set_biome(s, region); });
}

Dictionary CityLandscape::clear_relief() {
	return run_edit(*state_, "cmd.place.flattenMap", [](CityState &s) { return city::clear_relief(s); });
}

Dictionary CityLandscape::landscape() const {
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

int64_t CityLandscape::sky_revision() const { return int64_t(state_->nature.sky_revision); }

Dictionary CityLandscape::fog_field(const Vector3 &camera) {
	render_->fog().sync(*state_, camera.x, camera.z);
	return render_->fog().view();
}

Dictionary CityLandscape::canyon_field(const Vector3 &camera) {
	render_->canyon().sync(*state_, camera.x, camera.z);
	return render_->canyon().view();
}

Dictionary CityLandscape::shore_field() {
	render_->shore().sync(*state_);
	return render_->shore().view();
}

void CityLandscape::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_weather", "change"), &CityLandscape::set_weather);
	ClassDB::bind_method(D_METHOD("add_cloud", "cloud"), &CityLandscape::add_cloud);
	ClassDB::bind_method(D_METHOD("edit_cloud", "id", "cloud"), &CityLandscape::edit_cloud);
	ClassDB::bind_method(D_METHOD("remove_cloud", "id"), &CityLandscape::remove_cloud);
	ClassDB::bind_method(D_METHOD("scatter_clouds", "count", "like", "variation"), &CityLandscape::scatter_clouds);
	ClassDB::bind_method(D_METHOD("clear_clouds"), &CityLandscape::clear_clouds);
	ClassDB::bind_method(D_METHOD("cloud_at", "origin", "direction", "drift_x", "drift_z"), &CityLandscape::cloud_at);
	ClassDB::bind_method(D_METHOD("fog_brush", "position", "radius", "strength", "height", "speed", "erase"), &CityLandscape::fog_brush);
	ClassDB::bind_method(D_METHOD("clear_fog"), &CityLandscape::clear_fog);
	ClassDB::bind_method(D_METHOD("paint_brush", "position", "radius", "strength", "hardness", "kind"), &CityLandscape::paint_brush);
	ClassDB::bind_method(D_METHOD("clear_gullies"), &CityLandscape::clear_gullies);
	ClassDB::bind_method(D_METHOD("tree_brush", "position", "radius", "mode", "kind", "brush"), &CityLandscape::tree_brush);
	ClassDB::bind_method(D_METHOD("clear_trees"), &CityLandscape::clear_trees);
	ClassDB::bind_method(D_METHOD("element_brush", "position", "radius", "kind", "brush", "erase"), &CityLandscape::element_brush);
	ClassDB::bind_method(D_METHOD("clear_elements"), &CityLandscape::clear_elements);
	ClassDB::bind_method(D_METHOD("effects_near", "position", "reach", "first_effect"), &CityLandscape::effects_near);
	ClassDB::bind_method(D_METHOD("set_biome", "region"), &CityLandscape::set_biome);
	ClassDB::bind_method(D_METHOD("clear_relief"), &CityLandscape::clear_relief);
	ClassDB::bind_method(D_METHOD("landscape"), &CityLandscape::landscape);
	ClassDB::bind_method(D_METHOD("sky_revision"), &CityLandscape::sky_revision);
	ClassDB::bind_method(D_METHOD("fog_field", "camera"), &CityLandscape::fog_field);
	ClassDB::bind_method(D_METHOD("canyon_field", "camera"), &CityLandscape::canyon_field);
	ClassDB::bind_method(D_METHOD("shore_field"), &CityLandscape::shore_field);
}

} // namespace city
