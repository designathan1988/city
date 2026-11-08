# Panel: untranslated values reported where they are made.
extends Control

signal said(text: String)

var _title: String


func _ready() -> void:
	said.connect(_show)
	_title = "Title"
	Widgets.label(self, "Hello")
	Widgets.row(self, str(3))
	Widgets.label(self, _title)
	Widgets.label(self, _raw())
	said.emit("Raw")
	var b := Button.new()
	b.pressed.connect(func() -> void:
		b.text = Formatters.format_int(2) + "x")
	var d := {"text": "Hi"}
	Widgets.label(self, d.text)


func _show(text: String) -> void:
	Widgets.label(self, text)


func _raw() -> String:
	return "raw"
