## A modal dialog over the game: the map dimmed, a card in the middle; Escape or the Close button shut it. The card's rows come
## from the subclass (`fill`), rebuilt when the language changes. About is the plain one: which build this is.
class_name HudDialog
extends Control

signal closed

@onready var card: PanelContainer
@onready var column: VBoxContainer
@export var width_token := "dialog_w"


func _ready() -> void:
	set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	mouse_filter = Control.MOUSE_FILTER_STOP
	var scrim := HudWidgets.panel(self, &"Scrim")
	scrim.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	var center := CenterContainer.new()
	center.set_anchors_preset(Control.PRESET_FULL_RECT)
	add_child(center)
	card = HudWidgets.panel(center, &"Card2")
	card.custom_minimum_size.x = HudWidgets.px(width_token)
	column = HudWidgets.vbox(card, &"Gap")
	refill()


func refill() -> void:
	HudWidgets.clear(column)
	fill()


## The card's content; a subclass overrides it.
func fill() -> void:
	pass


func close_row() -> void:
	var b := HudWidgets.button(&"Ghost", "", tr("gfx.close"), "", "", "opt_row_h")
	b.size_flags_horizontal = Control.SIZE_SHRINK_BEGIN
	b.custom_minimum_size.x = HudWidgets.px("cat_w")
	b.pressed.connect(close)
	column.add_child(b)


func close() -> void:
	closed.emit()
	queue_free()


func _notification(what: int) -> void:
	if what == NOTIFICATION_TRANSLATION_CHANGED and is_node_ready():
		refill()


func _unhandled_key_input(event: InputEvent) -> void:
	var k := event as InputEventKey
	if k != null and k.pressed and k.keycode == KEY_ESCAPE:
		get_viewport().set_input_as_handled()
		close()


## A question before something that cannot wait for undo: yes runs `on_yes`, no or Escape only close.
class Confirm:
	extends HudDialog
	var _text: String
	var _on_yes: Callable

	func _init(text: String, on_yes: Callable) -> void:
		_text = text
		_on_yes = on_yes

	func fill() -> void:
		var q := HudWidgets.label(column, &"BodyLabel", _text)
		q.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
		var row := HudWidgets.hbox(column, &"Pad")
		var yes := HudWidgets.button(&"Primary", "", tr("ui.confirm.yes"), "", "cat_w", "opt_row_h")
		yes.pressed.connect(func() -> void:
			close()
			_on_yes.call())
		row.add_child(yes)
		var no := HudWidgets.button(&"Ghost", "", tr("ui.confirm.no"), "", "cat_w", "opt_row_h")
		no.pressed.connect(close)
		row.add_child(no)


## The About dialog: the build stamp (game/flow/build_stamp.gd).
class About:
	extends HudDialog

	func _init() -> void:
		width_token = "about_w"

	func fill() -> void:
		HudWidgets.label(column, &"HeadLabel", tr("about.title"))
		var row := HudWidgets.hbox(column, &"Pad")
		HudWidgets.label(row, &"HeadLabel", tr("about.build"))
		HudWidgets.label(row, &"BodyLabel", BuildStamp.text())
		close_row()
