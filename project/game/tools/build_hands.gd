## What the Build tool holds besides its pick: a rectangle to drag, a core or a piece for the cut floor, a run to trace, the brush. One at a time: a card for one of them puts
## the others down.
class_name BuildHands
extends RefCounted

const FURNISH_CARDS := ["core", "piece", "turn"]
const PAINT_CARDS := ["brush", "copy_style", "finish"]

var mass: MassDrag
var furnish: FurnishHand
var run: RunHand
var brush: PaintBrush
var primitive := {}
var reference: ReferenceHand

var _ctl: ToolController


func _init(ctl: ToolController) -> void:
	_ctl = ctl
	mass = MassDrag.new(ctl)
	furnish = FurnishHand.new(ctl)
	run = RunHand.new(ctl)
	brush = PaintBrush.new(ctl)
	reference = ReferenceHand.new(ctl)


func armed() -> bool:
	return mass.armed() or furnish.armed() or run.armed() or not primitive.is_empty()


func disarm() -> void:
	mass.disarm()
	furnish.disarm()
	run.disarm()
	primitive = {}


## A card of the strip; true when one of the hands took it.
func act(spec: Dictionary, value: int, picked: Dictionary, report: Callable) -> bool:
	if spec.do != "mass":
		mass.disarm()
	if not FURNISH_CARDS.has(spec.do):
		furnish.disarm()
	if spec.do != "run":
		run.disarm()
	if spec.do != "primitive":
		primitive = {}
	if not PAINT_CARDS.has(spec.do):
		brush.painting = false
	match spec.do:
		"core", "piece", "turn":
			report.call(furnish.act(spec, picked))
		"run":
			run.arm(spec)
			_tell(run.armed(), spec)
		"primitive":
			primitive = {} if primitive == spec else spec
			_tell(not primitive.is_empty(), spec)
		"mass":
			mass.arm(value)
			_tell(mass.armed(), spec)
		"brush", "copy_style":
			brush.act(spec, value, picked)
		"ref_load", "ref_build", "ref_align", "ref_turn", "ref_flip", "ref_fade", "ref_toggle", "ref_remove":
			report.call(reference.act(spec, picked))
		"finish":
			return brush.act(spec, value, picked)
		_:
			return false
	return true


func _tell(armed_now: bool, spec: Dictionary) -> void:
	if armed_now:
		_ctl.hint.emit(_ctl.tr("hint.builder.%s" % spec.id), true)


## A press with something in hand; true when it was for the hand.
func click(p: Vector3, picked: Dictionary, report: Callable) -> bool:
	if not primitive.is_empty():
		report.call(_throw(p))
		return true
	if picked.is_empty():
		return false
	if mass.armed():
		mass.press(p, picked)
	elif furnish.armed():
		report.call(furnish.click(picked.id))
	elif run.armed():
		run.click(p)
	else:
		return false
	return true


## The primitive in hand goes where the pointer's ray lands: on the roof or the wall it hits, or alone on the ground. Each click throws one, until Esc.
func _throw(ground: Vector3) -> Dictionary:
	var cam: Camera3D = _ctl.rig.camera
	var hit: Dictionary = CityStore.query(&"body_pick", [cam.project_ray_origin(_ctl.pointer), cam.project_ray_normal(_ctl.pointer)])
	if hit.is_empty():
		return CityStore.dispatch(&"drop_primitive", [ground, primitive])
	return CityStore.dispatch(&"body_add_primitive", [hit.id, hit.volume, hit.side, hit.storey, hit.point, primitive])


func key(code: Key, picked: Dictionary, report: Callable) -> void:
	if picked.is_empty():
		return
	if furnish.armed() and code == KEY_R:
		report.call(furnish.turn(picked.id))
	elif run.armed() and (code == KEY_ENTER or code == KEY_KP_ENTER):
		report.call(run.finish(picked))
	elif run.armed() and code == KEY_BACKSPACE:
		run.back()
