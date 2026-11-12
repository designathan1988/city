# A kiln below a base it calls through `super`, an inner class kept in a dictionary's entry, a function added to a list
# and run by the loop over the list.
class_name Kiln
extends "res://lib/oven.gd"

class Rack:
	func load(n):
		return n

var racks = {"top": Rack.new()}
var jobs = []

func heat(n):
	super(n)
	super.vent()
	self.racks["top"].load(n)
	jobs.append(cool)
	for job in jobs:
		job.call(n)

func cool(n):
	return n
