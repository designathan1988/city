## Pistol: a click fires one round along the line of sight through the pointer. The core finds what the round meets
## first (a walker, whose part struck is the one clicked, or a vehicle) and answers; a miss is a hint, not an error.
extends ToolBase


func click(_p: Vector3) -> void:
	var screen: Vector2 = ctl.pointer
	var camera: Camera3D = ctl.rig.camera
	var r: Dictionary = CityStore.dispatch(&"shoot", [camera.project_ray_origin(screen), camera.project_ray_normal(screen)])
	if r.get("hit", 0) == 0:
		ctl.hint.emit(tr("hint.shoot.miss"), false)
		return
	ctl.hint.emit(tr("hint.shoot.killed") if r.get("killed", false) else tr("hint.shoot.hit"), true)
