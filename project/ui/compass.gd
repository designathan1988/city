## The compass, a corner of the screen: the needle shows where north lies; a click turns the view to face it, a drag across it
## turns and tilts the camera (the dial follows the hand: dragged right, north swings clockwise).
class_name HudCompass
extends Button

@export var rig: CityCamera
var _needle: TextureRect
var _press := Vector2.ZERO
var _held := false
var _moved := false
var _turn := 0.0
var _tilt := 0.0
var _slop := 0.0


func _init(camera_rig: CityCamera) -> void:
	rig = camera_rig


func _ready() -> void:
	var size_px := HudWidgets.px("compass")
	var margin := HudWidgets.px("compass_margin")
	var t: Dictionary = DataTables.game("camera")
	_turn = 1.0 / t.compass_turn_px_per_rad
	_tilt = 1.0 / t.compass_tilt_px_per_rad
	_slop = t.compass_click_slop_px
	theme_type_variation = &"CompassButton"
	tooltip_text = tr("compass.label")
	focus_mode = Control.FOCUS_NONE
	custom_minimum_size = Vector2.ONE * size_px
	set_anchors_preset(Control.PRESET_BOTTOM_RIGHT)
	offset_left = -size_px - margin
	offset_top = -size_px - margin
	offset_right = -margin
	offset_bottom = -margin
	_needle = TextureRect.new()
	_needle.texture = HudWidgets.icon("compass_full")
	_needle.expand_mode = TextureRect.EXPAND_IGNORE_SIZE
	_needle.stretch_mode = TextureRect.STRETCH_KEEP_ASPECT_CENTERED
	_needle.mouse_filter = Control.MOUSE_FILTER_IGNORE
	_needle.set_anchors_preset(Control.PRESET_FULL_RECT)
	_needle.pivot_offset = Vector2.ONE * size_px * 0.5
	add_child(_needle)


func _gui_input(event: InputEvent) -> void:
	var b := event as InputEventMouseButton
	if b != null and b.button_index == MOUSE_BUTTON_LEFT:
		if b.pressed:
			_held = true
			_moved = false
			_press = b.position
		elif _held:
			_held = false
			if not _moved and rig:
				rig.north()
		accept_event()
	var m := event as InputEventMouseMotion
	if m != null and _held and rig:
		var d := m.position - _press
		if not _moved and d.length() < _slop:
			return
		_moved = true
		_press = m.position
		rig.orbit(-d.x * _turn, -d.y * _tilt)
		accept_event()


func _process(_delta: float) -> void:
	if rig:
		_needle.rotation = -rig.yaw
