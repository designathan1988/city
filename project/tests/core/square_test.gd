## A square of the city's plan is whole (probe square_fit: a square laid on a block of lots given by hand, no city generated): every
## tree is an element whose rectangle is the room of its crown, so each room lies on the square's own ground, no two rooms lie in one
## another, a room is as wide as the block's lines and the step between two trees give (never narrower than the clearance a trunk
## keeps from the lines, on either side), a tree narrowed by its room is no more slender than the plan's tallest on a whole step, and
## no trunk stands on a walk. And a block that is no rectangle is no square: a square is one record, and a record's lot is a rectangle.
## Run: godot --path project res://tests/core/square_test.tscn -- --no-flow
extends TestCase

const LAND_MAP := 0
const SEED := 7
const DRAWS := 6 # squares of each block, by their trees' draws
const SLACK := 0.01
const LOT_W := 18.0
const LOT_D := 27.0
const LOT_NARROW_W := 16.25 # six across: the lattice's last trees stand nearer the block's lines than half a step
const TURN := 0.4 # radians: a block laid at an angle to the world
## A block's lots as cells of two rows back to back: columns across, and the cells left out (row, column).
const WHOLE := []
const AN_L := [[1, 4], [1, 5]]
const A_NOTCH := [[1, 2]]


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(LAND_MAP, SEED, false)
	_check_square("a block of twelve lots, 108 x 54", _block(6, WHOLE, Vector2.ZERO, 0.0, LOT_W), 30, false)
	_check_square("a block of four lots, 36 x 54", _block(2, WHOLE, Vector2(300, -120), 0.0, LOT_W), 4, false)
	_check_square("a block of twelve lots laid at an angle", _block(6, WHOLE, Vector2(-500, 240), TURN, LOT_W), 30, false)
	_check_square("a block of twelve lots, 97.5 x 54, far from the map's middle", _block(6, WHOLE, Vector2(11000, -9000), TURN, LOT_NARROW_W), 40, true)
	_check_no_square("an L of ten lots", _block(6, AN_L, Vector2.ZERO, 0.0, LOT_W))
	_check_no_square("a block with a lot missing from a row", _block(6, A_NOTCH, Vector2(80, 80), TURN, LOT_W))
	finish()


## Two rows of `columns` lots `lot_w` wide back to back about `middle`, turned `turn` radians, less the cells of `left_out`: row 0
## faces the block's +z street, row 1 the other; a row is (x, z, width, depth, the way it faces).
func _block(columns: int, left_out: Array, middle: Vector2, turn: float, lot_w: float) -> Array:
	var lots := []
	for row in 2:
		for column in columns:
			if left_out.has([row, column]):
				continue
			var at := Vector2((float(column) - float(columns - 1) / 2.0) * lot_w, (0.5 - float(row)) * LOT_D)
			# A lot's +z faces its street: turned with the block, the far row half a turn more.
			lots.append([middle.x + at.x * cos(turn) + at.y * sin(turn), middle.y - at.x * sin(turn) + at.y * cos(turn), lot_w, LOT_D, turn + PI * float(row)])
	return lots


## `narrowed`: the block's lines stand nearer its last trees than half a step, so those trees' rooms are narrower than a step.
func _check_square(name: String, lots: Array, trees_least: int, narrowed: bool) -> void:
	for draw in DRAWS:
		var fit: Dictionary = CityStore.query(&"square_fit", [lots, draw + 1])
		if not check(bool(fit.laid), "%s: a square is laid on it" % name):
			return
		var step := float(fit.step_m)
		var edge := float(fit.edge_m)
		if draw == 0:
			print("%s: %.0f x %.0f, %d trees, rooms %.2f to %.2f m, the most slender %.2f, %d past the ground, %d in one another, %d on a walk"
					% [name, fit.w, fit.d, fit.trees, fit.least_room_m, fit.most_room_m, fit.slender, fit.out, fit.meet, fit.on_walk])
		check(int(fit.trees) >= trees_least, "%s: it has its trees: %d, at the least %d" % [name, fit.trees, trees_least])
		check(int(fit.out) == 0, "%s: every tree's room lies on the square's ground: %d of %d past it, the first (x, y, width, depth) %s" % [name, fit.out, fit.trees, fit.first_out])
		check(int(fit.meet) == 0, "%s: no two trees' rooms lie in one another: %d pairs, the first %s" % [name, fit.meet, fit.first_meet])
		check(int(fit.on_walk) == 0, "%s: no trunk stands on a walk: %d, the first %s" % [name, fit.on_walk, fit.first_walk])
		check(int(fit.plazas_round) == 1 and int(fit.plaza_covers) == 1, "%s: its plaza is one round cover %.1f m across about the walks' crossing: %d cover(s), the first (covers, round, off the crossing, size) %s"
				% [name, fit.plaza_m, fit.plaza_covers, fit.first_plaza])
		check(int(fit.covers_short) == 0, "%s: on sloping ground every cover of it is drawn whole: %d drawn short, the first (element, middle, size, m2 drawn, m2 of its outline) %s"
				% [name, fit.covers_short, fit.first_short])
		check(float(fit.least_room_m) >= minf(step, 2.0 * edge) - SLACK, "%s: a tree's room is the room it has: the narrowest %.2f m, with %.1f m to the lines on either side and %.1f m between trees"
				% [name, fit.least_room_m, edge, step])
		check(float(fit.most_room_m) <= step + SLACK, "%s: no room wider than the step between two trees: %.2f m of %.1f" % [name, fit.most_room_m, step])
		if narrowed:
			check(float(fit.least_room_m) < step - SLACK, "%s: a tree by the block's line has the room the line leaves it: the narrowest %.2f m of a step of %.1f" % [name, fit.least_room_m, step])
		check(float(fit.slender) <= float(fit.high_m) / step + SLACK, "%s: no tree more slender than the plan's tallest on a whole step: %.2f against %.2f"
				% [name, fit.slender, float(fit.high_m) / step])


func _check_no_square(name: String, lots: Array) -> void:
	var fit: Dictionary = CityStore.query(&"square_fit", [lots, 1])
	check(not bool(fit.laid), "%s: no square is laid on a block that is no rectangle (%d trees laid on %.0f x %.0f)" % [name, fit.trees, fit.w, fit.d])
