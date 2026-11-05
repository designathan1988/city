## Memory report shared by the visual captures (system, static, video and buffer memory), cross-platform.
class_name CaptureMemory
extends RefCounted

const MIB := 1024.0 * 1024.0


static func report(stage: String) -> void:
	var info := OS.get_memory_info()
	var system_used := (float(info.physical) - float(info.free)) / MIB
	print("memória [", stage, "]: sistema usado ", snappedf(system_used, 0.1), " MiB, estática ",
		snappedf(OS.get_static_memory_usage() / MIB, 0.1), " MiB, vídeo ",
		snappedf(Performance.get_monitor(Performance.RENDER_VIDEO_MEM_USED) / MIB, 0.1), " MiB, buffers ",
		snappedf(Performance.get_monitor(Performance.RENDER_BUFFER_MEM_USED) / MIB, 0.1), " MiB")
