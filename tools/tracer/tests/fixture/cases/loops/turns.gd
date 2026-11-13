class_name Heap
extends Node

var count = 0
var odd = false
var low = false

func spin(n):
	var i = 0
	var first = count
	while i < n:
		if i < 0:
			odd = true
		count = count + 2
		i = i + 1
	if count < first or i < 0:
		low = true
