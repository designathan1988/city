## Zone brush: click or drag to paint the chosen zone on the road sides under
## the brush; the core knocks down buildings of another zone there and queues the new lots for growth.
extends ToolBase

## A new dab once the brush moved this share of its radius (zoneTool dabs along the stroke).
const DAB_SPACING := 0.5
## A fast drag reports the pointer far apart: the whole way is painted, up to this many dabs at a report (lotTool.ts move).
const MOST_DABS := 400
## What the brush paints with Shift held: no zone (lotTool.ts `remove`).
const NO_ZONE := 0

var zone := 1
var radius := 0.0
var _last := Vector3.INF
var _painting := 0 # the zone of the stroke in hand


func enter() -> void:
	zone = params.get("zone", 1)
	radius = params.get("radius", DataTables.ui("drawers").zone_radius)
	ctl.hint.emit("", true)


func click(p: Vector3) -> void:
	_last = p
	_painting = NO_ZONE if Input.is_key_pressed(KEY_SHIFT) else zone
	CityStore.dispatch(&"begin_stroke", ["cmd.place.zone"])
	_dab(p)


## The whole stroke is one undo step.
func release(_p: Vector3) -> void:
	CityStore.dispatch(&"end_stroke")
	_last = Vector3.INF


func drag(p: Vector3) -> void:
	if _last == Vector3.INF:
		_last = p
		_dab(p)
		return
	var spacing := radius * DAB_SPACING
	var far := p.distance_to(_last)
	if far < spacing:
		return
	var steps := mini(ceili(far / spacing), MOST_DABS)
	for k in range(1, steps + 1):
		_dab(_last.lerp(p, float(k) / steps))
	_last = p


func hover(p: Vector3) -> void:
	ctl.preview.show_disc(p, radius, true)


func exit() -> void:
	CityStore.dispatch(&"end_stroke")
	ctl.preview.clear()


func _dab(p: Vector3) -> void:
	var lot: int = CityStore.query(&"lot_at", [p])
	if lot != 0: # a lot the player drew takes the zone whole
		CityStore.dispatch(&"lot_zone", [lot, _painting])
		return
	var r: Dictionary = CityStore.dispatch(&"paint_zone", [p, radius, _painting])
	if not r.get("ok", false) and r.get("error", "") != "tools.toast.zoneSame":
		ctl.hint.emit(refusal(r), false)
