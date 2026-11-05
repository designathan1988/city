## Where the opening of a city goes: the file (--file=PATH, a city saved by the game) opened twice in a real world, each stage
## as the core timed it (check, read, grade, compose, lots, traffic) and the whole. The second round is the one to compare:
## the first pays the disk.
## Run: godot --path project --resolution 640x360 res://tests/perf/open_probe.tscn -- --no-flow --file=PATH
extends Node3D

const ROUNDS := 2


func _ready() -> void:
	var file := ""
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--file="):
			file = arg.trim_prefix("--file=")
	var world := CityWorld.new()
	add_child(world)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty() or file == "":
		push_error("boot: %s, file: %s" % [errors, file])
		get_tree().quit(1)
		return
	for round in ROUNDS:
		var t0 := Time.get_ticks_msec()
		var r: Dictionary = CityStore.saves.open_file(file)
		print("open %d: %d ms, ok %s, %s" % [round, Time.get_ticks_msec() - t0, r.get("ok", false), r.get("ms", {})])
	get_tree().quit(0)
