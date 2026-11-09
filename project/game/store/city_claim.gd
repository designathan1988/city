## Whether the city on screen is the player's, and letting go of one that is not. The lab writes the map on screen whether the player
## touched it or not (main.ts snapshotApplied) and writes the next map over it (one autosave); here each city has its file (SaveSlots),
## so a city that never was the player's is forgotten when another takes its place: its file holds nothing its seed does not make
## again, and the saves folder keeps the player's cities and the last one on screen. A city is the player's once they edit it (the
## core's `edited`) or Save it, or when it opens from a file of theirs (Session's `kept`). CitySaves holds one and tells it each change.
class_name CityClaim
extends RefCounted

var kept := false


## A city taken in from `slot`: the player's when its file says so; one with no slot of its own ("": a new map, a file from elsewhere,
## a map copy) is not until they make it theirs.
func opened(slot: String) -> void:
	kept = not slot.is_empty() and Session.kept(slot)


## An edit of the player's makes the city theirs (the core's `edited`, reset only when another city takes its place).
func note(snapshot: Dictionary) -> void:
	if snapshot.get("edited", false):
		kept = true


## The city on screen, saved to `slot`, gives way to another: its file goes when it never was the player's.
func give_way(slot: String) -> void:
	if not slot.is_empty() and not kept:
		SaveSlots.forget(slot)
