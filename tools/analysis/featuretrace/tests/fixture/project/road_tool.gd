## Fixture road tool: lays a road through the store.
class_name RoadTool
extends ToolBase


func click(p: float) -> void:
	_lay(p)


func _lay(p: float) -> void:
	if p > 0.0:
		Store.dispatch(&"build_road", [p])
