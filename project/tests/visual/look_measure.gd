## The measures a look plan takes (tests/visual/look.gd), as the Roadcraft 4.0's look.mjs takes them, from the engine's own
## counters: the frames and the GPU's stamps (game/lab/frame_meter.gd), samples of what the process uses (usage) and what one
## frame drew (info). Every number comes from Godot's servers: RenderingServer (the viewport's render info), Performance (memory,
## objects, pipeline compilations).
class_name LookMeasure
extends FrameMeter

const BYTES_PER_MB := 1048576.0
const USAGE_EVERY_MS := 250.0
const USAGE_SECONDS := 4.0


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
		"pipelines": pipelines(),
		"nodes": Performance.get_monitor(Performance.OBJECT_NODE_COUNT),
	}
