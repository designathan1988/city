## What camera_input_test waits for, read from the rig's state and never a fixed time, so a machine sharing its cores cannot fail it;
## a state that never comes fails the check that follows the wait.
class_name CameraWaits
extends RefCounted


## The pull let go, as long as the slowest let-go takes (camera.json: the hold, then the ease from a full pull down to `t.EPS`).
static func pull_settled(t: Node) -> void:
	var cfg: Dictionary = t.rig.cfg
	var limit: float = cfg.pull_hold_s + cfg.pull_out_s * log(1.0 / t.EPS)
	var waited := 0.0
	while t.rig.pull.shown() >= t.EPS and waited < limit:
		await t.get_tree().process_frame
		waited += t.get_process_delta_time()


## The eye at rest: its rise over the ground is a spring (CameraHeights), so right after a drag it may still be on its way to the
## clearance. Waits at most `limit_s`.
static func eye_at_rest(t: Node, limit_s: float) -> void:
	var last: Vector3 = t.rig.camera.global_position
	var waited := 0.0
	while waited < limit_s:
		await t.get_tree().process_frame
		waited += t.get_process_delta_time()
		if t.rig.camera.global_position.distance_to(last) < t.EPS * t.EPS:
			return
		last = t.rig.camera.global_position
