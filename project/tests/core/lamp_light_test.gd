## The street lamps' light, on each renderer: by day none is on and the sun casts; by night the lamps near the eye have a real light,
## as many as the tier pays for, the road takes over past them (`lamp_lit`) and the moon's shadow pass is gone; on Compatibility, and
## on a tier with no lights, the road draws every pool itself.
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const STREET_PITCH := 18.0
const STREET_DISTANCE := 60.0
const SETTLE_FRAMES := 60
const SETTLE_LIMIT := 600
const WITH_LIGHTS := "high"
const WITHOUT_LIGHTS := "low"
## The traffic put on the streets before the headlamps are counted: 30 s of the city, stepped by the test.
const TRAFFIC_TICKS := 600


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
	check(not world.sun.shadow_enabled, "under the lamps the moon's shadow pass is gone")
	var budget: int = Settings.current().lamp_lights
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


func _lit() -> int:
	return int(CityStore.query(&"render_cost").lit_street_lamps)


func _handed_over() -> Vector2:
	return CityStore.query(&"render_cost").lamps_hand_over


func _settled() -> void:
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
	var left := SETTLE_LIMIT
	while left > 0 and CityStore.query(&"render_pending") > 0:
		left -= 1
		await get_tree().process_frame
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
