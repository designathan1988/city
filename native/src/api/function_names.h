// A name in a data table or a document and what it stands for; the functions and uses of a building, as models.json and the documents
// write them. A home's function is its kind's business: FN_NONE.
#pragma once

#include "sim/buildings/building_store.h"
#include "sim/citygen/road_gen.h"

namespace city {

struct Named {
	const char *name;
	int value;
};

constexpr Named FUNCTION_NAMES[] = { { "house", FN_NONE }, { "townhouse", FN_NONE }, { "apartments", FN_NONE }, { "residentialTower", FN_NONE },
	{ "school", FN_SCHOOL }, { "church", FN_CHURCH }, { "hospital", FN_HOSPITAL }, { "clinic", FN_CLINIC }, { "square", FN_SQUARE }, { "park", FN_PARK },
	{ "gasStation", FN_GAS_STATION }, { "supermarket", FN_SUPERMARKET }, { "cityHall", FN_CITY_HALL }, { "police", FN_POLICE }, { "policeStation", FN_POLICE },
	{ "fireStation", FN_FIRE_STATION }, { "mall", FN_MALL }, { "sportsCourt", FN_SPORTS_COURT }, { "council", FN_COUNCIL }, { "courthouse", FN_COURTHOUSE },
	{ "postOffice", FN_POST_OFFICE }, { "university", FN_UNIVERSITY }, { "library", FN_LIBRARY }, { "museum", FN_MUSEUM }, { "prison", FN_PRISON },
	{ "cemetery", FN_CEMETERY }, { "busStation", FN_BUS_STATION }, { "shop", FN_SHOP }, { "bank", FN_BANK }, { "pharmacy", FN_PHARMACY }, { "bakery", FN_BAKERY },
	{ "restaurant", FN_RESTAURANT }, { "snackBar", FN_SNACK_BAR }, { "bar", FN_BAR }, { "nightclub", FN_NIGHTCLUB }, { "cinema", FN_CINEMA }, { "hotel", FN_HOTEL },
	{ "gym", FN_GYM }, { "club", FN_CLUB }, { "office", FN_OFFICE }, { "factory", FN_FACTORY }, { "warehouse", FN_WAREHOUSE }, { "playground", FN_PLAYGROUND } };

constexpr Named USE_NAMES[] = { { "residential", ZONE_RES }, { "commercial", ZONE_COM }, { "industrial", ZONE_IND }, { "mixed", ZONE_MIXED } };

} // namespace city
