## The Build tool as the player drives it: pick a block by a ray, press the strip's tools from their data, undo. Every tool of
## data/ui/builder.json must do something or be refused for a reason the player can read.
## Run: godot --path project res://tests/core/building_tool_test.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const LOOK_FROM_M := 80.0
const BAY_BALCONY := 7
const SCOPE_ROW := 1

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var rig := CityCamera.new()
	add_child(rig)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	var tools := ToolController.new(rig)
	add_child(tools)
	await get_tree().process_frame
	tools.set_tool(ToolController.Tool.BUILD)
	var tool: BuildingTool = tools.active
	var id := _tall_building()
	var aim := _roof_ray(id)
	tool.pick(aim[0], aim[1])
	_check(tool.picked.get("id", -1) == id, "o raio escolhe o prédio %d: %s" % [id, tool.picked])
	_check(_outline(tool).size() >= 8, "o bloco escolhido tem contorno")
	_every_tool(tool, id)
	_arm_and_click(tool, id)
	tool.cancel()
	tool.cancel()
	_check(tool.picked.is_empty(), "Esc solta a seleção")
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


func _tall_building() -> int:
	for id: int in CityStore.query(&"list_buildings"):
		var v: Array = CityStore.query(&"body_info", [id]).volumes
		if v.size() == 1 and v[0].storeys >= 3:
			return id
	_check(false, "achou um prédio de um bloco só")
	return -1


## Straight down over the block's middle, in the frame of b40::lot_frame.
func _roof_ray(id: int) -> Array:
	var p: Dictionary = CityStore.query(&"probe_building", [id])
	var v: Dictionary = CityStore.query(&"body_info", [id]).volumes[0]
	var s: float = sin(p.rot)
	var c: float = cos(p.rot)
	var x: float = v.x + v.w * 0.5
	var y: float = v.y + v.d * 0.5
	var middle: Vector3 = p.position + Vector3(s, 0, c) * (p.d * 0.5) + Vector3(c, 0, -s) * x + Vector3(-s, 0, -c) * y
	return [middle + Vector3.UP * LOOK_FROM_M, Vector3.DOWN]


func _outline(tool: BuildingTool) -> PackedVector3Array:
	return CityStore.query(&"body_outline", [tool.picked.id, tool.picked.volume])


## Each edit of the tables changes the body or is refused with a text, and undo takes it back.
func _every_tool(tool: BuildingTool, id: int) -> void:
	var tabs: Dictionary = DataTables.ui("builder").tabs
	var count := 0
	for tab: String in tabs:
		for section: Dictionary in tabs[tab].sections:
			for spec: Dictionary in section.get("tools", []):
				if BuilderStrip.CHOICES.has(spec.do):
					tool.act(spec)
					continue
				count += 1
				_one_tool(tool, id, tab, spec)
	_check(count > 0, "%d ferramentas de edição exercidas" % count)


func _one_tool(tool: BuildingTool, id: int, tab: String, spec: Dictionary) -> void:
	var keep: Dictionary = tool.picked
	var before: int = CityStore.query(&"body_info", [id]).digest
	var r: Dictionary = tool._edit(spec, spec.get("value", 0))
	var after: int = CityStore.query(&"body_info", [id]).digest
	tool.picked = keep
	if r.get("ok", false) and after != before:
		_check(CityStore.dispatch(&"undo") and CityStore.query(&"body_info", [id]).digest == before, "%s/%s desfeito" % [tab, spec.id])
	else:
		var why: String = r.get("error", "")
		var said := why != "" and tr(why) != why
		_check(r.get("ok", false) or said, "%s/%s responde: %s" % [tab, spec.id, r])


func _arm_and_click(tool: BuildingTool, id: int) -> void:
	var v: Dictionary = CityStore.query(&"body_info", [id]).volumes[0]
	var aim := _roof_ray(id)
	var p: Dictionary = CityStore.query(&"probe_building", [id])
	var front: Vector3 = aim[0] - Vector3.UP * (LOOK_FROM_M - 1.0)
	var out: Vector3 = Vector3(sin(p.rot), 0, cos(p.rot)) * (v.d * 0.5 + 1.0)
	tool.act({"do": "scope", "value": SCOPE_ROW})
	tool.act({"do": "arm", "value": BAY_BALCONY})
	tool.pick(front + out + Vector3.UP * CityStore.query(&"body_info", [id]).ground_h, -out)
	_check(tool.picked.get("id", -1) == id and tool.picked.side >= 0, "o clique acerta a parede: %s" % tool.picked)
	var h: Dictionary = tool.picked
	_check(CityStore.query(&"body_bay", [id, h.volume, h.storey, h.side, h.bay]) == BAY_BALCONY, "o vão clicado virou varanda")
	CityStore.dispatch(&"undo")


func _check(ok: bool, what: String) -> void:
	print("ok   " if ok else "FAIL ", what)
	if not ok:
		failures += 1
