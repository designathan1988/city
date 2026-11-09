## The buildings' near level hands over to the mid level by a dissolve that runs once a sub-chunk crosses its switch and then ends
## (native/src/render/buildings/building_renderer_reach.cpp fade_levels): under a still eye no level stands part drawn, wherever the
## eye stands, so no screen-door of two surfaces is left for the temporal filter and the depth passes to see. Read on the core's probe
## (building_cost "dissolving"), from views that put sub-chunks round the near level's end.
extends TestCase

const TOWN_MAP := 0
const SEED := 1
## The views, metres from the town's middle at a pitch in degrees: the whole near reach and its band in sight, from three sides.
const VIEWS := [[30.0, 25.0, 476.0], [120.0, 35.0, 300.0], [210.0, 15.0, 650.0]]
const SETTLE_LIMIT_S := 50.0 # real time a view may take to be built and to end its dissolves (3000 frames of the old 60 Hz loop): StateWait


func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	for v: Array in VIEWS:
		rig.jump(rig.focus, v[0], v[1], v[2])
		# until the near level stands and the stream has nothing left to draw: a first frame has not asked for anything yet
		await StateWait.until(get_tree(), func() -> bool: return int(CityStore.query(&"building_cost").near_chunks) > 0 and int(CityStore.query(&"render_pending")) == 0, SETTLE_LIMIT_S)
		var cost: Dictionary = CityStore.query(&"building_cost")
		check(int(cost.near_chunks) > 0, "view %s: the near level is built (%d chunks)" % [v, int(cost.near_chunks)])
		await StateWait.until(get_tree(), func() -> bool: return int(CityStore.query(&"building_cost").dissolving) == 0, SETTLE_LIMIT_S)
		var left := int(CityStore.query(&"building_cost").dissolving)
		check(left == 0, "view %s: a still eye holds no level part drawn (%d left)" % [v, left])
	finish()
