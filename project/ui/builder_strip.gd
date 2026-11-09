## The Build drawer's strip: the sections of a tab (data/ui/builder.json), each a title over its tool cards, the Models shelf or the
## Selection panel. A card hands its spec to the tool in hand; "choice" tools (a component, a scope) stay lit while chosen. The
## sections marked advanced (the Roadcraft's "Advanced modelling") show only once the toggle at the strip's start is on.
class_name BuilderStrip
extends RefCounted

const TOGGLE_ICON := "b_refToggle" # the eye of "show / hide"

## Whether the advanced sections show: the player's choice for the session, the same in every tab.
static var advanced := false


## The builder's part of the drawer's strip, in a row of its own: what the toggle makes again (the strip may hold more after it).
static func fill(strip: Container, tab_id: String, tools: ToolController) -> void:
	_fill_own(HudWidgets.hbox(strip, &"Gap"), tab_id, tools)


static func _fill_own(strip: Container, tab_id: String, tools: ToolController) -> void:
	var tab: Dictionary = DataTables.ui("builder").tabs.get(tab_id, {})
	var sections: Array = tab.get("sections", [])
	if sections.any(func(s: Dictionary) -> bool: return s.get("advanced", false)):
		_toggle(strip, tab_id, tools)
	for section: Dictionary in sections:
		if section.get("advanced", false) and not advanced:
			continue
		var col := HudWidgets.vbox(strip)
		HudWidgets.label(col, &"MutedLabel", TranslationServer.translate(section.title_key))
		if section.get("shelf", "") == "models":
			ModelShelf.fill(col, tools)
			continue
		if section.get("shelf", "") == "selection":
			BuilderInspector.fill(col, tools)
			continue
		var row := HudWidgets.hbox(col)
		for spec: Dictionary in section.tools:
			row.add_child(_card(spec, row, tools))


## The advanced sections shown or hidden: the strip made again.
static func _toggle(strip: Container, tab_id: String, tools: ToolController) -> void:
	var text := TranslationServer.translate("builder.advanced.on" if advanced else "builder.advanced.off")
	var b := HudWidgets.button(&"Card", TOGGLE_ICON, text, "", "card_w", "card_h")
	b.toggle_mode = true
	b.set_pressed_no_signal(advanced)
	b.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	b.vertical_icon_alignment = VERTICAL_ALIGNMENT_TOP
	b.icon_alignment = HORIZONTAL_ALIGNMENT_CENTER
	b.pressed.connect(func() -> void:
		advanced = not advanced
		HudWidgets.clear(strip)
		_fill_own(strip, tab_id, tools))
	strip.add_child(b)


static func _card(spec: Dictionary, row: Container, tools: ToolController) -> Button:
	var text := TranslationServer.translate(spec.get("label_key", "builder.tool.%s" % spec.id))
	var b := HudWidgets.button(&"Card", "b_%s" % spec.get("icon", spec.id), text, "", "card_w", "card_h")
	var choice: bool = (DataTables.ui("builder").choices as Array).has(spec.do) # what stays chosen once pressed
	b.toggle_mode = choice
	b.vertical_icon_alignment = VERTICAL_ALIGNMENT_TOP
	b.icon_alignment = HORIZONTAL_ALIGNMENT_CENTER
	b.expand_icon = false
	b.pressed.connect(func() -> void:
		if tools.active != null and tools.active.has_method("act"):
			tools.active.act(spec)
		if choice:
			_light(row, b))
	return b


## In a row of choices only the one chosen stays lit.
static func _light(row: Container, chosen: Button) -> void:
	for c in row.get_children():
		if c is Button and c != chosen:
			c.set_pressed_no_signal(false)
