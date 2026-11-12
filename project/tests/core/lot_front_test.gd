## Every lot of the generated town meets what is round it (probe lot_fronts): the lot's covers just inside its front stand level with
## the sidewalk just outside it, no step down and no grass bank between them, as a lot's paving meets the pavement it opens onto; where
## a footway runs along a lot's front or side, the footway's outer edge is the lot's line, no strip of land between and nothing of the lot
## over the footway; and a lot beside another in a row shares its side, no strip of bare land between the two. And every edge of a lot
## laid round a building is closed once (probe lot_bounds): by its boundary (a wall, fence, railing, hedge or gate on the line) or by a
## ground block standing on the edge, never a boundary in front of a block's face or through it, never a stretch left open; and no part
## of the lot (a carport's post, a bin, a bench) stands in its boundary, nor in another part's footprint (a clothes line over a pool's
## deck). And nothing of a lot stands on the paving as it is drawn (probe lot_paving: the bands, the junctions' plates out to their
## corners' footway backs, the bulbs), in the town and in the metropolis, whose wide footways give the corners with a round back.
## And a lot's covers lie in their order (probe lot_covers): wherever a later cover lies over an earlier one (a forecourt's asphalt over
## its pavers, a court's lines over the court) it stands over it everywhere, never crossing it, never at its depth; and no band of lawn
## or bare ground too thin to plant stands beside a hard cover (a walk, a yard, a forecourt) of a lot.
## Run: godot --path project res://tests/core/lot_front_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const METROPOLIS_MAP := 5
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
	_check_paving("the town", CityStore.query(&"lot_paving"))
	_check_covers("the town", CityStore.query(&"lot_covers"))
	var b: Dictionary = CityStore.query(&"lot_bounds")
	print("lot edges: %d lots, %d boundary pieces, %d doubled; %d points, %d open" % [b.lots, b.pieces, b.doubled, b.samples, b.open])
	check(int(b.lots) > 0 and int(b.pieces) > 0, "the town has lots laid with a boundary: %d lots, %d pieces" % [b.lots, b.pieces])
	check(int(b.doubled) == 0, "no boundary piece stands along a ground block's face on the same edge: %d of %d, the first (building, edge, kind, x, z, metres along) %s, by edge, piece, lot kind and front use %s"
			% [b.doubled, b.pieces, b.doubled_first, b.doubled_by])
	check(int(b.open) == 0, "every edge of a lot is closed by its boundary or a block standing on it: %.1f m open, the first (building, edge, x, z, kind, front depth) %s, by edge, lot kind and front use %s"
			% [float(b.open) * float(b.step_m), b.open_first, b.open_by])
	check(int(b.runs_in) == 0, "no run of a lot's boundary (a wall, a fence, a hedge) stands in a building's footprint: %d, the first (building, kind, x, z, metres in) %s" % [b.runs_in, b.runs_first])
	check(int(b.parts_in) == 0, "no part of a lot stands in its boundary: %d, the first (building, part kind, piece kind, x, z, metres in) %s, by part, lot kind and front use %s"
			% [b.parts_in, b.parts_first, b.parts_by])
	check(int(b.parts_meet) == 0, "no part of a lot stands in another's footprint (a pool's with its deck): %d pairs, the first (building, kinds, x, z, metres in) %s, by kinds, lot kind and front use %s"
			% [b.parts_meet, b.meet_first, b.meet_by])
	# The metropolis: its avenues' wide footways give the corners whose back follows the kerb's return (junction_corner.h OFFSET, RUN).
	new_map(METROPOLIS_MAP, SEED, true)
	_check_paving("the metropolis", CityStore.query(&"lot_paving"))
	_check_covers("the metropolis", CityStore.query(&"lot_covers"))
	finish()


## Every later cover of a lot over the earlier ones it lies on, by half a step at least, everywhere.
func _check_covers(city: String, d: Dictionary) -> void:
	print("%s: covers over covers: %d points in %d lots, %d not over" % [city, d.samples, d.lots, d.under])
	check(int(d.samples) > 0, "%s has covers lying over covers: %d" % [city, d.samples])
	check(int(d.under) == 0, "%s: every later cover stands over the earlier one it lies on: %d of %d not, the worst (building, x, z, later less earlier, elements) %s"
			% [city, d.under, d.samples, d.worst])
	print("%s: the lots' ground: %d lots read, %.1f m2 under no cover, %d bands too thin to plant in %d lots" % [city, d.ground_lots, d.bare_m2, d.slivers, d.sliver_lots])
	check(int(d.ground_lots) > 0, "%s has lots whose ground was read: %d" % [city, d.ground_lots])
	check(float(d.bare_m2) <= 0.0, "%s: every lot's ground is covered from line to line: %.1f m2 under no cover" % [city, d.bare_m2])
	check(int(d.sliver_lots) == 0, "%s: no planted or bare band too thin to plant stands beside a hard cover of a lot: %d lots, %d bands by scan line, the first (building, x, y in the lot's frame, the line's way) %s"
			% [city, d.sliver_lots, d.slivers, d.sliver_first])
	if int(d.sliver_lots) > 0:
		print("%s: the first such lot's ground (x0, y0, x1, y1, what: 1 planted, 2 hard, 3 a ground volume): %s" % [city, d.sliver_boxes])


## Nothing of a lot stands on the paving as it is drawn: its bands, its junctions' plates out to their corners' footway backs, its bulbs.
func _check_paving(city: String, d: Dictionary) -> void:
	print("%s: lots on the paving: %d of %d" % [city, d.paved, d.lots])
	check(int(d.lots) > 0, "%s has lots held against its paving: %d" % [city, d.lots])
	check(int(d.paved) == 0, "%s: no lot stands on the paving as it is drawn (a band, a junction's corner, a bulb): %d of %d, the worst (building, x, z, m2 on it) %s"
			% [city, d.paved, d.lots, d.worst])
