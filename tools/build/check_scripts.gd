# Loads every GDScript of the project and prints those that fail to compile (their parse errors come out as SCRIPT ERROR).
extends SceneTree

func _walk(dir: String, out: Array) -> void:
	for f in DirAccess.get_files_at(dir):
		if f.ends_with(".gd"):
			out.push_back(dir.path_join(f))
	for d in DirAccess.get_directories_at(dir):
		if not d.begins_with(".") and d != "addons":
			_walk(dir.path_join(d), out)

func _init() -> void:
	process_frame.connect(_check, CONNECT_ONE_SHOT) # the autoloads are in the tree by the first frame


func _check() -> void:
	print("autoload CityStore: ", root.has_node("CityStore"))
	var all: Array = []
	_walk("res://", all)
	var bad := 0
	for p in all:
		var s: Script = ResourceLoader.load(p, "Script", ResourceLoader.CACHE_MODE_IGNORE)
		if s == null or not s.can_instantiate() and not s.is_abstract():
			print("FAIL ", p)
			bad += 1
	print("scripts: %d, FAILURES: %d" % [all.size(), bad])
	quit(1 if bad > 0 else 0)
