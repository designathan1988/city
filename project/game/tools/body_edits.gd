## What a card of the Build strip does to the block picked (data/ui/builder.json `do`): each edit goes to the core, which answers.
class_name BodyEdits
extends RefCounted

const CM := 0.01


static func run(spec: Dictionary, value: int, picked: Dictionary, paint_scope: int) -> Dictionary:
	var id: int = picked.id
	var vol: int = picked.volume
	match spec.do:
		"storeys":
			var v: Dictionary = CityStore.query(&"body_info", [id]).volumes[vol]
			return CityStore.dispatch(&"body_storeys", [id, vol, v.storeys + value])
		"split":
			return CityStore.dispatch(&"body_split", [id, vol, picked.level])
		"setback":
			return CityStore.dispatch(&"body_add_setback", [id, vol, -1.0, 2])
		"plan":
			return CityStore.dispatch(&"body_set_plan", [id, vol, value])
		"block_mode":
			return CityStore.dispatch(&"body_set_block_mode", [id, vol, value])
		"relief":
			if picked.side < 0 or picked.bay < 0:
				return {"ok": false, "error": "building.relief.pickFace"}
			var region := {"side": picked.side, "bay0": picked.bay, "bay1": picked.bay, "storey0": picked.storey, "storey1": picked.storey, "depth": value * CM}
			return CityStore.dispatch(&"body_set_relief", [id, vol, region, true])
		"remove":
			return CityStore.dispatch(&"body_remove_volume", [id, vol])
		"pattern":
			return CityStore.dispatch(&"body_set_pattern", [id, vol, -1, -1, value])
		"roof":
			return CityStore.dispatch(&"body_set_roof", [id, vol, value])
		"finish":
			return CityStore.dispatch(&"body_paint", [id, paint_scope, vol, maxi(picked.side, 0), picked.storey, value, spec.rgb])
	return {"ok": false, "error": "tool.failed"}
