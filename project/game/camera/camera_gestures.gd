## The camera's mouse gestures, the left button stays the
## tools': right drag turns and tilts about the ground under the press (a turntable: the near side follows the hand,
## dragging down lifts towards a plan view); middle drag or Shift + right drag grabs the ground and drags it (the point
## under the cursor stays under it; past the hold reach it slides). A turn is spent on a short spring, `orbit_smooth_s`: the mouse gives whole
## pixels, unevenly a frame, and a turn about a far point would swing the near view by each of them. A right press that moves less than
## `click_slop_px` is a click (CityCamera.right_clicked). Let go while moving, the camera coasts and slows down. The hand is timed on
## the camera's own clock, the frames' deltas summed (`advance`), the one time base its springs and coasts run on: a second clock
## (the wall's) could call a hand that stood still for a while of frames a moving one, and throw the view.
class_name CameraGestures
extends RefCounted

const MS_PER_S := 1000.0
const TWIST_SIGN := -1.0 # the map turns with the fingers

var rig: CityCamera
var cfg: Dictionary
var rad_per_px := 0.0
## The drag in progress: kind (&"orbit" / &"pan"), button, pressed and last pointer, ground height held, grabbed
## ground point (null: slide), moved past the click slop.
var drag := {}
## The drag's recent course: Vector3(x, y, time ms).
var samples: Array[Vector3] = []
var _fingers := {} # touch index -> where it is
var _pinch := {} # the last spread between two fingers
## The coast after a drag let go: kind, velocity (px/s or ground m/s), pointer speed px/s, pivot and its height.
var coast := {}
## The turn the hand asked for and the view has not made yet, radians, and what it turns about.
var turn_left := Vector2.ZERO
var turn_pivot := Vector2.ZERO
var turn_height := 0.0
var _clock_ms := 0.0 # the frames' deltas summed (advance)


func _init(camera_rig: CityCamera) -> void:
	rig = camera_rig
	cfg = rig.cfg
	rad_per_px = 1.0 / cfg.orbit_px_per_rad


func active() -> bool:
	return not drag.is_empty()


func cancel() -> void:
	drag = {}
	coast = {}
	turn_left = Vector2.ZERO
	samples.clear()


## A press over the map (unhandled input): starts a turn or a pan.
func press(event: InputEvent) -> bool:
	if not (event is InputEventMouseButton and event.pressed) or active():
		return false
	var b: MouseButton = event.button_index
	var at: Vector2 = event.position
	var pan: bool = b == MOUSE_BUTTON_MIDDLE or (b == MOUSE_BUTTON_RIGHT and event.shift_pressed)
	if b != MOUSE_BUTTON_RIGHT and not pan:
		return false
	coast = {}
	var h := rig.height_under(at)
	drag = {"kind": &"pan" if pan else &"orbit", "button": b, "pressed": at, "last": at, "height": h,
			"moved": false, "grabbed": null}
	if pan and CameraAim.holds(rig, at, h):
		drag.grabbed = rig.hit_at(at, h)
	samples = [Vector3(at.x, at.y, _now())]
	return true


## While a drag is held the pointer is its own, over the interface too: moves and the release.
func captured(event: InputEvent) -> bool:
	if not active():
		return false
	if event is InputEventMouseMotion:
		move(event.position)
		return true
	if event is InputEventMouseButton and not event.pressed and event.button_index == drag.button:
		release()
		return true
	return false


func move(at: Vector2) -> void:
	_track(at)
	if drag.kind == &"orbit":
		if not drag.moved:
			if at.distance_to(drag.pressed) < cfg.click_slop_px:
				return
			drag.moved = true
		var d: Vector2 = at - drag.last
		drag.last = at
		turn_left += Vector2(-d.x, d.y) * rad_per_px
		turn_pivot = drag.pressed
		turn_height = drag.height
		return
	var dp: Vector2 = at - drag.last
	drag.last = at
	if drag.grabbed != null:
		rig.pan_to(drag.grabbed, at, drag.height)
	else:
		var unit := 1.0 / rig.px_per_m()
		rig.slide(-dp.x * unit, dp.y * unit)


func release() -> void:
	if drag.kind == &"orbit" and not drag.moved:
		drag = {}
		rig.right_clicked.emit()
		return
	_throw()
	drag = {}


func _now() -> float:
	return _clock_ms


func _track(at: Vector2) -> void:
	var t := maxf(_now(), samples[-1].z if not samples.is_empty() else 0.0)
	samples.append(Vector3(at.x, at.y, t))
	while samples.size() > 2 and t - samples[0].z > cfg.speed_horizon_ms:
		samples.pop_front()


## The hand's speed as it let go, px/s: the slope of a least-squares line through the last samples.
func _hand_speed() -> Vector2:
	if samples.size() < 2 or samples[-1].z - samples[0].z < cfg.speed_min_span_ms:
		return Vector2.ZERO
	var t0 := samples[0].z
	var st := 0.0; var stt := 0.0; var sp := Vector2.ZERO; var stp := Vector2.ZERO
	for s in samples:
		var t := (s.z - t0) / MS_PER_S
		st += t; stt += t * t; sp += Vector2(s.x, s.y); stp += Vector2(s.x, s.y) * t
	var n := float(samples.size())
	var den := n * stt - st * st
	if absf(den) < CityCamera.PARALLEL:
		return Vector2.ZERO
	return (stp * n - sp * st) / den


## Two fingers: their spread zooms about the point between them and their turn turns the map with them. True when the event was theirs.
func touch(event: InputEvent) -> bool:
	var t := event as InputEventScreenTouch
	var d := event as InputEventScreenDrag
	if t != null:
		if t.pressed:
			_fingers[t.index] = t.position
		else:
			_fingers.erase(t.index)
		_pinch = {}
		return _fingers.size() >= 2
	if d == null or not _fingers.has(d.index):
		return false
	_fingers[d.index] = d.position
	if _fingers.size() != 2:
		return false
	pinch()
	return true


func pinch() -> void:
	var at: Array = _fingers.values()
	var span: Vector2 = at[1] - at[0]
	var mid: Vector2 = (at[0] + at[1]) * 0.5
	if not _pinch.is_empty() and span.length() > 0.0:
		rig.zoom_at(mid, span.length() / _pinch.span.length(), rig.height_under(mid))
		if span.length() >= cfg.pinch_min_spread_px:
			rig.orbit(_pinch.span.angle_to(span) * TWIST_SIGN, 0.0)
	_pinch = {"span": span}


## A drag let go: the camera keeps the hand's speed and slows down (`coast_decay_s`).
func _throw() -> void:
	var moving: bool = not samples.is_empty() and _now() - samples[-1].z < cfg.speed_stopped_ms
	var v := _hand_speed().limit_length(cfg.max_fling_px_s)
	samples.clear()
	if not moving:
		return
	if drag.kind == &"orbit":
		if v.length() * rad_per_px > cfg.coast_min_orbit:
			coast = {"kind": &"orbit", "v": v * rad_per_px, "px": v.length(), "pivot": drag.pressed,
					"height": drag.height}
		return
	if v.length() <= cfg.coast_min_pan_px:
		return
	if drag.grabbed == null:
		coast = {"kind": &"slide", "v": v, "px": v.length()}
		return
	# The ground under the hand moved at J v (the screen-to-ground Jacobian at the pointer).
	var p: Vector2 = drag.last
	var h: float = drag.height
	var ax: Variant = rig.hit_at(p - Vector2.RIGHT, h)
	var bx: Variant = rig.hit_at(p + Vector2.RIGHT, h)
	var ay: Variant = rig.hit_at(p - Vector2.DOWN, h)
	var by: Variant = rig.hit_at(p + Vector2.DOWN, h)
	if ax == null or bx == null or ay == null or by == null:
		return
	var g: Vector3 = ((bx - ax) * v.x + (by - ay) * v.y) * 0.5
	coast = {"kind": &"pan", "v": -g, "px": v.length()}


## How much of a tilt there is room for: it eases into the limit it goes to, so the turn does not stop dead on it.
func _tilt_room(towards: float) -> float:
	var limit: float = deg_to_rad(cfg.max_tilt_deg) if towards > 0.0 else CameraProfile.min_tilt(cfg, rig.half)
	return clampf(absf(limit - rig.pitch) / deg_to_rad(cfg.tilt_ease_deg), 0.0, 1.0)


## One frame of what the hand left: the turn still to make, then the coast, `dt` seconds.
func advance(dt: float) -> void:
	_clock_ms += dt * MS_PER_S
	if turn_left != Vector2.ZERO:
		var keep := exp(-dt / cfg.orbit_smooth_s)
		var spent := turn_left * (1.0 - keep) if turn_left.length() > cfg.coast_min_orbit * dt else turn_left
		turn_left -= spent
		rig.orbit(spent.x, spent.y * _tilt_room(spent.y), turn_pivot, turn_height)
	if coast.is_empty():
		return
	var orbit: bool = coast.kind == &"orbit"
	var least: float = cfg.coast_min_orbit / rad_per_px if orbit else cfg.coast_min_pan_px
	var left: float = cfg.coast_decay_s * log(maxf(1.0, coast.px / least))
	var seconds := minf(dt, left)
	var keep := exp(-seconds / cfg.coast_decay_s)
	var span: float = cfg.coast_decay_s * (1.0 - keep)
	if orbit:
		rig.orbit(-coast.v.x * span, coast.v.y * span, coast.pivot, coast.height)
	elif coast.kind == &"pan":
		rig.focus += Vector3(coast.v.x, 0.0, coast.v.z) * span
		rig.slide(0.0, 0.0)
	else:
		var units := span / rig.px_per_m()
		rig.slide(-coast.v.x * units, coast.v.y * units)
	coast.v *= keep
	coast.px *= keep
	if seconds >= left:
		coast = {}
