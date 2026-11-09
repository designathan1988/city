## The building picked in the Build drawer, shared by its tools: the Build tool sets it (BuildingTool.show_pick), the Draw tab's
## targets (beside, on the roof, cut out) and a sketch against a block read it, and a Build tab opened again starts from it.
class_name BuildPick
extends RefCounted

## A body_pick answer ({id, volume, side, storey, level, bay, part, gear}), {} for none.
static var current := {}


## The pick, when its building still stands; {} otherwise (an undo or a demolition took it away).
static func standing() -> Dictionary:
	if current.is_empty() or (CityStore.query(&"body_info", [current.id]) as Dictionary).get("volumes", []).is_empty():
		return {}
	return current
