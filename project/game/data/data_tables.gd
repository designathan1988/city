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
	"blast": "res://data/catalog/blast.json",
	"nature": "res://data/catalog/nature.json",
	"scenic": "res://data/catalog/scenic.json",
	"buildings": "res://data/catalog/buildings.json",
	"models": "res://data/catalog/models.json",
	"interiors": "res://data/catalog/interiors.json",
}
static var _catalog_cache := {}
const RENDER_TABLES := {
	"palette": "res://data/render/palette.json",
	"streets": "res://data/render/streets.json",
	"cover": "res://data/render/cover.json",
	"building_look": "res://data/render/building_look.json",
	"bay_props": "res://data/render/bay_props.json",
	"furniture": "res://data/render/furniture.json",
	"materials": "res://data/render/materials.json",
	"agents": "res://data/render/agents.json",
	"overlays": "res://data/render/overlays.json",
	"transit": "res://data/render/transit.json",
	"effects": "res://data/render/effects.json",
	"aftermath": "res://data/render/aftermath.json",
	"gore": "res://data/render/gore.json",
}
const UI := {
	"tokens": "res://data/ui/tokens.json",
	"land_panels": "res://data/ui/land_panels.json",
	"hud": "res://data/ui/hud.json",
	"drawers": "res://data/ui/drawers.json",
	"keys": "res://data/ui/keys.json",
	"help": "res://data/ui/help.json",
	"builder": "res://data/ui/builder.json",
}
const GAME := {
	"camera": "res://data/game/camera.json",
	"settings": "res://data/game/settings.json",
	"daylight": "res://data/game/daylight.json",
	"sky": "res://data/game/sky.json",
	"saves": "res://data/game/saves.json",
	"interior": "res://data/game/interior.json",
	"render_switches": "res://data/game/render_switches.json",
	"lab": "res://data/game/lab.json",
	"optimizations": "res://data/game/optimizations.json",
	"health": "res://data/game/health.json",
	"start": "res://data/game/start.json",
	"tools": "res://data/game/tools.json",
}
## The regional style a city's tables read (session-rules.md, section 10): this one until a city carries its own. The tables that
## keep what changes with it under `styles.<id>`, laid over the table when it is read.
const REGIONAL_STYLE := "roadcraft"
const LOT_PROJECTS := "res://assets/lots"
const LOT_EXT := "json"
const STYLED := ["res://data/catalog/street_furniture.json", "res://data/render/streets.json"]
const STRINGS := ["res://data/i18n/strings.csv"]

static var _cache: Dictionary = {}


static func _read(path: String) -> Variant:
	if _cache.has(path):
		return _cache[path]
	var parsed: Variant = _parse(path)
	_cache[path] = _styled(parsed, REGIONAL_STYLE) if path in STYLED else parsed
	return _cache[path]


static func _parse(path: String) -> Variant:
	var parsed: Variant = JSON.parse_string(FileAccess.get_file_as_string(path))
	if parsed == null:
		push_error("JSON inválido: %s" % path)
		return {}
	return parsed


## The table at `path` as style `id` draws it, read anew: the game keeps only the one it plays.
static func style_of(path: String, id: String) -> Dictionary:
	return _styled(_parse(path), id)


## What style `id` holds laid over `table`, block by block; the other styles dropped.
static func _styled(table: Dictionary, id: String) -> Dictionary:
	var over: Dictionary = table.styles[id]
	table.erase("styles")
	_lay(table, over)
	return table


static func _lay(under: Dictionary, over: Dictionary) -> void:
	for key: String in over:
		if over[key] is Dictionary and under.get(key) is Dictionary:
			_lay(under[key], over[key])
		else:
			under[key] = over[key]


static func _group(paths: Dictionary) -> Dictionary:
	var out := {}
	for key in paths:
		out[key] = _read(paths[key])
	return out


## One render table (kept once read): what the game reads while it plays asks this, not render(), which reads every table
## again once the core has its copy (release_boot_tables).
static func render_table(key: String) -> Dictionary:
	return _read(RENDER_TABLES[key])


## One table of the catalog (kept once read); the same, against catalog().
static func catalog_table(key: String) -> Dictionary:
	if not _catalog_cache.has(key):
		_catalog_cache[key] = _read(CATALOG[key])
	return _catalog_cache[key]


static func catalog() -> Dictionary:
	return _group(CATALOG)


## The maps of the catalogue, in the order the core numbers them.
static func maps() -> Array:
	return _read(CATALOG.maps).maps


## The index the core numbers map `id` by (maps.json), -1 when there is none.
static func map_index(id: String) -> int:
	var list := maps()
	for i in list.size():
		if list[i].id == id:
			return i
	return -1


static func render() -> Dictionary:
	return _group(RENDER_TABLES)


## The core holds its own copy of the catalog and of the render tables once it has booted: they need not stay here too.
## Tuning stays, the script reads it all along; anything asked again is read anew.
static func release_boot_tables() -> void:
	for group: Dictionary in [CATALOG, RENDER_TABLES]:
		for path: String in group.values():
			if path != CATALOG.tuning:
				_cache.erase(path)
	_catalog_cache.clear() # catalog_table's own copies: read anew too


static func ui(name: String) -> Dictionary:
	return _read(UI[name])


## The lots of the lot lab: each document with `key`, its file's name (LotProjects).
static func lot_projects() -> Array[Dictionary]:
	var out: Array[Dictionary] = []
	for file in DirAccess.get_files_at(LOT_PROJECTS):
		if file.get_extension() == LOT_EXT:
			var doc: Dictionary = _parse("%s/%s" % [LOT_PROJECTS, file])
			doc.key = file.get_basename()
			out.append(doc)
	return out


static func game(name: String) -> Dictionary:
	return _read(GAME[name])


static func tuning(key: String) -> float:
	var parts := key.split("_", true, 1)
	return float(_read(CATALOG.tuning)[parts[0]][parts[1]])


## Registers data/i18n/strings.csv (key, then one column per language) as translations.
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
