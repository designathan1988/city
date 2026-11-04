## The Effects panel (F7, the menu): every effect of the drawing and every heavy system switched on or off live, with what the GPU,
## the CPU and the memory spend beside them; not modal, the game stays in view. Two tabs: the switches and the optimisations,
## each option chosen alone. The readings run only while it is open, out of what it measures.
class_name EffectsPanel
extends PanelContainer

const MIB := 1048576.0
const MS_PER_S := 1000.0

@onready var _body: VBoxContainer
@onready var _stats: GridContainer
@onready var _tab := "switches"
@onready var _since := 0.0


func _ready() -> void:
	theme_type_variation = &"Popover"
	custom_minimum_size.x = HudWidgets.px("fx_w")
	set_anchors_preset(Control.PRESET_RIGHT_WIDE)
	grow_horizontal = Control.GROW_DIRECTION_BEGIN
	offset_top = HudWidgets.px("toast_top")
	offset_right = -HudWidgets.px("margin")
	offset_left = -HudWidgets.px("fx_w") - HudWidgets.px("margin")
	offset_bottom = -HudWidgets.px("margin")
	var scroll := ScrollContainer.new()
	scroll.horizontal_scroll_mode = ScrollContainer.SCROLL_MODE_DISABLED
	add_child(scroll)
	var inset := MarginContainer.new() # the scroll bar stands over the content's right edge: the text keeps clear of it
	inset.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	inset.theme_type_variation = &"ScrollInset"
	scroll.add_child(inset)
	_body = HudWidgets.vbox(inset, &"Pad")
	_body.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	hide()


func toggle() -> void:
	visible = not visible
	if visible:
		_render()


func close() -> bool:
	if not visible:
		return false
	hide()
	return true


func _render() -> void:
	HudWidgets.clear(_body)
	var head := HudWidgets.hbox(_body, &"Pad")
	var title := HudWidgets.label(head, &"HeadLabel", tr("fx.title"))
	title.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_small(head, tr("fx.reset"), _reset)
	var x := HudWidgets.button(&"IconButton", "close", "", "v2.close", "icon_btn", "opt_row_h")
	x.pressed.connect(hide)
	head.add_child(x)
	var note := HudWidgets.label(_body, &"MetaLabel", tr("fx.note"))
	note.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	_stats = GridContainer.new()
	_stats.columns = 2
	_stats.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_body.add_child(_stats)
	_read_stats()
	HudWidgets.choices(_body, [["switches", tr("fx.tab.switches")], ["optimizations", tr("fx.tab.optimizations")]], _tab,
		func(t: String) -> void:
			_tab = t
			_render.call_deferred())
	if _tab == "switches":
		_switches()
	else:
		_optimizations()


func _small(parent: Control, text: String, run: Callable) -> void:
	var b := HudWidgets.button(&"Choice", "", text, "", "", "opt_row_h")
	b.pressed.connect(func() -> void:
		run.call()
		_render.call_deferred())
	parent.add_child(b)


func _reset() -> void:
	Settings.switches.reset()
	Settings.optimizations = {}
	Settings.switches_changed()


func _switches() -> void:
	var t: Dictionary = DataTables.game("render_switches")
	var res := []
	for r: float in t.resolutions:
		res.append([r, "%d%%" % int(r * Formatters.PERCENT)])
	HudWidgets.label(_body, &"MutedLabel", tr("fx.resolution"))
	HudWidgets.choices(_body, res, Settings.switches.resolution, func(r: float) -> void:
		Settings.switches.resolution = r
		Settings.switches_changed())
	for g: Dictionary in t.groups:
		var head := HudWidgets.hbox(_body, &"Pad")
		var name := HudWidgets.label(head, &"TitleLabel", tr("fx.group." + g.id).to_upper())
		name.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		_small(head, tr("fx.allOn"), _group.bind(g.id, true))
		_small(head, tr("fx.allOff"), _group.bind(g.id, false))
		for item: Dictionary in g.items:
			var c := CheckBox.new()
			c.text = tr("fx." + item.key)
			c.focus_mode = Control.FOCUS_NONE
			c.button_pressed = Settings.switches.is_on(item.key)
			c.toggled.connect(func(on: bool) -> void:
				Settings.switches.set_on(item.key, on)
				Settings.switches_changed())
			_body.add_child(c)


func _group(id: String, on: bool) -> void:
	Settings.switches.set_group(id, on)
	Settings.switches_changed()


func _optimizations() -> void:
	for o: Dictionary in DataTables.game("optimizations").options:
		HudWidgets.label(_body, &"MutedLabel", tr("opt." + o.id))
		var items := []
		for c: Variant in o.offered:
			items.append([c, tr("opt.tier") if c is String else (tr("gfx.on") if c is bool and c else tr("gfx.off") if c is bool else str(c))])
		HudWidgets.choices(_body, items, Settings.optimizations.get(o.id, "tier"), func(v: Variant) -> void:
			if v is String:
				Settings.optimizations.erase(o.id)
			else:
				Settings.optimizations[o.id] = v
			Settings.switches_changed())


## The CPU's last frame: the core's work and the renderer's. Performance.TIME_PROCESS will not do: it is the worst frame of the last
## second (main/main.cpp process_max), so a single slow frame reads as the cost of all of them.
func _cpu_ms(rid: RID) -> float:
	return float(CityStore.query(&"render_cost").get("process", 0.0)) + RenderingServer.viewport_get_measured_render_time_cpu(rid) \
			+ RenderingServer.get_frame_setup_time_cpu()


## The language changed.
func relabel() -> void:
	if visible:
		_render()


func _read_stats() -> void:
	HudWidgets.clear(_stats)
	var rid := get_viewport().get_viewport_rid()
	var size := get_viewport().get_visible_rect().size * get_viewport().scaling_3d_scale
	var rows := [["fx.stat.gpu", "%.2f ms" % RenderingServer.viewport_get_measured_render_time_gpu(rid)],
		["fx.stat.cpu", "%.2f ms" % _cpu_ms(rid)],
		["fx.stat.fps", "%d" % Performance.get_monitor(Performance.TIME_FPS)],
		["fx.stat.calls", Formatters.format_int(int(Performance.get_monitor(Performance.RENDER_TOTAL_DRAW_CALLS_IN_FRAME)))],
		["fx.stat.triangles", Formatters.format_int(int(Performance.get_monitor(Performance.RENDER_TOTAL_PRIMITIVES_IN_FRAME)))],
		["fx.stat.textures", "%d MiB" % (Performance.get_monitor(Performance.RENDER_TEXTURE_MEM_USED) / MIB)],
		["fx.stat.geometries", "%d MiB" % (Performance.get_monitor(Performance.RENDER_BUFFER_MEM_USED) / MIB)],
		["fx.stat.static", "%d MiB" % (Performance.get_monitor(Performance.MEMORY_STATIC) / MIB)],
		["fx.stat.pixels", "%d × %d" % [size.x, size.y]]]
	for r: Array in rows:
		var name := HudWidgets.label(_stats, &"MutedLabel", tr(r[0]))
		name.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		HudWidgets.label(_stats, &"BodyLabel", r[1]).horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT


func _process(delta: float) -> void:
	if not visible:
		return
	_since += delta
	if _since >= DataTables.game("render_switches").stats_every_s:
		_since = 0.0
		_read_stats()
