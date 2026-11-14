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
const WAY_GROUND := 0 # the core's BuildWay (sim/economy/road_economy.h): plain ground
const WAY_BRIDGE := 3 # on piers
const JOINT_NEAR_M := 5.0 # either side of a ramp's joint: on its grade the road gains twice the half of this there
const LIFT_EDIT_M := 6.0
const CURVE_OUT_M := 60.0
const CURVE_LEAN := 0.85 # the bend's control point this far along the chord: the curve's parameter and its length part
const CUT_AT := 0.3
const SAME_M := 0.3 # a cut leaves the road where it was but for the level plate of the new node

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
	_stations(a - Vector3(0, 0, LEG_M * 2))
	_height_paid(a - Vector3(0, 0, LEG_M * 4))
	_curve_cut(a - Vector3(0, 0, LEG_M * 6))
	_set_height(0)


func _set_height(metres: float) -> void:
	CityStore.dispatch(&"set_road_height", [metres])


func _road(from: Vector3, to: Vector3, from_lift: float) -> Dictionary:
	return CityStore.dispatch(&"build_road", [from, to, false, Vector3.ZERO, PROFILE, PX, from_lift])


## How far over the ground the road stands at a point, by what a ray lands on.
func _over_ground(p: Vector3) -> float:
	var hit: Dictionary = CityStore.query(&"raycast", [Vector3(p.x, RAY_FROM_M, p.z), Vector3.DOWN])
	return hit.position.y - float(CityStore.query(&"height_at", [p.x, p.z])) if hit.hit else 0.0


## The height of the road's surface at a point, by what a ray lands on.
func _deck_y(p: Vector3) -> float:
	var hit: Dictionary = CityStore.query(&"raycast", [Vector3(p.x, RAY_FROM_M, p.z), Vector3.DOWN])
	return hit.position.y if hit.hit else NAN


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
	# a ramp laid in pieces is one line: through the joint in its middle it climbs on its grade, with no level step there
	var across := _over_ground(m.lerp(e, 0.5) + Vector3(JOINT_NEAR_M, 0, 0)) - _over_ground(m.lerp(e, 0.5) - Vector3(JOINT_NEAR_M, 0, 0))
	t.check(across > DECK_M / LEG_M * JOINT_NEAR_M, "the ramp goes through its middle on its grade (%.2f m gained in %d m)" % [across, int(JOINT_NEAR_M * 2)])


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
	# a press on what is seen, with no height set by hand, starts there at its height: the ramp's end stands at 8 m
	_set_height(0)
	var top := a + Vector3(LEG_M * 2, 0, 0)
	var seen: Dictionary = CityStore.query(&"road_start", [top, PX, true, false, 0.0, 0.0, true])
	t.check(seen.taken and absf(seen.lift - DECK_M) < NEAR_M, "a press on the deck's end starts there, at its height: %s" % [seen])
	var by_hand: Dictionary = CityStore.query(&"road_start", [top, PX, true, false, 0.0, 0.0, false])
	t.check(not by_hand.taken, "with the height set by hand at 0 m the start passes under the deck's end")


## A free end stops short of a height too steep to reach.
func _steepest(a: Vector3) -> void:
	_set_height(STEEP_ASK_M)
	var e := a + Vector3(SHORT_M, 0, 0)
	var pv: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE, PX, 0.0])
	var points: PackedVector3Array = pv.points
	var tip := points[-1]
	var reached: float = tip.y - float(CityStore.query(&"height_at", [tip.x, tip.z]))
	t.check(absf(reached - SHORT_M * GRADE) < NEAR_M, "the end is held to the steepest grade (%.2f m)" % reached)
	t.check(pv.limited, "and the draft is told its end stopped short of the height asked")


## A road on piers costs what it is built as: more than the same road on the ground, the preview's price paid, a quarter back when it goes.
func _price(a: Vector3) -> void:
	var e := a + Vector3(LEG_M, 0, 0)
	_set_height(0)
	var flat: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE, PX, 0.0])
	_set_height(DECK_M)
	var high: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE, PX, DECK_M])
	t.check(high.price > flat.price, "on piers it costs more: %s against %s" % [high.price, flat.price])
	t.check(high.built.bridge > LEG_M * 0.9 and flat.built.bridge == 0.0 and not flat.limited,
		"the draft names what it is built as: %s on piers, %s on the ground" % [high.built, flat.built])
	var r := _road(a, e, DECK_M)
	t.check(r.ok and absf(r.money - high.price) <= MONEY_ROUNDING, "the preview's price is paid: %s against %s" % [r.money, high.price])


## The draft carries the road as it will be built: each station's deck, the ground under it and its way of building; a road
## drawn at 8 m stands on piers, with its piers under it, and one on the ground is ground all along.
func _stations(a: Vector3) -> void:
	var e := a + Vector3(LEG_M, 0, 0)
	_set_height(DECK_M)
	var high: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE, PX, DECK_M])
	var deck: PackedVector3Array = high.deck
	var ground: PackedFloat32Array = high.ground
	var ways: PackedByteArray = high.ways
	t.check(deck.size() > 2 and deck.size() == ground.size() and deck.size() == ways.size(), "the draft has its stations: %d" % deck.size())
	var mid := deck.size() >> 1
	var over := deck[mid].y - ground[mid]
	t.check(ways[mid] == WAY_BRIDGE and absf(over - DECK_M) < NEAR_M, "its middle stands on piers at the height in hand: way %d, %.2f m" % [ways[mid], over])
	var piers: PackedVector3Array = high.piers
	t.check(piers.size() >= 2 and piers.size() % 2 == 0 and piers[1].y > piers[0].y, "with piers from the ground to the deck: %d" % (piers.size() >> 1))
	t.check(is_equal_approx(high.lift_a, DECK_M) and is_equal_approx(high.lift_b, DECK_M), "and it tells the height of its two ends: %s, %s" % [high.lift_a, high.lift_b])
	_set_height(0)
	var flat: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE, PX, 0.0])
	var plain := true
	for way in flat.ways:
		plain = plain and way == WAY_GROUND
	t.check(plain and flat.piers.is_empty(), "a road on the ground is plain ground all along, with no pier")


## A road's height changed by hand is paid for: its end raised 6 m, part of it stands on a fill and on piers, and the
## difference is charged; undo gives it back.
func _height_paid(a: Vector3) -> void:
	var e := a + Vector3(LEG_M, 0, 0)
	_set_height(0)
	t.check(_road(a, e, 0).ok, "a road is laid on the ground")
	var end: Dictionary = CityStore.query(&"road_start", [e, PX, true, false])
	CityStore.refresh()
	var before: float = CityStore.snapshot.money
	var r: Dictionary = CityStore.dispatch(&"set_node_height", [end.node, LIFT_EDIT_M])
	CityStore.refresh()
	t.check(r.get("ok", false) and r.money > 0.0 and is_equal_approx(before - CityStore.snapshot.money, r.money), "raising its end is paid for: %s" % [r])
	CityStore.dispatch(&"undo")
	CityStore.refresh()
	t.check(is_equal_approx(CityStore.snapshot.money, before), "and undo gives the money back: %s" % CityStore.snapshot.money)


## A curved road on a ramp stays where it was when it is cut: a height along a road is read by the length run, and on a
## bend whose control point leans to one end the length and the curve's parameter part (here by a seventh of the road).
func _curve_cut(a: Vector3) -> void:
	var e := a + Vector3(LEG_M, 0, 0)
	var control := a + Vector3(LEG_M * CURVE_LEAN, 0, CURVE_OUT_M)
	_set_height(DECK_M)
	t.check(CityStore.dispatch(&"build_road", [a, e, true, control, PROFILE, PX, 0.0]).ok, "a curved ramp is laid")
	var p := a.lerp(control, CUT_AT).lerp(control.lerp(e, CUT_AT), CUT_AT) # the curve at CUT_AT of its parameter
	var before := _deck_y(p)
	var climbed := before - _deck_y(a.lerp(control, 0.01))
	t.check(climbed > NEAR_M and climbed < DECK_M - NEAR_M, "part way up, the ramp has climbed part of the height (%.2f m)" % climbed)
	t.check(CityStore.dispatch(&"split_road", [p]).get("ok", false), "the curved ramp is cut there")
	var after := _deck_y(p)
	t.check(absf(after - before) < SAME_M, "the cut leaves the road at its height: %.2f m against %.2f m" % [after, before])


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
