# Calibration case (LAB_COPY): names that are single words of the domain, or compounds the reference never defines.
extends RefCounted

var oven: Dictionary
var loaf: Dictionary
var batch: Array = []
var rack := 0
var dough_rest := 0.0


func bake(dough: Dictionary) -> Dictionary:
	return {"loaf": dough, "batch": batch.size()}


func cool(minutes: float) -> int:
	rack += int(minutes)
	return rack
