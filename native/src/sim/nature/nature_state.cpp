// Painted fog at a point, and a new map's landscape (see nature_state.h).
#include "sim/nature/nature_state.h"

#include <cmath>

namespace city {

void fog_at(const std::vector<const FogDab *> &dabs, float x, float z, const FogDef &def, FogSample &out) {
	float d = 0, weight = 0, height = 0, speed = 0;
	for (const FogDab *dab : dabs) {
		float r = std::hypot(x - dab->x, z - dab->z) / dab->radius;
		if (r >= 1.f) continue;
		float s = 1.f - r * r, w = s * s * dab->strength;
		if (dab->erase) {
			d *= 1.f - w;
			continue;
		}
		d += (1.f - d) * w;
		if (weight == 0) {
			height = dab->height;
			speed = dab->speed;
		} else {
			height += (dab->height - height) * w;
			speed += (dab->speed - speed) * w;
		}
		weight += w;
	}
	out.density = d;
	out.height = weight > 0 ? height : def.height_m.def;
	out.speed = weight > 0 ? speed : def.speed_ms.def;
}

void NatureState::reset(const Weather &w, int region, uint32_t seed) {
	weather = w;
	clouds.clear();
	next_cloud = 1;
	fog.clear();
	ground.clear();
	planted.clear();
	nature = { region, seed };
	elements.clear();
	sky_revision++;
	fog_revision++;
}

} // namespace city
