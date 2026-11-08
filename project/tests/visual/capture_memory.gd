## Memory report shared by the visual captures (system, static, video and buffer memory), cross-platform.
class_name CaptureMemory
extends RefCounted



static func report(stage: String) -> void:
	var info := OS.get_memory_info()
	var system_used := (float(info.physical) - float(info.free)) / Units.BYTES_PER_MIB
	print("memory [", stage, "]: sistema usado ", snappedf(system_used, 0.1), " MiB, static ",
		snappedf(OS.get_static_memory_usage() / Units.BYTES_PER_MIB, 0.1), " MiB, video ",
		snappedf(Performance.get_monitor(Performance.RENDER_VIDEO_MEM_USED) / Units.BYTES_PER_MIB, 0.1), " MiB, buffers ",
		snappedf(Performance.get_monitor(Performance.RENDER_BUFFER_MEM_USED) / Units.BYTES_PER_MIB, 0.1), " MiB")
