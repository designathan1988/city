## The street lamps' light, on each renderer: by day none is on and the sun casts; by night the lamps near the eye have a real light,
## as many as the tier pays for, the road takes over past them (`lamp_lit`) and the moon's shadow pass is gone; on Compatibility, and
## on a tier with no lights, the road draws every pool itself.
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const STREET_PITCH := 18.0
const STREET_DISTANCE := 60.0
const SETTLE_FRAMES := 60
const SETTLE_LIMIT := 600
const WITH_LIGHTS := "high"
const WITHOUT_LIGHTS := "low"
const TRAFFIC_SPEED := 2

var failures := 0
var _world: CityWorld


func _ready() -> void:
	_world = CityWorld.new()
	add_child(_world)
	var rig := CityCamera.new()
	add_child(rig)
	_check(CityStore.boot(_world.terrain, _world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	Settings.attach(_world, get_viewport())
	Settings.set_preset(WITH_LIGHTS)
	rig.jump(rig.focus, 0.0, STREET_PITCH, STREET_DISTANCE)
	var daylight: Dictionary = DataTables.game("daylight")
	_world.set_hour(daylight.day_hour)
	await _settled()
	_check(_lit() == 0, "by day no lamp is on: %d" % _lit())
	_check(_world.sun.shadow_enabled, "by day the sun casts")
	_world.set_hour(daylight.night_hour)
	await _settled()
	_check(not _world.sun.shadow_enabled, "under the lamps the moon's shadow pass is gone")
	var budget: int = Settings.current().lamp_lights
	var lit := _handed_over()
	if _world.contact.available():
		print("  lamps lit %d of %d, the road's pools from %.0f to %.0f m" % [_lit(), budget, lit.x, lit.y])
		_check(_lit() > 0 and _lit() <= budget, "by night the near lamps are lit, within the tier's: %d of %d" % [_lit(), budget])
		_check(lit.y > lit.x and lit.x > 0.0, "the road takes over past them: %s" % lit)
	else:
		_check(_lit() == 0, "Compatibility has no lamp lights: %d" % _lit())
		_check(lit.y == 0.0, "so the road draws every pool: %s" % lit)
	CityStore.dispatch(&"set_speed", [TRAFFIC_SPEED])
	await _settled()
	var heads: int = CityStore.query(&"render_cost").lit_head_lamps
	var most: int = Settings.current().head_lights
	CityStore.dispatch(&"set_speed", [0])
	if _world.contact.available():
		print("  headlamps lit %d of %d" % [heads, most])
		_check(heads > 0 and heads <= most, "the vehicles near the eye throw their light: %d of %d" % [heads, most])
	else:
		_check(heads == 0, "Compatibility has no headlamp lights: %d" % heads)
	Settings.set_preset(WITHOUT_LIGHTS)
	await _settled()
	lit = _handed_over()
	_check(_lit() == 0 and lit.y == 0.0, "a tier with no lights leaves the pools to the road: %d, %s" % [_lit(), lit])
	Settings.set_preset(WITH_LIGHTS)
	_world.set_hour(daylight.day_hour)
	await _settled()
	_check(_lit() == 0 and _world.sun.shadow_enabled, "and the day comes back: %d" % _lit())
	print("FAILURES: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


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


func _check(ok: bool, what: String) -> void:
	print("ok   " if ok else "FAIL ", what)
	if not ok:
		failures += 1
