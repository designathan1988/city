## The graphics tier reaches the render whatever the order of the two calls a scene makes: applied before the core's boot (as a
## capture does) or after it (as the game does), the render holds the same tier: the street lamps with a light of their own and how
## many of them cast, read back from the lamps' pool (probe render_cost, `tier`). A tier that reaches a render not made yet must not
## be dropped, in part or whole. Forward+: on Compatibility the tier gives the lamps no light at all.
## Run: godot --path project --rendering-method forward_plus res://tests/core/tier_order_test.tscn -- --no-flow
extends TestCase

const TIER := "high" # a tier whose lamps cast


func _ready() -> void:
	world = CityWorld.new()
	add_child(world)
	Settings.preset = TIER
	Settings.apply(world, get_viewport()) # before the boot: the render is not made yet
	var errors := CityStore.boot(world.terrain, world.materials())
	check(errors.is_empty(), "boot %s" % [errors])
	var before: Dictionary = CityStore.query(&"render_cost").tier
	Settings.apply(world, get_viewport()) # and again, once it is
	var after: Dictionary = CityStore.query(&"render_cost").tier
	var asked: Dictionary = DataTables.game("settings").tiers[TIER]
	check(int(after.lamps) == int(asked.lamp_lights) and int(after.lamp_casting) == int(asked.lamp_shadows) and int(asked.lamp_shadows) > 0,
			"applied after the boot the lamps' pool holds the tier's lights: %s, the tier's %d of which %d cast" % [after, asked.lamp_lights, asked.lamp_shadows])
	check(before == after, "applied before the boot it holds the same: %s" % [before])
	finish()
