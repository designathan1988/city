## The sliders and choices of a Landscape tool with their defaults (data/ui/land_panels.json), over what the drawer
## last set; and what of a kept choice the panel still offers, for the choices a city comes back with (game/store/session.gd).
class_name BrushDefaults
extends RefCounted


static func of(tool_name: String, params: Dictionary) -> Dictionary:
	var out: Dictionary = {}
	var spec: Dictionary = DataTables.ui("land_panels")[tool_name]
	for s in spec.get("sliders", []):
		if s.has("key"):
			out[s.key] = s.default
	for g in spec.get("groups", []):
		out[g.key] = g.default
	out.merge(params, true)
	return out


## The Landscape tools whose panel holds a choice of the player's (a slider or a group of cards).
static func tools() -> Array[String]:
	var out: Array[String] = []
	var panels: Dictionary = DataTables.ui("land_panels")
	for name: String in panels:
		if ToolController.Tool.has(name) and panels[name] is Dictionary and _keys(panels[name]).size() > 0:
			out.append(name)
	return out


## The choices of `tool_name` in `held` (a file's, maybe edited by hand or from another version) that its panel offers: a
## slider's value into its range, a card's value only when a card has it; anything else is left out, so the default stands.
static func kept(tool_name: String, held: Variant) -> Dictionary:
	var out: Dictionary = {}
	if not held is Dictionary:
		return out
	var keys := _keys(DataTables.ui("land_panels")[tool_name])
	for key: String in keys:
		var v: Variant = held.get(key)
		if not (v is float or v is int) or not is_finite(float(v)):
			continue
		var spec: Variant = keys[key]
		if spec is Dictionary:
			out[key] = clampf(float(v), spec.min, spec.max)
		elif int(v) in spec:
			out[key] = int(v)
	return out


## Each choice of a panel by its key: a slider's spec, or the values its cards offer (one key may span several groups).
static func _keys(spec: Dictionary) -> Dictionary:
	var out: Dictionary = {}
	for s: Dictionary in spec.get("sliders", []):
		if s.has("key"):
			out[s.key] = s
	for g: Dictionary in spec.get("groups", []):
		var values: Array = out.get(g.key, [])
		for c: Dictionary in g.cards:
			values.append(int(c.value))
		out[g.key] = values
	return out
