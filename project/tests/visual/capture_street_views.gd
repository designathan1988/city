## The second half of the Street tab's capture (capture_street.gd, `cap`): the signs of the dropdown laid along the footway
## across the street, and the views at a person's height, close to each piece and along the street by day and by night.
class_name CaptureStreetViews
extends RefCounted

const RULE_PLATES := 2
const RULE_GAP := 2.0
const FIRST_STREET_FACED := 7 # direction, street and information look at the street; the others at the traffic
const FACE_M := 3.2
const FACE_ROAD_M := 1.6
const SHEET_FRAMES := 4 # a cell asked for is painted on the next frames
const INK_NEAR := 0.3 # how near the ink a pixel of the sheet reads as ink
const INK_LEAST := 0.01 # of a cell, however short its word


## At a person's height along the street, from both ends and from above, by day and by night; the HUD off.
static func views(cap: Node, a: Vector3, b: Vector3) -> void:
	cap.hud.visible = false
	var dir := (b - a).normalized()
	var side := dir.cross(Vector3.UP)
	await _close_views(cap, dir)
	for hour: float in [cap.DAY, cap.NIGHT]:
		cap.world.set_hour(hour)
		await cap.ui.frames(int(ProjectSettings.get_setting("rendering/reflections/sky_reflections/roughness_layers")) + 1)
		var tag := "dia" if hour == cap.DAY else "noite"
		await _eye(cap, "rua_10_rua_" + tag, a - dir * cap.BACK_M + side * cap.SIDE_M, a + dir * cap.RUN_M * 0.5)
		await _eye(cap, "rua_11_volta_" + tag, b + dir * cap.BACK_M + side * cap.SIDE_M, b - dir * cap.RUN_M * 0.5)
		await _eye(cap, "rua_13_ao_longo_" + tag, a - dir * cap.ALONG_BACK_M + Vector3.UP * cap.ALONG_UP_M, a + dir * cap.RUN_M)
		await _eye(cap, "rua_12_alto_" + tag, a + dir * cap.RUN_M * 0.3 + Vector3.UP * cap.TOP_M + side * cap.TOP_M, a + dir * cap.RUN_M * 0.3)


## Each sign of the dropdown on the footway across the street, with its words where it carries them.
	await CaptureStreetPairs.run(cap, a, dir)


static func signs(cap: Node, a: Vector3, dir: Vector3) -> void:
	var side := dir.cross(Vector3.UP)
	await cap._pick("streetscape.sign")
	var across := a
	for w in cap.ACROSS_TRIES:
		var q: Dictionary = CityStore.query(&"preview_item", [a + dir * cap.ACROSS_ALONG_M + side * (cap.ACROSS_FROM_M + w), cap.ITEM_BIN, 1.0])
		if q.get("ok", false):
			across = q.position
			break
	cap._check(across != a, "calçada em frente")
	cap._across = across
	var count: int = DataTables.ui("drawers").street.signs.size()
	var walked := 0.0
	for i in count:
		var kinds := _option(cap)
		kinds.select(i)
		kinds.item_selected.emit(i)
		await cap.ui.frames(2)
		var field := _field(cap)
		if field:
			field.text = cap.SIGN_WORDS
			field.text_changed.emit(cap.SIGN_WORDS)
		var want: Vector3 = across + dir * walked
		if i < RULE_PLATES: # Stop and Give Way away from a junction, or they are that leg's rule and no plate of the player's
			want = across - dir * cap.SIGN_GAP_M * (i + 1) * RULE_GAP
		var at: Dictionary = CityStore.query(&"preview_item", [want, cap.ITEM_SIGN, 1.0])
		while i >= RULE_PLATES and walked < cap.RUN_M and not at.get("ok", false):
			walked += cap.ITEM_STEP_M
			at = CityStore.query(&"preview_item", [across + dir * walked, cap.ITEM_SIGN, 1.0])
		if at.get("ok", false):
			var had: int = CityStore.query(&"street_counts").items
			await cap.ui.click(at.position)
			cap._check(CityStore.query(&"street_counts").items == had + 1, "placa %d colocada" % i)
			cap.spots["placa_%d" % i] = at.position
			if i >= RULE_PLATES:
				walked += cap.SIGN_GAP_M
	await cap.ui.shot("rua_07_placa_opcoes")
	await cap.ui.frames(SHEET_FRAMES)
	_check_ink(cap)


static func _option(cap: Node, under: Node = null) -> OptionButton:
	for c in (under if under else cap.hud.drawer._strip).get_children():
		if c is OptionButton:
			return c
		var inside := _option(cap, c)
		if inside:
			return inside
	return null


static func _field(cap: Node, under: Node = null) -> LineEdit:
	for c in (under if under else cap.hud.drawer._strip).get_children():
		if c is LineEdit:
			return c
		var inside := _field(cap, c)
		if inside:
			return inside
	return null


## The sheet as the plates read it: every cell painted holds the ink of its words and its figure, once for each type.
static func _check_ink(cap: Node) -> void:
	var sheet: SignSheet = cap.world._sign_sheet
	var image := sheet.get_texture().get_image()
	image.save_png("%s/rua_23_folha.png" % cap.out_dir)
	var look: Dictionary = DataTables.render().streets.signs
	for c: Array in sheet.drawn:
		var p: Dictionary = look.drawings[c[0]]
		if not p.has("ink"):
			continue
		var ink := Color(p.ink)
		var r: Rect2 = c[2]
		var found := 0
		for y in range(int(r.position.y), int(r.end.y)):
			for x in range(int(r.position.x), int(r.end.x)):
				var seen := image.get_pixel(x, y)
				if absf(seen.r - ink.r) + absf(seen.g - ink.g) + absf(seen.b - ink.b) < INK_NEAR:
					found += 1
		cap._check(found > r.get_area() * INK_LEAST, "tinta na placa %d \"%s\": %d" % [c[0], c[1], found])


## Close to each piece and barrier, from the road's side at a person's height, looking at it.
static func _close_views(cap: Node, dir: Vector3) -> void:
	var side := dir.cross(Vector3.UP)
	for key: String in cap.spots:
		var at: Vector3 = cap.spots[key]
		if key.begins_with("placa"):
			continue
		var toward := -side if key.begins_with("barrier") else side
		var name: String = key.get_slice(".", key.get_slice_count(".") - 1)
		await _look(cap, "rua_20_" + name, at + toward * cap.CLOSE_SIDE_M - dir * cap.CLOSE_BACK_M, at + Vector3.UP * cap.CLOSE_LOOK_M)
	for i in DataTables.ui("drawers").street.signs.size():
		if cap.spots.has("placa_%d" % i):
			var at: Vector3 = cap.spots["placa_%d" % i]
			var from: Vector3 = at - side * FACE_M if i >= FIRST_STREET_FACED else at - dir * FACE_M - side * FACE_ROAD_M
			await _look(cap, "rua_22_placa_%d" % i, from, at + Vector3.UP * cap.SIGN_LOOK_M)


static func _look(cap: Node, name: String, from: Vector3, at: Vector3) -> void:
	cap.rig.camera.global_position = cap.ui.ground(from.x, from.z) + Vector3.UP * cap.EYE_M
	cap.rig.camera.look_at(at)
	await cap.ui.drawn()
	await cap.ui.shot(name)


static func _eye(cap: Node, name: String, from: Vector3, at: Vector3) -> void:
	var eye: Vector3 = cap.ui.ground(from.x, from.z) + Vector3.UP * maxf(from.y - _floor(cap, from), cap.EYE_M)
	cap.rig.camera.global_position = eye
	cap.rig.camera.look_at(cap.ui.ground(at.x, at.z) + Vector3.UP * cap.LOOK_Y)
	await cap.ui.drawn()
	await cap.ui.shot(name)


static func _floor(cap: Node, p: Vector3) -> float:
	return cap.ui.ground(p.x, p.z).y
