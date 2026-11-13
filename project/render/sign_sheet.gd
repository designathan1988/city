## The sheets of plate pictures: the core asks for a cell the moment a chunk needs a plate (render/props/sign_atlas.h); here the
## cells waiting are painted (render/sign_paper.gd draws them), the words with the canvas's font, into a viewport, and the viewport's
## picture goes into its layer of the array the "prop" material reads (sign_atlas). Each layer's picture is also kept here, so the
## array is remade from these when the core opens a layer past its room (the room doubling each time) and a round on a layer begun
## lays what it holds under the new cells, with nothing read back from the GPU; the viewport's picture is the one read back, at most
## one a round (sheet_round_s). Between rounds the viewport shrinks to nothing. A new city (the core's sheets under a new generation)
## drops what was waiting and what the layers held. The words are the player's language: the player's own as written, a name the
## city gave and a plate's own words through the translations (the core keeps keys), so a change of language paints every cell again.
class_name SignSheet
extends SubViewport

const LAYER_FORMAT := Image.FORMAT_RGBA8
const IDLE_SIZE := Vector2i.ONE
## A cell as the core gives it (sign_atlas.h take_pending): [type, the player's words, rectangle in its layer, layer, generation,
## a given name's form, name and number] (the last three keys of the translations and a number, sim/street/street_works.h GivenName).
const CELL_TYPE := 0
const CELL_TEXT := 1
const CELL_PLACE := 2
const CELL_LAYER := 3
const CELL_GENERATION := 4
const CELL_FORM := 5
const CELL_NAME := 6
const CELL_ORDINAL := 7

var drawn: Array = [] ## Every cell painted in this city, as the core gave it (the captures check their ink).
var layers := Texture2DArray.new()

var _paper := SignPaper.new()
var _look: Dictionary
var _material: ShaderMaterial
var _waiting: Array = []
var _begun := -1 ## The last layer painted: the only one with cells to keep.
var _busy := false
var _since := 0.0
var _pictures: Array[Image] = [] ## What each layer of `layers` holds, kept as uploaded: its room is their count.
var _generation := -1 ## The core's generation of the sheets these layers hold.
var _painted := false ## Something of this generation was laid on the layers.


func _init(look: Dictionary, material: ShaderMaterial) -> void:
	_look = look
	_material = material
	size = IDLE_SIZE
	transparent_bg = false
	disable_3d = true
	render_target_clear_mode = SubViewport.CLEAR_MODE_ALWAYS
	render_target_update_mode = SubViewport.UPDATE_DISABLED
	_paper.look = look
	_paper.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	add_child(_paper)
	_grow(1)


## The language changed: every cell painted is painted again in it, layer by layer from the first, each layer laid fresh (its
## first round keeps nothing of the old picture), before the cells still waiting.
func _notification(what: int) -> void:
	if what != NOTIFICATION_TRANSLATION_CHANGED or drawn.is_empty():
		return
	_waiting = drawn + _waiting
	drawn = []
	_begun = -1


func _process(delta: float) -> void:
	_since += delta
	if _busy or _since < _look.sheet_round_s:
		return
	var fresh: Array = CityStore.query(&"sign_cells") # once a round, as they are drawn: an Array across the DLL otherwise
	if not fresh.is_empty() and int(fresh[0][CELL_GENERATION]) != _generation: # the core takes them all under its generation now
		_new_city(int(fresh[0][CELL_GENERATION]))
	_waiting.append_array(fresh)
	if _waiting.is_empty():
		_since = 0.0
		return
	var layer: int = _waiting[0][CELL_LAYER]
	var cut := 0
	while cut < _waiting.size() and _waiting[cut][CELL_LAYER] == layer:
		cut += 1
	var batch := _waiting.slice(0, cut)
	_waiting = _waiting.slice(cut)
	drawn.append_array(batch)
	var kept: Image = _pictures[layer] if layer == _begun and layer < _pictures.size() else null
	_paper.begin(batch, ImageTexture.create_from_image(kept) if kept else null)
	_begun = layer
	size = _layer_size()
	_busy = true
	render_target_update_mode = SubViewport.UPDATE_ONCE
	RenderingServer.frame_post_draw.connect(_upload.bind(layer, _generation), CONNECT_ONE_SHOT)


## The viewport has drawn: its picture goes to its layer (unless the city changed meanwhile), and the fonts and the drawing go.
func _upload(layer: int, generation: int) -> void:
	if generation == _generation:
		var image := get_texture().get_image()
		image.convert(LAYER_FORMAT)
		if layer >= _pictures.size():
			_grow(layer + 1)
		_pictures[layer] = image
		layers.update_layer(image, layer)
		_painted = true
	_paper.end()
	size = IDLE_SIZE
	_busy = false
	_since = 0.0


## The core's sheets started again for another city: nothing waiting or laid belongs to it, and its first cells reuse the layers'
## places, so the layers are emptied before any of its plates reads them.
func _new_city(generation: int) -> void:
	_generation = generation
	_waiting = []
	_begun = -1
	drawn = []
	if _painted:
		_painted = false
		_pictures = []
		_grow(1)


func _layer_size() -> Vector2i:
	return Vector2i(int(_look.sheet_w_px), int(_look.sheet_h_px))


## The array with room for at least `count` layers, twice the room it had while that is less: a texture's layers are fixed when
## made, so it is made again from the pictures kept, the new layers empty.
func _grow(count: int) -> void:
	var room := maxi(_pictures.size(), 1)
	while room < count:
		room *= 2
	while _pictures.size() < room:
		_pictures.append(Image.create_empty(_layer_size().x, _layer_size().y, false, LAYER_FORMAT))
	layers = Texture2DArray.new()
	layers.create_from_images(_pictures)
	_material.set_shader_parameter("sign_atlas", layers)
