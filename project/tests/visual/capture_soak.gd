## Traffic soak of the visual capture (--soak=N): N simulated minutes, one traffic_probe line per minute (fleet, share
## held over 20 s, why the heads are held, trips arrived and lost), then the jams still held. Vehicles standing in a
## junction are counted once a second and summed over the minute (one look a minute sees almost none of them).
class_name CaptureSoak
extends RefCounted

const TICKS_PER_SECOND := 20
const SECONDS_PER_MINUTE := 60
const SOAK_JAMS := 24


static func report(minutes: int) -> void:
	for m in minutes:
		var box := []
		var held_seconds := 0
		var p: Dictionary
		for second in SECONDS_PER_MINUTE:
			CityStore.step_ticks(TICKS_PER_SECOND)
			p = CityStore.query(&"traffic_probe", [0])
			held_seconds += int(p.waiting)
			var now: Array = p.get("box_stops", [])
			if box.is_empty():
				box.resize(now.size())
				box.fill(0)
			for k in now.size():
				box[k] += int(now[k])
		var share: float = 100.0 * p.waiting / maxf(1.0, p.vehicles)
		print("soak min %d: %d veículos, %d andando, %d retidos (%.1f %%), parados por %s, chegadas %d, perdidas %d, pessoas %d" % [
			m + 1, p.vehicles, p.moving, p.waiting, share, p.held_by, p.trips_arrived, p.trips_lost, p.people])
		print("  desvios %d, liberados %d, governador %.2f, perdidas por travamento %d, pessoas chegadas %d, esperando na guia %d" % [
			p.get("reroutes", 0), p.get("released", 0), p.get("governor", 1.0), p.get("lost_stuck", 0), p.get("people_arrived", 0),
			p.get("people_waiting", 0)])
		print("  retidos em média no minuto: %.1f" % (float(held_seconds) / SECONDS_PER_MINUTE))
		print("  na fila atrás de: ", p.get("queued_behind", []))
		print("  pedestres sobrepostos a outro (andando, esperando): ", p.get("people_overlaps", []))
		print("  parados no cruzamento, veículo-segundos no minuto (atrás de outro, por pedestres, partindo): ", box,
			", entradas no vermelho %d (%d paradas)" % [p.get("red_entries", 0), p.get("red_from_stop", 0)])
	for line in CityStore.query(&"traffic_probe", [SOAK_JAMS]).jam_info:
		print("  retido: ", line)
