// The profile editor's API (Customise, My roads): a road as elements for script, what stops it from being built, and the profile made
// of it. The dictionaries are the editor's own: {elements: [{kind, width, dir, bus, solid, flush, pave}], speed, priority, centre}.
#include "api/city_core.h"
#include "api/command_hold.h"

#include "sim/roads/road_spec.h"
#include "sim/roads/road_variants.h"
#include "sim/state/city_state.h"

#include <mutex>

using namespace godot;

namespace city {

namespace {
const char *const KIND_NAMES[] = { "footway", "parking", "cycle", "lane", "median" };
constexpr int KINDS = int(sizeof(KIND_NAMES) / sizeof(KIND_NAMES[0]));
constexpr int SURFACES = int(Surface::BALLAST) + 1; // every Surface, by its number (profile_types.h)

RoadSpec read_spec(const Dictionary &d) {
	RoadSpec s;
	s.speed = float(d.get("speed", 0));
	s.priority = int(d.get("priority", 0));
	s.centre = String(d.get("centre", "auto")).utf8().get_data();
	const int surface = int(d.get("surface", int(SURFACE_OF_CLASS)));
	s.surface = surface >= 0 && surface < SURFACES ? int8_t(surface) : SURFACE_OF_CLASS; // a number that names no surface: the class's
	const Array list = d.get("elements", Array());
	for (int i = 0; i < list.size(); i++) {
		const Dictionary x = list[i];
		SpecElement e;
		const String kind = x.get("kind", "lane");
		for (int k = 0; k < KINDS; k++)
			if (kind == KIND_NAMES[k]) e.kind = SpecKind(k);
		e.width = float(x.get("width", 0));
		e.dir = int(x.get("dir", 1));
		e.bus = bool(x.get("bus", false));
		e.solid = bool(x.get("solid", false));
		e.flush = bool(x.get("flush", false));
		e.pave = Paving(int(x.get("pave", 0)));
		s.elements.push_back(e);
	}
	return s;
}

Dictionary write_spec(const RoadSpec &s) {
	Dictionary d;
	d["speed"] = s.speed;
	d["priority"] = s.priority;
	d["centre"] = String(s.centre.c_str());
	d["surface"] = int(s.surface);
	Array list;
	for (const SpecElement &e : s.elements) {
		Dictionary x;
		x["kind"] = KIND_NAMES[int(e.kind)];
		x["width"] = e.width;
		x["dir"] = e.dir;
		x["bus"] = e.bus;
		x["solid"] = e.solid;
		x["flush"] = e.flush;
		x["pave"] = int(e.pave);
		list.push_back(x);
	}
	d["elements"] = list;
	return d;
}
} // namespace

// What the editor's widths run between, and the width of a band by the kerb.
Dictionary CityCore::road_spec_limits() const {
	Dictionary d;
	d["lane"] = Vector2(LANE_LEAST_M, LANE_MOST_M);
	d["footway"] = Vector2(WALK_LEAST_M, WALK_MOST_M);
	d["median"] = Vector2(MEDIAN_LEAST_M, MEDIAN_MOST_M);
	d["band"] = state_->catalog.tune.road_bay_m;
	return d;
}

Dictionary CityCore::road_spec(int profile) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return write_spec(spec_of(state_->catalog.profile(profile)));
}

Array CityCore::road_spec_issues(int base, const Dictionary &spec) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Array out;
	for (const SpecIssue &i : spec_issues(read_spec(spec), state_->catalog.profile(base).road_class)) {
		Dictionary d;
		d["problem"] = String(i.problem.c_str());
		Array list;
		for (int e : i.elements) list.push_back(e);
		d["elements"] = list;
		out.push_back(d);
	}
	return out;
}

// The profile made of `spec`: {ok, error, index}. Refused (index -1) when the spec has issues (road_spec_issues tells
// which) or when the catalogue holds no room for another variant (city::road_variant's -1 means only that).
Dictionary CityCore::road_spec_make(int base, const Dictionary &spec) {
	Dictionary d;
	d["ok"] = false;
	d["index"] = -1;
	CommandHold hold(*state_, "road_spec_make");
	const RoadSpec s = read_spec(spec);
	if (!spec_issues(s, state_->catalog.profile(base).road_class).empty()) {
		d["error"] = "hint.road.specInvalid";
		return d;
	}
	RoadVariant v;
	v.base = base;
	v.spec = s;
	const int index = city::road_variant(state_->catalog, v);
	if (index < 0) {
		d["error"] = "hint.road.variantsFull";
		return d;
	}
	d["ok"] = true;
	d["error"] = "";
	d["index"] = index;
	return d;
}

} // namespace city
