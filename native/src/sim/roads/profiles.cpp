// derive() of Roadcraft src/roads/profiles.ts: strip edges, asphalt widths, painted lines, lanes, footways, price.
#include "sim/roads/profiles.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {

LineKind centre_kind(const std::string &centre) {
	if (centre == "none") return LineKind::NONE;
	if (centre == "solid") return LineKind::SOLID;
	if (centre == "dashed") return LineKind::DASHED;
	return LineKind::DOUBLE;
}

void derive_lines(Profile &p) {
	p.lines.clear();
	if (p.rail) return;
	for (size_t i = 0; i + 1 < p.edges.size() && int(p.lines.size()) < MAX_PROFILE_LINES; i++) {
		const Strip &a = p.edges[i].strip, &b = p.edges[i + 1].strip;
		float at = p.edges[i].u1;
		bool la = a.kind == StripKind::LANE, lb = b.kind == StripKind::LANE;
		if (la && lb) {
			if (a.dir != b.dir) {
				if (centre_kind(p.centre) != LineKind::NONE) p.lines.push_back({ at, false, centre_kind(p.centre), LineColor::YELLOW, true });
			} else {
				bool solid = (a.dir == 1 ? b.solid : a.solid) || a.bus != b.bus;
				p.lines.push_back({ at, a.bus != b.bus, solid ? LineKind::SOLID : LineKind::DASHED, LineColor::WHITE });
			}
		} else if (a.kind == StripKind::MEDIAN && a.flush && i > 0) {
			p.lines.push_back({ p.edges[i].u0, false, LineKind::SOLID, LineColor::YELLOW });
			p.lines.push_back({ at, false, LineKind::SOLID, LineColor::YELLOW });
		}
	}
}

bool is_median(const Profile &p, size_t i) {
	auto median = [&](size_t j) { return j < p.edges.size() && p.edges[j].strip.kind == StripKind::MEDIAN; };
	return median(i + 1) || (i > 0 && median(i - 1));
}
} // namespace

bool is_asphalt(StripKind k) {
	return k == StripKind::LANE || k == StripKind::PARKING || k == StripKind::BIKE || k == StripKind::SHOULDER;
}

void derive_profile(Profile &p, const RoadPrices &prices) {
	float total = 0;
	for (const Strip &s : p.strips) total += s.width;
	float u = -total / 2;
	p.edges.clear();
	p.rail = false;
	for (const Strip &s : p.strips) {
		p.edges.push_back({ u, u + s.width, s });
		u += s.width;
		p.rail = p.rail || s.rail;
	}
	const StripEdge *first_asph = nullptr, *last_asph = nullptr;
	for (const StripEdge &e : p.edges) {
		if (is_asphalt(e.strip.kind) || (e.strip.flush && e.strip.kind != StripKind::SIDEWALK)) {
			if (!first_asph) first_asph = &e;
			last_asph = &e;
		}
	}
	p.right = p.left = total / 2;
	p.asph_right = first_asph ? -first_asph->u0 : 0;
	p.asph_left = last_asph ? last_asph->u1 : 0;
	derive_lines(p);
	p.fwd.clear();
	p.back.clear();
	for (const StripEdge &e : p.edges) {
		if (e.strip.kind != StripKind::LANE) continue;
		LaneInfo li{ (e.u0 + e.u1) / 2, e.u1 - e.u0, e.strip.bus, e.strip.rail, e.strip.turns };
		(e.strip.dir == -1 ? p.back : p.fwd).push_back(li);
	}
	std::reverse(p.back.begin(), p.back.end()); // in the b -> a direction, right is the larger u
	const StripEdge &r = p.edges.front(), &l = p.edges.back();
	p.has_walk_right = r.strip.kind == StripKind::SIDEWALK;
	p.has_walk_left = l.strip.kind == StripKind::SIDEWALK;
	p.walk_right = (r.u0 + r.u1) / 2;
	p.walk_left = (l.u0 + l.u1) / 2;
	float carriage = 0, median = 0, foot = 0;
	for (size_t i = 0; i < p.edges.size(); i++) {
		float w = p.edges[i].u1 - p.edges[i].u0;
		StripKind k = p.edges[i].strip.kind;
		bool by_median = k == StripKind::CURB && is_median(p, i); // a median's kerbs are priced with it
		if (k == StripKind::MEDIAN || k == StripKind::BARRIER || by_median) median += w;
		else if (k == StripKind::SIDEWALK || k == StripKind::CURB) foot += w;
		else carriage += w;
	}
	p.cost_per_m = std::round(carriage * prices.carriageway + median * prices.median + foot * prices.footway);
}

} // namespace city
