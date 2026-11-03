## The signal posts of a junction for the visual capture: from its four sides (one looks down an arm), low at the height
## of the heads, by day and by night (their posts, arms and lit lenses).
class_name CaptureSignals
extends RefCounted

const LOW_M := 26.0
const LOW_Y := 8.0
const LOOK_AT_Y := 3.0
const SIDES := 4


## `cap` is the capture scene (its world, _shot, _frames and _sky_frames); `hours` the day and the night hour.
static func shots(cap: Node, cross: Vector3, hours: Array) -> void:
	for i in hours.size():
		cap.world.set_hour(hours[i])
		await cap._frames(cap._sky_frames())
		for k in SIDES:
			var side := Vector3.FORWARD.rotated(Vector3.UP, k * TAU / SIDES) * LOW_M
			await cap._shot("19_semaforo_%s_%d" % ["dia" if i == 0 else "noite", k], cross + side + Vector3.UP * LOW_Y,
					cross + Vector3.UP * LOOK_AT_Y)
