## What the GPU holds of the city follows the view, never the map: the far level of the roads is built for the superchunks within
## the buildings' reach of the eye and let go past it. Read on the core's count of the far road superchunks held (road_mesh_counts),
## with the eye over the town, then far from it, then back.
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const VIEW := [30.0, 35.0, 300.0] # the rig's yaw and pitch in degrees, and metres from the point it looks at
const AWAY_M := 8000.0 # from the town's middle: past the far level's reach and its drop margin, inside the town map's 20 km land
const SETTLE_LIMIT_S := 50.0 # real time a view may take to be built or let go (3000 frames of the old 60 Hz loop): StateWait


func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var town := rig.focus
	rig.jump(town, VIEW[0], VIEW[1], VIEW[2])
	await StateWait.until(get_tree(), func() -> bool: return _held() > 0 and _settled(), SETTLE_LIMIT_S)
	var over := _held()
	check(over > 0, "over the town the far level of its roads is drawn (%d superchunks)" % over)
	rig.jump(town + Vector3(AWAY_M, 0.0, 0.0), VIEW[0], VIEW[1], VIEW[2])
	await StateWait.until(get_tree(), func() -> bool: return _held() == 0 and _settled(), SETTLE_LIMIT_S)
	check(_held() == 0, "%d m from the town none of its far roads is held (%d superchunks)" % [int(AWAY_M), _held()])
	rig.jump(town, VIEW[0], VIEW[1], VIEW[2])
	await StateWait.until(get_tree(), func() -> bool: return _held() == over and _settled(), SETTLE_LIMIT_S)
	check(_held() == over, "back over the town they are drawn again (%d of %d superchunks)" % [_held(), over])
	finish()


func _held() -> int:
	return int(CityStore.query(&"road_mesh_counts").far_supers)


func _settled() -> bool:
	return int(CityStore.query(&"render_pending")) == 0
