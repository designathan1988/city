## What the road tool does with the mouse and the keys, in camera_input_test: the buttons reach it, and its keys are its own and can be changed.
class_name CameraRoadKeyCases
extends RefCounted

const ZONE_ROAD_X := 700.0
const ZONE_ROAD_Z := 620.0
const ZONE_ROAD_M := 300.0
const ZONE_BRUSH_M := 30.0

var t: Node


func _init(test: Node) -> void:
	t = test


func run() -> void:
	t.home()
	t.tools.set_tool(ToolController.Tool.ROAD, {"profile": 0})
	await t.frames(1)
	var c = t.screen() * 0.5
	await t.click(MOUSE_BUTTON_LEFT, c)
	var road: Variant = t.tools.active
	t.check(road.chain != Vector3.INF, "left reaches the tool (road started)")
	await t.drag(MOUSE_BUTTON_RIGHT, c, c + Vector2(80, 0), false, true)
	t.check(road.chain != Vector3.INF, "dragging with the right does not cancel the road")
	await t.click(MOUSE_BUTTON_RIGHT, c)
	t.check(road.chain == Vector3.INF, "a right click cancels the road")
	await _height_keys()
	_blocks()
	_zone_brush()
	t.tools.set_tool(ToolController.Tool.NONE)


## Streets by the block from the road tool: the preview, and the grid laid where it was shown.
func _blocks() -> void:
	var at := Vector3(ZONE_ROAD_X, 0, -ZONE_ROAD_Z)
	var had: int = CityStore.query(&"inspect", [at]).size()
	RoadBlocks.show(t.tools, at)
	RoadBlocks.lay(t.tools.active, at)
	CityStore.dispatch(&"undo")
	t.check(CityStore.query(&"inspect", [at]).size() == had, "blocks by the tool: undone in one step")


## A fast drag reports two points far apart: the whole way between them is zoned, and with Shift held the same stroke takes the zone away.
func _zone_brush() -> void:
	var a := Vector3(ZONE_ROAD_X, 0, ZONE_ROAD_Z)
	var b := a + Vector3(ZONE_ROAD_M, 0, 0)
	var mid := a.lerp(b, 0.5)
	var built: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, 0, 0.25])
	t.check(built.get("ok", false), "a road for the zone brush: %s" % [built])
	t.tools.set_tool(ToolController.Tool.ZONE, {"zone": DataTables.zone_code("residential"), "radius": ZONE_BRUSH_M})
	var brush: Variant = t.tools.active
	brush.click(a)
	brush.drag(b)
	brush.release(b)
	var again: Dictionary = CityStore.dispatch(&"paint_zone", [mid, ZONE_BRUSH_M, DataTables.zone_code("residential")])
	t.check(again.get("error", "") == "tools.toast.zoneSame", "a fast drag zones the middle of the way: %s" % [again])
	var shift := InputEventKey.new()
	shift.keycode = KEY_SHIFT
	shift.pressed = true
	Input.parse_input_event(shift)
	Input.flush_buffered_events()
	brush.click(a)
	brush.drag(b)
	brush.release(b)
	shift = shift.duplicate()
	shift.pressed = false
	Input.parse_input_event(shift)
	Input.flush_buffered_events()
	var none: Dictionary = CityStore.dispatch(&"paint_zone", [mid, ZONE_BRUSH_M, 0])
	t.check(none.get("error", "") == "tools.toast.zoneSame", "with Shift the stroke takes the zone off: %s" % [none])


## With the road tool in hand Page Up and Down are the road's height, landing on whole metres, and the camera does not tilt.
func _height_keys() -> void:
	var pitch0 = t.rig.pitch
	await t.hold_key(KEY_PAGEUP, false, 0.05)
	await t.hold_key(KEY_PAGEUP, false, 0.05)
	await t.glide_settled()
	t.check(is_equal_approx(CityStore.query(&"road_height"), 2.0), "Page Up raises the road (%.1f m)" % CityStore.query(&"road_height"))
	t.check(is_equal_approx(t.rig.pitch, pitch0), "the camera does not tilt with a road in hand")
	CityStore.dispatch(&"set_road_height", [1.5])
	t.tools.active.height = 1.5
	await t.hold_key(KEY_PAGEDOWN, false, 0.05)
	t.check(is_equal_approx(CityStore.query(&"road_height"), 1.0), "Page Down lands on the whole metre (%.1f m)" % CityStore.query(&"road_height"))
	CityStore.dispatch(&"set_road_height", [0.0])
	t.tools.active.height = 0.0
	await _rebound_keys()


## A key of the road's can be changed, and then it is that key that works; one the game keeps is refused; two actions swap.
func _rebound_keys() -> void:
	t.check(not RoadKeys.bind(RoadKeys.RAISE, KEY_W), "a camera key is refused")
	t.check(not RoadKeys.bind(RoadKeys.RAISE, KEY_Z), "a tool's key is refused")
	t.check(RoadKeys.bind(RoadKeys.RAISE, KEY_N), "a free key is taken")
	await t.hold_key(KEY_N, false, 0.05)
	t.check(is_equal_approx(CityStore.query(&"road_height"), 1.0), "the new key raises the road")
	t.check(RoadKeys.bind(RoadKeys.LOWER, KEY_N), "a key of another action is taken")
	t.check(RoadKeys.key_of(RoadKeys.RAISE) == KEY_PAGEDOWN, "and that action takes the old key")
	RoadKeys.reset()
	t.check(RoadKeys.key_of(RoadKeys.RAISE) == KEY_PAGEUP, "the defaults come back")
	CityStore.dispatch(&"set_road_height", [0.0])
