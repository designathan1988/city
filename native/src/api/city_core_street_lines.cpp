// The streets' lines as one network, counted for the tests (CityStreet::furniture_census, street_works_test's lines): the line
// poles (the rows' own and the junctions' corner poles), every span (a row's, and a node's: a joint's, a corner pole's, across a
// junction, sim/roads/street_lines.h), those strung to a pole that does not stand, the longest and the shortest, the junctions
// whose lines in line are not strung across, the joints whose next row is not strung to the joint's pole, two line poles standing closer than poles.min_gap_m with no span between them, and a
// line pole whose footprint comes within items.gap_m of a signal post's.
#include "api/city_street.h"

#include "sim/roads/street_lines.h"
#include "sim/state/city_state.h"
#include "sim/street/footway.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <unordered_map>

using namespace godot;

namespace city {

namespace {

constexpr float HALF = 0.5f;

using PoleId = std::pair<int, int>;

// A line pole's key in one order: a corner pole by its node and corner, a row's by its segment and station.
PoleId id_of(const LinePole &p) { return p.corner >= 0 ? PoleId(-1 - p.node, p.corner) : PoleId(p.seg, p.station); }

struct Pole {
	LinePole key;
	double x = 0, z = 0;
};

struct Spans {
	int count = 0, across = 0, broken = 0;
	double longest = 0, shortest = 0, across_shortest = 0;
	Pole short_a, short_b; // the two poles of the shortest span along a street
	std::set<std::pair<PoleId, PoleId>> joined;
	void add(const Pole &a, const Pole &b, bool over) {
		const double len = std::hypot(a.x - b.x, a.z - b.z);
		joined.insert({ std::min(id_of(a.key), id_of(b.key)), std::max(id_of(a.key), id_of(b.key)) });
		if (over) {
			across_shortest = across++ == 0 ? len : std::min(across_shortest, len);
			return;
		}
		if (count++ == 0 || len < shortest) {
			shortest = len;
			short_a = a;
			short_b = b;
		}
		longest = std::max(longest, len);
	}
};

// Line poles standing closer than `gap` with no span between them, and line poles closer than `reach` to a signal post (their
// footprints within items.gap_m), looked for cell by cell.
std::pair<int, int> close_and_touching(const std::map<PoleId, Pole> &poles, const std::vector<std::pair<double, double>> &posts, const Spans &spans,
		double gap, double reach) {
	const double cell = std::max(gap, reach);
	const auto cell_of = [&](double x, double z) { return std::pair<int, int>(int(std::floor(x / cell)), int(std::floor(z / cell))); };
	std::map<std::pair<int, int>, std::vector<const std::pair<const PoleId, Pole> *>> grid;
	std::map<std::pair<int, int>, std::vector<std::pair<double, double>>> post_grid;
	for (const auto &e : poles) grid[cell_of(e.second.x, e.second.z)].push_back(&e);
	for (const auto &p : posts) post_grid[cell_of(p.first, p.second)].push_back(p);
	int close = 0, touch = 0;
	for (const auto &e : poles) {
		const auto [cx, cz] = cell_of(e.second.x, e.second.z);
		for (int ix = cx - 1; ix <= cx + 1; ix++)
			for (int iz = cz - 1; iz <= cz + 1; iz++) {
				for (const auto *o : grid[{ ix, iz }])
					close += e.first < o->first && std::hypot(e.second.x - o->second.x, e.second.z - o->second.z) < gap && !spans.joined.count({ e.first, o->first }) ? 1 : 0;
				for (const auto &[px, pz] : post_grid[{ ix, iz }]) touch += std::hypot(e.second.x - px, e.second.z - pz) < reach ? 1 : 0;
			}
	}
	return { close, touch };
}

} // namespace

void CityStreet::line_census(Dictionary &d) const {
	const CityState &s = *state_;
	const StreetFurnitureDef &def = s.catalog.furniture;
	std::unordered_map<int, std::vector<FurnitureSpot>> spots; // every live segment's pieces, by segment
	std::map<PoleId, Pole> poles;
	std::vector<std::pair<double, double>> posts; // where the signal posts stand
	int corners = 0;
	for (int sid = 0; sid < int(s.roads.segs.size()); sid++) {
		if (!s.roads.segs[size_t(sid)].alive) continue;
		std::vector<FurnitureSpot> &list = spots[sid];
		traffic::street_furniture_of(s, sid, FURNITURE_ALL, list);
		for (const FurnitureSpot &p : list) {
			double x, z, y;
			footway_point(s.roads, sid, p.v, p.u, x, z, y);
			if (p.kind == FurnitureKind::SIGNAL) posts.emplace_back(x, z);
			if (p.kind != FurnitureKind::LAMP || p.row != 0) continue;
			corners += p.corner >= 0 ? 1 : 0;
			const Pole pole{ p.corner >= 0 ? LinePole{ sid, -1, p.node, p.corner } : LinePole{ sid, p.station, -1, -1 }, x, z };
			poles[id_of(pole.key)] = pole;
		}
	}
	Spans spans;
	const auto span = [&](const LinePole &a, const LinePole &b, bool over) {
		const auto pa = poles.find(id_of(a)), pb = poles.find(id_of(b));
		if (pa == poles.end() || pb == poles.end()) spans.broken++;
		else spans.add(pa->second, pb->second, over);
	};
	for (const auto &[sid, list] : spots) // each row's own spans
		for (const FurnitureSpot &p : list)
			if (p.kind == FurnitureKind::LAMP && p.row == 0 && p.corner < 0 && p.strung >= 0) span(LinePole{ sid, p.strung, -1, -1 }, LinePole{ sid, p.station, -1, -1 }, false);
	const FirstStation first = [&](int seg, int end) {
		int best = -1;
		for (const FurnitureSpot &p : spots[seg])
			if (p.kind == FurnitureKind::LAMP && p.row == 0 && p.corner < 0 && p.station >= 0 && (best < 0 || (end == 0 ? p.station < best : p.station > best))) best = p.station;
		return best;
	};
	int unjoined = 0, joints_unjoined = 0;
	std::vector<LineSpan> at;
	for (int n = 0; n < int(s.roads.nodes.size()); n++) {
		if (!s.roads.nodes[size_t(n)].alive) continue;
		at.clear();
		node_line_spans(s.roads, n, first, at);
		bool across = false;
		for (const LineSpan &sp : at) {
			span(sp.a, sp.b, sp.across);
			across = across || sp.across;
		}
		// a line running on through the node on the other arm's pole (a joint) whose own first pole no span of the node reaches
		for (int sid : s.roads.nodes[size_t(n)].segs) {
			const RoadSegment &g = s.roads.segs[size_t(sid)];
			const int end = g.a == n ? 0 : 1;
			if (line_face(s.roads, sid) < 0 || !line_end_at(s.roads, sid, end).joint || line_end_at(s.roads, sid, end).own) continue;
			const int station = first(sid, end);
			if (station < 0) continue; // no pole of its own: its far end's pole takes the span
			const PoleId mine = id_of(LinePole{ sid, station, -1, -1 });
			bool strung = false;
			for (const LineSpan &sp : at) strung = strung || id_of(sp.a) == mine || id_of(sp.b) == mine;
			joints_unjoined += strung ? 0 : 1;
		}
		// a junction with lines on two arms in line, each ending on a corner pole that stands, and no span across it
		bool in_line = false;
		for (int sid : s.roads.nodes[size_t(n)].segs) {
			const int on = line_straight_on(s.roads, n, sid), ka = line_corner_at(s.roads, n, sid);
			const int kb = on >= 0 ? line_corner_at(s.roads, n, on) : -1;
			in_line = in_line || (ka >= 0 && kb >= 0 && ka != kb && corner_pole(s.roads, n, ka).stands && corner_pole(s.roads, n, kb).stands);
		}
		unjoined += in_line && !across ? 1 : 0;
	}
	// two line poles closer than poles.min_gap_m with no span between them; a line pole against a signal post
	const double gap = def.poles.min_gap_m;
	const float pole_r = std::max(def.footprint[int(FurnitureKind::LAMP)].x_m, def.footprint[int(FurnitureKind::LAMP)].z_m) * HALF;
	const float post_r = std::max(def.footprint[int(FurnitureKind::SIGNAL)].x_m, def.footprint[int(FurnitureKind::SIGNAL)].z_m) * HALF;
	const auto [close, touch] = close_and_touching(poles, posts, spans, gap, pole_r + post_r + def.items.gap_m);
	d["line_poles"] = int(poles.size());
	d["line_spans"] = spans.count + spans.across;
	d["corner_poles"] = corners;
	d["junction_spans"] = spans.across;
	d["junctions_unjoined"] = unjoined;
	d["joints_unjoined"] = joints_unjoined;
	d["line_broken"] = spans.broken;
	d["line_longest_m"] = spans.longest;
	d["line_shortest_m"] = spans.shortest;
	Array shortest; // its two poles: segment, station, node, corner, x, z each
	for (const Pole *p : { &spans.short_a, &spans.short_b }) shortest.append_array(Array::make(p->key.seg, p->key.station, p->key.node, p->key.corner, p->x, p->z));
	d["line_shortest_poles"] = shortest;
	d["junction_shortest_m"] = spans.across_shortest;
	d["line_close_pairs"] = close;
	d["line_signal_touch"] = touch;
}

} // namespace city
