## The fixed views of the captures (capture.gd): name, camera offset from the city centre, look-at offset, hour.
class_name CaptureViews
extends RefCounted

## Hours the light is pinned to (Roadcraft "always day" / "always night"); a view may name its own.
const DAY := 16.0
const NIGHT := 22.0
## Views: name, camera offset from the city centre, look-at offset (metres); 17-18: superchunk far switch.
const VIEWS := [
	["01_rua", Vector3(-58, 62, 40), Vector3(-58, 0, -70)],
	["02_bairro", Vector3(-260, 260, 260), Vector3(40, 0, -40)],
	["03_cidade", Vector3(-900, 700, 900), Vector3(0, 0, 0)],
	["04_rua_baixa", Vector3(-62, 14, 30), Vector3(-62, 6, -60)],
	["05_transito", Vector3(-60, 6, 18), Vector3(-62, 1, -30)],
	["06_cruzamento", Vector3(-20, 30, 40), Vector3(-60, 0, -20)],
	["13_calcada", Vector3(-53, 2.2, 6), Vector3(-55, 1.0, -22)],
	["10_noite", Vector3(-58, 62, 40), Vector3(-58, 0, -70), NIGHT],
	["11_relevo", Vector3(1750, 150, 1950), Vector3(2050, 0, 1650), DAY],
	["12_encosta", Vector3(3750, 70, 300), Vector3(4400, 0, 80), DAY],
	["17_horizonte", Vector3(-1500, 140, 1500), Vector3(600, 0, -600), DAY],
	["18_alto", Vector3(0, 2600, 2600), Vector3(0, 0, 0), DAY],
	["21_noite_bairro", Vector3(-260, 260, 260), Vector3(40, 0, -40), NIGHT],
	["21_noite_cidade", Vector3(-900, 700, 900), Vector3(0, 0, 0), NIGHT],
]
