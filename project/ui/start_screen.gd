## The title screen over the game: play on (the city saved, or the one generated), open a map, start a new one, the graphics
## options, the language and About. Gone at the first choice; Enter or Escape play on. Until the city has drawn what the camera
## sees it stands over a curtain (a town put together unseen is shown whole), never longer than reveal_limit_s of work.
class_name StartScreen
extends Control

signal chosen(action: String)

const LANGUAGES := [["en", "English"], ["pt_BR", "Português"]]

@export var saved := false
@onready var _curtain: PanelContainer
@onready var _card_box: VBoxContainer
@onready var _worked := 0.0


func _ready() -> void:
	set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	mouse_filter = Control.MOUSE_FILTER_STOP
	_curtain = HudWidgets.panel(self, &"Surface")
	_curtain.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	var scrim := HudWidgets.panel(self, &"Scrim")
	scrim.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	var center := CenterContainer.new()
	center.set_anchors_preset(Control.PRESET_FULL_RECT)
	add_child(center)
	var card := HudWidgets.panel(center, &"StartCard")
	card.custom_minimum_size.x = HudWidgets.px("start_w")
	_card_box = HudWidgets.vbox(card, &"Gap")
	_fill()


func _fill() -> void:
	HudWidgets.clear(_card_box)
	var title := HudWidgets.label(_card_box, &"TitleBig", "Roadcraft")
	title.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	var tag := HudWidgets.label(_card_box, &"TagLabel", tr("start.tagline"))
	tag.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	_action(&"Primary", "start.continue" if saved else "start.play", "play", "start_btn_h")
	if saved:
		_action(&"StartBtn", "action.newMap", "new", "start_btn2_h")
	_action(&"StartBtn", "action.openMap", "open", "start_btn2_h")
	_action(&"StartBtn", "menu.graphicsOptions", "graphics", "start_btn2_h")
	var langs := HudWidgets.hbox(_card_box, &"Pad")
	langs.alignment = BoxContainer.ALIGNMENT_CENTER
	for l: Array in LANGUAGES:
		var b := HudWidgets.button(&"Chip", "", l[1], "", "", "opt_row_h")
		b.toggle_mode = true
		b.set_pressed_no_signal(TranslationServer.get_locale() == l[0])
		b.pressed.connect(func() -> void:
			Settings.set_language(l[0])
			_fill.call_deferred())
		langs.add_child(b)
	var about := LinkButton.new()
	about.text = tr("action.about")
	about.focus_mode = Control.FOCUS_NONE
	about.size_flags_horizontal = Control.SIZE_SHRINK_CENTER
	about.pressed.connect(_pick.bind("about"))
	_card_box.add_child(about)


func _action(variation: StringName, key: String, action: String, h: String) -> void:
	var b := HudWidgets.button(variation, "", tr(key), "", "", h)
	b.pressed.connect(_pick.bind(action))
	_card_box.add_child(b)


func _pick(action: String) -> void:
	chosen.emit(action)
	queue_free()


func _process(delta: float) -> void:
	if not _curtain.visible:
		return
	_worked += delta
	if CityStore.query(&"render_pending") == 0 or _worked > DataTables.game("start").reveal_limit_s:
		_curtain.hide()


func _unhandled_key_input(event: InputEvent) -> void:
	var k := event as InputEventKey
	if k != null and k.pressed and (k.keycode == KEY_ENTER or k.keycode == KEY_ESCAPE):
		get_viewport().set_input_as_handled()
		_pick("play")
