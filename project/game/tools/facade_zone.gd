## The Facade tab's zone scope (the Roadcraft's FacadeScope 'zone'): with a component in hand, a first click on a wall marks a bay, a
## second click on the same wall of the same block puts the component in every bay of the rectangle between the two (storeys by bays);
## any other scope puts it at once (body_set_bay).
class_name FacadeZone
extends RefCounted

## builder.json's zone card: not a core FacadeScope (types.h), the region between two clicks.
const ZONE := -1

## The first corner of the zone, a body_pick answer; {} for none.
static var _corner := {}


## The component `what` put where the click `hit` asks, at scope `scope`: the core's answer.
static func put(hit: Dictionary, what: int, scope: int) -> Dictionary:
	if scope != ZONE:
		return CityStore.dispatch(&"body_set_bay", [hit.id, hit.volume, hit.storey, hit.side, hit.bay, what, scope])
	var a := _corner
	if a.is_empty() or a.id != hit.id or a.volume != hit.volume or a.side != hit.side:
		_corner = hit
		return {"ok": true}
	_corner = {}
	return CityStore.dispatch(&"body_set_zone", [hit.id, hit.volume, hit.side, mini(a.storey, hit.storey), maxi(a.storey, hit.storey),
		mini(a.bay, hit.bay), maxi(a.bay, hit.bay), what])


static func forget() -> void:
	_corner = {}
