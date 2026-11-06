## The bomb's tab of the Demolish drawer: the force as a stepper (shell.ts renderSimple of the 4.0): by ones up to
## `strike_force_fine`, past it by `strike_force_coarse`, between the limits of data/catalog/blast.json.
class_name StrikeStrip
extends RefCounted


static func build(drawer: HudDrawer) -> void:
	var row := HudWidgets.option_row(drawer.options, TranslationServer.translate("strike.force"))
	var less := HudWidgets.button(&"IconButton", "minus", "", "strike.force.less", "icon_btn", "icon_btn")
	row.add_child(less)
	var shown := HudWidgets.label(row, &"BodyLabel", Formatters.format_int(_force(drawer)))
	var more := HudWidgets.button(&"IconButton", "plus", "", "strike.force.more", "icon_btn", "icon_btn")
	row.add_child(more)
	for b: Button in [less, more]:
		b.pressed.connect(func() -> void:
			var next := next_force(_force(drawer), -1 if b == less else 1)
			shown.text = Formatters.format_int(next)
			drawer.retool(next, "STRIKE", "strength"))


## The force in hand, read at each press: a lambda holds a copy of a local, so a count kept there would stop one step on.
static func _force(drawer: HudDrawer) -> int:
	return int(drawer.param("STRIKE", "strength", DataTables.tuning("strike_force_start")))


## The force one press away. Going down from just past the fine range lands back on its top, as going up left it.
static func next_force(force: int, way: int) -> int:
	var blast: Dictionary = DataTables.catalog_table("blast")
	var fine := int(DataTables.tuning("strike_force_fine"))
	var coarse := int(DataTables.tuning("strike_force_coarse"))
	if way < 0:
		return maxi(int(blast.strength_min), force - (coarse if force > fine else 1))
	return mini(int(blast.strength_max), force + (coarse if force >= fine else 1))
