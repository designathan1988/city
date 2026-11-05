## Build > Models as the player drives it: every model of the table is a valid body, the same bytes each time; a body in hand goes to the
## road's edge, is refused on a road, is set down in one undo step and welds with what it lands on; the player's own models keep; a
## picture has something in it.
## Run: godot --path project res://tests/core/model_test.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const SNAP_ROAD := 2
const HOUSE := "house"
const SHOP := "shop"
const REACH_M := 12.0
const WAIT_FRAMES := 240
const SEARCH_STEP_M := 36.0

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
	_catalogue()
	var road: Vector3 = _a_road()
	_snap_and_refuse(road)
	_place_undo_redo(tools.active, road)
	_weld(road)
	_library()
	await _picture(tools)
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


func _catalogue() -> void:
	var list: Array = CityStore.query(&"model_list")
	_check(list.size() == 53, "o catálogo tem 42 modelos da cidade e 11 genéricos: %d" % list.size())
	for m: Dictionary in list:
		var a: PackedByteArray = CityStore.query(&"model_bytes", [m.id])
		_check(not a.is_empty() and a == CityStore.query(&"model_bytes", [m.id]), "%s: mesmos bytes nas duas montagens" % m.id)
		var size: Vector3 = CityStore.query(&"body_size", [a])
		_check(size.x > 0.0 and size.y > 0.0, "%s: tem lote %s" % [m.id, size])
		_check(TranslationServer.translate("model.%s" % m.id) != "model.%s" % m.id, "%s: tem nome" % m.id)
		var problem: String = CityStore.query(&"place_check", [a, 0.0, 0.0, 0.0])
		_check(not problem in ["building.problem.size", "building.problem.footprint", "building.problem.support"], "%s: corpo válido: %s" % [m.id, problem])


## A point by a road with nothing built where a house would stand: tried by setting one down, and taken back.
func _a_road() -> Vector3:
	var bytes: PackedByteArray = CityStore.query(&"model_bytes", [HOUSE])
	var size: Vector3 = CityStore.query(&"body_size", [bytes])
	var bounds: Dictionary = CityStore.query(&"city_bounds")
	var x: float = bounds.min.x
	while x < bounds.max.x:
		var z: float = bounds.min.z
		while z < bounds.max.z:
			var at := Vector3(x, 0, z)
			var fit: Dictionary = CityStore.query(&"place_fit", [at, Vector2(size.x, size.y), 0.0])
			if fit.kind == SNAP_ROAD and CityStore.query(&"place_check", [bytes, fit.x, fit.z, fit.rot]) == "":
				var r: Dictionary = CityStore.dispatch(&"place_model", [bytes, fit.x, fit.z, fit.rot, 0, 1])
				CityStore.dispatch(&"undo")
				if r.get("ok", false) and r.buildings_removed == 0:
					return at
			z += SEARCH_STEP_M
		x += SEARCH_STEP_M
	_check(false, "achou uma rua com chão livre")
	return Vector3.ZERO


func _snap_and_refuse(road: Vector3) -> void:
	var bytes: PackedByteArray = CityStore.query(&"model_bytes", [HOUSE])
	var size: Vector3 = CityStore.query(&"body_size", [bytes])
	var fit: Dictionary = CityStore.query(&"place_fit", [road, Vector2(size.x, size.y), 0.0])
	_check(fit.kind == SNAP_ROAD, "cola na rua")
	var into := Vector3(-sin(fit.rot), 0, -cos(fit.rot))
	var over_road := Vector3(fit.x, 0, fit.z) - into * (size.y + REACH_M)
	var problem: String = CityStore.query(&"place_check", [bytes, over_road.x, over_road.z, fit.rot])
	_check(problem == "building.problem.road", "sobre a rua é recusado: %s" % problem)
	var far: String = CityStore.query(&"place_check", [bytes, 1.0e6, 0.0, 0.0])
	_check(far == "building.problem.bounds", "fora do mundo é recusado: %s" % far)


func _place_undo_redo(tool: BuildingTool, road: Vector3) -> void:
	var before: int = CityStore.query(&"list_buildings").size()
	tool.take({"bytes": CityStore.query(&"model_bytes", [HOUSE]), "function": 0, "zone": 1})
	tool.key(KEY_R)
	tool.hover(road)
	tool.click(road)
	_check(tool.hand.is_empty(), "a mão fica vazia depois de colocar")
	_check(not tool.picked.is_empty(), "o prédio colocado fica escolhido")
	var id: int = tool.picked.get("id", -1)
	var info: Dictionary = CityStore.query(&"body_info", [id])
	_check(info.get("edited", false) and info.volumes.size() == 1 and info.volumes[0].storeys == 2, "a casa tem um bloco de dois andares: %s" % info)
	var digest: int = info.digest
	var grown: int = CityStore.query(&"list_buildings").size()
	CityStore.dispatch(&"undo")
	_check(CityStore.query(&"list_buildings").size() < grown, "desfazer tira o prédio")
	CityStore.dispatch(&"redo")
	_check(CityStore.query(&"list_buildings").size() == grown and CityStore.query(&"body_info", [id]).digest == digest, "refazer devolve os mesmos bytes")
	_check(grown >= before, "contagem coerente: %d depois de %d" % [grown, before])
	tool.picked = {}


func _weld(road: Vector3) -> void:
	var house: PackedByteArray = CityStore.query(&"model_bytes", [HOUSE])
	var shop: PackedByteArray = CityStore.query(&"model_bytes", [SHOP])
	var size: Vector3 = CityStore.query(&"body_size", [shop])
	var fit: Dictionary = CityStore.query(&"place_fit", [road, Vector2(size.x, size.y), 0.0])
	var r: Dictionary = CityStore.dispatch(&"place_model", [shop, fit.x, fit.z, fit.rot, 0, 2])
	_check(r.get("ok", false) and r.buildings_removed >= 1, "a loja cai sobre a casa e funde: %s" % r)
	var info: Dictionary = CityStore.query(&"body_info", [r.get("placed", -1)])
	_check(info.volumes.size() >= 2, "o prédio fundido tem os blocos dos dois: %d" % info.volumes.size())
	CityStore.dispatch(&"undo")
	var again: Dictionary = CityStore.query(&"body_info", [r.get("placed", -1)])
	_check(again.is_empty() or again.get("digest", 0) != info.digest, "desfazer separa os dois")
	_check(house.size() > 0, "a casa segue no catálogo")


func _library() -> void:
	var id: int = CityStore.query(&"list_buildings")[0]
	var entry: Dictionary = CityStore.query(&"building_bytes", [id])
	_check(ModelLibrary.save("Minha casa", entry), "salva o modelo")
	_check(ModelLibrary.save("Minha casa", entry), "salva outro de mesmo nome")
	var list := ModelLibrary.list()
	_check(list.size() >= 2, "os dois estão na lista: %d" % list.size())
	var got: Vector3 = CityStore.query(&"body_size", [list[0].bytes])
	_check(got.x > 0.0, "o arquivo guarda o corpo: %s" % got)
	for m in list:
		_check(ModelLibrary.remove(m.key), "remove %s" % m.key)
	_check(ModelLibrary.list().is_empty(), "a lista fica vazia")


func _picture(tools: ToolController) -> void:
	var seen: int = CityStore.query(&"stage_show", [CityStore.query(&"model_bytes", [HOUSE]), get_viewport().find_world_3d().scenario, false])
	var triangles: int = CityStore.query(&"stage_triangles", [seen])
	_check(triangles > 0, "a malha do palco tem triângulos: %d" % triangles)
	print("palco: ", triangles, " triângulos")
	CityStore.query(&"stage_free", [seen])
	var studio := ModelStudio.new()
	tools.add_child(studio)
	var got := {}
	studio.pictured.connect(func(key: String, picture: Texture2D) -> void: got[key] = picture)
	studio.ask(HOUSE, CityStore.query(&"model_bytes", [HOUSE]))
	for i in WAIT_FRAMES:
		if got.has(HOUSE):
			break
		await get_tree().process_frame
	_check(got.has(HOUSE), "a foto chegou")
	if got.has(HOUSE):
		var image: Image = got[HOUSE].get_image()
		image.save_png("user://model_test_house.png")
		_check(not image.is_invisible(), "a foto tem o prédio: %s %s usado %s meio %s" % [image.get_size(), image.get_format(), image.get_used_rect(), image.get_pixel(image.get_width() / 2, image.get_height() / 2)])


func _check(ok: bool, what: String) -> void:
	if ok:
		return
	failures += 1
	printerr("FALHOU: ", what)
