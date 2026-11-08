# Widgets: the declared owner of verbatim text.
class_name HudWidgets
extends RefCounted


static func verbatim(control: Control, text: String) -> Control:
	control.auto_translate_mode = Node.AUTO_TRANSLATE_MODE_DISABLED
	control.set(&"text", text)
	return control
