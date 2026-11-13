## Refusals: the one formatter of a refused command's answer ({ok: false, error: key}), shared by the HUD's one listener of
## CityStore.command_failed, the tools and the panels. A `refuse.*` or `road.reason.*` key reads "Cannot build here: {why}."
## (tools.toast.cannotBuild); any other key, its own text. Static: it keeps nothing, every text is read when asked.
class_name Refusals
extends RefCounted

## The keys that say why something cannot be built there: they are told inside tools.toast.cannotBuild.
const BUILD_KEYS: Array[String] = ["refuse.", "road.reason."]


## The refusal's text for the hint bar.
static func text(result: Dictionary) -> String:
	var why: String = str(result.get("error", ""))
	for prefix in BUILD_KEYS:
		if why.begins_with(prefix):
			return _translated("tools.toast.cannotBuild").format({"why": reason(result)})
	return _translated(why)


## Why a command was refused, in the few words the label at the tip of a road has room for.
static func reason(result: Dictionary) -> String:
	return _translated(str(result.get("error", ""))).format({"max": DataTables.tuning("road_max_degree")})


## Object.tr needs an instance: from a static function the translation server is asked.
static func _translated(key: String) -> String:
	return String(TranslationServer.translate(key))
