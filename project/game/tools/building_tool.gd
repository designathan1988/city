## Build (Massa, Fachada, Telhado, Cores): a click picks a block, its face and its bay; the strip's tools then act on the pick. With a
## component in hand a click puts it in the wall, at the scope chosen (the zone scope: the region between two clicks, FacadeZone).
## Holds what is picked, never a number of the game: each edit goes to the core, which answers, and undo is the game's own. A model in
## hand (ModelHand: the Models shelf, or the clipboard) follows the pointer as a ghost and a click sets it down. A click on a free part
## or a thing on a roof picks it; the Builder's keys act on the pick (PickKeys).
class_name BuildingTool
extends ToolBase

const NO_BAY := -1
## What a click on a roof does with the card in hand: a thing set on it, or the flag of the mast clicked.
const ROOF_TOOLS := ["gear", "flag", "flag_colour"]

## What the last click hit (CityStore query "body_pick"), {} for nothing.
var picked := {}
## The component a click puts in the wall, NO_BAY for none.
var armed := NO_BAY
## The part a click sets (a spec of data/ui/builder.json), {} for none.
var part := {}
var scope := 0
var paint_scope := 0
## Which of a flag's three colours the next colour card sets.
var flag_slot := 0

var hands: BuildHands
var handles: HandleDrag
var model: ModelHand
## The model in hand (ModelHand's): {bytes, function, zone}, {} for none.
var hand: Dictionary:
	get:
		return model.hand if model != null else {}

var _box: SelectionBox


func enter() -> void:
	hands = BuildHands.new(ctl)
	handles = HandleDrag.new(ctl)
	model = ModelHand.new(ctl)
	_box = SelectionBox.new()
	ctl.add_child(_box)
	picked = BuildPick.standing()
	show_pick()


func exit() -> void:
	model.put_down()
	hands.disarm()
	handles.cancel()
	handles.marks.get_parent().queue_free()
	hands.retire()
	_box.queue_free()


## A body in hand (the Models shelf, the clipboard): its ghost shows from the next pointer move.
func take(entry: Dictionary) -> void:
	model.take(entry)


func hover(p: Vector3) -> void:
	hands.run.hover(p)
	hands.furnish.hover(picked)
	handles.ghost_in_hand(part if not part.is_empty() and part.do == "part" else hands.primitive)
	model.hover(p)


func key(code: Key) -> void:
	hands.key(code, picked, _report)
	if model.key(code):
		return
	var done := PickKeys.run(code, picked, paint_scope) if not model.holding() and not hands.armed() else {}
	if done.has("take"): # Ctrl+V: the building copied, in hand
		model.take(done.take)
	elif not done.is_empty(): # a key of the Builder on the pick: what it did is told, the pick follows it
		picked = done.picked
		_report(done.result)
		show_pick()


func keeps_key(code: Key) -> bool: # Page Up and Page Down: a storey of the block picked (PickKeys), not the camera's, while nothing is in hand
	return (code == KEY_PAGEUP or code == KEY_PAGEDOWN) and not model.holding() and not hands.armed() and not picked.is_empty()


func click(p: Vector3) -> void:
	if model.holding():
		var r := model.set_down(p)
		_report(r)
		if r.get("ok", false):
			picked = PickKeys.new_pick(r.placed)
			show_pick()
			ctl.hint.emit(tr("building.placed"), true)
		return
	if hands.click(p, picked, _report):
		return
	if armed == NO_BAY and part.is_empty() and handles.press():
		return
	var cam: Camera3D = ctl.rig.camera
	var had := picked
	pick(cam.project_ray_origin(ctl.pointer), cam.project_ray_normal(ctl.pointer))
	if not part.is_empty() and part.do == "part" and picked.is_empty() and not had.is_empty():
		picked = had
		_report(CityStore.dispatch(&"body_add_part_at", [had.id, p, part]))
		show_pick()


func drag(p: Vector3) -> void:
	hands.mass.move(p)
	handles.move(Input.is_key_pressed(KEY_SHIFT))


func release(p: Vector3) -> void:
	if hands.mass.dragging():
		_report(hands.mass.finish(p))
		show_pick()
	elif handles.dragging():
		_report(handles.finish())
		if handles.building() != picked.id:
			picked = PickKeys.new_pick(handles.building())
		show_pick()


## What the ray hits becomes the pick; with a component in hand and a wall hit, the component goes there.
func pick(origin: Vector3, dir: Vector3) -> void:
	var hit: Dictionary = CityStore.query(&"body_pick", [origin, dir])
	picked = hit
	if not hit.is_empty() and hands.brush.on():
		_report(hands.brush.paint(hit, paint_scope))
	elif not hit.is_empty() and hit.side < 0 and hit.part < 0 and not part.is_empty() and ROOF_TOOLS.has(part.do):
		_report(_roof(hit))
	elif hit.is_empty() or hit.side < 0:
		pass
	elif armed != NO_BAY:
		_report(FacadeZone.put(hit, armed, scope))
	elif not part.is_empty() and part.do == "part":
		_report(CityStore.dispatch(&"body_add_part", [hit.id, hit.volume, hit.storey, hit.side, hit.bay, part]))
	show_pick()


## The roof clicked, with a card in hand: the thing lands at the point, or the mast there gets the flag or the colour.
func _roof(hit: Dictionary) -> Dictionary:
	match part.do:
		"gear":
			return CityStore.dispatch(&"body_add_gear", [hit.id, hit.volume, hit.point, part])
		"flag":
			return CityStore.dispatch(&"body_set_flag", [hit.id, hit.volume, hit.point, part.value])
		"flag_colour":
			return CityStore.dispatch(&"body_flag_colour", [hit.id, hit.volume, hit.point, flag_slot, part.rgb])
	return {"ok": false, "error": "tool.failed"}


## What Escape steps back first: the body in hand, the bay armed or the part held (the pick itself goes with the tool).
func in_progress() -> bool:
	return model.holding() or armed != NO_BAY or not part.is_empty() or hands.armed()


func cancel() -> void:
	if model.holding():
		model.put_down()
	elif hands.armed():
		hands.disarm()
	elif armed != NO_BAY or not part.is_empty():
		armed = NO_BAY
		part = {}
		FacadeZone.forget()
	else:
		picked = {}
	show_pick()


## A tool of the strip (data/ui/builder.json): `spec.do` names what it does, `spec.value` its choice.
func act(spec: Dictionary) -> void:
	var value: int = spec.get("value", 0)
	handles.preview(-1, {})
	if hands.act(spec, value, picked, _report):
		armed = NO_BAY
		part = {}
		return
	match spec.do:
		"arm":
			armed = NO_BAY if armed == value else value
			part = {}
		"part", "gear":
			part = {} if part == spec else spec
			armed = NO_BAY
		"flag", "flag_colour":
			part = {} if part == spec else spec
			armed = NO_BAY
			if not part.is_empty():
				ctl.hint.emit(tr("hint.builder.flag"), true)
				return
		"flag_slot":
			flag_slot = value
		"scope":
			scope = value
			FacadeZone.forget()
		"paint_scope":
			paint_scope = value
		"inside", "floor":
			hands.furnish.inside(spec.do, value, picked)
			return
		"points", "point_add", "point_remove", "bevel_corner":
			_report(handles.act(spec, value, picked))
		_:
			if picked.is_empty():
				ctl.hint.emit(tr("hint.builder.select"), false)
				return
			var r := BodyEdits.run(spec, value, picked, paint_scope)
			if r.get("ok", false) and (spec.do == "remove" or spec.do == "duplicate"): # the block gone, or the copy picked
				picked = {} if spec.do == "remove" else PickKeys.new_pick(r.placed)
			_report(r)
	show_pick()


func _report(r: Dictionary) -> void:
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)


## The box and handles again: after an edit of the tool's or of the Selection panel's (BuilderInspector). The pick is the drawer's
## (BuildPick): the Draw tab's targets and the next Build tab act on it.
func show_pick() -> void:
	BuildPick.current = picked
	handles.refresh(picked)
	if picked.is_empty():
		_box.clear()
		ctl.hint.emit(tr("hint.builder.select"), true)
		return
	_box.show_block(CityStore.query(&"body_outline", [picked.id, picked.volume]))
