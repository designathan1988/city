## The Effects panel's switches (data/game/render_switches.json) on the renderer: each layer hidden by the camera's cull mask,
## each effect by the Environment, the Viewport or the sun, each system of the tick by the core. Every switch on is the game as
## it draws; nothing here runs per frame.
class_name RenderSwitches
extends RefCounted

## What the sun lights a shadow from when "bShadows" is off: everything but the buildings.
const BUILDING_LAYERS := ["building_masses", "building_facades", "building_shells", "building_parts", "building_details"]
const ALL_LAYERS := 0xFFFFF # the 20 render layers of a Camera3D's cull mask

var off: Dictionary = {} # the keys switched off
var resolution := 1.0


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
	if not tier.detail_props:
		mask &= ~int(names.furniture)
	return mask


func _shadow_casters() -> int:
	var names: Dictionary = CityStore.constants.render_layers
	var mask := ALL_LAYERS
	if not is_on("bShadows"):
		for l: String in BUILDING_LAYERS:
			mask &= ~int(names[l])
	return mask


## Everything live: called after the tier (the tier sets the effects a switch can only take away).
func apply(world: CityWorld, viewport: Viewport, tier: Dictionary) -> void:
	var camera := viewport.get_camera_3d()
	if camera != null:
		camera.cull_mask = cull_mask(tier)
	world.sun.shadow_caster_mask = _shadow_casters()
	world.sun.shadow_enabled = tier.shadows and is_on("shadows")
	world.environment.ssao_enabled = tier.ssao and is_on("ambientOcclusion")
	if not is_on("smaa"):
		viewport.screen_space_aa = Viewport.SCREEN_SPACE_AA_DISABLED
	world.set_light_switches(is_on("bloom"), is_on("environmentMap"), is_on("ambient"))
	for g: Dictionary in DataTables.game("render_switches").groups:
		for item: Dictionary in g.items:
			if item.has("sim"):
				CityStore.set_sim_switch(item.sim, is_on(item.key))
