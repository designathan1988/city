// The hand-over between a chunk's near level and its mid one (level_fade.h).
#include "render/buildings/level_fade.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>

using namespace godot;

namespace city {

float LevelFade::hand_over(float d, float end, float band, float step) {
	const float flip = side == IN ? OUT_SHARE : side == OUT ? IN_SHARE : FIRST_SHARE;
	const int8_t want = d < end - band * flip ? IN : OUT;
	if (side == UNSEEN) keep = want == IN ? 1.f : 0.f;
	side = want;
	keep = std::clamp(keep + (want == IN ? step : -step), 0.f, 1.f);
	return keep;
}

void FadeClock::tick(double now_s, float fade_s) {
	step = last_s < 0 || fade_s <= 0 ? 1.f : std::min(1.f, float((now_s - last_s) / fade_s));
	last_s = now_s;
}

} // namespace city
