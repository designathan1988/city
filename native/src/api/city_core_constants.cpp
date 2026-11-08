// What the scripts read once at boot (CityStore.constants): the core's enums, cells, sections, sentinels and the values the shaders
// share, each by its own name, so no script types a number the core means.
#include "api/city_core.h"
#include "api/frame_sections.h"
#include "api/script_enums.h"

#include "core/grid.h"
#include "core/shader_contracts/all.h"
#include "core/units.h"
#include "render/render_layers.h"
#include "sim/catalog/catalog.h"
#include "sim/commands/body_ops.h"
#include "sim/persistence/save_street.h"
#include "sim/roads/road_spec.h"
#include "sim/simulation/simulation.h"

#include <godot_cpp/variant/array.hpp>

using namespace godot;

namespace city {

Dictionary CityCore::constants() const {
	Dictionary d;
	d["speed_paused"] = int(SimSpeed::PAUSED);
	d["speed_fastest"] = int(SimSpeed::FASTEST);
	d["minutes_per_day"] = MINUTES_PER_DAY;
	d["minutes_per_hour"] = MINUTES_PER_HOUR;
	d["zone_codes"] = ZONE_CODES;
	Dictionary layers;
	for (const RenderLayerName &l : RENDER_LAYERS) layers[l.name] = int64_t(l.bit);
	d["render_layers"] = layers;
	d["enums"] = script_enums(); // every enum a script names (script_enums.h)
	d["chunk_m"] = CHUNK; // the core's cell: lots, props and the far levels are read a cell at a time
	d["setback_by_module"] = SETBACK_BY_MODULE; // the body edits' own choices (body_ops.h)
	d["every_corner"] = EVERY_CORNER;
	Dictionary spec_strips; // the catalogue's strip each element of a composed road draws as (road_spec.h strip_of)
	for (int k = 0; k < int(SpecKind::COUNT); k++) spec_strips[SPEC_KIND_NAMES[k]] = STRIP_KIND_NAMES[int(strip_of(SpecKind(k)))];
	d["spec_strips"] = spec_strips;
	d["save_tag_street"] = int64_t(SAVE_TAG_STREET); // the street block's tag in a save (save_street.h)
	Array sections;
	for (const char *name : FRAME_SECTIONS) sections.push_back(name);
	d["frame_sections"] = sections;
	Dictionary shader; // what the shaders read too, by the shaders' names (core/shader_contracts/all.h)
	for (const ShaderContractGroup &g : SHADER_CONTRACT_GROUPS)
		for (size_t i = 0; i < g.count; i++) {
			const ShaderContract &c = g.rows[i];
			shader[String(c.name)] = c.size == 1 ? Variant(c.value[0]) : Variant(Vector3(real_t(c.value[0]), real_t(c.value[1]), real_t(c.value[2])));
		}
	d["shader"] = shader;
	return d;
}

} // namespace city
