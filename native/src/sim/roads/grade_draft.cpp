// A road solved by itself (road_grade.h solve_alone), on the parts the network's solve is made of (grade_parts.h): its
// frame along its curve, the ground read across its casing, its design line, one span between the heights its two ends
// stand at. What the road tool's draft shows and what a laid road is priced by come from here: the cost is the road's
// own stations, never a solve of the map.
#include "sim/roads/grade_parts.h"

#include "sim/catalog/catalog.h"
#include "sim/roads/grade_line.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double LEAST_LEN_M = 1e-3;
constexpr int FEWEST_STATIONS = 2;
} // namespace

AloneStations solve_alone(const RoadAlone &road, const GroundAt &ground_at, const Tuning &tn) {
	const GradeTune t = grade_tune(tn);
	const std::vector<double> &table = *road.table;
	GradeProfile p;
	p.len = float(std::max(LEAST_LEN_M, table.empty() ? 0.0 : table.back()));
	const int count = std::max(FEWEST_STATIONS, std::min(STATIONS_MOST, int(std::ceil(p.len / STATION_M)) + 1));
	p.step = p.len / float(count - 1);
	p.half = road.half;
	p.structure = road.structure;
	p.manual = road.authored;
	p.h.assign(size_t(count), 0.f);
	p.valid = true;
	GroundLines lines;
	ground_under(*road.q, table, p, ground_at, t, lines);
	lines.line = lines.mean;
	mean_over(lines.line, p.step, t.line_reach);
	within_grade(lines.line, p.step, t.ground);
	if (is_raised(p.structure)) {
		span_raised(p, lines, road.stand_a, road.stand_b, RaisedEnds{}, t);
	} else if (is_sunken(p.structure)) {
		// a bore's depth: under the lowest of its ground and of where its two ends stand
		const float lowest = std::min({ *std::min_element(lines.top.begin(), lines.top.end()), road.stand_a, road.stand_b });
		span_sunken(p, road.stand_a, road.stand_b, lowest + t.of(p.structure).clearance, t);
	} else {
		span_at_grade(p, lines, road.stand_a, road.stand_b, p.manual ? t.ramp : t.ground, t);
	}
	AloneStations out;
	out.step = p.step;
	out.deck = p.h;
	out.ground.resize(size_t(count));
	for (int i = 0; i < count; i++) {
		double x, z;
		road.q->at(t_at_length(table, double(p.step) * double(i)), x, z);
		out.ground[size_t(i)] = ground_at(float(x), float(z));
	}
	return out;
}

} // namespace city
