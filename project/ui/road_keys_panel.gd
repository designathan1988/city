## The road tool's keys in a small panel beside its options (Roadcraft ui/roads/keysPanel.ts): a chip per action; pressed, it takes
## the next key before the game sees it, Escape lets it go, and a key the game keeps is refused by name.
class_name RoadKeysPanel
extends PopupPanel

signal keys_changed

var _rows := VBoxContainer.new()


func _ready() -> void:
	add_child(_rows)
	window_input.connect(_key)
	refresh()


func refresh() -> void:
	HudWidgets.clear(_rows)
	HudWidgets.label(_rows, &"BodyLabel", tr("roadKeys.title"))
	var step := Formatters.format_measure(DataTables.game("tools").road.height_step_m)
	for action in RoadKeys.ACTIONS:
		var row := HudWidgets.hbox(_rows, &"Pad")
		var name := HudWidgets.label(row, &"MutedLabel", tr(RoadKeys.LABEL_KEYS[action]).format({"step_m": step}))
		name.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		var text := tr("roadKeys.press") if RoadKeys.waiting == action else RoadKeys.label(RoadKeys.key_of(action))
		var chip := HudWidgets.verbatim(HudWidgets.button(&"Choice", "", "", "", "icon_btn", "icon_btn"), text) as Button
		chip.toggle_mode = true
		chip.set_pressed_no_signal(RoadKeys.waiting == action)
		chip.pressed.connect(func() -> void:
			RoadKeys.waiting = "" if RoadKeys.waiting == action else action
			RoadKeys.note = ""
			refresh())
		row.add_child(chip)
	if RoadKeys.note != "":
		HudWidgets.label(_rows, &"MutedLabel", RoadKeys.note)
	var reset := HudWidgets.button(&"Choice", "", tr("roadKeys.reset"), "", "icon_btn", "icon_btn")
	reset.pressed.connect(func() -> void:
		RoadKeys.reset()
		RoadKeys.waiting = ""
		RoadKeys.note = ""
		refresh()
		keys_changed.emit())
	_rows.add_child(reset)


## While a chip waits, the next key is its.
func _key(event: InputEvent) -> void:
	var k := event as InputEventKey
	if k == null or not k.pressed or k.echo or RoadKeys.waiting == "":
		return
	set_input_as_handled()
	if k.keycode == KEY_ESCAPE:
		RoadKeys.waiting = ""
		RoadKeys.note = ""
	elif RoadKeys.bind(RoadKeys.waiting, k.keycode):
		RoadKeys.waiting = ""
		RoadKeys.note = ""
		keys_changed.emit()
	else:
		RoadKeys.note = tr("roadKeys.reserved").format({"key": RoadKeys.label(k.keycode)})
	refresh()
