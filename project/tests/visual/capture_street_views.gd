## The second half of the Street tab's capture (capture_street.gd, `cap`): the signs of the dropdown laid along the footway
## across the street, and the views at a person's height, close to each piece and along the street by day and by night.
class_name CaptureStreetViews
extends RefCounted


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
		var at: Dictionary = {}
		while walked < cap.RUN_M and not at.get("ok", false):
			at = CityStore.query(&"preview_item", [across + dir * walked, cap.ITEM_SIGN, 1.0])
			walked += cap.ITEM_STEP_M
		if at.get("ok", false):
			await cap.ui.click(at.position)
			walked += cap.SIGN_GAP_M
			print("placa ", i, " em ", at.position, " ", CityStore.query(&"street_counts").items)
		if i == 0:
			cap.spots["placas"] = at.get("position", across)
	await cap.ui.shot("rua_07_placa_opcoes")


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


## Close to each piece and barrier, from the road's side at a person's height, looking at it.
static func _close_views(cap: Node, dir: Vector3) -> void:
	var side := dir.cross(Vector3.UP)
	for key: String in spots:
		var at: Vector3 = cap.spots[key]
		var toward := -side if key == "placas" or key.begins_with("barrier") else side
		var name: String = key.get_slice(".", key.get_slice_count(".") - 1)
		await _look(cap, "rua_20_" + name, at + toward * cap.CLOSE_SIDE_M - dir * cap.CLOSE_BACK_M, at + Vector3.UP * cap.CLOSE_LOOK_M)
	var row: Vector3 = cap.spots.get("placas", Vector3.ZERO)
	await _look(cap, "rua_21_placas", row - side * cap.SIGNS_SIDE_M + dir * cap.SIGNS_ALONG_M, row + dir * cap.SIGNS_ALONG_M + Vector3.UP * cap.SIGN_LOOK_M)


static func _look(cap: Node, name: String, from: Vector3, at: Vector3) -> void:
	cap.rig.camera.global_position = cap.ui.ground(from.x, from.z) + Vector3.UP * cap.EYE_M
	cap.rig.camera.look_at(at)
	await cap.ui.drawn()
	await cap.ui.shot(name)


static func _eye(cap: Node, name: String, from: Vector3, at: Vector3) -> void:
	var eye := cap.ui.ground(from.x, from.z) + Vector3.UP * maxf(from.y - _floor(cap, from), cap.EYE_M)
	cap.rig.camera.global_position = eye
	cap.rig.camera.look_at(cap.ui.ground(at.x, at.z) + Vector3.UP * cap.LOOK_Y)
	await cap.ui.drawn()
	await cap.ui.shot(name)


static func _floor(cap: Node, p: Vector3) -> float:
	return cap.ui.ground(p.x, p.z).y
