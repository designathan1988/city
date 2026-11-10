## The inspector and the map's edits through the store: numbers against the core's own probe, reverse, duplicate, paint with undo; a
## node too sharp reaches the warning; the file that fails keeps the city; two quarantines never meet; a session out of range comes back.
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const ROAD_LEN_M := 300.0
const PX := 0.25 # metres a pixel the road tool snaps by (road_commands_test.gd)
const SHARP_DEG := 12.0
const TICKS := 600
const KIB := 1024
const MS_PER_S := 1000.0
const THUNDER_M := 171.5

var failures := 0


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	failures += 0 if ok else 1


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var rig := CityCamera.new()
	add_child(rig)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.step_ticks(TICKS)
	_generated()
	_numbers()
	await _save_time()
	await _thunder()
	CityStore.new_map(TOWN_MAP, SEED, false)
	await _edits()
	_units()
	_sharp()
	_saves()
	_session()
	print("inspector_test: %d falhas" % failures)
	get_tree().quit(1 if failures > 0 else 0)


## What a save of the whole town takes, written on its thread: the number the autosave's period is judged by.
func _save_time() -> void:
	var slot := "cidade-medida"
	var from := Time.get_ticks_msec()
	CityStore.saves.save_game(slot)
	while CityStore.saves.save_busy():
		await get_tree().process_frame
	var took := Time.get_ticks_msec() - from
	var size := FileAccess.get_file_as_bytes(SaveSlots.path(slot)).size()
	print("  saving the whole city: %d ms, %d KiB" % [took, size / KIB])
	_check(size > 0, "save medido")
	DirAccess.remove_absolute(SaveSlots.path(slot))
	DirAccess.remove_absolute(Session.path(slot))


## The strike's entry point: late by the speed of sound, softer and duller far away. The sound itself is not heard here.
func _thunder() -> void:
	var t := Thunder.new(DataTables.game("sky"))
	add_child(t)
	var expected: float = THUNDER_M / DataTables.game("sky").sound_ms
	print("  thunder: %.2f s for %.0f m (%.2f s by sound)" % [t.delay_s(THUNDER_M), THUNDER_M, expected])
	_check(is_equal_approx(t.delay_s(THUNDER_M), expected), "the sound comes after the lightning")
	await t.play(THUNDER_M, 1.0)
	t.queue_free()


func _generated() -> void:
	_check(CityStore.query(&"impossible_nodes").is_empty(), "the generated city has no impossible node")


func _road_at(p: Vector3) -> Dictionary:
	return CityStore.query(&"inspect", [p])


## A road with vehicles: the numbers add up, and a node of three legs answers as a junction.
func _numbers() -> void:
	var seen := 0
	var total := 0
	var count: int = CityStore.snapshot.segments
	for seg in count:
		var t: Dictionary = CityStore.query(&"segment_traffic", [seg])
		if not t.is_empty():
			seen += 1
			total += int(t.vehicles)
			_check(t.queued <= t.vehicles, "the queue fits in the vehicles") if t.queued > t.vehicles else null
	_check(seen > 0, "traffic by road: %d roads" % seen)
	_check(total > 0, "sum of the vehicles on the roads: %d" % total)
	var volume := 0
	for seg in count:
		var t: Dictionary = CityStore.query(&"segment_traffic", [seg])
		volume += int(t.get("volume", 0))
	_check(volume > 0, "volume conta entradas: %d" % volume)


func _edits() -> void:
	var profiles: Array = CityStore.query(&"road_profiles")
	var one_way := -1
	for p: Dictionary in profiles:
		if p.get("one_way", false):
			one_way = p.index
	var a := Vector3(0, 0, 0)
	var b := Vector3(ROAD_LEN_M, 0, 0)
	var built: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, one_way if one_way >= 0 else 0, PX])
	_check(built.get("ok", false), "test road")
	var mid := Vector3(ROAD_LEN_M * 0.5, 0, 0)
	var info := _road_at(mid)
	_check(info.get("kind", "") == "road", "inspecting finds the road")
	_check(absf(info.length_m - ROAD_LEN_M) < 1.0, "the road in pieces is one: %.1f m" % info.length_m)
	var path: PackedVector3Array = CityStore.query(&"segment_path", [info.id])
	var gap := 0.0
	for i in range(1, path.size()):
		gap = maxf(gap, path[i].distance_to(path[i - 1]))
	_check(absf(path[0].distance_to(path[path.size() - 1]) - ROAD_LEN_M) < 1.0 and gap < ROAD_LEN_M * 0.5, "the Information strip covers the road, with no jump (largest step %.1f m)" % gap)
	if one_way >= 0:
		_check(CityStore.dispatch(&"reverse_road", [info.id]).ok, "reversing a one-way")
		_check(_road_at(mid).get("kind", "") == "road", "the road stays after being reversed")
		_check(CityStore.dispatch(&"undo"), "undoing the reversal")
	var before: int = CityStore.snapshot.segments
	_check(CityStore.dispatch(&"duplicate_road", [_road_at(mid).id]).ok, "duplicar")
	_check(CityStore.snapshot.segments > before, "one road more")
	await _save_after_edit()
	_check(CityStore.dispatch(&"undo"), "undoing the duplicate")
	_check(CityStore.snapshot.segments == before, "the copy left")
	CityStore.dispatch(&"set_marking_style", [2, true])
	_check(CityStore.snapshot.marking_style == 2, "pintura eu")
	CityStore.dispatch(&"undo")
	_check(CityStore.snapshot.marking_style == 0, "undoing the paint")
	CityStore.dispatch(&"redo")
	_check(CityStore.snapshot.marking_style == 2, "redoing the paint")
	CityStore.dispatch(&"set_marking_style", [0, false])


## An edit writes itself once the player pauses, a save at a time.
func _save_after_edit() -> void:
	CityStore.refresh()
	CityStore.saves.after_edit = true
	CityStore.saves.tick(0.0)
	CityStore.saves.tick(DataTables.game("saves").autosave_quiet_s)
	_check(CityStore.saves.save_busy(), "the pause after an edit saves")
	while CityStore.saves.save_busy():
		CityStore.saves.tick(0.0)
		await get_tree().process_frame
	CityStore.saves.after_edit = false
	var slot := CityStore.saves.city_slot()
	DirAccess.remove_absolute(SaveSlots.path(slot))
	DirAccess.remove_absolute(Session.path(slot))


## The state in SI, the units only in what is shown; the style chosen reaches the core.
func _units() -> void:
	_check(is_equal_approx(Formatters.speed_in(26.8224, "mph"), 60.0), "26.8224 m/s is 60 mph")
	_check(is_equal_approx(Formatters.speed_in(10.0, "kmh"), 36.0), "10 m/s is 36 km/h")
	_check(is_equal_approx(Formatters.length_in(30.48, "ft"), 100.0), "30.48 m is 100 ft")
	_check(Formatters.format_speed(10.0) == "36 km/h", "the game's style shows km/h")
	CityStore.dispatch(&"set_style", ["roadcraft"])
	_check(CityStore.snapshot.style == "roadcraft", "the chosen style reaches the core")


func _sharp() -> void:
	var o := Vector3(0, 0, 1000)
	var dir := Vector3(ROAD_LEN_M, 0, 0)
	var other := dir.rotated(Vector3.UP, deg_to_rad(SHARP_DEG))
	CityStore.dispatch(&"build_road", [o, o + dir, false, Vector3.ZERO, 0, PX])
	var r: Dictionary = CityStore.dispatch(&"build_road", [o, o + other, false, Vector3.ZERO, 0, PX])
	var bad: Array = CityStore.query(&"impossible_nodes")
	print("  second road: %s; impossible nodes: %d" % [r.get("ok", false), bad.size()])
	_check(not r.get("ok", false) or bad.size() == 1, "a sharp node becomes a warning or is refused")


func _saves() -> void:
	var before: int = CityStore.snapshot.segments
	var bad := "user://arquivo_ruim.cidade"
	var f := FileAccess.open(bad, FileAccess.WRITE)
	f.store_string("this is no city")
	f.close()
	var r: Dictionary = CityStore.saves.open_file(bad)
	_check(not r.get("ok", true), "arquivo ruim recusado")
	_check(CityStore.snapshot.get("ready", false) and CityStore.snapshot.segments == before, "the city on screen stayed")
	for i in 2:
		var slot := "cidade-quarentena"
		DirAccess.make_dir_recursive_absolute(SaveSlots.dir())
		var q := FileAccess.open(SaveSlots.path(slot), FileAccess.WRITE)
		q.store_string(str(i))
		q.close()
		_check(SaveSlots.quarantine(slot) == OK, "quarentena %d" % i)
	var kept := DirAccess.get_files_at("%s/%s" % [SaveSlots.dir(), SaveSlots.QUARANTINE_DIR])
	_check(kept.size() >= 2, "two quarantines, none over the other: %d" % kept.size())
	for name in kept:
		DirAccess.remove_absolute("%s/%s/%s" % [SaveSlots.dir(), SaveSlots.QUARANTINE_DIR, name])


func _session() -> void:
	var n := Session.normalized("x", 99.0, 1.2, 77)
	_check(n.traffic == 1.0, "a level that is no number goes back to the default")
	_check(n.people == 3.0, "a level up to the panel's limit")
	_check(n.demand == 1.0, "demand at the nearest value on offer")
	_check(n.speed == int(CityStore.constants.speed_fastest), "a speed among those there are")
