// The hand-over between a chunk's near level and its mid one (building_renderer_reach.cpp fade_levels), as engines cross-fade their
// levels of detail: each sub-chunk's side of its switch for each level and the share of it still drawn, moving to that side over
// lod.level_fade_s; the near level's surfaces that carry the discarding twin; the clock that says how far a dissolve moves between two
// views; and the twins' materials with the instances that warm their pipelines.
#pragma once

#include "render/render_context.h"

#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/rid.hpp>

#include <array>
#include <cstdint>

namespace city {

// One level of one sub-chunk: its side of the switch (not yet seen, drawn, handed over to what follows), the share of it drawn, and
// whether it wears the discarding twin (dissolving, or dissolved and still in range).
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

// A sub-chunk's hand-over: its near level's and its parts', and the near level's surfaces that carry the twin (-1 none).
struct SubFade {
	LevelFade near, parts;
	int8_t wall_at = 0, glass_at = 0;
};

// The share of a dissolve the time since the last view covers, at most one: a stall ends it.
struct FadeClock {
	double last_s = -1;
	float step = 1;
	void tick(double now_s, float fade_s);
};

// The discarding twins' materials (the near walls, their glass, the parts), and the instances never drawn that ask the renderer for
// their pipelines before the first band needs them (building_renderer_reach.cpp warm_fades fills them).
struct FadeTwins {
	static constexpr int WARMERS = 3; // the walls with their glass, a part band, the parts' mesh
	godot::RID walls, glass, parts;
	std::array<godot::RID, WARMERS> warmers;
	void load(const RenderContext &ctx, godot::PackedStringArray &errors);
	void free_warmers();
};

} // namespace city
