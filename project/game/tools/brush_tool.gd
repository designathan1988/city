## Base of the Landscape brushes that lay dabs along a held stroke (ground paint, trees, fog): one undo step a stroke,
## dabs spaced by a share of the radius, the disc shown under the pointer. The dabs of one pointer move go to the core as
## one command; a subclass says which (`_dab_path`) and names its undo step (`_label`).
class_name BrushTool
extends ToolBase

## Between dabs along a stroke, and at most how many at once (data/game/tools.json brush).
static var step_share: float = DataTables.game("tools").brush.step_share
static var step_least_m: float = DataTables.game("tools").brush.step_least_m
static var most_steps: int = DataTables.game("tools").brush.most_steps

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
	var step := maxf(step_least_m, radius * step_share)
	var gap := _last.distance_to(p)
	if gap < step:
		return
	var steps := mini(most_steps, maxi(1, roundi(gap / step)))
	var dabs := PackedVector3Array()
	for i in range(1, steps + 1):
		dabs.append(_last.lerp(p, float(i) / steps))
	_lay(dabs)
	_last = p


func release(_p: Vector3) -> void:
	_last = Vector3.INF
	CityStore.dispatch(&"end_stroke")


func _lay(dabs: PackedVector3Array) -> void:
	var r: Dictionary = _dab_path(dabs)
	if not r.get("error", "").is_empty(): # a dab refused (the last refusal: those before it were shown and replaced)
		ctl.hint.emit(refusal(r), false)


## The dabs in order, as one command; its result as lay_dabs sums it (sim/commands/commands.h).
func _dab_path(_dabs: PackedVector3Array) -> Dictionary:
	return {}


func _label() -> String:
	return ""
