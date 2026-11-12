## Ordered edit input recipes: retain interpolation/timing debt, execute by shared frame credits, and complete barriers explicitly.
## CityStore owns execution/publication; this queue never interprets Pending as a successful native command result.
class_name InputSequence
extends RefCounted

const PAGE_ITEMS := 64

enum Kind { COMMAND, PATH, TIMED, ZONE, BARRIER, OPERATION, FENCE }

class Ticket:
	extends RefCounted
	signal completed(result: Variant)
	var done := false
	var result: Variant
	func finish(value: Variant) -> void:
		assert(not done)
		done = true
		result = value
		completed.emit(value)

## A desired asynchronous publication can be retired without owning or calling its former UI/source.
class Guard:
	extends RefCounted
	var live := true

class Clock:
	extends RefCounted
	var held := 0.0

class Entry:
	extends RefCounted
	var kind: Kind = Kind.COMMAND
	var command: StringName
	var args: Array = []
	var first := Vector3.ZERO
	var last := Vector3.ZERO
	var steps := 1
	var next := 1
	var clock: Clock
	var delta := 0.0
	var quantum := 0.0
	var started := false
	var owner: WeakRef
	var method: StringName
	var ticket: Ticket
	var guard: Guard
	var result: Variant

class Page:
	extends RefCounted
	var rows: Array[Entry] = []
	var read := 0
	var next: Page

var _head: Page
var _tail: Page
var _size := 0
var _pumping := false
var _capturing_children := false
var _children_head: Page
var _children_tail: Page

func pending() -> bool:
	return _size != 0

func pumping() -> bool:
	return _pumping


func in_operation() -> bool:
	return _capturing_children

func _push(entry: Entry) -> void:
	var tail := _children_tail if _capturing_children else _tail
	if tail == null:
		tail = Page.new()
		if _capturing_children:
			_children_head = tail
		else:
			_head = tail
	elif tail.rows.size() == PAGE_ITEMS:
		tail.next = Page.new()
		tail = tail.next
	tail.rows.append(entry)
	if _capturing_children:
		_children_tail = tail
	else:
		_tail = tail
	_size += 1


func _join_children() -> void:
	if _children_head == null:
		return
	_children_tail.next = _head
	_head = _children_head
	if _tail == null:
		_tail = _children_tail
	_children_head = null
	_children_tail = null


func _pop() -> void:
	_head.rows[_head.read] = null
	_head.read += 1
	_size -= 1
	if _head.read == _head.rows.size():
		_head = _head.next
		if _head == null:
			_tail = null

func command(name: StringName, args: Array = [], owner: Object = null, method: StringName = &"", guard: Guard = null) -> Ticket:
	var entry := _entry(Kind.COMMAND, name, args, owner, method)
	entry.guard = guard
	_push(entry)
	return entry.ticket

func path(name: StringName, args: Array, first: Vector3, last: Vector3, steps: int, owner: Object = null, method: StringName = &"") -> Ticket:
	assert(steps > 0 and not args.is_empty())
	var entry := _entry(Kind.PATH, name, args, owner, method)
	entry.first = first
	entry.last = last
	entry.steps = steps
	_push(entry)
	return entry.ticket

func timed(name: StringName, args: Array, clock: Clock, delta: float, quantum: float, owner: Object = null, method: StringName = &"") -> Ticket:
	assert(clock != null and quantum > 0.0 and delta >= 0.0)
	var entry := _entry(Kind.TIMED, name, args, owner, method)
	entry.clock = clock
	entry.delta = delta
	entry.quantum = quantum
	_push(entry)
	return entry.ticket

func zone(first: Vector3, last: Vector3, steps: int, radius: float, zone_kind: int, owner: Object = null, method: StringName = &"") -> Ticket:
	assert(steps > 0)
	var entry := _entry(Kind.ZONE, &"paint_zone", [Vector3.ZERO, radius, zone_kind], owner, method)
	entry.first = first
	entry.last = last
	entry.steps = steps
	_push(entry)
	return entry.ticket

## A complete synchronous gesture/consumer is one scope. Its queued children precede later accepted gestures.
## Async work initiated by a gesture must publish through a later explicit ordered boundary, never a direct post-await edit.
func operation(owner: Object, method: StringName, args: Array = []) -> Ticket:
	var entry := _entry(Kind.OPERATION, &"", args, owner, method)
	_push(entry)
	return entry.ticket


func barrier(name: StringName, args: Array = []) -> Ticket:
	var entry := _entry(Kind.BARRIER, name, args, null, &"")
	_push(entry)
	return entry.ticket

func _entry(kind: Kind, name: StringName, args: Array, owner: Object, method: StringName) -> Entry:
	var entry := Entry.new()
	entry.kind = kind
	entry.command = name
	entry.args = args.duplicate(true) # capture slider/dictionary values now, never a later live tool's params
	entry.owner = weakref(owner) if owner != null else null
	entry.method = method
	entry.ticket = Ticket.new()
	return entry

## `store` supplies apply_input_command/input_barrier and one publication boundary. It is borrowed only in this advance.
## Every sample or iterator transition spends a credit, including timer debt which has no dab ready yet.
func advance(store: Node, units: int) -> void:
	if _pumping or units <= 0 or not pending():
		return
	_pumping = true
	store.begin_input_batch()
	var notices: Array[Entry] = []
	while units > 0 and pending():
		units -= 1
		var entry: Entry = _head.rows[_head.read]
		var stop_after := false
		if entry.guard != null and not entry.guard.live:
			entry.result = {"ok": false, "cancelled": true}
			_pop()
			notices.append(entry)
			continue
		if entry.kind == Kind.TIMED and not entry.started:
			entry.started = true
			entry.clock.held += entry.delta
			continue
		if entry.kind == Kind.TIMED and entry.clock.held < entry.quantum:
			_pop()
			notices.append(entry)
			continue
		if entry.kind == Kind.OPERATION:
			var target: Object = entry.owner.get_ref() if entry.owner != null else null
			if is_instance_valid(target):
				_capturing_children = true
				target.callv(entry.method, entry.args) # void gesture, not an asynchronously fabricated native result
			else:
				entry.result = {"cancelled": true}
			_capturing_children = true
			var fence := Entry.new()
			fence.kind = Kind.FENCE
			fence.ticket = entry.ticket
			fence.result = entry.result
			_push(fence) # completion follows every child/grandchild, even across one-credit frames
			_capturing_children = false
			_pop()
			_join_children()
			continue
		if entry.kind == Kind.FENCE:
			_pop()
			notices.append(entry)
			continue
		if entry.kind == Kind.BARRIER:
			store.end_input_batch()
			var outcome: Dictionary = store.input_barrier(entry.command, entry.args, entry.started)
			store.begin_input_batch()
			entry.started = true
			if outcome.get("pending", false):
				break # explicit barrier wait; no later input overtakes it and no synchronous drain
			entry.result = outcome.get("result")
			stop_after = outcome.get("stop_after", false)
		else:
			var args := entry.args.duplicate()
			if entry.kind == Kind.PATH or entry.kind == Kind.ZONE:
				args[0] = entry.first.lerp(entry.last, float(entry.next) / entry.steps)
			if entry.kind == Kind.TIMED:
				entry.clock.held -= entry.quantum # same serial subtraction as the former while, no dt coalescing/rounding
			var name := entry.command
			if entry.kind == Kind.ZONE:
				var lot: int = store.query(&"lot_at", [args[0]]) # selection observes the preceding committed samples
				if lot != 0:
					name = &"lot_zone"
					args = [lot, args[2]]
			entry.result = store.apply_input_command(name, args)
			_notify(entry)
			if entry.kind == Kind.TIMED:
				continue
			if (entry.kind == Kind.PATH or entry.kind == Kind.ZONE) and entry.next < entry.steps:
				entry.next += 1
				continue
		_pop()
		notices.append(entry)
		if stop_after:
			break # closing completion resumes its owner before any later operation can change what was saved
	store.end_input_batch()
	for entry: Entry in notices:
		entry.ticket.finish(entry.result) # snapshot publication precedes a result-dependent continuation
	_pumping = false

func _notify(entry: Entry) -> void:
	if entry.owner == null or entry.method == &"":
		return
	var target: Object = entry.owner.get_ref()
	if is_instance_valid(target):
		target.call(entry.method, entry.result)
