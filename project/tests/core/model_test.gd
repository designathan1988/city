## Build > Models as the player drives it: every model of the table is a valid body, the same bytes each time; a body in hand goes to the
## road's edge, is refused on a road, is set down in one undo step and welds with what it lands on; the player's own models keep; a
## picture has something in it.
## Run: godot --path project res://tests/core/model_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const SNAP_ROAD := 2
const HOUSE := "house"
const SHOP := "shop"
const REACH_M := 12.0
const WAIT_FRAMES := 240
const SEARCH_STEP_M := 36.0

const LOT_TRIES := 12
const LOT_TRY_M := 10.0
const OPEN_OFF_M := 300.0 # past a lot of the lot lab, the church 125 m deep: it stands clear of the ring road
const OPEN_ROAD_M := 200.0
const OPEN_REACH_M := 40.0
const OPEN_PX := 0.25
const CHURCH_VIEW_M := 150.0 # the camera over the church, near enough for its chunk to be built
const CHURCH_YAW := 30.0
const CHURCH_PITCH := 35.0
const DRAWN_LIMIT_S := 50.0 # real time the view may take to be built (StateWait)


func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var tools := ToolController.new(rig)
	add_child(tools)
	await get_tree().process_frame
	tools.set_tool(ToolController.Tool.BUILD)
	_catalogue()
	var road: Vector3 = _a_road()
	_snap_and_refuse(road)
	await _lots(road, rig)
	_place_undo_redo(tools.active, road)
	_weld(road)
	_library()
	await _picture(tools)
	finish()


func _catalogue() -> void:
	var list: Array = CityStore.query(&"model_list")
	var table: Array = DataTables.style_of(DataTables.RENDER_TABLES.models, DataTables.REGIONAL_STYLE).models
	var listed: Array = list.map(func(m: Dictionary) -> String: return m.id)
	var written: Array = table.map(func(m: Dictionary) -> String: return m.id)
	listed.sort()
	written.sort()
	check(not written.is_empty() and listed == written, "the catalogue lists every model of models.json and no other: %d of %d" % [listed.size(), written.size()])
	for m: Dictionary in list:
		var a: PackedByteArray = CityStore.query(&"model_bytes", [m.id])
		check(not a.is_empty() and a == CityStore.query(&"model_bytes", [m.id]), "%s: the same bytes in both builds" % m.id)
		var size: Vector3 = CityStore.query(&"body_size", [a])
		check(size.x > 0.0 and size.y > 0.0, "%s: has lot %s" % [m.id, size])
		check(TranslationServer.translate("model.%s" % m.id) != "model.%s" % m.id, "%s: has a name" % m.id)
		var problem: String = CityStore.query(&"place_check", [a, 0.0, 0.0, 0.0])
		check(not problem in ["building.problem.size", "building.problem.footprint", "building.problem.support"], "%s: a valid body: %s" % [m.id, problem])


## The lots of the lot lab: all made into bodies, and one set down: the church, which stands in its own design once its chunk is drawn
## (signature_bodies.cpp: the design asked of a job, made ready and placed).
func _lots(at: Vector3, rig: CityCamera) -> void:
	var lots := LotProjects.entries()
	var files := DataTables.lot_projects().size()
	check(files > 0 and lots.size() == files, "every lot project becomes a body: %d of %d files" % [lots.size(), files])
	for e: Dictionary in lots:
		check(not (e.bytes as PackedByteArray).is_empty() and e.label != "lot." + e.key.trim_prefix("lot:"), "%s has a body and a name" % e.key)
	var church: Dictionary = lots.filter(func(e: Dictionary) -> bool: return e.key == "lot:church")[0]
	var size: Vector3 = CityStore.query(&"body_size", [church.bytes])
	var fit: Dictionary = {}
	var where := at
	for k in LOT_TRIES: # the church is wider than a house: along the street till it stands clear of the other end
		fit = CityStore.query(&"place_fit", [where, Vector2(size.x, size.y), 0.0])
		if CityStore.query(&"place_check", [church.bytes, fit.x, fit.z, fit.rot]) == "":
			break
		where += Vector3(0, 0, LOT_TRY_M)
	var before: int = CityStore.query(&"list_buildings").size()
	var r: Dictionary = CityStore.dispatch(&"place_model", [church.bytes, fit.x, fit.z, fit.rot, church.function, church.zone])
	print("church: ", r.get("error", "ok"), " size ", size)
	check(r.get("ok", false) and CityStore.query(&"list_buildings").size() > before, "the church is placed on the street: %s" % [r])
	check(CityStore.query(&"body_info", [r.get("placed", -1)]).get("signature", 0) > 0, "the church keeps its own design")
	rig.jump(Vector3(fit.x, 0, fit.z), CHURCH_YAW, CHURCH_PITCH, CHURCH_VIEW_M)
	var drawn: bool = await StateWait.drawn(get_tree(), DRAWN_LIMIT_S)
	var standing: int = CityStore.query(&"building_cost").signatures
	check(drawn and standing > 0, "the church stands in its own design: %d instances of a design (the view drawn: %s)" % [standing, drawn])
	CityStore.dispatch(&"undo")


## A point by a road with nothing built where a house would stand: tried by setting one down, and taken back.
func _a_road() -> Vector3:
	var bytes: PackedByteArray = CityStore.query(&"model_bytes", [HOUSE])
	var size: Vector3 = CityStore.query(&"body_size", [bytes])
	# every street of the town has its lots built, as in the 4.0: a new one out on the open land has room
	var edge: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(edge.max.x + OPEN_OFF_M, 0, 0)
	var laid: Dictionary = CityStore.dispatch(&"build_road", [a, a + Vector3(0, 0, OPEN_ROAD_M), false, Vector3.ZERO, CityStore.first_road(), OPEN_PX])
	check(laid.get("ok", false), "a road in the open: %s" % [laid])
	var bounds := {"min": a - Vector3(OPEN_REACH_M, 0, 0), "max": a + Vector3(OPEN_REACH_M, 0, OPEN_ROAD_M)}
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
	check(false, "found a street with free ground")
	return Vector3.ZERO


func _snap_and_refuse(road: Vector3) -> void:
	var bytes: PackedByteArray = CityStore.query(&"model_bytes", [HOUSE])
	var size: Vector3 = CityStore.query(&"body_size", [bytes])
	var fit: Dictionary = CityStore.query(&"place_fit", [road, Vector2(size.x, size.y), 0.0])
	check(fit.kind == SNAP_ROAD, "it snaps to the street")
	var into := Vector3(-sin(fit.rot), 0, -cos(fit.rot))
	var over_road := Vector3(fit.x, 0, fit.z) - into * (size.y + REACH_M)
	var problem: String = CityStore.query(&"place_check", [bytes, over_road.x, over_road.z, fit.rot])
	check(problem == "building.problem.road", "on the street is refused: %s" % problem)
	var far: String = CityStore.query(&"place_check", [bytes, 1.0e6, 0.0, 0.0])
	check(far == "building.problem.bounds", "off the world is refused: %s" % far)


func _place_undo_redo(tool: BuildingTool, road: Vector3) -> void:
	var before: int = CityStore.query(&"list_buildings").size()
	tool.take({"bytes": CityStore.query(&"model_bytes", [HOUSE]), "function": 0, "zone": 1})
	tool.key(KEY_R)
	tool.hover(road)
	tool.click(road)
	check(tool.hand.is_empty(), "the hand is empty after placing")
	check(not tool.picked.is_empty(), "the placed building stays picked")
	var id: int = tool.picked.get("id", -1)
	var info: Dictionary = CityStore.query(&"body_info", [id])
	check(info.get("edited", false) and info.volumes.size() == 1 and info.volumes[0].storeys == 2, "the house has a block of two storeys: %s" % info)
	var digest: int = info.digest
	var grown: int = CityStore.query(&"list_buildings").size()
	CityStore.dispatch(&"undo")
	check(CityStore.query(&"list_buildings").size() < grown, "undo takes the building away")
	CityStore.dispatch(&"redo")
	check(CityStore.query(&"list_buildings").size() == grown and CityStore.query(&"body_info", [id]).digest == digest, "redo gives the same bytes back")
	check(grown >= before, "a coherent count: %d after %d" % [grown, before])
	tool.picked = {}


func _weld(road: Vector3) -> void:
	var house: PackedByteArray = CityStore.query(&"model_bytes", [HOUSE])
	var shop: PackedByteArray = CityStore.query(&"model_bytes", [SHOP])
	var size: Vector3 = CityStore.query(&"body_size", [shop])
	var fit: Dictionary = CityStore.query(&"place_fit", [road, Vector2(size.x, size.y), 0.0])
	var r: Dictionary = CityStore.dispatch(&"place_model", [shop, fit.x, fit.z, fit.rot, 0, 2])
	check(r.get("ok", false) and r.buildings_removed >= 1, "the shop falls on the house and merges: %s" % r)
	var info: Dictionary = CityStore.query(&"body_info", [r.get("placed", -1)])
	check(info.volumes.size() >= 2, "the merged building has the blocks of both: %d" % info.volumes.size())
	CityStore.dispatch(&"undo")
	var again: Dictionary = CityStore.query(&"body_info", [r.get("placed", -1)])
	check(again.is_empty() or again.get("digest", 0) != info.digest, "undo parts the two")
	check(house.size() > 0, "the house stays in the catalogue")


func _library() -> void:
	var id: int = CityStore.query(&"list_buildings")[0]
	var entry: Dictionary = CityStore.query(&"building_bytes", [id])
	check(ModelLibrary.save("My house", entry), "the model is saved")
	check(ModelLibrary.save("My house", entry), "another of the same name is saved")
	var list := ModelLibrary.list()
	check(list.size() >= 2, "both are on the list: %d" % list.size())
	var got: Vector3 = CityStore.query(&"body_size", [list[0].bytes])
	check(got.x > 0.0, "the file keeps the body: %s" % got)
	for m in list:
		check(ModelLibrary.remove(m.key), "remove %s" % m.key)
	check(ModelLibrary.list().is_empty(), "the list is empty")


func _picture(tools: ToolController) -> void:
	var seen: int = CityStore.query(&"stage_show", [CityStore.query(&"model_bytes", [HOUSE]), get_viewport().find_world_3d().scenario, false])
	var triangles: int = CityStore.query(&"stage_triangles", [seen])
	check(triangles > 0, "the stage's mesh has triangles: %d" % triangles)
	print("stage: ", triangles, " triangles")
	CityStore.query(&"stage_free", [seen])
	var studio := ModelStudio.new()
	tools.add_child(studio)
	var got := {}
	studio.pictured.connect(func(key: String, picture: Texture2D) -> void: got[key] = picture)
	studio.ask(HOUSE, func() -> PackedByteArray: return CityStore.query(&"model_bytes", [HOUSE]))
	for i in WAIT_FRAMES:
		if got.has(HOUSE):
			break
		await get_tree().process_frame
	check(got.has(HOUSE), "the picture came")
	if got.has(HOUSE):
		var image: Image = got[HOUSE].get_image()
		image.save_png(scratch("model_test_house.png"))
		check(not image.is_invisible(), "the picture has the building: %s %s used %s middle %s" % [image.get_size(), image.get_format(), image.get_used_rect(), image.get_pixel(image.get_width() / 2, image.get_height() / 2)])
