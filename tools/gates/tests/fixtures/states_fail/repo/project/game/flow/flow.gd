## A flow whose state is also written beside its setter.
extends Node

enum State { A, B }

const TRANSITIONS := {State.A: [State.B], State.B: []}

var state: State = State.A


func _to(next: State) -> void:
	if TRANSITIONS[state].has(next):
		state = next


func jump() -> void:
	state = State.B
