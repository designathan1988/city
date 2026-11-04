## The game's keys, by rule (data/ui/keys.json): F9 the monitor, Shift+F9 the profiler, F7 the effects, Ctrl+S / O / Z / Y, Space
## pauses and resumes, Home the view that frames the map, Escape one step back (the open panel, then the tool), each tool's
## letter or tab, Delete what the inspector shows, and the number row picks a road of the catalogue. Text fields keep their keys: this only hears what no control used.
class_name GameKeys
extends Node

var hud: RoadcraftHud
var _rules: Array = []
var _tools: Dictionary = {}
var _tabs: Dictionary = {}
var _digits: Dictionary = {}
var _resume_speed := 1


func _init(owner_hud: RoadcraftHud) -> void:
	hud = owner_hud


func _ready() -> void:
	var t: Dictionary = DataTables.ui("keys")
	for r: Dictionary in t.rules:
		var rule := r.duplicate()
		rule.code = OS.find_keycode_from_string(r.key)
		_rules.append(rule)
	for letter: String in t.tools:
		_tools[OS.find_keycode_from_string(letter)] = t.tools[letter]
	for letter: String in t.tabs:
		_tabs[OS.find_keycode_from_string(letter)] = t.tabs[letter]
	for digit: String in t.digit_keys:
		_digits[OS.find_keycode_from_string(digit)] = int(digit) - 1


func _unhandled_key_input(event: InputEvent) -> void:
	var k := event as InputEventKey
	if k == null or not k.pressed or k.echo:
		return
	for r: Dictionary in _rules:
		if k.keycode == r.code and k.ctrl_pressed == r.get("ctrl", false) and k.shift_pressed == r.get("shift", false):
			if _run(r.action):
				get_viewport().set_input_as_handled()
			return
	if k.ctrl_pressed or k.alt_pressed:
		return
	if _tools.has(k.keycode):
		hud.open_category(_tools[k.keycode])
	elif _tabs.has(k.keycode):
		hud.open_tab(_tabs[k.keycode][0], _tabs[k.keycode][1])
	elif _digits.has(k.keycode):
		hud.pick_road(_digits[k.keycode])
	else:
		return
	get_viewport().set_input_as_handled()


func _run(action: String) -> bool:
	match action:
		"health": hud.health.toggle()
		"profiler": hud.profiler.toggle()
		"effects": hud.toggle_effects()
		"save": hud.actions.menu("save")
		"open": hud.open_popover("files")
		"undo", "redo": hud.actions.history(action)
		"pause": _pause()
		"home":
			if hud.rig:
				hud.rig.reset_view()
		"delete": return hud.inspector.demolish_shown()
		"escape": return hud.escape()
	return true


## Space: pauses, and resumes at the speed it was running.
func _pause() -> void:
	var now := int(CityStore.snapshot.get("speed", 1))
	if now != int(CityStore.constants.speed_paused):
		_resume_speed = now
		CityStore.dispatch(&"set_speed", [CityStore.constants.speed_paused])
	else:
		CityStore.dispatch(&"set_speed", [_resume_speed])
