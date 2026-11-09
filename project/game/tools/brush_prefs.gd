## What the Landscape brushes keep between sessions (Roadcraft ui/toolChoices.ts): of each panel, the keys data/ui/land_panels.json
## lists as "kept" (the ground the paint lays, the tree and element brushes, the cloud's and the fog's), written by Settings into
## settings.cfg and handed back to ToolController.remembered at the next start, so a brush reopens as it was left.
class_name BrushPrefs
extends RefCounted

const SECTION := "land_brushes"

var kept: Dictionary = {} # tool name -> {key: value}


func read(cfg: ConfigFile) -> void:
	kept = {}
	for tool_name: String in BrushPrefs.panels():
		var held: Variant = cfg.get_value(SECTION, tool_name, {})
		var own: Dictionary = BrushPrefs.kept_of(tool_name, held) if held is Dictionary else {}
		if not own.is_empty(): # a panel the last session never touched keeps its defaults
			kept[tool_name] = own


func write(cfg: ConfigFile) -> void:
	for tool_name: String in kept:
		cfg.set_value(SECTION, tool_name, kept[tool_name])


## The keys of `params` the panel keeps; false when nothing the panel keeps changed (nothing to write).
func keep(tool_name: String, params: Dictionary) -> bool:
	if not BrushPrefs.panels().has(tool_name):
		return false
	var now: Dictionary = kept.get(tool_name, {}).duplicate()
	now.merge(BrushPrefs.kept_of(tool_name, params), true)
	if now == kept.get(tool_name, {}):
		return false
	kept[tool_name] = now
	return true


## The panels that keep something, by tool name.
static func panels() -> Array:
	var out := []
	var table: Dictionary = DataTables.ui("land_panels")
	for tool_name: String in table:
		if table[tool_name] is Dictionary and table[tool_name].has("kept"):
			out.append(tool_name)
	return out


static func kept_of(tool_name: String, params: Dictionary) -> Dictionary:
	var out := {}
	for key: String in DataTables.ui("land_panels")[tool_name].kept:
		if params.has(key):
			out[key] = params[key]
	return out
