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


## How far from `p` a click still catches something: `px` pixels on the ground there, never under `least_m`.
func pick_m(p: Vector3, least_m: float, px: float) -> float:
	var cam := ctl.rig.camera
	var per_px := 2.0 * tan(deg_to_rad(cam.fov) * 0.5) * cam.global_position.distance_to(p) / cam.get_viewport().get_visible_rect().size.y
	return maxf(least_m, px * per_px)


## The refusal text for a command result: "Não dá para construir aqui: {why}." / money / other keys as they are.
func refusal(result: Dictionary, price: float = 0.0) -> String:
	var why: String = result.get("error", "")
	if why == "ui.insp.noMoney":
		return tr(why).format({"price": Formatters.format_money(price)})
	if why.begins_with("refuse.") or why.begins_with("road.reason."):
		return tr("tools.toast.cannotBuild").format({"why": tr(why).format({"max": DataTables.tuning("road_max_degree")})})
	return tr(why)
