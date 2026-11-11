## Every lot of the generated town meets the footway it faces at the footway's top: the lot's covers just inside its front stand
## level with the sidewalk just outside it, no step down and no grass bank between them (probe lot_fronts), as a lot's paving
## meets the pavement it opens onto.
## Run: godot --path project res://tests/core/lot_front_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var d: Dictionary = CityStore.query(&"lot_fronts")
	check(int(d.fronts) > 0, "the town has lot fronts with a sidewalk outside them: %d" % d.fronts)
	check(int(d.off) == 0, "every lot front meets the footway's top: %d of %d not, the worst (building, x, z, lot less footway) %s"
			% [d.off, d.fronts, d.worst])
	finish()
