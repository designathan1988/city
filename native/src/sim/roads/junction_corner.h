// A junction corner's outline, the one owner of its shape: between arm a and the next arm anticlockwise b (junction.h corner_fillet),
// the kerb line from a's mouth round to b's, the back of its kerb stone and the back of its footway. The road surfaces draw it
// (render/roads/road_junction.cpp) and start each arm's footway where it says (road_segment.cpp, road_verge.cpp).
//
// The footway's back, as the reference lays a corner's footway, fitted to this game's lots, which end square where two footways
// meet: squared on that point when it stands short of both mouths; else the kerb line moved out by the footway's widths (the
// reference's back concentric with the return, where the return reaches past the point); else the footway level's own run (the
// point lies past a mouth, the footway wider than the return): each arm's footway runs on along it to the point. Whichever is taken
// is a simple polygon, checked side against side, and the kerb line never stands past either arm's mouth.
#pragma once

#include "core/math.h"
#include "sim/catalog/profile_types.h"
#include "sim/roads/geometry.h"

#include <cstdint>
#include <vector>

namespace city {

// A road's section on one side as a corner reads it, metres from the centre line: the kerb line (the carriageway's edge), the back
// of its kerb stone (the CURB strip at the carriageway's edge when a raised footway lies against it, else the kerb line), the outer
// edge; whether a raised footway lies on that side, and whether a kerb stone does.
struct SideSection {
	float kerb = 0, back = 0, outer = 0;
	bool raised = false, stone = false;
};
// `plus_u`: the profile's +u side (its left looking from a to b).
SideSection side_section(const Profile &p, bool plus_u);

// One arm's side of a corner in the node's frame (metres from the node): its carriageway's mouth on the centre line, the unit
// outward along the arm there, its section on the corner's side and the mouth's setback.
struct CornerSide {
	V2 mouth, dir;
	SideSection section;
	double at = 0;
};

enum class CornerBack : uint8_t { SQUARE, OFFSET, RUN };

struct CornerOutline {
	std::vector<V2> kerb; // the kerb line, from a's mouth to b's
	std::vector<V2> stone; // each kerb station moved out to the kerb stone's back
	std::vector<float> share; // of each kerb station: 0 at a's mouth to 1 at b's, what its height follows between the two
	// The footway's inner side, from a's footway mouth to b's: along a's run back to its mouth, the stone, out along b's run.
	std::vector<V2> inner;
	std::vector<V2> back; // the footway's back, from a's outer edge at its footway mouth to b's
	std::vector<float> back_share;
	float run_a = 0, run_b = 0; // how far past its carriageway's mouth each side's footway starts, along its arm
	CornerBack kind = CornerBack::OFFSET;
	bool simple = true; // the footway's polygon (inner, then back the other way) has no side across or along another
};

// `a` is arm a's left side, `b` arm b's right side; the arms as junction.h corner_fillet takes them. `arc_step`: the return's
// angle between stations; `bevel_steps`: the stations of a corner whose kerb is no return's arc.
CornerOutline corner_outline(const Arm &arm_a, const Arm &arm_b, const CornerSide &a, const CornerSide &b, float arc_step, int bevel_steps);

// The game's left of a direction in the plane (x, z): (z, -x).
inline V2 left_of(const V2 &d) { return V2(d.y, -d.x); }

} // namespace city
