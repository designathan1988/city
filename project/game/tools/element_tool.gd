## Landscape › Nature: the element brush. A held stroke lays the kind chosen (stones, grass, smoke...) under the disc, or
## takes away that kind or everything; one undo step a stroke. Each kind has its own brush (Roadcraft ui/toolChoices.ts
## elementBrushes): it starts from the kind's defaults (land_panels.json "kind_brush"), a slider the player moves changes the
## brush of the kind in hand only, and another kind taken up shows its own; the brushes are kept between sessions ("kept").
extends BrushTool

const ERASE_KIND := 1
const ERASE_ALL := 2

var kind := 0
var mode := 0
var brush := {}


func enter() -> void:
	var s := settings("ELEMENTS")
	kind = s.kind
	mode = s.mode
	radius = s.radius
	var panel: Dictionary = DataTables.ui("land_panels").ELEMENTS
	var own: Dictionary = panel.kind_brush[kind]
	_hold_kind_brush(panel.sliders, own)
	brush = {}
	for key: String in own:
		brush[key] = params.get(key, own[key])
	brush.strength = float(brush.strength) / Units.PERCENT
	brush.variation = float(brush.variation) / Units.PERCENT
	brush.intensity = float(brush.intensity) / Units.PERCENT
	ctl.hint.emit(tr("hint.terrain.elements"), true)


## The sliders of the kind's brush (those of its kind_brush) in `params` are the brush of the kind they were set for
## ("brush_kind"): kept there under "brushes", and the kind in hand's own brush put in their place, its defaults where it has none.
## The params are the ones ToolController remembers, so the panel shows the kind's brush.
func _hold_kind_brush(sliders: Array, own: Dictionary) -> void:
	var brushes: Dictionary = params.get("brushes", {}).duplicate(true)
	var was: int = params.get("brush_kind", kind)
	var held: Dictionary = brushes.get(was, {}).duplicate()
	for sl: Dictionary in sliders:
		if own.has(sl.key) and params.has(sl.key):
			held[sl.key] = params[sl.key]
	if not held.is_empty():
		brushes[was] = held
	var mine: Dictionary = brushes.get(kind, {})
	for sl: Dictionary in sliders:
		if own.has(sl.key):
			params[sl.key] = mine.get(sl.key, own[sl.key])
	params.brushes = brushes
	params.brush_kind = kind


func _dab_path(dabs: PackedVector3Array) -> Dictionary:
	return CityStore.dispatch(&"element_path", [dabs, radius, -1 if mode == ERASE_ALL else kind, brush, mode != 0])


func _label() -> String:
	return "cmd.place.element"
