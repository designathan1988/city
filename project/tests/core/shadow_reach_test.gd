## The sun's cascades reach what the view shows and no further (render/sun_cascades.gd): looking down on the town the picture ends at the
## ground its top edge meets, so the shadows reach that far, do not fade inside the picture and stop a step or two past it (every cascade
## past it draws the city's casters for pixels no one sees); with the horizon in the picture they reach what the data and the tier give
## and fade out by the data's share. The view's end is worked out here from the camera's own ray through the top of the screen.
## Run: godot --path project res://tests/core/shadow_reach_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const SETTLE_FRAMES := 6
const DOWN_VIEW := [0.0, 0.0, 30.0, 40.0, 1200.0] # x, z, yaw, pitch, distance: the town from above, the horizon out of the picture
const LEVEL_VIEW := [80.0, -40.0, 210.0, 2.0, 40.0] # from a street, along it: the horizon in the picture
const MOST_PAST := 1.5 # how far past the view's end the reach may stand: its steps, no more
const FADE_SLACK := 0.98 # of the view's end: where the fade may begin at the earliest


func _ready() -> void:
	if not boot():
		finish()
		return
	var rig := CityCamera.new()
	add_child(rig)
	new_map(TOWN_MAP, SEED, true)
	Settings.attach(world, get_viewport())
	var day: Dictionary = DataTables.game("daylight")
	await _stand(rig, DOWN_VIEW)
	var cam := get_viewport().get_camera_3d()
	var end := _ground_depth(cam, 0.0)
	var nearest := _ground_depth(cam, get_viewport().get_visible_rect().size.y)
	var reach := world.sun.directional_shadow_max_distance
	var first: float = reach * world.sun.directional_shadow_split_1
	print("from above: the view shows the ground from %.0f to %.0f m; the shadows reach %.0f m, fading from %.2f, the first cascade to %.0f m" % [
			nearest, end, reach, world.sun.directional_shadow_fade_start, first])
	check(end < INF, "the view from above ends before the horizon: %.0f m" % end)
	check(reach >= end, "the shadows reach the farthest ground the view shows: %.0f of %.0f m" % [reach, end])
	check(reach <= end * MOST_PAST, "and stop there: %.0f m, %.2f of the view's end" % [reach, reach / end])
	check(reach * world.sun.directional_shadow_fade_start >= end * FADE_SLACK, "no shadow fades inside the picture: from %.0f m" % (reach * world.sun.directional_shadow_fade_start))
	check(first > nearest, "the first cascade holds ground the view shows: it ends at %.0f m, the nearest ground is at %.0f m" % [first, nearest])
	await _stand(rig, LEVEL_VIEW)
	reach = world.sun.directional_shadow_max_distance
	print("along the streets: the shadows reach %.0f m, fading from %.2f" % [reach, world.sun.directional_shadow_fade_start])
	check(_ground_depth(get_viewport().get_camera_3d(), 0.0) == INF, "the view along the streets shows the horizon")
	check(reach >= world.daylight.cascades.least_m, "there the shadows reach what the tier asks at least: %.0f of %.0f m" % [reach, world.daylight.cascades.least_m])
	check(is_equal_approx(world.sun.directional_shadow_fade_start, day.shadow_fade_start), "and fade out by the data's share: %.2f" % world.sun.directional_shadow_fade_start)
	finish()


func _stand(rig: CityCamera, view: Array) -> void:
	rig.jump(Vector3(view[0], 0.0, view[1]), view[2], view[3], view[4])
	for i in SETTLE_FRAMES:
		await get_tree().process_frame


## How deep into the view the ground on the screen's row `y` lies (0 the top edge: the farthest the picture shows; the screen's height
## the bottom one: the nearest): the camera's ray through the middle of that row down to the ground there (the ground under the eye
## first, then the ground where that ray lands), as depth along the view; INF when the ray never comes down.
func _ground_depth(cam: Camera3D, y: float) -> float:
	var spot := Vector2(get_viewport().get_visible_rect().size.x * 0.5, y)
	var from := cam.project_ray_origin(spot)
	var dir := cam.project_ray_normal(spot)
	if dir.y >= 0.0:
		return INF
	var ground: float = CityStore.query(&"height_at", [from.x, from.z])
	var at := from
	for i in 2:
		at = from + dir * ((from.y - ground) / -dir.y)
		ground = CityStore.query(&"height_at", [at.x, at.z])
	return (at - cam.global_position).dot(-cam.global_basis.z)
