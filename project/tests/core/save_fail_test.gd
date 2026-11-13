## Core test of leaving with a save that fails (sheet backbone-errors-seen, rule 9 and its save case): the save on close
## (CitySaves.write_before_leaving, what closing the window and opening a new map run first) answers the settled outcome. A city
## with nothing due answers ok; a write the core cannot make (a folder stands at the temporary file's place) answers
## save.writeFailed through `saved` (what HudActions shows) and in the result, with the previous file byte for byte, the city
## still ready at the edit it had and still unsaved; once the folder is gone the same call writes it. The close that quits
## unsaved still waits for a save in flight (settle_in_flight).
## Run: godot --headless --path project res://tests/core/save_fail_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const PROFILE := 0
const OUT_M := 260.0
const SECOND_OFF := Vector3(40, 0, 90) # clear of the first road: no generated road end beside it
const SNAP := 0.25
const TMP_EXT := ".tmp" # write_atomic's temporary file beside the target (city_core_persist.cpp)
const OLD_EXT := ".old" # the previous generation set aside while the new one is renamed in
const WRITE_FAILED := "save.writeFailed"


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	await _close_after_failed_write()
	finish()


func _close_after_failed_write() -> void:
	var none: Dictionary = CityStore.saves.write_before_leaving()
	check(none.get("ok", false), "a city never edited nor saved: nothing due, the close goes on %s" % none)
	var b: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(b.max.x - 1.0, 0, (b.min.z + b.max.z) * 0.5)
	if not _road(a, "the first edit"):
		return
	var first: Dictionary = await save()
	if not check(first.get("ok", false), "the first save written %s" % first):
		return
	var file := SaveSlots.path(CityStore.saves.city_slot())
	var bytes := FileAccess.get_file_as_bytes(file)
	var had_old := FileAccess.file_exists(file + OLD_EXT)
	check(not bytes.is_empty(), "the first save's file read back (%d bytes)" % bytes.size())
	var tmp := file + TMP_EXT
	if not check(DirAccess.make_dir_recursive_absolute(tmp) == OK, "a folder stands at the temporary file's place %s" % tmp):
		return
	if not _road(a + SECOND_OFF, "the second edit"):
		return
	var edit: int = CityStore.snapshot.get("edit_revision", -1)
	var heard := {}
	var hear := func(_slot: String, result: Dictionary) -> void: heard.merge(result, true)
	CityStore.saved.connect(hear)
	var r: Dictionary = CityStore.saves.write_before_leaving()
	CityStore.saved.disconnect(hear)
	check(not r.get("ok", true) and r.get("error", "") == WRITE_FAILED, "the save on close refused: %s" % r)
	check(not heard.get("ok", true) and heard.get("error", "") == WRITE_FAILED, "`saved` told the failure (HudActions shows it) %s" % heard)
	check(FileAccess.get_file_as_bytes(file) == bytes, "the previous save is byte for byte the first one")
	check(FileAccess.file_exists(file + OLD_EXT) == had_old, "the old copy as it was (%s)" % had_old)
	var s: Dictionary = CityStore.snapshot
	check(s.get("ready", false) and s.get("edit_revision", -1) == edit, "the city still ready at its edit %d: %s" % [edit, s.get("edit_revision", -1)])
	check(s.get("unsaved", false), "the second edit is still unsaved")
	check(not CityStore.saves.save_busy(), "no save left in flight")
	if not check(DirAccess.remove_absolute(tmp) == OK, "the test's folder removed"):
		return
	var again: Dictionary = CityStore.saves.write_before_leaving()
	check(again.get("ok", false), "with the folder gone the save on close writes %s" % again)
	check(not CityStore.snapshot.get("unsaved", true), "nothing left unsaved")
	check(FileAccess.get_file_as_bytes(file) != bytes, "the file now holds the second edit")
	var after: Dictionary = CityStore.saves.write_before_leaving()
	check(after.get("ok", false) and after.size() == 1, "nothing due again: {ok: true} %s" % after)
	_close_unsaved_waits()


## The second close quits unsaved, but a save started since (save_on_hide) ends first (settle_in_flight): its outcome comes back
## and nothing is left writing; with none in flight, nothing to wait for.
func _close_unsaved_waits() -> void:
	var started: Dictionary = CityStore.saves.save_city()
	if not check(started.get("pending", false), "a save started on the core's thread %s" % started):
		return
	var flight: Dictionary = CityStore.saves.settle_in_flight()
	check(flight.get("ok", false), "the save in flight waited for, written %s" % flight)
	check(not CityStore.saves.save_busy(), "nothing left writing once it is settled")
	var none: Dictionary = CityStore.saves.settle_in_flight()
	check(none.is_empty(), "no save in flight: nothing waited for %s" % none)


## A road out of `from` (clear ground east of the city): false, and a failure, when the core refuses it.
func _road(from: Vector3, what: String) -> bool:
	var r: Dictionary = CityStore.dispatch(&"build_road", [from, from + Vector3(OUT_M, 0, 0), false, Vector3.ZERO, PROFILE, SNAP])
	return check(r.get("ok", false), "%s: %s" % [what, r])
