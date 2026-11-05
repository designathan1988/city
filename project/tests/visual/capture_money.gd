## The money as the player meets it, by real input: the balance on the bar, the catalogue's prices, a road in hand, the
## refusal without funds, paying, demolishing, undoing and a generated road demolished. Same steps and camera as the
## 4.0's run (azimuth 45, tilt 48, 1 m per pixel), so each picture pairs with it.
## Run: godot --path project --resolution 1920x1080 res://tests/visual/capture_money.tscn -- --capture-dir=/folder
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const AZIMUTH := 45.0
const TILT := 48.0
const RIGHT_ANGLE := 90.0
const VIEW_LINES := 1080.0
const FIELD_OUT_M := 500.0
const FROM := Vector2(700, 540)
const TO := Vector2(1200, 540)
const LOW_FUNDS := 1000.0
const START_FUNDS := 20000000.0
const CLOSE_ZOOM := 4.0
const STREET := "urbanStreet" # the card the 4.0 opens on
const SHOT_SIZE := Vector2i(1920, 1080) # a window with a title bar loses 19 lines on a 1080-line screen
const NOON := 12.0
const SEARCH_STEP_M := 8.0
const SEARCH_STEPS := 200

var out_dir := "user://captures"
var rig: CityCamera
var tools: ToolController
var hud: RoadcraftHud
var ui: UiDriver


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	get_window().borderless = true
	get_window().size = SHOT_SIZE
	var world := CityWorld.new()
	add_child(world)
	rig = CityCamera.new()
	add_child(rig)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		get_tree().quit(1)
		return
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	world.set_hour(NOON)
	ui = UiDriver.new(rig, get_tree(), out_dir)
	tools = ToolController.new(rig)
	add_child(tools)
	hud = RoadcraftHud.new()
	add_child(hud)
	await _run()


func _run() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var centre := ui.ground((b.min.x + b.max.x) * 0.5, (b.min.z + b.max.z) * 0.5)
	print("janela ", get_window().size)
	await _view(centre, 1.0)
	_state("inicio")
	await ui.shot("dinheiro_barra")
	for p in CityStore.query(&"road_profiles"):
		print("perfil ", p.index, " ", p.id, " ", p.width, " m ", p.cost_per_m, "/m")
	await _view(ui.ground(b.max.x + FIELD_OUT_M, centre.z), 1.0)
	hud.open_category("roads")
	await ui.frames(4)
	await ui.press(hud.drawer.catalog._cards_row.get_child(_street_card()).get_meta(&"button"))
	await ui.shot("catalogo")
	var a: Vector3 = rig.ground_at(FROM)
	var e: Vector3 = rig.ground_at(TO)
	await _draft(a, e)
	await ui.shot("via_na_mao")
	await ui.key(KEY_ESCAPE, false)
	_set_money(LOW_FUNDS)
	await _draft(a, e)
	await ui.shot("via_sem_dinheiro")
	await ui.click(e)
	await ui.shot("recusa_clique")
	await ui.key(KEY_ESCAPE, false)
	_set_money(START_FUNDS)
	await _draft(a, e)
	await ui.click(e)
	await ui.key(KEY_ESCAPE, false)
	await ui.drawn()
	_state("via paga")
	await ui.shot("via_paga")
	hud.open_category("demolish")
	await ui.frames(4)
	var mid := a.lerp(e, 0.5)
	await ui.hover(mid)
	await ui.shot("demolir_mira")
	await ui.click(mid)
	await ui.drawn()
	_state("via demolida")
	await ui.shot("via_demolida")
	await ui.key(KEY_Z, true)
	await ui.drawn()
	_state("desfeita")
	var road := _generated_road(centre)
	await _view(road, CLOSE_ZOOM)
	await ui.hover(road)
	await ui.shot("demolir_gerada_mira")
	await ui.click(road)
	await ui.drawn()
	_state("gerada demolida")
	await ui.shot("gerada_demolida")
	print("capture_money: pronto")
	get_tree().quit(0)


## The street's place among the cards of its category.
func _street_card() -> int:
	var card := 0
	for p in CityStore.query(&"road_profiles"):
		if p.id == STREET:
			break
		if p.category == "ruas":
			card += 1
	return card


func _view(at: Vector3, zoom: float) -> void:
	var half := VIEW_LINES / (2.0 * zoom)
	rig.jump(at, RIGHT_ANGLE - AZIMUTH, TILT, CameraProfile.view_distance(rig.cfg, half))
	await ui.frames(4)
	await ui.drawn()


func _draft(a: Vector3, e: Vector3) -> void:
	await ui.click(a)
	await ui.hover(e)


func _set_money(value: float) -> void:
	CityStore.query(&"probe_set_money", [value])
	CityStore.refresh()


func _state(what: String) -> void:
	CityStore.refresh()
	print(what, ": dinheiro ", CityStore.snapshot.money, " segmentos ", CityStore.snapshot.segments)


## The first road found walking east from the middle of town.
func _generated_road(from: Vector3) -> Vector3:
	for i in SEARCH_STEPS:
		var p := ui.ground(from.x + i * SEARCH_STEP_M, from.z)
		var seg: Dictionary = CityStore.query(&"segment_at", [p])
		if seg.get("seg", -1) >= 0:
			print("via gerada ", seg.seg, " perfil ", seg.profile)
			return p
	return from
