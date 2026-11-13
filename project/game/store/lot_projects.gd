## The lots of the Roadcraft's lot lab (lotLibrary.ts): landmarks and whole projects, each a building with its lot, offered on the shelf of Models. The
## documents come from DataTables; the core makes each a body once, the first time the shelf opens.
class_name LotProjects
extends RefCounted

const GROUP := "lots"
const KEY_PREFIX := "lot:"
const LABEL_PREFIX := "lot."
const USE_ZONES := {"residential_low": "res_low", "residential_medium": "residential", "residential_high": "res_high", "commercial_low": "com_low",
	"commercial_medium": "commercial", "commercial_high": "com_high", "industrial_high": "industrial", "industrial_medium": "industrial", "mixed_high": "mixed", "mixed_medium": "mixed"}

static var _entries: Array[Dictionary] = []


static func _zone(doc: Dictionary) -> int:
	var id: String = USE_ZONES.get("%s_%s" % [doc.use, doc.density], "")
	for z: Dictionary in DataTables.catalog_table("zones").zones:
		if z.id == id:
			return int(z.code)
	return 0


## Each project as the shelf lists a model: {key, label, group, function, zone, bytes}.
static func entries() -> Array[Dictionary]:
	if _entries.is_empty():
		for doc: Dictionary in DataTables.lot_projects():
			var made: Dictionary = CityStore.query(&"doc_body", [doc.body])
			if not made.has("bytes"):
				push_error("lot %s refused: %s" % [doc.key, made.errors])
				continue
			_entries.append({"key": KEY_PREFIX + doc.key, "label": TranslationServer.translate(LABEL_PREFIX + doc.key), "group": GROUP, "function": made.function,
				"zone": _zone(doc), "bytes": made.bytes})
	return _entries
