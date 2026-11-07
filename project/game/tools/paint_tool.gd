## Landscape › Ground: paints the chosen surface, cover or biome (data/catalog/nature.json paint.kinds) under the disc.
extends BrushTool

var kind := 0
var strength := 0.0
var hardness := 0.0


func enter() -> void:
	var s := settings("PAINT")
	kind = s.kind
	radius = s.radius
	var paint: Dictionary = DataTables.catalog_table("nature").paint
	strength = maxf(paint.min_strength, minf(1.0, s.strength / paint.strength_scale))
	hardness = s.hardness / PERCENT
	ctl.hint.emit(tr("hint.tool.paint"), true)


func _dab_path(dabs: PackedVector3Array) -> Dictionary:
	return CityStore.dispatch(&"paint_path", [dabs, radius, strength, hardness, kind])


func _label() -> String:
	return "cmd.place.paintGround"
