// Roadcraft 4.0 src/sim/roads/controlAdvisor.ts: what other owners call.
// Translated from Roadcraft 4.0 src/sim/roads/controlAdvisor.ts by the converter; the owner is native/src/sim/roads.
#pragma once

#include "sim/roads/control_advisor_types.h"

namespace city {

// Vehicles an hour into a junction below which `warrantedControl` is 'priority' whatever it has now (the least
// major flow of a step up, with the hysteresis).
float quiet_flow(const Tuning &tn);

// The control a junction's flows warrant, given what it has now (hysteresis).
AutoControl warranted_control(float major, float minor_max, float minor_total, AutoControl current, const Tuning &tn);

} // namespace city
