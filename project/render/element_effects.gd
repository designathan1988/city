## The effects the element brush lays (smoke, fire, steam, dust, soot, sparks, spray): a pool of particle emitters over
## the ones nearest the camera, read from the core now and then. Each kind's puff comes from data/render/cover.json
## "elements.effects". Past `reach_m` an effect gives off nothing: a budget for the pool.
class_name ElementEffects
extends Node3D

var _d: Dictionary
var _pool: Array[GPUParticles3D] = []
var _wait := 0.0
var _puff: Texture2D


func _ready() -> void:
	_d = DataTables.render_table("cover").elements
	var soft := Gradient.new()
	soft.set_color(0, Color.WHITE)
	soft.set_color(1, Color(1, 1, 1, 0))
	var tex := GradientTexture2D.new()
	tex.gradient = soft
	tex.fill = GradientTexture2D.FILL_RADIAL
	tex.fill_from = Vector2(0.5, 0.5)
	tex.fill_to = Vector2(1.0, 0.5)
	_puff = tex


func _process(delta: float) -> void:
	_wait -= delta
	var camera := get_viewport().get_camera_3d()
	if _wait > 0.0 or camera == null or not CityStore.ready_to_read():
		return
	_wait = _d.read_every_s
	var at := camera.global_position
	var near: Array = CityStore.query(&"effects_near", [at, float(_d.reach_m), int(_d.props.size())])
	near.sort_custom(func(a: Dictionary, b: Dictionary) -> bool: return a.position.distance_squared_to(at) < b.position.distance_squared_to(at))
	var used := mini(near.size(), int(_d.max_emitters))
	while _pool.size() < used:
		var p := GPUParticles3D.new()
		add_child(p)
		_pool.append(p)
	for i in _pool.size():
		_pool[i].emitting = i < used
		if i < used:
			_dress(_pool[i], near[i])


func _dress(p: GPUParticles3D, e: Dictionary) -> void:
	var look: Dictionary = _d.effects[e.kind]
	p.global_position = e.position
	if p.get_meta("kind", -1) == e.kind:
		return
	p.set_meta("kind", e.kind)
	p.lifetime = look.life
	p.amount = maxi(1, int(look.rate * look.life * e.intensity))
	var m := ParticleProcessMaterial.new()
	m.emission_shape = ParticleProcessMaterial.EMISSION_SHAPE_SPHERE
	m.emission_sphere_radius = look.spread * e.size
	m.direction = Vector3.UP
	m.spread = look.cone_deg
	m.initial_velocity_min = look.rise * e.size
	m.initial_velocity_max = look.rise * e.size * 2.0
	m.gravity = Vector3.ZERO
	m.scale_min = look.size * e.size
	m.scale_max = look.size * e.size * 2.0
	p.process_material = m
	var quad := QuadMesh.new()
	var mat := StandardMaterial3D.new()
	mat.billboard_mode = BaseMaterial3D.BILLBOARD_PARTICLES
	mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED if look.glows else BaseMaterial3D.SHADING_MODE_PER_PIXEL
	mat.albedo_color = Color.html(look.color)
	mat.albedo_texture = _puff
	quad.material = mat
	p.draw_pass_1 = quad
