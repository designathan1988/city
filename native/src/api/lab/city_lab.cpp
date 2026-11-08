// CityLab (city_lab.h): the script's readings as the Diagnosis's plain types, and its answers as Dictionaries.
#include "api/lab/city_lab.h"

#include "api/lab/diagnosis.h"
#include "render/data_reader.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_float64_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <cmath>
#include <optional>
#include <vector>

using namespace godot;

namespace city {

namespace {

constexpr double MS_PER_S = 1000.0;

std::vector<double> samples_of(const Variant &value) {
	const PackedFloat64Array list = value;
	std::vector<double> out;
	out.reserve(static_cast<std::size_t>(list.size()));
	for (int64_t i = 0; i < list.size(); i++) out.push_back(list[i]);
	return out;
}

// The `doctor` rules through the strict reader (render/data_reader.h): a missing or mistyped key is pushed as an error and the
// diagnosis refused, never read as 0 (a missing budget_fps was a budget of 1000 / 0 ms).
bool rules_of(const Dictionary &d, lab::Rules &r) {
	PackedStringArray errors;
	DataReader in{ errors, "render_switches.json.doctor" };
	const double fps = in.num(d, "budget_fps");
	if (d.has("budget_fps") && fps <= 0) in.fail("budget_fps: not above 0");
	r.budget_ms = fps > 0 ? MS_PER_S / fps : 0;
	r.bound_share = in.num(d, "bound_share");
	r.high_ms = in.num(d, "high_ms");
	r.medium_ms = in.num(d, "medium_ms");
	r.low_ms = in.num(d, "low_ms");
	r.stage_min_ms = in.num(d, "stage_min_ms");
	r.stage_tail = in.num(d, "stage_tail");
	r.stutter_ms = in.num(d, "stutter_ms");
	r.many_hitches = in.num(d, "many_hitches");
	for (const String &e : errors) UtilityFunctions::push_error(e);
	return errors.is_empty();
}

std::optional<double> median_of_gpu(const Dictionary &frames) {
	const std::vector<double> gpu = samples_of(frames["gpu"]);
	if (gpu.empty()) return std::nullopt;
	return lab::median(gpu);
}

Variant maybe(std::optional<double> value) {
	return value ? Variant(*value) : Variant();
}

String bound_name(lab::Bound b) {
	switch (b) {
		case lab::Bound::Gpu: return "gpu";
		case lab::Bound::Cpu: return "cpu";
		case lab::Bound::Headroom: return "headroom";
		case lab::Bound::None: break;
	}
	return "none";
}

String severity_name(lab::Severity s) {
	switch (s) {
		case lab::Severity::High: return "high";
		case lab::Severity::Medium: return "medium";
		case lab::Severity::Low: return "low";
		case lab::Severity::Info: break;
	}
	return "info";
}

std::vector<lab::Stage> stages_of(const Dictionary &frames) {
	std::vector<lab::Stage> out;
	const Dictionary stages = frames["stages"];
	const Array ids = stages.keys();
	for (int64_t i = 0; i < ids.size(); i++) {
		const String id = ids[i];
		out.push_back({ id.utf8().get_data(), samples_of(stages[ids[i]]) });
	}
	return out;
}

Array findings_as_array(const std::vector<lab::Finding> &findings) {
	Array out;
	for (const lab::Finding &f : findings) {
		Dictionary row;
		row["id"] = String(f.id.c_str());
		row["ms"] = f.ms;
		row["tail_ms"] = f.tail_ms;
		row["share"] = f.share;
		row["severity"] = severity_name(f.severity);
		out.push_back(row);
	}
	return out;
}

lab::Side side_of(const Dictionary &frames) {
	lab::Side s;
	s.gpu_ms = median_of_gpu(frames);
	s.cpu_ms = lab::median(samples_of(frames["cpu"]));
	s.draws = lab::median(samples_of(frames["draws"]));
	s.primitives = lab::median(samples_of(frames["primitives"]));
	return s;
}

} // namespace

Dictionary CityLab::diagnose(const Dictionary &frames, const Dictionary &rules) const {
	lab::Rules r;
	if (!rules_of(rules, r)) return Dictionary();
	const std::vector<double> intervals = samples_of(frames["interval"]);
	lab::Times t;
	t.frame_ms = lab::median(intervals);
	t.cpu_ms = lab::median(samples_of(frames["cpu"]));
	t.gpu_ms = median_of_gpu(frames);
	const std::size_t hitches = lab::hitches_in(intervals, r);
	Dictionary out;
	out["bound"] = bound_name(lab::bound_of(t, r));
	out["frame_ms"] = t.frame_ms;
	out["fps"] = MS_PER_S / t.frame_ms;
	out["cpu_ms"] = t.cpu_ms;
	out["gpu_ms"] = maybe(t.gpu_ms);
	out["budget_ms"] = r.budget_ms;
	out["frames"] = static_cast<int64_t>(intervals.size());
	out["hitches"] = static_cast<int64_t>(hitches);
	out["many_hitches"] = static_cast<double>(hitches) > r.many_hitches;
	const std::vector<lab::Stage> stages = stages_of(frames);
	double stages_ms = 0;
	for (const lab::Stage &s : stages) stages_ms += lab::mean_of(s.samples);
	out["stages_ms"] = stages_ms; // every stage's mean, to hold against the same frames' mean
	out["gpu_mean_ms"] = lab::mean_of(samples_of(frames["gpu"]));
	out["findings"] = findings_as_array(lab::findings_of(stages, t.gpu_ms, r));
	return out;
}

Dictionary CityLab::compare(const Dictionary &before, const Dictionary &after, const Dictionary &again) const {
	const lab::Comparison c = lab::compare(side_of(before), side_of(after), side_of(again));
	Dictionary out;
	out["gpu_before"] = maybe(c.gpu_before);
	out["gpu_after"] = maybe(c.gpu_after);
	out["cpu_before"] = c.cpu_before;
	out["cpu_after"] = c.cpu_after;
	out["draws_before"] = c.draws_before;
	out["draws_after"] = c.draws_after;
	out["primitives_before"] = c.primitives_before;
	out["primitives_after"] = c.primitives_after;
	out["noise_gpu"] = c.noise_gpu;
	out["noise_cpu"] = c.noise_cpu;
	out["real"] = c.real;
	return out;
}

void CityLab::_bind_methods() {
	ClassDB::bind_method(D_METHOD("diagnose", "frames", "rules"), &CityLab::diagnose);
	ClassDB::bind_method(D_METHOD("compare", "before", "after", "again"), &CityLab::compare);
}

} // namespace city
