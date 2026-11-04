## The camera of a photo of the Roadcraft 4.0: its `viewCamera` read back gives pos = target + distance * (cos el cos az, sin el,
## cos el sin az) with a lens of 35 degrees at any zoom, where ours widens with the view. The rig stays stopped until the caller
## starts it again.
class_name PairCamera
extends RefCounted

const FOV := 35.0
const LINES := 900.0 # the 4.0's zoom is pixels a metre, on the lines of its photos
const RIGHT_ANGLE := 90.0
const PRESET := "high" # the quality the 4.0 takes its photos at, its occlusion among it: `--ssao=0` measures what that does
## The same use the 4.0's field is written with (look-par13): a value on each arm of the crossing, x0, x1, z0, z1 off the node.
const WORN_ARMS := [[12.0, 32.0, -9.0, 9.0, 0.25], [-32.0, -12.0, -9.0, 9.0, 0.5], [-12.0, 12.0, 12.0, 32.0, 0.75], [-12.0, 12.0, -32.0, -12.0, 1.0]]


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


## As the player of that preset sees it, the viewport's anti-aliasing with it. Not saved: the player's own choice stays.
static func quality(world: CityWorld, viewport: Viewport) -> void:
	Settings.preset = PRESET
	Settings.apply(world, viewport)
	world.environment.ssao_enabled = world.environment.ssao_enabled and not "--ssao=0" in OS.get_cmdline_user_args()


static func lay_wear(p: Vector3) -> void:
	for r in WORN_ARMS:
		CityStore.query(&"wear_paint", [p.x + r[0], p.z + r[2], p.x + r[1], p.z + r[3], r[4]])
