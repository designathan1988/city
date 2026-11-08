## The poles case of strike_test: a bomb on a street with the player's line of utility poles along its footway.
class_name StrikePoleCases
extends RefCounted

## A road on free ground past the town, the line strung along its footway, the charge in the middle of its carriageway.
const FIELD_M := 400.0
const ROAD_LEN_M := 240.0
const RUN_SHARE := 0.2 # the line runs from this share of the road to the same share short of its end
const FOOTWAY_OFF_M := 5.0
const PX := 0.25
const REACH := 6.0
const FORCE := 5
const DRAWN_FRAMES := 600 # frames the effects have to play the blow
const MIDDLE := 0.5
const GROVE_OFF := Vector3(0, 0, 14) # a grove planted off the street, inside the charge's reach
const GROVE_M := 8.0
const GROVE := {"density": 400.0, "height": 8.0, "variation": 0.0, "spacing": 3.0} # trees a hectare
const PARK_FROM := Vector3(-12, 0, -8) # a park drawn across the street from the grove: undo and redo hold it with the rest (its trees thrown: bomb_street_plan.json, the render throws only near the camera)
const PARK_TO := Vector3(12, 0, -24)
const BRIDGE_OFF_M := 300.0 # the bridge's road, off the poles' street
const BRIDGE_LIFT_M := 9.0 # its deck over the ground, at both ends
const DECK_LEAST_M := 6.0 # more than the crater of a charge digs
const BRIDGE_FORCE := 45 # a charge whose crater leaves no way over the deck
const GROUND_STILL_M := 0.01

var t: Node # the test (TestCase): its check and frames


func _init(test: Node) -> void:
	t = test


## A charge on the carriageway digs its crater though the road stands over the terrain, lays the ground it heaved round it, brings
## down the poles within its reach (the next one pulled over now and then) and leaves the wires of those left standing hanging from
## them; undoing it stands every pole and its wires again, redoing it brings the same down.
func run() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(b.max.x + FIELD_M, 0, (b.min.z + b.max.z) * 0.5)
	var e := a + Vector3(ROAD_LEN_M, 0, 0)
	CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, CityStore.first_road(), PX])
	var from := a.lerp(e, RUN_SHARE) + Vector3(0, 0, FOOTWAY_OFF_M)
	var to := a.lerp(e, 1.0 - RUN_SHARE) + Vector3(0, 0, FOOTWAY_OFF_M)
	var run: Dictionary = CityStore.dispatch(&"build_pole_run", [from, to, REACH, CityStore.code(&"LampMode", &"ALL"), CityStore.code(&"PoleKind", &"URBAN")])
	t.check(run.get("ok", false), "a line of poles along the street: %s" % run)
	if not run.get("ok", false):
		return
	var mid := a.lerp(e, MIDDLE)
	var hit: Dictionary = CityStore.query(&"raycast", [mid + Vector3.UP * FIELD_M, Vector3.DOWN])
	var deck: Vector3 = hit.position if hit.get("hit", false) else mid
	CityStore.dispatch(&"begin_stroke", ["cmd.place.trees"])
	CityStore.dispatch(&"tree_path", [PackedVector3Array([mid + GROVE_OFF]), GROVE_M, 0, 0, GROVE])
	CityStore.dispatch(&"end_stroke")
	var park: Dictionary = CityStore.dispatch(&"draw_building", [mid + PARK_FROM, mid + PARK_TO, CityStore.code(&"BuildingFn", &"FN_PARK"), DataTables.zone_code("residential"), 0])
	t.check(park.get("ok", false), "a park across the street: %s" % [park])
	var grove: Dictionary = CityStore.query(&"landscape")
	t.check(grove.planted > 0, "a grove planted beside the street (%d trees)" % grove.planted)
	var standing: Dictionary = CityStore.query(&"street_counts")
	var damage: Dictionary = CityStore.query(&"damage_probe")
	var before: Dictionary = CityStore.query(&"state_hash")
	t.check(CityStore.dispatch(&"strike", [deck, FORCE]).get("ok", false), "a charge on the carriageway")
	var after: Dictionary = CityStore.query(&"state_hash")
	var down: Dictionary = CityStore.query(&"street_counts")
	t.check(CityStore.query(&"damage_probe").craters == damage.craters + 1, "it dug a crater on the road, over the terrain as the road stands")
	t.check(down.poles < standing.poles and down.spans < standing.spans, "poles came down with their wires (%d -> %d)" % [standing.poles, down.poles])
	t.check(down.poles > 0, "the poles past its reach stand")
	var felled: Dictionary = CityStore.query(&"landscape")
	t.check(felled.planted < grove.planted and felled.clearings == grove.clearings + 1, "the trees in its reach came down, the woods' with them (%d -> %d)" % [grove.planted, felled.planted])
	for f in DRAWN_FRAMES:
		var seen: Dictionary = CityStore.query(&"damage_probe")
		if seen.wires_down > 0 and seen.heave > 0:
			break
		await t.get_tree().process_frame
	var drawn: Dictionary = CityStore.query(&"damage_probe")
	t.check(drawn.wires_down > 0, "the wires torn off hang from the poles left (%d)" % drawn.wires_down)
	t.check(drawn.heave > 0, "the crater laid the ground it heaved (%d pieces)" % drawn.heave)
	t.check(CityStore.dispatch(&"undo", []), "undid the charge")
	t.check(CityStore.query(&"street_counts").poles == standing.poles, "undo stood every pole again")
	t.check(CityStore.query(&"landscape").planted == grove.planted, "and every tree")
	t.check(CityStore.query(&"state_hash").whole == before.whole, "and left the city as it was")
	t.check(CityStore.dispatch(&"redo", []), "redid the charge")
	t.check(CityStore.query(&"state_hash").whole == after.whole, "redo brought the same down")


## A charge on a bridge cuts its deck and digs no crater in the ground under it; a charge on that ground digs its crater and leaves
## the bridge over it standing.
func bridge() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(b.max.x + FIELD_M, 0, (b.min.z + b.max.z) * 0.5 - BRIDGE_OFF_M)
	var e := a + Vector3(ROAD_LEN_M, 0, 0)
	CityStore.dispatch(&"set_road_height", [BRIDGE_LIFT_M])
	var laid: Dictionary = CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, CityStore.first_road(), PX, BRIDGE_LIFT_M])
	CityStore.dispatch(&"set_road_height", [0.0])
	t.check(laid.get("ok", false), "a road raised over the ground: %s" % [laid])
	var mid := a.lerp(e, MIDDLE)
	var hit: Dictionary = CityStore.query(&"raycast", [mid + Vector3.UP * FIELD_M, Vector3.DOWN])
	var ground: float = CityStore.query(&"height_at", [mid.x, mid.z])
	var deck: Vector3 = hit.get("position", mid)
	t.check(deck.y - ground > DECK_LEAST_M, "it stands as a bridge (%.1f m over the ground)" % (deck.y - ground))
	var damage: Dictionary = CityStore.query(&"damage_probe")
	var on_deck: Dictionary = CityStore.dispatch(&"strike", [deck, BRIDGE_FORCE])
	var heights: float = CityStore.query(&"height_at", [mid.x, mid.z])
	t.check(on_deck.get("segments", 0) > 0 and absf(heights - ground) < GROUND_STILL_M, "a strong charge on the deck cut it and dug nothing in the ground under it")
	CityStore.dispatch(&"undo", [])
	var under: Dictionary = CityStore.dispatch(&"strike", [Vector3(mid.x, ground, mid.z), FORCE])
	t.check(under.get("segments", 0) == 0 and CityStore.query(&"damage_probe").craters == damage.craters + 1, "a charge on the ground under it dug its crater and left the bridge over it")
	CityStore.dispatch(&"undo", [])
