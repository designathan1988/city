## Landscape › Nature: the element brush. A held stroke lays the kind chosen (stones, grass, smoke...) under the disc, or
## takes away that kind or everything; one undo step a stroke. Each kind starts from its own brush (land_panels.json
## "kind_brush"); a slider the player moved is kept over it.
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
	var own: Dictionary = DataTables.ui("land_panels").ELEMENTS.kind_brush[kind]
	brush = {}
	for key: String in own:
		brush[key] = params.get(key, own[key])
	brush.strength = float(brush.strength) / PERCENT
	brush.variation = float(brush.variation) / PERCENT
	brush.intensity = float(brush.intensity) / PERCENT
	ctl.hint.emit(tr("hint.terrain.elements"), true)


func _dab_path(dabs: PackedVector3Array) -> Dictionary:
	return CityStore.dispatch(&"element_path", [dabs, radius, -1 if mode == ERASE_ALL else kind, brush, mode != 0])


func _label() -> String:
	return "cmd.place.element"
