## The city on disk: SaveSlots owns where the files live (one per city). The city on screen is written to its own slot (`_city_slot`,
## given at its first save or by the slot it was opened from): Save, Ctrl+S and the autosave every autosave_s of real time while it has
## unsaved work and is the player's (it has a slot or they edited it); and when the window closes or loses the focus. The core writes on
## its own thread; CityStore's `saved` carries the outcome, so the frame never waits for a save.
class_name CitySaves
extends RefCounted

const SAVE_WAIT_MS := 10

var _store: Node
var _core: Node3D
var _since_autosave := 0.0
var _save_pending := false
var _save_slot := ""
var _city_slot := ""
var _export_to := "" # a map copy to make once the save in flight is written
var _save_auto := false # the save in flight is the autosave
## The save after an edit (Roadcraft persistence.ts saveSoon). Tests that edit and look at what is unsaved keep it off.
var after_edit := not GameFlow.tests_drive
var _edit_seen := 0
var _quiet := 0.0 # since the last edit
var _waited := -1.0 # since the first edit not yet saved; < 0: none


func _init(store: Node, core: Node3D) -> void:
	_store = store
	_core = core


## A new map on screen: its first save gives it a slot of its own.
func new_city() -> void:
	_settle_save()
	_city_slot = ""
	_waited = -1.0


## The slot the city on screen is saved to ("" until its first save).
func city_slot() -> String:
	return _city_slot


## A save is being written (its result comes with `saved`).
func save_busy() -> bool:
	return _save_pending


## Save (Menu, Ctrl+S): the city on screen to its own slot. The result is `saved`'s; this returns only a refusal.
func save_city() -> Dictionary:
	if _city_slot.is_empty():
		_city_slot = SaveSlots.free_city_slot(int(_store.snapshot.get("seed", 0)))
	return save_game(_city_slot)


## Save map: the city is saved, and its file is copied to mapa-<seed> once it is written.
func export_map() -> Dictionary:
	var to := SaveSlots.path(SaveSlots.export_slot(int(_store.snapshot.get("seed", 0))))
	var r := save_city()
	if r.get("pending", false):
		_export_to = to
	return r


## Starts writing the city to the slot (a file name without extension, SaveSlots.path) on the core's save thread:
## {pending: true}, then `saved` with the outcome; or a refusal ({ok: false, error}) when one is already writing.
func save_game(slot: String) -> Dictionary:
	if _save_pending:
		return {"ok": false, "error": "save.busy"}
	DirAccess.make_dir_recursive_absolute(SaveSlots.dir())
	_save_slot = slot
	var r: Dictionary = _core.save_city(save_path(slot))
	_since_autosave = 0.0
	_save_pending = r.get("pending", false)
	if not _save_pending:
		_finish_save(slot, r)
	_store.refresh()
	return r


## The core reads a city aside on a thread of its own while the frames go on, then takes it in whole (load_poll).
func _opened(started: Dictionary) -> Dictionary:
	var r := started
	while r.get("pending", false):
		await _store.get_tree().process_frame
		r = _core.load_poll()
	return r


## Replaces the city with the saved one (the core reads it aside, stops the simulation, takes it in and redraws everything). The
## city keeps that slot, unless it is a map copy (then its first save gives it one of its own).
func load_game(slot: String, paused := false) -> Dictionary:
	var r: Dictionary = await _opened(_core.load_city(save_path(slot), paused)) # waits for a save in flight first
	_settle_save()
	_since_autosave = 0.0
	if r.get("ok", false):
		_city_slot = slot if SaveSlots.is_city_slot(slot) else ""
		if not _city_slot.is_empty():
			SaveSlots.remember_last(_city_slot)
		Session.read(slot, _store.get_tree().get_first_node_in_group(&"camera_rig"), paused)
	_store.refresh()
	_store.loaded.emit(slot, r)
	return r


## Load the last game: the city the index names (SaveSlots.last).
func load_last() -> Dictionary:
	var slot := SaveSlots.last()
	if slot.is_empty():
		return {"ok": false, "error": "save.none"}
	return await load_game(slot)


func save_path(slot: String) -> String:
	return SaveSlots.path(slot)


## The window closes, or a new city takes this one's place: what is unsaved is written, and the thread waited for (a city never lost
## to the close button).
func save_on_close() -> void:
	if not _autosave_due():
		return
	save_city()
	while _save_pending and _core.save_status().get("busy", true):
		OS.delay_msec(SAVE_WAIT_MS)
	_settle_save()


## The window went behind another or was minimised: what is unsaved starts writing, the frame does not wait (Roadcraft saves when
## the page is hidden, with no promise it finishes; here the thread does).
func save_on_hide() -> void:
	if _autosave_due():
		_save_auto = true
		save_city()


## A city file from anywhere on the disk: opened as a new city, its first save takes a slot of its own. The core keeps the city on
## screen when the file is no good, so nothing is lost.
func open_file(file: String) -> Dictionary:
	var r: Dictionary = await _opened(_core.load_city(file))
	_settle_save()
	_since_autosave = 0.0
	if r.get("ok", false):
		_city_slot = ""
	_store.refresh()
	return r


## A file the core could not read goes aside, never deleted (Roadcraft quarantineStored); the player starts afresh.
func quarantine(slot: String) -> void:
	SaveSlots.quarantine(slot)


## The saved cities and map copies, newest first: [{slot, modified (unix seconds), size (bytes)}].
func list_saves() -> Array:
	return SaveSlots.list()


## Every frame: a write that ended, and the autosave's clock.
func tick(delta: float) -> void:
	if _save_pending and not _core.save_status().get("busy", true):
		_settle_save()
		_store.refresh()
	_tick_edits(delta)
	_since_autosave += delta
	if _since_autosave >= DataTables.game("saves").autosave_s:
		_since_autosave = 0.0 # checked every autosave_s, saved only when due
		if _autosave_due():
			_save_auto = true
			save_city()


## An edit is written once the player pauses (autosave_quiet_s), or autosave_most_s after it in a run of edits: the file costs a thread
## some hundred ms and the frame nothing (inspector_test measures it), so a crash loses seconds, as in the reference.
func _tick_edits(delta: float) -> void:
	var edit: int = _store.snapshot.get("edit_revision", 0)
	if edit != _edit_seen:
		_edit_seen = edit
		_quiet = 0.0
		_waited = maxf(_waited, 0.0)
	if not after_edit or _waited < 0.0:
		return
	_quiet += delta
	_waited += delta
	var cfg: Dictionary = DataTables.game("saves")
	if _quiet >= cfg.autosave_quiet_s or _waited >= cfg.autosave_most_s:
		_waited = -1.0
		if _autosave_due():
			_save_auto = true
			save_city()


## Unsaved work of the player's: a city never saved nor edited is not written.
func _autosave_due() -> bool:
	var s: Dictionary = _store.snapshot
	var players: bool = not _city_slot.is_empty() or s.get("edited", false)
	return s.get("ready", false) and s.get("unsaved", false) and not _save_pending and players


## The save thread finished (or the core waited for it before a new map or a load): its outcome goes out.
func _settle_save() -> void:
	if not _save_pending:
		return
	_save_pending = false
	var st: Dictionary = _core.save_status()
	_finish_save(_save_slot, {"ok": st.get("ok", false), "error": st.get("error", ""), "bytes": st.get("bytes", 0)})


## A save that ended: the city's own slot becomes the last city, a map copy asked for is made; listeners get the
## result (`auto` for the autosave).
func _finish_save(slot: String, result: Dictionary) -> void:
	if result.get("ok", false) and slot == _city_slot:
		SaveSlots.remember_last(slot)
		Session.write(slot, _store.get_tree().get_first_node_in_group(&"camera_rig"))
	if result.get("ok", false) and not _export_to.is_empty():
		if DirAccess.copy_absolute(save_path(slot), _export_to) != OK:
			result = {"ok": false, "error": "save.writeFailed"}
	result["auto"] = _save_auto
	_export_to = ""
	_save_auto = false
	_store.saved.emit(slot, result)
