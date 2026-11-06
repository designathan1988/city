## A folder of things the player keeps, a file each: what it holds, by name. One owner for ModelLibrary and RoadModels.
class_name SavedEntries
extends RefCounted


## Every entry of `folder` with this extension, by name: what the file holds, with `key` its file's name.
static func list(folder: String, extension: String) -> Array[Dictionary]:
	var out: Array[Dictionary] = []
	if not DirAccess.dir_exists_absolute(folder): # nothing saved yet
		return out
	for file in DirAccess.get_files_at(folder):
		if file.get_extension() != extension:
			continue
		var f := FileAccess.open("%s/%s" % [folder, file], FileAccess.READ)
		var entry: Variant = f.get_var() if f != null else null
		if entry is Dictionary:
			entry.key = file.get_basename()
			out.append(entry)
	out.sort_custom(func(a: Dictionary, b: Dictionary) -> bool: return a.name.naturalnocasecmp_to(b.name) < 0)
	return out
