// How fast a walker goes against its own pace (Roadcraft walk.ts stepWalkers, WOUNDED_PACE / WOUNDED_RUSH, PANIC_RUN): a
// frightened one runs (blast.json people.panic_gain); a wounded one never walks its own walk again (shot.pace_*), and getting
// away hurt is a run bent over the wound, a hobble on a hurt leg or when gravely hurt (traffic.json ped_rush_*), whatever
// the fright would have it do. shoot_people.cpp inflicts the wound, wounds.cpp bleeds it.
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {
enum class Wound { NONE, LIGHT, LEG, GRAVE };

// The grave (half the health gone or a limb lost), the leg, any other wound.
Wound wound_of(const BlastParams &b, const Pedestrian &w) {
	if (w.hp >= b.walker_hp) return Wound::NONE;
	if (w.hp < b.grave_hp || w.lost != 0) return Wound::GRAVE;
	return w.hurt_limb[2] + w.hurt_limb[3] > 0 ? Wound::LEG : Wound::LIGHT;
}
} // namespace

float pace_gain(const Catalog &c, const Pedestrian &w, bool fleeing, bool crossing) {
	const BlastParams &b = c.blast;
	const TrafficParams &p = c.traffic;
	// the hurry over a zebra is part of what the wound caps (walk.ts: the crossing gain inside `want`, then the wound's min)
	const float run = (fleeing ? b.panic_gain : 1.f) * (crossing ? p.ped_crossing_pace : 1.f);
	switch (wound_of(b, w)) {
	case Wound::NONE: return run;
	case Wound::LIGHT: return std::min(run, fleeing ? p.ped_rush_light : b.pace_light);
	case Wound::LEG: return std::min(run, fleeing ? p.ped_rush_leg : b.pace_leg);
	case Wound::GRAVE: return std::min(run, fleeing ? p.ped_rush_grave : b.pace_grave);
	}
	return run;
}

} // namespace traffic
} // namespace city
