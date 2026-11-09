# The textures of scripts/replay.gd as three.js's GPU held them: each uploaded through the RenderingDevice in the format of its
# captured kind (sRGB decoded by a shareable view, like WebGL; uint and int as integer formats; arrays; every mip level), kept
# by its files, and freed after their Texture2DRD wrappers when the replay ends. Copied beside replay.gd (frame-replay.mjs).
# A texture whose kind is unknown, or whose captured bytes are not its levels' size, is refused and told (push_error): the
# capture is wrong, and a texture padded or cut to fit would draw something three never drew.
extends RefCounted

# Bytes a texel of each captured kind (frame-replay.mjs writes them so): RGBA of 8, 16 and 32 bits a channel.
const TEXEL_BYTES := { "unorm": 4, "uint16": 8, "uint": 16, "int": 16, "float": 16 }

var rd: RenderingDevice
var cache := {}
# the RenderingDevice textures made here, freed by release() (after their Texture2DRD wrappers)
var rids: Array[RID] = []


func _init() -> void:
	rd = RenderingServer.get_rendering_device()


## The RenderingDevice format of a captured kind. UNORM storage is made shareable as its sRGB view when three decoded it so:
## Godot views a Texture2DRD through its Image format (UNORM) and samples the sRGB view (decoded by the hardware, like WebGL's
## SRGB8_ALPHA8) for a source_color uniform. 16-bit uint is the widest integer format Godot 4.7's Image maps (FORMAT_RGBA16I).
func _format(t: Dictionary) -> RDTextureFormat:
	var fmt := RDTextureFormat.new()
	match t.kind:
		"unorm":
			fmt.format = RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM
			if t.get("srgb", false):
				fmt.add_shareable_format(RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM)
				fmt.add_shareable_format(RenderingDevice.DATA_FORMAT_R8G8B8A8_SRGB)
		"uint16": fmt.format = RenderingDevice.DATA_FORMAT_R16G16B16A16_UINT
		"uint": fmt.format = RenderingDevice.DATA_FORMAT_R32G32B32A32_UINT
		"int": fmt.format = RenderingDevice.DATA_FORMAT_R32G32B32A32_SINT
		"float": fmt.format = RenderingDevice.DATA_FORMAT_R32G32B32A32_SFLOAT
	var levels: Array = t.levels
	fmt.width = int(levels[0].width)
	fmt.height = int(levels[0].height)
	fmt.mipmaps = levels.size()
	fmt.array_layers = int(t.get("layers", 1))
	fmt.texture_type = RenderingDevice.TEXTURE_TYPE_2D_ARRAY if t.get("array", false) else RenderingDevice.TEXTURE_TYPE_2D
	fmt.usage_bits = RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice.TEXTURE_USAGE_CAN_UPDATE_BIT
	return fmt


## A texture as three's GPU held it: every level of every layer, from the captured files. Null, told, when the capture is wrong.
func texture(t: Dictionary) -> Texture:
	var key := JSON.stringify(t.levels)
	if cache.has(key):
		return cache[key]
	if not TEXEL_BYTES.has(t.kind):
		push_error("REPLAY texture %s: unknown kind %s" % [t.uniform, t.kind])
		return null
	var data := []
	for layer in int(t.get("layers", 1)):
		var bytes := PackedByteArray()
		for l in t.levels:
			var name = l.files[layer] if l.has("files") else l.file
			var b := FileAccess.get_file_as_bytes("res://data/" + name)
			var want := int(l.width) * int(l.height) * int(TEXEL_BYTES[t.kind])
			if b.size() != want:
				push_error("REPLAY texture %s: %s holds %d bytes, its level wants %d" % [t.uniform, name, b.size(), want])
				return null
			bytes.append_array(b)
		data.append(bytes)
	var rid := rd.texture_create(_format(t), RDTextureView.new(), data)
	if not rid.is_valid():
		push_error("REPLAY texture not created: " + t.uniform)
		return null
	rids.append(rid)
	var tex: Texture
	if t.get("array", false):
		tex = Texture2DArrayRD.new()
		tex.texture_rd_rid = rid
	else:
		tex = Texture2DRD.new()
		tex.texture_rd_rid = rid
	cache[key] = tex
	return tex


## Every texture made here freed: the wrappers first (they free through the RenderingServer), then the RenderingDevice's.
func release() -> void:
	cache.clear()
	for r in rids:
		if rd.texture_is_valid(r):
			rd.free_rid(r)
	rids.clear()
