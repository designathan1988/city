// How a job-path function reads the city (CityView): references to the stores it reads. A reader that holds the map hands its
// CityState and the view reads the live stores in place; a mesh job hands the view of its cut (sim/state/city_cut.h), the copy
// it made of its place under a short lock. The same functions draw from both, so no producer builds while it holds the map.
// It holds what the converted producers read: a store joins it with the first producer that cuts it.
#pragma once

#include "sim/state/city_state.h"
#include "sim/terrain/ground_patch.h"

namespace city {

struct CityCut;
struct CutFacts;

struct CityView {
	const Catalog &catalog;
	GroundRead terrain;
	const RoadNetwork &roads;
	const StreetWorks &street;
	const BuildingRecords &buildings;
	const b40::Data &designs; // the generator's tables: the session's, read beside the records
	const NatureState &nature; // what the plants read of it: painted ground, planted trees and clearings, elements, ecosystem
	const CarriedAtomic<int> &marking_style;
	uint64_t revision;
	uint32_t seed;
	const CityState *live; // the city read in place (its reader holds the map); null for a cut
	const CutFacts *facts; // what a cut carries of the live city's kept facts (city_cut.h); null for the live city

	CityView(const CityState &s) :
			catalog(s.catalog), terrain(s.terrain), roads(s.roads), street(s.street), buildings(s.buildings), designs(s.buildings.designs),
			nature(s.nature), marking_style(s.marking_style), revision(s.revision), seed(s.seed), live(&s), facts(nullptr) {}
	CityView(const CityCut &cut); // city_cut.h
};

} // namespace city
