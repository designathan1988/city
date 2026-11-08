## The hands of the Landscape drawer: a card that changes the panel, the whole-map actions and the Relief brush by key. The
## strips (ui/land_strips.gd) and the keys (game/flow/game_keys.gd) act through here, on the drawer they are given.
class_name LandHands
extends RefCounted



## A card of a panel: the tool's parameter, and the panel again (what it shows goes by the mode).
static func pick(value: Variant, tool_name: String, key: String, drawer: HudDrawer) -> void:
	drawer.retool(value, tool_name, key)
	_rebuild(drawer)


## A card of the Sky tab: another tool in hand, its panel in the strip.
static func switch_to(tool_name: String, drawer: HudDrawer) -> void:
	var tools := drawer.tools
	var params: Dictionary = tools.remembered.get(ToolController.Tool[tool_name], {}).duplicate() if tools else {}
	drawer.tool_requested.emit(tool_name, params)
	_rebuild(drawer)


## What wipes the whole map asks first, and only when there is something to wipe (land_panels.json "confirm").
static func action(name: String, drawer: HudDrawer) -> void:
	var ask: Dictionary = DataTables.ui("land_panels").confirm.get(name, {})
	if ask.is_empty():
		_run(name, drawer)
	elif _holds(ask):
		var dialog := ConfirmationDialog.new()
		dialog.dialog_text = TranslationServer.translate(ask.key)
		dialog.confirmed.connect(_run.bind(name, drawer))
		dialog.visibility_changed.connect(func() -> void:
			if not dialog.visible:
				dialog.queue_free())
		drawer.add_child(dialog)
		dialog.popup_centered()


## The Relief brush by key: `key` a step up or down, the sliders drawn again.
static func step_brush(drawer: HudDrawer, key: String, dir: int) -> void:
	for spec: Dictionary in DataTables.ui("land_panels").TERRAIN.sliders:
		if spec.get("key", "") == key:
			var now: float = drawer.param("TERRAIN", key, spec.default)
			pick(clampf(now + spec.step * dir, spec.min, spec.max), "TERRAIN", key, drawer)


## The Relief brush's operation by the number row; false: no such operation.
static func pick_relief(drawer: HudDrawer, index: int) -> bool:
	var cards: Array = DataTables.ui("land_panels").TERRAIN.groups[0].cards
	if index >= cards.size():
		return false
	pick(cards[index].value, "TERRAIN", "mode", drawer)
	return true


## Whether the landscape holds what the action would wipe: any of the counts named, or always when none is.
static func _holds(ask: Dictionary) -> bool:
	var counts: Array = ask.get("counts", [])
	if counts.is_empty():
		return true
	var land: Dictionary = CityStore.query(&"landscape")
	for key: String in counts:
		var held: Variant = land[key]
		if (held.size() if held is Array else int(held)) > 0:
			return true
	return false


static func _run(name: String, drawer: HudDrawer) -> void:
	var tools := drawer.tools
	if name == "scatter_clouds":
		var s := BrushDefaults.of("CLOUD", tools.remembered.get(ToolController.Tool.CLOUD, {}) if tools else {})
		var like := {"height": s.height, "size": s.size, "density": s.density / Units.PERCENT}
		var r: Dictionary = CityStore.dispatch(&"scatter_clouds", [int(s.count), like, s.variation / Units.PERCENT])
		if r.get("error", "") != "" and tools:
			tools.hint.emit(TranslationServer.translate(r.error), false)
	else:
		CityStore.dispatch(StringName(name))


static func _rebuild(drawer: HudDrawer) -> void:
	HudWidgets.clear(drawer.strip)
	HudWidgets.clear(drawer.options)
	LandStrips.build(drawer, drawer.strip, ToolController.Tool.find_key(drawer.tools.tool))
	drawer.settle()
