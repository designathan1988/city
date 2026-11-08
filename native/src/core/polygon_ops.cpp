// Boolean operations on plans (polygon_ops.h).
#include "core/polygon_ops.h"

#include <iterator> // before Clipper2: its header needs it and does not say so

#include <clipper.h>

#include <algorithm>
#include <cmath>

namespace city {

namespace {
using Clipper2Lib::ClipType;
using Clipper2Lib::Clipper64;
using Clipper2Lib::FillRule;
using Clipper2Lib::Path64;
using Clipper2Lib::Paths64;
using Clipper2Lib::Point64;
using Clipper2Lib::PolyPath64;
using Clipper2Lib::PolyTree64;

constexpr double UNITS_PER_M = 1000; // Clipper2 works on integers: plans go to it in millimetres
constexpr int MOST_SPLITS = 4; // of one piece: past that a hole stays filled, and that is a plan of many holes
constexpr int64_t PAST_BOX = 1000; // a split's halves reach a metre past the piece's box, so no edge lies on theirs

Paths64 paths_of(const std::vector<PlanRing> &rings) {
	Paths64 out;
	out.reserve(rings.size());
	for (const PlanRing &ring : rings) {
		Path64 path;
		path.reserve(ring.size());
		for (const V2 &p : ring) path.push_back(Point64(std::llround(double(p.x) * UNITS_PER_M), std::llround(double(p.y) * UNITS_PER_M)));
		if (Clipper2Lib::Area(path) < 0) std::reverse(path.begin(), path.end()); // under the non-zero rule rings wound against each other cancel
		out.push_back(std::move(path));
	}
	return out;
}

PlanRing ring_of(Path64 path) {
	if (Clipper2Lib::Area(path) < 0) std::reverse(path.begin(), path.end());
	PlanRing out;
	out.reserve(path.size());
	for (const Point64 &p : path) out.push_back(V2(float(double(p.x) / UNITS_PER_M), float(double(p.y) / UNITS_PER_M)));
	return out;
}

void gather(const PolyPath64 &parent, int splits, std::vector<PlanRing> &out);

void clip_to(const Paths64 &whole, const Path64 &box, int splits, std::vector<PlanRing> &out) {
	Clipper64 c;
	c.AddSubject(whole);
	c.AddClip(Paths64{ box });
	PolyTree64 tree;
	c.Execute(ClipType::Intersection, FillRule::EvenOdd, tree);
	gather(tree, splits, out);
}

// A piece with holes in two, through the first hole: each half is cut again until none has any.
void split(const PolyPath64 &piece, int splits, std::vector<PlanRing> &out) {
	Paths64 whole{ piece.Polygon() };
	for (const auto &hole : piece) {
		whole.push_back(hole->Polygon());
		gather(*hole, splits, out); // the islands inside a hole stand by themselves
	}
	const Path64 &first = piece.Child(0)->Polygon();
	int64_t lo = first[0].x, hi = lo, x0 = piece.Polygon()[0].x, x1 = x0, y0 = piece.Polygon()[0].y, y1 = y0;
	for (const Point64 &p : first) {
		lo = std::min(lo, p.x);
		hi = std::max(hi, p.x);
	}
	for (const Point64 &p : piece.Polygon()) {
		x0 = std::min(x0, p.x);
		x1 = std::max(x1, p.x);
		y0 = std::min(y0, p.y);
		y1 = std::max(y1, p.y);
	}
	const int64_t cut = (lo + hi) / 2, top = y1 + PAST_BOX, bottom = y0 - PAST_BOX;
	clip_to(whole, Path64{ Point64(x0 - PAST_BOX, bottom), Point64(cut, bottom), Point64(cut, top), Point64(x0 - PAST_BOX, top) }, splits + 1, out);
	clip_to(whole, Path64{ Point64(cut, bottom), Point64(x1 + PAST_BOX, bottom), Point64(x1 + PAST_BOX, top), Point64(cut, top) }, splits + 1, out);
}

void gather(const PolyPath64 &parent, int splits, std::vector<PlanRing> &out) {
	for (const auto &piece : parent) {
		if (piece->Count() == 0 || splits >= MOST_SPLITS) out.push_back(ring_of(piece->Polygon()));
		else split(*piece, splits, out);
	}
}

std::vector<PlanRing> run(ClipType kind, const std::vector<PlanRing> &a, const std::vector<PlanRing> &b) {
	Clipper64 c;
	c.AddSubject(paths_of(a));
	c.AddClip(paths_of(b));
	PolyTree64 tree;
	c.Execute(kind, FillRule::NonZero, tree);
	std::vector<PlanRing> out;
	gather(tree, 0, out);
	return out;
}
} // namespace

std::vector<PlanRing> plan_intersection(const std::vector<PlanRing> &a, const std::vector<PlanRing> &b) { return run(ClipType::Intersection, a, b); }
std::vector<PlanRing> plan_difference(const std::vector<PlanRing> &a, const std::vector<PlanRing> &b) { return run(ClipType::Difference, a, b); }

} // namespace city
