## Every lot of the generated town meets what is round it (probe lot_fronts): the lot's covers just inside its front stand level with
## the sidewalk just outside it, no step down and no grass bank between them, as a lot's paving meets the pavement it opens onto; where
## a footway runs along a lot's front or side, the footway's outer edge is the lot's line, no strip of land between and nothing of the lot
## over the footway; and a lot beside another in a row shares its side, no strip of bare land between the two. And every edge of a lot
## laid round a building is closed once (probe lot_bounds): by its boundary (a wall, fence, railing, hedge or gate on the line) or by a
## ground block standing on the edge, never a boundary in front of a block's face or through it, never a stretch left open; and no part
## of the lot (a carport's post, a bin, a bench) stands in its boundary.
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
	print("lot fronts: %d, off %d; seams %d, off %d; sides %d, apart %d" % [d.fronts, d.off, d.seams, d.seams_off, d.sides, d.sides_apart])
	check(int(d.fronts) > 0, "the town has lot fronts with a sidewalk outside them: %d" % d.fronts)
	check(int(d.off) == 0, "every lot front meets the footway's top: %d of %d not, the worst (building, x, z, lot less footway) %s"
			% [d.off, d.fronts, d.worst])
	check(int(d.seams) > 0, "the town has lots with a footway along them: %d" % d.seams)
	check(int(d.seams_off) == 0, "every lot's line along a footway is the footway's outer edge: %d of %d not, the worst (building, x, z, edge outside the line) %s"
			% [d.seams_off, d.seams, d.seam_worst])
	check(int(d.sides) > 0, "the town has lots side by side: %d" % d.sides)
	check(int(d.sides_apart) == 0, "every lot shares its side with the lot beside it: %d of %d with bare land between, the first (building, x, z, reach) %s"
			% [d.sides_apart, d.sides, d.side_worst])
	var b: Dictionary = CityStore.query(&"lot_bounds")
	print("lot edges: %d lots, %d boundary pieces, %d doubled; %d points, %d open" % [b.lots, b.pieces, b.doubled, b.samples, b.open])
	check(int(b.lots) > 0 and int(b.pieces) > 0, "the town has lots laid with a boundary: %d lots, %d pieces" % [b.lots, b.pieces])
	check(int(b.doubled) == 0, "no boundary piece stands along a ground block's face on the same edge: %d of %d, the first (building, edge, kind, x, z, metres along) %s, by edge, piece, lot kind and front use %s"
			% [b.doubled, b.pieces, b.doubled_first, b.doubled_by])
	check(int(b.open) == 0, "every edge of a lot is closed by its boundary or a block standing on it: %.1f m open, the first (building, edge, x, z, kind, front depth) %s, by edge, lot kind and front use %s"
			% [float(b.open) * float(b.step_m), b.open_first, b.open_by])
	check(int(b.parts_in) == 0, "no part of a lot stands in its boundary: %d, the first (building, part kind, piece kind, x, z, metres in) %s, by part, lot kind and front use %s"
			% [b.parts_in, b.parts_first, b.parts_by])
	finish()
