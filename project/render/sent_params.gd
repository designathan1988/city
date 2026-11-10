## A material's shader values as last sent: a value goes to the engine only when it differs from the one it has. A value set,
## changed or not, makes the engine write the material's uniforms again and remake its set of textures (and a sky's radiance
## map): an owner that works its values out every frame sends them through this, so the engine hears of the ones that moved.
class_name SentParams
extends RefCounted

static var _globals := {} # the shader globals as last sent (send_global)

var _material: ShaderMaterial
var _sent := {}


func _init(material: ShaderMaterial) -> void:
	_material = material


## A uniform of the material's shader.
func send(name: StringName, value: Variant) -> void:
	if _moved(_sent, name, value):
		_material.set_shader_parameter(name, value)


## A value the ShaderMaterial does not list (a name of the engine's own or of another module's shader), given to the server.
func send_raw(name: StringName, value: Variant) -> void:
	if _moved(_sent, name, value):
		RenderingServer.material_set_param(_material.get_rid(), name, value)


## A shader global.
static func send_global(name: StringName, value: Variant) -> void:
	if _moved(_globals, name, value):
		RenderingServer.global_shader_parameter_set(name, value)


## Whether `value` differs from the one kept under `name`; it is kept when it does.
static func _moved(kept: Dictionary, name: StringName, value: Variant) -> bool:
	if kept.has(name):
		var last: Variant = kept[name]
		if typeof(last) == typeof(value) and last == value:
			return false
	kept[name] = value
	return true
