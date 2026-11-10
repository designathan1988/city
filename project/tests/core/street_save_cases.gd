## The save cases of street_works_test: the street's block of the save (sim/persistence/save_street.cpp) round trip, and a
## save from before it.
class_name StreetSaveCases
extends RefCounted

const TOWN_MAP := 0
const OTHER_SEED := 7
const SLOT := "teste_rua"
const TAG_BYTES := [0x53, 0x54, 0x52, 0x54] # save_street.h SAVE_TAG_STREET: STRT
const SIZE_BYTES := 4
const KEYS := ["items", "poles", "rural_poles", "spans", "barriers", "cuts"]

var _host: Node
var _check: Callable


func _init(host: Node, check: Callable) -> void:
	_host = host
	_check = check


## Saved and opened, the street records come back; a save with the block cut out opens with none and the same roads.
func run() -> void:
	var before: Dictionary = CityStore.query(&"street_counts")
	print("contagens: ", before)
	CityStore.saves.save_game(SLOT)
	while CityStore.saves.save_busy():
		await _host.get_tree().process_frame
	CityStore.new_map(TOWN_MAP, OTHER_SEED, true)
	_check.call(CityStore.query(&"street_counts").poles == 0, "a new city with no poles")
	var loaded: Dictionary = CityStore.saves.load_game(SLOT)
	_check.call(loaded.get("ok", false), "abrir")
	var after: Dictionary = CityStore.query(&"street_counts")
	for key in KEYS:
		_check.call(after[key] == before[key], "save guarda " + key)
	_check.call(after.poles > 0 and after.rural_poles > 0 and after.spans > 0 and after.items > 0 and after.barriers > 0, "something was saved and opened")
	await _old_save(before)
	DirAccess.remove_absolute(CityStore.saves.save_path(SLOT))
	DirAccess.remove_absolute(Session.path(SLOT)) # and the session the save wrote beside it


## The save with the street's block cut out, as one from before it: it opens, with no street records and the same roads.
func _old_save(with_street: Dictionary) -> void:
	var path: String = CityStore.saves.save_path(SLOT)
	var f := FileAccess.open_compressed(path, FileAccess.READ, FileAccess.COMPRESSION_ZSTD)
	var bytes := f.get_buffer(f.get_length())
	f.close()
	var o := bytes.size() - TAG_BYTES.size()
	while o >= 0 and not (bytes[o] == TAG_BYTES[0] and bytes[o + 1] == TAG_BYTES[1] and bytes[o + 2] == TAG_BYTES[2] and bytes[o + 3] == TAG_BYTES[3]):
		o -= 1
	_check.call(o > 0 and with_street.poles > 0, "the street block found in the save")
	var size := bytes.decode_u32(o + TAG_BYTES.size())
	var old := bytes.slice(0, o)
	old.append_array(bytes.slice(o + TAG_BYTES.size() + SIZE_BYTES + size))
	var w := FileAccess.open_compressed(path, FileAccess.WRITE, FileAccess.COMPRESSION_ZSTD)
	w.store_buffer(old)
	w.close()
	var bounds: Dictionary = CityStore.query(&"city_bounds")
	var r: Dictionary = CityStore.saves.load_game(SLOT)
	_check.call(r.get("ok", false), "a save with no block opens %s" % r)
	var now: Dictionary = CityStore.query(&"street_counts")
	for key in KEYS:
		_check.call(now[key] == 0, "with no block, no " + key)
	_check.call(CityStore.query(&"city_bounds").max == bounds.max, "the old save's roads come back")
