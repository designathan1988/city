# The kiln's loads. Behaviour read from the reference's OvenQueue.nextBatch and batchMinutes (ref/oven.ts): a loaf not
# proofed waits; a load fills while the shelves have room. Written for this game: the table comes from data.
extends RefCounted

var table: Dictionary
var shelves_open := 0


func fill(line: Array, room: int) -> Array:
	var taken: Array = []
	for bread in line:
		if taken.size() == room or shelves_open == 0:
			break
		if bread.proofed >= table.proof_needed:
			taken.append(bread)
			shelves_open -= 1
	return taken
