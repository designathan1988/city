## The switch at the right of a Layers row: a small track with a knob, accent when on (drawn: a Button row owns the click).
class_name HudSwitch
extends Control

const KNOB_SHARE := 0.32

@export var on := false


func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	var w := HudWidgets.px("switch_w")
	var h := w * 0.5
	set_anchors_preset(Control.PRESET_CENTER_RIGHT)
	offset_left = -w - HudWidgets.px("pad")
	offset_right = -HudWidgets.px("pad")
	offset_top = -h * 0.5
	offset_bottom = h * 0.5


func _draw() -> void:
	var h := size.y
	var track := ThemeBuilder.color("accent") if on else ThemeBuilder.color("raise_2")
	draw_circle(Vector2(h * 0.5, h * 0.5), h * 0.5, track)
	draw_circle(Vector2(size.x - h * 0.5, h * 0.5), h * 0.5, track)
	draw_rect(Rect2(h * 0.5, 0, size.x - h, h), track)
	var x := size.x - h * 0.5 if on else h * 0.5
	draw_circle(Vector2(x, h * 0.5), h * KNOB_SHARE, ThemeBuilder.color("text"))
