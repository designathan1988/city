## A window is the same window at the near and at the mid level (probe bay_census: every building of the town laid as the near level's
## job and as the mid level's job lay it): each pane of the near level finds, among the mid level's bays of its building, one with its
## room's seed, its bay byte, its glass and its room's floor, the four things a lit window is drawn by (building_lamps.gdshaderinc),
## so a room lit near is lit from afar, with the same lamp.
extends TestCase

const TOWN_MAP := 0
const SEED := 1


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	await get_tree().process_frame # the levels know their map once the first frame drew it
	await get_tree().process_frame
	var census: Dictionary = CityStore.query(&"bay_census")
	var panes := int(census.get("near_panes", 0))
	var bays := int(census.get("mid_bays", 0))
	print("panes: near %d, mid bays %d, unmatched %d, shutters %d, rooms %d" % [panes, bays, census.get("panes_unmatched", -1),
		census.get("mid_shutters", 0), census.get("mid_rooms", 0)])
	check(panes > 0 and bays >= panes, "the town has panes at both levels: %d near, %d mid bays" % [panes, bays])
	check(int(census.get("panes_unmatched", -1)) == 0,
		"every near pane has a mid bay with its room, its bay byte, its glass and its floor: %d of %d without one" % [census.get("panes_unmatched", -1), panes])
	check(int(census.get("mid_shutters", 0)) > 0, "the mid level's bays carry their roller shutters: %d" % census.get("mid_shutters", 0))
	check(int(census.get("mid_rooms", 0)) > int(census.get("buildings", 0)),
		"the mid level's bays light by rooms, more than one a building: %d rooms in %d buildings" % [census.get("mid_rooms", 0), census.get("buildings", 0)])
	finish()
