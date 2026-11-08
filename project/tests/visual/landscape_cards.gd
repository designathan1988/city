## The cards the landscape capture measures alone, with the same camera as the 4.0's runs: the grey ramp, a grey box and
## planted trees. `on` is the capture, whose shot() takes the picture.
class_name LandscapeCards
extends RefCounted

const RAMP_STEPS := 17
const RAMP_LEVELS := 16.0
const BYTE_TOP := 255.0
const RAMP_CARD_M := 6.0
const RAMP_HEIGHT_M := 80.0
const CUBE_SIZE := Vector3(40.0, 120.0, 40.0)
const CUBE_ALBEDO := 0.216
const TREE_KINDS := 3
const PILLAR_SIZE := Vector3(6.0, 50.0, 6.0)
const PILLAR_LIFT_M := 30.0
const PILLARS: Array[Vector2] = [Vector2(37.0, -20.0), Vector2(17.0, -40.0)]
const SHADOW_BLURS: Array[float] = [1.0, 0.6]
const BLUR_TENTHS := 10.0
const TREE_APART_M := 20.0
const TREE_DAB_M := 4.0
const TREE_DENSITY := 200.0
const TREE_HEIGHT_M := 14.0
const CASCADES := 4
const BLADE_TILT := 8.0
const BLADE_ZOOM := 6.0
const PLATES := 8
const PLATE_FIRST_TEXEL := 0.125
const PLATE_TEXELS := 16.0
const PLATE_APART := 2.5
const PLATE_THIN := 0.02
const PLATE_MOST_OF_CASCADE := 0.3
const BLOCK_PLATES := 2.0


## Unlit cards of the same radiance in both games: what the tone chain does to each grey.
static func ramp(on: Node3D, at: Vector3) -> void:
	var day: Dictionary = DataTables.game("daylight")
	for k in RAMP_STEPS:
		var v := minf(1.0, k * RAMP_LEVELS / BYTE_TOP)
		var m := StandardMaterial3D.new()
		m.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
		m.no_depth_test = true
		var l: float = Color(v, v, v).srgb_to_linear().r * day.sky_gain
		m.albedo_color = Color(l, l, l).linear_to_srgb()
		var card := MeshInstance3D.new()
		var quad := PlaneMesh.new()
		quad.size = Vector2(RAMP_CARD_M, RAMP_CARD_M)
		card.mesh = quad
		card.material_override = m
		card.position = Vector3(at.x + (k - RAMP_STEPS / 2) * RAMP_CARD_M, RAMP_HEIGHT_M, at.z)
		on.add_child(card)
	await on.shot("rampa", Vector3(at.x, 0.0, at.z), 90.0, 50.0, 6.0)


## A grey box in both games, the same albedo on every face: how much light each side gets, in the sun and out of it.
static func cube(on: Node3D, at: Vector3) -> void:
	var box := BoxMesh.new()
	box.size = CUBE_SIZE
	var m := StandardMaterial3D.new()
	m.albedo_color = Color(CUBE_ALBEDO, CUBE_ALBEDO, CUBE_ALBEDO).linear_to_srgb()
	m.roughness = 1.0
	box.material = m
	var cube_node := MeshInstance3D.new()
	cube_node.mesh = box
	cube_node.position = Vector3(at.x, 0.0, at.z)
	on.add_child(cube_node)
	await on.shot("cubo_a", Vector3(at.x, 0.0, at.z), 45.0, 35.0, 3.0)
	await on.shot("cubo_b", Vector3(at.x, 0.0, at.z), 225.0, 35.0, 3.0)


## The shadow's edge on a plain wall, for each filter and blur: two pillars cast on the lit faces of the grey box.
static func weave(on: Node3D, at: Vector3) -> void:
	var m := StandardMaterial3D.new()
	m.albedo_color = Color(CUBE_ALBEDO, CUBE_ALBEDO, CUBE_ALBEDO).linear_to_srgb()
	m.roughness = 1.0
	for spot: Vector3 in [Vector3.ZERO, Vector3(PILLARS[0].x, 0, PILLARS[0].y), Vector3(PILLARS[1].x, 0, PILLARS[1].y)]:
		var box := BoxMesh.new()
		box.size = CUBE_SIZE if spot == Vector3.ZERO else PILLAR_SIZE
		box.material = m
		var node := MeshInstance3D.new()
		node.mesh = box
		node.position = Vector3(at.x, 0.0 if spot == Vector3.ZERO else PILLAR_LIFT_M, at.z) + spot
		on.add_child(node)
	for f in [RenderingServer.SHADOW_QUALITY_SOFT_LOW, RenderingServer.SHADOW_QUALITY_SOFT_MEDIUM, RenderingServer.SHADOW_QUALITY_SOFT_HIGH, RenderingServer.SHADOW_QUALITY_SOFT_ULTRA]:
		for b in SHADOW_BLURS:
			RenderingServer.directional_soft_shadow_filter_set_quality(f as RenderingServer.ShadowQuality)
			on.world.sun.shadow_blur = b
			await on.shot("trama_%d_%d" % [f, roundi(b * BLUR_TENTHS)], at + Vector3(0, 20.0, 0), 45.0, 15.0, 6.0)


## How far above the ground a caster starts to mark it, in each cascade: a row of plates that only cast, at heights doubling from a
## fraction of a texel, and a grey block for acne. The bias and the normal offset together are the height below which a plate leaves no mark.
static func blades(on: Node3D, at: Vector3) -> void:
	await on.shot("lamina_vazia", at, 45.0, BLADE_TILT, BLADE_ZOOM)
	var sun: DirectionalLight3D = on.world.sun
	var cam: Camera3D = on.rig.camera
	var end: float = minf(sun.directional_shadow_max_distance, cam.far)
	var splits: Array[float] = [0.0, sun.directional_shadow_split_1, sun.directional_shadow_split_2, sun.directional_shadow_split_3, 1.0]
	var right := Vector3(cam.global_basis.x.x, 0.0, cam.global_basis.x.z).normalized()
	var to_sun: Vector3 = sun.global_basis.z
	var marks: Array[String] = []
	print("escada vies ", sun.shadow_bias, " normal ", sun.shadow_normal_bias, " fim ", end, " lado ", on.world.daylight.cascades.side_px, " cortes ", splits)
	for j in CASCADES:
		var depth: float = (splits[j] + splits[j + 1]) * 0.5 * end
		var spot := _ground_at_depth(cam, at.y, depth)
		if spot == Vector3.INF:
			print("escada ", j, " out of the photo, depth ", depth)
			continue
		var texel: float = _texel(cam, splits[j] * end, splits[j + 1] * end, on.world.daylight.cascades.side_px)
		var side: float = texel * PLATE_TEXELS
		var long: float = minf(side * cam.global_position.distance_to(spot) / (cam.global_position.y - spot.y), (splits[j + 1] - splits[j]) * end * PLATE_MOST_OF_CASCADE)
		for k in PLATES:
			var high: float = texel * PLATE_FIRST_TEXEL * pow(2.0, k)
			var foot: Vector3 = spot + right * (k - (PLATES - 1) * 0.5) * side * PLATE_APART
			foot.y = CityStore.query(&"height_at", [foot.x, foot.z])
			_plate(on, Vector3(side, texel * PLATE_THIN, long), foot + Vector3.UP * high, cam.global_rotation.y)
			var mark: Vector3 = foot - Vector3(to_sun.x, 0.0, to_sun.z) / to_sun.y * high
			marks.append("%d %d %f %f %f" % [j, k, high, texel, depth])
			marks[-1] += " %s %s %s" % [foot, mark, Vector2(side, long)]
		var block: Vector3 = spot + right * (PLATES * 0.5 + 1.0) * side * PLATE_APART
		block.y = CityStore.query(&"height_at", [block.x, block.z])
		_grey(on, Vector3.ONE * side * BLOCK_PLATES, block + Vector3.UP * side * BLOCK_PLATES * 0.5, 0.0)
	await on.shot("lamina", at, 45.0, BLADE_TILT, BLADE_ZOOM)
	for line in marks:
		var cut: PackedStringArray = line.split(" ", false, 5)
		print("degrau ", " ".join(cut.slice(0, 5)), " ", _on_screen(cam, cut[5], right))


## Where a plate's mark falls on the picture and how far it reaches, for the measure.
static func _on_screen(cam: Camera3D, text: String, right: Vector3) -> String:
	var n: PackedFloat64Array = []
	for bit in text.replace("(", " ").replace(")", " ").replace(",", " ").split(" ", false):
		n.append(float(bit))
	var mark := Vector3(n[3], n[4], n[5])
	var ahead := Vector3(-right.z, 0.0, right.x)
	var mid: Vector2 = cam.unproject_position(mark)
	var across: Vector2 = cam.unproject_position(mark + right * n[6] * 0.5) - mid
	var along: Vector2 = cam.unproject_position(mark + ahead * n[7] * 0.5) - mid
	return "%f %f %f %f" % [mid.x, mid.y, absf(across.x), absf(along.y)]


## A cascade's texel in metres, from the sphere round its slice of the view (the sum Daylight makes for the first).
static func _texel(cam: Camera3D, from: float, to: float, side_px: float) -> float:
	var screen := cam.get_viewport().get_visible_rect().size
	var across: float = to * tan(deg_to_rad(cam.fov) * 0.5) * Vector2(screen.x / screen.y, 1.0).length()
	return 2.0 * Vector2(across, (to - from) * 0.5).length() / side_px


## A plate that only casts: the picture shows its mark and nothing of it.
static func _plate(on: Node3D, size: Vector3, spot: Vector3, yaw: float) -> void:
	var box := BoxMesh.new()
	box.size = size
	var node := MeshInstance3D.new()
	node.mesh = box
	node.position = spot
	node.rotation.y = yaw
	node.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_SHADOWS_ONLY
	on.add_child(node)


## The ground on the middle column of the picture at this depth along the view, by walking up the rows; INF past the horizon.
static func _ground_at_depth(cam: Camera3D, ground_y: float, depth: float) -> Vector3:
	var size := cam.get_viewport().get_visible_rect().size
	var look: Vector3 = -cam.global_basis.z
	for row in range(int(size.y) - 1, 0, -1):
		var from: Vector3 = cam.project_ray_origin(Vector2(size.x * 0.5, row))
		var way: Vector3 = cam.project_ray_normal(Vector2(size.x * 0.5, row))
		if way.y >= 0.0:
			break
		var hit: Vector3 = from + way * ((ground_y - from.y) / way.y)
		if (hit - cam.global_position).dot(look) >= depth:
			return hit
	return Vector3.INF


static func _grey(on: Node3D, size: Vector3, spot: Vector3, tilt_deg: float) -> void:
	var m := StandardMaterial3D.new()
	m.albedo_color = Color(CUBE_ALBEDO, CUBE_ALBEDO, CUBE_ALBEDO).linear_to_srgb()
	m.roughness = 1.0
	var box := BoxMesh.new()
	box.size = size
	box.material = m
	var node := MeshInstance3D.new()
	node.mesh = box
	node.position = spot
	node.rotation_degrees.z = tilt_deg
	on.add_child(node)


## An oak, a cypress and a palm planted side by side.
static func plant(at: Vector3) -> void:
	CityStore.dispatch(&"begin_stroke", ["cmd.place.tree"])
	for i in TREE_KINDS:
		var brush := {"density": TREE_DENSITY, "height": TREE_HEIGHT_M, "variation": 0.0, "spacing": TREE_DAB_M}
		CityStore.dispatch(&"tree_brush", [at + Vector3((i - 1) * TREE_APART_M, 0, 0), TREE_DAB_M, 0, i + 1, brush])
	CityStore.dispatch(&"end_stroke")


## The same three on bare ground, from the distance of the 4.0's run: each shows whole.
static func alone(on: Node3D, at: Vector3) -> void:
	plant(at)
	await on.shot("solta_palma", at + Vector3(TREE_APART_M, 0, 0), 45.0, 12.0, 25.0)
	await on.shot("solta_cipreste", at, 45.0, 12.0, 25.0)
	await on.shot("solta_carvalho", at - Vector3(TREE_APART_M, 0, 0), 45.0, 12.0, 25.0)
	await on.shot("solta_palma_perto", at + Vector3(TREE_APART_M, 0, 0), 45.0, 8.0, 60.0)


## An oak, a cypress and a palm planted at the same spot as in the 4.0's run, from close up to the woods.
static func trees(on: Node3D, at: Vector3) -> void:
	plant(at)
	await on.shot("arv_perto", at, 45.0, 12.0, 25.0)
	await on.shot("arv_copa", at + Vector3(-TREE_APART_M, 0, 0), 45.0, 8.0, 50.0)
	await on.shot("arv_media", at, 45.0, 30.0, 8.0)
	await on.shot("arv_longe", at, 45.0, 40.0, 1.5)
	await on.shot("arv_alto", at, 45.0, 45.0, 0.6)
