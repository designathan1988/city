## Landscape › Sky › Weather: the sliders of the drawer set the map's rain, wind and storm; a click calls a bolt down
## there (render/weather_view.gd).
extends ToolBase


func enter() -> void:
	ctl.hint.emit(tr("hint.tool.weather"), true)


func click(p: Vector3) -> void:
	var sky := ctl.get_tree().get_first_node_in_group(&"sky")
	if sky:
		sky.strike_at(p)
