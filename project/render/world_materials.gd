## The materials the C++ renderers draw with (data/render/materials.json), made once: each row's shared sets under its own
## params, the road's surface means, the verge's grass, the lenses' and the vehicles' radiance, and the copies a level asks for
## (fade, ruin, body, layers). CityWorld keeps them alive: the core draws with their RIDs.
class_name WorldMaterials
extends RefCounted


## A material's parameters: the shared sets it uses (materials.json _shared), then its own.
static func params(table: Dictionary, key: String) -> Dictionary:
	var out := {}
	for set_name: String in table[key].get("uses", []):
		out.merge(table._shared[set_name])
	out.merge(table[key].params, true)
	return out


## Every material by name; `ground` lays the terrain's grass on the verge's (CityWorld owns the terrain's table).
static func make_all(environment: Environment, ground: Callable) -> Dictionary:
	var out := {}
	var table: Dictionary = DataTables.render_table("materials")
	for key in table:
		if key.begins_with("_"):
			continue
		var m := EffectMaterials.make(key, table[key], environment)
		var values := params(table, key)
		for name in values:
			m.set_shader_parameter(name, CityWorld.shader_value(values[name]))
		if key == "road":
			SurfaceMeans.apply(m, values, CityWorld.shader_value)
		elif key == "verge":
			ground.call(m)
		elif key == "prop":
			LampRadiance.lenses(m, environment)
		elif key == "agent":
			LampRadiance.vehicles(m, environment)
		out[key] = m
		if table[key].has("fade_shader"): # the same material for what is handing over to another level (building_level.gdshaderinc)
			var fade: ShaderMaterial = m.duplicate()
			fade.shader = load(table[key].fade_shader)
			out[key + "_fade"] = fade
		for variant: String in ["ruin", "body"]: # the same material for a building coming down in pieces (ruin_pieces.h) and somebody off their feet (body_renderer.h)
			if table[key].has(variant + "_shader"):
				var other: ShaderMaterial = m.duplicate()
				other.shader = load(table[key][variant + "_shader"])
				out[key + "_" + variant] = other
		for layer in int(table[key].get("layers", 0)): # the same material drawn in this order (render_priority)
			var over: ShaderMaterial = m.duplicate()
			over.render_priority = layer
			out["%s_%d" % [key, layer]] = over
	return out
