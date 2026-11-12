## A plant of a lot's garden is drawn inside the room its element is (probe garden_fit: one plant element alone, each of its
## instances' model measured point by point as the kit baked it, a leaf card's leaves as far as the shader spreads them): a tree's
## crown, a shrub, a run of hedge and a flower bed never pass their rectangle; a tree, a shrub and a hedge never stand taller than
## their element; and each fills its room (a hedge its run from end to end and its thickness, a tree and a shrub most of theirs), so
## no plant fits by being nothing, and a hedge is not cut into more pieces than its length asks; a run laid at an angle to its lot (a
## hedge the player traces) is planted along its own line. On an empty land, with no generated city: the rooms are those the lots'
## layout gives (sim/buildings/b40: lot_front, lot_yard, lot_back, lot_boundary, open_lots). And an open lot's grove (probe grove_fit:
## a park or a square alone): no crown past the lot's lines, no tree more slender than a wood's, every tree still a tree.
## Run: godot --path project res://tests/core/garden_room_test.tscn -- --no-flow
extends TestCase

const LAND_MAP := 0
const SEED := 7
const DRAWS := 24 # of each room: the species, the turn, the share of the room taken
const SLACK_M := 0.01
const END_M := 0.02 # a hedge's drawn end stands this near its run's end
const PIECE_LEAST_M := 1.5 # a hedge's pieces are no shorter than this, but for the last
## kind, width, depth, height of the rooms the layout gives its plants
const ROOMS := [
	["tree", 2.6, 2.6, 4.5], ["tree", 3.0, 3.0, 5.0], ["tree", 3.0, 3.0, 4.2], ["tree", 2.5, 2.5, 5.0], ["tree", 1.6, 1.6, 2.67], ["tree", 0.7, 0.7, 1.17],
	["shrub", 1.3, 1.3, 1.4], ["shrub", 1.2, 1.2, 0.8],
	["hedge", 12.0, 0.7, 1.2], ["hedge", 0.7, 9.0, 1.8], ["hedge", 30.0, 0.8, 1.1], ["hedge", 1.1, 0.7, 1.0],
	["flowers", 1.6, 1.2, 0.4], ["flowers", 1.6, 0.8, 0.35], ["flowers", 1.2, 1.2, 0.8], ["flowers", 1.4, 1.4, 0.4],
]
## The rooms of a square's trees (sim/citygen/squares.cpp): a whole step of its lattice with the plan's tallest and lowest tree, and
## the narrowest room, by the square's line.
const SQUARE_TREES := [["tree", 9.0, 9.0, 14.0], ["tree", 9.0, 9.0, 9.0], ["tree", 6.0, 6.0, 6.0]]
const TURNED := [["hedge", 14.0, 0.7, 1.4], ["flowers", 2.4, 1.0, 0.5]]
const TURN_DEG := 37.0
const ASKEW_M := 0.6 # a run's box in the lot's frame against its turned rectangle's: the pieces' rounded ends and corners are inside it
## The least a plant fills of its room: a tree and a shrub of its smaller side, by their wider side; a hedge of its thickness; a bed of each side.
const FILLS := {"tree": 0.6, "shrub": 0.5, "hedge": 0.9, "flowers": 0.6}
const TALL := {"tree": 0.6, "shrub": 0.5, "hedge": 0.9} # the least of its element's height a plant stands
## The open lots' groves: the lot's function (BuildingFn: 5 a square, 6 a park), its width and depth
const GROVES := [[6, 30.0, 40.0], [6, 44.0, 60.0], [6, 24.0, 30.0], [6, 70.0, 70.0], [5, 40.0, 40.0]]
const GROVE_DRAWS := 12
const GROVE_TREE_LEAST_M := 4.0 # a grove's tree stands at least this tall, however near the lot's line


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(LAND_MAP, SEED, false)
	for room: Array in ROOMS + SQUARE_TREES:
		_check_room(room[0], room[1], room[2], room[3], 0.0)
	# A run the player traces across the lot is laid at an angle: the same plant, in its own turned frame.
	for room: Array in TURNED:
		_check_room(room[0], room[1], room[2], room[3], deg_to_rad(TURN_DEG))
	for lot: Array in GROVES:
		_check_grove(lot[0], lot[1], lot[2])
	finish()


## An open lot's grove (probe grove_fit): no leaf past the lot's lines, and a tree narrowed by its room still a tree.
func _check_grove(fn: int, w: float, d: float) -> void:
	var trees := 0
	var past := 0.0
	var slender := 0.0
	var most := 0.0
	var low := 1e9
	for draw in GROVE_DRAWS:
		var fit: Dictionary = CityStore.query(&"grove_fit", [fn, w, d, draw + 1])
		trees += int(fit.trees)
		most = float(fit.slender_most)
		if int(fit.trees) == 0:
			continue
		past = maxf(past, float(fit.past_m))
		slender = maxf(slender, float(fit.slender))
		low = minf(low, float(fit.least_tall_m))
	var name := "grove of an open lot (function %d) %.0f x %.0f" % [fn, w, d]
	print("%s: %d trees in %d draws, past %.3f m, most slender %.2f (the land's %.2f), least %.1f m tall" % [name, trees, GROVE_DRAWS, past, slender, most, low])
	if not check(trees > 0, "%s: it has trees" % name):
		return
	check(past <= SLACK_M, "%s: every crown inside the lot's lines: %.3f m past them" % [name, past])
	check(slender <= most + SLACK_M, "%s: no tree more slender than a wood's (%.2f against %.2f)" % [name, slender, most])
	check(low >= GROVE_TREE_LEAST_M, "%s: its trees are trees: the least %.1f m tall" % [name, low])


func _check_room(kind: String, w: float, d: float, h: float, turn: float) -> void:
	var past := 0.0
	var over := 0.0
	var thin := 1.0
	var low := 1.0
	var short := 0.0
	var askew := 0.0
	var pieces := 0
	for draw in DRAWS:
		var fit: Dictionary = CityStore.query(&"garden_fit", [kind, w, d, h, turn, draw + 1])
		if not check(int(fit.instances) > 0, "%s in %.1f x %.1f: it is drawn (draw %d)" % [kind, w, d, draw]):
			return
		var box: Array = fit.box
		var across := Vector2(float(box[2]) - float(box[0]), float(box[3]) - float(box[1]))
		past = maxf(past, float(fit.past_m))
		over = maxf(over, float(fit.over_m))
		low = minf(low, float(fit.top) / h)
		pieces = maxi(pieces, int(fit.instances))
		# Laid at an angle, the plant lies along its own line: in the lot's frame its box is the turned rectangle's.
		var lot: Array = fit.lot_box
		var wide := absf(w * cos(turn)) + absf(d * sin(turn))
		var deep := absf(w * sin(turn)) + absf(d * cos(turn))
		askew = maxf(askew, maxf(absf(float(lot[2]) - float(lot[0]) - wide), absf(float(lot[3]) - float(lot[1]) - deep)))
		if kind == "hedge":
			thin = minf(thin, minf(across.x, across.y) / minf(w, d))
			short = maxf(short, maxf(w, d) - maxf(across.x, across.y))
		elif kind == "flowers":
			thin = minf(thin, minf(across.x / w, across.y / d))
		else:
			thin = minf(thin, maxf(across.x, across.y) / minf(w, d))
	var name := "%s in %.1f x %.1f, %.2f tall, turned %.0f deg" % [kind, w, d, h, rad_to_deg(turn)]
	print("%s: past %.3f m, over %.3f m, fills %.2f, stands %.2f, %d piece(s)" % [name, past, over, thin, low, pieces])
	check(past <= SLACK_M, "%s: drawn inside its room: %.3f m past it" % [name, past])
	check(thin >= float(FILLS[kind]), "%s: fills its room: %.2f of it, at the least %.2f" % [name, thin, FILLS[kind]])
	if kind == "hedge" or kind == "flowers":
		check(askew <= ASKEW_M, "%s: lies along its own line in the lot: its box there %.2f m off the turned room's" % [name, askew])
	if kind == "flowers":
		return # a bed's clumps have their own height
	check(over <= SLACK_M, "%s: no taller than its element: %.3f m over it" % [name, over])
	check(low >= float(TALL[kind]), "%s: stands its height: %.2f of it, at the least %.2f" % [name, low, TALL[kind]])
	if kind == "hedge":
		check(short <= 2.0 * END_M, "%s: runs from end to end of its room: %.3f m short" % [name, short])
		check(pieces <= int(ceil(maxf(w, d) / PIECE_LEAST_M)) + 1, "%s: in no more pieces than its length asks: %d" % [name, pieces])
