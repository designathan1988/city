## Build (Massa, Fachada, Telhado, Cores): a click picks a block, its face and its bay; the strip's tools then act on the pick. With a
## component in hand a click puts it in the wall, at the scope chosen. Holds what is picked, never a number of the game: each edit
## goes to the core, which answers, and undo is the game's own.
class_name BuildingTool
extends ToolBase

const NO_BAY := -1

## What the last click hit (CityStore query "body_pick"), {} for nothing.
var picked := {}
## The component a click puts in the wall, NO_BAY for none.
var armed := NO_BAY
## The part a click sets (a spec of data/ui/builder.json), {} for none.
var part := {}
var scope := 0
var paint_scope := 0

var _box: SelectionBox


func enter() -> void:
	_box = SelectionBox.new()
	ctl.add_child(_box)
	ctl.hint.emit(tr("hint.builder.select"), true)


func exit() -> void:
	_box.queue_free()


func click(p: Vector3) -> void:
	var cam: Camera3D = ctl.rig.camera
	var screen := ctl.get_viewport().get_mouse_position()
	var had := picked
	pick(cam.project_ray_origin(screen), cam.project_ray_normal(screen))
	if not part.is_empty() and part.do == "part" and picked.is_empty() and not had.is_empty():
		picked = had
		_report(CityStore.dispatch(&"body_add_part_at", [had.id, p, part]))
		_show()


## What the ray hits becomes the pick; with a component in hand and a wall hit, the component goes there.
func pick(origin: Vector3, dir: Vector3) -> void:
	var hit: Dictionary = CityStore.query(&"body_pick", [origin, dir])
	picked = hit
	if not hit.is_empty() and hit.side < 0 and not part.is_empty() and part.do == "gear":
		_report(CityStore.dispatch(&"body_add_gear", [hit.id, hit.volume, hit.point, part]))
	elif hit.is_empty() or hit.side < 0:
		pass
	elif armed != NO_BAY:
		_report(CityStore.dispatch(&"body_set_bay", [hit.id, hit.volume, hit.storey, hit.side, hit.bay, armed, scope]))
	elif not part.is_empty() and part.do == "part":
		_report(CityStore.dispatch(&"body_add_part", [hit.id, hit.volume, hit.storey, hit.side, hit.bay, part]))
	_show()


func cancel() -> void:
	if armed != NO_BAY or not part.is_empty():
		armed = NO_BAY
		part = {}
	else:
		picked = {}
	_show()


## A tool of the strip (data/ui/builder.json): `spec.do` names what it does, `spec.value` its choice.
func act(spec: Dictionary) -> void:
	var value: int = spec.get("value", 0)
	match spec.do:
		"arm":
			armed = NO_BAY if armed == value else value
			part = {}
		"part", "gear":
			part = {} if part == spec else spec
			armed = NO_BAY
		"scope":
			scope = value
		"paint_scope":
			paint_scope = value
		_:
			if picked.is_empty():
				ctl.hint.emit(tr("hint.builder.select"), false)
				return
			_report(_edit(spec, value))
	_show()


func _edit(spec: Dictionary, value: int) -> Dictionary:
	var id: int = picked.id
	var vol: int = picked.volume
	match spec.do:
		"storeys":
			var v: Dictionary = CityStore.query(&"body_info", [id]).volumes[vol]
			return CityStore.dispatch(&"body_storeys", [id, vol, v.storeys + value])
		"split":
			return CityStore.dispatch(&"body_split", [id, vol, picked.level])
		"setback":
			return CityStore.dispatch(&"body_add_setback", [id, vol, -1.0, 2])
		"wing":
			return CityStore.dispatch(&"body_add_wing", [id, vol, maxi(picked.side, 0), 0.0, 0.0])
		"remove":
			var r: Dictionary = CityStore.dispatch(&"body_remove_volume", [id, vol])
			if r.get("ok", false):
				picked = {}
			return r
		"pattern":
			return CityStore.dispatch(&"body_set_pattern", [id, vol, -1, -1, value])
		"roof":
			return CityStore.dispatch(&"body_set_roof", [id, vol, value])
		"finish":
			return CityStore.dispatch(&"body_paint", [id, paint_scope, vol, maxi(picked.side, 0), picked.storey, value, spec.rgb])
	return {"ok": false, "error": "tool.failed"}


func _report(r: Dictionary) -> void:
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)


func _show() -> void:
	if picked.is_empty():
		_box.clear()
		ctl.hint.emit(tr("hint.builder.select"), true)
		return
	_box.show_block(CityStore.query(&"body_outline", [picked.id, picked.volume]))
