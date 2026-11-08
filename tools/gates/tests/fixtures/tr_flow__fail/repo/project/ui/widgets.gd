# Widgets: a text helper and a helper of the helper.
class_name Widgets
extends RefCounted


static func label(parent: Control, text: String = "") -> Label:
	var l := Label.new()
	l.text = text
	parent.add_child(l)
	return l


static func row(parent: Control, name_text: String) -> Label:
	return label(parent, name_text)
