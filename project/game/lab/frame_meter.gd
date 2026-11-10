## The frames as the engine counts them, the one owner of those readings (the F7 panel, the Diagnosis, the look plans): a window
## of frames (intervals, CPU, GPU, what was drawn) and, while the clock is on, what each stage of the 3D frame took on the GPU.
class_name FrameMeter
extends Node

## A frame over this is a hitch the player sees (look.mjs frameStats).
const HITCH_MS := 25.0
## The display's period is read off the intervals: their 10th percentile (no frame comes faster than the display), kept
## between these periods (past a 250 Hz panel, and a 30 Hz one); a 60 Hz panel's otherwise.
const SHORTEST_PERIOD_MS := 4.0
const LONGEST_PERIOD_MS := MS_PER_S / 30.0
const USUAL_PERIOD_MS := MS_PER_S / 60.0
const FAST_SHARE := 0.1
const US_PER_MS := 1000.0
const NS_PER_MS := 1000000.0 # the device gives the GPU's stamps in nanoseconds, the CPU's in microseconds
const MS_PER_S := 1000.0
const PERCENTILES := [0.5, 0.95, 0.99]
const PERCENT := 100
## What a report keeps of a time: hundredths of a millisecond, thousandths for a single stamp.
const SHOWN_MS := 0.01
const STAMP_SHOWN_MS := 0.001
## The engine's own stamp on a measured viewport, before its id; a band that starts there has no clock of its own.
const VIEWPORT_STAMP := "vp_begin_"
const VIEWPORT_BAND := "begin"
const STAGE_CONSTANT := "EFFECT_CALLBACK_TYPE_"

var _viewport: RID
var _recording := false
var _last_us := 0
var _gaps: PackedFloat64Array
var _cpu: PackedFloat64Array
var _gpu: PackedFloat64Array
var _draws: PackedFloat64Array
var _primitives: PackedFloat64Array
var _stages := {} # stage id -> ms of each frame
var _stamps := {} # a stamp's name -> the stage that starts there
var _camera: Camera3D
var _stamped_frame := -1


func _init(viewport: Viewport) -> void:
	_viewport = viewport.get_viewport_rid()
	RenderingServer.viewport_set_measure_render_time(_viewport, true)


## The stages' clock on `camera`: `bands` are the rows of `doctor.stages`, each one from its stamp to the next. Nothing without a
## rendering device (the Compatibility renderer has no stamps).
func clock_on(camera: Camera3D, bands: Array) -> void:
	if RenderingServer.get_rendering_device() == null or camera == null:
		return
	var effects: Array[CompositorEffect] = []
	_stamps = {}
	for band: Dictionary in bands:
		if band.from == VIEWPORT_BAND:
			_stamps[VIEWPORT_STAMP + str(_viewport.get_id())] = band.id
			continue
		var stage := ClassDB.class_get_integer_constant(&"CompositorEffect", STAGE_CONSTANT + band.from)
		effects.append(PassClock.new(stage, band.from))
		_stamps[PassClock.PREFIX + band.from] = band.id
	var compositor := Compositor.new()
	compositor.compositor_effects = effects
	camera.compositor = compositor
	_camera = camera


func clock_off() -> void:
	if _camera != null:
		_camera.compositor = null
	_camera = null
	_stamps = {}


func _process(_delta: float) -> void:
	if not _recording:
		return
	var now := Time.get_ticks_usec()
	if _last_us > 0:
		_gaps.append(float(now - _last_us) / US_PER_MS)
		_cpu.append(render_cpu_ms(_viewport))
		var gpu := RenderingServer.viewport_get_measured_render_time_gpu(_viewport)
		if gpu > 0.0:
			_gpu.append(gpu)
		_read_drawn()
		_read_stages()
	_last_us = now


func _read_drawn() -> void:
	var rs := RenderingServer
	var seen := rs.VIEWPORT_RENDER_INFO_TYPE_VISIBLE
	var shadow := rs.VIEWPORT_RENDER_INFO_TYPE_SHADOW
	_draws.append(rs.viewport_get_render_info(_viewport, seen, rs.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME)
			+ rs.viewport_get_render_info(_viewport, shadow, rs.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME))
	_primitives.append(rs.viewport_get_render_info(_viewport, seen, rs.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME)
			+ rs.viewport_get_render_info(_viewport, shadow, rs.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME))


## The stamps come from a frame already finished, and the same one can be read twice: each frame counts once.
func _read_stages() -> void:
	var rd := RenderingServer.get_rendering_device()
	if rd == null or _stamps.is_empty() or rd.get_captured_timestamps_frame() == _stamped_frame:
		return
	_stamped_frame = rd.get_captured_timestamps_frame()
	for i in range(rd.get_captured_timestamps_count() - 1):
		var stage: String = _stamps.get(rd.get_captured_timestamp_name(i), "")
		if stage != "":
			var spent := rd.get_captured_timestamp_gpu_time(i + 1) - rd.get_captured_timestamp_gpu_time(i)
			_stages[stage].append(float(spent) / NS_PER_MS)


func record() -> void:
	_gaps = PackedFloat64Array()
	_cpu = PackedFloat64Array()
	_gpu = PackedFloat64Array()
	_draws = PackedFloat64Array()
	_primitives = PackedFloat64Array()
	_stages = {}
	for stage: String in _stamps.values():
		_stages[stage] = PackedFloat64Array()
	_last_us = 0
	_recording = true


## The window since `record`, frame by frame.
func frames() -> Dictionary:
	_recording = false
	return {"interval": _gaps, "cpu": _cpu, "gpu": _gpu, "draws": _draws, "primitives": _primitives, "stages": _stages}


## The frames since `record`, summed up: intervals (p50, p95, p99, max), hitches, vsyncs missed, and the render times.
func record_end() -> Dictionary:
	_recording = false
	var gaps := _sorted(_gaps)
	var fast: float = gaps[int(gaps.size() * FAST_SHARE)] if gaps.size() > 0 else 0.0
	var vsync := fast if fast >= SHORTEST_PERIOD_MS and fast <= LONGEST_PERIOD_MS else USUAL_PERIOD_MS
	var dropped := 0
	var hitches := 0
	for g in _gaps:
		dropped += maxi(0, roundi(g / vsync) - 1)
		if g > HITCH_MS:
			hitches += 1
	var out := {"frames": _gaps.size(), "hitches": hitches, "dropped": dropped, "vsync_ms": snappedf(vsync, SHOWN_MS)}
	out.merge(_spread("interval", gaps))
	out.merge(_spread("cpu", _sorted(_cpu)))
	out.merge(_spread("gpu", _sorted(_gpu)))
	return out


## The GPU's stamps of the last captured frame, each the time to the next one. Empty without a rendering device.
func passes() -> Array[Dictionary]:
	var out: Array[Dictionary] = []
	var rd := RenderingServer.get_rendering_device()
	if rd == null:
		return out
	for i in range(rd.get_captured_timestamps_count() - 1):
		out.append({"pass": rd.get_captured_timestamp_name(i),
			"gpu_ms": snappedf(float(rd.get_captured_timestamp_gpu_time(i + 1) - rd.get_captured_timestamp_gpu_time(i)) / NS_PER_MS, STAMP_SHOWN_MS),
			"cpu_ms": snappedf(float(rd.get_captured_timestamp_cpu_time(i + 1) - rd.get_captured_timestamp_cpu_time(i)) / US_PER_MS, STAMP_SHOWN_MS)})
	return out


## The render CPU of the last frame: the core's _process (render_cost "process": the render's sync, uploads and agents), the
## renderer's drawing of the viewport and its frame setup. Not the whole CPU: the scripts' own work and the simulation on its
## thread are outside it. Performance.TIME_PROCESS will not do: it is the worst frame of the second (main/main.cpp process_max), so
## a single slow frame reads as the cost of all of them.
static func render_cpu_ms(viewport: RID) -> float:
	return float(CityStore.query(&"render_cost").get("process", 0.0)) + RenderingServer.viewport_get_measured_render_time_cpu(viewport) \
			+ RenderingServer.get_frame_setup_time_cpu()


## Pipelines compiled so far: a change that makes new ones is still settling.
static func pipelines() -> int:
	var rs := RenderingServer
	return rs.get_rendering_info(rs.RENDERING_INFO_PIPELINE_COMPILATIONS_SURFACE) \
			+ rs.get_rendering_info(rs.RENDERING_INFO_PIPELINE_COMPILATIONS_DRAW) \
			+ rs.get_rendering_info(rs.RENDERING_INFO_PIPELINE_COMPILATIONS_SPECIALIZATION)


static func _sorted(values: PackedFloat64Array) -> PackedFloat64Array:
	var out := values.duplicate()
	out.sort()
	return out


static func _spread(name: String, sorted: PackedFloat64Array) -> Dictionary:
	var out := {}
	if sorted.is_empty():
		return out
	for p: float in PERCENTILES:
		out["%s_p%d" % [name, roundi(p * PERCENT)]] = snappedf(sorted[mini(sorted.size() - 1, ceili(sorted.size() * p) - 1)], SHOWN_MS)
	out["%s_max" % name] = snappedf(sorted[-1], SHOWN_MS)
	return out
