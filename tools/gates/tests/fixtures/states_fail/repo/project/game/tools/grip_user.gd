## A grip written from another script, through a typed hand and through the hand's own name (an autoload).
extends Node


func shut(hand: Hand, how: int) -> void:
	hand.grip = how


func open_all(how: int) -> void:
	Hand.grip = how
