# Calls a global the project file declares, and extends a script by its path from the project's folder.
extends "res://base/plain.gd"

func press():
	Ledger.post(&"stamp")

func hello():
	return greet()
