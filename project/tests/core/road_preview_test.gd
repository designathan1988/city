## Core test of the road tool's preview against its build (the lab's roadTool.ts verdict: the preview is a dry run of the commit):
## a stretch squeezed between junctions is said before the road is laid, by the preview, with the same refusal the build gives,
## and a road with room for its junctions is previewed and built alike. On open ground past the town.
## Run: godot --headless --path project res://tests/core/road_preview_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const OUT_M := 460.0
## Metres a screen pixel covers (the tool asks the camera; here a close view).
const PX := 0.25
const ROAD_M := 120.0 # the two parallel avenues
const TIGHT_GAP_M := 20.0 # two avenues this far apart leave no room for the junctions of a road between them
const ROOMY_GAP_M := 80.0 # and this far apart, room enough
const ROOMY_OFF_M := 200.0 # the second pair, clear of the first
const CROSS_OFF_M := 400.0 # the crossing case, clear of both
const SQUEEZED := "hint.rule.squeezed"


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var b: Dictionary = CityStore.query(&"city_bounds")
	var e := Vector3(b.max.x + OUT_M, 0, (b.min.z + b.max.z) * 0.5)
	var avenue: int = CityStore.query(&"road_profiles").filter(func(p: Dictionary) -> bool: return p.id == "avenue")[0].index
	_between(e, TIGHT_GAP_M, avenue, false)
	_between(e + Vector3(0, 0, ROOMY_OFF_M), ROOMY_GAP_M, avenue, true)
	_crossing(e + Vector3(0, 0, CROSS_OFF_M), avenue)
	finish()


## Two parallel avenues `gap` apart and a road from the middle of one to the other: the preview says what the build does.
func _between(at: Vector3, gap: float, profile: int, room: bool) -> void:
	var far := at + Vector3(0, 0, gap)
	check(CityStore.dispatch(&"build_road", [at, at + Vector3(ROAD_M, 0, 0), false, Vector3.ZERO, profile, PX]).get("ok", false), "the first avenue")
	check(CityStore.dispatch(&"build_road", [far, far + Vector3(ROAD_M, 0, 0), false, Vector3.ZERO, profile, PX]).get("ok", false), "the second avenue")
	var mid := at + Vector3(ROAD_M * 0.5, 0, 0)
	_agree(mid, mid + Vector3(0, 0, gap), profile, room, "a road %.0f m long between two avenues" % gap)


## A road crossing an avenue close to a junction on it: the stretch of the avenue left between the two crossings is squeezed.
func _crossing(at: Vector3, profile: int) -> void:
	var start := at - Vector3(ROAD_M * 0.5, 0, 0)
	check(CityStore.dispatch(&"build_road", [start, at + Vector3(ROAD_M * 0.5, 0, 0), false, Vector3.ZERO, profile, PX]).get("ok", false), "the avenue")
	var side := at + Vector3(0, 0, -ROAD_M * 0.5)
	check(CityStore.dispatch(&"build_road", [side, at + Vector3(0, 0, ROAD_M * 0.5), false, Vector3.ZERO, profile, PX]).get("ok", false), "the junction on the avenue")
	var near := at + Vector3(TIGHT_GAP_M, 0, 0)
	_agree(near + Vector3(0, 0, -ROAD_M * 0.5), near + Vector3(0, 0, ROAD_M * 0.5), profile, false, "a crossing next to a junction")


## The preview of a road and its build: both refuse it as squeezed, or both take it.
func _agree(a: Vector3, b: Vector3, profile: int, room: bool, what: String) -> void:
	var plan: Dictionary = CityStore.dispatch(&"preview_road", [a, b, false, Vector3.ZERO, profile, PX])
	CityStore.refresh()
	var segments: int = CityStore.snapshot.segments
	var built: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, profile, PX])
	if room:
		check(plan.get("ok", false), "%s: the preview takes it: %s" % [what, plan])
		check(built.get("ok", false), "%s: the build takes it: %s" % [what, built])
		return
	check(plan.get("error", "") == SQUEEZED, "%s: the preview says it is squeezed: %s" % [what, plan])
	check(built.get("error", "") == SQUEEZED, "%s: the build refuses it the same: %s" % [what, built])
	CityStore.refresh()
	check(CityStore.snapshot.segments == segments, "%s: the previewed refusal changed nothing" % what)
