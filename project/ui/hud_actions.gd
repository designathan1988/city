## What the Menu rows, the undo / redo buttons and the Ctrl shortcuts do: new map,
## save the city (its own slot), save a copy of the map, open a saved city, load the last game, undo and redo.
## Everything goes through CityStore; the hint bar reports the result.
class_name HudActions
extends Node

const MS_PER_S := 1000.0
## What asks first (Roadcraft window.confirm): the map on screen is saved, then the new one takes its place.
const CONFIRM := {"new_empty": "confirm.newMap", "new_city": "confirm.newWorld", "new_large": "confirm.newLargeWorld"}

var _failed_at := -INF # when a failed save last said so

## The HUD this node is a child of.
func _hud() -> RoadcraftHud:
	return get_parent() as RoadcraftHud


func _ready() -> void:
	CityStore.saved.connect(_on_saved)


func menu(action: String) -> void:
	match action:
		"new_empty", "new_city", "new_large":
			_hud().open_dialog(HudDialog.Confirm.new(tr(CONFIRM[action]), new_city.bind(action)))
		"save":
			_refused(CityStore.saves.save_city())
		"save_map":
			_refused(CityStore.saves.export_map())
		"load_last":
			_loaded(CityStore.saves.load_last())
		"open":
			_hud().open_popover("files")
		"open_file":
			_pick_file()
		"graphics":
			_hud().open_dialog(GraphicsPanel.new())
		"effects":
			_hud().toggle_effects()
		"about":
			_hud().open_dialog(HudDialog.About.new())


## The confirmed new map: what the player had is written first, as the question says.
func new_city(action: String) -> void:
	var map := _large_map() if action == "new_large" else DataTables.map_index("town")
	open_new(map, randi_range(1, int(DataTables.game("start").seed_max)), action != "new_empty")
	if action == "new_empty":
		_hud().flash(tr("hint.newMap"), true)


## The city the title screen's form asks for.
func new_city_at(map: int, seed: int) -> void:
	open_new(map, seed, true)


## A new city on screen, as the start screen's form opens one: the city standing is saved first.
func open_new(map: int, seed: int, with_city: bool) -> void:
	CityStore.saves.save_on_close()
	_hud().drawer.close()
	CityStore.new_map(map, seed, with_city)


## Where a map of that id stands in the catalogue; -1: the table does not have it.
static func map_index(id: String) -> int:
	var maps: Array = DataTables.maps()
	for i in maps.size():
		if maps[i].id == id:
			return i
	return -1


## The first large map the table has.
func _large_map() -> int:
	for id: String in DataTables.ui("drawers").popovers.menu.large_maps:
		if map_index(id) >= 0:
			return map_index(id)
	return DataTables.map_index("town")


## Open a file from anywhere: the system's own dialog.
func _pick_file() -> void:
	var d := FileDialog.new()
	d.use_native_dialog = true
	d.access = FileDialog.ACCESS_FILESYSTEM
	d.file_mode = FileDialog.FILE_MODE_OPEN_FILE
	d.add_filter("*.%s" % SaveSlots.EXT, tr("action.openMap"))
	d.file_selected.connect(open_file)
	d.file_selected.connect(d.queue_free.unbind(1))
	d.canceled.connect(d.queue_free)
	add_child(d)
	d.popup_centered()


## A city file: opened, or nothing changed and the player told so.
func open_file(file: String) -> void:
	var r := CityStore.saves.open_file(file)
	_hud().drawer.close()
	_hud().flash(tr("hint.opened") if r.get("ok", false) else tr("hint.openFailed"), r.get("ok", false))


## The warning in the bar: the first node that cannot be built, shown in the Inspect tool (Roadcraft showImpossible).
func show_impossible() -> void:
	var bad: Array = CityStore.query(&"impossible_nodes")
	var hud := _hud()
	if bad.is_empty():
		return
	hud.drawer.close()
	if hud.rig:
		hud.rig.jump(bad[0].position, rad_to_deg(hud.rig.yaw), rad_to_deg(hud.rig.pitch), hud.rig.distance)
	hud.inspector.show_info({"kind": "node", "id": bad[0].id}, bad[0].position)


func load_slot(slot: String) -> void:
	_loaded(CityStore.saves.load_game(slot))


func _loaded(r: Dictionary) -> void:
	_hud().drawer.close()
	_hud().flash(tr("save.loaded") if r.ok else tr(r.error), r.ok)


## A save the store could not start (one is still being written, no whole city): said at once.
func _refused(r: Dictionary) -> void:
	if not r.get("ok", false):
		_hud().flash(tr(r.get("error", "")), false)


## A save that ended (CityStore.saved): written, written by the autosave, or why not.
func _on_saved(_slot: String, r: Dictionary) -> void:
	if r.get("ok", false):
		_hud().flash(tr("save.auto" if r.get("auto", false) else "save.done"), true)
		return
	var now := Time.get_ticks_msec() / MS_PER_S
	if r.get("auto", false) and now - _failed_at < DataTables.game("saves").failed_hint_s:
		return
	_failed_at = now
	_hud().flash(tr("save.failed").format({"message": tr(r.get("error", ""))}), false)


## Undo / redo (top bar buttons, Ctrl+Z, Ctrl+Shift+Z / Ctrl+Y): the core's history; the hint names the step.
func history(action: String) -> void:
	var label: String = CityStore.snapshot.get("undo_label" if action == "undo" else "redo_label", "")
	if CityStore.dispatch(StringName(action)):
		_hud().flash(tr("v2.do." + action) + " · " + tr(label), true)
