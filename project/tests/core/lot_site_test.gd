## The lots' sites in the generated cities: where two streets meet, the main street's end lot is a corner lot that fronts the other
## street too (its record says which side), and an uphill house digs its garage into the slope at the street, and only an uphill
## lot does. The town stands on flat land (a medium map's), so its lots are cut for the corners; the scenic Sao Paulo has its
## hills, where the garages are dug. Writes each city's corner and dug-garage lots to lot_sites_<map>.json in the tests' own
## folder (TestCase.SCRATCH), where the look plan's views were chosen from.
## Run: godot --path project res://tests/core/lot_site_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const HILL_MAP := "sao_paulo"
const SEED := 1
const OUT_FILE := "lot_sites_%d.json"
const SITE_CORNER_LEFT := 1 # b40::LotSite (native/src/sim/buildings/b40/types.h)
const SITE_CORNER_RIGHT := 2
const SITE_UPHILL := 4
const CORNER_SHARE_LEAST := 0.03 # a town of blocks has a corner at every block's end


func _ready() -> void:
	if not boot():
		finish()
		return
	var town := _sites(TOWN_MAP)
	check(town.total > 0, "the town grew buildings")
	check(float(town.corners) / town.total >= CORNER_SHARE_LEAST, "town: corner lots %d of %d" % [town.corners, town.total])
	check(town.uphill == 0 and town.dug == 0, "town: flat land, no uphill lot and no dug garage (%d, %d)" % [town.uphill, town.dug])
	var hills := _sites(DataTables.map_index(HILL_MAP))
	check(hills.dug > 0, "Sao Paulo: uphill houses dig their garages into the slope (%d of %d uphill lots)" % [hills.dug, hills.uphill])
	check(hills.dug_flat.is_empty(), "Sao Paulo: only an uphill lot digs a garage (flat lots that did: %s)" % [hills.dug_flat.slice(0, 8)])
	finish()


## One city made again from the seed: its lots counted by site, and its corner and dug-garage lots written for the look plan.
func _sites(map: int) -> Dictionary:
	var made: Dictionary = new_map(map, SEED, true)
	print("map %d: %d lots, %d buildings" % [map, made.lots, made.buildings])
	var corners: Array = []
	var dug: Array = []
	var out := {"total": 0, "corners": 0, "uphill": 0, "dug": 0, "dug_flat": []}
	for id: int in CityStore.query(&"list_buildings"):
		var b: Dictionary = CityStore.query(&"probe_building", [id])
		var site: int = b.site
		var row := [id, b.position.x, b.position.z, b.rot, site, b.zone, b.dug_garages]
		out.total += 1
		if site & (SITE_CORNER_LEFT | SITE_CORNER_RIGHT):
			corners.append(row)
		if site & SITE_UPHILL:
			out.uphill += 1
		if b.dug_garages > 0:
			dug.append(row)
			if not (site & SITE_UPHILL):
				out.dug_flat.append(id)
	out.corners = corners.size()
	out.dug = dug.size()
	print("map %d: corner lots %d, uphill lots %d, dug garages %d of %d buildings" % [map, out.corners, out.uphill, out.dug, out.total])
	var f := FileAccess.open(scratch(OUT_FILE % map), FileAccess.WRITE)
	f.store_string(JSON.stringify({"corners": corners, "dug": dug}))
	return out
