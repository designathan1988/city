## The left button's stroke and a tool change, in camera_input_test: a tool taken while the button is held gets no stroke it
## was not pressed for (ToolController's Phase machine cancels the held stroke as the tool changes).
class_name CameraStrokeCases
extends RefCounted

const STEPS := 8
const STEP_PX := 4.0 # how far the hand moves each step of the held stroke
const HOLD_S := 0.3 # the held hand rests past three of the relief brush's dabs (terrain_tool.gd DAB_S)
const PAUSED := 0 # SimSpeed.PAUSED: no growth moves the map while the stroke is judged

var t: Node


func _init(test: Node) -> void:
	t = test


## The inspect tool pressed on the town, the relief brush taken before the button goes up, the hand moved and held past a
## brush's dabs: the brush was never pressed, so the ground stays as it was (the map's revision does not move).
func run() -> void:
	CityStore.dispatch(&"set_speed", [PAUSED])
	t.home()
	await t.frames(1)
	var c: Vector2 = t.screen() * 0.5
	t.tools.set_tool(ToolController.Tool.INSPECT)
	await t._move(c)
	await t._button(MOUSE_BUTTON_LEFT, c, true, false)
	var before := int(CityStore.query(&"map_revision"))
	t.tools.set_tool(ToolController.Tool.TERRAIN)
	for i in STEPS:
		await t._move(c + Vector2(i, 0) * STEP_PX, t._mask(MOUSE_BUTTON_LEFT))
	await t.get_tree().create_timer(HOLD_S).timeout
	await t._button(MOUSE_BUTTON_LEFT, c, false, false)
	var after := int(CityStore.query(&"map_revision"))
	t.check(after == before, "a tool taken mid-stroke shapes nothing it was not pressed for (revision %d, then %d)" % [before, after])
	t.tools.set_tool(ToolController.Tool.NONE)
