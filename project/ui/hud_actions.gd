## What the Menu rows, the undo / redo buttons and the Ctrl shortcuts do (Roadcraft _hud().ts §7.1, keys §10): new map,
## save the city (its own slot), save a copy of the map, open a saved city, load the last game, undo and redo.
## Everything goes through CityStore; the hint bar reports the result.
class_name HudActions
extends Node

const TOWN_MAP := 0
const SEED_LIMIT := 9999

## The HUD this node is a child of.
func _hud() -> RoadcraftHud:
	return get_parent() as RoadcraftHud


func _ready() -> void:
	CityStore.saved.connect(_on_saved)


func menu(action: String) -> void:
	match action:
		"new_empty", "new_city":
			_hud().drawer.close()
			CityStore.new_map(TOWN_MAP, randi_range(1, SEED_LIMIT), action == "new_city")
		"save":
			_refused(CityStore.save_city())
		"save_map":
			_refused(CityStore.export_map())
		"load_last":
			_loaded(CityStore.load_last())
		"open":
			_hud().open_popover("files")


func load_slot(slot: String) -> void:
	_loaded(CityStore.load_game(slot))


func _loaded(r: Dictionary) -> void:
	_hud().drawer.close()
	_hud().show_hint(tr("save.loaded") if r.ok else tr(r.error), r.ok)


## A save the store could not start (one is still being written, no whole city): said at once.
func _refused(r: Dictionary) -> void:
	if not r.get("ok", false):
		_hud().show_hint(tr(r.get("error", "")), false)


## A save that ended (CityStore.saved): written, written by the autosave, or why not.
func _on_saved(_slot: String, r: Dictionary) -> void:
	if r.get("ok", false):
		_hud().show_hint(tr("save.auto" if r.get("auto", false) else "save.done"), true)
	else:
		_hud().show_hint(tr(r.get("error", "")), false)


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
