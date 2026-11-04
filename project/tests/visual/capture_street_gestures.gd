## The gestures of the Street tab, by the mouse (capture_street.gd, `cap`): a row dragged along a footway, a piece taken away
## with Shift, and the set of furniture a new road is laid with, picked on the Roads drawer.
class_name CaptureStreetGestures
extends RefCounted

const ROW_M := 65.0
const ROW_BACK_M := 34.0
const LEVEL_BASIC := 1
const LEVEL_FULL := 0


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
	cap._check(laid >= 2, "fileira arrastada: %d peças" % laid)
	CityStore.dispatch(&"undo")
	cap._check(CityStore.query(&"street_counts").items == before, "a fileira sai num desfazer só")


static func _shift_click(cap: Node) -> void:
	var at: Vector3 = cap.spots["streetscape.bench"]
	var before: int = CityStore.query(&"street_counts").items
	await cap.ui.modifier(KEY_SHIFT, true)
	await cap.ui.click(at)
	await cap.ui.modifier(KEY_SHIFT, false)
	cap._check(CityStore.query(&"street_counts").items == before - 1, "Shift tira a peça")
	CityStore.dispatch(&"undo")


static func _furniture_set(cap: Node) -> void:
	cap.hud.open_category("roads")
	await cap.ui.frames(4)
	await cap._pick("roadFurniture.basic")
	cap._check(CityStore.query(&"road_furniture") == LEVEL_BASIC, "conjunto básico pelo cartão")
	await cap.ui.shot("rua_08_conjunto_via")
	await cap._pick("roadFurniture.complete")
	cap._check(CityStore.query(&"road_furniture") == LEVEL_FULL, "conjunto completo de volta")
	cap.hud.open_category("roads")
	await cap.ui.frames(2)
