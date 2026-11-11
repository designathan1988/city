// How high a kerb and the footway's bands beside it stand at an arc of a segment, side by side: the one owner the road surface draws
// by (road_segment.cpp) and the probes read (CityProbes::drive_kerbs). At a crossing mouth both kerbs drop to the crossing's foot
// (ramp_low within ramp_full of the mouth, flat to ramp_flat, back up over ramp_flare) and the whole crossing band slopes down to it.
// At a lot's car gate, a vehicle access (ABNT NBR 9050), that side's kerb drops to the drive's lip (drive_low) over the gate's width
// and rises back over drive_flare either side, and only the service strip beside it (drive_band) slopes: the walk behind stays level.
#pragma once

#include "render/roads/road_look.h"
#include "sim/roads/parking_layout.h"
#include "sim/state/city_view.h"

#include <vector>

namespace city {

// Where a segment's kerbs are lowered.
struct KerbDrops {
	double v0 = 0, v1 = 0; // the segment's mouths, in metres along it from node a
	bool ca = false, cb = false; // a crossing at the mouth at v0, at the one at v1
	std::vector<ArcSpan> drives[2]; // per side (STREET_RIGHT, STREET_LEFT), the arcs of the lots' car gates facing it
};

// The kerb drops of segment `sid` as the city stands (the job's cut or the live city under the map's lock): its mouths, a crossing at
// a mouth where a crosswalk lies over this arm (not on a ring or a rail), and the car gates of the lots facing each side.
KerbDrops kerb_drops_of(const CityView &s, int sid);
// The top of the kerb at arc `at` on `side`: the full kerb (L.h_curb) where nothing lowers it, down to a lowered kerb's foot.
float kerb_height(const StreetLook &L, const KerbDrops &k, int side, double at);
// The top of a footway band `from_kerb` metres from its kerb, at arc `at` on `side`: its walk height `walk_h`, down to each lowered
// kerb across the width that lowering takes (a crossing the footway's crossing band `band`, a drive the service strip).
float band_height(const StreetLook &L, const KerbDrops &k, int side, double at, float from_kerb, float band, float walk_h);
// Every arc along the segment where a lowering starts or ends its slope (the samples a surface must have to show it).
void kerb_marks(const StreetLook &L, const KerbDrops &k, std::vector<double> &out);

} // namespace city
