## The length, price and refusal of the road in hand, beside its end (Roadcraft main.ts drawRoadPreview): it follows the
## world point on the screen and stays inside it. The road tool fills it through ToolController.tagged.
class_name PreviewTag
extends PanelContainer

@onready var _label: Label
@onready var _at := Vector3.ZERO


func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	_label = HudWidgets.label(self, &"BodyLabel")
	hide()


func show_at(text: String, at: Vector3, ok: bool) -> void:
	visible = text != ""
	theme_type_variation = &"Toast" if ok else &"DangerToast"
	_label.theme_type_variation = &"BodyLabel" if ok else &"DangerLabel"
	_label.text = text
	_at = at
	reset_size()
	_place()


func _process(_delta: float) -> void:
	if visible:
		_place()


func _place() -> void:
	var camera := get_viewport().get_camera_3d()
	if camera == null:
		return
	var end := camera.unproject_position(_at)
	var view := get_viewport_rect().size
	var edge := HudWidgets.px("preview_tag_edge")
	position = Vector2(
		clampf(end.x + HudWidgets.px("preview_tag_dx"), edge, maxf(edge, view.x - size.x - edge)),
		clampf(end.y - HudWidgets.px("preview_tag_dy"), HudWidgets.px("preview_tag_top"), view.y - size.y - edge))
