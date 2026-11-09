// What a cooked clip says on one body, as the lab's gait reads it (citizenGait.ts naturalPace, fractionAtTravel, fractionAtYaw, travelAt,
// clipSpeed, stopEntry, startEntry): its seconds for the body's sex, its natural pace at a size, its curve (the ground a start or stop has
// covered, the angle a turn has turned) at a fraction and back, and a clip played by the clock from each person's own place in it.
#pragma once

#include "render/agents/people_asset.h"

#include <cstdint>

namespace city {

class GaitClipFacts {
public:
	// `spread`: each person's own place in a clip played by the clock, a share of a turn per seed step (agents.json people.gait.idle_spread).
	void init(const PeopleAsset &asset, float spread) {
		asset_ = &asset;
		spread_ = spread;
	}
	int female(int body) const;
	float seconds(int clip, int body) const;
	float stride(int clip, int body, float size) const; // the ground one cycle covers on this body at `size`, metres
	float natural_pace(int clip, int body, float size) const;
	float clock_phase(int clip, int body, double clock, uint32_t seed) const;
	float curve_at(int clip, int body, float fraction) const;
	float fraction_at(int clip, int body, float value) const; // where a monotone curve reaches `value`
	float clip_speed(int clip, int body, int frame, float size) const; // a forward clip's speed at a frame, m/s
	float stop_entry(int stop, int body, float speed, float size) const; // past its fastest point, where the stop moves at `speed`
	float start_entry(int start, int body, float speed, float size) const; // where the start first moves at `speed`

private:
	const PeopleAsset *asset_ = nullptr;
	float spread_ = 0;
	int last_frame(int clip) const; // a loop's curve ends at its first row again, a one-shot's at its last
};

} // namespace city
