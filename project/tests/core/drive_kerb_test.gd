## Every car gate of the generated town meets a lowered kerb: at the middle of each lot's car gate that faces a kerb, the road surface
## draws the kerb at the lip a vehicle access keeps (streets.json dropped_kerb drive_low_m, ABNT NBR 9050), as the cars, vans and
## lorries that cross the footway there need (probe drive_kerbs).
## Run: godot --path project res://tests/core/drive_kerb_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var d: Dictionary = CityStore.query(&"drive_kerbs")
	check(int(d.gates) > 0, "the town has car gates facing a kerb: %d" % d.gates)
	check(int(d.not_lowered) == 0, "every car gate's kerb stands at the lip (%.2f m): %d of %d not, the first (segment, side, arc, top) %s"
			% [d.lip_m, d.not_lowered, d.gates, d.first])
	finish()
