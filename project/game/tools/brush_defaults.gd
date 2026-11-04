## The sliders and choices of a Landscape tool with their defaults (data/ui/drawers.json `land`), over what the drawer
## last set.
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
