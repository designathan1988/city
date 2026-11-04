// What the player built as the pieces of a street (Roadcraft streetFurniture.ts furnitureOf): the items and poles standing
// on a segment's sidewalks join its derived kit, the cuts leave it, and a derived piece an item stands on gives way. Read
// through traffic::street_furniture_of by the walkers and the render.
#pragma once

#include "sim/roads/street_furniture.h"
#include "sim/street/footway.h"
#include "sim/street/street_works.h"

#include <string>
#include <vector>

namespace city {

// A plate standing: where on the road's level, what it says, which way its face looks.
struct SignStand {
	float x = 0, y = 0, z = 0;
	SignType type = SignType::STOP;
	std::string text;
	float face_x = 0, face_z = 1;
};

// The street of a segment: through the nodes, the straightest continuation (edit_street_cos) with the same profile.
std::vector<int> street_chain(const RoadNetwork &net, int seg);
// The kind of piece an item is; FurnitureKind::COUNT for the ones with no footprint of their own.
FurnitureKind item_piece(ItemKind k);
// Applies the player's records to the derived pieces `out` of segment `sid`, of the kinds in `kinds`.
void merge_street(const StreetWorks &w, const RoadNetwork &net, int sid, uint32_t kinds, std::vector<FurnitureSpot> &out);
// The plates of a placed sign, or of a street's name at both ends of each segment of `chain`, on the right of the corner.
void placed_sign(const RoadNetwork &net, const StreetItem &item, std::vector<SignStand> &out);
void name_plates(const RoadNetwork &net, const StreetItem &item, const std::vector<int> &chain, std::vector<SignStand> &out);

} // namespace city
