## A tool any tool may follow: its one setter, no table.
extends Node

enum Tool { NONE, ROAD }

var tool: Tool = Tool.NONE


func set_tool(next: Tool) -> void:
	tool = next
