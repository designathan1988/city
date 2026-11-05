// Fixture: interface code that writes state owned by other subsystems, and a value it holds itself.
#include "sim/state/world.h"

namespace {
// Another class named Car, in another file: a different class.
struct Car {
	int seats = 0;
};
} // namespace

namespace city {

void reset_from_panel(World &world, Tally &tally) {
	world.fleet.total = 0;
	world.money = 0;
	tally.count = 0;
	world.mail.slot.code = 1;
}

} // namespace city
