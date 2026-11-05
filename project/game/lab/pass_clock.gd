## A stamp on the GPU's timeline at one stage of the 3D frame: the engine gives the passes only to the debugger's profiler, so
## the lab asks for its own, through the compositor (Forward+ and Mobile), on the rendering thread. Costs nothing unless attached.
class_name PassClock
extends CompositorEffect

const PREFIX := "lab:"

var _stamp := ""


func _init(stage: EffectCallbackType = EFFECT_CALLBACK_TYPE_PRE_OPAQUE, stamp := "") -> void:
	effect_callback_type = stage
	_stamp = PREFIX + stamp


func _render_callback(_stage: int, _data: RenderData) -> void:
	RenderingServer.get_rendering_device().capture_timestamp(_stamp)
