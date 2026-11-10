// Roadcraft 4.0 src/sim/roads/flowStats.ts: types and constants.
// Translated from Roadcraft 4.0 src/sim/roads/flowStats.ts by the converter; the owner is native/src/sim/roads.
#pragma once

#include "core/js_runtime.h"
#include "sim/catalog/catalog.h"
#include "sim/roads/road_grade.h"

#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace city {

// The 3600 the source writes here (Roadcraft 4.0 src/sim/roads/flowStats.ts:47)
constexpr int RATE = 3600;

struct FlowStats;
struct Rate;
// (Roadcraft 4.0 src/sim/roads/flowStats.ts:12)
struct FlowStats : std::enable_shared_from_this<FlowStats> {
	js::Map<int64_t, js::Map<int64_t, float>> counts;
	js::Map<int64_t, js::Map<int64_t, Rate>> rates;
	float carry = 0;
	int32_t second = 0; // Seconds aged so far: the clock the rates are kept against.
	float measured = 0; // Seconds of traffic measured since the town was laid out (a trend needs `trendWindow`).
	js::Set<int64_t> counted;
	js::Map<int64_t, int> held; // Junctions whose rates stand still (out of the micro window: nothing is counted there), by the second they were held at.
	void record(int64_t node, int64_t from);
	void take_counted(js::Set<int64_t> &into);
	void step(float dt, const Tuning &tn);
	float rate(int64_t node, int64_t seg, const Tuning &tn);
	void hold(int64_t node);
	void resume(int64_t node);
	void keep_only(const js::Set<int64_t> &nodes);
	void drop(const js::Set<int64_t> &nodes);
	FlowStats();
};

// A leg's rate as it stood at the end of second `at`.
// (Roadcraft 4.0 src/sim/roads/flowStats.ts:10)
struct Rate {
	float rate = 0;
	int32_t at = 0;
};

} // namespace city
