# Panel: verbatim text only through the declared owner.
extends Control


func _ready() -> void:
	var l := Label.new()
	l.text = "Raw"
	l.set(&"text", "Raw")
	l.set("tooltip_text", "Tip")
	_verbatim(l, "Other")


func _verbatim(control: Control, text: String) -> void:
	control.set(&"text", text)
