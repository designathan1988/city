// A fixture owner: how a post of a kind looks (the target's own type and function).
#pragma once

namespace city {

struct PostLookC {
	float height = 0;
	bool lit = false;
};

inline PostLookC post_look(int kind) { return PostLookC{float(kind), kind > 1}; }

} // namespace city
