## The sheet of plate pictures: the core asks for a cell the moment a chunk needs a plate (render/props/sign_atlas.h); here
## the cells waiting are painted, the words with the canvas's font, into a viewport the "prop" material reads (sign_atlas).
## Only new cells are drawn: the viewport keeps what it had and updates once per batch.
class_name SignSheet
extends SubViewport

const TEXT_FILL := 0.72
const BORDER_INSET := 0.047 # of the cell's height
const BORDER_WIDTH := 0.06

var _paper := _Paper.new()
var _look: Dictionary


func _init(look: Dictionary) -> void:
	_look = look
	size = Vector2i(int(look.sheet_w_px), int(look.sheet_h_px))
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
	if not RenderingServer.frame_post_draw.is_connected(_paper.forget_glyphs):
		RenderingServer.frame_post_draw.connect(_paper.forget_glyphs, CONNECT_ONE_SHOT)


## Draws each plate as a real one reads: the shape's colours on its cell, cut by the plate's own outline in the mesh.
class _Paper:
	extends Node2D

	var look: Dictionary
	var cells: Array = []
	var painted := false
	var font: FontFile = ThemeDB.fallback_font.duplicate() # its own: the glyphs are big and the sheet keeps what they painted

	## The sheet holds the picture now: the big glyphs need not stay in video memory.
	func forget_glyphs() -> void:
		font.clear_cache()

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
			draw_rect(r.grow(-r.size.y * SignSheet.BORDER_INSET), Color(p.border), false, r.size.y * SignSheet.BORDER_WIDTH)
		var words: String = text if not text.is_empty() else p.get("words", "")
		if not words.is_empty():
			_words(words, r, Color(p.ink))

	func _words(words: String, r: Rect2, ink: Color) -> void:
		var px := int(r.size.y * SignSheet.TEXT_FILL)
		while px > 10 and font.get_string_size(words, HORIZONTAL_ALIGNMENT_LEFT, -1, px).x > r.size.x * 0.84:
			px -= 2
		var wide := font.get_string_size(words, HORIZONTAL_ALIGNMENT_LEFT, -1, px)
		var at := r.get_center() + Vector2(-wide.x * 0.5, font.get_ascent(px) - wide.y * 0.5)
		draw_string(font, at, words, HORIZONTAL_ALIGNMENT_LEFT, -1, px, ink)
