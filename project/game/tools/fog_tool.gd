## Landscape › Sky › Fog: lays banks of mist where the player wants them, or wipes them; each dab keeps the thickness,
## height and drift it was laid with.
extends BrushTool

var strength := 0.0
var height := 0.0
var drift_ms := 0.0
var erase := false


func enter() -> void:
	var s := settings("FOG")
	radius = s.radius
	strength = s.strength / PERCENT
	height = s.height
	drift_ms = s.speed
	erase = s.erase != 0
	ctl.hint.emit(tr("hint.tool.fog"), true)


func _dab_path(dabs: PackedVector3Array) -> Dictionary:
	return CityStore.dispatch(&"fog_path", [dabs, radius, strength, height, drift_ms, erase])


func _label() -> String:
	return "cmd.fog"
