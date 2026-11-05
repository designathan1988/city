## The road tool's keys in a small panel beside its options (Roadcraft ui/roads/keysPanel.ts): a chip per action; pressed, it takes
## the next key before the game sees it, Escape lets it go, and a key the game keeps is refused by name.
class_name RoadKeysPanel
extends PopupPanel

signal keys_changed

var _rows: VBoxContainer
## The action whose chip waits for a key, and the line under the chips.
var _waiting := ""
var _note := ""


func _ready() -> void:
	_rows = VBoxContainer.new()
	add_child(_rows)
	window_input.connect(_key)
	refresh()


func refresh() -> void:
	HudWidgets.clear(_rows)
	HudWidgets.label(_rows, &"BodyLabel", tr("roadKeys.title"))
	for action in RoadKeys.ACTIONS:
		var row := HudWidgets.hbox(_rows, &"Pad")
		var name := HudWidgets.label(row, &"MutedLabel", tr(RoadKeys.LABEL_KEYS[action]))
		name.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		var text := tr("roadKeys.press") if _waiting == action else RoadKeys.label(RoadKeys.key_of(action))
		var chip := HudWidgets.button(&"Choice", "", text, "", "icon_btn", "icon_btn")
		chip.toggle_mode = true
		chip.set_pressed_no_signal(_waiting == action)
		chip.pressed.connect(func() -> void:
			_waiting = "" if _waiting == action else action
			_note = ""
			refresh())
		row.add_child(chip)
	if _note != "":
		HudWidgets.label(_rows, &"MutedLabel", _note)
	var reset := HudWidgets.button(&"Choice", "", tr("roadKeys.reset"), "", "icon_btn", "icon_btn")
	reset.pressed.connect(func() -> void:
		RoadKeys.reset()
		_waiting = ""
		_note = ""
		refresh()
		keys_changed.emit())
	_rows.add_child(reset)


## While a chip waits, the next key is its.
func _key(event: InputEvent) -> void:
	var k := event as InputEventKey
	if k == null or not k.pressed or k.echo or _waiting == "":
		return
	set_input_as_handled()
	if k.keycode == KEY_ESCAPE:
		_waiting = ""
		_note = ""
	elif RoadKeys.bind(_waiting, k.keycode):
		_waiting = ""
		_note = ""
		keys_changed.emit()
	else:
		_note = tr("roadKeys.reserved").format({"key": RoadKeys.label(k.keycode)})
	refresh()
