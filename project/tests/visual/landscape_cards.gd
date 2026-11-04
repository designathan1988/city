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
const SHADOW_FILTERS: Array[int] = [2, 3, 4, 5]
const SHADOW_BLURS: Array[float] = [1.0, 0.6]
const BLUR_TENTHS := 10.0
const TREE_APART_M := 20.0
const TREE_DAB_M := 4.0
const TREE_DENSITY := 200.0
const TREE_HEIGHT_M := 14.0


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
	for f in SHADOW_FILTERS:
		for b in SHADOW_BLURS:
			RenderingServer.directional_soft_shadow_filter_set_quality(f as RenderingServer.ShadowQuality)
			on.world.sun.shadow_blur = b
			await on.shot("trama_%d_%d" % [f, roundi(b * BLUR_TENTHS)], at + Vector3(0, 20.0, 0), 45.0, 15.0, 6.0)


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
