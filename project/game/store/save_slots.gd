## Where cities are saved (one save per city, so saving a city never overwrites
## another): the city on screen is written to a slot of its own, cidade-<seed> (cidade-<seed>-2... when another city
## holds that name); "Save map" copies it to mapa-<seed>, a copy that opens as a new city; the index names the
## player's last city (what "Load the last game" opens). Runs driven by tests (GameFlow.tests_drive) keep their files
## in a folder of their own process, so they never touch the player's saves nor each other's.
class_name SaveSlots
extends RefCounted

const SAVES_DIR := "user://saves"
const TEST_SAVES_DIR := "user://saves_tests"
const EXT := "cidade"
const CITY_SLOT := "cidade-%d"
const CITY_SLOT_AGAIN := "cidade-%d-%d"
const FIRST_AGAIN := 2
const EXPORT_PREFIX := "mapa-"
const INDEX_FILE := "cidades.cfg"
const INDEX_SECTION := "index"
const INDEX_LAST := "last"
const QUARANTINE_DIR := "quarentena"


## Runs driven by tests save in a folder of their own process: the suite runs its tests in parallel, each its own Godot, and in
## one shared folder a test read or set aside another's city, index or quarantine (log/pendencias.md, save_undo_test).
static func dir() -> String:
	return "%s/%d" % [TEST_SAVES_DIR, OS.get_process_id()] if GameFlow.tests_drive else SAVES_DIR


static func path(slot: String) -> String:
	return "%s/%s.%s" % [dir(), slot, EXT]


static func exists(slot: String) -> bool:
	return FileAccess.file_exists(path(slot))


## A slot of its own for a city that was never saved (the first name no file holds).
static func free_city_slot(seed: int) -> String:
	var slot := CITY_SLOT % seed
	var again := FIRST_AGAIN
	while exists(slot):
		slot = CITY_SLOT_AGAIN % [seed, again]
		again += 1
	return slot


static func export_slot(seed: int) -> String:
	return EXPORT_PREFIX + str(seed)


## A city's own slot (saved again in place); a map copy (mapa-*) opens as a new city, saved to a slot of its own.
static func is_city_slot(slot: String) -> bool:
	return not slot.begins_with(EXPORT_PREFIX)


## The saved cities and map copies, newest first: [{slot, modified (unix seconds), size (bytes)}].
static func list() -> Array:
	var out := []
	if not DirAccess.dir_exists_absolute(dir()): # nothing saved yet
		return out
	for f in DirAccess.get_files_at(dir()):
		if f.get_extension() == EXT:
			var file := "%s/%s" % [dir(), f]
			out.append({"slot": f.get_basename(), "modified": FileAccess.get_modified_time(file),
				"size": FileAccess.get_size(file)})
	out.sort_custom(func(a: Dictionary, b: Dictionary) -> bool: return a.modified > b.modified)
	return out


## The player's last city: the index's, else (saves older than the index) the newest city slot; "" when none.
static func last() -> String:
	var index := ConfigFile.new()
	if index.load(_index_path()) == OK:
		var slot: String = index.get_value(INDEX_SECTION, INDEX_LAST, "")
		if not slot.is_empty() and exists(slot):
			return slot
	for f in list():
		if is_city_slot(f.slot):
			return f.slot
	return ""


## Records the city last opened or saved. Returns the ConfigFile error.
static func remember_last(slot: String) -> Error:
	var index := ConfigFile.new()
	index.load(_index_path()) # a missing index is a new one
	index.set_value(INDEX_SECTION, INDEX_LAST, slot)
	return index.save(_index_path())


## Sets an unreadable city aside in the quarantine folder, under the date and time (kept, never deleted nor written over). Returns the error.
static func quarantine(slot: String) -> Error:
	var to := "%s/%s" % [dir(), QUARANTINE_DIR]
	DirAccess.make_dir_recursive_absolute(to)
	var stamp := Time.get_datetime_string_from_system(true).replace(":", "-")
	var name := "%s/%s-%s" % [to, slot, stamp]
	var again := FIRST_AGAIN
	while FileAccess.file_exists("%s.%s" % [name, EXT]): # the same second: never over an earlier one
		name = "%s/%s-%s-%d" % [to, slot, stamp, again]
		again += 1
	return DirAccess.rename_absolute(path(slot), "%s.%s" % [name, EXT])


static func _index_path() -> String:
	return "%s/%s" % [dir(), INDEX_FILE]
