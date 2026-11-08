## What the zones table says of a zone (data/catalog/zones.json, through DataTables): its code by id and its name by code.
class_name ZoneTable
extends RefCounted


## The code of zone `id` (zones.json), -1 when there is none.
static func code(id: String) -> int:
	for z: Dictionary in DataTables.catalog_table("zones").zones:
		if z.id == id:
			return int(z.code)
	return -1


## The text key of the zone of code `zone` (zones.json name_key), "" when there is none.
static func name_key(zone: int) -> String:
	for z: Dictionary in DataTables.catalog_table("zones").zones:
		if int(z.code) == zone:
			return z.name_key
	return ""
