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
const ITEM_NAME := 11
const NAMES_TRIED := 312 # three times what one sheet of 104 held
const NAMES_FRAMES := 1200 # a chunk job, then a layer each sheet_round_s
const LIFT_STEP_M := 10.0 # a view from above rises this much at a time over a building in its way


## At a person's height along the street, from both ends and from above, by day and by night; the HUD off.
static func views(cap: Node, a: Vector3, b: Vector3) -> void:
	cap.hud.visible = false
	var dir := (b - a).normalized()
	var side := dir.cross(Vector3.UP)
	await _close_views(cap, dir)
	for hour: float in [cap.DAY, cap.NIGHT]:
		UiDriver.at_hour(cap.world, hour)
		await cap.ui.frames(int(ProjectSettings.get_setting("rendering/reflections/sky_reflections/roughness_layers")) + 1)
		var tag := "day" if hour == cap.DAY else "night"
		await _eye(cap, "street_10_street_" + tag, a - dir * cap.BACK_M + side * cap.SIDE_M, a + dir * cap.RUN_M * 0.5)
		await _eye(cap, "street_11_back_" + tag, b + dir * cap.BACK_M + side * cap.SIDE_M, b - dir * cap.RUN_M * 0.5)
		await _eye(cap, "street_13_along_" + tag, a - dir * cap.ALONG_BACK_M + Vector3.UP * cap.ALONG_UP_M, a + dir * cap.RUN_M)
		await _eye(cap, "street_12_high_" + tag, a + dir * cap.RUN_M * 0.3 + Vector3.UP * cap.TOP_M + side * cap.TOP_M, a + dir * cap.RUN_M * 0.3, true)


## Each sign of the dropdown on the footway across the street, with its words where it carries them.
	await CaptureStreetPairs.run(cap, a, dir)
	await many_names(cap, (a + cap._across) * 0.5)


static func signs(cap: Node, a: Vector3, dir: Vector3) -> void:
	var side := dir.cross(Vector3.UP)
	await cap._pick("streetscape.sign")
	var across := a
	for w in cap.ACROSS_TRIES:
		var q: Dictionary = CityStore.query(&"preview_item", [a + dir * cap.ACROSS_ALONG_M + side * (cap.ACROSS_FROM_M + w), cap.ITEM_BIN, 1.0])
		if q.get("ok", false):
			across = q.position
			break
	cap._check(across != a, "pavement in front")
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
			cap._check(CityStore.query(&"street_counts").items == had + 1, "sign %d placed" % i)
			cap.spots["sign_%d" % i] = at.position
			if i >= RULE_PLATES:
				walked += cap.SIGN_GAP_M
	await cap.ui.shot("street_07_sign_options")
	await cap.ui.frames(SHEET_FRAMES)
	_check_ink(cap)


static func _option(cap: Node, under: Node = null) -> OptionButton:
	for c in (under if under else cap.hud.drawer.strip).get_children():
		if c is OptionButton:
			return c
		var inside := _option(cap, c)
		if inside:
			return inside
	return null


static func _field(cap: Node, under: Node = null) -> LineEdit:
	for c in (under if under else cap.hud.drawer.strip).get_children():
		if c is LineEdit:
			return c
		var inside := _field(cap, c)
		if inside:
			return inside
	return null


## The sheet as the plates read it: every cell painted holds the ink of its words and its figure, once for each type.
static func _check_ink(cap: Node) -> void:
	var sheet: SignSheet = _sheet(cap)
	var images: Array[Image] = []
	for i in sheet.layers.get_layers():
		images.append(sheet.layers.get_layer_data(i))
		images[i].save_png("%s/street_23_sheet_%d.png" % [cap.out_dir, i])
	var look: Dictionary = DataTables.render().streets.signs
	for c: Array in sheet.drawn:
		var p: Dictionary = look.drawings[c[0]]
		if not p.has("ink"):
			continue
		var ink := Color(p.ink)
		var r: Rect2 = c[2]
		var image := images[c[3]]
		var found := 0
		for y in range(int(r.position.y), int(r.end.y)):
			for x in range(int(r.position.x), int(r.end.x)):
				var seen := image.get_pixel(x, y)
				if absf(seen.r - ink.r) + absf(seen.g - ink.g) + absf(seen.b - ink.b) < INK_NEAR:
					found += 1
		cap._check(found > r.get_area() * INK_LEAST, "paint on sign %d \"%s\": %d" % [c[0], c[1], found])


## More names than any sheet holds: each one painted, none left blank (the sheet grows a layer when it fills).
static func many_names(cap: Node, road: Vector3) -> void:
	var sheet: SignSheet = _sheet(cap)
	var had: int = CityStore.query(&"street_counts").sign_cells
	for i in NAMES_TRIED:
		CityStore.dispatch(&"place_item", [road, ITEM_NAME, 0, "Test Road %d" % i, 1.0])
	for f in NAMES_FRAMES:
		if sheet.drawn.size() >= had + NAMES_TRIED:
			break
		await cap.ui.frames(1)
	cap._check(sheet.drawn.size() >= had + NAMES_TRIED, "%d names painted in %d layers" % [sheet.drawn.size(), sheet.layers.get_layers()])
	_check_ink(cap)


## Close to each piece and barrier, from the road's side at a person's height, looking at it.
static func _close_views(cap: Node, dir: Vector3) -> void:
	var side := dir.cross(Vector3.UP)
	for key: String in cap.spots:
		var at: Vector3 = cap.spots[key]
		if key.begins_with("sign_"):
			continue
		var toward := -side if key.begins_with("barrier") else side
		var name: String = key.get_slice(".", key.get_slice_count(".") - 1)
		await _look(cap, "street_20_" + name, at + toward * cap.CLOSE_SIDE_M - dir * cap.CLOSE_BACK_M, at + Vector3.UP * cap.CLOSE_LOOK_M)
	for i in DataTables.ui("drawers").street.signs.size():
		if cap.spots.has("sign_%d" % i):
			var at: Vector3 = cap.spots["sign_%d" % i]
			var from: Vector3 = at - side * FACE_M if i >= FIRST_STREET_FACED else at - dir * FACE_M - side * FACE_ROAD_M
			await _look(cap, "street_22_sign_%d" % i, from, at + Vector3.UP * cap.SIGN_LOOK_M)


## The camera held at a person's height at `from`, looking at `at`, for one shot (CityCamera.pin: the rig would put its own view back).
static func _look(cap: Node, name: String, from: Vector3, at: Vector3) -> void:
	cap.rig.pin(cap.ui.ground(from.x, from.z) + Vector3.UP * cap.EYE_M, at)
	await cap.ui.drawn()
	await cap.ui.shot(name)
	cap.rig.unpin()


## A view of the street from `from` looking at `at`. It must see what it looks at: a building between the two (the eye
## inside a tower of the town, its floors drawn from within) fails the capture. A view from above (`lift`) rises over it
## instead, LIFT_STEP_M at a time; the views at a person's height stay where they stand.
static func _eye(cap: Node, name: String, from: Vector3, at: Vector3, lift: bool = false) -> void:
	var eye: Vector3 = cap.ui.ground(from.x, from.z) + Vector3.UP * maxf(from.y - _floor(cap, from), cap.EYE_M)
	var look: Vector3 = cap.ui.ground(at.x, at.z) + Vector3.UP * cap.LOOK_Y
	var rose := 0.0
	var blocker := _blocker(look, eye)
	# the tallest building there can be (tuning.json edit.max_floors of build.max_storey_m): above it nothing is in the way
	var tallest: float = DataTables.tuning("edit_max_floors") * DataTables.tuning("build_max_storey_m")
	while lift and blocker >= 0 and rose < tallest:
		rose += LIFT_STEP_M
		eye += Vector3.UP * LIFT_STEP_M
		blocker = _blocker(look, eye)
	cap._check(blocker < 0, "%s sees the street (building %d in the way, eye %s)" % [name, blocker, eye])
	if rose > 0:
		print("  %s: eye %.0f m above, over the building in the way" % [name, rose])
	cap.rig.pin(eye, look)
	await cap.ui.drawn()
	await cap.ui.shot(name)
	cap.rig.unpin()


## The building between `look` and `eye` (-1: none), by the core's pick of the bodies.
static func _blocker(look: Vector3, eye: Vector3) -> int:
	var hit: Dictionary = CityStore.query(&"body_pick", [look, eye - look])
	if hit.is_empty() or look.distance_to(hit.point) >= look.distance_to(eye):
		return -1
	return hit.id


static func _floor(cap: Node, p: Vector3) -> float:
	return cap.ui.ground(p.x, p.z).y


## The sheet of the signs, a child of the world.
static func _sheet(cap: Node) -> SignSheet:
	for child in cap.world.get_children():
		if child is SignSheet:
			return child
	return null
