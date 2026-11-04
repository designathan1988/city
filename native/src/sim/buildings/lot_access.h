// The ways in and out of a lot, for the traffic that parks, delivers and walks there, read back from the lot's laid
// elements (sim/buildings/b40/lot_*.cpp): its gates, and each car bay and lorry dock with the way a vehicle drives from
// in front of it out through its gate. The way is found on a grid over the lot (a cell a quarter metre): every solid
// part and the building's walls are grown by the vehicle's half width (its configuration space), the distance to each
// car gate spread over the free cells (Dijkstra, eight neighbours), the cells walked down from the bay, and the walk
// pulled straight where the line of sight is free. A bay no gate reaches keeps an empty way (the traffic leaves it).
// World coordinates (XZ). Made on demand for the lots the traffic looks at, never for the whole map.
#pragma once

#include "core/math.h"
#include "sim/buildings/b40/record.h"
#include "sim/catalog/traffic_params.h"

#include <vector>

namespace city {

struct LotGate {
	V2 at; // the middle of the gate
	V2 in; // unit direction into the lot
	float w = 0;
	bool car = false;
};

struct LotBay {
	V2 at; // the middle of the bay
	V2 open; // unit direction its open end looks (a car drives in against it)
	float w = 0, d = 0;
	bool dock = false; // a lorry's dock (else a car's bay)
	int gate = -1; // the gate its way leads to (-1: none reaches it)
	std::vector<V2> way; // from in front of the bay to just inside its gate
};

struct LotAccess {
	std::vector<LotGate> gates;
	std::vector<LotBay> bays;
	int reached() const;
};

// The lot's access. `car_half` / `lorry_half`: half the widest car that parks and half the widest lorry (the traffic
// catalogue's), the room every way keeps round its centre line.
// Half the widest car that parks in a bay (sedan to pickup) and half the widest lorry (a truck), from the catalogue.
float car_half_width(const TrafficParams &p);
float lorry_half_width(const TrafficParams &p);

void lot_access(const b40::Data &data, const b40::Body &b, const b40::LotFrame &f, float car_half, float lorry_half, LotAccess &out);

} // namespace city
