## The bodies of the Simulation and Help popovers. Simulation: the traffic and people levels (shares of the city's own density, sent
## to the core as they move), the demand, the trips and the network. Help: the keys of the tool in hand, the camera's, the game's
## and each tool's letter, a key cap and what it does per row. Rows come from data/ui/drawers.json.
class_name PopBodies
extends RefCounted



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
	HudWidgets.range_row(col, TranslationServer.translate("sim.traffic"), 0, spec.level_max_pct, spec.level_step_pct,
		s.get("traffic_level", 1.0) * Units.PERCENT, Formatters.format_points, func(v: float) -> void:
			send.call(v / Units.PERCENT, CityStore.snapshot.people_level, CityStore.snapshot.demand))
	HudWidgets.range_row(col, TranslationServer.translate("sim.people"), 0, spec.level_max_pct, spec.level_step_pct,
		s.get("people_level", 1.0) * Units.PERCENT, Formatters.format_points, func(v: float) -> void:
			send.call(CityStore.snapshot.traffic_level, v / Units.PERCENT, CityStore.snapshot.demand))
	var row := HudWidgets.option_row(col, TranslationServer.translate("sim.demand"))
	var demand_row := HudWidgets.choices(row, s.get("demand", 1.0), func(v: float) -> void:
		send.call(CityStore.snapshot.traffic_level, CityStore.snapshot.people_level, v))
	for d: Array in spec.demand:
		HudWidgets.choice(demand_row, d[0], TranslationServer.translate(d[1]))
	col.add_child(HSeparator.new())
	var g := grid(col)
	for m: Array in spec.metrics:
		metrics[m[1]] = metric(g, TranslationServer.translate(m[0]))
	col.add_child(HSeparator.new())
	metrics["network"] = metric(grid(col), TranslationServer.translate("sim.metric.network"))


static func sync_sim(metrics: Dictionary, s: Dictionary) -> void:
	var traffic: Dictionary = CityStore.query(&"traffic_metrics")
	_put(metrics, "trips_arrived", Formatters.format_int(s.trips_arrived))
	_put(metrics, "trips_lost", Formatters.format_int(s.trips_lost))
	_put(metrics, "mean_speed", Formatters.format_speed(traffic.mean_speed_ms))
	_put(metrics, "queued", Formatters.format_int(traffic.queued))
	_put(metrics, "network", "%s · %s" % [Formatters.count_label("status.roads", s.segments), Formatters.count_label("status.nodes", s.nodes)])


## A metric's value, when the popover shows that metric (data/ui/drawers.json sim metrics, and the network).
static func _put(metrics: Dictionary, key: String, text: String) -> void:
	if metrics.has(key):
		metrics[key].text = text


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
		cap.custom_minimum_size.x = HudWidgets.px("help_w") * float(ThemeBuilder.tokens().help_key_share)
		HudWidgets.kbd(cap, TranslationServer.translate(r[0])).size_flags_vertical = Control.SIZE_SHRINK_BEGIN
		var what := HudWidgets.label(g, &"MutedLabel", TranslationServer.translate(r[1]))
		what.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
		what.custom_minimum_size.x = HudWidgets.px("help_w") * (1.0 - float(ThemeBuilder.tokens().help_key_share)) - HudWidgets.px("help_inset")
