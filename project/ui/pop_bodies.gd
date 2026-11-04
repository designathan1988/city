## The bodies of the Simulation and Help popovers. Simulation: the traffic and people levels (shares of the city's own density, sent
## to the core as they move), the demand, the trips and the network. Help: the keys of the tool in hand, the camera's, the game's
## and each tool's letter, a key cap and what it does per row. Rows come from data/ui/drawers.json.
class_name PopBodies
extends RefCounted

const PERCENT := 100.0
const KEY_SHARE := 0.42 # of the Help popover: the key caps' column


static func grid(parent: Control) -> GridContainer:
	var g := GridContainer.new()
	g.columns = 2
	parent.add_child(g)
	return g


## "<name> ...... <value>": the value label.
static func metric(g: GridContainer, name_text: String) -> Label:
	var name := HudWidgets.label(g, &"MutedLabel", name_text)
	name.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	var value := HudWidgets.label(g, &"BodyLabel", "—")
	value.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT
	return value


static func sim(col: Control, spec: Dictionary, metrics: Dictionary) -> void:
	var s := CityStore.snapshot
	var send := func(traffic: float, people: float, demand: float) -> void:
		CityStore.set_levels(traffic, people, demand)
	HudWidgets.range_row(col, _short("sim.traffic"), 0, spec.level_max_pct, spec.level_step_pct,
		s.get("traffic_level", 1.0) * PERCENT, " %", func(v: float) -> void:
			send.call(v / PERCENT, CityStore.snapshot.people_level, CityStore.snapshot.demand))
	HudWidgets.range_row(col, _short("sim.people"), 0, spec.level_max_pct, spec.level_step_pct,
		s.get("people_level", 1.0) * PERCENT, " %", func(v: float) -> void:
			send.call(CityStore.snapshot.traffic_level, v / PERCENT, CityStore.snapshot.demand))
	var row := HudWidgets.option_row(col, TranslationServer.translate("sim.demand"))
	var items := []
	for d: Array in spec.demand:
		items.append([d[0], TranslationServer.translate(d[1])])
	HudWidgets.choices(row, items, s.get("demand", 1.0), func(v: float) -> void:
		send.call(CityStore.snapshot.traffic_level, CityStore.snapshot.people_level, v))
	col.add_child(HSeparator.new())
	var g := grid(col)
	for m: Array in spec.metrics:
		metrics[m[1]] = metric(g, TranslationServer.translate(m[0]))
	col.add_child(HSeparator.new())
	metrics["network"] = metric(grid(col), TranslationServer.translate("sim.metric.network"))


## One word at the left: the name without its unit in brackets.
static func _short(key: String) -> String:
	return TranslationServer.translate(key).get_slice(" (", 0)


static func sync_sim(metrics: Dictionary, s: Dictionary) -> void:
	var traffic: Dictionary = CityStore.query(&"traffic_metrics")
	var values := {"trips_arrived": Formatters.format_int(s.trips_arrived), "trips_lost": Formatters.format_int(s.trips_lost),
		"mean_speed": Formatters.format_speed(traffic.mean_speed_ms), "queued": Formatters.format_int(traffic.queued),
		"network": "%s · %s" % [Formatters.count_label("status.roads", s.segments), Formatters.count_label("status.nodes", s.nodes)]}
	for k: String in metrics:
		metrics[k].text = values.get(k, "")


static func help(col: Control, spec: Dictionary, tool_name: String) -> void:
	var own: Array = spec.tool_keys.get(tool_name, [])
	if not own.is_empty():
		_keys(col, "help.section.tool", own)
	for g: Dictionary in spec.groups:
		_keys(col, g.title_key, g.rows)
	var rows := []
	var keys: Dictionary = DataTables.ui("keys").tools
	for letter: String in keys:
		rows.append([letter, spec.tool_names[keys[letter]]])
	_keys(col, "v2.do.tools", rows)


static func _keys(col: Control, title_key: String, rows: Array) -> void:
	col.add_child(HSeparator.new())
	HudWidgets.label(col, &"TitleLabel", TranslationServer.translate(title_key).to_upper())
	var g := grid(col)
	for r: Array in rows:
		var cap := HudWidgets.hbox(g)
		cap.custom_minimum_size.x = HudWidgets.px("help_w") * KEY_SHARE
		HudWidgets.kbd(cap, TranslationServer.translate(r[0])).size_flags_vertical = Control.SIZE_SHRINK_BEGIN
		var what := HudWidgets.label(g, &"MutedLabel", TranslationServer.translate(r[1]))
		what.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
		what.custom_minimum_size.x = HudWidgets.px("help_w") * (1.0 - KEY_SHARE) - HudWidgets.px("help_inset")
