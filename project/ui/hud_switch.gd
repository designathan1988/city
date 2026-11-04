## The switch at the right of a Layers row: a small track with a knob, accent when on (drawn: a Button row owns the click).
class_name HudSwitch
extends Control

const ARC_POINTS := 12

@export var on := false


func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	var w := HudWidgets.px("switch_w")
	var h := HudWidgets.px("switch_h")
	set_anchors_preset(Control.PRESET_CENTER_RIGHT)
	offset_left = -w - HudWidgets.px("switch_right")
	offset_right = -HudWidgets.px("switch_right")
	offset_top = -h * 0.5
	offset_bottom = h * 0.5


func _draw() -> void:
	var h := size.y
	var r := HudWidgets.px("switch_knob") * 0.5
	var track := ThemeBuilder.color("switch_on" if on else "switch_off")
	draw_rect(Rect2(h * 0.5, 0, size.x - h, h), track)
	draw_arc(Vector2(h * 0.5, h * 0.5), h * 0.25, PI * 0.5, PI * 1.5, ARC_POINTS, track, h * 0.5)
	draw_arc(Vector2(size.x - h * 0.5, h * 0.5), h * 0.25, -PI * 0.5, PI * 0.5, ARC_POINTS, track, h * 0.5)
	var x := size.x - h * 0.5 if on else h * 0.5
	draw_circle(Vector2(x, h * 0.5), r, ThemeBuilder.color("accent" if on else "muted"))
