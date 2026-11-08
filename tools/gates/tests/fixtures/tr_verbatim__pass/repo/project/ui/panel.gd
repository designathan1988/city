# Panel: a player's name and an engine message, shown as given.
extends Control


func _ready() -> void:
	var l := Label.new()
	HudWidgets.verbatim(l, "Raw words")
	HudWidgets.verbatim(l, OS.get_keycode_string(KEY_A))
	l.set(&"text", tr("A"))
