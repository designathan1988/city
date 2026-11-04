## The player's own models: one file each, kept for every city. A model is the bytes of a body and what the building is used for;
## runs driven by tests keep theirs apart, so they never touch the player's.
class_name ModelLibrary
extends RefCounted

const MODELS_DIR := "user://models"
const TEST_MODELS_DIR := "user://models_tests"
const EXT := "modelo"


static func dir() -> String:
	return TEST_MODELS_DIR if GameFlow.tests_drive else MODELS_DIR


static func path(key: String) -> String:
	return "%s/%s.%s" % [dir(), key, EXT]


## Every model saved, by name: {key, name, bytes, function, zone}.
static func list() -> Array[Dictionary]:
	var out: Array[Dictionary] = []
	for file in DirAccess.get_files_at(dir()):
		if file.get_extension() != EXT:
			continue
		var f := FileAccess.open("%s/%s" % [dir(), file], FileAccess.READ)
		if f == null:
			continue
		var entry: Variant = f.get_var()
		if entry is Dictionary:
			entry.key = file.get_basename()
			out.append(entry)
	out.sort_custom(func(a: Dictionary, b: Dictionary) -> bool: return a.name.naturalnocasecmp_to(b.name) < 0)
	return out


## False when the file could not be written: a save that did not happen is told, not left for the player to find.
static func save(model_name: String, entry: Dictionary) -> bool:
	if DirAccess.make_dir_recursive_absolute(dir()) != OK:
		return false
	var key := _free_key(model_name)
	var f := FileAccess.open(path(key), FileAccess.WRITE)
	if f == null:
		return false
	f.store_var({"name": model_name, "bytes": entry.bytes, "function": entry.function, "zone": entry.zone})
	return f.get_error() == OK


static func remove(key: String) -> bool:
	return DirAccess.remove_absolute(path(key)) == OK


## A file name of its own for the name: its letters, then a number when another model holds them.
static func _free_key(model_name: String) -> String:
	var base := model_name.validate_filename().to_lower()
	var key := base
	var again := 1
	while FileAccess.file_exists(path(key)):
		again += 1
		key = "%s-%d" % [base, again]
	return key
