## What the road tool does with the mouse and the keys, in camera_input_test: the buttons reach it, and its keys are its own and can be changed.
class_name CameraRoadKeyCases
extends RefCounted

var t: Node


func _init(test: Node) -> void:
	t = test


func run() -> void:
	t._home()
	t.tools.set_tool(ToolController.Tool.ROAD, {"profile": 0})
	await t._frames(1)
	var c = t._screen() * 0.5
	await t._click(MOUSE_BUTTON_LEFT, c)
	var road: Variant = t.tools.active
	t._check(road.chain != Vector3.INF, "esquerdo chega à ferramenta (via começada)")
	await t._drag(MOUSE_BUTTON_RIGHT, c, c + Vector2(80, 0), false, true)
	t._check(road.chain != Vector3.INF, "arrastar com o direito não cancela a via")
	await t._click(MOUSE_BUTTON_RIGHT, c)
	t._check(road.chain == Vector3.INF, "clique direito cancela a via")
	await _height_keys()
	t.tools.set_tool(ToolController.Tool.NONE)


## With the road tool in hand Page Up and Down are the road's height, landing on whole metres, and the camera does not tilt.
func _height_keys() -> void:
	var pitch0 = t.rig.pitch
	await t._hold_key(KEY_PAGEUP, false, 0.05)
	await t._hold_key(KEY_PAGEUP, false, 0.05)
	await t._glide_settled()
	t._check(is_equal_approx(CityStore.query(&"road_height"), 2.0), "Page Up sobe a via (%.1f m)" % CityStore.query(&"road_height"))
	t._check(is_equal_approx(t.rig.pitch, pitch0), "a câmera não inclina com a via na mão")
	CityStore.dispatch(&"set_road_height", [1.5])
	t.tools.active.height = 1.5
	await t._hold_key(KEY_PAGEDOWN, false, 0.05)
	t._check(is_equal_approx(CityStore.query(&"road_height"), 1.0), "Page Down pousa no metro inteiro (%.1f m)" % CityStore.query(&"road_height"))
	CityStore.dispatch(&"set_road_height", [0.0])
	t.tools.active.height = 0.0
	await _rebound_keys()


## A key of the road's can be changed, and then it is that key that works; one the game keeps is refused; two actions swap.
func _rebound_keys() -> void:
	t._check(not RoadKeys.bind(RoadKeys.RAISE, KEY_W), "a camera key is refused")
	t._check(not RoadKeys.bind(RoadKeys.RAISE, KEY_Z), "a tool's key is refused")
	t._check(RoadKeys.bind(RoadKeys.RAISE, KEY_N), "a free key is taken")
	await t._hold_key(KEY_N, false, 0.05)
	t._check(is_equal_approx(CityStore.query(&"road_height"), 1.0), "the new key raises the road")
	t._check(RoadKeys.bind(RoadKeys.LOWER, KEY_N), "a key of another action is taken")
	t._check(RoadKeys.key_of(RoadKeys.RAISE) == KEY_PAGEDOWN, "and that action takes the old key")
	RoadKeys.reset()
	t._check(RoadKeys.key_of(RoadKeys.RAISE) == KEY_PAGEUP, "the defaults come back")
	CityStore.dispatch(&"set_road_height", [0.0])
