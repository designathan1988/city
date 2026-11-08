## Random edits against what must hold for any of them, on the state hash (probes/state_hash: the save's bytes): undoing every step
## walks back through the same hashes, redoing walks forward through them, a save opened again is the same city, and the same seed
## makes the same city. A failure prints its seed and step; `--seed=N` and `--steps=N` run that one again.
extends TestCase

const BodyEdits := preload("res://tests/core/fuzz_body_edits.gd")
const BrushEdits := preload("res://tests/core/fuzz_brush_edits.gd")
const LOT_M := 30.0
const MAP := 0
const CITY_SEED := 7
const SEEDS := [1, 2, 3]
const STEPS := 40
const GRID_M := 40.0
const CELLS := 12
const FIELD_OUT := Vector3(1600, 0, 900)
const PX := 0.25
const LIFTS := [0.0, 0.0, 0.0, 6.0, 8.0, -4.0, -12.0]
const ZONE_RADIUS := 30.0
const SLOT := "edit_fuzz"
const SHOWN_PARTS := 6
const BRUSH_M := 30.0
const BRUSH_DT := 0.2
const BRUSH_MODES := 3
const ITEM_KINDS := 7
const PLAN_SHAPES := 8
const REACH_M := 12.0
const STRIKE_MAX := 12
## The cells waiting to grow are a queue, not an edit: an undone road leaves its cells there, and a cell with no lot leaves it alone.
const GROWTH_QUEUE := "growth"

var drawn := -1
var rng := RandomNumberGenerator.new()
var roads: Array[Vector3] = [] # the middles of what was laid, for the edits that need a road
var last_kind := 0
## The edits the fuzz draws from, by what each does (every number below DEMOLISH lays a road: five chances in the draw).
enum Edit { LAY, DEMOLISH = 5, SPLIT, UPGRADE, ZONE, REVERSE, TERRAIN, ITEM, BARRIER, BUILDING, STOP, BLAST, LOT, LANE_LINK, BEND, NODE_HEIGHT,
	JOIN, CROSSING, UTURN, BAN_TURN, REMOVE_NODE, PATH }


const BEND_M := 30.0
const BEND_AT_LEAST := 0.2
const BEND_LIFT_M := 4.0


func _ready() -> void:
	if not boot():
		finish()
		return
	var seeds: Array = SEEDS
	var steps := STEPS
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--seed="):
			seeds = [arg.trim_prefix("--seed=").to_int()]
		if arg.begins_with("--steps="):
			steps = arg.trim_prefix("--steps=").to_int()
	for s: int in seeds:
		await _run(s, steps)
	finish()


func _hash() -> Dictionary:
	return CityStore.query(&"state_hash")


## `ignoring`: a part that may differ, said by the caller.
func _same(a: Dictionary, b: Dictionary, what: String, ignoring := "") -> void:
	var differ := []
	for part: String in a.parts:
		if a.parts[part] != b.parts.get(part) and part != ignoring:
			differ.push_back(part)
	if not differ.is_empty():
		check(false, "%s: differs in %s (%d -> %d bytes)" % [what, differ.slice(0, SHOWN_PARTS), a.bytes, b.bytes])


func _run(seed: int, steps: int) -> void:
	var trail := _walk(seed, steps)
	print("seed %d: %d edits kept of %d" % [seed, trail.size() - 1, steps])
	var last := trail.size() - 1
	for i in range(last, 0, -1):
		CityStore.dispatch(&"undo")
		_same(trail[i - 1], _hash(), "seed %d undo to step %d" % [seed, i - 1], GROWTH_QUEUE)
	for i in range(1, last + 1):
		CityStore.dispatch(&"redo")
		_same(trail[i], _hash(), "seed %d redo to step %d" % [seed, i], GROWTH_QUEUE)
	await _reopened(seed, trail[last])
	var again := _walk(seed, steps)
	_same(trail[last], again[-1], "seed %d the same edits again" % seed)
	CityStore.query(&"traffic_hashes", [1]) # the graph takes the edits
	var fault: String = CityStore.query(&"graph_fault")
	check(fault.is_empty(), "seed %d lane graph: %s" % [seed, fault])


func _reopened(seed: int, before: Dictionary) -> void:
	var slot := "%s_%d" % [SLOT, seed]
	var saved: Dictionary = await save(slot)
	check(saved.get("ok", false), "seed %d is written: %s" % [seed, saved])
	var r: Dictionary = await CityStore.saves.load_game(slot, true)
	check(r.get("ok", false), "seed %d opens: %s" % [seed, r])
	_same(before, _hash(), "seed %d save and open" % seed)
	DirAccess.remove_absolute(CityStore.saves.save_path(slot))


## A new map and the seed's edits; the hash after each edit that took.
func _walk(seed: int, steps: int) -> Array:
	new_map(MAP, CITY_SEED, false)
	rng.seed = seed
	roads.clear()
	drawn = -1
	var trail := [_hash()]
	for i in steps:
		var before: String = trail[-1].whole
		var steps_before: int = CityStore.query(&"undo_steps")
		_edit()
		CityStore.dispatch(&"set_road_height", [0.0])
		var now := _hash()
		if int(CityStore.query(&"undo_steps")) != steps_before and now.whole == before:
			check(false, "seed %d an edit of kind %d left an undo step that changes nothing" % [seed, last_kind])
		if now.whole != before:
			trail.push_back(now)
	return trail


func _point() -> Vector3:
	return FIELD_OUT + Vector3(rng.randi_range(0, CELLS) * GRID_M, 0, rng.randi_range(0, CELLS) * GRID_M)


func _edit() -> void:
	var kind := rng.randi_range(0, Edit.PATH) # PATH: a held brush's pointer move, the dabs of one move as one command
	last_kind = kind
	var road: Vector3 = roads[rng.randi_range(0, roads.size() - 1)] if not roads.is_empty() else Vector3.ZERO
	if kind < Edit.DEMOLISH or roads.is_empty():
		_lay()
	elif kind == Edit.PATH:
		BrushEdits.run(rng, _point(), GRID_M, BRUSH_M, BRUSH_MODES, ZONE_RADIUS, road)
	elif kind >= Edit.BEND: # what the Inspector does to a road's shape
		_shape(kind, road)
	elif kind >= Edit.TERRAIN: # what is not a road
		_other(kind, road)
	elif kind == Edit.DEMOLISH:
		CityStore.dispatch(&"demolish", [road])
	elif kind == Edit.SPLIT:
		CityStore.dispatch(&"split_road", [road])
	elif kind == Edit.UPGRADE:
		CityStore.dispatch(&"upgrade_road", [road, rng.randi_range(0, 3), false])
	elif kind == Edit.ZONE:
		CityStore.dispatch(&"begin_stroke", ["cmd.place.zone"])
		CityStore.dispatch(&"paint_zone", [road, ZONE_RADIUS, ZoneTable.code("residential")])
		CityStore.dispatch(&"end_stroke")
	else:
		var seg: Dictionary = CityStore.query(&"segment_at", [road])
		if seg.get("id", -1) >= 0:
			CityStore.dispatch(&"reverse_road", [seg.id])


## What the Inspector does to a road's shape.
func _shape(kind: int, road: Vector3) -> void:
	var seg: Dictionary = CityStore.query(&"segment_at", [road])
	var node: int = CityStore.query(&"node_at", [road, PX]).get("node", -1)
	if kind == Edit.BEND and seg.get("id", -1) >= 0:
		CityStore.dispatch(&"bend_road", [seg.id, rng.randf_range(-BEND_M, BEND_M), rng.randf_range(BEND_AT_LEAST, 1.0 - BEND_AT_LEAST)])
	elif kind == Edit.NODE_HEIGHT and node >= 0:
		CityStore.dispatch(&"set_node_height", [node, rng.randf_range(-BEND_LIFT_M, BEND_LIFT_M)])
	elif kind == Edit.JOIN and node >= 0:
		CityStore.dispatch(&"join_roads", [node])
	elif kind == Edit.CROSSING:
		CityStore.dispatch(&"add_crossing", [road, rng.randi_range(CityStore.code(&"NodeCrossing", &"CROSSING_ZEBRA"), CityStore.code(&"NodeCrossing", &"CROSSING_SIGNAL"))])
	elif kind == Edit.UTURN:
		CityStore.dispatch(&"add_uturn", [road, rng.randf() < 0.5])
	elif kind == Edit.BAN_TURN and node >= 0:
		var legs: Array = CityStore.query(&"node_legs", [node])
		if legs.size() >= 2:
			CityStore.dispatch(&"set_movement_banned", [node, legs[0].seg, legs[1].seg, legs[0].banned.is_empty()])
	elif kind == Edit.REMOVE_NODE and node >= 0:
		CityStore.dispatch(&"remove_node", [node])


## What is not a road: relief, street pieces, a barrier, a drawn building, a stop, a bomb, the demolish box.
func _other(kind: int, road: Vector3) -> void:
	var a := _point()
	var b := a + Vector3(GRID_M, 0, GRID_M)
	match kind:
		Edit.TERRAIN:
			CityStore.dispatch(&"begin_stroke", ["cmd.place.terrainBrush"])
			CityStore.dispatch(&"terrain_brush", [a, BRUSH_M, 1.0, BRUSH_DT, rng.randi_range(0, BRUSH_MODES), 0.5])
			CityStore.dispatch(&"end_stroke")
		Edit.ITEM:
			CityStore.dispatch(&"place_item", [road, rng.randi_range(0, ITEM_KINDS), 0, "", REACH_M])
		Edit.BARRIER:
			CityStore.dispatch(&"build_barrier", [PackedVector3Array([a, b]), rng.randi_range(0, 1)])
		Edit.BUILDING:
			var box: PackedVector3Array = CityStore.query(&"body_outline", [drawn, 0]) if drawn >= 0 else PackedVector3Array()
			if not box.is_empty() and rng.randf() < 0.5:
				BodyEdits.run(rng, drawn, box)
			else:
				var r: Dictionary = CityStore.dispatch(&"draw_building", [a, b, 0, ZoneTable.code("residential"), rng.randi_range(0, PLAN_SHAPES)])
				drawn = r.get("placed", -1) if r.get("ok", false) else drawn
		Edit.STOP:
			CityStore.dispatch(&"transit_stop", [road])
		Edit.LANE_LINK:
			var junctions: Array = CityStore.query(&"junctions")
			if not junctions.is_empty():
				var node: int = junctions[rng.randi_range(0, junctions.size() - 1)].node
				var built: Array = CityStore.query(&"node_lanes", [node]).built
				if not built.is_empty():
					var one: Array = built[rng.randi_range(0, built.size() - 1)]
					CityStore.dispatch(&"toggle_lane_link", [node, one[0], one[1], one[2], one[3]])
		Edit.LOT:
			var lots: Array = CityStore.query(&"lots")
			if lots.is_empty() or rng.randf() < 0.5:
				CityStore.dispatch(&"lot_add_rect", [a, a + Vector3(LOT_M, 0, LOT_M), 0.0])
			else:
				CityStore.dispatch(&"lot_zone", [lots[rng.randi_range(0, lots.size() - 1)].id, ZoneTable.code("residential")])
		_: # Edit.BLAST
			if rng.randf() < 0.5:
				CityStore.dispatch(&"strike", [road, rng.randi_range(1, STRIKE_MAX)])
			else:
				CityStore.dispatch(&"demolish_box", [a, b])


func _lay() -> void:
	var a := _point()
	var b := _point()
	if a.is_equal_approx(b):
		return
	var lift: float = LIFTS[rng.randi_range(0, LIFTS.size() - 1)]
	CityStore.dispatch(&"set_road_walls", [rng.randf() < 0.5])
	CityStore.dispatch(&"set_road_height", [LIFTS[rng.randi_range(0, LIFTS.size() - 1)]])
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, rng.randi_range(0, 3), PX, lift])
	if r.get("ok", false):
		roads.push_back((a + b) * 0.5)
