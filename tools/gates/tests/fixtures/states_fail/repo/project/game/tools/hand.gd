## A grip whose setter reads no table.
extends Node

enum Grip { OPEN, SHUT }

var grip := Grip.OPEN


func set_grip(next: Grip) -> void:
	grip = next
