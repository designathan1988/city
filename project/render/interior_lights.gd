## The room lights of the floor a building is cut open at: a few point lights on its lamps, without shadows, stronger at night.
## The 4.0's way (renderer.ts:1620-1713, roomLamps.ts): a fixed set, the first lamps of the floor in the order its furniture
## lists them, and one intensity for all, 0.25 by day to 1 at night. The numbers are interior.json's.
class_name InteriorLights
extends Node3D

var _cfg: Dictionary
var _lights: Array[OmniLight3D] = []


func _init(cfg: Dictionary) -> void:
	_cfg = cfg
	for i in int(cfg.count):
		var light := OmniLight3D.new()
		light.omni_range = cfg.range_m
		light.omni_attenuation = cfg.attenuation
		light.light_color = Color(cfg.color)
		light.shadow_enabled = false
		light.visible = false
		add_child(light)
		_lights.append(light)


## The lights stand on the first lamps; the ones without a lamp stay off.
func set_lamps(lamps: PackedVector3Array) -> void:
	for i in _lights.size():
		_lights[i].visible = i < lamps.size()
		if _lights[i].visible:
			_lights[i].global_position = lamps[i]


## `night`: 0 day, 1 night, the daylight's own.
func set_night(night: float) -> void:
	var energy: float = _cfg.energy * lerpf(_cfg.day_share, 1.0, night)
	for light in _lights:
		light.light_energy = energy
