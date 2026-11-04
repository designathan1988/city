## The camera of a photo of the Roadcraft 4.0: its `viewCamera` read back gives pos = target + distance * (cos el cos az, sin el,
## cos el sin az) with a lens of 35 degrees at any zoom, where ours widens with the view. The rig stays stopped until the caller
## starts it again.
class_name PairCamera
extends RefCounted

const FOV := 35.0
const LINES := 900.0 # the 4.0's zoom is pixels a metre, on the lines of its photos
const RIGHT_ANGLE := 90.0


static func stand(rig: CityCamera, ui: UiDriver, p: Vector3, azimuth: float, tilt: float, zoom: float) -> void:
	var dist := LINES / (2.0 * zoom) / tan(deg_to_rad(FOV) * 0.5)
	rig.jump(p, RIGHT_ANGLE - azimuth, tilt, dist)
	await ui.frames(2)
	rig.set_process(false)
	var az := deg_to_rad(azimuth)
	var el := deg_to_rad(tilt)
	var back := Vector3(cos(el) * cos(az), sin(el), cos(el) * sin(az))
	rig.camera.fov = FOV
	rig.camera.global_transform = Transform3D(Basis.looking_at(-back, Vector3.UP), p + back * dist)
