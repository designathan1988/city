## The street lamps' light, on each renderer: by day none is on and the sun casts; by night the lamps near the eye have a real light,
## as many as the tier pays for, the road takes over past them (`lamp_lit`) and the moon casts the city's shadows; on Compatibility, and
## on a tier with no lights, the road draws every pool itself.
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const STREET_PITCH := 18.0
const STREET_DISTANCE := 60.0
const SETTLE_FRAMES := 60
const SETTLE_LIMIT_S := 10.0 # real time the mesh jobs may take (600 frames of the old 60 Hz loop): StateWait
const WITH_LIGHTS := "high"
const WITHOUT_LIGHTS := "low"
## The traffic put on the streets before the headlamps are counted: 30 s of the city, stepped by the test.
const TRAFFIC_TICKS := 600
const QUADRANTS := 4 # of a positional shadow atlas
const NEAREST_CASTERS := 2 # the lamps either side of the eye: what a view of a street shows up close


func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	Settings.attach(world, get_viewport())
	Settings.set_preset(WITH_LIGHTS)
	rig.jump(rig.focus, 0.0, STREET_PITCH, STREET_DISTANCE)
	var daylight: Dictionary = DataTables.game("daylight")
	world.set_hour(daylight.day_hour)
	await _settled()
	check(_lit() == 0, "by day no lamp is on: %d" % _lit())
	check(world.sun.shadow_enabled, "by day the sun casts")
	world.set_hour(daylight.night_hour)
	await _settled()
	# the moon is the sun's light at night and keeps its cascades, as the reference's does (environment.ts setTimeOfDay): the city's
	# shadows fall from it, faint as its light, whole
	check(world.sun.shadow_enabled and is_equal_approx(world.sun.shadow_opacity, 1.0), "by night the moon casts the city's shadows: %s, %.2f" % [
			world.sun.shadow_enabled, world.sun.shadow_opacity])
	var budget: int = Settings.current().lamp_lights
	_check_casting(Settings.current())
	var lit := _handed_over()
	if world.contact.available():
		print("  lamps lit %d of %d, the road's pools from %.0f to %.0f m" % [_lit(), budget, lit.x, lit.y])
		check(_lit() > 0 and _lit() <= budget, "by night the near lamps are lit, within the tier's: %d of %d" % [_lit(), budget])
		check(lit.y > lit.x and lit.x > 0.0, "the road takes over past them: %s" % lit)
	else:
		check(_lit() == 0, "Compatibility has no lamp lights: %d" % _lit())
		check(lit.y == 0.0, "so the road draws every pool: %s" % lit)
	ticks(TRAFFIC_TICKS)
	await _settled()
	var heads: int = CityStore.query(&"render_cost").lit_head_lamps
	var most: int = Settings.current().head_lights
	if world.contact.available():
		print("  headlamps lit %d of %d" % [heads, most])
		check(heads > 0 and heads <= most, "the vehicles near the eye throw their light: %d of %d" % [heads, most])
	else:
		check(heads == 0, "Compatibility has no headlamp lights: %d" % heads)
	Settings.set_preset(WITHOUT_LIGHTS)
	await _settled()
	lit = _handed_over()
	check(_lit() == 0 and lit.y == 0.0, "a tier with no lights leaves the pools to the road: %d, %s" % [_lit(), lit])
	Settings.set_preset(WITH_LIGHTS)
	world.set_hour(daylight.day_hour)
	await _settled()
	check(_lit() == 0 and world.sun.shadow_enabled, "and the day comes back: %d" % _lit())
	finish()


## The tier's casting lamps by night: a map of the atlas each, the nearest of the tier's side, and their shadow read through the lens.
func _check_casting(q: Dictionary) -> void:
	var near_maps := _slots_at_least(get_viewport(), int(q.lamp_shadow_px))
	var maps := _slots_at_least(get_viewport(), 1)
	print("  positional atlas %d px: %d maps, %d of %d px or more, for %d casting lamps" % [
			get_viewport().positional_shadow_atlas_size, maps, near_maps, q.lamp_shadow_px, q.lamp_shadows])
	check(maps >= int(q.lamp_shadows), "every casting lamp gets a shadow map: %d maps for %d lamps" % [maps, q.lamp_shadows])
	check(near_maps >= mini(NEAREST_CASTERS, int(q.lamp_shadows)), "the lamps nearest the eye get maps of the tier's side: %d of %d px" % [near_maps,
			q.lamp_shadow_px])
	if int(q.lamp_shadows) == 0 or not world.contact.available():
		return
	# a spot's shadow blur scales its lens's penumbra, its filter and its depth bias (Godot light_storage.cpp, spot): at 0 all three are
	# gone, one hard tap that shows the map's texels as stairs and a surface that shades itself in bands
	var shadow := _shadow()
	print("  casting lamps' shadow: lens %.2f m, blur %.2f, bias %.3f, normal bias %.2f" % [shadow.get("lens_m", 0.0), shadow.get("blur", 0.0),
			shadow.get("bias", 0.0), shadow.get("normal_bias", 0.0)])
	check(float(shadow.get("lens_m", 0.0)) > 0.0, "a casting lamp's shadow has its lens's penumbra (light size): %s" % shadow)
	check(float(shadow.get("blur", 0.0)) > 0.0, "a casting lamp's shadow is filtered through its lens: %s" % shadow)
	check(float(shadow.get("bias", 0.0)) > 0.0 and float(shadow.get("normal_bias", 0.0)) > 0.0,
			"a casting lamp's shadow stands off the surface it falls on: %s" % shadow)


func _lit() -> int:
	return int(CityStore.query(&"render_cost").lit_street_lamps)


## How many maps of the viewport's positional atlas have a side of `px` or more: each quadrant is a quarter of the atlas, split in
## 4^(subdiv - 1) maps (Viewport.PositionalShadowAtlasQuadrantSubdiv; 0 unused).
func _slots_at_least(vp: Viewport, px: int) -> int:
	var count := 0
	var quadrant := vp.positional_shadow_atlas_size / 2
	for i in QUADRANTS:
		var subdiv := int(vp.get_positional_shadow_atlas_quadrant_subdiv(i))
		if subdiv == Viewport.SHADOW_ATLAS_QUADRANT_SUBDIV_DISABLED:
			continue
		var per_side := 1 << (subdiv - 1)
		if quadrant / per_side >= px:
			count += per_side * per_side
	return count


## What the casting lamps' shadow is read through (render_cost lamp_shadow): the lens, the blur, the depth and normal bias.
func _shadow() -> Dictionary:
	return CityStore.query(&"render_cost").get("lamp_shadow", {})


func _handed_over() -> Vector2:
	return CityStore.query(&"render_cost").lamps_hand_over


func _settled() -> void:
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
	await StateWait.drawn(get_tree(), SETTLE_LIMIT_S)
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
