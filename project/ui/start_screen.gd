## The title screen over the game: play on (the city saved, or the one generated), open a map, start a new one, the graphics
## options, the language and About. Gone at the first choice; Enter or Escape play on. Until the city has drawn what the camera
## sees it stands over a curtain (a town put together unseen is shown whole), never longer than reveal_limit_s of work.
class_name StartScreen
extends Control

signal chosen(action: String)
signal city_chosen(map: int, seed: int) # the form names another city than the one standing

const LANGUAGES := [["en", "English"], ["pt_BR", "Português"]]

@export var saved := false
@onready var _curtain: PanelContainer
@onready var _card_box: VBoxContainer
@onready var _worked := 0.0
@onready var _map := 0 # what the form holds, until a choice sends it
@onready var seed_box: SpinBox
@onready var _seed := 0
@onready var _first_seed := 0


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
	var t: Dictionary = DataTables.game("start")
	_city_form(t)
	var styles := []
	for id: String in t.styles:
		styles.append([id, tr(t.styles[id].name_key)])
	var now: String = CityStore.snapshot.get("style", "")
	HudWidgets.dropdown(_card_box, tr("style.label"), styles, now if t.styles.has(now) else t.default_style,
		func(id: String) -> void: CityStore.dispatch(&"set_style", [id]))
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


## Size and seed of a new city. The map and seed on screen are the form's start: left alone, it opens nothing new.
func _city_form(t: Dictionary) -> void:
	var sizes := []
	var maps: Array = DataTables.maps()
	for id: String in t.sizes:
		var at := HudActions.map_index(id)
		if at >= 0:
			sizes.append([at, tr(maps[at].name_key)])
	HudWidgets.dropdown(_card_box, tr("start.size"), sizes, _map, func(at: int) -> void: _map = at)
	if _first_seed == 0:
		_first_seed = mini(int(CityStore.snapshot.get("seed", 1)), int(t.seed_max))
		_seed = _first_seed
	var row := HudWidgets.option_row(_card_box, tr("start.seed"))
	seed_box = SpinBox.new()
	seed_box.min_value = 1
	seed_box.max_value = t.seed_max
	seed_box.value = _seed
	seed_box.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	seed_box.value_changed.connect(func(v: float) -> void: _seed = int(v))
	row.add_child(seed_box)


func _action(variation: StringName, key: String, action: String, h: String) -> void:
	var b := HudWidgets.button(variation, "", tr(key), "", "", h)
	b.pressed.connect(_pick.bind(action))
	_card_box.add_child(b)


func _pick(action: String) -> void:
	var another := _map != 0 or _seed != _first_seed
	if another and (action == "play" or action == "new"):
		city_chosen.emit(_map, _seed)
		action = "play"
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
