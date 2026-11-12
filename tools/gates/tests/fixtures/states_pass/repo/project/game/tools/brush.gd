## A brush's own tool, written from a value through a typed brush: not the hand's.
extends Node


func pick(brush: Brush, which: int) -> void:
	brush.tool = which
