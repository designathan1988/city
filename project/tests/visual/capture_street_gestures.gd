## The gestures of the Street tab, by the mouse (capture_street.gd, `cap`): the barriers traced point to point, a row dragged along a footway, a piece taken away
## with Shift, and the set of furniture a new road is laid with, picked on the road's options (the Furniture row).
class_name CaptureStreetGestures
extends RefCounted

const ROW_M := 65.0
const ROW_BACK_M := 34.0
const LEVEL_BASIC := 1
const LEVEL_FULL := 0
const BARRIER_LEG_M := 8.0
const BARRIER_GAP_M := 10.0
const BARRIER_FROM_M := 8.0


static func run(cap: Node, dir: Vector3) -> void:
	await _row(cap, dir)
	await _shift_click(cap)
	await _furniture_set(cap)


static func _row(cap: Node, dir: Vector3) -> void:
	await cap._pick("streetscape.bin")
	var from: Vector3 = cap._across - dir * ROW_BACK_M
	var to: Vector3 = from + dir * ROW_M
	var before: int = CityStore.query(&"street_counts").items
	await cap.ui.hover(from)
	await cap.ui.button(cap.rig.camera.unproject_position(cap.ui.ground(from.x, from.z)), true)
	await cap.ui.hover(from.lerp(to, 0.5))
	await cap.ui.hover(to)
	await cap.ui.button(cap.rig.camera.unproject_position(cap.ui.ground(to.x, to.z)), false)
	var laid: int = CityStore.query(&"street_counts").items - before
	cap._check(laid >= 2, "row dragged: %d pieces" % laid)
	CityStore.dispatch(&"undo")
	cap._check(CityStore.query(&"street_counts").items == before, "the row leaves in one undo")


static func _shift_click(cap: Node) -> void:
	if not cap.spots.has("streetscape.bench"):
		cap._check(false, "a bench put first, for Shift to take")
		return
	var at: Vector3 = cap.spots["streetscape.bench"]
	var before: int = CityStore.query(&"street_counts").items
	await cap.ui.modifier(KEY_SHIFT, true)
	await cap.ui.click(at)
	await cap.ui.modifier(KEY_SHIFT, false)
	cap._check(CityStore.query(&"street_counts").items == before - 1, "Shift takes the piece")
	CityStore.dispatch(&"undo")


static func _furniture_set(cap: Node) -> void:
	cap.hud.open_category("roads")
	await cap.ui.frames(4)
	await cap._pick("roadFurniture.basic")
	cap._check(CityStore.query(&"road_furniture") == LEVEL_BASIC, "the basic set by the Furniture row")
	await cap.ui.shot("street_08_road_set")
	await cap._pick("roadFurniture.complete")
	cap._check(CityStore.query(&"road_furniture") == LEVEL_FULL, "the full set back")
	cap.hud.open_category("roads")
	await cap.ui.frames(2)


## Each barrier on the footway across the street, away from the signs: the points go to its back, or to the kerb.
static func barriers(cap: Node, a: Vector3, b: Vector3) -> void:
	var names := ["barrier.kind.fence", "barrier.kind.wall", "barrier.kind.hedge", "barrier.kind.guardrail", "barrier.kind.railing"]
	var dir := (b - a).normalized()
	var before: Dictionary = CityStore.query(&"street_counts")
	var built := 0
	cap._over(cap._across - dir * BARRIER_FROM_M)
	await cap.ui.drawn()
	for i in names.size():
		await cap._pick(names[i])
		var o: Vector3 = cap._across - dir * (BARRIER_FROM_M + BARRIER_GAP_M * i)
		await cap.ui.click(o)
		await cap.ui.click(o - dir * BARRIER_LEG_M)
		await cap.ui.key(KEY_ENTER, false)
		cap.spots[names[i]] = o - dir * BARRIER_LEG_M * 0.5
		var now: int = CityStore.query(&"street_counts").barriers
		print("barrier ", names[i], ": ", now - before.barriers - built)
		built = now - before.barriers
	await cap.ui.drawn()
	await cap.ui.shot("street_06_barriers")
	cap._check(built == names.size(), "barriers by mouse %d" % built)
