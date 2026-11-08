## The Build tool as the player drives it: pick a block by a ray, press the strip's tools from their data, undo. Every tool of
## data/ui/builder.json must do something or be refused for a reason the player can read.
## Run: godot --path project res://tests/core/building_tool_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const LOOK_FROM_M := 80.0
const BAY_BALCONY := 7
const SCOPE_ROW := 1
const WING_M := 6.0
const MOVE_M := 4.0


func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var tools := ToolController.new(rig)
	add_child(tools)
	add_child(InteriorController.new())
	await get_tree().process_frame
	tools.set_tool(ToolController.Tool.BUILD)
	var tool: BuildingTool = tools.active
	var id := _tall_building()
	var aim := _roof_ray(id)
	tool.pick(aim[0], aim[1])
	check(tool.picked.get("id", -1) == id, "the ray picks building %d: %s" % [id, tool.picked])
	check(_outline(tool).size() >= 8, "the picked block has an outline")
	_inside(tool, id)
	_every_tool(tool, id)
	_arm_and_click(tool, id)
	_drag_a_wing(tool, id)
	_moved_by_its_corner(tool, id)
	tool.cancel()
	tool.cancel()
	check(tool.picked.is_empty(), "Esc lets the selection go")
	finish()


## Construir > Interior from its cards in the data: the building chosen opens, goes up a floor, and closes.
func _inside(tool: BuildingTool, id: int) -> void:
	var cards := {}
	for s: Dictionary in DataTables.ui("builder").tabs.interior.sections:
		for c: Dictionary in s.tools:
			cards[c.id] = c
	tool.act(cards.interiorView)
	var open: Dictionary = CityStore.query(&"interior")
	check(open.on and open.target == id, "Cut and see opens the picked building: %s" % open)
	tool.act(cards.floorUp)
	check(int(CityStore.query(&"interior").level) == 1, "Floor up goes up a floor")
	tool.act(cards.floorDown)
	check(int(CityStore.query(&"interior").level) == 0, "Floor down goes down")
	tool.act(cards.interiorView)
	check(not CityStore.query(&"interior").on, "Cut and see again closes")


func _tall_building() -> int:
	for id: int in CityStore.query(&"list_buildings"):
		var v: Array = CityStore.query(&"body_info", [id]).volumes
		if v.size() == 1 and v[0].storeys >= 3:
			return id
	check(false, "found a building of one block")
	return -1


## Straight down over the block's middle, in the frame of b40::lot_frame.
func _roof_ray(id: int) -> Array:
	return [BuildingProbe.block_middle(id) + Vector3.UP * LOOK_FROM_M, Vector3.DOWN]


func _outline(tool: BuildingTool) -> PackedVector3Array:
	return CityStore.query(&"body_outline", [tool.picked.id, tool.picked.volume])


## Each edit of the tables changes the body or is refused with a text, and undo takes it back.
func _every_tool(tool: BuildingTool, id: int) -> void:
	var tabs: Dictionary = DataTables.ui("builder").tabs
	var count := 0
	for tab: String in tabs:
		for section: Dictionary in tabs[tab].sections:
			for spec: Dictionary in section.get("tools", []):
				if spec.do == "ref_load": # the operating system's file dialog: nobody here to answer it
					continue
				if BuilderStrip.CHOICES.has(spec.do):
					tool.act(spec)
					continue
				count += 1
				_one_tool(tool, id, tab, spec)
	check(count > 0, "%d editing tools exercised" % count)


func _one_tool(tool: BuildingTool, id: int, tab: String, spec: Dictionary) -> void:
	var keep: Dictionary = tool.picked
	var before: int = CityStore.query(&"body_info", [id]).digest
	var r: Dictionary = BodyEdits.run(spec, spec.get("value", 0), tool.picked, tool.paint_scope)
	var after: int = CityStore.query(&"body_info", [id]).digest
	tool.picked = keep
	if r.get("ok", false) and after != before:
		check(CityStore.dispatch(&"undo") and CityStore.query(&"body_info", [id]).digest == before, "%s/%s undone" % [tab, spec.id])
	else:
		var why: String = r.get("error", "")
		var said := why != "" and tr(why) != why
		check(r.get("ok", false) or said, "%s/%s answers: %s" % [tab, spec.id, r])


## Massa › Ala as the player does it: the card, a press beside the block, a drag and a release make a block; pressing the card again ends it.
func _drag_a_wing(tool: BuildingTool, id: int) -> void:
	var card := {"id": "wing", "do": "mass", "value": 0}
	var before: int = CityStore.query(&"body_info", [id]).volumes.size()
	var outline := _outline(tool)
	var corner: Vector3 = outline[0]
	var along: Vector3 = (outline[1] - corner).normalized()
	var out: Vector3 = (corner - outline[3]).normalized()
	tool.act(card)
	check(tool.in_progress(), "the card arms the drag")
	tool.click(corner)
	tool.drag(corner + along * WING_M + out * WING_M)
	tool.release(corner + along * WING_M + out * WING_M)
	check(CityStore.query(&"body_info", [id]).volumes.size() == before + 1, "the drag made a wing")
	CityStore.dispatch(&"undo")
	tool.act(card)
	check(not tool.in_progress(), "the card again disarms")


## The Move handle's edit: the building goes, one step of undo brings it back.
func _moved_by_its_corner(tool: BuildingTool, id: int) -> void:
	var d: Dictionary = CityStore.query(&"body_draft", [id, {}])
	var r: Dictionary = CityStore.dispatch(&"building_place", [id, d.x + MOVE_M, d.z + MOVE_M, d.rot])
	if not r.get("ok", false):
		var why: String = r.get("error", "")
		check(why != "" and tr(why) != why, "moving answers: %s" % r)
		return
	check(CityStore.dispatch(&"undo") and not CityStore.query(&"body_info", [id]).is_empty(), "moving undone")


func _arm_and_click(tool: BuildingTool, id: int) -> void:
	var v: Dictionary = CityStore.query(&"body_info", [id]).volumes[0]
	var aim := _roof_ray(id)
	var p: Dictionary = CityStore.query(&"probe_building", [id])
	var front: Vector3 = aim[0] - Vector3.UP * (LOOK_FROM_M - 1.0)
	var out: Vector3 = Vector3(sin(p.rot), 0, cos(p.rot)) * (v.d * 0.5 + 1.0)
	tool.act({"do": "scope", "value": SCOPE_ROW})
	tool.act({"do": "arm", "value": BAY_BALCONY})
	tool.pick(front + out + Vector3.UP * CityStore.query(&"body_info", [id]).ground_h, -out)
	check(tool.picked.get("id", -1) == id and tool.picked.side >= 0, "the click hits the wall: %s" % tool.picked)
	var h: Dictionary = tool.picked
	check(CityStore.query(&"body_bay", [id, h.volume, h.storey, h.side, h.bay]) == BAY_BALCONY, "the clicked bay became a balcony")
	CityStore.dispatch(&"undo")
