## GameFlow's shape: one member, one setter, one table.
extends Node

enum State { A, B }

const TRANSITIONS := {State.A: [State.B], State.B: []}

var state: State = State.A


func _to(next: State) -> void:
	if TRANSITIONS[state].has(next):
		state = next


## state = State.B written in a comment changes nothing
func is_b() -> bool:
	var seen: State = state
	return seen == State.B
