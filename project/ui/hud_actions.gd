## What the Menu rows, the undo / redo buttons and the Ctrl shortcuts do (Roadcraft _hud().ts §7.1, keys §10): new map,
## save to the game's slot or to a file named after the seed, open a saved city, load the last game, undo and redo.
## Everything goes through CityStore; the hint bar reports the result.
class_name HudActions
extends Node

const TOWN_MAP := 0
const SEED_LIMIT := 9999
## Roadcraft exportMap names the file after the seed (cidade-<seed>).
const SAVE_MAP_SLOT := "cidade-%d"

## The HUD this node is a child of.
func _hud() -> RoadcraftHud:
	return get_parent() as RoadcraftHud


func menu(action: String) -> void:
	match action:
		"new_empty", "new_city":
			_hud().drawer.close()
			CityStore.new_map(TOWN_MAP, randi_range(1, SEED_LIMIT), action == "new_city")
		"save":
			_saved(CityStore.save_game())
		"save_map":
			_saved(CityStore.save_game(SAVE_MAP_SLOT % CityStore.snapshot.get("seed", 0)))
		"load_last":
			load_slot(CityStore.GAME_SLOT)
		"open":
			_hud().open_popover("files")


func load_slot(slot: String) -> void:
	_hud().drawer.close()
	var r := CityStore.load_game(slot)
	_hud().show_hint(tr("save.loaded") if r.ok else tr(r.error), r.ok)


func _saved(r: Dictionary) -> void:
	_hud().show_hint(tr("save.done") if r.ok else tr(r.error), r.ok)


## Undo / redo (top bar buttons, Ctrl+Z, Ctrl+Shift+Z / Ctrl+Y): the core's history; the hint names the step.
func history(action: String) -> void:
	var label: String = CityStore.snapshot.get("undo_label" if action == "undo" else "redo_label", "")
	if CityStore.dispatch(StringName(action)):
		_hud().show_hint(tr("v2.do." + action) + " · " + tr(label), true)


func _unhandled_key_input(event: InputEvent) -> void:
	var k := event as InputEventKey
	if k == null or not k.pressed or k.echo or not k.ctrl_pressed:
		return
	match k.keycode:
		KEY_Z: history("redo" if k.shift_pressed else "undo")
		KEY_Y: history("redo")
		KEY_S: menu("save")
		KEY_O: _hud().open_popover("files")
		_: return
	get_viewport().set_input_as_handled()
