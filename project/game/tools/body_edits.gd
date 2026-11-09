## What a card of the Build strip does to the block picked (data/ui/builder.json `do`): each edit goes to the core, which answers.
class_name BodyEdits
extends RefCounted



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
			return CityStore.dispatch(&"body_add_setback", [id, vol, CityStore.constants.setback_by_module, value]) # value: its storeys
		"plan":
			return CityStore.dispatch(&"body_set_plan", [id, vol, value])
		"block_mode":
			return CityStore.dispatch(&"body_set_block_mode", [id, vol, value])
		"extrude":
			return _on_face(&"body_extrude_face", picked, value * Units.M_PER_CM)
		"extrude_block":
			return _on_face(&"body_extrude_block", picked, value * Units.M_PER_CM)
		"inset_face":
			return _on_face(&"body_inset_face", picked, value * Units.M_PER_CM)
		"offset":
			return CityStore.dispatch(&"body_offset", [id, vol, value * Units.M_PER_CM])
		"bevel":
			return CityStore.dispatch(&"body_bevel", [id, vol, value * Units.M_PER_CM, CityStore.constants.every_corner])
		"copy_block":
			return CityStore.dispatch(&"body_copy_block", [id, vol])
		"detach":
			return CityStore.dispatch(&"body_detach", [id, vol])
		"center_block":
			return CityStore.dispatch(&"body_center_block", [id, vol])
		"turn_block":
			return CityStore.dispatch(&"body_turn_block", [id, vol, float(value)])
		"relief":
			if picked.side < 0 or picked.bay < 0:
				return {"ok": false, "error": "building.relief.pickFace"}
			var region := {"side": picked.side, "bay0": picked.bay, "bay1": picked.bay, "storey0": picked.storey, "storey1": picked.storey, "depth": value * Units.M_PER_CM}
			return CityStore.dispatch(&"body_set_relief", [id, vol, region, true])
		"remove":
			return CityStore.dispatch(&"body_remove_volume", [id, vol])
		"duplicate":
			return CityStore.dispatch(&"building_duplicate", [id])
		"pattern":
			return CityStore.dispatch(&"body_set_pattern", [id, vol, -1, -1, value])
		"roof":
			return CityStore.dispatch(&"body_set_roof", [id, vol, value])
		"finish":
			return CityStore.dispatch(&"body_paint", [id, paint_scope, vol, maxi(picked.side, 0), picked.storey, value, spec.rgb])
	return {"ok": false, "error": "tool.failed"}


## What needs a face: the pick must have one.
static func _on_face(command: StringName, picked: Dictionary, amount: float) -> Dictionary:
	if picked.side < 0:
		return {"ok": false, "error": "builder.pickFace"}
	return CityStore.dispatch(command, [picked.id, picked.volume, picked.side, amount])
