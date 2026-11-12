// The graphics tier as the render holds it: each part from the moment it is given, whatever the order of a scene's calls (a tier
// applied before the core's boot, as a capture does, or after it, as the game does). A part never given is left unset, so the
// renderers stay as they are made.
#pragma once

#include <optional>

namespace city {

struct RenderTier {
	struct Lamps {
		int count = 0, casting = 0, heads = 0; // the street lamps' lights, those of them that cast, the vehicles' headlamps
	};
	struct Agents {
		float cabin_m = 0; // how far a vehicle is drawn with its cabin
		int cabin_cap = 0, people_floor = 0; // the most vehicles with one; the people's nearest level
	};
	std::optional<Lamps> lamps;
	std::optional<Agents> agents;
	std::optional<float> detail_scale, tree_share; // the buildings' detail; the share of the wild trees
};

} // namespace city
