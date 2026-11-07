## Zone brush: click or drag to paint the chosen zone on the road sides under
## the brush; the core knocks down buildings of another zone there and queues the new lots for growth.
extends ToolBase

## A new dab once the brush moved this share of its radius (zoneTool dabs along the stroke).
const DAB_SPACING := 0.5
## A fast drag reports the pointer far apart: the whole way is painted, up to this many dabs at a report (lotTool.ts move),
## sent as one command (paint_zone_path).
const MOST_DABS := 400
## A side the brush passes that already has the zone says nothing.
const ZONE_SAME := "tools.toast.zoneSame"
## What the brush paints with Shift held: no zone (lotTool.ts `remove`).
const UNZONED := 0

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
	_painting = UNZONED if Input.is_key_pressed(KEY_SHIFT) else zone
	CityStore.dispatch(&"begin_stroke", ["cmd.place.zone"])
	_paint(PackedVector3Array([p]))


## The whole stroke is one undo step.
func release(_p: Vector3) -> void:
	CityStore.dispatch(&"end_stroke")
	_last = Vector3.INF


func drag(p: Vector3) -> void:
	if _last == Vector3.INF:
		_last = p
		_paint(PackedVector3Array([p]))
		return
	var spacing := radius * DAB_SPACING
	var far := p.distance_to(_last)
	if far < spacing:
		return
	var steps := mini(ceili(far / spacing), MOST_DABS)
	var dabs := PackedVector3Array()
	for k in range(1, steps + 1):
		dabs.append(_last.lerp(p, float(k) / steps))
	_paint(dabs)
	_last = p


func hover(p: Vector3) -> void:
	ctl.preview.show_disc(p, radius, true)


func exit() -> void:
	CityStore.dispatch(&"end_stroke")
	ctl.preview.clear()


## The dabs of one pointer move as one command: a lot the player drew takes the zone whole, elsewhere the road sides.
func _paint(dabs: PackedVector3Array) -> void:
	var r: Dictionary = CityStore.dispatch(&"paint_zone_path", [dabs, radius, _painting])
	var why: String = r.get("error", "")
	if not why.is_empty() and why != ZONE_SAME:
		ctl.hint.emit(refusal(r), false)
