## Zone brush: click or drag to paint the chosen zone on the road sides under
## the brush; the core knocks down buildings of another zone there and queues the new lots for growth.
extends ToolBase

## A new dab once the brush moved this share of its radius (zoneTool dabs along the stroke).
const DAB_SPACING := 0.5

var zone := 1
var radius := 0.0
var _last := Vector3.INF


func enter() -> void:
	zone = params.get("zone", 1)
	radius = params.get("radius", DataTables.ui("drawers").zone_radius)
	ctl.hint.emit("", true)


func click(p: Vector3) -> void:
	_last = p
	CityStore.dispatch(&"begin_stroke", ["cmd.place.zone"])
	_dab(p)


## The whole stroke is one undo step.
func release(_p: Vector3) -> void:
	CityStore.dispatch(&"end_stroke")
	_last = Vector3.INF


func drag(p: Vector3) -> void:
	if _last == Vector3.INF or p.distance_to(_last) >= radius * DAB_SPACING:
		_last = p
		_dab(p)


func hover(p: Vector3) -> void:
	ctl.preview.show_disc(p, radius, true)


func exit() -> void:
	CityStore.dispatch(&"end_stroke")
	ctl.preview.clear()


func _dab(p: Vector3) -> void:
	var lot: int = CityStore.query(&"lot_at", [p])
	if lot != 0: # a lot the player drew takes the zone whole
		CityStore.dispatch(&"lot_zone", [lot, zone])
		return
	var r: Dictionary = CityStore.dispatch(&"paint_zone", [p, radius, zone])
	if not r.get("ok", false) and r.get("error", "") != "tools.toast.zoneSame":
		ctl.hint.emit(refusal(r), false)
