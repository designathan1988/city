## The lanes of a junction as the links panel holds them: the node, its lanes as read, the one picked, and the plan's frame (ConnectorPanel draws it).
class_name LaneLinksEdit
extends Resource

var node := -1
var lanes: Dictionary = {}
var picked: Dictionary = {}
var lo := Vector2.ZERO
var scale := 1.0
