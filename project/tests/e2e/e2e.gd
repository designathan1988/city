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
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


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
