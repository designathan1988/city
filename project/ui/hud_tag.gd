## The label at the tip of a road in hand: its length, its price and, in red, why it would be refused. It stands beside
## the point it is given on screen (ToolController follows the road's end) and stays inside the window.
class_name HudTag
extends PanelContainer

@onready var _label := HudWidgets.label(self, &"TagLabel")


func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	_label.mouse_filter = Control.MOUSE_FILTER_IGNORE
	hide()


## An empty text hides it.
func show_at(text: String, ok: bool, screen: Vector2) -> void:
	visible = text != ""
	if not visible:
		return
	theme_type_variation = &"Tag" if ok else &"DangerTag"
	_label.theme_type_variation = &"TagLabel" if ok else &"DangerTagLabel"
	_label.text = text
	reset_size()
	var room := get_viewport_rect().size
	var edge := HudWidgets.px("tag_edge")
	position = Vector2(
		clampf(screen.x + HudWidgets.px("tag_dx"), edge, maxf(edge, room.x - size.x - edge)),
		clampf(screen.y + HudWidgets.px("tag_dy"), HudWidgets.px("tag_top"), maxf(edge, room.y - size.y - edge)))
