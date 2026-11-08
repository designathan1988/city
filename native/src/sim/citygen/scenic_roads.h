// The streets of a scenic city (sim/catalog/scenic.h) as a generated graph: each district's grid inside its cell (the city's
// square split between the district seeds, Voronoi), running a little past it to meet the seam streets laid on the cells'
// borders, and the named roads on their lines; district streets stop at water, in parks and short of the highways, the named
// main roads bridge the rivers. The pieces then go through the generated city's graph builder under the scenic rules
// (scenic_rules.h): crossings, merged nodes, zones, tidying, the cut ends taken back.
#pragma once

#include "sim/citygen/road_gen.h"

namespace city {

struct Catalog;
struct ScenicCity;

GeneratedRoads scenic_roads(const ScenicCity &city, const Catalog &catalog, TerrainGen &land);

} // namespace city
