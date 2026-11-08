## The top bar: sky (day, night, the residents' clock), clock, money, counts | speed pill | layers, simulation, camera, undo, redo,
## help, menu. Reads CityStore.snapshot; the buttons ask the HUD for what they open.
class_name HudTopBar
extends PanelContainer

signal popover_pressed(which: String)
signal history_pressed(action: String)
signal impossible_pressed

const SKY := [["sun", "sky.day", "day"], ["moon", "sky.night", "night"], ["cycle", "sky.cycle", "cycle"]]
## The speeds of the core's SimSpeed, each with the steps a tick its label shows (tuning sim.speed_fast and sim.speed_fastest).
const SPEEDS := [[&"PAUSED", ""], [&"NORMAL", ""], [&"FAST", "sim_speed_fast"], [&"FASTEST", "sim_speed_fastest"]]

var action_buttons: Dictionary = {}
@export var world: CityWorld
var _clock: Label
var _money: Label
var _stats: Label
var _speed_buttons: Array[Button] = []
var _sky_buttons: Array[Button] = []


## The button that sets the sky to `mode` (SKY: "day", "night", "cycle").
func sky_button(mode: String) -> Button:
	for i in SKY.size():
		if SKY[i][2] == mode:
			return _sky_buttons[i]
	return null


## The button that sets the speed named `speed` (the core's SimSpeed).
func speed_button(speed: StringName) -> Button:
	for i in SPEEDS.size():
		if SPEEDS[i][0] == speed:
			return _speed_buttons[i]
	return null
var _history_buttons: Dictionary = {}
var _tipped: Array = [] # [button, tip key]: drawn again when the language changes
var _warn: Button


func _ready() -> void:
	theme_type_variation = &"Bar"
	anchor_right = 1.0
	offset_left = HudWidgets.px("margin")
	offset_top = HudWidgets.px("margin")
	offset_right = -HudWidgets.px("margin")
	custom_minimum_size.y = HudWidgets.px("hud_h")
	var row := HudWidgets.hbox(self, &"Gap")
	var left := HudWidgets.hbox(row, &"Gap")
	left.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	var sky := HudWidgets.hbox(HudWidgets.panel(left, &"Seg"))
	for s in SKY:
		var b := HudWidgets.button(&"SegButton", s[0], "", s[1], "seg_min_w", "seg_h")
		b.toggle_mode = true
		b.pressed.connect(set_sky.bind(s[2]))
		sky.add_child(b)
		_sky_buttons.append(b)
		_tipped.append([b, s[1]])
	_clock = HudWidgets.label(left, &"ClockLabel")
	_money = HudWidgets.label(left, &"MoneyLabel")
	_warn = HudWidgets.button(&"Chip", "warn", "", "", "", "seg_h")
	_warn.pressed.connect(func() -> void: impossible_pressed.emit())
	_warn.hide()
	left.add_child(_warn)
	_money.mouse_filter = Control.MOUSE_FILTER_PASS
	_stats = HudWidgets.label(left, &"StatsLabel")
	_stats.mouse_filter = Control.MOUSE_FILTER_PASS
	var pill := HudWidgets.hbox(HudWidgets.panel(row, &"Seg"))
	for s in SPEEDS:
		var paused: bool = s[0] == &"PAUSED"
		var label := "" if paused else "%d×" % (int(DataTables.tuning(s[1])) if s[1] != "" else 1)
		var setting := CityStore.code(&"SimSpeed", s[0])
		var b := HudWidgets.button(&"SegButton", "pause" if paused else "", label, "sim.speed", "seg_min_w", "seg_h")
		b.toggle_mode = true
		b.pressed.connect(func() -> void: CityStore.dispatch(&"set_speed", [setting]))
		pill.add_child(b)
		_speed_buttons.append(b)
		_tipped.append([b, "sim.speed"])
	var right := HudWidgets.hbox(row, &"Pad")
	right.alignment = BoxContainer.ALIGNMENT_END
	right.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	for a in DataTables.ui("drawers").actions:
		var b := HudWidgets.button(&"IconButton", a.icon, "", a.tip_key, "icon_btn", "icon_btn")
		right.add_child(b)
		_tipped.append([b, a.tip_key])
		if a.has("popover"):
			b.toggle_mode = true
			b.pressed.connect(func() -> void: popover_pressed.emit(a.popover))
			action_buttons[a.popover] = b
		elif a.has("action"):
			b.pressed.connect(func() -> void: history_pressed.emit(a.action))
			_history_buttons[a.action] = b
		else:
			HudWidgets.soon(b, a.tip_key)
	CityStore.changed.connect(_on_changed)
	relabel()


## The texts that do not follow the numbers, in the language now.
func relabel() -> void:
	for t: Array in _tipped:
		HudWidgets.tip_for(t[0], tr(t[1]))
	_money.tooltip_text = tr("ui.money.tip").format({"refund": int(round(DataTables.tuning("economy_refund_share") * Units.PERCENT))})
	_on_changed(CityStore.snapshot)


func set_sky(mode: String) -> void:
	for i in SKY.size():
		_sky_buttons[i].set_pressed_no_signal(SKY[i][2] == mode)
	if world:
		world.set_sky_mode(mode)


## Nodes that cannot be built: a warning with their count, hidden with none (the core keeps the list until the network changes).
func _sync_impossible() -> void:
	var n: int = CityStore.query(&"impossible_nodes").size()
	_warn.visible = n > 0
	_warn.text = str(n)
	_warn.tooltip_text = Formatters.count_label("status.impossible", n)


func _on_changed(s: Dictionary) -> void:
	if not s.get("ready", false):
		return
	var h: float = s.hour
	var mph: float = CityStore.constants.minutes_per_hour
	_clock.text = "%02d:%02d" % [int(h), int(fmod(h * mph, mph))]
	_money.text = Formatters.format_money(s.money)
	_stats.text = "%s · %s" % [Formatters.count_label("status.vehicles", s.vehicles), Formatters.count_label("status.people", s.people)]
	_stats.tooltip_text = tr("ui.stats.tip").format({"residents": Formatters.format_int(s.residents),
		"jobs": Formatters.format_int(s.jobs), "buildings": Formatters.format_int(s.buildings), "speed": "—"})
	_sync_impossible()
	for i in SPEEDS.size():
		_speed_buttons[i].set_pressed_no_signal(CityStore.code(&"SimSpeed", SPEEDS[i][0]) == int(s.speed))
	for action in _history_buttons:
		var b: Button = _history_buttons[action]
		b.disabled = not s.get("can_" + action, false)
		var label: String = s.get(action + "_label", "")
		b.tooltip_text = tr("action." + action) + ("" if label.is_empty() else " · " + tr(label))
