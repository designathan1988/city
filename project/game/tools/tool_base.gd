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


func key(_keycode: Key) -> void:
	pass


## The refusal text for a command result: "Não dá para construir aqui: {why}." / other keys as they are.
func refusal(result: Dictionary) -> String:
	var why: String = result.get("error", "")
	if why.begins_with("refuse.") or why.begins_with("road.reason."):
		return tr("tools.toast.cannotBuild").format({"why": reason(result)})
	return tr(why)


## Why a command was refused, in the few words the label at the tip of a road has room for.
func reason(result: Dictionary) -> String:
	return tr(result.get("error", "")).format({"max": DataTables.tuning("road_max_degree")})
