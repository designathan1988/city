## The floor bar of the Interior layer: floor below, the floor's name, floor above. Shown only while a building is open. It reads
## the core's state (CityStore.query interior) and only says "a floor up" or "a floor down": the floor itself lives in the core.
class_name InteriorBar
extends PanelContainer

signal floor_step(delta: int)

@onready var _label: Label
@onready var _down: Button
@onready var _up: Button


func _ready() -> void:
	theme_type_variation = &"Surface"
	var row := HudWidgets.hbox(self, &"Gap")
	_down = HudWidgets.button(&"IconButton", "floorDown", "", "inside.down", "icon_btn", "icon_btn")
	_down.pressed.connect(func() -> void: floor_step.emit(-1))
	row.add_child(_down)
	_label = HudWidgets.label(row, &"BodyLabel")
	_up = HudWidgets.button(&"IconButton", "floorUp", "", "inside.up", "icon_btn", "icon_btn")
	_up.pressed.connect(func() -> void: floor_step.emit(1))
	row.add_child(_up)
	set_anchors_preset(Control.PRESET_TOP_RIGHT)
	grow_horizontal = Control.GROW_DIRECTION_BEGIN
	offset_top = HudWidgets.px("toast_top") + HudWidgets.px("seg_h")
	offset_right = -HudWidgets.px("margin")
	hide()


## The bar for the core's state: the floor's name, and a button only where there is a floor to go to.
func show_state(s: Dictionary) -> void:
	visible = s.get("on", false)
	if not visible:
		return
	_label.text = tr("inside.ground") if s.level == 0 else tr("inside.floor").format({"n": s.level})
	_down.disabled = s.level <= 0
	_up.disabled = s.level >= s.floors - 1
