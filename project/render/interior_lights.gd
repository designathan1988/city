## The room lights of the floor a building is cut open at: a few point lights on its lamps, without shadows, and stronger at night.
## The core says where the lamps are (the lamps of its interior query); the nearest to the camera's focus carry the lights.
class_name InteriorLights
extends Node3D

var _cfg: Dictionary
var _lamps := PackedVector3Array()
var _lights: Array[OmniLight3D] = []


func _init(cfg: Dictionary) -> void:
	_cfg = cfg
	for i in int(cfg.count):
		var light := OmniLight3D.new()
		light.omni_range = cfg.range_m
		light.light_color = Color(cfg.color)
		light.shadow_enabled = false
		light.visible = false
		add_child(light)
		_lights.append(light)


func set_lamps(lamps: PackedVector3Array) -> void:
	_lamps = lamps
	_place(Vector3.ZERO)


## The lights go to the lamps nearest the focus, as bright as the hour asks.
func follow(focus: Vector3) -> void:
	_place(focus)
	var night: float = RenderingServer.global_shader_parameter_get(&"night")
	for light in _lights:
		light.light_energy = lerpf(_cfg.energy_day, _cfg.energy_night, night)


func _place(focus: Vector3) -> void:
	var order := range(_lamps.size())
	order.sort_custom(func(a: int, b: int) -> bool: return _lamps[a].distance_squared_to(focus) < _lamps[b].distance_squared_to(focus))
	for i in _lights.size():
		_lights[i].visible = i < order.size()
		if _lights[i].visible:
			_lights[i].global_position = _lamps[order[i]]
