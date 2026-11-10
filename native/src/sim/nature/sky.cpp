// The weather's limits and the placed clouds' geometry (see sky.h).
#include "sim/nature/sky.h"

#include "core/random.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float LEVEL_RAY = 1e-5f, NO_CLOUD_YET = 1e30f;

float wrapped(float v, float span) {
	return v - span * std::floor(v / span + 0.5f);
}
} // namespace

Weather default_weather(const WeatherDef &d) {
	Weather w;
#define CITY_WEATHER_DEFAULT(name) w.name = d.name.def;
	CITY_WEATHER_FIELDS(CITY_WEATHER_DEFAULT)
#undef CITY_WEATHER_DEFAULT
	return w;
}

Weather clamped_weather(const Weather &in, const WeatherDef &d) {
	Weather w;
#define CITY_WEATHER_CLAMP(name) w.name = d.name.clamp(in.name);
	CITY_WEATHER_FIELDS(CITY_WEATHER_CLAMP)
#undef CITY_WEATHER_CLAMP
	w.wind_direction = std::fmod(w.wind_direction, d.wind_direction.hi);
	return w;
}

PlacedCloud clamped_cloud(const PlacedCloud &in, const CloudDef &d) {
	PlacedCloud c = in;
	c.height = d.height_m.clamp(c.height);
	c.size = d.size_m.clamp(c.size);
	c.density = d.density.clamp(c.density);
	return c;
}

bool ViewRay::at_height(float y, float &x, float &z) const {
	if (std::fabs(dy) < LEVEL_RAY) return false;
	float t = (y - oy) / dy;
	if (t < 0) return false;
	x = ox + dx * t;
	z = oz + dz * t;
	return true;
}

DriftedCloud drifted_cloud(const PlacedCloud &c, float drift_x, float drift_z, float about_x, float about_z, const CloudDef &d) {
	float dx = wrapped(c.x + drift_x - about_x, d.span_m), dz = wrapped(c.z + drift_z - about_z, d.span_m);
	float out = (std::max(std::fabs(dx), std::fabs(dz)) - d.span_m * d.show_from) / (d.span_m * d.show_span);
	return { about_x + dx, about_z + dz, 1.f - smooth01(out) };
}

int cloud_under(const std::vector<PlacedCloud> &clouds, const ViewRay &ray, float drift_x, float drift_z, const CloudDef &d) {
	int best = -1;
	float nearest = NO_CLOUD_YET;
	for (const PlacedCloud &c : clouds) {
		float ax = ray.ox, az = ray.oz;
		ray.at_height(c.height, ax, az);
		DriftedCloud at = drifted_cloud(c, drift_x, drift_z, ax, az, d);
		float cy = c.height + c.size * d.pick_centre, radius = c.size * d.pick_radius;
		float ox = at.x - ray.ox, oy = cy - ray.oy, oz = at.z - ray.oz;
		float along = ox * ray.dx + oy * ray.dy + oz * ray.dz;
		float miss = ox * ox + oy * oy + oz * oz - along * along;
		if (miss >= radius * radius) continue;
		float entry = along - std::sqrt(radius * radius - miss);
		if (entry < nearest && along > 0) { nearest = entry; best = c.id; }
	}
	return best;
}

std::vector<PlacedCloud> scatter_clouds(int count, const PlacedCloud &like, float variation, const std::vector<PlacedCloud> &existing,
		uint32_t seed, const CloudDef &d) {
	std::vector<PlacedCloud> out;
	Rng r(uint64_t(seed) + 1);
	auto vary = [&](float v) { return v * (1.f + (r.f() * 2.f - 1.f) * variation); };
	float reach = d.span_m * 0.5f * d.scatter_reach;
	auto crowded = [&](const PlacedCloud &n, const std::vector<PlacedCloud> &list) {
		return std::any_of(list.begin(), list.end(), [&](const PlacedCloud &c) {
			return std::hypot(c.x - n.x, c.z - n.z) < (c.size + n.size) * d.crowd_share;
		});
	};
	for (int tries = 0; int(out.size()) < count && tries < count * d.scatter_tries; tries++) {
		PlacedCloud n = like;
		n.size = vary(like.size);
		n.x = (r.f() * 2.f - 1.f) * reach;
		n.z = (r.f() * 2.f - 1.f) * reach;
		n.height = vary(like.height);
		n.density = vary(like.density);
		n.yaw = r.f() * TAU_F;
		n = clamped_cloud(n, d);
		if (crowded(n, existing) || crowded(n, out)) continue;
		out.push_back(n);
	}
	return out;
}

} // namespace city
