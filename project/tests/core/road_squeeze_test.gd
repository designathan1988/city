## Core test of the tight stretch through the store: the road tool's preview says what the build refuses, with the build's own
## refusal (a stretch too short for its two junctions to open out, sim/commands/road_rules.cpp plan_squeezed): a link between two
## avenues too close, a road crossing two, a crossing too near a junction; and an open link passes both; on open ground past the town.
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
const OPEN_GAP_M := 80.0
const CROSS_OFF_M := 1200.0
const NEAR_OFF_M := 1500.0
const PAST_M := 30.0
const SQUEEZED := "hint.rule.squeezed"


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
	_near_junction(e + Vector3(0, 0, NEAR_OFF_M), avenue)
	finish()


## The road from a to b as the tool hands it to the preview: one straight piece (start, control, end).
func _preview(a: Vector3, b: Vector3, profile: int) -> Dictionary:
	return CityStore.dispatch(&"preview_road_path", [PackedVector3Array([a, (a + b) * 0.5, b]), profile, PX, 0.0])


func _build(a: Vector3, b: Vector3, profile: int) -> Dictionary:
	return CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, profile, PX])


## The preview of a to b and the build of it give the same answer: both refuse with the squeeze, or both pass.
func _agree(a: Vector3, b: Vector3, profile: int, refused: bool, what: String) -> void:
	var shown := _preview(a, b, profile)
	if refused:
		check(not shown.get("ok", true) and shown.get("error", "") == SQUEEZED, "the preview warns: %s: %s" % [what, shown])
	else:
		check(shown.get("ok", false), "the preview passes: %s: %s" % [what, shown])
	var built := _build(a, b, profile)
	if refused:
		check(not built.get("ok", true) and built.get("error", "") == SQUEEZED, "and the build refuses: %s: %s" % [what, built])
	else:
		check(built.get("ok", false), "and the build passes: %s: %s" % [what, built])


## Three parallel avenues: a 20 m link is warned of and refused alike; an 80 m one passes both.
func _ending(c: Vector3, avenue: int) -> void:
	var d := c + Vector3(0, 0, TIGHT_GAP_M)
	var f := d + Vector3(0, 0, OPEN_GAP_M)
	for at: Vector3 in [c, d, f]:
		_build(at, at + Vector3(LEN_M, 0, 0), avenue)
	var mid := c + Vector3(LEN_M * 0.5, 0, 0)
	_agree(mid, mid + Vector3(0, 0, TIGHT_GAP_M), avenue, true, "a 20 m link")
	var lower := mid + Vector3(0, 0, TIGHT_GAP_M)
	_agree(lower, lower + Vector3(0, 0, OPEN_GAP_M), avenue, false, "an 80 m link")


## A road drawn across two parallel avenues too close: the stretch between its two crossings.
func _crossing(c: Vector3, avenue: int) -> void:
	var d := c + Vector3(0, 0, TIGHT_GAP_M)
	for at: Vector3 in [c, d]:
		_build(at, at + Vector3(LEN_M, 0, 0), avenue)
	_agree(c + Vector3(LEN_M * 0.5, 0, -PAST_M), d + Vector3(LEN_M * 0.5, 0, PAST_M), avenue, true, "across two avenues")


## An avenue with a side road: a road crossing it 20 m from that junction leaves the avenue too short between the two.
func _near_junction(c: Vector3, avenue: int) -> void:
	_build(c, c + Vector3(LEN_M, 0, 0), avenue)
	var mid := c + Vector3(LEN_M * 0.5, 0, 0)
	_build(mid, mid + Vector3(0, 0, LEN_M * 0.5), avenue)
	var x := mid + Vector3(TIGHT_GAP_M, 0, 0)
	_agree(x + Vector3(0, 0, -PAST_M), x + Vector3(0, 0, PAST_M), avenue, true, "a crossing 20 m from a junction")
