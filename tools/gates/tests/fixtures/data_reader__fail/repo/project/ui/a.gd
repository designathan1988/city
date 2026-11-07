# A.
extends Node
var p = "res://data/ui/hud.json"
func f(n):
	return load("res://data/catalog/" + n + ".json")
