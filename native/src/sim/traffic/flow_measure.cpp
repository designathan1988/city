// FlowMeasure (flow_measure.h): the per-leg entering rates of the junctions, counted each second and aged when read.
#include "sim/traffic/flow_measure.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {

constexpr float SECONDS_PER_HOUR = 3600; // a count of one second as vehicles an hour

// The weighted mean's time constant, in seconds: a third of the trend window, so the window holds 95% of what it measured.
float time_constant(const Tuning &tn) { return tn.flow_trend_window / 3; }

// A rate brought to the end of `second`: the mean keeps 1 - 1 / tau of itself each second nobody entered.
float aged_to(const FlowMeasure::Leg &leg, int32_t second, const Tuning &tn) {
	const int quiet = second - leg.at;
	return quiet <= 0 ? leg.rate : leg.rate * std::pow(1 - 1 / time_constant(tn), quiet);
}

FlowMeasure::Leg *leg_of(FlowMeasure::Junction &j, int seg) {
	for (FlowMeasure::Leg &leg : j.legs)
		if (leg.seg == seg) return &leg;
	return nullptr;
}

} // namespace

void FlowMeasure::record(int node, int seg) {
	Junction &j = junctions_[node];
	Leg *leg = leg_of(j, seg);
	if (!leg) {
		j.legs.push_back(Leg{ seg });
		leg = &j.legs.back();
	}
	leg->entered += 1;
	leg->counting = true;
	if (!j.counting) {
		j.counting = true;
		counted_.push_back(node);
	}
	if (std::find(counted_since_.begin(), counted_since_.end(), node) == counted_since_.end()) counted_since_.push_back(node);
}

std::vector<int> FlowMeasure::take_counted() {
	std::vector<int> out;
	out.swap(counted_since_);
	return out;
}

void FlowMeasure::step(float dt, const Tuning &tn) {
	carry_ += dt;
	measured += dt;
	while (carry_ >= 1) {
		carry_ -= 1;
		second_++;
		for (int node : counted_) {
			Junction &j = junctions_[node];
			for (Leg &leg : j.legs) {
				if (!leg.counting) continue;
				const float n = leg.entered;
				if (!leg.known) {
					leg.rate = (n * SECONDS_PER_HOUR) / time_constant(tn);
					leg.known = true;
				} else {
					const float aged = aged_to(leg, second_ - 1, tn);
					leg.rate = aged + (n * SECONDS_PER_HOUR - aged) / time_constant(tn);
				}
				leg.at = second_;
				leg.entered = 0;
				leg.counting = false;
			}
			j.counting = false;
		}
		counted_.clear();
	}
}

float FlowMeasure::rate(int node, int seg, const Tuning &tn) const {
	const auto it = junctions_.find(node);
	if (it == junctions_.end()) return 0;
	for (const Leg &leg : it->second.legs)
		if (leg.seg == seg && leg.known) return aged_to(leg, second_, tn);
	return 0;
}

void FlowMeasure::hold(int node) {
	Junction &j = junctions_[node];
	if (j.held_since < 0) j.held_since = second_;
}

void FlowMeasure::resume(int node) {
	const auto it = junctions_.find(node);
	if (it == junctions_.end() || it->second.held_since < 0) return;
	Junction &j = it->second;
	const int32_t from = j.held_since;
	j.held_since = -1;
	const int32_t out = second_ - from;
	if (out <= 0) return;
	for (Leg &leg : j.legs)
		if (leg.known && leg.at <= from) leg.at += out;
}

} // namespace city
