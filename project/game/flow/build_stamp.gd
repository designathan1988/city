## The build stamp the About dialog shows: branch, short commit and date of what is running. tools/build/build.sh writes it at
## export (res://build_stamp.cfg); a run from the source reads the repository's HEAD instead.
class_name BuildStamp
extends RefCounted

const STAMP_PATH := "res://build_stamp.cfg"
const HEAD_PATH := "res://../.git/HEAD"
const REFS := "ref: refs/heads/"
const SHORT_HASH := 8


static func text() -> String:
	var cfg := ConfigFile.new()
	if cfg.load(STAMP_PATH) == OK:
		return TranslationServer.translate("build.stamp").format({"branch": cfg.get_value("build", "branch", "?"),
			"hash": cfg.get_value("build", "hash", "?"), "date": cfg.get_value("build", "date", "")})
	var head := FileAccess.get_file_as_string(ProjectSettings.globalize_path(HEAD_PATH)).strip_edges()
	var branch := head.trim_prefix(REFS)
	var hash := head
	if head.begins_with(REFS):
		var ref := ProjectSettings.globalize_path("res://../.git/" + head.trim_prefix("ref: "))
		hash = FileAccess.get_file_as_string(ref).strip_edges()
	return TranslationServer.translate("build.stamp").format({"branch": branch, "hash": hash.left(SHORT_HASH),
		"date": Time.get_date_string_from_system()})
