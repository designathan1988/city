## The sheet of plate pictures: the core asks for a cell the moment a chunk needs a plate (render/props/sign_atlas.h); here
## the cells waiting are painted, the words with the canvas's font, into a viewport the "prop" material reads (sign_atlas).
## Only new cells are drawn: the viewport keeps what it had and updates once per batch.
class_name SignSheet
extends SubViewport

const SHEET_PX := 2048
const TEXT_FILL := 0.72

var _paper := _Paper.new()
var _look: Dictionary


func _init(look: Dictionary) -> void:
	_look = look
	size = Vector2i(SHEET_PX, SHEET_PX)
	transparent_bg = false
	disable_3d = true
	render_target_clear_mode = SubViewport.CLEAR_MODE_ONCE
	render_target_update_mode = SubViewport.UPDATE_DISABLED
	_paper.look = look
	add_child(_paper)


func _process(_delta: float) -> void:
	var cells: Array = CityStore.query(&"sign_cells")
	if cells.is_empty():
		return
	_paper.cells = cells
	_paper.queue_redraw()
	render_target_clear_mode = SubViewport.CLEAR_MODE_NEVER if _paper.painted else SubViewport.CLEAR_MODE_ONCE
	_paper.painted = true
	render_target_update_mode = SubViewport.UPDATE_ONCE


## Draws each plate as a real one reads: the shape's colours on its cell, cut by the plate's own outline in the mesh.
class _Paper:
	extends Node2D

	var look: Dictionary
	var cells: Array = []
	var painted := false

	func _draw() -> void:
		for c: Array in cells:
			_plate(c[0], c[1], c[2])

	func _plate(type: int, text: String, r: Rect2) -> void:
		var p: Dictionary = look.drawings[type]
		var mid := r.get_center()
		var radius := minf(r.size.x, r.size.y) * 0.5
		draw_rect(r, Color(p.field))
		if p.has("ring"):
			draw_circle(mid, radius, Color(p.ring))
			draw_circle(mid, radius * p.ring_inner, Color(p.field))
		if p.has("bar"):
			var bar := Vector2(radius * 1.3, radius * 0.32)
			draw_rect(Rect2(mid - bar * 0.5, bar), Color(p.bar))
		if p.has("slash"):
			draw_line(mid - Vector2.ONE * radius * 0.6, mid + Vector2.ONE * radius * 0.6, Color(p.slash), radius * 0.16)
		if p.has("border"):
			draw_rect(r.grow(-6.0), Color(p.border), false, 8.0)
		var words: String = text if not text.is_empty() else p.get("words", "")
		if not words.is_empty():
			_words(words, r, Color(p.ink))

	func _words(words: String, r: Rect2, ink: Color) -> void:
		var font: Font = ThemeDB.fallback_font
		var px := int(r.size.y * SignSheet.TEXT_FILL)
		while px > 10 and font.get_string_size(words, HORIZONTAL_ALIGNMENT_LEFT, -1, px).x > r.size.x * 0.84:
			px -= 2
		var wide := font.get_string_size(words, HORIZONTAL_ALIGNMENT_LEFT, -1, px)
		var at := r.get_center() + Vector2(-wide.x * 0.5, font.get_ascent(px) - wide.y * 0.5)
		draw_string(font, at, words, HORIZONTAL_ALIGNMENT_LEFT, -1, px, ink)
