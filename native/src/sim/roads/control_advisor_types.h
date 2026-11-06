// Roadcraft 4.0 src/sim/roads/controlAdvisor.ts: types and constants.
// Translated from Roadcraft 4.0 src/sim/roads/controlAdvisor.ts by the converter; the owner is native/src/sim/roads.
#pragma once

#include "sim/roads/control_advisor_enums.h"
#include "core/js_runtime.h"
#include "sim/catalog/catalog.h"
#include "sim/roads/road_grade.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <vector>

namespace city {

struct ControlAdvisor;
// (Roadcraft 4.0 src/sim/roads/controlAdvisor.ts:24)
struct ControlAdvisor : std::enable_shared_from_this<ControlAdvisor> {
	js::Map<int64_t, AutoControl> choices;
	js::Map<int64_t, float> changed_at;
	float since = 0;
	static inline const int every = 10;
	std::optional<AutoControl> choice(int64_t node);
	bool tick(float dt);
	bool evaluate(int64_t node, float major, float minor_max, float minor_total, AutoControl initial, float now, const Tuning &tn);
	void forget(const js::Set<int64_t> &nodes);
	void drop(const js::Set<int64_t> &nodes);
	ControlAdvisor();
};

} // namespace city
