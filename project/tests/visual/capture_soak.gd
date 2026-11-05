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
		print("soak min %d: %d veículos, %d andando, %d retidos (%.1f %%), parados por %s, chegadas %d, perdidas %d, pessoas %d" % [
			m + 1, p.vehicles, p.moving, p.waiting, share, p.held_by, p.trips_arrived, p.trips_lost, p.people])
		print("  desvios %d, liberados %d, governador %.2f, perdidas por travamento %d, pessoas chegadas %d, esperando na guia %d" % [
			p.get("reroutes", 0), p.get("released", 0), p.get("governor", 1.0), p.get("lost_stuck", 0), p.get("people_arrived", 0),
			p.get("people_waiting", 0)])
		print("  retidos em média no minuto: %.1f" % (float(held_seconds) / SECONDS_PER_MINUTE))
		print("  lotes: %d carros (%d a caminho, %d manobrando), chamados %d, estacionaram %d, saíram %d; parados de cenário %d, vagas %d" % [
			p.get("lot_cars", 0), p.get("lot_coming", 0), p.get("lot_moving", 0), p.get("lot_called", 0), p.get("lot_parked_in", 0),
			p.get("lot_left_by", 0), p.get("parked_rows", 0), p.get("bays", 0)])
		print("  passo, ms somados (vagas, frota, lotes, pedestres, olhada: saídas, chamadas, publicação): ", p.get("step_ms", []), "; última entrada no vermelho (tipo, m/s, ds, tempo): ", p.get("red_last", []))
		var ticks := TICKS_PER_SECOND * SECONDS_PER_MINUTE
		print("  na fila atrás de: ", p.get("queued_behind", []), ", núcleo %.0f ms no minuto (%.3f ms/passo)" % [spent, spent / ticks])
		print("  pedestres sobrepostos a outro (andando, esperando): ", p.get("people_overlaps", []), ", em média no minuto: %.1f, %.1f" % [
			float(overlap_seconds[0]) / SECONDS_PER_MINUTE, float(overlap_seconds[1]) / SECONDS_PER_MINUTE],
			"; dentro do mobiliário da calçada: %d, pedestre-segundos no minuto: %d" % [p.get("people_in_furniture", 0),
			furniture_seconds])
		print("  parados no cruzamento, veículo-segundos no minuto (atrás de outro, por pedestres, por semáforo à frente, partindo): ", box,
			", entradas no vermelho %d (%d paradas)" % [p.get("red_entries", 0), p.get("red_from_stop", 0)])
	var last: Dictionary = CityStore.query(&"traffic_probe", [SOAK_JAMS])
	print("  mobiliário da rua (semáforos, postes, hidrantes, lixeiras, bancos, árvores): ", last.get("street_furniture", []))
	for line in last.jam_info:
		print("  retido: ", line)
