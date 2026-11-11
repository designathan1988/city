// The profile's strips as pieces (road_section.h).
#include "render/roads/road_section.h"

#include <algorithm>

using namespace godot;

namespace city {

namespace {
constexpr float WALK_KEEP = 0.2f;

RoadMat paving_mat(Paving p) {
	switch (p) {
		case Paving::PAVERS: return RoadMat::PAVERS;
		case Paving::STONE: return RoadMat::STONE;
		case Paving::GRASS: return RoadMat::GRASS;
		default: return RoadMat::FOOTWAY;
	}
}

RoadMat surface_mat(Surface s) {
	switch (s) {
		case Surface::CONCRETE: return RoadMat::CONCRETE;
		case Surface::COBBLE: return RoadMat::COBBLE;
		case Surface::BALLAST: return RoadMat::BALLAST;
		default: return RoadMat::ASPHALT;
	}
}

bool kind_at(const Profile &p, int i, StripKind k) { return i >= 0 && i < int(p.edges.size()) && p.edges[i].strip.kind == k; }
bool raised_walk(const Profile &p, int i) { return kind_at(p, i, StripKind::SIDEWALK) && !p.edges[i].strip.flush; }
// A kerb between a raised footway and the carriageway (the ones that get gutters and dropped kerbs).
bool outer_kerb(const Profile &p, int i) { return kind_at(p, i, StripKind::CURB) && (raised_walk(p, i - 1) || raised_walk(p, i + 1)); }

bool on_island(const Profile &p, int i) { return island_at(p, i) || island_at(p, i - 1) || island_at(p, i + 1); }

// Where the carriageway around strip `i` begins and ends: its lanes, bays and shoulders, up to a kerb or a raised strip.
// An island does not end it: lanes and grime are counted from the outer kerbs, an island only stands on them.
void carriage_span(const Profile &p, int i, float &edge0, float &edge1) {
	auto flat = [&](int k) {
		if (k < 0 || k >= int(p.edges.size())) return false;
		const StripKind kind = p.edges[k].strip.kind;
		return kind == StripKind::LANE || kind == StripKind::PARKING || kind == StripKind::BIKE || kind == StripKind::SHOULDER || on_island(p, k);
	};
	int a = i, b = i;
	while (flat(a - 1)) a--;
	while (flat(b + 1)) b++;
	edge0 = p.edges[a].u0;
	edge1 = p.edges[b].u1;
}

// A raised footway beside an outer kerb, from the kerb: the band a car gate's drive slopes (L.drive_band, the service strip), the rest
// of the band a crossing slopes (L.ramp_run), then the walk part, which no lowering touches. Whole without `drops`.
void walk(const StreetLook &L, const Profile &p, int i, const Piece &base, bool drops, std::vector<Piece> &out) {
	Piece pc = base;
	const bool kerb_u1 = outer_kerb(p, i + 1), kerb_u0 = !kerb_u1 && outer_kerb(p, i - 1);
	pc.edge0 = kerb_u1 ? pc.u1 : pc.u0; // from the kerb: the footway's far side is the lot's
	pc.edge1 = kerb_u1 ? pc.u0 : pc.u1;
	const float w = pc.u1 - pc.u0, band = std::min(L.ramp_run, w - WALK_KEEP);
	if (!drops || pc.h <= 0 || w < L.ramp_min_walk || !(kerb_u1 || kerb_u0)) {
		out.push_back(pc);
		return;
	}
	const float cuts[3] = { 0.f, std::min(L.drive_band, band), band }; // the bands' edges, metres from the kerb
	const auto at_u = [&](float d) { return kerb_u1 ? pc.u1 - d : pc.u0 + d; };
	Piece rest = pc;
	if (kerb_u1) rest.u1 = at_u(band);
	else rest.u0 = at_u(band);
	Piece bands[2];
	for (int k = 0; k < 2; k++) {
		Piece &b = bands[k];
		b = pc;
		b.ramp = Ramp::BAND;
		b.band = band;
		b.u0 = kerb_u1 ? at_u(cuts[k + 1]) : at_u(cuts[k]);
		b.u1 = kerb_u1 ? at_u(cuts[k]) : at_u(cuts[k + 1]);
		b.kerb_d0 = kerb_u1 ? cuts[k + 1] : cuts[k];
		b.kerb_d1 = kerb_u1 ? cuts[k] : cuts[k + 1];
	}
	const bool behind = cuts[2] > cuts[1]; // a footway narrower than the drive's band has no band behind it
	if (kerb_u1) {
		out.push_back(rest);
		if (behind) out.push_back(bands[1]);
		out.push_back(bands[0]);
	} else {
		out.push_back(bands[0]);
		if (behind) out.push_back(bands[1]);
		out.push_back(rest);
	}
}

// Strip `i`'s piece moved to where the island stands (road_mesh.h island_at).
void onto_island(const Profile &p, int i, Piece &pc) {
	auto width = [&](int k) { return p.edges[k].u1 - p.edges[k].u0; };
	if (island_at(p, i)) {
		pc.u0 = p.edges[i - 1].u0;
		pc.u1 = p.edges[i + 1].u1;
	} else if (island_at(p, i + 1)) { // its kerb on this side
		pc.u1 = pc.u0;
		pc.u0 -= width(i);
	} else if (island_at(p, i - 1)) {
		pc.u0 = pc.u1;
		pc.u1 += width(i);
	} else if (on_island(p, i + 1)) { // the lane beside that kerb
		pc.u1 -= width(i + 1);
	} else if (on_island(p, i - 1)) {
		pc.u0 += width(i - 1);
	}
	pc.island = on_island(p, i);
}
} // namespace

bool island_at(const Profile &p, int i) {
	auto is = [&](int k, StripKind kind) { return k >= 0 && k < int(p.edges.size()) && p.edges[k].strip.kind == kind; };
	return is(i, StripKind::MEDIAN) && !p.edges[i].strip.flush && is(i - 1, StripKind::CURB) && is(i + 1, StripKind::CURB)
			&& is(i - 2, StripKind::LANE) && is(i + 2, StripKind::LANE);
}

bool island_span(const Profile &p, float &u0, float &u1) {
	for (int i = 0; i < int(p.edges.size()); i++) {
		if (!island_at(p, i)) continue;
		u0 = p.edges[i - 1].u0;
		u1 = p.edges[i + 1].u1;
		return true;
	}
	return false;
}

std::vector<Piece> pieces_of(const StreetLook &L, const Profile &p, bool drops) {
	std::vector<Piece> out;
	for (int i = 0; i < int(p.edges.size()); i++) {
		const Strip &s = p.edges[i].strip;
		Piece pc;
		pc.u0 = p.edges[i].u0;
		pc.u1 = p.edges[i].u1;
		onto_island(p, i, pc);
		pc.mat = surface_mat(p.surface);
		pc.col = L.carriage(p);
		switch (s.kind) {
			case StripKind::BIKE: pc.col = L.bike; pc.mat = RoadMat::ASPHALT; break;
			case StripKind::SHOULDER: if (p.surface != Surface::BALLAST) pc.col = L.shoulder; break;
			case StripKind::CURB:
				pc.mat = RoadMat::KERB;
				pc.col = L.curb;
				pc.h = outer_kerb(p, i) ? L.h_curb : L.h_curb_median;
				pc.ramp = outer_kerb(p, i) ? Ramp::KERB : Ramp::NONE;
				break;
			case StripKind::SIDEWALK:
				pc.mat = paving_mat(s.pave);
				pc.col = L.paving[int(s.pave) & 3];
				pc.face = s.pave == Paving::GRASS ? L.curb : pc.col;
				pc.h = s.flush ? 0 : L.h_sidewalk;
				walk(L, p, i, pc, drops, out);
				continue;
			case StripKind::MEDIAN:
				pc.mat = paving_mat(s.pave);
				pc.col = L.median[int(s.pave) & 3];
				pc.h = s.flush ? 0 : L.h_median;
				pc.face = L.curb;
				out.push_back(pc);
				continue;
			case StripKind::BARRIER: pc.mat = RoadMat::CONCRETE; pc.col = L.barrier; pc.h = L.h_barrier; break;
			default: break;
		}
		if (s.kind == StripKind::LANE && pc.mat == RoadMat::ASPHALT && !p.rail) {
			pc.mat = RoadMat::LANE;
			pc.lane = s.width;
			pc.tone = int(p.road_class);
			carriage_span(p, i, pc.edge0, pc.edge1);
		}
		pc.face = pc.col;
		out.push_back(pc);
	}
	return out;
}

std::vector<Piece> without_island(const std::vector<Piece> &ps) {
	std::vector<Piece> out;
	for (const Piece &pc : ps) {
		if (!pc.island) out.push_back(pc);
		else if (out.back().u1 < pc.u1) out.back().u1 = pc.u1;
	}
	return out;
}

} // namespace city
