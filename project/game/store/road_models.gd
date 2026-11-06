## The player's own roads (Meus modelos): one file each, kept for every city, as ModelLibrary keeps the buildings. A road is {name, base, spec}: the
## class it is drawn in, by id, and its elements. Runs driven by tests keep theirs apart.
class_name RoadModels
extends RefCounted

const FOLDER := "user://road_models"
const TEST_DIR := "user://road_models_tests"
const EXT := "via"


static func dir() -> String:
	return TEST_DIR if GameFlow.tests_drive else FOLDER


static func list() -> Array[Dictionary]:
	return SavedEntries.list(dir(), EXT)


## Under its name, replacing one of that name. False when the file could not be written: what was not kept is told.
static func save(road_name: String, base: String, spec: Dictionary) -> bool:
	if road_name.strip_edges() == "" or DirAccess.make_dir_recursive_absolute(dir()) != OK:
		return false
	var f := FileAccess.open("%s/%s.%s" % [dir(), road_name.validate_filename(), EXT], FileAccess.WRITE)
	if f == null:
		return false
	f.store_var({"name": road_name, "base": base, "spec": spec})
	return true


static func remove(key: String) -> void:
	DirAccess.remove_absolute("%s/%s.%s" % [dir(), key, EXT])
