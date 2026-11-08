## The title screen over the game: play on (the city saved, or the one generated), open a map, start a new one, the graphics
## options, the language and About. Gone at the first choice; Enter or Escape play on. Until the city has drawn what the camera
## sees it stands over a curtain (a town put together unseen is shown whole), never longer than reveal_limit_s of work.
class_name StartScreen
extends Control

signal chosen(action: String)
signal city_chosen(map: int, seed: int) # the form names another city than the one standing


@export var saved := false
var _curtain: PanelContainer
var _card_box: VBoxContainer
var _worked := 0.0
var _map := 0 # what the form holds, until a choice sends it
var seed_box: SpinBox
var _seed := 0


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
	var title := HudWidgets.label(_card_box, &"TitleBig", tr("ui.title"))
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
	var now: String = CityStore.snapshot.get("style", "")
	var styles := HudWidgets.dropdown(_card_box, tr("style.label"), now if t.styles.has(now) else t.default_style,
		func(id: String) -> void: CityStore.dispatch(&"set_style", [id]))
	for id: String in t.styles:
		HudWidgets.dropdown_item(styles, id, tr(t.styles[id].name_key))
	var langs := HudWidgets.hbox(_card_box, &"Pad")
	langs.alignment = BoxContainer.ALIGNMENT_CENTER
	for locale: String in locales():
		var b := HudWidgets.button(&"Chip", "", language_name(locale), "", "", "opt_row_h")
		b.toggle_mode = true
		b.set_pressed_no_signal(TranslationServer.get_locale() == locale)
		b.pressed.connect(func() -> void:
			Settings.set_language(locale)
			_fill.call_deferred())
		langs.add_child(b)
	var about := LinkButton.new()
	about.text = tr("action.about")
	about.focus_mode = Control.FOCUS_NONE
	about.size_flags_horizontal = Control.SIZE_SHRINK_CENTER
	about.pressed.connect(pick.bind("about"))
	_card_box.add_child(about)
	for name: Control in _card_box.find_children("*", "Label", true, false):
		if name.theme_type_variation == &"FieldLabel":
			name.custom_minimum_size.x = HudWidgets.px("start_name_w")


## Size and seed of a new city. The map and seed on screen are the form's start: left alone, it opens nothing new.
func _city_form(t: Dictionary) -> void:
	var maps: Array = DataTables.maps()
	var sizes := HudWidgets.dropdown(_card_box, tr("start.size"), _map, func(at: int) -> void: _map = at)
	for id: String in t.sizes:
		var at := HudActions.map_index(id)
		if at >= 0:
			HudWidgets.dropdown_item(sizes, at, tr(maps[at].name_key))
	if _seed == 0:
		_seed = _standing_seed()
	var row := HudWidgets.option_row(_card_box, tr("start.seed"))
	seed_box = SpinBox.new()
	seed_box.min_value = 1
	seed_box.max_value = t.seed_max
	seed_box.value = _seed
	seed_box.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	seed_box.value_changed.connect(func(v: float) -> void: _seed = int(v))
	row.add_child(seed_box)



## The seed of the city standing, as the form can hold it: the form starts there, and only another seed asks for a new city.
func _standing_seed() -> int:
	return mini(int(CityStore.snapshot.get("seed", 1)), int(DataTables.game("start").seed_max))

func _action(variation: StringName, key: String, action: String, h: String) -> void:
	var b := HudWidgets.button(variation, "", tr(key), "", "", h)
	b.pressed.connect(pick.bind(action))
	_card_box.add_child(b)


## A card's action, as its button and Enter choose it: another city when the form names one, then `chosen`.
func pick(action: String) -> void:
	var another := _map != 0 or _seed != _standing_seed()
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
		pick("play")


## The languages offered, in their order (data/game/start.json): [locale, its name in itself (strings.csv language.self, its own
## column)].
## The languages offered (data/game/start.json), by locale.
static func locales() -> Array:
	return DataTables.game("start").languages


## A language's name in itself: its own translation's `language.self`.
static func language_name(locale: String) -> String:
	return TranslationServer.get_translation_object(locale).get_message(&"language.self")
