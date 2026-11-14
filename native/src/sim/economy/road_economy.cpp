// The money of road edits (see road_economy.h). The value of an edit comes from its journal entry, which already holds
// every road it added or removed: nothing keeps a sum of the map, so undo and redo have nothing to correct.
#include "sim/economy/road_economy.h"

#include "sim/commands/commands.h"
#include "sim/commands/edit_journal.h"
#include "sim/commands/road_grade_sync.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr double MIN_CHARGE = 1; // an edit worth less than this moves no money
constexpr double ANY_HEIGHT = std::numeric_limits<double>::infinity();
constexpr double ANY_BALANCE = std::numeric_limits<double>::infinity();
constexpr double HALF = 0.5;
constexpr int BANDS = 5; // the ways a road at grade is built, by its authored height: bored, cut, on the ground, filled, on piers

// How a recorded road stands: its structure and the heights authored at its ends.
RoadBuild built(const RoadNetwork &net, const RoadSegment &g) {
	return { structure_of(g.flags), net.nodes[size_t(g.a)].lift, net.nodes[size_t(g.b)].lift };
}

// What a recorded road is worth as a road of `profile`: the class's cost a metre by the worth it was built with (the metres
// of plain ground road it stands for). A record with none (a generated road, an older save's) is worth its authored
// heights' bands, as it always was: nothing walks the map to fill them in.
double recorded_value(const CityState &s, const RoadSegment &g, int profile) {
	if (g.worth > 0) return s.catalog.profile(profile).cost_per_m * double(g.worth);
	return build_value(s, profile, g.len, built(s.roads, g));
}
} // namespace

void BuildLengths::add(const BuildLengths &o) {
	ground += o.ground;
	embankment += o.embankment;
	trench += o.trench;
	bridge += o.bridge;
	tunnel += o.tunnel;
}

// A road built as a structure is that structure all along. At grade its authored height runs straight from one end to the
// other, and each band of height takes the share of the length that lies in it: under the cover a bore needs it is bored
// (the depth the render bores at: road_structures.cpp), down to the fill's depth a cutting, within it the ground, up to
// the piers' height an embankment, over it a bridge. A height on a band's lower edge is of that band.
BuildLengths build_lengths(const Tuning &tn, const RoadBuild &b, double len) {
	BuildLengths out;
	if (b.structure != Structure::GROUND) {
		(b.structure == Structure::TUNNEL ? out.tunnel : out.bridge) = len;
		return out;
	}
	const double bore = double(tn.grade_tunnel_headroom_m) + tn.grade_tunnel_arch_m + tn.structure_bore_over_m;
	const double tops[BANDS] = { -bore, -double(tn.economy_fill_from_m), tn.economy_fill_from_m, tn.economy_on_piers_from_m, ANY_HEIGHT };
	double *const bands[BANDS] = { &out.tunnel, &out.trench, &out.ground, &out.embankment, &out.bridge };
	const double low = std::min(b.height_a, b.height_b), high = std::max(b.height_a, b.height_b);
	double from = low; // the bands walked upwards
	for (int k = 0; k < BANDS; k++) {
		if (high == low) {
			if (low >= tops[k]) continue;
			*bands[k] = len;
			break;
		}
		const double to = std::min(high, tops[k]);
		if (to <= from) continue;
		*bands[k] = len * (to - from) / (high - low);
		from = to;
	}
	return out;
}

BuildWay way_at(const Tuning &tn, Structure structure, float over) {
	if (is_sunken(structure)) return BuildWay::TUNNEL;
	if (is_raised(structure)) return BuildWay::BRIDGE;
	if (over < -(tn.grade_tunnel_headroom_m + tn.grade_tunnel_arch_m + tn.structure_bore_over_m)) return BuildWay::TUNNEL;
	if (over > tn.economy_on_piers_from_m) return BuildWay::BRIDGE;
	if (over >= tn.economy_fill_from_m) return BuildWay::EMBANKMENT;
	return over <= -tn.economy_fill_from_m ? BuildWay::TRENCH : BuildWay::GROUND;
}

BuildLengths lengths_along(const Tuning &tn, Structure structure, const AloneStations &st) {
	BuildLengths out;
	double *const ways[] = { &out.ground, &out.embankment, &out.trench, &out.bridge, &out.tunnel }; // by BuildWay
	const size_t last = st.deck.empty() ? 0 : st.deck.size() - 1;
	for (size_t i = 0; i < st.deck.size(); i++) {
		const double share = i == 0 || i == last ? HALF : 1.0; // an end station stands for half a step
		*ways[size_t(way_at(tn, structure, st.deck[i] - st.ground[i]))] += double(st.step) * share;
	}
	return out;
}

double worth_of(const CityState &s, const BuildLengths &l) {
	const StructureCost &k = s.catalog.structure;
	return l.ground * k.ground + l.embankment * k.embankment + l.trench * k.trench + l.bridge * k.bridge + l.tunnel * k.tunnel;
}

BuiltAlone built_alone(const CityState &s, const Quad &q, const std::vector<double> &table, int profile, uint16_t flags, float stand_a, float stand_b,
		bool authored) {
	RoadAlone road;
	road.q = &q;
	road.table = &table;
	road.half = s.catalog.profile(profile).half() + CASING_BAND_M;
	road.structure = structure_of(flags);
	road.authored = authored;
	road.stand_a = stand_a;
	road.stand_b = stand_b;
	BuiltAlone out;
	out.stations = solve_alone(road, road_ground(s), s.catalog.tune);
	out.lengths = lengths_along(s.catalog.tune, road.structure, out.stations);
	out.worth = worth_of(s, out.lengths);
	return out;
}

void place_stations(const Tuning &tn, const Quad &q, const std::vector<double> &table, Structure structure, const AloneStations &st,
		std::vector<DraftStation> &out) {
	for (size_t i = 0; i < st.deck.size(); i++) {
		double x, z;
		q.at(t_at_length(table, double(st.step) * double(i)), x, z);
		out.push_back({ float(x), float(z), st.deck[i], st.ground[i], way_at(tn, structure, st.deck[i] - st.ground[i]), structure });
	}
}

double worth_recorded(const CityState &s, int seg) {
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	return g.worth > 0 ? double(g.worth) : worth_of(s, build_lengths(s.catalog.tune, built(s.roads, g), g.len));
}

double worth_solved(const CityState &s, int seg) {
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	if (!s.roads.grade.has(seg)) return g.worth;
	const GradeProfile &p = s.roads.grade.prof[size_t(seg)];
	const GroundAt ground = road_ground(s);
	AloneStations st;
	st.step = p.step;
	st.deck = p.h;
	st.ground.resize(p.h.size());
	for (size_t i = 0; i < p.h.size(); i++) {
		double x, z;
		g.q.at(t_at_length(g.table, double(p.step) * double(i)), x, z);
		st.ground[i] = ground(float(x), float(z));
	}
	return worth_of(s, lengths_along(s.catalog.tune, p.structure, st));
}

double build_value(const CityState &s, int profile, double len, const RoadBuild &b) {
	return s.catalog.profile(profile).cost_per_m * worth_of(s, build_lengths(s.catalog.tune, b, len));
}

double road_value(const CityState &s, int profile, double len) { return build_value(s, profile, len, RoadBuild{}); }

RoadCharge charge_for(double before, double after, double balance, double refund_share) {
	double delta = after - before;
	if (std::fabs(delta) < MIN_CHARGE) return { 0, balance, true };
	if (delta > 0) {
		double amount = std::round(delta);
		return { amount, balance - amount, amount <= balance };
	}
	double refund = std::round(-delta * refund_share);
	return { -refund, balance + refund, true };
}

RoadCharge charge_of(const CityState &s, double value_change) {
	return charge_for(0, value_change, s.unlimited_money ? ANY_BALANCE : s.money, s.catalog.tune.economy_refund_share);
}

double edit_value_change(const CityState &s, size_t from) {
	const std::vector<EditOp> &ops = s.history.open.ops;
	double change = 0;
	for (size_t i = from; i < ops.size(); i++) {
		const EditOp &op = ops[i];
		if (op.kind == OpKind::ADD_SEG) change += recorded_value(s, op.get<RoadSegment>(), op.get<RoadSegment>().profile);
		else if (op.kind == OpKind::REM_SEG) change -= recorded_value(s, op.get<RoadSegment>(), op.get<RoadSegment>().profile);
		else if (op.kind == OpKind::PROFILE) {
			const ProfileChange &c = op.get<ProfileChange>();
			change += recorded_value(s, c.seg, c.to) - recorded_value(s, c.seg, c.from);
		} else if (op.kind == OpKind::SEG_WORTH) {
			// a standing road built otherwise: the difference at its class's cost; one that had no worth recorded is only
			// given the worth it already had (worth_recorded): no money in that
			const LiftChange &c = op.get<LiftChange>();
			if (c.from > 0) change += s.catalog.profile(s.roads.segs[size_t(op.id)].profile).cost_per_m * double(c.to - c.from);
		}
	}
	return change;
}

bool settle_road_edit(CityState &s, EditResult &r) { return settle_road_edit(s, r, s.history.command_start()); }

bool settle_road_edit(CityState &s, EditResult &r, size_t from) {
	RoadCharge charge = charge_of(s, edit_value_change(s, from));
	r.money = s.unlimited_money ? 0 : charge.amount;
	if (!charge.affordable) {
		r.error = REFUSED_FUNDS;
		return false;
	}
	j_money(s, -charge.amount);
	return true;
}

} // namespace city
