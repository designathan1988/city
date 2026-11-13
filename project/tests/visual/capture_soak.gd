## Traffic soak of the visual capture (--soak=N): N simulated minutes, one traffic_probe line per minute (fleet, share
## held over 20 s, why the heads are held, trips arrived and lost, walkers in each other or in the street furniture, the
## core's time for the minute), then the jams still held. Vehicles standing in a junction and walkers in each other or in
## the furniture are counted once a second and summed over the minute (one look a minute sees almost none of them).
class_name CaptureSoak
extends RefCounted

const TICKS_PER_SECOND := 20
const SECONDS_PER_MINUTE := 60
const SOAK_JAMS := 24
const USEC_PER_MS := 1000.0


static func report(minutes: int) -> void:
	for m in minutes:
		var box := []
		var held_seconds := 0
		var overlap_seconds := [0, 0]
		var furniture_seconds := 0
		var spent := 0.0
		var p: Dictionary
		for second in SECONDS_PER_MINUTE:
			var started := Time.get_ticks_usec()
			CityStore.step_ticks(TICKS_PER_SECOND)
			spent += (Time.get_ticks_usec() - started) / USEC_PER_MS
			p = CityStore.query(&"traffic_probe", [0])
			held_seconds += int(p.waiting)
			for k in overlap_seconds.size():
				overlap_seconds[k] += int(p.people_overlaps[k])
			furniture_seconds += int(p.get("people_in_furniture", 0))
			var now: Array = p.get("box_stops", [])
			if box.is_empty():
				box.resize(now.size())
				box.fill(0)
			for k in now.size():
				box[k] += int(now[k])
		var share: float = 100.0 * p.waiting / maxf(1.0, p.vehicles)
		print("soak min %d: %d vehicles, %d moving, %d held (%.1f %%), stopped by %s, arrivals %d, lost %d, people %d" % [
			m + 1, p.vehicles, p.moving, p.waiting, share, p.held_by, p.trips_arrived, p.trips_lost, p.people])
		print("  detours %d, freed %d, governor %.2f, lost to gridlock %d, people arrived %d, waiting at the kerb %d" % [
			p.get("reroutes", 0), p.get("released", 0), p.get("governor", 1.0), p.get("lost_stuck", 0), p.get("people_arrived", 0),
			p.get("people_waiting", 0)])
		print("  held on average in the minute: %.1f" % (float(held_seconds) / SECONDS_PER_MINUTE))
		print("  lots: %d cars (%d on the way, %d manoeuvring), called %d, parked %d, left %d; scenery parked %d, bays %d" % [
			p.get("lot_cars", 0), p.get("lot_coming", 0), p.get("lot_moving", 0), p.get("lot_called", 0), p.get("lot_parked_in", 0),
			p.get("lot_left_by", 0), p.get("parked_rows", 0), p.get("bays", 0)])
		print("  step, ms summed (bays, fleet, lots, pedestrians, glance: exits, calls, publishing): ", p.get("step_ms", []), "; last entry on red (kind, m/s, ds, time): ", p.get("red_last", []))
		var ticks := TICKS_PER_SECOND * SECONDS_PER_MINUTE
		print("  queued behind: ", p.get("queued_behind", []), ", core %.0f ms in the minute (%.3f ms/step)" % [spent, spent / ticks])
		print("  pedestrians overlapping another (walking, waiting): ", p.get("people_overlaps", []), ", on average in the minute: %.1f, %.1f" % [
			float(overlap_seconds[0]) / SECONDS_PER_MINUTE, float(overlap_seconds[1]) / SECONDS_PER_MINUTE],
			"; inside the pavement's furniture: %d, pedestrian-seconds in the minute: %d" % [p.get("people_in_furniture", 0),
			furniture_seconds])
		print("  stopped at the junction, vehicle-seconds in the minute (behind another, for pedestrians, for a signal ahead, starting): ", box,
			", entries on red %d (%d from a stop)" % [p.get("red_entries", 0), p.get("red_from_stop", 0)])
	var last: Dictionary = CityStore.query(&"traffic_probe", [SOAK_JAMS])
	print("  street furniture (signals, poles, hydrants, bins, benches, trees): ", last.get("street_furniture", []))
	for line in last.jam_info:
		print("  held: ", line)
