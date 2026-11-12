## No two road surfaces of the generated town stand in one another (probe road_overlaps): the city's road surfaces built as the road
## chunks build them, no two top faces at one height over the same ground, so nothing of one shows through another (a corner's paving
## over an arm's planting strip, two arms' footways over one corner); every junction corner's footway outline a simple polygon; and
## the asphalt's wear as those meshes carry it (the probe's `wear`: plate_tone_test.gd holds the same on a junction laid by hand).
## Run: godot --path project res://tests/core/road_overlap_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var d: Dictionary = CityStore.query(&"road_overlaps")
	print("road tops: %d, pairs in one another: %d, ground %.2f m2, places %s" % [d.tops, d.pairs, d.ground_m2, d.places])
	check(int(d.tops) > 0, "the town has road surfaces: %d top faces" % d.tops)
	check(int(d.corners) > 0, "the town has junction corners: %d" % d.corners)
	check(int(d.corners_crossed) == 0, "every junction corner's footway is a simple polygon: %d of %d cross themselves (by back: squared, moved out, run on %s)"
			% [d.corners_crossed, d.corners, d.corners_kinds])
	check(int(d.corners_holed) == 0, "every junction corner's footway is drawn whole: %d of %d leave part of it out, the first (node, polygon) %s"
			% [d.corners_holed, d.corners, d.holed_first])
	check(int(d.buried) == 0, "the ground stands over no road surface: %d tops over %.2f m2 (by what drew them %s), the first (x, z, how far over, source) %s"
			% [d.buried, d.buried_m2, d.buried_by, d.buried_first])
	check(int(d.pairs) == 0, "no two road tops stand at one height over the same ground: %d pairs over %.2f m2, the first (x, z, area, heights) %s, by what drew them %s, the largest (x, z, area, sources) %s"
			% [d.pairs, d.ground_m2, d.first, d.by, d.worst])
	var w: Dictionary = d.wear
	print("the asphalt's wear as built: ", w)
	check(int(w.mouth_points) > 0 and int(w.mouth_off) == 0, "every junction's paving wears each arm's own asphalt on that arm's mouth: of %d vertices on a mouth %d do not, the first (x, y, z) %s"
			% [w.mouth_points, w.mouth_off, w.first_off])
	check(int(w.split) == 0, "every junction's paving has one colour at every point: %d vertices share a point with another colour" % w.split)
	check(int(w.kerb_points) > 0 and int(w.kerb_off) == 0, "every junction's paving knows where its kerb is: of %d vertices on a corner's kerb line %d are not at nought from it"
			% [w.kerb_points, w.kerb_off])
	check(int(w.lane_wide) > 0 and int(w.lane_off) == 0, "every lane knows where it is across itself: of %d lane vertices %d are at its width, %d before its own edge"
			% [w.lane_points, w.lane_wide, w.lane_off])
	finish()
