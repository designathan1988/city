# Quits.
extends Node
func f():
	var tree := get_tree()
	tree.quit()
	get_tree().paused = true
	tree.change_scene_to_file("res://x.tscn")
