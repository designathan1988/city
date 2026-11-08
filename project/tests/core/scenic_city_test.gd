## The scenic Sao Paulo (data/catalog/scenic.json) as the player meets it: the city stands (streets, buildings, the parks'
## trees), every named road found its street, Paulista runs on its ridge over the rivers' valleys with its own section, the
## Marginal Tiete runs beside the Tiete, Ibirapuera holds no building, and the city is one of towers.
## Run: godot --path project res://tests/core/scenic_city_test.tscn -- --no-flow
extends TestCase

const MAP_ID := "sao_paulo"
const SEED := 11
const MIN_STREETS := 4000
const MIN_BUILDINGS := 6000
const MIN_TREES := 1000
const TOWER_FLOORS := 10
const TOWER_SHARE := 0.2 # a city of towers: at least a fifth of its buildings ten storeys or more
const RIDGE_OVER_VALLEY_M := 25.0 # Paulista over the Tiete's banks
## Points of the layout (scenic.json): Paulista's middle, the Marginal Tiete south of the Tiete at x = 0, the Tiete's south bank.
const PAULISTA := Vector3(-325, 0, 375)
const MARGINAL := Vector3(0, 0, -2010)
const TIETE_BANK := Vector3(0, 0, -1950)


func _ready() -> void:
	if not boot():
		finish()
		return
	var map := _map_index()
	if not check(map >= 0, "the map %s is in maps.json" % MAP_ID):
		finish()
		return
	var city: Dictionary = DataTables.catalog_table("scenic").cities[0]
	var made: Dictionary = new_map(map, SEED, true)
	print("sao paulo: %d nodes, %d streets, %d lots, %d buildings, %d squares, %d trees in %.1f s" % [made.nodes, made.segments, made.lots, made.buildings, made.squares, made.trees, made.seconds])
	check(made.get("ok", false), "the map opens")
	check(made.segments > MIN_STREETS, "streets: %d" % made.segments)
	check(made.buildings > MIN_BUILDINGS, "buildings: %d" % made.buildings)
	check(made.trees > MIN_TREES, "park trees: %d" % made.trees)
	var named := 0
	for road: Dictionary in city.roads:
		named += 1 if road.has("name_key") else 0
	check(made.named == named, "every named road found its street: %d of %d" % [made.named, named])
	_roads()
	_parks(city)
	_towers()
	finish()


func _map_index() -> int:
	var maps := DataTables.maps()
	for i in maps.size():
		if maps[i].id == MAP_ID:
			return i
	return -1


func _profile(id: String) -> int:
	for p: Dictionary in CityStore.query(&"road_profiles"):
		if p.id == id:
			return p.index
	return -1


func _roads() -> void:
	var paulista: Dictionary = CityStore.query(&"segment_at", [PAULISTA])
	check(paulista.get("profile", -1) == _profile("avenue6LanesMedian"), "Paulista has its section: %s" % [paulista.get("profile", -1)])
	var marginal: Dictionary = CityStore.query(&"segment_at", [MARGINAL])
	check(marginal.get("profile", -1) == _profile("highway6Lanes"), "the Marginal Tiete is a highway: %s" % [marginal.get("profile", -1)])
	var ridge: float = CityStore.query(&"height_at", [PAULISTA.x, PAULISTA.z])
	var bank: float = CityStore.query(&"height_at", [TIETE_BANK.x, TIETE_BANK.z])
	check(ridge - bank > RIDGE_OVER_VALLEY_M, "Paulista %.1f m over the Tiete's bank at %.1f m" % [ridge, bank])


func _parks(city: Dictionary) -> void:
	var ring := PackedVector2Array()
	for p: Array in city.parks[0].ring:
		ring.append(Vector2(p[0], p[1]))
	var inside := 0
	for id: int in CityStore.query(&"list_buildings"):
		var b: Dictionary = CityStore.query(&"probe_building", [id])
		inside += 1 if Geometry2D.is_point_in_polygon(Vector2(b.position.x, b.position.z), ring) else 0
	check(inside == 0, "Ibirapuera holds no building: %d inside" % inside)


func _towers() -> void:
	var towers := 0
	var count := 0
	for id: int in CityStore.query(&"list_buildings"):
		var top := 0
		for v: Dictionary in CityStore.query(&"body_info", [id]).volumes:
			top = maxi(top, v.base + v.storeys)
		count += 1
		towers += 1 if top >= TOWER_FLOORS else 0
	print("towers: %d of %d" % [towers, count])
	check(float(towers) / maxi(1, count) > TOWER_SHARE, "a city of towers: %d of %d buildings %d storeys or more" % [towers, count, TOWER_FLOORS])
