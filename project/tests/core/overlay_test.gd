## The map layers (native/src/render/overlays). The congestion map lays a chunk's carriageways once and colours them from a
## texture of levels its shader reads, a texel a segment: a refresh sends that texture and makes no mesh. The zone colours stand
## beside the zoned sides. Neither builds while its thread holds the map (probe map_hold). Read on the core's own counts (probe
## overlay_probe) and on the level texture as the GPU has it.
extends TestCase

const TOWN_MAP := 0
const SEED := 1
## The view, metres from the town's middle at a pitch in degrees: the streets within the detailed roads' reach.
const VIEW := [120.0, 35.0, 300.0]
const WARM_TICKS := 600 # simulated ticks before the layer is read: vehicles on the streets
const REFRESHES := 2 # refreshes waited for once the chunks are laid
const SETTLE_LIMIT_S := 50.0 # real time a layer may take to be drawn (3000 frames of the old 60 Hz loop): StateWait
const LEVEL_TOP := 255 # a texel of the level texture at free flow
## The lanes of the layers' jobs (core/job_pool.h job_lane_name): each must have built nothing under the map.
const LANES := ["zones", "congestion"]


func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	CityStore.query(&"map_hold", [true])
	rig.jump(rig.focus, VIEW[0], VIEW[1], VIEW[2])
	await StateWait.until(get_tree(), func() -> bool: return int(CityStore.query(&"road_mesh_counts").roads.chunks) > 0 and int(CityStore.query(&"render_pending")) == 0, SETTLE_LIMIT_S)
	ticks(WARM_TICKS)
	await _congestion()
	await _zones()
	var held: Dictionary = CityStore.query(&"map_hold", [false])
	check(held.counted, "this DLL counts what is built under the map (the tests' DLL)")
	for lane: String in LANES:
		var built := int(held.built.get(lane, 0))
		check(built == 0, "%s built nothing while it held the map (%d vertices)" % [lane, built])
	finish()


func _probe() -> Dictionary:
	return CityStore.query(&"overlay_probe")


func _laid(p: Dictionary) -> bool:
	return int(p.flow_chunks) > 0 and int(p.pending) == 0 and int(p.refreshes) > 0


func _congestion() -> void:
	CityStore.dispatch(&"set_overlay", ["congestion", true])
	await StateWait.until(get_tree(), func() -> bool: return _laid(_probe()), SETTLE_LIMIT_S)
	var laid := _probe()
	check(_laid(laid), "the congestion map is laid (%d chunks, %d to come, %d refreshes)" % [laid.flow_chunks, laid.pending, laid.refreshes])
	await StateWait.until(get_tree(), func() -> bool: return int(_probe().refreshes) >= int(laid.refreshes) + REFRESHES, SETTLE_LIMIT_S)
	var after := _probe()
	check(int(after.refreshes) >= int(laid.refreshes) + REFRESHES, "the layer was refreshed again (%d times)" % (int(after.refreshes) - int(laid.refreshes)))
	check(int(after.meshes_made) == int(laid.meshes_made), "a refresh makes no mesh (%d made while refreshing)" % (int(after.meshes_made) - int(laid.meshes_made)))
	# the ramp: jammed at and under jam_share, free at and over free_share, half way between them in the middle
	var ramp: PackedByteArray = after.get("ramp", PackedByteArray())
	check(ramp.size() == 5 and ramp[0] == 0 and ramp[1] == 0 and absi(ramp[2] * 2 - LEVEL_TOP) <= 1 and ramp[3] == LEVEL_TOP and ramp[4] == LEVEL_TOP,
			"a speed's level on the ramp, from stopped to free: %s" % [ramp])
	var levels: PackedByteArray = after.get("levels", PackedByteArray())
	var segments := int(after.get("segments", 0))
	check(segments > 0 and levels.size() >= segments, "a level a segment (%d levels, %d segments)" % [levels.size(), segments])
	var texture: ImageTexture = after.get("texture")
	check(texture != null and texture.get_image().get_data() == levels, "the level texture on the GPU holds those levels")


func _zoned(p: Dictionary) -> bool:
	return int(p.zone_chunks) > 0 and int(p.pending) == 0


func _zones() -> void:
	CityStore.dispatch(&"show_every_zone", [true])
	CityStore.dispatch(&"set_overlay", ["zones", true])
	await StateWait.until(get_tree(), func() -> bool: return _zoned(_probe()), SETTLE_LIMIT_S)
	var zoned := _probe()
	check(_zoned(zoned), "the zones of every zoned side are drawn (%d chunks, %d to come)" % [zoned.zone_chunks, zoned.pending])
