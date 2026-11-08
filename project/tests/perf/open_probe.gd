## Where the opening of a city goes: the file (--file=PATH, a city saved by the game) opened twice in a real world, each stage
## as the core timed it (check, read, grade, compose, lots, traffic) and the whole. The second round is the one to compare:
## the first pays the disk.
## With --digest: no file, the map 1 of seed 7 made and its state hashed, one line for tools/build/quick.sh to hold to the golden.
## Run: godot --path project --resolution 640x360 res://tests/perf/open_probe.tscn -- --no-flow --file=PATH
extends Node3D

const ROUNDS := 2
const DIGEST_MAP := 1
const DIGEST_SEED := 7
const DIGEST_TICKS := 200


func _ready() -> void:
	var file := ""
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--file="):
			file = arg.trim_prefix("--file=")
	var world := CityWorld.new()
	add_child(world)
	var errors := CityStore.boot(world.terrain, world.materials())
	var digest := OS.get_cmdline_user_args().has("--digest")
	if not errors.is_empty() or (file == "" and not digest):
		push_error("boot: %s, file: %s" % [errors, file])
		get_tree().quit(1)
		return
	if digest:
		CityStore.new_map(DIGEST_MAP, DIGEST_SEED, true, true)
		var hash: Dictionary = CityStore.query(&"state_hash")
		var fleet: PackedStringArray = CityStore.query(&"traffic_hashes", [DIGEST_TICKS])
		print("DIGEST ", hash.whole, " ", JSON.stringify(hash.derived, "", true), " fleet ", fleet[DIGEST_TICKS - 1])
		get_tree().quit(0)
		return
	for round in ROUNDS:
		var t0 := Time.get_ticks_msec()
		var r: Dictionary = await CityStore.saves.open_file(file)
		print("open %d: %d ms, ok %s, %s" % [round, Time.get_ticks_msec() - t0, r.get("ok", false), r.get("ms", {})])
	get_tree().quit(0)
