// The Diagnosis's rules, plain C++ (the 4.0's src/lab/doctor): what bounds the frame, how much a cost weighs, what the GPU's stages
// spend, and an A/B read against its own noise. No Godot here: api/lab/city_lab.cpp turns the readings into these and back.
#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace city::lab {

// project/data/game/render_switches.json `doctor`.
struct Rules {
	double budget_ms = 0; // a frame at the target rate
	double bound_share = 0; // share of the budget from which a side bounds the frame
	double high_ms = 0, medium_ms = 0, low_ms = 0; // from these a cost weighs that much
	double stage_min_ms = 0; // a stage under this is no finding
	double stage_tail = 0; // the share of a stage's frames its tail is read at
	double stutter_ms = 0; // a frame over this is a hitch
	double many_hitches = 0; // more than this many in a measure weigh more
};

enum class Bound { None, Gpu, Cpu, Headroom };
enum class Severity { Info, Low, Medium, High };

// The frame's medians; the CPU is NaN with no frame drawn, the GPU absent with no timer.
struct Times {
	double frame_ms = 0;
	double cpu_ms = 0;
	std::optional<double> gpu_ms;
};

struct Stage {
	std::string id;
	std::vector<double> samples; // ms of each frame
};

struct Finding {
	std::string id;
	double ms = 0;
	double tail_ms = 0;
	double share = 0; // of the frame's GPU time
	Severity severity = Severity::Info;
};

// One window of an A/B.
struct Side {
	std::optional<double> gpu_ms;
	double cpu_ms = 0, draws = 0, primitives = 0;
};

// Before (the mean of the two windows around the change) against after; the gap of those two windows is the noise.
struct Comparison {
	std::optional<double> gpu_before, gpu_after;
	double cpu_before = 0, cpu_after = 0;
	double draws_before = 0, draws_after = 0;
	double primitives_before = 0, primitives_after = 0;
	double noise_gpu = 0, noise_cpu = 0;
	bool real = false; // a side moved more than its noise
};

double median(std::vector<double> values); // NaN with nothing to read
double mean_of(const std::vector<double> &values); // 0 with nothing to read
Bound bound_of(const Times &t, const Rules &r);
Severity severity_of(double ms, const Rules &r);
// The stages that weigh, heaviest first.
std::vector<Finding> findings_of(const std::vector<Stage> &stages, std::optional<double> gpu_ms, const Rules &r);
std::size_t hitches_in(const std::vector<double> &intervals, const Rules &r);
Comparison compare(const Side &before, const Side &after, const Side &again);

} // namespace city::lab
