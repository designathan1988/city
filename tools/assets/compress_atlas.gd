## The cast's atlases, as the game reads them (project/assets/people/atlas.bin): each PNG with its mipmaps, compressed to DXT5, in the
## order of the names given. No decoding at boot and a quarter of the video memory of raw colours. Run with the editor's binary, which has
## the compressors: godot --headless --path project -s tools/assets/compress_atlas.gd -- <png dir> <out file> <name> <name> ...
## Layout (little-endian): u32 count, u32 side, u32 bytes each, then the images, mipmaps included.
extends SceneTree


func _init() -> void:
	var args := OS.get_cmdline_user_args()
	var names := args.slice(2)
	var out := FileAccess.open(args[1], FileAccess.WRITE)
	var side := 0
	var images: Array[PackedByteArray] = []
	for name in names:
		var image := Image.load_from_file("%s/%s.png" % [args[0], name])
		image.generate_mipmaps()
		image.compress(Image.COMPRESS_S3TC, Image.COMPRESS_SOURCE_SRGB)
		print(name, " ", image.get_format(), " ", image.get_width())
		side = image.get_width()
		images.push_back(image.get_data())
	out.store_32(images.size())
	out.store_32(side)
	out.store_32(images[0].size())
	for data in images:
		out.store_buffer(data)
	out.close()
	quit()
