## No two buildings of the generated city stand on the same ground: every record's lot (its centre, width, depth and turn, as
## probe_building gives them) is a rectangle, and no two of them overlap by more than a hand's width. Two lots that share a line
## touch; they do not overlap. And what is drawn over a lot stays on it: no pitched roof reaches past its lot's lines (probe
## bay_census, roof_past_lot), so two halls on a shared line do not put their roofs through one another; nor does its ground or its
## boundary (lot_past_lot: a wall's plinth and coping), so two lots' walls on one line do not stand in one another. And every lot's gate
## stands open (gates_shut): nothing of it in the passage a car or a person goes through; and no street piece stands in front of it
## (traffic_probe furniture_in_gates: the kerb's strip holds the vehicles' ramps and the furniture alike, NBR 9050), a traffic signal's
## post neither (signals_in_gates): no lot opens a gate onto a junction's kerb, its corner, crosswalk and the place of its posts
## (gates_on_junctions). A lot's side a street runs along is closed as its front, never by a blind party wall (lot_bounds street_walls).
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const CELL_M := 64.0 # the grid the lots are filed in to find neighbours
const TOUCH_M := 0.1 # how far two rectangles may enter one another and still only touch
const SHOWN := 8
const FURNITURE_SAMPLE := 1 # traffic_probe counts the street furniture only with a sample


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var lots: Array = []
	var cells: Dictionary = {}
	for id: int in CityStore.query(&"list_buildings"):
		var b: Dictionary = CityStore.query(&"probe_building", [id])
		var rot: float = b.rot
		var along: Vector2 = Vector2(cos(rot), -sin(rot)) * (float(b.w) * 0.5)
		var deep: Vector2 = Vector2(sin(rot), cos(rot)) * (float(b.d) * 0.5)
		var centre := Vector2(b.position.x, b.position.z)
		var reach: float = along.length() + deep.length()
		var lot := {"id": id, "fn": b.fn, "zone": b.zone, "centre": centre, "along": along, "deep": deep}
		for cx in range(int(floor((centre.x - reach) / CELL_M)), int(floor((centre.x + reach) / CELL_M)) + 1):
			for cz in range(int(floor((centre.y - reach) / CELL_M)), int(floor((centre.y + reach) / CELL_M)) + 1):
				var key := Vector2i(cx, cz)
				if not cells.has(key):
					cells[key] = []
				cells[key].append(lots.size())
		lots.append(lot)
	var seen: Dictionary = {}
	var crossing: Array = []
	var worst := 0.0
	for members: Array in cells.values():
		for i in members.size():
			for k in range(i + 1, members.size()):
				var pair := Vector2i(mini(members[i], members[k]), maxi(members[i], members[k]))
				if seen.has(pair):
					continue
				seen[pair] = true
				var depth := _overlap(lots[pair.x], lots[pair.y])
				if depth > TOUCH_M:
					worst = maxf(worst, depth)
					crossing.append([lots[pair.x].id, lots[pair.y].id, snappedf(depth, 0.01), lots[pair.x].zone, lots[pair.y].zone, lots[pair.x].fn, lots[pair.y].fn])
	print("lots: %d, pairs looked at: %d, overlapping: %d, deepest %.2f m" % [lots.size(), seen.size(), crossing.size(), worst])
	check(lots.size() > 0 and seen.size() > 0, "the town has lots side by side: %d lots, %d neighbouring pairs" % [lots.size(), seen.size()])
	check(crossing.is_empty(), "no two lots overlap ([id, id, metres, zones, functions] of the first: %s)" % [crossing.slice(0, SHOWN)])
	await get_tree().process_frame # the building levels know their map once the first frame drew it
	await get_tree().process_frame
	var census: Dictionary = CityStore.query(&"bay_census")
	var past := int(census.get("roof_past_lot", -1))
	check(past == 0, "no pitched roof is written past its lot's lines (its eaves take the room the lot leaves): %d vertices past" % past)
	var walls := int(census.get("lot_past_lot", -1))
	check(walls == 0, "no lot's ground or boundary is written past its lot's lines (plinths and copings stay on its land): %d vertices past, the first (seed, kind, x, y, w, d, vertices) %s"
			% [walls, census.get("lot_past_first", [])])
	var gates := int(census.get("gates", 0))
	var shut := int(census.get("gates_shut", -1))
	check(gates > 0 and shut == 0, "the lots' gates stand open, their leaves swung into the lot beside the passage: %d gates, %d vertices in a passage, the first (seed, kind, x, y, w, vertices) %s"
			% [gates, shut, census.get("gates_shut_first", [])])
	var blocked := int(census.get("leaves_blocked", -1))
	check(blocked == 0, "nothing of a lot stands where its open gates' leaves stand: %d parts or blocks in a leaf, the first (seed, gate kind, kind or -1 a block, x, y, w, d) %s"
			% [blocked, census.get("leaves_blocked_first", [])])
	_street_checks()
	finish()


## What stands between the lots and their streets: no street piece and no signal post in front of a gate, no gate on a junction's kerb,
## no party wall along a street.
func _street_checks() -> void:
	var street: Dictionary = CityStore.query(&"traffic_probe", [FURNITURE_SAMPLE])
	check(street.furniture_in_gates == 0, "no street piece stands in front of a lot's gate: %d" % street.furniture_in_gates)
	check(street.signals_in_gates == 0, "no traffic signal's post stands in front of a lot's gate: %d" % street.signals_in_gates)
	check(street.gates_on_junctions == 0, "no lot's gate opens onto a junction's kerb (its corner, crosswalk, the place of its signal posts): %d (%d a car's), the first (lot, kind, x, z, street, arc, front x, w, shut stretches, street sides) %s"
			% [street.gates_on_junctions, street.car_gates_on_junctions, street.gates_on_junctions_first])
	var bounds: Dictionary = CityStore.query(&"lot_bounds")
	check(int(bounds.street_sides) > 0 and int(bounds.street_walls) == 0,
			"a lot's side on a street is closed as its front, never by a party wall: %d sides on a street, %d party walls on them, the first (building, edge, x, z, height, kind) %s"
			% [bounds.street_sides, bounds.street_walls, bounds.street_first])


## How far two turned rectangles enter one another: the least overlap over their four axes (separating axis theorem); 0 or
## less when an axis separates them.
func _overlap(a: Dictionary, b: Dictionary) -> float:
	var least := INF
	var apart: Vector2 = b.centre - a.centre
	for axis: Vector2 in [a.along.normalized(), a.deep.normalized(), b.along.normalized(), b.deep.normalized()]:
		var reach_a: float = absf(a.along.dot(axis)) + absf(a.deep.dot(axis))
		var reach_b: float = absf(b.along.dot(axis)) + absf(b.deep.dot(axis))
		least = minf(least, reach_a + reach_b - absf(apart.dot(axis)))
	return least
