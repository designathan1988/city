## The measures a look plan takes (tests/visual/look.gd), as the Roadcraft 4.0's look.mjs takes them, from the engine's own
## counters: a recording of the frames (record), samples of what the process uses (usage), the GPU's passes (passes) and what
## one frame drew (info). Every number comes from Godot's servers: RenderingServer (the viewport's measured times and render
## info, the device's captured timestamps), Performance (memory, objects, pipeline compilations).
class_name LookMeasure
extends Node

## A frame over this is a hitch the player sees (look.mjs frameStats).
const HITCH_MS := 25.0
## The display's period is read off the intervals: their 10th percentile (no frame comes faster than the display), kept
## between these refresh rates; 60 Hz otherwise.
const FASTEST_HZ := 240.0
const SLOWEST_HZ := 30.0
const DEFAULT_HZ := 60.0
const FAST_SHARE := 0.1
const US_PER_MS := 1000.0
const MS_PER_S := 1000.0
const BYTES_PER_MB := 1048576.0
const PERCENTILES := [0.5, 0.95, 0.99]
const USAGE_EVERY_MS := 250.0
const USAGE_SECONDS := 4.0

var _viewport: RID
var _recording := false
var _last_us := 0
var _gaps: PackedFloat64Array
var _cpu: PackedFloat64Array
var _gpu: PackedFloat64Array


func _init(viewport: Viewport) -> void:
	_viewport = viewport.get_viewport_rid()
	RenderingServer.viewport_set_measure_render_time(_viewport, true)


func _process(_delta: float) -> void:
	if not _recording:
		return
	var now := Time.get_ticks_usec()
	if _last_us > 0:
		_gaps.append(float(now - _last_us) / US_PER_MS)
		_cpu.append(RenderingServer.viewport_get_measured_render_time_cpu(_viewport))
		_gpu.append(RenderingServer.viewport_get_measured_render_time_gpu(_viewport))
	_last_us = now


func record() -> void:
	_gaps = PackedFloat64Array()
	_cpu = PackedFloat64Array()
	_gpu = PackedFloat64Array()
	_last_us = 0
	_recording = true


## The frames since `record`: intervals (p50, p95, p99, max), hitches, vsyncs missed, and the viewport's render times.
func record_end() -> Dictionary:
	_recording = false
	var gaps := _sorted(_gaps)
	var fast: float = gaps[int(gaps.size() * FAST_SHARE)] if gaps.size() > 0 else 0.0
	var vsync := fast if fast >= MS_PER_S / FASTEST_HZ and fast <= MS_PER_S / SLOWEST_HZ else MS_PER_S / DEFAULT_HZ
	var dropped := 0
	var hitches := 0
	for g in _gaps:
		dropped += maxi(0, roundi(g / vsync) - 1)
		if g > HITCH_MS:
			hitches += 1
	var out := {"frames": _gaps.size(), "hitches": hitches, "dropped": dropped, "vsync_ms": snappedf(vsync, 0.01)}
	out.merge(_spread("interval", gaps))
	out.merge(_spread("cpu", _sorted(_cpu)))
	out.merge(_spread("gpu", _sorted(_gpu)))
	return out


## What the process uses over `seconds` (every USAGE_EVERY_MS): each figure's peak, mean and least, not one last reading.
func usage(host: Node, seconds: float = USAGE_SECONDS) -> Dictionary:
	var samples: Array[Dictionary] = []
	var t0 := Time.get_ticks_msec()
	while Time.get_ticks_msec() - t0 < seconds * MS_PER_S:
		samples.append(info())
		await host.get_tree().create_timer(USAGE_EVERY_MS / MS_PER_S).timeout
	var out := {"samples": samples.size()}
	for key: String in samples[0].keys():
		var values := PackedFloat64Array()
		for s in samples:
			values.append(float(s[key]))
		var sorted := _sorted(values)
		var sum := 0.0
		for v in values:
			sum += v
		out[key] = {"peak": sorted[-1], "mean": snappedf(sum / values.size(), 0.01), "least": sorted[0]}
	return out


## What the last frame drew and what the process holds: the viewport's objects, primitives and draws, seen and in the
## shadows apart, the memory and the pipelines compiled so far, the frame's CPU and GPU time.
func info() -> Dictionary:
	var rs := RenderingServer
	var seen := rs.VIEWPORT_RENDER_INFO_TYPE_VISIBLE
	var shadow := rs.VIEWPORT_RENDER_INFO_TYPE_SHADOW
	return {
		"draws": rs.viewport_get_render_info(_viewport, seen, rs.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME),
		"primitives": rs.viewport_get_render_info(_viewport, seen, rs.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME),
		"objects": rs.viewport_get_render_info(_viewport, seen, rs.VIEWPORT_RENDER_INFO_OBJECTS_IN_FRAME),
		"shadow_draws": rs.viewport_get_render_info(_viewport, shadow, rs.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME),
		"shadow_primitives": rs.viewport_get_render_info(_viewport, shadow, rs.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME),
		"cpu_ms": rs.viewport_get_measured_render_time_cpu(_viewport),
		"gpu_ms": rs.viewport_get_measured_render_time_gpu(_viewport),
		"video_mb": rs.get_rendering_info(rs.RENDERING_INFO_VIDEO_MEM_USED) / BYTES_PER_MB,
		"texture_mb": rs.get_rendering_info(rs.RENDERING_INFO_TEXTURE_MEM_USED) / BYTES_PER_MB,
		"buffer_mb": rs.get_rendering_info(rs.RENDERING_INFO_BUFFER_MEM_USED) / BYTES_PER_MB,
		"static_mb": Performance.get_monitor(Performance.MEMORY_STATIC) / BYTES_PER_MB,
		"pipelines": rs.get_rendering_info(rs.RENDERING_INFO_PIPELINE_COMPILATIONS_SURFACE)
				+ rs.get_rendering_info(rs.RENDERING_INFO_PIPELINE_COMPILATIONS_DRAW)
				+ rs.get_rendering_info(rs.RENDERING_INFO_PIPELINE_COMPILATIONS_SPECIALIZATION),
		"nodes": Performance.get_monitor(Performance.OBJECT_NODE_COUNT),
	}


## The GPU's passes of the last captured frame: the device's named timestamps (what the editor's visual profiler reads),
## each pass the time to the next one. Empty without a rendering device (the Compatibility renderer).
func passes() -> Array[Dictionary]:
	var out: Array[Dictionary] = []
	var rd := RenderingServer.get_rendering_device()
	if rd == null:
		return out
	var n := rd.get_captured_timestamps_count()
	for i in range(n - 1):
		out.append({"pass": rd.get_captured_timestamp_name(i),
			"gpu_ms": snappedf(float(rd.get_captured_timestamp_gpu_time(i + 1) - rd.get_captured_timestamp_gpu_time(i)) / US_PER_MS, 0.001),
			"cpu_ms": snappedf(float(rd.get_captured_timestamp_cpu_time(i + 1) - rd.get_captured_timestamp_cpu_time(i)) / US_PER_MS, 0.001)})
	return out


static func _sorted(values: PackedFloat64Array) -> PackedFloat64Array:
	var out := values.duplicate()
	out.sort()
	return out


static func _spread(name: String, sorted: PackedFloat64Array) -> Dictionary:
	var out := {}
	if sorted.is_empty():
		return out
	for p: float in PERCENTILES:
		out["%s_p%d" % [name, roundi(p * 100)]] = snappedf(sorted[mini(sorted.size() - 1, ceili(sorted.size() * p) - 1)], 0.01)
	out["%s_max" % name] = snappedf(sorted[-1], 0.01)
	return out
