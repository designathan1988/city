## Relief brush (Landscape › Relief): hold the left button to raise, lower, smooth or level
## (or lay a landform) under the disc; the core applies one dab per DAB_S held and the whole stroke is one undo step.
## Roads keep their corridors and buildings settle on the new ground.
extends ToolBase

## One dab per DAB_S held keeps each step above the 1 cm height quantum.
const DAB_S := 0.1
const PERCENT := 100.0

var mode := 0
var radius := 0.0
var strength := 0.0
var hardness := 0.0
var _at := Vector3.INF
var _held := 0.0


func enter() -> void:
	var spec: Dictionary = _spec()
	mode = params.get("mode", 0)
	radius = params.get("radius", spec.radius.default)
	strength = params.get("strength", spec.strength.default) / PERCENT
	hardness = params.get("hardness", spec.hardness.default) / PERCENT
	ctl.hint.emit("", true)


func _spec() -> Dictionary:
	var out := {}
	for s in DataTables.ui("land_panels").TERRAIN.sliders:
		if s.has("key"):
			out[s.key] = s
	return out


func exit() -> void:
	CityStore.dispatch(&"end_stroke")
	ctl.preview.clear()


func hover(p: Vector3) -> void:
	ctl.preview.show_disc(p, radius, true)


func click(p: Vector3) -> void:
	_at = p
	_held = 0.0
	CityStore.dispatch(&"begin_stroke", ["cmd.place.terrainBrush"])
	_dab(DAB_S)


func drag(p: Vector3) -> void:
	_at = p
	ctl.preview.show_disc(p, radius, true)


func tick(delta: float) -> void:
	if _at == Vector3.INF:
		return
	_held += delta
	while _held >= DAB_S:
		_held -= DAB_S
		_dab(DAB_S)


func release(_p: Vector3) -> void:
	_at = Vector3.INF
	CityStore.dispatch(&"end_stroke")


func _dab(dt: float) -> void:
	var r: Dictionary
	if mode < 0: # the gully brush: cut or wipe, on the light the land reads (sim/nature/ground_paint.h)
		r = CityStore.dispatch(&"paint_brush", [_at, radius, strength * dt / DAB_S, 0.0, mode])
	else:
		r = CityStore.dispatch(&"terrain_brush", [_at, radius, strength, dt, mode, hardness])
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)
