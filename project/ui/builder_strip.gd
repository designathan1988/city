## The Build drawer's strip: the sections of a tab (data/ui/builder.json), each a title over its tool cards. A card hands its spec
## to the tool in hand; "choice" tools (a component, a scope) stay lit while chosen.
class_name BuilderStrip
extends RefCounted

const CHOICES := ["arm", "part", "gear", "flag", "flag_colour", "flag_slot", "scope", "paint_scope"]


static func fill(strip: Container, tab_id: String, tools: ToolController) -> void:
	var tab: Dictionary = DataTables.ui("builder").tabs.get(tab_id, {})
	for section: Dictionary in tab.get("sections", []):
		var col := HudWidgets.vbox(strip)
		HudWidgets.label(col, &"MutedLabel", TranslationServer.translate(section.title_key))
		if section.has("shelf"):
			ModelShelf.fill(col, tools)
			continue
		var row := HudWidgets.hbox(col)
		for spec: Dictionary in section.tools:
			row.add_child(_card(spec, row, tools))


static func _card(spec: Dictionary, row: Container, tools: ToolController) -> Button:
	var text := TranslationServer.translate(spec.get("label_key", "builder.tool.%s" % spec.id))
	var b := HudWidgets.button(&"Card", "b_%s" % spec.get("icon", spec.id), text, "", "card_w", "card_h")
	var choice: bool = CHOICES.has(spec.do)
	b.toggle_mode = choice
	b.vertical_icon_alignment = VERTICAL_ALIGNMENT_TOP
	b.icon_alignment = HORIZONTAL_ALIGNMENT_CENTER
	b.expand_icon = false
	b.pressed.connect(func() -> void:
		if tools.active is BuildingTool:
			tools.active.act(spec)
		if choice:
			_light(row, b))
	return b


## In a row of choices only the one chosen stays lit.
static func _light(row: Container, chosen: Button) -> void:
	for c in row.get_children():
		if c is Button and c != chosen:
			c.set_pressed_no_signal(false)
