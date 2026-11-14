## The far traffic as flows and the window that follows the view (probe far_traffic), on the town, in seconds: the town opens with
## its streets carrying their traffic; from a view beside the town the window holds the streets round it and the rest stay flows;
## no far street holds more than its room; vehicles pass both ways between the flows and the window, and the window carries its
## share of traffic; the flows' step keeps up with the clock; the hour set, the streets follow it; the window grows with the view
## at once, does not shrink for a small change and shrinks for a large one; the far cars the render draws are those the flows carry.
## Run: godot --path project res://tests/core/flows_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 7
const HOUR := 8.0 # the morning's traffic
const NIGHT_HOUR := 3.0
const ASIDE_SHARE := 0.6 # the view stands this share of the least window off the town's middle: part of the town in it, part out
const CLOSE_REACH_M := 100.0 # a street view: the window at its least
const WIDE_REACH_M := 1000.0 # the window at its most
const NEAR_WIDE_M := 850.0 # a little closer than the wide view: the window stays
const MID_REACH_M := 500.0 # much closer: it shrinks to what that view asks
const WARM_TICKS := 900
const LOOK_TICKS := 120 # past the window's own look (a second) and the flows' step
const RADIUS_SLACK_M := 1.0
const CARRIED_SHARE := 0.5 # the window carries at least this share of what its lanes ask
const NIGHT_SHARE := 0.6 # by night the far streets carry at most this share of the morning's
const AWAY_M := 3000.0 # the last view's ground point, east of the town's middle: past the window's out radius from every street
const AWAY_UP_M := 500.0 # the eye over it: its frame's corners reach well short of the town
const STRAIGHT_DOWN_DEG := -90.0
const TOLD_ROUNDS := 2 # the view taken and the streets told, then what the first frames asked told again (the render's rebuild)
const DRAW_FRAMES := 3 # a frame takes what was told, the next draws it
## Each street shows a whole number of cars within one of what it carries, by a share spread evenly over the streets: over the
## town's hundreds of streets the sum drawn stays within a fifth of the sum carried (some 15 cars of standard deviation at most).
const DRAWN_SLACK := 0.2


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	CityStore.query(&"set_city_hour", [HOUR])
	var p: Dictionary = DataTables.catalog_table("traffic").params # data is read by its one reader
	var at := Vector3(p.window_min_m * ASIDE_SHARE, 0, 0)
	var open: Dictionary = CityStore.query(&"far_traffic", [at, CLOSE_REACH_M])
	check(open.streets > 0 and open.on_far > 0.0, "the town opens with its streets carrying traffic: %.0f vehicles on %d streets" % [open.on_far, open.streets])
	ticks(WARM_TICKS)
	var c: Dictionary = CityStore.query(&"far_traffic", [at, -1.0])
	var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
	check(absf(c.window[2] - p.window_min_m) <= RADIUS_SLACK_M, "a close view keeps the window at its least: %.0f m" % c.window[2])
	check(c.window_streets > 0 and c.far_streets > 0 and c.ways_out > 0,
			"the window holds the streets round the view, the rest stay flows: %d inside, %d far, %d ways out" % [c.window_streets, c.far_streets, c.ways_out])
	check(c.on_far > 0.0 and c.over_room == 0, "the far streets carry traffic within their room: %.0f vehicles, %.0f of them queued, %d streets over" % [c.on_far, c.queued, c.over_room])
	check(c.handed_out > 0.0 and float(probe.window[2]) >= CARRIED_SHARE * float(probe.window[1]),
			"the flows hand the window its traffic: %.0f handed, %d drive in it of the %.0f its lanes ask" % [c.handed_out, probe.window[2], probe.window[1]])
	check(c.steps > 0 and c.late == 0, "the flows' step keeps up with the clock: %d steps, %d late" % [c.steps, c.late])
	CityStore.query(&"set_city_hour", [NIGHT_HOUR])
	ticks(LOOK_TICKS)
	var night: Dictionary = CityStore.query(&"far_traffic", [at, -1.0])
	check(night.on_far <= NIGHT_SHARE * c.on_far, "the hour set, the far streets follow it: %.0f vehicles by night, %.0f in the morning" % [night.on_far, c.on_far])
	CityStore.query(&"set_city_hour", [HOUR])
	CityStore.query(&"far_traffic", [at, WIDE_REACH_M])
	ticks(LOOK_TICKS)
	var wide: Dictionary = CityStore.query(&"far_traffic", [at, NEAR_WIDE_M])
	check(absf(wide.window[2] - p.meso_window_m) <= RADIUS_SLACK_M and wide.window_streets > c.window_streets,
			"the window grows with the view: %.0f m, %d streets" % [wide.window[2], wide.window_streets])
	ticks(LOOK_TICKS)
	var held: Dictionary = CityStore.query(&"far_traffic", [at, MID_REACH_M])
	check(absf(held.window[2] - p.meso_window_m) <= RADIUS_SLACK_M, "a little closer, the window stays as it is: %.0f m" % held.window[2])
	ticks(LOOK_TICKS)
	var shrunk: Dictionary = CityStore.query(&"far_traffic", [at, -1.0])
	var asked := clampf(MID_REACH_M * float(p.window_view_share), p.window_min_m, p.meso_window_m) # the window's rule (window.cpp wanted_radius)
	check(absf(shrunk.window[2] - asked) <= RADIUS_SLACK_M and shrunk.window_streets < wide.window_streets,
			"much closer, it shrinks: %.0f m, %d streets" % [shrunk.window[2], shrunk.window_streets])
	check(shrunk.handed_in > 0.0, "the vehicles on the streets that left the window are the flows' again: %.0f taken back" % shrunk.handed_in)
	check(shrunk.over_room == 0, "no far street holds more than its room after the window's moves: %d over" % shrunk.over_room)
	await _far_cars_drawn()
	finish()


## The far cars are drawn from the flows: the view straight down over free ground east of the town, every street of the town
## outside the window, and the cars the render draws on them are about those the flows carry there (a street shows whole cars: its
## whole part, and one more by its own fixed share, flow_parts.h flow_shown). A frame gives the simulation the view; the ticks
## move the window and tell the render; the frames after them draw what was told.
func _far_cars_drawn() -> void:
	var camera := Camera3D.new()
	add_child(camera)
	camera.make_current()
	camera.rotation_degrees.x = STRAIGHT_DOWN_DEG
	await _look_down(camera, AWAY_M)
	var far: Dictionary = CityStore.query(&"far_traffic", [Vector3.ZERO, -1.0])
	var drawn: int = CityStore.query(&"traffic_probe", [0]).drawn_far_cars
	check(far.window_streets == 0 and far.on_far > 0.0, "the view away from the town, all its streets are the flows': %d in the window, %.0f vehicles on %d far streets" % [far.window_streets, far.on_far, far.far_streets])
	check(absf(float(drawn) - far.on_far) <= DRAWN_SLACK * far.on_far, "the far cars drawn are those the flows carry: %d drawn, %.0f on the far streets" % [drawn, far.on_far])
	# back over the town: the window's streets have their cars written too, shown where the detailed vehicles are not drawn
	await _look_down(camera, 0.0)
	var near: Dictionary = CityStore.query(&"far_traffic", [Vector3.ZERO, -1.0])
	drawn = CityStore.query(&"traffic_probe", [0]).drawn_far_cars
	check(near.window_streets > 0 and float(drawn) >= near.on_far + CARRIED_SHARE * float(near.detailed),
			"the window's streets have their cars written too: %d written, %.0f on the far streets, %d detailed on %d streets of the window" % [drawn, near.on_far, near.detailed, near.window_streets])


## The view straight down on the ground `x` metres east of the town's middle, taken by the simulation, told to the render and drawn.
func _look_down(camera: Camera3D, x: float) -> void:
	camera.global_position = Vector3(x, AWAY_UP_M, 0)
	for _round in TOLD_ROUNDS:
		for _frame in DRAW_FRAMES:
			await get_tree().process_frame
		ticks(LOOK_TICKS)
	for _frame in DRAW_FRAMES:
		await get_tree().process_frame
