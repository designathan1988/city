## Core test of the tight stretch through the store: the road tool's preview says what the build refuses (a stretch too short for
## its two junctions to open out, sim/commands/road_rules.cpp plan_squeezed), for a road ending on another and for one crossing
## two, and lets an open stretch pass; on open ground past the town.
## Run: godot --headless --path project res://tests/core/road_squeeze_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const OUT_M := 460.0
## Metres a screen pixel covers (the tool asks the camera; here a close view).
const PX := 0.25
const OFF_M := 900.0
const LEN_M := 120.0
## Two avenues this far apart leave no room for a link's two junctions; this far apart they do.
const TIGHT_GAP_M := 20.0
const OPEN_GAP_M := 90.0
const CROSS_OFF_M := 1200.0
const PAST_M := 30.0


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var b: Dictionary = CityStore.query(&"city_bounds")
	var e := Vector3(b.max.x + OUT_M, 0, (b.min.z + b.max.z) * 0.5)
	var avenue: int = CityStore.query(&"road_profiles").filter(func(p: Dictionary) -> bool: return p.id == "avenue")[0].index
	_ending(e + Vector3(0, 0, OFF_M), avenue)
	_crossing(e + Vector3(0, 0, CROSS_OFF_M), avenue)
	finish()


## The road from a to b as the tool hands it to the preview: one straight piece (start, control, end).
func _preview(a: Vector3, b: Vector3, profile: int) -> Dictionary:
	return CityStore.dispatch(&"preview_road_path", [PackedVector3Array([a, (a + b) * 0.5, b]), profile, PX, 0.0])


## Two parallel avenues: a link between them too close is warned of and refused alike; far enough apart it passes both.
func _ending(c: Vector3, avenue: int) -> void:
	var d := c + Vector3(0, 0, TIGHT_GAP_M)
	var f := c + Vector3(0, 0, OPEN_GAP_M)
	for at: Vector3 in [c, d, f]:
		CityStore.dispatch(&"build_road", [at, at + Vector3(LEN_M, 0, 0), false, Vector3.ZERO, avenue, PX])
	var mid := c + Vector3(LEN_M * 0.5, 0, 0)
	var shown := _preview(mid, mid + Vector3(0, 0, TIGHT_GAP_M), avenue)
	check(not shown.get("ok", true) and shown.get("error", "") == "road.reason.squeezed", "the preview warns of the tight stretch: %s" % shown)
	var built: Dictionary = CityStore.dispatch(&"build_road", [mid, mid + Vector3(0, 0, TIGHT_GAP_M), false, Vector3.ZERO, avenue, PX])
	check(not built.get("ok", true), "and the build refuses it: %s" % built)
	var lower := mid + Vector3(0, 0, TIGHT_GAP_M)
	var open := _preview(lower, lower + Vector3(0, 0, OPEN_GAP_M - TIGHT_GAP_M), avenue)
	check(open.get("ok", false), "an open stretch passes the preview: %s" % open)
	built = CityStore.dispatch(&"build_road", [lower, lower + Vector3(0, 0, OPEN_GAP_M - TIGHT_GAP_M), false, Vector3.ZERO, avenue, PX])
	check(built.get("ok", false), "and the build: %s" % built)


## A road drawn across two parallel avenues too close: the stretch between its two crossings is warned of and refused.
func _crossing(c: Vector3, avenue: int) -> void:
	var d := c + Vector3(0, 0, TIGHT_GAP_M)
	for at: Vector3 in [c, d]:
		CityStore.dispatch(&"build_road", [at, at + Vector3(LEN_M, 0, 0), false, Vector3.ZERO, avenue, PX])
	var from := c + Vector3(LEN_M * 0.5, 0, -PAST_M)
	var to := d + Vector3(LEN_M * 0.5, 0, PAST_M)
	var shown := _preview(from, to, avenue)
	check(not shown.get("ok", true) and shown.get("error", "") == "road.reason.squeezed", "the preview warns across two roads: %s" % shown)
	var built: Dictionary = CityStore.dispatch(&"build_road", [from, to, false, Vector3.ZERO, avenue, PX])
	check(not built.get("ok", true), "and the build refuses it: %s" % built)
