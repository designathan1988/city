// The money of road edits (see road_economy.h). The value of an edit comes from its journal entry, which already holds
// every road it added or removed: nothing keeps a sum of the map, so undo and redo have nothing to correct.
#include "sim/economy/road_economy.h"

#include "sim/commands/commands.h"
#include "sim/commands/edit_journal.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr double MIN_CHARGE = 1; // an edit worth less than this moves no money
constexpr double ANY_HEIGHT = std::numeric_limits<double>::infinity();
constexpr double ANY_BALANCE = std::numeric_limits<double>::infinity();

// How a recorded road stands: its structure and the heights authored at its ends.
RoadBuild built(const RoadNetwork &net, const RoadSegment &g) {
	return { structure_of(g.flags), net.nodes[size_t(g.a)].lift, net.nodes[size_t(g.b)].lift };
}

double recorded_value(const CityState &s, const RoadSegment &g, int profile) {
	return build_value(s, profile, g.len, built(s.roads, g));
}
} // namespace

BuildLengths build_lengths(const Tuning &tn, const RoadBuild &b, double len) {
	BuildLengths out;
	if (b.structure == Structure::TUNNEL) {
		out.tunnel = len;
		return out;
	}
	if (b.structure != Structure::GROUND) {
		out.bridge = len;
		return out;
	}
	double fill = tn.economy_fill_from_m, piers = tn.economy_on_piers_from_m;
	// The share of the road whose height lies in [lo, hi).
	auto share = [&](double lo, double hi) {
		if (b.height_a == b.height_b) return b.height_a >= lo && b.height_a < hi ? 1.0 : 0.0;
		double t0 = (lo - b.height_a) / (b.height_b - b.height_a), t1 = (hi - b.height_a) / (b.height_b - b.height_a);
		return std::max(0.0, std::min(1.0, std::max(t0, t1)) - std::max(0.0, std::min(t0, t1)));
	};
	out.trench = len * share(-ANY_HEIGHT, -fill);
	out.ground = len * share(-fill, fill);
	out.embankment = len * share(fill, piers);
	out.bridge = len * share(piers, ANY_HEIGHT);
	return out;
}

double build_value(const CityState &s, int profile, double len, const RoadBuild &b) {
	const StructureCost &k = s.catalog.structure;
	BuildLengths l = build_lengths(s.catalog.tune, b, len);
	return s.catalog.profile(profile).cost_per_m * (l.ground * k.ground + l.embankment * k.embankment + l.trench * k.trench +
			l.bridge * k.bridge + l.tunnel * k.tunnel);
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
