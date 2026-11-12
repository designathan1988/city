// A lot car's phase (parking_state.h CarPhase), its one setter, its one maker and the moves its visit makes: called to a free bay it comes
// along the lanes to its gate, leaves the road and drives in, stands in its bay, and sent out drives to the lane again. It is made either
// called (coming) or out of a car that stood in its bay as a row (parked, sent out at once). A move or a making its visit never has is a
// mistake of its caller: the tests' DLL stops on it (core/checks.h), the game keeps the car where it was.
#include "core/checks.h"
#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

namespace {

constexpr int PHASES = int(CarPhase::LEAVING) + 1;

constexpr uint8_t phase_bit(CarPhase p) { return uint8_t(1u << unsigned(p)); }

// CAR_PHASES[from]: the phases a lot car in `from` may enter (each may stay where it is). It leaves the lots' list from any of them (given
// up on its way, its bay gone, a row of its bay, on the lane again): no phase follows that.
constexpr uint8_t CAR_PHASES[PHASES] = {
		// COMING: stopped before its gate with the stretch it turns across clear, it leaves the road
		uint8_t(phase_bit(CarPhase::COMING) | phase_bit(CarPhase::PARKING)),
		// PARKING: its way in done, it stands in its bay
		uint8_t(phase_bit(CarPhase::PARKING) | phase_bit(CarPhase::PARKED)),
		// PARKED: sent out, it starts on its way to the lane
		uint8_t(phase_bit(CarPhase::PARKED) | phase_bit(CarPhase::LEAVING)),
		// LEAVING: on the lane past its gate it is a car of the traffic again, no lot car
		uint8_t(phase_bit(CarPhase::LEAVING)),
};

// The phases a lot car is made in: called to a bay, or taken back out of the row it stood in.
constexpr uint8_t CAR_BIRTHS = uint8_t(phase_bit(CarPhase::COMING) | phase_bit(CarPhase::PARKED));

} // namespace

bool enter_car_phase(LotCar &car, CarPhase next) {
	if (!(CAR_PHASES[int(car.phase)] & phase_bit(next))) {
		if (CHECKS) city_stop("a lot car's move its visit never makes (car_phase.cpp CAR_PHASES)");
		return false;
	}
	car.phase = next;
	return true;
}

LotCar new_lot_car(int id, int bay, CarPhase born) {
	if (CHECKS && !(CAR_BIRTHS & phase_bit(born))) city_stop("a lot car made in a phase its visit never starts in (car_phase.cpp CAR_BIRTHS)");
	return LotCar(id, bay, born);
}

} // namespace traffic
} // namespace city
