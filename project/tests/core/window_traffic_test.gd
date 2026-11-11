## The streets round the view carry their traffic in any city: in the 24 km city, a while after it opens, the vehicles that drive
## round the camera are at least half of what the window's lanes carry at the hour (traffic.json density_per_lane_m and the day's
## profile, the probe's `window`), as in the town. Run: godot --path project res://tests/core/window_traffic_test.tscn -- --no-flow
extends TestCase

const BIG_MAP := 4 # maps.json km24
const SEED := 1
const HOUR := 16.0
const SETTLE_TICKS := 1200
const FILLED_SHARE := 0.5 # the opening fills to traffic.json fill_share (0.8) of the demand; the hour and the governor move it


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(BIG_MAP, SEED, true)
	CityStore.query(&"set_city_hour", [HOUR])
	ticks(SETTLE_TICKS)
	var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
	var w: Array = probe.window
	print("window: %.0f lane metres, they carry %.0f, %d drive there, %d in the fleet" % [w[0], w[1], w[2], probe.vehicles])
	check(float(w[1]) > 0.0 and float(w[2]) >= FILLED_SHARE * float(w[1]), "the streets round the view carry their traffic: %d drive there of the %.0f its lanes carry" % [w[2], w[1]])
	finish()
