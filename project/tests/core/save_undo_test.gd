## Core test of the history and the save file through the store (no rendering needed): every edit kind is undone to
## exactly the state before it and redone to the state after it (road, demolish, zone stroke, relief stroke, upgrade,
## split, roundabout, drawn building, and the Landscape: weather, cloud, fog, painted ground, planted trees); then a city with edits, grown buildings and sculpted ground is saved, a new map
## replaces it, and opening the file brings back the same counts, money, clock, buildings, heights and landscape.
## Run: godot --headless --path project res://tests/core/save_undo_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const OTHER_SEED := 7
const VARIANT_OFF := Vector3(40, 0, 90) # clear of the city: no generated road end beside it
const PROFILE := 0
const UPGRADE_PROFILE := 3
const OUT_M := 260.0
const ZONE_RADIUS := 60.0
const ZONE_RES := 1
const BRUSH_RADIUS := 60.0
const BRUSH_STRENGTH := 0.4
const BRUSH_DT := 0.5
const BRUSH_DABS := 12
const RING_RADIUS := 22.0
const RING_OUT_M := 700.0
const LOT_M := 30.0
const GROW_TICKS := 600
const PARK_FN := 6
const SLOT := "round_trip_test"
const MONEY_STEP := 0.01
## The balance and an edit of the open city, which a file refused on opening must leave as they were.
const KEPT_MONEY := 123456.0
const MARKING_STYLES := 2
const NATURE_OUT_M := 900.0
const RAIN := 0.7
const PAINT_KIND := 0
const TREES := {"density": 200.0, "height": 14.0, "variation": 0.3, "spacing": 6.0}
const KEYS := ["segments", "nodes", "buildings", "money", "zoned_sides"]


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var b: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(b.max.x - 1.0, 0, (b.min.z + b.max.z) * 0.5)
	var e := a + Vector3(OUT_M, 0, 0)
	_undo_cases(a, e)
	SignalCases.new(self).run()
	await _save_round_trip(a, e)
	await _refuses_bad_balance()
	finish()


func _undo_cases(a: Vector3, e: Vector3) -> void:
	var mid := (a + e) * 0.5
	_round("road", func() -> Dictionary: return CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, PROFILE, 0.25]))
	var four: int = CityStore.dispatch(&"road_variant", [CityStore.first_road(), 4, 0.0, 0])
	_round("road with a variant", func() -> Dictionary:
		return CityStore.dispatch(&"build_road", [a + VARIANT_OFF, e + VARIANT_OFF, false, Vector3.ZERO, four, 0.25]))
	_round("zone (stroke)", func() -> Dictionary:
		CityStore.dispatch(&"begin_stroke", ["cmd.place.zone"])
		var r: Dictionary = CityStore.dispatch(&"paint_zone", [mid, ZONE_RADIUS, ZONE_RES])
		CityStore.dispatch(&"paint_zone", [mid + Vector3(ZONE_RADIUS, 0, 0), ZONE_RADIUS, ZONE_RES])
		CityStore.dispatch(&"end_stroke")
		return r)
	_round("upgrade", func() -> Dictionary: return CityStore.dispatch(&"upgrade_road", [mid, UPGRADE_PROFILE, true]))
	_round("split", func() -> Dictionary: return CityStore.dispatch(&"split_road", [mid]))
	_round("demolish", func() -> Dictionary: return CityStore.dispatch(&"demolish", [mid]))
	var ring := e + Vector3(RING_OUT_M, 0, 0)
	_round("roundabout", func() -> Dictionary: return CityStore.dispatch(&"roundabout", [ring, RING_RADIUS, false]))
	var lot := mid + Vector3(0, 0, LOT_M)
	_round("drawn building", func() -> Dictionary:
		return CityStore.dispatch(&"draw_building", [lot, lot + Vector3(LOT_M, 0, LOT_M), PARK_FN, ZONE_RES, 0]))
	var hill := _hill(e)
	var h0 := _height(hill)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.terrainBrush"])
	for i in BRUSH_DABS:
		CityStore.dispatch(&"terrain_brush", [hill, BRUSH_RADIUS, BRUSH_STRENGTH, BRUSH_DT, 0, 0.0])
	CityStore.dispatch(&"end_stroke")
	var h1 := _height(hill)
	check(h1 > h0 + 1.0, "the relief rose %.2f -> %.2f" % [h0, h1])
	CityStore.dispatch(&"undo")
	check(is_equal_approx(_height(hill), h0), "undoing relief returns to %.2f (%.2f)" % [h0, _height(hill)])
	CityStore.dispatch(&"redo")
	check(is_equal_approx(_height(hill), h1), "redoing relief returns to %.2f" % h1)
	_nature_cases(hill + Vector3(NATURE_OUT_M, 0, 0))


func _nature_cases(p: Vector3) -> void:
	_nature_round("weather", func() -> void: CityStore.dispatch(&"set_weather", [{"rain": RAIN}]))
	_nature_round("cloud", func() -> void: CityStore.dispatch(&"add_cloud", [_cloud(p)]))
	_stroke("fog", "cmd.fog", &"fog_brush", [p, BRUSH_RADIUS, 0.5, 18.0, 4.0, false])
	_stroke("painted ground", "cmd.place.paintGround", &"paint_brush", [p, BRUSH_RADIUS, 1.0, 0.0, PAINT_KIND])
	_stroke("trees", "cmd.place.tree", &"tree_brush", [p, BRUSH_RADIUS, 0, 0, TREES])


func _cloud(p: Vector3) -> Dictionary:
	return {"x": p.x, "z": p.z, "height": 180.0, "size": 150.0, "density": 0.8, "yaw": 0.0}


func _stroke(what: String, label: String, command: StringName, args: Array) -> void:
	_nature_round(what, func() -> void:
		CityStore.dispatch(&"begin_stroke", [label])
		CityStore.dispatch(command, args)
		CityStore.dispatch(&"end_stroke"))


## Runs a Landscape edit, checks undo returns to the landscape before it and redo to the landscape after it.
func _nature_round(what: String, edit: Callable) -> void:
	var before := _landscape()
	edit.call()
	var after := _landscape()
	check(after != before, "%s changed %s" % [what, after])
	CityStore.dispatch(&"undo")
	check(_landscape() == before, "%s undone %s" % [what, _landscape()])
	CityStore.dispatch(&"redo")
	check(_landscape() == after, "%s redone %s" % [what, _landscape()])


func _landscape() -> Dictionary:
	var l: Dictionary = CityStore.query(&"landscape")
	return {"rain": l.weather.rain, "clouds": l.clouds.size(), "fog": l.fog_dabs, "planted": l.planted, "tiles": l.paint_tiles}


## Free ground for the relief brush: beside the far end of the new road, away from the roundabout.
func _hill(e: Vector3) -> Vector3:
	return e + Vector3(RING_OUT_M * 0.5, 0, RING_OUT_M * 0.5)


## Runs an edit, checks undo returns to the counts before it and redo to the counts after it.
func _round(what: String, edit: Callable) -> void:
	var before := _counts()
	var r: Dictionary = edit.call()
	var after := _counts()
	check(r.get("ok", false) and after != before, "%s: %s" % [what, r])
	check(CityStore.dispatch(&"undo") and _counts() == before, "%s undone %s" % [what, _counts()])
	check(CityStore.dispatch(&"redo") and _counts() == after, "%s redone %s" % [what, _counts()])


## A save whose balance is not a number is not a city (the 4.0's readEconomy).
func _refuses_bad_balance() -> void:
	CityStore.query(&"probe_set_money", [INF])
	var w: Dictionary = await save(SLOT)
	check(w.get("ok", false), "saved an infinite balance for the test")
	CityStore.query(&"probe_set_money", [KEPT_MONEY])
	CityStore.dispatch(&"set_marking_style", [(int(CityStore.snapshot.marking_style) + 1) % MARKING_STYLES, true])
	var before := _counts()
	var could_undo: bool = CityStore.snapshot.can_undo
	var r: Dictionary = CityStore.saves.load_game(SLOT, true)
	check(not r.get("ok", false) and r.get("error", "") == "save.file.damaged", "an infinite balance refused: %s" % r)
	check(could_undo and CityStore.snapshot.can_undo and _counts() == before, "the open city stayed as it was, with what to undo: %s == %s" % [_counts(), before])
	_drop(SLOT)


## What a slot leaves on disk, the save and its session, goes: the next run starts from none.
func _drop(slot: String) -> void:
	DirAccess.remove_absolute(CityStore.saves.save_path(slot))
	DirAccess.remove_absolute(Session.path(slot))


func _save_round_trip(a: Vector3, e: Vector3) -> void:
	CityStore.dispatch(&"paint_zone", [(a + e) * 0.5, ZONE_RADIUS, ZONE_RES])
	ticks(GROW_TICKS)
	var sculpted := _hill(e)
	var saved := _counts()
	saved.grown = CityStore.snapshot.grown
	saved.minutes = CityStore.snapshot.minutes
	var heights := [_height(sculpted), _height(a), _height((a + e) * 0.5)]
	var probes := _probes()
	var signal_at: Vector3 = CityStore.query(&"traffic_probe", [0]).get("busy_crossing", Vector3.ZERO)
	var plan: Dictionary = CityStore.query(&"inspect", [signal_at])
	if plan.get("kind", "") == "node" and plan.has("greens"):
		CityStore.dispatch(&"set_signal", [plan.id, true, plan.greens, SignalCases.SIGNAL_OFFSET])
		plan = CityStore.query(&"inspect", [signal_at])
	var landscape := _landscape()
	check(landscape.planted > 0 and landscape.tiles > 0, "there is landscape to save %s" % landscape)
	var w: Dictionary = await save(SLOT)
	check(w.get("ok", false) and not CityStore.snapshot.unsaved, "saved %s" % w)
	new_map(TOWN_MAP, OTHER_SEED, false)
	check(_counts() != saved, "a new map is another")
	var t0 := Time.get_ticks_usec()
	var r: Dictionary = CityStore.saves.load_game(SLOT, true)
	print("opening: %d ms" % ((Time.get_ticks_usec() - t0) / 1000))
	var got := _counts()
	got.grown = CityStore.snapshot.grown
	got.minutes = CityStore.snapshot.minutes
	check(r.ok, "opened %s" % r)
	check(not CityStore.snapshot.unsaved and not CityStore.snapshot.can_undo, "opened with no work to save and no history (to save: %s, undo: %s)" % [CityStore.snapshot.unsaved, CityStore.snapshot.can_undo])
	CityStore.dispatch(&"set_marking_style", [CityStore.snapshot.marking_style, false])
	check(not CityStore.snapshot.unsaved, "the same paint, put again as the session does on opening, does not change the city")
	check(got == saved, "the same numbers %s == %s" % [got, saved])
	check([_height(sculpted), _height(a), _height((a + e) * 0.5)] == heights, "the same heights %s" % [heights])
	check(_probes() == probes, "the same buildings (%d probed)" % probes.size())
	ticks(SignalCases.SIGNAL_TICKS)
	check(CityStore.query(&"inspect", [signal_at]) == plan, "the same signal %s" % [plan])
	check(_landscape() == landscape, "the same landscape %s == %s" % [_landscape(), landscape])
	await SaveLandCases.run(self, SLOT)
	_drop(SLOT)


func _counts() -> Dictionary:
	var s := CityStore.snapshot
	var out := {}
	for k in KEYS:
		out[k] = s[k]
	out.money = snappedf(s.money, MONEY_STEP)
	return out


func _probes() -> Array:
	var out := []
	for id in CityStore.query(&"list_buildings"):
		out.append(CityStore.query(&"probe_building", [id]))
	return out


func _height(p: Vector3) -> float:
	var hit: Dictionary = CityStore.query(&"raycast", [Vector3(p.x, 2000, p.z), Vector3.DOWN])
	return hit.position.y if hit.get("hit", false) else NAN
