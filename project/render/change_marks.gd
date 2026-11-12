## F8: the latest edits marked on the map, a rectangle round what each touched (the core's undo steps). Off, it draws nothing and
## reads nothing. Colour: the theme's warning; how many: data/game/health.json changes_shown.
class_name ChangeMarks
extends MeshInstance3D

const GROUND_LIFT_M := 3.0

var _lines := ImmediateMesh.new()
var _shown := -1
var _revision: Array = []


func _ready() -> void:
	mesh = _lines
	var mat := StandardMaterial3D.new()
	mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	mat.albedo_color = ThemeBuilder.color("warn")
	mat.no_depth_test = true
	material_override = mat
	cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	layers = int(CityStore.constants.render_layers.base)
	visible = false
	CityStore.watch(self, ["city_generation", "history_revision"], _on_changed)


func toggle() -> void:
	visible = not visible
	_shown = -1
	_revision.clear()
	_on_changed(CityStore.snapshot)


## How many marks are drawn (tests).
func count() -> int:
	return maxi(_shown, 0)


func _on_changed(snapshot: Dictionary) -> void:
	if not visible:
		return
	var revision := [snapshot.get("city_generation", -1), snapshot.get("history_revision", -1)]
	if revision == _revision:
		return
	_revision = revision
	var edits: Array = CityStore.query(&"recent_edits", [int(DataTables.game("health").changes_shown)])
	_shown = edits.size()
	_lines.clear_surfaces()
	for e: Dictionary in edits:
		if e.has_box:
			_box(e.min, e.max)


func _box(a: Vector2, b: Vector2) -> void:
	var corners := [Vector2(a.x, a.y), Vector2(b.x, a.y), Vector2(b.x, b.y), Vector2(a.x, b.y), Vector2(a.x, a.y)]
	_lines.surface_begin(Mesh.PRIMITIVE_LINE_STRIP)
	for c: Vector2 in corners:
		_lines.surface_add_vertex(Vector3(c.x, float(CityStore.query(&"height_at", [c.x, c.y])) + GROUND_LIFT_M, c.y))
	_lines.surface_end()
