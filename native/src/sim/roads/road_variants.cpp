// The cross-section of a class road under the tool's options (Roadcraft 4.0 world/roadTypes.ts travelProfile, resolveProfile,
// world/roadSection.ts sectionForWidth), written as the catalogue writes its strips: right edge to left edge, a footway and
// a median at their whole width with their kerbs taken out of them.
#include "sim/roads/road_variants.h"

#include "sim/roads/profiles.h"
#include "sim/roads/road_spec.h"

#include <algorithm>
#include <cmath>
#include <string>

namespace city {

namespace {

enum Band { BAND_NONE, BAND_BAY, BAND_CYCLE };
struct KerbBands {
	Band right, left;
};
// By preset (RoadVariant::parking), the order of data/ui/drawers.json road_options.parking.
constexpr KerbBands KERB_BANDS[] = { { BAND_NONE, BAND_NONE }, { BAND_BAY, BAND_BAY }, { BAND_BAY, BAND_NONE },
	{ BAND_NONE, BAND_BAY }, { BAND_CYCLE, BAND_CYCLE }, { BAND_CYCLE, BAND_BAY } };
constexpr int PRESETS = int(sizeof(KERB_BANDS) / sizeof(KERB_BANDS[0]));

// What a class road is made of, read back from its strips. `edge` is what runs at each side, a footway or a highway's shoulder;
// `walk` its whole width with the kerb between it and the carriageway (`edge_kerb`, none beside a shoulder).
struct Section {
	float walk = 0, kerb = 0, edge_kerb = 0, lane = 0, median = 0;
	int lanes = 0;
	Strip edge, planted;
	bool edged = false;
};

Section read_section(const Profile &b) {
	Section s;
	for (const Strip &x : b.strips) {
		if (x.kind == StripKind::LANE) {
			s.lanes++;
			s.lane = x.width;
		} else if (x.kind == StripKind::CURB) s.kerb = x.width;
		else if (x.kind == StripKind::MEDIAN) {
			s.planted = x;
			s.median = x.width;
		}
	}
	const Strip &outer = b.strips.front();
	s.edged = outer.kind == StripKind::SIDEWALK || outer.kind == StripKind::SHOULDER;
	if (s.edged) {
		s.edge = outer;
		s.edge_kerb = b.strips.size() > 1 && b.strips[1].kind == StripKind::CURB ? b.strips[1].width : 0.f;
		s.walk = outer.width + s.edge_kerb;
	}
	if (s.median > 0) s.median += 2 * s.kerb;
	return s;
}

// The total width on the 1 m grid: the lanes share what the footways and median leave, the footways give or take the rest.
void fit_width(Section &s, int lanes, float total, const Tuning &t) {
	total = std::round(total);
	s.lane = (total - s.median - 2 * s.walk) / float(lanes);
	if (s.lane >= t.road_variant_lane_lo_m && s.lane <= t.road_variant_lane_hi_m) return;
	s.lane = std::clamp(s.lane, t.road_variant_lane_lo_m, t.road_variant_lane_hi_m);
	s.walk = std::clamp((total - s.median - float(lanes) * s.lane) / 2, t.road_variant_walk_lo_m, t.road_variant_walk_hi_m);
}

Strip band_strip(Band b, float width) {
	Strip s;
	s.kind = b == BAND_BAY ? StripKind::PARKING : StripKind::BIKE;
	s.width = width;
	return s;
}

void push_lanes(std::vector<Strip> &out, int count, float width, int dir) {
	Strip s;
	s.kind = StripKind::LANE;
	s.width = width;
	s.dir = dir;
	out.insert(out.end(), size_t(count), s);
}

std::vector<Strip> strips_of(const Profile &b, const Section &s, int lanes, const KerbBands &bands, float bay) {
	Strip edge = s.edge, kerb, edge_kerb;
	edge.width = s.walk - s.edge_kerb;
	kerb.kind = edge_kerb.kind = StripKind::CURB;
	kerb.width = s.kerb;
	edge_kerb.width = s.edge_kerb;
	std::vector<Strip> out;
	if (s.edged) out.push_back(edge);
	if (s.edge_kerb > 0) out.push_back(edge_kerb);
	if (bands.right != BAND_NONE) out.push_back(band_strip(bands.right, bay));
	if (b.one_way) push_lanes(out, lanes, s.lane, 1);
	else {
		const int side = std::max(1, lanes / 2);
		push_lanes(out, side, s.lane, 1);
		if (s.median > 0) {
			Strip planted = s.planted;
			planted.width = s.median - 2 * s.kerb;
			out.insert(out.end(), { kerb, planted, kerb });
		}
		push_lanes(out, side, s.lane, -1);
	}
	if (bands.left != BAND_NONE) out.push_back(band_strip(bands.left, bay));
	if (s.edge_kerb > 0) out.push_back(edge_kerb);
	if (s.edged) out.push_back(edge);
	return out;
}

bool same(const RoadVariant &a, const RoadVariant &b) {
	return a.base == b.base && a.lanes == b.lanes && a.parking == b.parking && a.width == b.width && a.spec == b.spec;
}

// The kerb of the class's own section, or of the first road that has one: a road of the class with no kerb composes with the city's.
float kerb_of(const Catalog &c, const Section &s) {
	if (s.kerb > 0) return s.kerb;
	for (const Profile &p : c.profiles)
		for (const Strip &x : p.strips)
			if (x.kind == StripKind::CURB) return x.width;
	return 0;
}

// The road as the player composed it, in the class's asphalt and tone.
Profile composed(const Catalog &c, const Profile &b, const Section &s, const RoadSpec &spec) {
	Profile p = b;
	p.id = b.id + "~" + std::to_string(c.variants.size());
	p.category.clear();
	p.custom = true;
	p.class_card = false;
	p.strips = spec_strips(spec, kerb_of(c, s));
	if (s.edged && s.edge.kind == StripKind::SHOULDER) { // a shoulder is the class's, not what was composed (road_spec.cpp spec_of)
		p.strips.insert(p.strips.begin(), s.edge);
		p.strips.push_back(s.edge);
	}
	p.speed = spec.speed;
	p.priority = spec.priority;
	p.centre = spec_centre(spec);
	p.one_way = std::none_of(spec.elements.begin(), spec.elements.end(), [](const SpecElement &e) { return e.kind == SpecKind::LANE && e.dir < 0; })
			|| std::none_of(spec.elements.begin(), spec.elements.end(), [](const SpecElement &e) { return e.kind == SpecKind::LANE && e.dir > 0; });
	return p;
}
} // namespace

int road_variant(Catalog &c, const RoadVariant &v) {
	const Profile &b = c.profile(v.base);
	const bool plain = v.lanes == 0 && v.width <= 0 && v.parking == 0, spec = !v.spec.elements.empty();
	if (!spec && (!b.class_card || plain)) return v.base;
	const size_t own = c.profiles.size() - c.variants.size();
	for (size_t i = 0; i < c.variants.size(); i++)
		if (same(c.variants[i], v)) return int(own + i);
	if (c.profiles.size() == c.profiles.capacity()) return -1;
	Section s = read_section(b);
	if (spec) {
		Profile p = composed(c, b, s, v.spec);
		derive_profile(p, c.prices);
		const int id = c.add_profile(std::move(p));
		c.variants.push_back(v);
		return id;
	}
	const int lanes = v.lanes > 0 ? v.lanes : s.lanes;
	if (v.width > 0) fit_width(s, lanes, v.width, c.tune);
	// highways and ramps park nothing (Roadcraft 4.0 parkingAllowed)
	const bool parks = b.road_class != RoadClass::HIGHWAY && b.road_class != RoadClass::RAMP;
	const KerbBands bands = KERB_BANDS[parks && v.parking > 0 && v.parking < PRESETS ? v.parking : 0];
	Profile p = b;
	p.id = b.id + "~" + std::to_string(c.variants.size());
	p.category.clear();
	p.custom = true;
	p.class_card = false;
	p.strips = strips_of(b, s, lanes, bands, c.tune.road_bay_m);
	if (lanes != s.lanes) p.centre = lanes <= 1 || s.median > 0 ? "none" : lanes == 2 ? "dashed" : "solid";
	derive_profile(p, c.prices);
	const int id = c.add_profile(std::move(p));
	c.variants.push_back(v);
	return id;
}

void clear_variants(Catalog &c) { c.drop_variants(); }

} // namespace city
