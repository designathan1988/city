## The height a road is drawn at, in road_commands_test: the ramp from the height it was begun at, the steepest grade, a road that
## passes over another, one too close to do either, what the pointer can land on at a height, and the sides of a cutting.
class_name RoadHeightCases
extends RefCounted

const PROFILE := 0
const PX := 0.25
const FIELD_OUT_M := 2000.0
const LEG_M := 200.0
const CROSS_M := 120.0
const DECK_M := 8.0
const STEEP_ASK_M := 60.0
const SHORT_M := 100.0
const GRADE := 0.12 # the steepest grade a road is drawn at (tuning grade.authored)
const NEAR_M := 0.7 # the deck stands the road's build-up off the height asked
const RAY_FROM_M := 400.0
const NO_NODE := -1
const MONEY_ROUNDING := 1.0 # each piece of a road is priced to the unit

var t: Node # the test (TestCase): its check and ticks


func _init(test: Node) -> void:
	t = test


func run(b: Dictionary) -> void:
	var a := Vector3(b.max.x + FIELD_OUT_M, 0, (b.min.z + b.max.z) * 0.5)
	_ramp(a)
	_passing(a)
	_landing(a)
	_steepest(a + Vector3(0, 0, LEG_M * 2))
	_walls(a + Vector3(0, 0, LEG_M * 4))
	_price(a + Vector3(0, 0, LEG_M * 6))
	_set_height(0)


func _set_height(metres: float) -> void:
	CityStore.dispatch(&"set_road_height", [metres])


func _road(from: Vector3, to: Vector3, from_lift: float) -> Dictionary:
	return CityStore.dispatch(&"build_road", [from, to, false, Vector3.ZERO, PROFILE, PX, from_lift])


## How far over the ground the road stands at a point, by what a ray lands on.
func _over_ground(p: Vector3) -> float:
	var hit: Dictionary = CityStore.query(&"raycast", [Vector3(p.x, RAY_FROM_M, p.z), Vector3.DOWN])
	return hit.position.y - float(CityStore.query(&"height_at", [p.x, p.z])) if hit.hit else 0.0


## A road begun on the ground and ended at 8 m climbs all the way, and its preview is drawn along the same ramp.
func _ramp(a: Vector3) -> void:
	var m := a + Vector3(LEG_M, 0, 0)
	var e := m + Vector3(LEG_M, 0, 0)
	_set_height(0)
	t.check(_road(a, m, 0).ok, "the approach is laid on the ground")
	_set_height(DECK_M)
	var pv: Dictionary = CityStore.dispatch(&"preview_road", [m, e, false, Vector3.ZERO, PROFILE, PX, 0.0])
	var points: PackedVector3Array = pv.points
	var tip := points[-1]
	var drawn: float = tip.y - float(CityStore.query(&"height_at", [tip.x, tip.z]))
	t.check(pv.ok and absf(drawn - DECK_M) < NEAR_M, "the preview climbs to the height in hand (%.2f m)" % drawn)
	t.check(_road(m, e, 0).ok, "the ramp is laid")
	var mid := _over_ground(m.lerp(e, 0.5))
	t.check(absf(mid - DECK_M * 0.5) < DECK_M * 0.25, "halfway up the ramp the road stands half way (%.2f m)" % mid)
	var top := _over_ground(e - Vector3(1, 0, 0))
	t.check(absf(top - DECK_M) < NEAR_M * 2, "the ramp ends at the height asked (%.2f m)" % top)


## Under a deck at 8 m a street passes with no junction; under the ramp at 3 m it can neither pass nor join.
func _passing(a: Vector3) -> void:
	_set_height(0)
	var over := a + Vector3(LEG_M * 2 - 2, 0, 0)
	var under: Dictionary = CityStore.dispatch(&"preview_road", [over - Vector3(0, 0, CROSS_M), over + Vector3(0, 0, CROSS_M), false, Vector3.ZERO, PROFILE, PX, 0.0])
	t.check(under.ok and under.crossings == 0, "a street passes under the deck with no junction: %s %s" % [under.error, under.crossings])
	var low := a + Vector3(LEG_M * 1.4, 0, 0)
	var clash: Dictionary = CityStore.dispatch(&"preview_road", [low - Vector3(0, 0, CROSS_M), low + Vector3(0, 0, CROSS_M), false, Vector3.ZERO, PROFILE, PX, 0.0])
	t.check(not clash.ok and clash.error == "hint.road.clearance", "too close in height to pass or join: %s" % clash.error)


## The pointer lands on what stands at the height in hand: a node on the ground is not taken at 8 m.
func _landing(a: Vector3) -> void:
	_set_height(0)
	var at_ground: Dictionary = CityStore.query(&"road_start", [a, PX, true, false])
	t.check(at_ground.node != NO_NODE, "on the ground the start takes the node")
	_set_height(DECK_M)
	var over: Dictionary = CityStore.query(&"road_start", [a, PX, true, false])
	t.check(over.node == NO_NODE, "at 8 m the start passes over the node")


## A free end stops short of a height too steep to reach.
func _steepest(a: Vector3) -> void:
	_set_height(STEEP_ASK_M)
	var e := a + Vector3(SHORT_M, 0, 0)
	var pv: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE, PX, 0.0])
	var points: PackedVector3Array = pv.points
	var tip := points[-1]
	var reached: float = tip.y - float(CityStore.query(&"height_at", [tip.x, tip.z]))
	t.check(absf(reached - SHORT_M * GRADE) < NEAR_M, "the end is held to the steepest grade (%.2f m)" % reached)


## A road on piers costs what it is built as: more than the same road on the ground, the preview's price paid, a quarter back when it goes.
func _price(a: Vector3) -> void:
	var e := a + Vector3(LEG_M, 0, 0)
	_set_height(0)
	var flat: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE, PX, 0.0])
	_set_height(DECK_M)
	var high: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE, PX, DECK_M])
	t.check(high.price > flat.price, "on piers it costs more: %s against %s" % [high.price, flat.price])
	var r := _road(a, e, DECK_M)
	t.check(r.ok and absf(r.money - high.price) <= MONEY_ROUNDING, "the preview's price is paid: %s against %s" % [r.money, high.price])


## The sides of a cutting are the road tool's choice, and the furniture set leaves it alone.
func _walls(a: Vector3) -> void:
	_set_height(0)
	CityStore.dispatch(&"set_road_walls", [true])
	var level: int = CityStore.query(&"road_furniture")
	CityStore.dispatch(&"set_road_furniture", [level])
	t.check(CityStore.query(&"road_walls"), "the furniture choice keeps the walls")
	t.check(_road(a, a + Vector3(LEG_M, 0, 0), 0).ok, "a road is laid with the walls chosen")
	CityStore.dispatch(&"set_road_walls", [false])
	t.check(not CityStore.query(&"road_walls"), "the walls are put off")
