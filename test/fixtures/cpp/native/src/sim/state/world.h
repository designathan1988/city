// Fixture: the state root (the single source of truth). Each member belongs to the subsystem of its type.
#pragma once

#include "sim/fleet.h"

namespace city {

// A value type held by nobody in the state tree: whoever holds an instance owns it.
struct Tally {
	int count = 0;
};

// A channel every producer writes, and the value it carries.
struct Letter {
	int code = 0;
};
struct Mailbox {
	Letter slot;
};

struct World {
	Fleet fleet;
	Mailbox mail;
	double money = 0;
};

} // namespace city
