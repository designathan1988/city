## The Effects panel's switches (data/game/render_switches.json) on the renderer: each layer hidden by the camera's cull mask,
## each effect by the Environment, the Viewport or the sun, each system of the tick by the core. Every switch on is the game as
## it draws; nothing here runs per frame.
class_name RenderSwitches
extends RefCounted

## What the sun lights a shadow from when "bShadows" is off: everything but the buildings.
const BUILDING_LAYERS := ["building_masses", "building_facades", "building_shells", "building_parts", "building_details",
		"building_flags"]
const ALL_LAYERS := 0xFFFFF # the 20 render layers of a Camera3D's cull mask

var off: Dictionary = {} # the keys switched off
var resolution := 1.0
var far := false # the camera is past the tier's detail cut-off
var _lots := true
var globals := {} # the shader globals in force (the engine reads them back only in the editor)


func _init(kept: Dictionary) -> void:
	off = kept.get("off", {})
	resolution = kept.get("resolution", 1.0)


func saved() -> Dictionary:
	return {"off": off, "resolution": resolution}


func is_on(key: String) -> bool:
	return not off.has(key)


func set_on(key: String, on: bool) -> void:
	if on:
		off.erase(key)
	else:
		off[key] = true


## Every switch of a group, on or off.
func set_group(id: String, on: bool) -> void:
	for g: Dictionary in DataTables.game("render_switches").groups:
		if g.id == id:
			for item: Dictionary in g.items:
				set_on(item.key, on)


func reset() -> void:
	off = {}
	resolution = 1.0


## The cull mask of the switches and the tier: all layers but those of a switch that is off.
func cull_mask(tier: Dictionary) -> int:
	var names: Dictionary = CityStore.constants.render_layers
	var mask := ALL_LAYERS
	for g: Dictionary in DataTables.game("render_switches").groups:
		for item: Dictionary in g.items:
			if item.has("layers") and not is_on(item.key):
				for l: String in item.layers:
					mask &= ~int(names[l])
	if not tier.detail_props or far:
		mask &= ~int(names.furniture)
	if far:
		mask &= ~(int(names.vehicles) | int(names.people))
	return mask


func set_global(name: StringName, value: float) -> void:
	globals[name] = value
	RenderingServer.global_shader_parameter_set(name, value)


func _shadow_casters(tier: Dictionary) -> int:
	var names: Dictionary = CityStore.constants.render_layers
	var mask := ALL_LAYERS
	if not tier.agent_shadows:
		mask &= ~(int(names.vehicles) | int(names.people))
	if not is_on("bShadows"):
		for l: String in BUILDING_LAYERS:
			mask &= ~int(names[l])
	return mask


## Everything live: called after the tier (the tier sets the effects a switch can only take away).
func apply(world: CityWorld, viewport: Viewport, tier: Dictionary) -> void:
	var camera := viewport.get_camera_3d()
	if camera != null:
		camera.cull_mask = cull_mask(tier)
	viewport.use_occlusion_culling = is_on("occlusion") and tier.occlusion
	world.sun.shadow_caster_mask = _shadow_casters(tier)
	world.sun.shadow_enabled = tier.shadows and is_on("shadows")
	world.occlusion.turn(tier.ssao and tier.post and is_on("ambientOcclusion"))
	world.contact.turn(world.sun.shadow_enabled and tier.post and is_on("canyonLight"))
	if not is_on("smaa") or not tier.post:
		viewport.screen_space_aa = Viewport.SCREEN_SPACE_AA_DISABLED
	world.daylight.glow_on = is_on("bloom") and tier.post
	world.daylight.ambient_on = is_on("ambient")
	world.environment.reflected_light_source = Environment.REFLECTION_SOURCE_SKY if is_on("environmentMap") else Environment.REFLECTION_SOURCE_DISABLED
	world.daylight.relight()
	set_global(&"road_paint", 0.0 if far else 1.0)
	for g: Dictionary in DataTables.game("render_switches").groups:
		for item: Dictionary in g.items:
			if item.has("sim"):
				CityStore.set_sim_switch(item.sim, is_on(item.key))
			elif item.has("global"):
				set_global(StringName(item.global), 1.0 if is_on(item.key) else 0.0)
			elif item.has("lots") and _lots != is_on(item.key):
				_lots = is_on(item.key)
				CityStore.dispatch(&"set_lots_drawn", [_lots])


## The detail cut-off: the camera crossing the tier's zoom, in either direction, redraws the mask; nothing else happens per frame.
func follow_zoom(world: CityWorld, viewport: Viewport, q: Dictionary) -> void:
	var camera := viewport.get_camera_3d()
	var rig := camera.get_parent() as CityCamera if camera != null else null
	if rig == null:
		return
	var beyond: bool = rig.px_per_m() < q.detail_cutoff_zoom
	if beyond != far:
		far = beyond
		apply(world, viewport, q)
