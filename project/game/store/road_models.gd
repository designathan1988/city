## The player's own roads (Meus modelos): one file each, kept for every city, as ModelLibrary keeps the buildings. A road is {name, base, spec}: the
## class it is drawn in, by id, and its elements. Runs driven by tests keep theirs apart.
class_name RoadModels
extends RefCounted

const DIR := "user://road_models"
const TEST_DIR := "user://road_models_tests"
const EXT := "via"


static func dir() -> String:
	return TEST_DIR if GameFlow.tests_drive else DIR


static func list() -> Array[Dictionary]:
	var out: Array[Dictionary] = []
	if not DirAccess.dir_exists_absolute(dir()): # nothing saved yet
		return out
	for file in DirAccess.get_files_at(dir()):
		if file.get_extension() != EXT:
			continue
		var f := FileAccess.open("%s/%s" % [dir(), file], FileAccess.READ)
		var entry: Variant = f.get_var() if f != null else null
		if entry is Dictionary:
			entry.key = file.get_basename()
			out.append(entry)
	out.sort_custom(func(a: Dictionary, b: Dictionary) -> bool: return a.name.naturalnocasecmp_to(b.name) < 0)
	return out


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
