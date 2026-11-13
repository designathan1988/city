## Bomb: a click lets a charge of the chosen force fall where the pointer is: on the first building along its line of sight
## (the roof or the face under it), else on the ground; the core says what it brought down, worn and killed, and the whole
## blow is one Undo step. The force is the tool's `strength` param, which the bomb tab's stepper sets (StrikeStrip).
extends ToolBase


func click(p: Vector3) -> void:
	var force: int = int(params.get("strength", DataTables.tuning("strike_force_start")))
	var r: Dictionary = CityStore.dispatch(&"strike", [_landing(p), force])
	if not r.get("ok", false): # told by the store's channel
		return
	ctl.hint.emit(tr("hint.strike.done").format({"force": force}), true)


## Where the charge lands: the wall or roof the line of sight meets before the ground at `p`, or `p`.
func _landing(p: Vector3) -> Vector3:
	var camera: Camera3D = ctl.rig.camera
	var eye := camera.project_ray_origin(ctl.pointer)
	var hit: Dictionary = CityStore.query(&"body_pick", [eye, camera.project_ray_normal(ctl.pointer)])
	if hit.is_empty() or eye.distance_to(hit.point) > eye.distance_to(p):
		return p
	return hit.point
