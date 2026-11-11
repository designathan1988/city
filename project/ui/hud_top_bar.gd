## The top bar: sky (day, night, the residents' clock), clock, money, counts | speed pill | layers, simulation, camera, undo, redo,
## help, menu. Reads CityStore.snapshot and shows the sky mode the world's daylight holds (Daylight.mode_changed, whoever set it);
## the buttons ask the HUD for what they open. Each button carries what it stands for as metadata (its tip key, its sky mode, the
## popover it opens, the history step it runs), so the bar keeps no table beside its nodes.
class_name HudTopBar
extends PanelContainer

signal popover_pressed(which: String)
signal history_pressed(action: String)
signal impossible_pressed

const SKY := [["sun", "sky.day", "day"], ["moon", "sky.night", "night"], ["cycle", "sky.cycle", "cycle"]]
const SPEEDS := [["", 0], ["1×", 1], ["2×", 2], ["4×", 3]]
const TIP := &"tip_key" # the key of a button's tip, drawn again when the language changes
const SOON := &"being_built" # the button stands for what is still being built (HudWidgets.soon)
const SKY_MODE := &"sky_mode"
const POPOVER := &"popover"
const STEP := &"history_step"

@export var world: CityWorld
@onready var _clock: Label
@onready var _money: Label
@onready var _stats: Label
@onready var _speed_buttons: Array[Button] = []
@onready var _sky_buttons: Array[Button] = []
@onready var _popover_buttons: Array[Button] = []
@onready var _history_buttons: Array[Button] = []
@onready var _tipped: Array[Button] = []
@onready var _warn: Button


func _ready() -> void:
	theme_type_variation = &"Bar"
	anchor_right = 1.0
	offset_left = HudWidgets.px("margin")
	offset_top = HudWidgets.px("margin")
	offset_right = -HudWidgets.px("margin")
	custom_minimum_size.y = HudWidgets.px("hud_h")
	var row := HudWidgets.hbox(self, &"Gap")
	_build_left(row)
	var pill := HudWidgets.hbox(HudWidgets.panel(row, &"Seg"))
	for s in SPEEDS:
		var b := HudWidgets.button(&"SegButton", "pause" if s[0] == "" else "", s[0], "sim.speed", "seg_min_w", "seg_h")
		b.toggle_mode = true
		b.pressed.connect(func() -> void: CityStore.dispatch(&"set_speed", [s[1]]))
		pill.add_child(b)
		_speed_buttons.append(b)
		_tip(b, "sim.speed")
	_build_actions(row)
	if world:
		world.daylight.mode_changed.connect(_show_sky)
	_show_sky(world.sky_mode if world else "")
	CityStore.changed.connect(_on_changed)
	relabel()


## The sky segment, the clock, the money, the warning of nodes that cannot be built and the counts.
func _build_left(row: HBoxContainer) -> void:
	var left := HudWidgets.hbox(row, &"Gap")
	left.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	var sky := HudWidgets.hbox(HudWidgets.panel(left, &"Seg"))
	for s in SKY:
		var b := HudWidgets.button(&"SegButton", s[0], "", s[1], "seg_min_w", "seg_h")
		b.toggle_mode = true
		b.set_meta(SKY_MODE, s[2])
		b.pressed.connect(set_sky.bind(s[2]))
		sky.add_child(b)
		_sky_buttons.append(b)
		_tip(b, s[1])
	_clock = HudWidgets.label(left, &"ClockLabel")
	_money = HudWidgets.label(left, &"MoneyLabel")
	_warn = HudWidgets.button(&"Chip", "warn", "", "", "", "seg_h")
	_warn.pressed.connect(func() -> void: impossible_pressed.emit())
	_warn.hide()
	left.add_child(_warn)
	_money.mouse_filter = Control.MOUSE_FILTER_PASS
	_stats = HudWidgets.label(left, &"StatsLabel")
	_stats.mouse_filter = Control.MOUSE_FILTER_PASS


## The actions of drawers.json: a popover, a step of the history, or a button of what is still being built.
func _build_actions(row: HBoxContainer) -> void:
	var right := HudWidgets.hbox(row, &"Pad")
	right.alignment = BoxContainer.ALIGNMENT_END
	right.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	for a in DataTables.ui("drawers").actions:
		var b := HudWidgets.button(&"IconButton", a.icon, "", a.tip_key, "icon_btn", "icon_btn")
		right.add_child(b)
		_tip(b, a.tip_key)
		if a.has("popover"):
			b.toggle_mode = true
			b.set_meta(POPOVER, a.popover)
			b.pressed.connect(func() -> void: popover_pressed.emit(a.popover))
			_popover_buttons.append(b)
		elif a.has("action"):
			b.set_meta(STEP, a.action)
			b.pressed.connect(func() -> void: history_pressed.emit(a.action))
			_history_buttons.append(b)
		else:
			b.set_meta(SOON, true)
			HudWidgets.soon(b, a.tip_key)


func _tip(b: Button, key: String) -> void:
	b.set_meta(TIP, key)
	_tipped.append(b)


## The texts that do not follow the numbers, in the language now.
func relabel() -> void:
	for b in _tipped:
		if b.get_meta(SOON, false):
			HudWidgets.soon(b, b.get_meta(TIP))
		else:
			HudWidgets.tip_for(b, tr(b.get_meta(TIP)))
	_money.tooltip_text = tr("ui.money.tip").format({"refund": int(round(DataTables.tuning("economy_refund_share") * Formatters.PERCENT))})
	_on_changed(CityStore.snapshot)


## The player's sun, moon or clock: the world's daylight takes the mode and the bar shows what it took (_show_sky).
func set_sky(mode: String) -> void:
	if world:
		world.set_sky_mode(mode)
	else:
		_show_sky("")


## The mode the world's daylight holds: its button pressed and the others not, whoever changed it.
func _show_sky(mode: String) -> void:
	for b in _sky_buttons:
		b.set_pressed_no_signal(b.get_meta(SKY_MODE) == mode)


## The mode the pressed sky button stands for ("" with none): what the bar tells the player.
func sky_shown() -> String:
	for b in _sky_buttons:
		if b.button_pressed:
			return b.get_meta(SKY_MODE)
	return ""


## The button that opens a popover, or null when none does.
func popover_button(which: String) -> Button:
	for b in _popover_buttons:
		if b.get_meta(POPOVER) == which:
			return b
	return null


## The open popover's button pressed and the others not ("" presses none).
func press_popover(which: String) -> void:
	for b in _popover_buttons:
		b.set_pressed_no_signal(b.get_meta(POPOVER) == which)


## The button of a step of the history (undo, redo), or null.
func history_button(step: String) -> Button:
	for b in _history_buttons:
		if b.get_meta(STEP) == step:
			return b
	return null


## Nodes that cannot be built: a warning with their count, hidden with none (the core keeps the list until the network changes).
func _sync_impossible() -> void:
	var n: int = CityStore.query(&"impossible_nodes").size()
	_warn.visible = n > 0
	_warn.text = Formatters.format_int(n)
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
		_speed_buttons[i].set_pressed_no_signal(SPEEDS[i][1] == int(s.speed))
	for b in _history_buttons:
		var step: String = b.get_meta(STEP)
		b.disabled = not s.get("can_" + step, false)
		var label: String = s.get(step + "_label", "")
		b.tooltip_text = tr("action." + step) if label.is_empty() else "%s · %s" % [tr("action." + step), tr(label)]
