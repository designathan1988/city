// How a wound slows a walker (shoot_people.cpp inflicts it, wounds.cpp bleeds it).
#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

// The share of its own pace a hurt walker keeps (shot.pace_*): all of it unhurt, the grave (half the health gone or a limb
// lost), the leg, any other wound.
float wounded_pace(const BlastParams &b, const Pedestrian &w) {
	if (w.hp >= b.walker_hp) return 1.f;
	if (w.hp < b.grave_hp || w.lost != 0) return b.pace_grave;
	return w.hurt_limb[2] + w.hurt_limb[3] > 0 ? b.pace_leg : b.pace_light;
}

} // namespace traffic
} // namespace city
