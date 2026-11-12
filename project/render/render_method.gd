## What the rendering method in force draws of what the game asks for: the one place a method is named and an effect is tied to it.
## An effect asked of a method that lacks it is a warning from the engine and nothing on the screen (Godot docs: Environment,
## 3D antialiasing, RenderingServer.get_current_rendering_method).
class_name RenderMethod
extends RefCounted

const FORWARD_PLUS := "forward_plus"
const MOBILE := "mobile"
const COMPATIBILITY := "gl_compatibility"
## Each thing asked and the methods that draw it.
const DRAWN_BY := {
	&"smaa": [FORWARD_PLUS, MOBILE],
	&"temporal": [FORWARD_PLUS], # TAA and FSR2 read the motion vectors of Forward+ alone
	&"ssil": [FORWARD_PLUS], # the light surfaces throw back on each other
	&"linear_picture": [FORWARD_PLUS], # what a screen pass multiplies: elsewhere the picture is tone mapped in the material pass
	&"many_lights": [FORWARD_PLUS], # lights by cluster: elsewhere a mesh takes 8 of each kind
}


## Whether the method in force draws `effect`, one of DRAWN_BY's.
static func draws(effect: StringName) -> bool:
	return RenderingServer.get_current_rendering_method() in DRAWN_BY[effect]
