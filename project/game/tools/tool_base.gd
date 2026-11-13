## Base of the map tools: receives ground points from ToolController and dispatches store commands. Holds no game
## numbers: prices, refusals and results come back from the core in each command's Dictionary.
class_name ToolBase
extends RefCounted

var ctl: ToolController
var params: Dictionary


func _init(controller: ToolController, p: Dictionary) -> void:
	ctl = controller
	params = p
	enter()


func enter() -> void:
	pass


func exit() -> void:
	pass


func click(_p: Vector3) -> void:
	pass


func hover(_p: Vector3) -> void:
	pass


func drag(_p: Vector3) -> void:
	pass


## The left button went up (p = ground under it, or Vector3.INF off the map).
func release(_p: Vector3) -> void:
	pass


## Every frame while the left button is held.
func tick(_delta: float) -> void:
	pass


func cancel() -> void:
	pass


## Something is being drawn that cancel() drops: Escape takes it before the tool itself (Roadcraft main.ts gestureInProgress).
func in_progress() -> bool:
	return false


## Keeps a key from the camera, which takes it otherwise.
func keeps_key(_keycode: Key) -> bool:
	return false


func key(_keycode: Key) -> void:
	pass


## The road under the point, or {} with the preview cleared.
func road_under(p: Vector3) -> Dictionary:
	var seg: Dictionary = CityStore.query(&"segment_at", [p])
	if seg.get("seg", -1) < 0:
		ctl.preview.clear()
		return {}
	return seg


## How far from `p` a click still catches something: `px` pixels on the ground there, never under `least_m`.
func pick_m(p: Vector3, least_m: float, px: float) -> float:
	return maxf(least_m, px * ctl.px_at(p))


## The refusal text for a command result: "Cannot build here: {why}." / other keys as they are.
func refusal(result: Dictionary) -> String:
	var why: String = result.get("error", "")
	if why.begins_with("refuse.") or why.begins_with("road.reason."):
		return tr("tools.toast.cannotBuild").format({"why": reason(result)})
	return tr(why)


## Why a command was refused, in the few words the label at the tip of a road has room for.
func reason(result: Dictionary) -> String:
	return tr(result.get("error", "")).format({"max": DataTables.tuning("road_max_degree")})
