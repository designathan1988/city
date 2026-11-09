## The buildings' near level hands over to the mid level by a dissolve that runs once a sub-chunk crosses its switch and then ends
## (native/src/render/buildings/building_renderer_reach.cpp fade_levels): under a still eye no level stands part drawn, wherever the
## eye stands, so no screen-door of two surfaces is left for the temporal filter and the depth passes to see. Read on the core's probe
## (building_cost "dissolving"), from views that put sub-chunks round the near level's end.
extends TestCase

const TOWN_MAP := 0
const SEED := 1
## The views, metres from the town's middle at a pitch in degrees: the whole near reach and its band in sight, from three sides.
const VIEWS := [[30.0, 25.0, 476.0], [120.0, 35.0, 300.0], [210.0, 15.0, 650.0]]
const SETTLE_LIMIT := 3000 # frames a view may take to be built and to end its dissolves, before the test calls it stuck


func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	for v: Array in VIEWS:
		rig.jump(rig.focus, v[0], v[1], v[2])
		var waited := 0 # until the near level stands and the stream has nothing left to draw: a first frame has not asked for anything yet
		while waited < SETTLE_LIMIT and (int(CityStore.query(&"building_cost").near_chunks) == 0 or CityStore.query(&"render_pending") > 0):
			await get_tree().process_frame
			waited += 1
		var cost: Dictionary = CityStore.query(&"building_cost")
		check(int(cost.near_chunks) > 0, "view %s: the near level is built (%d chunks)" % [v, int(cost.near_chunks)])
		while waited < SETTLE_LIMIT and int(CityStore.query(&"building_cost").dissolving) > 0:
			await get_tree().process_frame
			waited += 1
		var left := int(CityStore.query(&"building_cost").dissolving)
		check(left == 0, "view %s: a still eye holds no level part drawn (%d left after %d frames)" % [v, left, waited])
	finish()
