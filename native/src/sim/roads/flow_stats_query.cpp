// Roadcraft 4.0 src/sim/roads/flowStats.ts: the queries (they only read).
// Translated from Roadcraft 4.0 src/sim/roads/flowStats.ts by the converter; the owner is native/src/sim/roads.
#include "sim/roads/flow_stats.h"

namespace city {

namespace {

float aged_to(const Rate &r, int32_t second, const Tuning &tn);

// A rate brought up to the end of `second`: a factor `KEEP` for each second since it was kept, none counted.
float aged_to(const Rate &r, int32_t second, const Tuning &tn) {
	const int quiet = second - r.at;
	return (quiet <= 0 ? r.rate : r.rate * std::pow((1 - 1 / (tn.flow_trend_window / 3)), quiet));
}

} // namespace

void FlowStats::record(int64_t node, int64_t from) {
	auto &legs = this->counts[node];
	legs[from] = ([&] { const auto found_3 = [&]() -> const int * { auto it = legs.find(from); return it == legs.end() ? nullptr : &it->second; }(); return found_3 ? *found_3 : int(0); }()) + 1;
	this->counted.insert(node);
}

// The junctions counted at since the last call, added to `into` (the advisor's, `SimWorld.stepControlAdvisor`).
void FlowStats::take_counted(js::Set<int64_t> &into) {
	for (int64_t node : this->counted)
		into.insert(node);
	this->counted.clear();
}

void FlowStats::step(float dt, const Tuning &tn) {
	this->carry += dt;
	this->measured += dt;
	while (this->carry >= 1) {
		this->carry -= 1;
		this->second++;
		for (const auto &item_4 : this->counts) {
			int64_t node = item_4.first;
			const js::Map<int64_t, int> &counted = item_4.second;
			auto &legs = this->rates[node];
			for (const auto &item_5 : counted) {
				int64_t seg = item_5.first;
				int n = item_5.second;
				Rate *known = const_cast<Rate *>([&]() -> const Rate * { auto it = legs.find(seg); return it == legs.end() ? nullptr : &it->second; }());
				if (!known) {
					legs[seg] = Rate{(n * RATE) / (tn.flow_trend_window / 3), this->second};
					continue;
				}
				const float aged = aged_to(*known, this->second - 1, tn);
				known->rate = aged + (n * RATE - aged) / (tn.flow_trend_window / 3);
				known->at = this->second;
			}
		}
		this->counts.clear();
	}
}

// Vehicles an hour entering `node` from `seg`, as a trend.
float FlowStats::rate(int64_t node, int64_t seg, const Tuning &tn) {
	const Rate *known = [&]() -> const Rate * { const auto &from_6 = [&]() -> const js::Map<int64_t, Rate> * { auto it = this->rates.find(node); return it == this->rates.end() ? nullptr : &it->second; }(); if (!from_6) return nullptr; return [&]() -> const Rate * { auto it = (*from_6).find(seg); return it == (*from_6).end() ? nullptr : &it->second; }(); }();
	return (known ? aged_to(*known, this->second, tn) : 0);
}

// A junction's rates held as they are (`resume` lets them age again): DO NOT let them fall while nobody counts
// there, a signal came back as a priority junction.
void FlowStats::hold(int64_t node) {
	if (!(this->held.count(node) != 0))
		this->held[node] = this->second;
}

// A held junction's rates go on from where they were held, the seconds out not aged.
void FlowStats::resume(int64_t node) {
	std::optional<int> from = ([&]() -> const int * { auto it = this->held.find(node); return it == this->held.end() ? nullptr : &it->second; }() ? std::optional<int>(*[&]() -> const int * { auto it = this->held.find(node); return it == this->held.end() ? nullptr : &it->second; }()) : std::nullopt);
	if (from == std::nullopt)
		return;
	this->held.erase(node);
	const int out = this->second - (*from);
	if (out <= 0)
		return;
	if (auto it_7 = this->rates.find(node); it_7 != this->rates.end())
		for (auto &known : js::values_of(it_7->second)) {
			if (known.at <= (*from))
				known.at += out;
		}
}

// Forgets what a removed junction measured.
void FlowStats::keep_only(const js::Set<int64_t> &nodes) {
	for (int64_t node : std::vector<int64_t>(js::keys_of(this->rates))) {
		if (!(nodes.count(node) != 0))
			this->rates.erase(node);
	}
	for (int64_t node : std::vector<int64_t>(js::keys_of(this->held))) {
		if (!(nodes.count(node) != 0))
			this->held.erase(node);
	}
}

// These junctions' flows forgotten (gone with an edit).
void FlowStats::drop(const js::Set<int64_t> &nodes) {
	for (int64_t node : nodes) {
		this->rates.erase(node);
		this->held.erase(node);
	}
}

FlowStats::FlowStats() {
	this->counts = js::Map<int64_t, js::Map<int64_t, int>>{};
	this->rates = js::Map<int64_t, js::Map<int64_t, Rate>>{};
	this->carry = 0;
	this->second = 0;
	this->measured = 0;
	this->counted = js::Set<int64_t>{};
	this->held = js::Map<int64_t, int>{};
}

} // namespace city
