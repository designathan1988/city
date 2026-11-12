// The map's sky: its weather (rain, wind, storm, water, mist) and the clouds the player places, each its own size,
// base height and thickness. Pure data and geometry; the commands change it (sim/commands/nature_commands.cpp) and the
// render only reads it (project/render/sky_view.gd).
#pragma once

#include "core/math.h"
#include "sim/catalog/nature_types.h"

#include <cstdint>
#include <vector>

namespace city {

struct Weather {
#define CITY_WEATHER_MEMBER(name) float name = 0;
	CITY_WEATHER_FIELDS(CITY_WEATHER_MEMBER)
#undef CITY_WEATHER_MEMBER
	bool operator==(const Weather &o) const {
#define CITY_WEATHER_EQUAL(name) if (name != o.name) return false;
		CITY_WEATHER_FIELDS(CITY_WEATHER_EQUAL)
#undef CITY_WEATHER_EQUAL
		return true;
	}
};

Weather default_weather(const WeatherDef &d);
// `w` with every field inside its limits (the direction wrapped round the circle).
Weather clamped_weather(const Weather &w, const WeatherDef &d);

struct PlacedCloud {
	int32_t id = 0;
	float x = 0, z = 0; // over the cloud's middle
	float height = 0, size = 0; // its flat base over the sea, and its width (metres)
	float density = 0; // 0..1: thin and wispy .. a dense white heap
	float yaw = 0; // which way its puffs lie
	bool operator==(const PlacedCloud &o) const {
		return id == o.id && x == o.x && z == o.z && height == o.height && size == o.size && density == o.density && yaw == o.yaw;
	}
};

PlacedCloud clamped_cloud(const PlacedCloud &c, const CloudDef &d);

// A ray of the view: where it starts and its unit direction.
struct ViewRay {
	float ox = 0, oy = 0, oz = 0, dx = 0, dy = 0, dz = 0;
	// Where it crosses the level `y`; false when it runs away from it.
	bool at_height(float y, float &x, float &z) const;
};

// Where a cloud is drawn now: its repeat of the sky's period nearest `about`, and how much of it shows (1 .. 0 towards
// the edge it wraps round).
struct DriftedCloud {
	float x = 0, z = 0, show = 1;
};
DriftedCloud drifted_cloud(const PlacedCloud &c, float drift_x, float drift_z, float about_x, float about_z, const CloudDef &d);

// The id of the cloud under the ray among `clouds` (each seen as `drifted_cloud` draws it), the nearest the eye; -1 none.
int cloud_under(const std::vector<PlacedCloud> &clouds, const ViewRay &ray, float drift_x, float drift_z, const CloudDef &d);

// Up to `count` clouds spread over the sky (a procedural spawner: each is then the tool's to edit), none crowding
// another; each like `like`, varied by `variation` either way. Ids are the caller's.
std::vector<PlacedCloud> scatter_clouds(int count, const PlacedCloud &like, float variation, const std::vector<PlacedCloud> &existing,
		uint32_t seed, const CloudDef &d);

} // namespace city
