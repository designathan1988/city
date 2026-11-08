# Panel: every value a helper shows is translated where it is made.
extends Control

signal said(text: String)

var _title: String
var _field: LineEdit


func _ready() -> void:
	said.connect(_show)
	_title = tr("T")
	Widgets.label(self, tr("A"))
	Widgets.row(self, Formatters.format_int(3))
	Widgets.label(self, _title)
	Widgets.label(self, _named())
	said.emit(tr("B"))
	var shown := Widgets.label(self)
	shown.text = shown.text.format({"n": 1})
	_field = LineEdit.new()
	_field.text = _typed()
	var b := Button.new()
	b.pressed.connect(func() -> void:
		shown.text = Formatters.format_int(2)
		b.text = "%s" % [tr("C")])


func _show(text: String) -> void:
	Widgets.label(self, text)


func _named() -> String:
	return tr("N") if visible else tr("M")


func _typed() -> String:
	return "words"
