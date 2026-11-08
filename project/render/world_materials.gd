## The materials the C++ renderers draw with (data/render/materials.json), made once: each row's shared sets under its own
## params, the road's surface means, the verge's grass, the lenses' radiance, and the copies a level asks for
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


## The type of each uniform a shader declares (its includes' too), by name.
static func uniform_kinds(shader: Shader) -> Dictionary:
	var out := {}
	for u: Dictionary in shader.get_shader_uniform_list():
		out[u.name] = u.type
	return out


## A param as its uniform takes it: a list of numbers is the vector a vec2, vec3 or vec4 uniform is, or the Color a source_color
## one is (sRGB, which Godot makes linear only for a Color: servers/rendering/storage/variant_converters.h convert_to_vector);
## a list of such lists the vec3[] or vec4[] it fills; a float[] uniform takes the list. Anything else as CityWorld.shader_value
## reads it (a colour, a texture, a list of colours).
static func typed(kinds: Dictionary, name: String, value: Variant) -> Variant:
	if value is Array and not value.is_empty() and value[0] is Array: # a list of vectors, for a vec3[] or vec4[] uniform
		match kinds.get(name, TYPE_NIL):
			TYPE_PACKED_VECTOR3_ARRAY:
				var v3 := PackedVector3Array()
				for p: Array in value:
					v3.append(Vector3(p[0], p[1], p[2]))
				return v3
			TYPE_PACKED_VECTOR4_ARRAY:
				var v4 := PackedVector4Array()
				for p: Array in value:
					v4.append(Vector4(p[0], p[1], p[2], p[3]))
				return v4
	if value is Array and not value.is_empty() and not (value[0] is String) and not (value[0] is Array):
		match kinds.get(name, TYPE_NIL):
			TYPE_COLOR:
				return Color(value[0], value[1], value[2], value[3] if value.size() > 3 else 1.0)
			TYPE_VECTOR2:
				return Vector2(value[0], value[1])
			TYPE_VECTOR3:
				return Vector3(value[0], value[1], value[2])
			TYPE_VECTOR4:
				return Vector4(value[0], value[1], value[2], value[3])
	return CityWorld.shader_value(value)


## Every material by name; `ground` lays the terrain's grass on the verge's (CityWorld owns the terrain's table).
static func make_all(environment: Environment, ground: Callable) -> Dictionary:
	var out := {}
	var table: Dictionary = DataTables.render_table("materials")
	for key in table:
		if key.begins_with("_"):
			continue
		var m := EffectMaterials.make(key, table[key], environment)
		var values := params(table, key)
		var kinds := uniform_kinds(m.shader)
		for name in values:
			m.set_shader_parameter(name, typed(kinds, name, values[name]))
		if key == "road":
			SurfaceMeans.apply(m, values, CityWorld.shader_value)
		elif key == "verge":
			ground.call(m)
		elif key == "prop":
			LampRadiance.lenses(m)
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
