## A look plan's view is the player's own: the tool that photographs the game (tests/visual/look.gd, LookAim.stand) stands the game's
## camera rig and leaves it living, so the eye is in no building, the lens is the one the game takes at that distance, the camera
## stands where the rig itself puts it and the point looked at shows at the plan's zoom (screen pixels a metre). A photo taken from
## where no player can stand shows defects the game does not have and hides the ones it has.
extends TestCase

const TOWN_MAP := 0
const SEED := 1
## A signalled T of the town with buildings on its corners: the plan's view from the street stood the eye inside one of them.
const JOINT := Vector2(540.0, -468.0)
## The plan's views there: [zoom, azimuth, elevation], near the ground to far above.
const VIEWS := [[60.0, 30.0, 12.0], [25.0, 30.0, 35.0], [25.0, 210.0, 35.0], [8.0, 30.0, 40.0], [2.0, 30.0, 48.0]]
const NEAR_M := 0.01 # two places this near are one
const ZOOM_SHARE := 0.02 # the zoom shown against the zoom asked


func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	Settings.attach(world, get_viewport())
	var ui := UiDriver.new(rig, get_tree(), SCRATCH)
	var joint := ui.ground(JOINT.x, JOINT.y)
	for view: Array in VIEWS:
		await LookAim.stand(rig, ui, joint, view)
		await ui.frames(2)
		var cam := rig.camera
		var eye := cam.global_position
		var free: PackedVector3Array = CityStore.query(&"eye_free", [rig.focus, eye, 0.0])
		check(free[1].distance_to(eye) < NEAR_M, "view %s: the eye stands in no building (the core takes it %.2f m out)" % [view, free[1].distance_to(eye)])
		check(rig.is_processing(), "view %s: the game's rig goes on living" % [view])
		check(is_equal_approx(cam.fov, CameraProfile.fov_deg(rig.cfg, rig.half)), "view %s: the lens is the game's at this distance (%.1f, the game's %.1f)" % [view, cam.fov, CameraProfile.fov_deg(rig.cfg, rig.half)])
		var stood := cam.global_transform
		rig.apply()
		check(stood.origin.distance_to(cam.global_position) < NEAR_M and stood.basis.z.distance_to(cam.global_transform.basis.z) < NEAR_M, "view %s: the camera stands where the rig puts it" % [view])
		var asked := float(get_viewport().get_visible_rect().size.y) / (2.0 * float(view[0]))
		check(absf(rig.half - asked) <= asked * ZOOM_SHARE, "view %s: the ground in view is the zoom's (%.2f m half, asked %.2f)" % [view, rig.half, asked])
	finish()
