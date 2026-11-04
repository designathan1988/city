// Strict Dictionary -> TransitParams conversion for project/data/catalog/transit.json. The CITY_TRANSIT_FIELDS X-macro
// drives the scalar section, so a field added to sim/catalog/transit_params.h is automatically required here.
#include "api/catalog_loader.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

bool load_transit_params(const Dictionary &t, TransitParams &p, PackedStringArray &errors) {
	Dictionary params = t.has("params") ? Dictionary(t["params"]) : Dictionary();
#define CITY_TRANSIT_LOAD(name) \
	if (!params.has(#name)) errors.push_back("transit.params: falta '" #name "'"); \
	else p.name = float(double(params[#name]));
	CITY_TRANSIT_FIELDS(CITY_TRANSIT_LOAD)
#undef CITY_TRANSIT_LOAD
	Array colours = t.has("line_colours") ? Array(t["line_colours"]) : Array();
	if (colours.is_empty()) errors.push_back("transit.line_colours: nenhuma cor");
	p.line_colours.clear();
	for (const Variant &c : colours) p.line_colours.push_back(CatalogLoader::parse_color(String(c)));
	return errors.is_empty();
}

} // namespace city
