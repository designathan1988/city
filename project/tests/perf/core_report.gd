## camera_pan's report of what the core says at the end of each movement: the memory and what the render built, then the simulation:
## the fleet and the walkers, what arrived, was lost, removed for standing or released from a jam since the movement before, who waits
## and for what, the near vehicles standing inside another's body, and the process's memory by owner and the video memory by kind.
class_name CoreReport
extends RefCounted

const FLEET_SAMPLE := 1000000 # every near vehicle
const OVERLAPS_SHOWN := 8
const OVERLAP_M := 1.5 # two vehicles' centres closer than this stand inside one another: narrower than any body (a motorbike beside a car is farther)

var lost_before := {}


## The simulation's counters at the end of a movement: the fleet and the walkers, what was lost, removed for standing or released from a
## jam since the last movement, who waits and for what, how many near vehicles stand inside another's body (centres closer than
## OVERLAP_M), and the process's memory by owner.
func report() -> void:
	print("  memory: static %.0f MiB, video %.0f MiB, buffers %.0f MiB | chunks to draw: %d" % [
		Performance.get_monitor(Performance.MEMORY_STATIC) / Units.BYTES_PER_MIB, Performance.get_monitor(Performance.RENDER_VIDEO_MEM_USED) / Units.BYTES_PER_MIB,
		Performance.get_monitor(Performance.RENDER_BUFFER_MEM_USED) / Units.BYTES_PER_MIB, CityStore.query(&"render_pending")])
	print("  largest superchunk HLOD built: %.1f MiB" % float(CityStore.query(&"render_cost").get("super_mb_max", 0.0)))
	print("  buildings: ", JSON.stringify(CityStore.query(&"building_cost")))
	print("  vehicles: ", JSON.stringify(CityStore.query(&"agent_cost")))
	var tp: Dictionary = CityStore.query(&"traffic_probe", [FLEET_SAMPLE])
	var since := {}
	for key: String in ["trips_arrived", "trips_lost", "released", "reroutes", "red_entries", "people_arrived"]:
		since[key] = int(tp[key]) - int(lost_before.get(key, 0))
		lost_before[key] = int(tp[key])
	var at: Array = tp.sample
	var cells := {}
	for i in at.size():
		var c := Vector2i(floori(at[i].x / OVERLAP_M), floori(at[i].z / OVERLAP_M))
		if not cells.has(c):
			cells[c] = []
		cells[c].append(i)
	var inside := 0
	for i in at.size():
		var c := Vector2i(floori(at[i].x / OVERLAP_M), floori(at[i].z / OVERLAP_M))
		for dx in [-1, 0, 1]:
			for dz in [-1, 0, 1]:
				for j: int in cells.get(c + Vector2i(dx, dz), []):
					if j > i and at[i].distance_to(at[j]) < OVERLAP_M:
						inside += 1
						if inside <= OVERLAPS_SHOWN: # where and how each stands: on a lane (metres from its nearer end), or 0 on a connector or off the lanes
							print("    inside one another: %s and %s, %.2f m apart | lane mid %.1f / %.1f m | %.1f / %.1f m/s | types %d / %d" % [
								at[i], at[j], at[i].distance_to(at[j]), tp.sample_mid_m[i], tp.sample_mid_m[j], tp.sample_speed[i], tp.sample_speed[j], tp.sample_type[i], tp.sample_type[j]])
	print("  fleet: %s vehicles (%s near sampled, %s moving, %s waiting, %s on connectors), %s people (%s waiting, %s inside another, %s in furniture)" % [
		tp.vehicles, at.size(), tp.moving, tp.waiting, tp.on_connectors, tp.people, tp.people_waiting, tp.people_overlaps, tp.people_in_furniture])
	print("  since the last movement: ", JSON.stringify(since), " | longest a near vehicle has stood: %.0f s" % float(tp.stuck_longest_s))
	print("  held by: %s | queued behind: %s | box stops: %s | vehicle pairs inside one another: %d" % [tp.held_by, tp.queued_behind, tp.box_stops, inside])
	var mem: Dictionary = CityStore.query(&"memory_census")
	mem.erase("mimalloc")
	print("  memory by owner (MiB): ", JSON.stringify(mem))
	print("  video by kind (MiB): texture %.0f, buffer %.0f" % [
		RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_TEXTURE_MEM_USED) / Units.BYTES_PER_MIB,
		RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_BUFFER_MEM_USED) / Units.BYTES_PER_MIB])
