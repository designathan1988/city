## The save_undo_test case of a file from before the land's side was in it: a real file cut back to that format opens on the land it was made on, not on what the table says today.
class_name SaveLandCases
extends RefCounted

const TOWN_MAP := 0
const TEN_KM_MAP := 1 # the land of an old file: its terrain block has the size it was made on
const SEED := 11
const VERSION_AT := 4 # the header: magic, version, map, seed, tiles
const MAP_AT := 8
const TILES_AT := 16
const TILES_BYTES := 4
const OLD_VERSION := 7
const OLD_HALF_M := 5120.0


## `t`: the save_undo_test (TestCase), whose map, save and check these are.
static func run(t: Node, slot: String) -> void:
	t.new_map(TEN_KM_MAP, SEED, false)
	var written: Dictionary = await t.save(slot)
	t.check(written.get("ok", false), "the file to cut back is written: %s" % written) # else what is read is another city's
	if not written.get("ok", false):
		return
	var path: String = CityStore.saves.save_path(slot)
	var f := FileAccess.open_compressed(path, FileAccess.READ, FileAccess.COMPRESSION_ZSTD)
	var bytes := f.get_buffer(f.get_length())
	f.close()
	bytes.encode_u32(VERSION_AT, OLD_VERSION)
	bytes.encode_s32(MAP_AT, TOWN_MAP)
	var old := bytes.slice(0, TILES_AT)
	old.append_array(bytes.slice(TILES_AT + TILES_BYTES))
	var w := FileAccess.open_compressed(path, FileAccess.WRITE, FileAccess.COMPRESSION_ZSTD)
	w.store_buffer(old)
	w.close()
	var r: Dictionary = CityStore.saves.load_game(slot, true)
	t.check(r.get("ok", false), "a file of the format before opens: %s" % r)
	var half: float = CityStore.query(&"city_bounds").half
	t.check(is_equal_approx(half, OLD_HALF_M), "and stands on the land it was made on: %.0f m" % half)
