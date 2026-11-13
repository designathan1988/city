// The street's pieces in volume where something asks about them: every piece the segments near a box keep (street_furniture_of,
// FurnitureRead::KEPT, read under the map's shared lock) and every plate standing in the box (the rules' at its junctions, the
// player's, the streets' names), each as its rectangle on the ground and its parts' boxes over its foot (sim/roads/standing_body.h).
// One owner for whoever asks: the street's audit over the whole map (api/city_core_bodies_probe.cpp) and a body thrown among them
// (render/agents/body_walls.cpp).
#pragma once

#include "sim/roads/standing_body.h"
#include "sim/state/city_state.h"

#include <vector>

namespace city {

// A piece of the street: its spot (a plate: kind SIGN, station its SignType), its rectangle on the ground, its parts as boxes.
struct StreetPiece {
	FurnitureSpot spot;
	StandingBody body;
	std::vector<BodyBox> boxes;
};

// The pieces standing within the box (x0, z0)-(x1, z1), appended to `out`: the segments the box may reach, through the network's
// hash. The caller holds the map shared.
void street_pieces_in(const CityState &s, float x0, float z0, float x1, float z1, std::vector<StreetPiece> &out);
// Every piece of the map, segment by segment and junction by junction (a box round the map would walk every cell of its hash).
void street_pieces_all(const CityState &s, std::vector<StreetPiece> &out);

} // namespace city
