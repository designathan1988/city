## The save cases of street_works_test: the street's block of the save (sim/persistence/save_street.cpp) round trip, and a
## save from before it.
class_name StreetSaveCases
extends RefCounted

const TOWN_MAP := 0
const OTHER_SEED := 7
const SLOT := "street_test"
const TAG_BYTES := 4 # a block's tag, a u32 (the street's: CityStore.constants.save_tag_street)
const SIZE_BYTES := 4
const KEYS := ["items", "poles", "rural_poles", "spans", "barriers", "cuts"]

var t: Node # the test (TestCase): its check, ticks and tree


func _init(test: Node) -> void:
	t = test


## Saved and opened, the street records come back; a save with the block cut out opens with none and the same roads.
func run() -> void:
	var before: Dictionary = CityStore.query(&"street_counts")
	print("counts: ", before)
	var saved: Dictionary = await t.save(SLOT)
	t.check(saved.get("ok", false), "the city is written: %s" % saved)
	t.new_map(TOWN_MAP, OTHER_SEED, true)
	t.check(CityStore.query(&"street_counts").poles == 0, "a new city with no poles")
	var loaded: Dictionary = await CityStore.saves.load_game(SLOT, true)
	t.check(loaded.get("ok", false), "the file opens: %s" % loaded)
	var after: Dictionary = CityStore.query(&"street_counts")
	for key in KEYS:
		t.check(after[key] == before[key], "the save keeps " + key)
	t.check(after.poles > 0 and after.rural_poles > 0 and after.spans > 0 and after.items > 0 and after.barriers > 0, "something was saved and opened")
	await _old_save(before)
	DirAccess.remove_absolute(CityStore.saves.save_path(SLOT))
	DirAccess.remove_absolute(Session.path(SLOT)) # and the session the save wrote beside it


## The save with the street's block cut out, as one from before it: it opens, with no street records and the same roads.
func _old_save(with_street: Dictionary) -> void:
	var path: String = CityStore.saves.save_path(SLOT)
	var f := FileAccess.open_compressed(path, FileAccess.READ, FileAccess.COMPRESSION_ZSTD)
	var bytes := f.get_buffer(f.get_length())
	f.close()
	var tag: int = CityStore.constants.save_tag_street
	var o := bytes.size() - TAG_BYTES
	while o >= 0 and bytes.decode_u32(o) != tag:
		o -= 1
	t.check(o > 0 and with_street.poles > 0, "the street block found in the save")
	var size := bytes.decode_u32(o + TAG_BYTES)
	var old := bytes.slice(0, o)
	old.append_array(bytes.slice(o + TAG_BYTES + SIZE_BYTES + size))
	var w := FileAccess.open_compressed(path, FileAccess.WRITE, FileAccess.COMPRESSION_ZSTD)
	w.store_buffer(old)
	w.close()
	var bounds: Dictionary = CityStore.query(&"city_bounds")
	var r: Dictionary = await CityStore.saves.load_game(SLOT, true)
	t.check(r.get("ok", false), "a save with no block opens %s" % r)
	var now: Dictionary = CityStore.query(&"street_counts")
	for key in KEYS:
		t.check(now[key] == 0, "with no block, no " + key)
	t.check(CityStore.query(&"city_bounds").max == bounds.max, "the old save's roads come back")
