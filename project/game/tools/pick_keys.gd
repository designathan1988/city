## The Build tool's keys on what is picked (the Roadcraft's Builder): R turns the free part or the roof thing picked, or else the whole
## building (Shift: a small step), Delete takes it away (or, with none, the block), Page Up and Page Down add and take a storey, Ctrl+D
## copies the building beside itself, Ctrl+C keeps it and Ctrl+V puts it in hand to set down again. Each edit goes to the core; the
## answer says what was done and what is picked after it.
class_name PickKeys
extends RefCounted

## {result, picked} for a key that edited something, {take} for Ctrl+V with a building kept, {} for a key that is not one of these.
static func run(code: Key, picked: Dictionary, paint_scope: int) -> Dictionary:
	var ctrl := Input.is_key_pressed(KEY_CTRL)
	if ctrl and code == KEY_V:
		return {} if ModelHand.clipboard.is_empty() else {"take": ModelHand.clipboard}
	if picked.is_empty():
		return {}
	if ctrl and code == KEY_C:
		ModelHand.copy(picked)
		return {"result": {"ok": true}, "picked": picked}
	var part: int = picked.get("part", -1)
	var gear: int = picked.get("gear", -1)
	var r := {}
	var after := picked
	match code:
		KEY_R:
			if part >= 0:
				r = CityStore.dispatch(&"body_turn_part", [picked.id, part])
			elif gear >= 0:
				r = CityStore.dispatch(&"body_turn_gear", [picked.id, picked.volume, gear])
			else:
				var step := deg_to_rad(DataTables.tuning("build_gear_turn_deg")) if Input.is_key_pressed(KEY_SHIFT) else ModelHand.plain_turn
				r = CityStore.dispatch(&"building_turn", [picked.id, step])
		KEY_DELETE:
			if part >= 0:
				r = CityStore.dispatch(&"body_remove_part", [picked.id, part])
			elif gear >= 0:
				r = CityStore.dispatch(&"body_remove_gear", [picked.id, picked.volume, gear])
			else:
				r = BodyEdits.run({"do": "remove"}, 0, picked, paint_scope)
			if r.get("ok", false):
				after = {} if part < 0 and gear < 0 else block_of(picked)
		KEY_PAGEUP, KEY_PAGEDOWN:
			r = BodyEdits.run({"do": "storeys"}, 1 if code == KEY_PAGEUP else -1, picked, paint_scope)
		KEY_D:
			if ctrl:
				r = BodyEdits.run({"do": "duplicate"}, 0, picked, paint_scope)
				if r.get("ok", false):
					after = new_pick(r.placed)
	return {} if r.is_empty() else {"result": r, "picked": after}


## The block a part or a roof thing was picked with, the thing let go.
static func block_of(was: Dictionary) -> Dictionary:
	var block := was.duplicate()
	block.part = -1
	block.gear = -1
	return block


## A building just made, picked whole: its first block, no face.
static func new_pick(id: int) -> Dictionary:
	return {"id": id, "volume": 0, "side": -1, "storey": 0, "level": 0, "bay": BuildingTool.NO_BAY, "part": -1, "gear": -1}
