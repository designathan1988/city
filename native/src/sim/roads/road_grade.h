// A road's height along its length when it stands clear of the ground (Roadcraft world/elevation.ts, structures.ts): the deck keeps
// its clearance over the ground under it, ramps carry it up from the ends, and a vertical curve rounds every change of grade.
#pragma once

#include <cstdint>
#include <functional>
#include <vector>

namespace city {

struct Tuning;

// What a road is built as (Roadcraft RoadStructure), in the segment's flags. The tunnel is not here: `CLAUDE.md`, Pendencias.
enum class RoadStructure : uint8_t { GROUND, ELEVATED, BRIDGE };

struct RaisedRules {
	float clearance = 0, ramp = 0, deck = 0, curve = 0, step = 1;
};

RaisedRules raised_rules(const Tuning &tn, RoadStructure structure);

// Heights at equal steps from 0 to `len`, from ya to yb, over the ground `ground(metres along)`.
std::vector<float> raised_heights(double len, float ya, float yb, const RaisedRules &rules, const std::function<float(double)> &ground);

// A profile at `share` of its length, 0 to 1.
float height_on(const std::vector<float> &heights, double share);

// What `height(share)` stands above the chord between `from` and `to` over a piece `len` long, at equal steps: a segment's lift.
std::vector<float> lift_over(const std::function<float(double)> &height, double from, double to, double len, float step);

} // namespace city
