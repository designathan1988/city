## The city's audit: one run that reads every defect a rule can state, each from the state or from the geometry sent to the GPU
## (never from pixels, so two runs say the same), with how many and where the worst stand. A generated city is opened paused and
## stepped some simulated minutes; then:
## - the lots (lot_audit): fronts standing off their footway, buildings on a street's paving or in one another, lots that are mostly
##   lawn though no house's, and the ground's square metres by kind;
## - the traffic (traffic_probe): vehicles inside one another, taken away in sight, standing for good, trips lost; walkers inside one
##   another and in the street's furniture;
## - the roads (road_surface_probe, road_seam_probe): the surface off its true shape past its tolerance, seam cells refused or with
##   no height.
## Every line is a rule: `ok` or `FAIL` with the count and the worst places (x, z), so a fix is checked by the line it moves.
## Run: godot --path project res://tests/audit/city_audit.tscn -- --no-flow [--map=0] [--seed=7] [--minutes=3]
## (hidden, under a deadline: python tools/tasks/cli.py run audit)
extends TestCase

const TICKS_A_MINUTE := 1200 # sim.tick_hz 20 (tuning.json) x 60 s
const KINDS: Array[String] = ["house", "flats", "tower", "shop", "office", "works"] # b40::Kind's order
## The longest a near vehicle may stand: a red light, a queue behind it and a turn across traffic (tests/core/sight_test.gd).
const STOOD_MOST_S := 180.0
const WORST_SHOWN := 5


func _ready() -> void:
	if not boot():
		finish()
		return
	var map := int(_arg("map", "0"))
	var seed := int(_arg("seed", "7"))
	var minutes := int(_arg("minutes", "3"))
	var made: Dictionary = new_map(map, seed, true)
	print("audit of map %d seed %d: %d buildings, %d simulated minutes" % [map, seed, made.get("buildings", 0), minutes])
	for m in minutes:
		ticks(TICKS_A_MINUTE)
	_lots()
	_traffic()
	_roads()
	finish()


func _lots() -> void:
	var a: Dictionary = CityStore.query(&"lot_audit")
	print("lots: %d read; ground by kind (m2 built / paved / lawn / bare):" % a.lots)
	for k in KINDS.size():
		var g: Array = a.ground_m2_by_kind[k]
		var all := float(int(g[0]) + int(g[1]) + int(g[2]) + int(g[3]))
		if all > 0.0:
			print("  %-6s %7d / %7d / %7d / %7d   lawn and bare %.0f%% of the lots' ground" % [KINDS[k], g[0], g[1], g[2], g[3], 100.0 * (int(g[2]) + int(g[3])) / all])
	check(int(a.fronts_off) == 0, "every lot's front stands on its footway's edge: %d off it%s" % [a.fronts_off, _worst(a.front_gap_m, "m")])
	check(int(a.on_paving) == 0, "no building stands on a street's paving: %d%s" % [a.on_paving, _worst(a.on_paving_m, "m in")])
	check(int(a.in_another) == 0, "no building stands in another: %d pairs%s" % [a.in_another, _worst(a.in_another_at, "m apart")])
	check((a.lawn_lots as Array).is_empty(), "no lot but a house's is mostly lawn%s" % _worst(a.lawn_lots, "of the lot"))


func _traffic() -> void:
	var p: Dictionary = CityStore.query(&"traffic_probe", [1])
	print("traffic: %d vehicles, %d people, %d trips arrived, %d walks ended" % [p.vehicles, p.people, p.trips_arrived, p.people_arrived])
	check(int(p.vehicles) > 0 and int(p.people) > 0, "the city has traffic and walkers")
	check(int(p.gone_in_sight[0]) == 0, "no vehicle taken away in sight: %d" % p.gone_in_sight[0])
	check(int(p.gone_in_sight[1]) == 0, "no walker taken away in sight: %d" % p.gone_in_sight[1])
	check(float(p.stuck_longest_s) < STOOD_MOST_S, "no near vehicle stands for good: the longest %.0f s" % p.stuck_longest_s)
	check(int(p.trips_lost) == 0, "no trip lost: %d" % p.trips_lost)
	var inside: Array = p.people_overlaps
	check(int(inside[0]) == 0 and int(inside[1]) == 0, "no walker stands inside another: %s" % [inside])
	check(int(p.people_in_furniture) == 0, "no walker stands in the street's furniture: %d" % p.people_in_furniture)
	check(int(p.red_from_stop) == 0, "no vehicle sets off through a red light: %d" % p.red_from_stop)


func _roads() -> void:
	var surface: Dictionary = CityStore.query(&"road_surface_probe")
	var seam: Dictionary = CityStore.query(&"road_seam_probe")
	print("roads: surface %s" % [surface])
	print("roads: seam %s" % [seam])
	check(int(seam.refused) == 0, "no ground seam cell refused: %d" % seam.refused)
	check(int(seam.unheighted) == 0, "no ground seam point without a height: %d" % seam.unheighted)


## The worst places of a defect, as lot_audit lists them ([measure, where]).
func _worst(list: Array, unit: String) -> String:
	if list.is_empty():
		return ""
	var parts: PackedStringArray = []
	for i in mini(WORST_SHOWN, list.size()):
		var where: Vector3 = list[i][1]
		parts.append("%.2f %s at (%.0f, %.0f)" % [list[i][0], unit, where.x, where.z])
	return "; worst: " + ", ".join(parts)


static func _arg(name: String, fallback: String) -> String:
	for a in OS.get_cmdline_user_args():
		if a.begins_with("--%s=" % name):
			return a.get_slice("=", 1)
	return fallback
