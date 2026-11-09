## The variety of the generated city's buildings, as the lab's generator gives it (Roadcraft procedural.ts, architecture.ts,
## zoning.ts quarterOf): over three seeds of the town map every kind of building shows every style its kind is drawn from
## (buildings.json kind_weights), shops show shopfronts and the shop's long awning, a street side shares a style more than the
## town does (its character), and a seed gives the same city twice. Prints what it counted, and one building of each kind and
## style (where it stands) for the captures.
## Run: godot --path project res://tests/core/building_variety_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEEDS: Array[int] = [7, 11, 23]
const KINDS: Array[String] = ["house", "flats", "tower", "shop", "office", "works"]
const SHOP := 3
const BAY_SHOPFRONT := 11 # b40::Bay's order (sim/buildings/b40/types.h)
const NEIGHBOUR_M := 40.0 # within one street side's character (buildings.json shapes.facade street_m: 60 m cells)
const SAME_FACING := 0.3 # radians: two lots facing one way
const SAME_STREET_MORE := 1.2 # a street side's buildings share a style this much more often than any two of the town's kind


func _ready() -> void:
	if not boot():
		finish()
		return
	var table: Dictionary = DataTables.style_of(DataTables.CATALOG.buildings, DataTables.REGIONAL_STYLE)
	var names: Array = table.architectures.map(func(a: Dictionary) -> String: return a.name)
	var seen := {} # kind -> style -> true, over the seeds
	var shopfronts := 0
	var awnings := {}
	for seed in SEEDS:
		var made: Dictionary = new_map(TOWN_MAP, seed, true)
		print("seed %d: %d buildings" % [seed, made.buildings])
		var census := _census()
		for kind in KINDS.size():
			var c: Dictionary = census[kind]
			print("  %s: n %d styles %s grounds %s palettes %d walls %d bays %s awnings %s" % [KINDS[kind], c.n, _named(c.styles, names),
					c.grounds, c.palettes.size(), c.walls.size(), c.bays.keys(), c.awnings])
			for s: int in c.styles:
				seen.get_or_add(kind, {})[s] = true
		shopfronts += int(census[SHOP].bays.get(BAY_SHOPFRONT, 0))
		awnings.merge(census[SHOP].awnings)
		if seed == SEEDS[0]:
			_examples(names)
			_street_character()
	for kind in KINDS.size():
		var want: Array = table.kind_weights[KINDS[kind]].map(func(w: Array) -> int: return names.find(w[0]))
		var missing := want.filter(func(s: int) -> bool: return not seen.get(kind, {}).has(s))
		check(missing.is_empty(), "%s: every style it is drawn from stands (missing %s)" % [KINDS[kind], missing.map(func(s: int) -> String: return names[s])])
	check(shopfronts > 0, "shops have shopfronts on the street (%d bays)" % shopfronts)
	check(awnings.size() > 1, "the shops' long awnings come in several colours (%d)" % awnings.size())
	var again: Dictionary = new_map(TOWN_MAP, SEEDS[0], true)
	var first := str(_census())
	new_map(TOWN_MAP, SEEDS[0], true)
	check(str(_census()) == first, "a seed gives the same buildings again (%d)" % again.buildings)
	finish()


## The census of every kind (CityStore body_census): its counts by style, ground, palette, wall, bay component and awning.
func _census() -> Array:
	var out: Array = []
	for kind in KINDS.size():
		out.append(CityStore.query(&"body_census", [kind]))
	return out


func _named(counts: Dictionary, names: Array) -> Dictionary:
	var out := {}
	for s: int in counts:
		out[names[s]] = counts[s]
	return out


## One building of each kind and style: where it stands, its storeys, its facing.
func _examples(names: Array) -> void:
	var shown := {}
	for id: int in CityStore.query(&"list_buildings"):
		var b: Dictionary = CityStore.query(&"probe_building", [id])
		if b.fn != 0:
			continue
		var key := "%s/%s" % [KINDS[b.lot.kind], names[b.lot.body.style]]
		if shown.has(key):
			continue
		shown[key] = true
		print("  example %s id %d at (%.0f, %.0f) rot %.2f floors %d" % [key, id, b.position.x, b.position.z, b.rot, b.floors])


## A street side's buildings (near and facing one way) share a style more often than any two of the town's of the same kind.
func _street_character() -> void:
	var rows: Array = []
	for id: int in CityStore.query(&"list_buildings"):
		var b: Dictionary = CityStore.query(&"probe_building", [id])
		if b.fn == 0:
			rows.append([Vector2(b.position.x, b.position.z), b.rot, b.lot.kind, b.lot.body.style])
	var near := [0, 0]
	var town := [0, 0]
	for i in rows.size():
		for j in range(i + 1, rows.size()):
			var a: Array = rows[i]
			var o: Array = rows[j]
			if a[2] != o[2]:
				continue
			var same := 1 if a[3] == o[3] else 0
			town[0] += same
			town[1] += 1
			if a[0].distance_to(o[0]) < NEIGHBOUR_M and absf(angle_difference(a[1], o[1])) < SAME_FACING:
				near[0] += same
				near[1] += 1
	var near_share := float(near[0]) / maxi(near[1], 1)
	var town_share := float(town[0]) / maxi(town[1], 1)
	check(near[1] > 0 and near_share > town_share * SAME_STREET_MORE,
			"a street side shares a style: %.2f of %d neighbours against %.2f of the town" % [near_share, near[1], town_share])
