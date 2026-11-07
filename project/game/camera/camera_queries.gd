## The camera's questions about the world (what the player sees under a point, the pull in front of a building, the eye taken out of one):
## each answered by the core once for the same arguments while the map stays as it was (its revision, which every change to the map moves:
## city_state.h touch). A view at rest asks nothing again; a moving one asks what it asked before. The answers kept are those of this
## settle and the one before, so nothing grows while the view travels.
class_name CameraQueries
extends RefCounted

var _revision := -1
var _now := {} # [query, arguments] -> the core's answer, asked in this settle
var _last := {} # the same, in the settle before


## A settle begins: the map's revision read once, the answers of a map that changed dropped.
func next_settle() -> void:
	var revision: int = CityStore.query(&"map_revision")
	if revision != _revision:
		_revision = revision
		_last = {}
	else:
		_last = _now
	_now = {}


## The core's answer to `name` with `args` (a CityStore query): read-only, shared by whoever asks the same.
func ask(name: StringName, args: Array) -> Variant:
	var key := [name, args]
	if _now.has(key):
		return _now[key]
	var answer: Variant = _last[key] if _last.has(key) else CityStore.query(name, args)
	_now[key] = answer
	return answer
