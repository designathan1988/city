## End-to-end test of the layer the player sees: boots the real world and HUD, generates the city, waits for every
## chunk, warms the traffic and checks the instance buffers the renderer sends to the GPU and the HUD nodes.
## Run: godot --path project res://tests/e2e/e2e.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const SETTLE_FRAMES := 2
const BUILD_FRAMES := 400
const WARMUP_TICKS := 2400
const MIN_DRAWN_VEHICLES := 1
const MIN_DRAWN_PEOPLE := 1
const LENSES_PER_POST := 2
const NO_TURN := 1000.0 # m/s: no limit at the line
const AMBER_EPS_S := 0.01
const HIGH_EYE := Vector3(1000, 1830, 1000) # past the walkers' window from the city's edge, looking at its middle
const HIGH_TICKS := 600


func _ready() -> void:
	var cam := Camera3D.new()
	add_child(cam) # look_at needs the node in the tree (its global transform)
	cam.look_at_from_position(Vector3(0, 300, 300), Vector3.ZERO)
	cam.current = true
	if not boot():
		finish()
		return
	var hud := RoadcraftHud.new()
	add_child(hud)
	await _frames(SETTLE_FRAMES)
	check(hud.get_child_count() > 0, "the HUD built %d nodes" % hud.get_child_count())
	var report: Dictionary = new_map(TOWN_MAP, SEED, true)
	check(report.get("ok", false), "map made %s" % [report])
	await _wait_render()
	ticks(WARMUP_TICKS)
	await _frames(5) # let the core consume the published poses and refill the instance buffers
	var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
	print("traffic: ", probe.vehicles, " vehicles, ", probe.people, " people, drawn ", probe.drawn_vehicles,
			"/", probe.drawn_people)
	check(int(probe.drawn_vehicles) >= MIN_DRAWN_VEHICLES, "vehicles in the instance buffer")
	check(int(probe.drawn_people) >= MIN_DRAWN_PEOPLE, "people in the instance buffer")
	check(int(CityStore.query(&"building_cost").props) > 0, "balcony furniture in the instance buffer")
	check(int(CityStore.query(&"agent_cost").plants_near) > 0, "lot garden plants in the instance buffer")
	check(int(CityStore.query(&"agent_cost").rows_lost) == 0, "every vehicle seen has its row in the instance buffer")
	check(int(probe.vehicles) > 0 and int(probe.people) > 0, "fleet and pedestrians alive")
	check(int(probe.moving) > 0, "vehicles moving")
	# every signal post near the camera lights its car lens and its walkers' lens from the lights the simulation published
	print("signals: ", probe.signals, ", lenses lit ", probe.drawn_signal_lenses)
	check(int(probe.signals) == 0 or int(probe.drawn_signal_lenses) >= LENSES_PER_POST, "signal lenses lit")
	check(int(probe.red_entries) == 0, "nobody went in on red (%d)" % probe.red_entries)
	_check_amber_rule()
	# from high up the eye stands past the city's edge: the walkers stay where the camera looks
	cam.look_at_from_position(HIGH_EYE, Vector3.ZERO)
	await _frames(5)
	ticks(HIGH_TICKS)
	var far: Dictionary = CityStore.query(&"traffic_probe", [0])
	check(int(far.people) > 0, "from above, the pedestrians stay where the camera looks (%d)" % far.people)
	finish()


## The amber's decision (light_rule.cpp) in numbers: speed, metres to where it stops, speed at the line, amber left.
func _check_amber_rule() -> void:
	check(CityStore.query(&"amber_stops", [10.0, 30.0, NO_TURN, 4.0, CityStore.code(&"VehicleType", &"SUV")]), "amber: who can stop as the intergreen supposes stops")
	check(not CityStore.query(&"amber_stops", [16.67, 50.0, NO_TURN, 4.0, CityStore.code(&"VehicleType", &"SUV")]), "amber: who does not stop and reaches the line goes on")
	# just out of the far model near the line, as the soak saw it: slow enough to stop, the amber nearly over
	check(CityStore.query(&"amber_stops", [7.32, 9.57, NO_TURN, 0.1, CityStore.code(&"VehicleType", &"SUV")]), "amber: who does not reach the line in time stops")
	check(not CityStore.query(&"amber_stops", [14.0, 40.0, NO_TURN, 3.5, CityStore.code(&"VehicleType", &"SUV")]), "amber: straight on arrives in time")
	check(CityStore.query(&"amber_stops", [14.0, 40.0, DataTables.catalog_table("traffic").params.turn_speed_kmh / Units.KMH_PER_M_S, 3.5, CityStore.code(&"VehicleType", &"SUV")]), "amber: braking for the turn it does not arrive and stops")
	check(not CityStore.query(&"amber_stops", [14.0, 10.0, NO_TURN, 0.1, CityStore.code(&"VehicleType", &"SUV")]), "amber: who not even an emergency stops goes on")
	check(absf(CityStore.query(&"amber_wanted", [60.0]) - 4.0) < AMBER_EPS_S, "4 s of amber at 60 km/h (MBST vol. V 5.7.1)")
	check(absf(CityStore.query(&"amber_wanted", [70.0]) - 5.0) < AMBER_EPS_S, "5 s of amber at 70 km/h")


func _wait_render() -> void:
	var waited := 0
	await _frames(SETTLE_FRAMES)
	while CityStore.query(&"render_pending") > 0 and waited < BUILD_FRAMES:
		await _frames(1)
		waited += 1
	print("build frames: ", waited, " pending at the end: ", CityStore.query(&"render_pending"))


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame
