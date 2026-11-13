## How the town's drivers drive, over a morning's minutes (probe driving_census): nobody takes a turn faster than its own driver
## takes that bend; the turns are taken at their own speeds, by their curves, not at one speed; the drivers differ; whoever turns
## shows that turn's indicator, and lane changes are signalled before they are made.
## Run: godot --path project res://tests/core/driving_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 7
const HOUR := 8.0 # the morning's traffic
const WARM_TICKS := 1200
const SAMPLES := 60
const EVERY_TICKS := 10
const VIEW_M := 300.0
const AT := Vector3.ZERO # the window's middle: the camera of a test stands at the origin
const LEAST_SEEN := 20
const OVER_MS := 1.0 # over its bend speed by less than this: the end of its braking as it comes onto the curve
const TURN_SPREAD_MS := 0.5 # the map's turns differ at least this much in the speed they are taken at
const TEMPER_SPAN := 0.6 # the most timid and the most pushy seen are at least this far apart
const TEMPER_MEAN := 0.25 # and the fleet's mean this near the ordinary driver


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	CityStore.query(&"set_city_hour", [HOUR])
	ticks(WARM_TICKS)
	var worst := 0.0
	var on_bends := 0
	var turning := 0
	var shown := 0
	var signalling := 0
	var wanting := 0
	var c := {}
	for i in SAMPLES:
		ticks(EVERY_TICKS)
		c = CityStore.query(&"driving_census", [AT, VIEW_M])
		worst = maxf(worst, c.over_bend_ms)
		on_bends += c.on_bends
		turning += c.turning
		shown += c.turning_shown
		signalling += c.signalling
		wanting += c.wanting
	check(c.vehicles >= LEAST_SEEN, "the town drives round the camera: %d vehicles" % c.vehicles)
	check(on_bends > 0 and worst <= OVER_MS,
			"nobody takes a turn faster than its own driver takes that bend: %d seen on bends, the worst %.2f m/s over" % [on_bends, worst])
	check(c.turn_bends > 0 and c.turn_fastest_ms - c.turn_slowest_ms >= TURN_SPREAD_MS,
			"the turns are taken by their curves: %d of them, an ordinary driver from %.1f to %.1f m/s; %d streets curve" %
			[c.turn_bends, c.turn_slowest_ms, c.turn_fastest_ms, c.curved_streets])
	check(c.temper_highest - c.temper_lowest >= TEMPER_SPAN and absf(c.temper_mean) <= TEMPER_MEAN,
			"the drivers differ: tempers from %.2f to %.2f, mean %.2f" % [c.temper_lowest, c.temper_highest, c.temper_mean])
	check(turning > 0 and shown == turning, "whoever turns shows that turn's indicator: %d of %d" % [shown, turning])
	check(signalling > shown, "turns are signalled before their line: %d signals seen, %d of them in the turn" % [signalling, shown])
	check(c.lane_changes > 0 and c.lane_signals >= c.lane_changes and wanting > 0,
			"lane changes are signalled, then made: %d signalled, %d made after their signal, %d forced at a line (%d streets of more than one lane)" %
			[c.lane_signals, c.lane_changes, c.lane_forced, c.wide_streets])
	finish()
