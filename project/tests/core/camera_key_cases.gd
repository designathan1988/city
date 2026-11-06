## The keys and the tool of camera_input_test: W and Q glide, Shift+E turns a quarter, a right click cancels the road and a right drag
## does not, the road's height keys and the keys it gives back. Everything goes through the test's own input.
class_name CameraKeyCases
extends RefCounted

const EPS := 1e-3

var t: Node3D


func _init(test: Node3D) -> void:
	t = test


func run() -> void:
	await _keys()
	await _tool()


func _keys() -> void:
	t._home()
	await t._frames(1)
	var f0: Vector3 = t.rig.focus
	var fwd := Vector3(-sin(t.rig.yaw), 0, -cos(t.rig.yaw))
	await t._hold_key(KEY_W, false, 0.5)
	t._check((t.rig.focus - f0).dot(fwd) > 0.0, "W anda para a frente (%.1f m)" % (t.rig.focus - f0).dot(fwd))
	var yaw0: float = t.rig.yaw
	await t._hold_key(KEY_Q, false, 0.3)
	t._check(angle_difference(yaw0, t.rig.yaw) > 0.0, "Q gira")
	await t._glide_settled()
	yaw0 = t.rig.yaw
	await t._hold_key(KEY_E, true, 0.05)
	await t._glide_settled()
	var quarter: float = angle_difference(yaw0, t.rig.yaw)
	t._check(absf(quarter + deg_to_rad(t.cfg.quarter_turn_deg)) < EPS, "Shift+E: um quarto de volta (%.1f graus)" % rad_to_deg(quarter))


func _tool() -> void:
	t._home()
	t.tools.set_tool(ToolController.Tool.ROAD, {"profile": 0})
	await t._frames(1)
	var c: Vector2 = t._screen() * 0.5
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
	var pitch0: float = t.rig.pitch
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
	t._check(not RoadKeys.bind(RoadKeys.HEIGHT_UP, KEY_W), "a camera key is refused")
	t._check(not RoadKeys.bind(RoadKeys.HEIGHT_UP, KEY_Z), "a tool's key is refused")
	t._check(RoadKeys.bind(RoadKeys.HEIGHT_UP, KEY_N), "a free key is taken")
	await t._hold_key(KEY_N, false, 0.05)
	t._check(is_equal_approx(CityStore.query(&"road_height"), 1.0), "the new key raises the road")
	t._check(RoadKeys.bind(RoadKeys.HEIGHT_DOWN, KEY_N), "a key of another action is taken")
	t._check(RoadKeys.key_of(RoadKeys.HEIGHT_UP) == KEY_PAGEDOWN, "and that action takes the old key")
	RoadKeys.reset()
	t._check(RoadKeys.key_of(RoadKeys.HEIGHT_UP) == KEY_PAGEUP, "the defaults come back")
	CityStore.dispatch(&"set_road_height", [0.0])
