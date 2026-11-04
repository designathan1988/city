## The only reader of project/data/*.json (gate DADOS_POR_UM_LEITOR). Everything else asks this class.
class_name DataTables
extends RefCounted

const CATALOG := {
	"tuning": "res://data/catalog/tuning.json",
	"maps": "res://data/catalog/maps.json",
	"road_profiles": "res://data/catalog/road_profiles.json",
	"zones": "res://data/catalog/zones.json",
	"growth": "res://data/catalog/growth.json",
	"traffic": "res://data/catalog/traffic.json",
	"street_furniture": "res://data/catalog/street_furniture.json",
}
## Tables split only to keep each file small; merged under the same key they extend.
const CATALOG_FRAGMENTS := {
	"road_profiles": [
		"res://data/catalog/road_profiles_a.json",
		"res://data/catalog/road_profiles_b.json",
	],
}
const RENDER_TABLES := {
	"palette": "res://data/render/palette.json",
	"streets": "res://data/render/streets.json",
	"buildings": "res://data/render/buildings.json",
	"interiors": "res://data/render/interiors.json",
	"materials": "res://data/render/materials.json",
	"agents": "res://data/render/agents.json",
	"overlays": "res://data/render/overlays.json",
}
const UI := {
	"tokens": "res://data/ui/tokens.json",
	"toolbar": "res://data/ui/toolbar.json",
	"hud": "res://data/ui/hud.json",
	"drawers": "res://data/ui/drawers.json",
}
const GAME := {
	"camera": "res://data/game/camera.json",
	"settings": "res://data/game/settings.json",
	"daylight": "res://data/game/daylight.json",
	"saves": "res://data/game/saves.json",
}
const STRINGS := [
	"res://data/i18n/strings.csv",
	"res://data/i18n/strings_b.csv",
	"res://data/i18n/strings_c.csv",
]

static var _cache: Dictionary = {}


static func _read(path: String) -> Variant:
	if _cache.has(path):
		return _cache[path]
	var text := FileAccess.get_file_as_string(path)
	var parsed: Variant = JSON.parse_string(text)
	if parsed == null:
		push_error("JSON inválido: %s" % path)
		parsed = {}
	_cache[path] = parsed
	return parsed


static func _group(paths: Dictionary) -> Dictionary:
	var out := {}
	for key in paths:
		out[key] = _read(paths[key])
	return out


static func catalog() -> Dictionary:
	var out := _group(CATALOG)
	for key in CATALOG_FRAGMENTS:
		var list: Array = out[key].get("profiles", [])
		for path in CATALOG_FRAGMENTS[key]:
			list.append_array(_read(path).get("profiles", []))
		out[key]["profiles"] = list
	return out


static func render() -> Dictionary:
	return _group(RENDER_TABLES)


static func ui(name: String) -> Dictionary:
	return _read(UI[name])


static func game(name: String) -> Dictionary:
	return _read(GAME[name])


static func tuning(key: String) -> float:
	var parts := key.split("_", true, 1)
	return float(_read(CATALOG.tuning)[parts[0]][parts[1]])


## Registers data/i18n/strings*.csv (key, pt_BR, en) as translations. The catalog is split only to keep
## each file small; every part repeats the header.
static func load_translations() -> void:
	var first := FileAccess.open(STRINGS[0], FileAccess.READ)
	if first == null:
		push_error("strings.csv ausente")
		return
	var header := first.get_csv_line()
	first.close()
	var translations := []
	for i in range(1, header.size()):
		var t := Translation.new()
		t.locale = header[i]
		translations.append(t)
	for path in STRINGS:
		var file := FileAccess.open(path, FileAccess.READ)
		if file == null:
			push_error("strings ausente: %s" % path)
			continue
		file.get_csv_line()
		while not file.eof_reached():
			var row := file.get_csv_line()
			if row.size() < header.size() or row[0].is_empty():
				continue
			for i in range(1, header.size()):
				translations[i - 1].add_message(row[0], row[i])
		file.close()
	for t in translations:
		TranslationServer.add_translation(t)
