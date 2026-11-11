// The one scale every lot's ground covers lie on: the lot's zones (lot_space.cpp), a civic building's and an open lot's own grounds
// (civic.cpp, civic_services.cpp, open_lots.cpp) and what is painted on them. A cover's thickness is its layer's, and the higher layer
// lies over the lower (render/buildings/lot_covers.h lays them in that order, the body's own order breaking a tie): a building's own
// ground is never under the lot's generic cover round it, a court's lines never under the court.
#pragma once

namespace city::b40 {

enum CoverLayer : int {
	COVER_GROUND, // the lot's ground: its zones round the building, an open lot's base
	COVER_WAY, // what is laid on that ground: drives, walks, an open lot's paths, plazas, borders and sand
	COVER_OWN, // a building's or a lot part's own ground over the lot's: a station's apron, a car park, a school's field, a court's
			   // run-off, the walk to a door, a terrace, a deck, sand, a pool's coping, a court's square, a car pad
	COVER_COURT, // a court on its own ground
	COVER_MARK, // what is painted on a ground or a court: lines, stalls, a centre circle
	COVER_INSET, // what lies inside a mark: a centre circle's fill
};

constexpr float COVER_BASE_H = 0.06f, COVER_LAYER_H = 0.02f; // the lowest layer's thickness, and what each layer over it adds

constexpr float cover_h(CoverLayer layer) { return COVER_BASE_H + COVER_LAYER_H * float(int(layer)); }

} // namespace city::b40
