// The hand-over between a chunk's near level and its mid one (building_renderer_reach.cpp fade_levels), as engines cross-fade their
// levels of detail: each sub-chunk's side of its switch for each level and the share of it still drawn, moving to that side over
// lod.level_fade_s, and the clock that says how far a dissolve moves between two views. The dissolve is the levels' own materials'
// (building_level.gdshaderinc, by the instance's level_keep): no material is swapped, so no pipeline is made in play.
#pragma once

#include <cstdint>

namespace city {

// One level of one sub-chunk: its side of the switch (not yet seen, drawn, handed over to what follows), the share of it drawn, and
// whether it is dissolving (or dissolved and still in range).
struct LevelFade {
	static constexpr int8_t UNSEEN = 0, IN = 1, OUT = -1;
	// Where in the band the switch flips, as shares of the band back from the level's end: out past the first, in again within the
	// second, so an eye standing still in the band stands on one side, and the dissolve has the band's last quarter to end before the
	// range does.
	static constexpr float OUT_SHARE = 0.25f, IN_SHARE = 0.75f, FIRST_SHARE = 0.5f;
	int8_t side = UNSEEN;
	float keep = 0;
	bool twin = false;
	// At `d` from the eye, its range ending at `end` with a band `band` deep: its side of the switch, and the share of it drawn moved
	// `step` towards that side. Seen for the first time it stands at its side at once (it comes up with the chunk). Returns the share.
	float hand_over(float d, float end, float band, float step);
};

// A sub-chunk's hand-over: its near level's and its parts'.
struct SubFade {
	LevelFade near, parts;
};

// The share of a dissolve the time since the last view covers, at most one: a stall ends it.
struct FadeClock {
	double last_s = -1;
	float step = 1;
	void tick(double now_s, float fade_s);
};

} // namespace city
