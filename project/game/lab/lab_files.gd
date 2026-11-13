## The folder the F7 lab writes to and the files in it kept to a bounded set: each one written is listed in <folder>/kept.json
## under its kind, oldest first, and past `kept.<kind>` (data/game/lab.json) the oldest listed is removed. Only listed files are
## ever removed: whatever else the folder holds (the player's older captures, anything not made by this list) stays. Runs driven
## by tests keep theirs apart (as RoadModels does). Two processes writing at once can lose a listing: that file is then never
## removed (a leak, never a deletion of something unlisted).
class_name LabFiles
extends RefCounted

const FOLDER := "user://lab"
const TEST_DIR := "user://lab_tests"
const KEPT_FILE := "kept.json"
const BESIDE := {"captures": ".png"} # the picture written next to a kind's file, removed with it
const OWN := ".json" # the extension the lab's own files end in, the one a picture beside replaces


## Where the lab writes: the player's folder, or the tests' own when tests drive the game.
static func dir() -> String:
	return TEST_DIR if GameFlow.tests_drive else FOLDER


## The list of what the lab wrote, in the folder it writes to.
static func list_path() -> String:
	return "%s/%s" % [dir(), KEPT_FILE]


## The picture beside a file of `kind`, "" when that kind has none.
static func beside(kind: String, path: String) -> String:
	if not BESIDE.has(kind) or not path.ends_with(OWN):
		return ""
	return path.trim_suffix(OWN) + BESIDE[kind]


## Lists `path` as the newest file of `kind` and removes the oldest listed past the data's bound.
static func keep(kind: String, path: String) -> void:
	var listed := _read()
	var files: Array[String] = []
	var before: Variant = listed.get(kind, [])
	if before is Array:
		for p: Variant in before:
			# a file wholly gone needs no removing; the path rewritten (same second) moves to the newest place
			if p is String and _ours(p) and p != path and _present(kind, p):
				files.append(p)
	files.append(path)
	var most := int(DataTables.game("lab").kept[kind])
	while files.size() > most:
		if not _remove(kind, files[0]):
			break
		files.pop_front()
	listed[kind] = files
	_write(listed)


## The list as the lab last wrote it; empty when there is none yet.
static func _read() -> Dictionary:
	var list := list_path()
	if not FileAccess.file_exists(list):
		return {}
	var parsed: Variant = JSON.parse_string(FileAccess.get_file_as_string(list))
	if parsed is Dictionary:
		return parsed
	push_error("LabFiles: %s is not a JSON object; the list starts again and the files it named stay" % list)
	return {}


static func _write(listed: Dictionary) -> void:
	DirAccess.make_dir_recursive_absolute(dir())
	var list := list_path()
	var out := FileAccess.open(list, FileAccess.WRITE)
	if out == null:
		push_error("LabFiles: %s could not be opened (%s)" % [list, error_string(FileAccess.get_open_error())])
	elif not out.store_string(JSON.stringify(listed, "\t")):
		push_error("LabFiles: %s could not be written" % list)


## A path inside the lab's own folder: the list never reaches outside it, whatever the file says.
static func _ours(path: String) -> bool:
	return path.begins_with(dir() + "/") and path != list_path() and not path.contains("..")


## The file or its picture is still there: an entry stays listed until both are gone.
static func _present(kind: String, path: String) -> bool:
	var picture := beside(kind, path)
	return FileAccess.file_exists(path) or (picture != "" and FileAccess.file_exists(picture))


## Removes a listed file and the picture beside it, the picture first, so a refused file leaves the entry listed for the next
## try; false (and the file stays listed) when the system refused.
static func _remove(kind: String, path: String) -> bool:
	for target: String in [beside(kind, path), path]:
		if target == "" or not FileAccess.file_exists(target):
			continue
		var err := DirAccess.remove_absolute(target)
		if err != OK:
			push_error("LabFiles: %s could not be removed (%s)" % [target, error_string(err)])
			return false
	return true
