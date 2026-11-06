## What each render layer costs the GPU at the views of the game: the frame with it and without it (FrameCapture). Quiet machine:
## godot_console --path project res://tests/perf/layer_gpu.tscn -- --no-flow [--preset=high]
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const VIEWS := [["perto", 28.0, 18.0], ["rua", 90.0, 25.0], ["bairro", 500.0, 40.0], ["cidade", 2200.0, 55.0]]
const SETTLE_FRAMES := 90
const SETTLE_LIMIT := 900
const MILLION := 1000000.0

var _rig: CityCamera


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	_rig = CityCamera.new()
	add_child(_rig)
	CityStore.boot(world.terrain, world.materials())
	CityStore.new_map(TOWN_MAP, SEED, true)
	Settings.attach(world, get_viewport())
	var frame := FrameCapture.new()
	add_child(frame)
	for v: Array in VIEWS:
		_rig.jump(_rig.focus, 0.0, v[2], v[1])
		var waited := 0
		while waited < SETTLE_LIMIT and (waited < SETTLE_FRAMES or CityStore.query(&"render_pending") > 0):
			await get_tree().process_frame
			waited += 1
		await frame.take()
		print("%s | %.2f ms | %.2f Mprim, %.2f de sombra" % [v[0], frame.whole.gpu_ms, frame.whole.primitives / MILLION, frame.whole.shadow_primitives / MILLION])
		var by_time := frame.rows.duplicate()
		by_time.sort_custom(func(a: Dictionary, b: Dictionary) -> bool: return a.gpu_ms > b.gpu_ms)
		for r: Dictionary in by_time:
			if r.gpu_ms > 0.03 or r.primitives > 0:
				print("  %s %.2f ms, %.2f Mprim, %.2f sombra, %d chamadas" % [r.layer, r.gpu_ms, r.primitives / MILLION, r.shadow_primitives / MILLION, r.draws])
	get_tree().quit(0)
