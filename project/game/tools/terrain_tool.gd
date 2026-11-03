## Relief brush (Roadcraft world/brush.ts, Landscape › Relief): hold the left button to raise, lower, smooth or level
## (or lay a landform) under the disc; the core applies one dab per DAB_S held and the whole stroke is one undo step.
## Roads keep their corridors and buildings settle on the new ground.
extends ToolBase

## Roadcraft dabs every frame with its dt; one dab per DAB_S keeps each step above the 1 cm height quantum.
const DAB_S := 0.1
const PERCENT := 100.0

var mode := 0
var radius := 0.0
var strength := 0.0
var _at := Vector3.INF
var _held := 0.0


func enter() -> void:
	var spec: Dictionary = _spec()
	mode = params.get("mode", 0)
	radius = params.get("radius", spec.radius.default)
	strength = params.get("strength", spec.strength.default) / PERCENT
	ctl.hint.emit(tr("hint.terrain"), true)


func _spec() -> Dictionary:
	for c in DataTables.ui("drawers").categories:
		if c.has("brush"):
			return c.brush
	return {}


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
	var r: Dictionary = CityStore.dispatch(&"terrain_brush", [_at, radius, strength, dt, mode])
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)
