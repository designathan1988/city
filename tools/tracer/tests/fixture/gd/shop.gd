# A shop: its own methods through `self` and bare names, a typed field, a preloaded script, a signal and its handler,
# a lambda kept in a variable, a function kept in a dictionary, an override below a base.
class_name Shop
extends "res://lib/base.gd"

signal sold(item)
const Tool = preload("res://lib/tool.gd")
var stock: Crate = Crate.new()
var hook = func(x): return polish(x)

func _init():
	sold.connect(_on_sold)

func open(a):
	self.stock.lift(a)
	count(a)
	var t = Tool.new()
	t.use()
	hook.call(a)

func sell(item):
	sold.emit(item)

func _on_sold(item):
	polish(item)

func polish(x):
	return x

func count(n):
	var table = {"run": polish}
	table["run"].call(n)
	return n

func greet():
	return 1

func unknown(thing):
	thing.lift(1)
