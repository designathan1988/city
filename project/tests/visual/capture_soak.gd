## Traffic soak of the visual capture (--soak=N): N simulated minutes, one traffic_probe line per minute (fleet, share
## held over 20 s, why the heads are held, trips arrived and lost), then the jams still held.
class_name CaptureSoak
extends RefCounted

const TICKS_PER_MINUTE := 1200
const SOAK_JAMS := 24


static func report(minutes: int) -> void:
	for m in minutes:
		CityStore.step_ticks(TICKS_PER_MINUTE)
		var p: Dictionary = CityStore.query(&"traffic_probe", [0])
		var share: float = 100.0 * p.waiting / maxf(1.0, p.vehicles)
		print("soak min %d: %d veículos, %d andando, %d retidos (%.1f %%), parados por %s, chegadas %d, perdidas %d, pessoas %d" % [
			m + 1, p.vehicles, p.moving, p.waiting, share, p.held_by, p.trips_arrived, p.trips_lost, p.people])
		print("  desvios %d, liberados %d, governador %.2f, perdidas por travamento %d, pessoas chegadas %d, esperando na guia %d" % [
			p.get("reroutes", 0), p.get("released", 0), p.get("governor", 1.0), p.get("lost_stuck", 0), p.get("people_arrived", 0),
			p.get("people_waiting", 0)])
		print("  na fila atrás de: ", p.get("queued_behind", []))
		print("  parados no cruzamento (atrás de outro, por pedestres, sem motivo): ", p.get("box_stops", []), ", entradas no vermelho %d (%d paradas)" % [p.get("red_entries", 0), p.get("red_from_stop", 0)])
	for line in CityStore.query(&"traffic_probe", [SOAK_JAMS]).jam_info:
		print("  retido: ", line)
