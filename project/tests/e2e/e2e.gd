## End-to-end test of the layer the player sees: boots the real world and HUD, generates the city, waits for every
## chunk, warms the traffic and checks the instance buffers the renderer sends to the GPU and the HUD nodes.
## Run: godot --path project res://tests/e2e/e2e.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const SETTLE_FRAMES := 2
const BUILD_FRAMES := 400
const WARMUP_TICKS := 2400
const MIN_DRAWN_VEHICLES := 1
const MIN_DRAWN_PEOPLE := 1
const LENSES_PER_POST := 2
const SUV := 2 # traffic.json classes: b 2,8 and b_emergency 5,8 m/s²
const NO_TURN := 1000.0 # m/s: no limit at the line
const TURN_MS := 6.11 # turn_speed_kmh in m/s
const AMBER_EPS_S := 0.01

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var cam := Camera3D.new()
	add_child(cam) # look_at needs the node in the tree (its global transform)
	cam.look_at_from_position(Vector3(0, 300, 300), Vector3.ZERO)
	cam.current = true
	var errors := CityStore.boot(world.terrain, world.materials())
	_check(errors.is_empty(), "boot %s" % [errors])
	var hud := RoadcraftHud.new()
	add_child(hud)
	await _frames(SETTLE_FRAMES)
	_check(hud.get_child_count() > 0, "HUD montou %d nós" % hud.get_child_count())
	var report: Dictionary = CityStore.new_map(TOWN_MAP, SEED, true)
	_check(report.get("ok", false), "mapa gerado %s" % [report])
	CityStore.dispatch(&"set_speed", [0])
	await _wait_render()
	CityStore.step_ticks(WARMUP_TICKS)
	await _frames(5) # let the core consume the published poses and refill the instance buffers
	var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
	print("tráfego: ", probe.vehicles, " veículos, ", probe.people, " pessoas, desenhados ", probe.drawn_vehicles,
			"/", probe.drawn_people)
	_check(int(probe.drawn_vehicles) >= MIN_DRAWN_VEHICLES, "veículos no buffer de instâncias")
	_check(int(probe.drawn_people) >= MIN_DRAWN_PEOPLE, "pessoas no buffer de instâncias")
	_check(int(probe.vehicles) > 0 and int(probe.people) > 0, "frota e pedestres vivos")
	_check(int(probe.moving) > 0, "veículos andando")
	# every signal post near the camera lights its car lens and its walkers' lens from the lights the simulation published
	print("semáforos: ", probe.signals, ", lentes acesas ", probe.drawn_signal_lenses)
	_check(int(probe.signals) == 0 or int(probe.drawn_signal_lenses) >= LENSES_PER_POST, "lentes dos semáforos acesas")
	_check(int(probe.red_entries) == 0, "ninguém entrou no vermelho (%d)" % probe.red_entries)
	_check_amber_rule()
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## The amber's decision (light_rule.cpp) in numbers: speed, metres to where it stops, speed at the line, amber left.
func _check_amber_rule() -> void:
	var core: Object = CityStore._core
	_check(core.amber_stops(10.0, 30.0, NO_TURN, 4.0, SUV), "amarelo: quem pode parar como o entreverdes supõe para")
	_check(not core.amber_stops(16.67, 50.0, NO_TURN, 4.0, SUV), "amarelo: quem não para e chega à linha segue")
	# just out of the far model near the line, as the soak saw it: slow enough to stop, the amber nearly over
	_check(core.amber_stops(7.32, 9.57, NO_TURN, 0.1, SUV), "amarelo: quem não chega à linha a tempo para")
	_check(not core.amber_stops(14.0, 40.0, NO_TURN, 3.5, SUV), "amarelo: em frente chega a tempo")
	_check(core.amber_stops(14.0, 40.0, TURN_MS, 3.5, SUV), "amarelo: freando para a conversão não chega e para")
	_check(not core.amber_stops(14.0, 10.0, NO_TURN, 0.1, SUV), "amarelo: quem nem a emergência para segue")
	_check(absf(core.amber_wanted(60.0) - 4.0) < AMBER_EPS_S, "amarelo de 4 s a 60 km/h (MBST vol. V 5.7.1)")
	_check(absf(core.amber_wanted(70.0) - 5.0) < AMBER_EPS_S, "amarelo de 5 s a 70 km/h")


func _wait_render() -> void:
	var waited := 0
	await _frames(SETTLE_FRAMES)
	while CityStore.query(&"render_pending") > 0 and waited < BUILD_FRAMES:
		await _frames(1)
		waited += 1
	print("quadros de construção: ", waited, " pendentes ao fim: ", CityStore.query(&"render_pending"))


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
