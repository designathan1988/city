## The Landscape brushes kept between sessions (game/tools/brush_prefs.gd, Roadcraft ui/toolChoices.ts): of each panel only the
## keys data/ui/land_panels.json lists as "kept" are kept, a choice that changes nothing kept is not written again, what is
## written to a settings file reads back the same, and a new ToolController opens each brush as the last session left it. The
## element brush keeps a brush per kind (game/tools/element_tool.gd, Roadcraft ui/toolChoices.ts elementBrushes): a slider moved
## for one kind leaves the others' as they were, and going back to it shows what was moved.
## Run: godot --path project res://tests/core/brush_prefs_test.tscn -- --no-flow
extends TestCase

const PAINT_KIND := 3 # a ground of the paint panel's cards
const TREE_DENSITY := 200
const TREE_MODE := 2
const STONES := 0 # rows of land_panels.json ELEMENTS kind_brush
const PEBBLES := 1
const MOVED_STRENGTH := 90


func _ready() -> void:
	var prefs := BrushPrefs.new()
	check(prefs.keep("PAINT", {"kind": PAINT_KIND, "radius": 50.0, "strength": 10.0}), "a new paint ground is kept")
	check(prefs.kept.get("PAINT", {}) == {"kind": PAINT_KIND}, "only the paint's kept key: %s" % [prefs.kept.get("PAINT")])
	check(not prefs.keep("PAINT", {"kind": PAINT_KIND, "radius": 90.0}), "the same ground again writes nothing")
	check(not prefs.keep("TERRAIN", {"mode": 1, "radius": 40.0}), "the relief keeps nothing between sessions")
	check(prefs.keep("TREES", {"density": TREE_DENSITY, "mode": TREE_MODE}), "a tree brush is kept")
	check(prefs.kept.TREES == {"density": TREE_DENSITY}, "the tree mode is not kept: %s" % [prefs.kept.TREES])
	var cfg := ConfigFile.new()
	prefs.write(cfg)
	var back := BrushPrefs.new()
	back.read(cfg)
	check(back.kept == prefs.kept, "read back as written: %s" % [back.kept])
	var was: Dictionary = Settings.land.kept
	Settings.land.kept = prefs.kept.duplicate(true)
	var rig := CityCamera.new()
	var tools := ToolController.new(rig)
	check(tools.last_param(ToolController.Tool.PAINT, "kind", -1) == PAINT_KIND, "the paint reopens on the kept ground")
	check(tools.last_param(ToolController.Tool.TREES, "density", -1) == TREE_DENSITY, "the trees reopen on the kept density")
	_element_brushes(tools, prefs)
	Settings.land.kept = was
	tools.free()
	rig.free()
	finish()


## The element tool taken up as the panel hands it its params: each kind's brush apart, kept by kind.
func _element_brushes(tools: ToolController, prefs: BrushPrefs) -> void:
	var elements: GDScript = ToolController.SCRIPTS[ToolController.Tool.ELEMENTS]
	var own: Array = DataTables.ui("land_panels").ELEMENTS.kind_brush
	var p := {"kind": STONES}
	elements.new(tools, p)
	check(p.strength == own[STONES].strength and p.variation == own[STONES].variation, "the stones open on their own brush: %s" % [p])
	p.strength = MOVED_STRENGTH # the slider moved for the stones
	elements.new(tools, p)
	p.kind = PEBBLES
	elements.new(tools, p)
	check(p.strength == own[PEBBLES].strength and p.variation == own[PEBBLES].variation, "the pebbles keep their own brush: %s" % [p])
	p.kind = STONES
	elements.new(tools, p)
	check(p.strength == MOVED_STRENGTH, "back to the stones, their moved strength: %s" % [p])
	check(prefs.keep("ELEMENTS", p), "the element brushes are kept")
	check(prefs.kept.ELEMENTS.keys() == ["brushes"], "only the brushes by kind: %s" % [prefs.kept.ELEMENTS])
	check(prefs.kept.ELEMENTS.brushes[STONES].strength == MOVED_STRENGTH, "the stones' brush kept: %s" % [prefs.kept.ELEMENTS])
