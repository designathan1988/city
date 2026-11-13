## The road tool in use, for capture_roads: the drawer, a chain, a curve, a stroke by hand, and the Control and Move tabs.
class_name CaptureRoadTools
extends RefCounted

var t: Node


func _init(capture: Node) -> void:
	t = capture


func run() -> void:
	t.rig.jump(t.at(40, -320), 0.0, 70.0, t.WIDE_M)
	await t.ui.drawn()
	t.hud.open_category("roads")
	await t.ui.frames(4)
	await t.ui.shot("tool_drawer")
	await t.ui.click(t.at(-120, -380))
	await t.ui.hover(t.at(120, -380))
	await t.ui.shot("tool_chain")
	t.tools.active.cancel()
	var street: int = t.profiles["urbanStreet"]
	t.tools.set_tool(ToolController.Tool.ROAD, {"profile": street, "alignment": 1, "curve": true})
	await t.ui.click(t.at(-120, -300))
	await t.ui.click(t.at(120, -300))
	await t.ui.hover(t.at(0, -420))
	await t.ui.shot("tool_curve_bend")
	t.tools.active.cancel()
	if StoreApi.COMMANDS.has(&"build_road_path"):
		await _freehand(street)
	if ToolController.Tool.has("MOVE"):
		await _node_tools()
	t.hud.drawer.close()


## The Control and Move tabs on the urban crossing.
func _node_tools() -> void:
	var junction = t.at(0, -120)
	t.rig.jump(junction, 0.0, 70.0, t.NODE_VIEW_M)
	await t.ui.drawn()
	t.tools.set_tool(ToolController.Tool.CONTROL, {})
	await t.ui.hover(junction)
	await t.ui.shot("tool_control")
	await t.ui.click(junction)
	await t.ui.hover(t.at(60, -60))
	await t.ui.shot("tool_control_signal")
	t.tools.set_tool(ToolController.Tool.MOVE, {})
	await t.ui.hover(junction)
	await t.ui.button(t.rig.camera.unproject_position(junction), true)
	for i in range(1, t.DRAG_STEPS + 1):
		await t.ui.hover(junction.lerp(t.at(30, -100), float(i) / t.DRAG_STEPS))
	await t.ui.shot("tool_move")
	await t.ui.button(t.rig.camera.unproject_position(t.at(30, -100)), false)
	await t.ui.drawn()
	await t.ui.shot("tool_moved")


func _freehand(street: int) -> void:
	t.tools.set_tool(ToolController.Tool.ROAD, {"profile": street, "alignment": 2})
	var s0: int = CityStore.snapshot.segments
	var start = t.at(-180, -470)
	await t.ui.hover(start)
	await t.ui.button(t.rig.camera.unproject_position(start), true)
	var end = start
	for i in range(1, t.STROKE_STEPS + 1):
		end = t.at(-180 + i * 9.0, -470 + t.STROKE_SWING_M * sin(i * 0.3))
		await t.ui.hover(end)
	await t.ui.shot("tool_free_stroke")
	await t.ui.button(t.rig.camera.unproject_position(end), false)
	await t.ui.drawn()
	t._check(CityStore.snapshot.segments > s0, "freehand stroke laid (%d -> %d)" % [s0, CityStore.snapshot.segments])
	await t.ui.shot("tool_free_laid")
