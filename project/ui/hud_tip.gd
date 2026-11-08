## The interface's own tip (shell.ts tip): under the control the pointer rests on, centred on it and kept on the screen, above it when there
## is no room below; only a control with a tooltip_text, which HudWidgets.tip_for leaves empty when its name is already in sight.
class_name HudTip
extends PanelContainer

var _label: Label
var _over: Control
var _rested := 0.0


func _ready() -> void:
	theme_type_variation = &"Tip"
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	_label = HudWidgets.label(self, &"TipLabel")
	hide()


## What the pointer stands on now, a press putting the tip away until it moves to another control.
func _process(delta: float) -> void:
	var now := get_viewport().gui_get_hovered_control()
	if now != _over or Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT):
		_over = now
		_rested = 0.0
		hide()
		return
	_rested += delta
	if visible or _over == null or _over.tooltip_text == "" or _rested < HudWidgets.px("tip_wait_ms") / HudWidgets.MS_PER_S:
		return
	_label.text = _over.tooltip_text
	show()
	_place.call_deferred()


func _place() -> void:
	size = get_combined_minimum_size()
	var box := _over.get_global_rect() if is_instance_valid(_over) else Rect2()
	var room := get_viewport_rect().size
	var gap := HudWidgets.px("pad")
	var at := Vector2(box.get_center().x - size.x * 0.5, box.end.y + gap)
	if at.y + size.y > room.y:
		at.y = box.position.y - size.y - gap
	global_position = Vector2(clampf(at.x, gap, room.x - size.x - gap), at.y)
