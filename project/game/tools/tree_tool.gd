## Landscape › Nature: plants a stand under the disc, one tree at a click, or cuts the trees there (the woods' own too)
## and leaves a clearing. A held stroke is one undo step.
extends BrushTool

var kind := 0
var mode := 0
var brush := {}


func enter() -> void:
	var s := settings("TREES")
	kind = s.kind
	mode = s.mode
	radius = s.radius
	brush = {"density": s.density, "height": s.height, "variation": s.variation / PERCENT, "spacing": s.spacing}
	ctl.hint.emit(tr("hint.tool.trees"), true)


func _dab(p: Vector3) -> Dictionary:
	return CityStore.dispatch(&"tree_brush", [p, radius, mode, kind, brush])


func _label() -> String:
	return "cmd.place.tree"
