## The F9 monitor: three tabs. Health: the log's latest entries, a dot by severity, when, what, how much; a click shows the detail.
## Changes: the core's revision counters. State: what the game holds now (tool, speed, tier, frame). Copy puts it all on the
## clipboard as JSON. Refreshed once a second, only while open.
class_name HealthPanel
extends PanelContainer

const TABS := ["health", "world", "state"]

var _tab := "health"
var _body: VBoxContainer
var _tabs := {}
var _picked := -1
var _since := 0.0
var _seen := -1
var _copy_button: Button


func _ready() -> void:
	theme_type_variation = &"Popover"
	custom_minimum_size.x = HudWidgets.px("health_w")
	var col := HudWidgets.vbox(self, &"Pad")
	var head := HudWidgets.hbox(col, &"Pad")
	for id: String in TABS:
		var b := HudWidgets.button(&"DrawerTab", "", tr("health.tab." + id), "", "", "opt_row_h")
		b.toggle_mode = true
		b.focus_mode = Control.FOCUS_CLICK
		b.pressed.connect(_select.bind(id))
		b.gui_input.connect(_tab_key)
		head.add_child(b)
		_tabs[id] = b
	var gap := Control.new()
	gap.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	head.add_child(gap)
	_copy_button = HudWidgets.button(&"Ghost", "", tr("health.copy"), "", "", "opt_row_h")
	var copy := _copy_button
	copy.pressed.connect(_copy)
	head.add_child(copy)
	_body = HudWidgets.vbox(col)
	hide()


func toggle() -> void:
	visible = not visible
	if visible:
		position = Vector2.ONE * HudWidgets.px("margin")
		_select(_tab)


## Escape: false when it was not open.
func close() -> bool:
	if not visible:
		return false
	hide()
	return true


## Arrows, Home and End move along the tabs that has the focus (WAI-ARIA tabs).
func _tab_key(event: InputEvent) -> void:
	var k := event as InputEventKey
	if k == null or not k.pressed:
		return
	var at := TABS.find(_tab)
	match k.keycode:
		KEY_LEFT: at = posmod(at - 1, TABS.size())
		KEY_RIGHT: at = posmod(at + 1, TABS.size())
		KEY_HOME: at = 0
		KEY_END: at = TABS.size() - 1
		_: return
	_select(TABS[at])
	_tabs[TABS[at]].grab_focus()
	accept_event()


## The language changed.
func relabel() -> void:
	for id: String in TABS:
		_tabs[id].text = tr("health.tab." + id)
	_copy_button.text = tr("health.copy")
	if visible:
		_render()


func _select(id: String) -> void:
	_tab = id
	for k: String in _tabs:
		_tabs[k].set_pressed_no_signal(k == id)
	_render()


func _render() -> void:
	HudWidgets.clear(_body)
	match _tab:
		"health": _render_health()
		"world": _lines(_world_lines())
		"state": _lines(_state_lines())
	reset_size()


func _render_health() -> void:
	HudWidgets.label(_body, &"MutedLabel", tr("health.status." + Health.status()).format({"seconds": int(DataTables.game("health").status_window_s)}))
	for e: Dictionary in Health.entries.slice(0, int(DataTables.game("health").rows)):
		var row := HudWidgets.hbox(HudWidgets.panel(_body, &"RowBand"), &"Gap")
		var dot := Panel.new()
		dot.theme_type_variation = &"DotBroken" if e.severity == "broken" else &"DotSlow" if e.severity == "slow" else &"DotNote"
		dot.custom_minimum_size = Vector2.ONE * HudWidgets.px("dot")
		dot.size_flags_vertical = Control.SIZE_SHRINK_CENTER
		row.add_child(dot)
		HudWidgets.label(row, &"MutedLabel", tr("health.earlier") if e.earlier else e.wall)
		var what := HudWidgets.button(&"Row", "", e.message, e.message, "", "")
		what.alignment = HORIZONTAL_ALIGNMENT_LEFT
		what.clip_text = true
		what.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		what.pressed.connect(func() -> void:
			_picked = -1 if _picked == e.serial else int(e.serial)
			_render())
		row.add_child(what)
		var much := ("%d ms " % e.ms if e.ms > 0 else "") + ("×%d" % e.count if e.count > 1 else "")
		HudWidgets.label(row, &"MutedLabel", much)
		if _picked == e.serial and e.detail != "":
			var detail := HudWidgets.label(_body, &"MetaLabel", e.detail)
			detail.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART


func _world_lines() -> Array:
	var s := CityStore.snapshot
	var lines := ["%s %s" % [Formatters.format_int(s.get("segments", 0)), tr("status.roads.other")],
		"%s %s" % [Formatters.format_int(s.get("buildings", 0)), tr("fx.buildings")]]
	for e: Dictionary in CityStore.query(&"recent_edits", [int(DataTables.game("health").rows)]):
		lines.append(tr("health.world.undo").format({"label": tr(e.label)}))
	return lines


func _state_lines() -> Array:
	var out := []
	for k: String in ["ready", "speed", "unsaved", "edited", "map", "seed", "vehicles", "people"]:
		out.append("%s: %s" % [k, CityStore.snapshot.get(k, "")])
	out.append("tier: %s (%s)" % [Settings.level, Settings.preset])
	out.append("fps: %d" % int(Performance.get_monitor(Performance.TIME_FPS)))
	return out


func _lines(lines: Array) -> void:
	for l: String in lines:
		HudWidgets.label(HudWidgets.panel(_body, &"RowBand"), &"BodyLabel", l)


func _copy() -> void:
	DisplayServer.clipboard_set(JSON.stringify({"health": Health.entries, "world": _world_lines(), "state": _state_lines()}, " "))


func _process(delta: float) -> void:
	if not visible:
		return
	_since += delta
	if _since >= DataTables.game("health").refresh_s:
		_since = 0.0
		if _tab != "health" or Health.changes != _seen:
			_seen = Health.changes
			_render()
