## What a city holds in memory, by owner: a map opened (with its city or the bare land), drawn until nothing is left to draw, then the
## process's working set and private bytes, Godot's own allocator, the video memory by kind, and the core's census (the land's
## samples, the bodies kept, the heaps). One line a number, so two runs (two map sizes, with and without the city) are read side by
## side: what grows with the map and not with the view is the land's.
## Run: godot --path project res://tests/perf/memory_probe.tscn -- --no-flow --map=0 --city=1 [--seed=7] [--preset=high]
extends TestCase

const SETTLE_FRAMES := 240 # frames drawn before the wait for the render's pending work (StateWait.drawn): the streams ask first
const DRAW_LIMIT_S := 60.0
const MIB := 1048576.0


func _ready() -> void:
	if not boot():
		finish()
		return
	var map := int(_arg("map", "0"))
	var with_city := _arg("city", "1") == "1"
	CityStore.new_map(map, int(_arg("seed", "7")), with_city, true)
	for f in SETTLE_FRAMES:
		await get_tree().process_frame
	await StateWait.drawn(get_tree(), DRAW_LIMIT_S)
	var census: Dictionary = CityStore.query(&"memory_census")
	print("MEMORY map %d city %s" % [map, with_city])
	print("  working set %.0f MiB, private %.0f MiB" % [census.working_set, census.private])
	print("  godot static %.0f MiB" % (Performance.get_monitor(Performance.MEMORY_STATIC) / MIB))
	print("  video %.0f MiB: textures %.0f, buffers %.0f" % [
		RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_VIDEO_MEM_USED) / MIB,
		RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_TEXTURE_MEM_USED) / MIB,
		RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_BUFFER_MEM_USED) / MIB])
	for key: String in census:
		print("  census %s: %s" % [key, census[key]])
	# what the core's allocator holds beyond what lives in it: every thread gives back, and the census is read again
	var threads := int(CityStore.query(&"memory_release"))
	var after: Dictionary = CityStore.query(&"memory_census")
	print("  after %d threads gave back: working set %.0f MiB, private %.0f MiB, core live %.0f MiB, core held %.0f MiB" % [
		threads, after.working_set, after.private, after.core_live, after.core_held])
	print("  after, by thread: %s" % [after.core_by_thread])
	print("  after, by size: %s" % [after.core_by_size])
	check(float(census.working_set) > 0.0, "the census reads the process")
	finish()


static func _arg(name: String, fallback: String) -> String:
	for a in OS.get_cmdline_user_args():
		if a.begins_with("--%s=" % name):
			return a.get_slice("=", 1)
	return fallback
