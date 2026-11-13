## Base of the Landscape brushes that lay dabs along a held stroke (ground paint, trees, fog): one undo step a stroke,
## dabs spaced by a share of the radius, the disc shown under the pointer. The dabs of one pointer move go to the core as
## one command; a subclass says which (`_dab_path`) and names its undo step (`_label`).
class_name BrushTool
extends ToolBase

## Between dabs along a stroke, as a share of the radius, and never closer than this many metres.
const STEP_SHARE := 0.2
const MIN_STEP_M := 1.6
## A pointer coming back from far away lays at most this many dabs at once.
const MAX_STEPS := 12
const PERCENT := 100.0

var radius := 0.0
var _last := Vector3.INF


func settings(tool_name: String) -> Dictionary:
	return BrushDefaults.of(tool_name, params)


func exit() -> void:
	CityStore.dispatch(&"end_stroke")
	ctl.preview.clear()


func hover(p: Vector3) -> void:
	ctl.preview.show_disc(p, radius, true)


func click(p: Vector3) -> void:
	_last = p
	CityStore.dispatch(&"begin_stroke", [_label()])
	_lay(PackedVector3Array([p]))


func drag(p: Vector3) -> void:
	ctl.preview.show_disc(p, radius, true)
	if _last == Vector3.INF:
		return
	var step := maxf(MIN_STEP_M, radius * STEP_SHARE)
	var gap := _last.distance_to(p)
	if gap < step:
		return
	var steps := mini(MAX_STEPS, maxi(1, roundi(gap / step)))
	var dabs := PackedVector3Array()
	for i in range(1, steps + 1):
		dabs.append(_last.lerp(p, float(i) / steps))
	_lay(dabs)
	_last = p


func release(_p: Vector3) -> void:
	_last = Vector3.INF
	CityStore.dispatch(&"end_stroke")


## A move refused, whole or in part (some dabs laid), is told by the store's channel (CityStore.command_failed); nothing here.
func _lay(dabs: PackedVector3Array) -> void:
	_dab_path(dabs)


## The dabs in order, as one command; its result as lay_dabs sums it (sim/commands/commands.h).
func _dab_path(_dabs: PackedVector3Array) -> Dictionary:
	return {}


func _label() -> String:
	return ""
