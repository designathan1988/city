## The graphics tiers on the running game: a tier's table (data/game/settings.json "tiers") with the player's kept choices and
## the optimisations over it, and each of its fields on the renderer that owns it: the Viewport (MSAA, SMAA, 3D scale,
## anisotropy), the sun (shadows), the Environment (SSAO), the RenderingServer (shadow atlas, SSAO quality) and the core.
class_name QualityTier
extends RefCounted

const MSAA_MODES := [Viewport.MSAA_DISABLED, Viewport.MSAA_2X, Viewport.MSAA_4X, Viewport.MSAA_8X]
const FILTER_LEVELS := [Viewport.ANISOTROPY_DISABLED, Viewport.ANISOTROPY_2X, Viewport.ANISOTROPY_4X, Viewport.ANISOTROPY_8X,
	Viewport.ANISOTROPY_16X]
## Screen pixels per inch of a 1:1 display (Windows' 96 DPI): what a device pixel ratio is measured against.
const BASE_DPI := 96.0
const MS_PER_S := 1000.0
## Four cascades tile an atlas of twice their side (PSSM 4 splits).
const ATLAS_PER_CASCADE := 2
const COMPATIBILITY := "gl_compatibility"
const FORWARD_PLUS := "forward_plus"
## settings.json tiers "temporal": what resolves the shimmer of edges, shaders and shadows over the frames. FSR2 at the full resolution
## is the sharper of the two, and pays its own pass (Godot docs: FSR2 as the antialiasing when the GPU has room).
const TEMPORAL_NONE := 0
const TEMPORAL_TAA := 1
const TEMPORAL_FSR2 := 2
const FALLBACK_AA := 1 # where SMAA does not exist the same tier asks for this entry of MSAA_MODES
const MSAA_SETTING := "rendering/anti_aliasing/quality/msaa_3d" # what the engine's pipeline precompilation reads (apply)
## pedestrian_detail 0: the people's nearest level is this one (their far level).
const PEOPLE_FAR := 1


## The tier the GPU's name gives: the first rule that matches, else gpu_default.
static func tier_for_gpu(cfg: Dictionary, name: String) -> String:
	for rule: Array in cfg.gpu_rules:
		var re := RegEx.create_from_string(rule[0])
		if re != null and re.search(name) != null:
			return rule[1]
	return cfg.gpu_default


## The tier a preset stands for: auto is the one the GPU gives.
static func level_of(preset: String, hardware: String) -> String:
	return hardware if preset == "auto" else preset


## The GPU's last frame against the budget: the refresh rate or the player's limit.
static func budget_ms(fps_limit: int) -> float:
	var hz := DisplayServer.screen_get_refresh_rate()
	var fps := float(fps_limit) if fps_limit > 0 else (hz if hz > 0.0 else float(DataTables.game("settings").frame_cap.max_fps))
	return MS_PER_S / fps


## The tier's fields with the kept choices over them; `shadows` kept 0 is no shadows, ao and smaa force their pass, `temporal`
## its filter (TEMPORAL_*).
static func merged(tier: Dictionary, kept: Dictionary) -> Dictionary:
	var out := tier.duplicate()
	if kept.has("shadows"):
		out.shadows = int(kept.shadows) > 0
		if out.shadows:
			out.shadow_px = int(kept.shadows)
	if kept.has("ssao"):
		out.ssao = kept.ssao
	if kept.has("smaa"):
		out.smaa = kept.smaa
	if kept.has("temporal"):
		out.temporal = int(kept.temporal)
	return out


## The tier in force: its table with the kept choices and the Effects panel's options over it (data/game/optimizations.json).
static func resolved(level: String, kept: Dictionary, optimizations: Dictionary) -> Dictionary:
	var table: Dictionary = DataTables.game("optimizations")
	var q: Dictionary = table.defaults.duplicate()
	q.merge(merged(DataTables.game("settings").tiers[level], kept), true)
	for o: Dictionary in table.options:
		if optimizations.has(o.id):
			q[o.field] = q[o.field] * optimizations[o.id] if o.get("scale", false) else optimizations[o.id]
	return q


## The 3D scale the tier's pixel ratio cap allows on this screen (1 on a display of one pixel per CSS pixel).
static func cap_scale(pixel_ratio: float) -> float:
	var dpi := float(DisplayServer.screen_get_dpi())
	var ratio := maxf(1.0, dpi / BASE_DPI)
	return minf(1.0, pixel_ratio / ratio)


## SMAA: Forward+ and Mobile only (Godot docs, 3D antialiasing).
static func has_smaa() -> bool:
	return RenderingServer.get_current_rendering_method() != COMPATIBILITY


## TAA and FSR2 read the motion vectors of Forward+ alone (Godot docs, 3D antialiasing).
static func has_temporal() -> bool:
	return RenderingServer.get_current_rendering_method() == FORWARD_PLUS


## The cabin level's reach: where a vehicle stands at occupant_zoom, drawn out by the error the tier accepts (a fixed shape's error on
## the screen falls with the distance).
static func cabin_reach(q: Dictionary, height_px: float) -> float:
	var cfg: Dictionary = DataTables.game("camera")
	return CameraProfile.metres_at(cfg, q.occupant_zoom, height_px) * 0.5 / q.vehicle_error_px


## Everything a tier sets, live.
static func apply(q: Dictionary, world: CityWorld, viewport: Viewport, dynamic_scale: float) -> void:
	world.daylight.shadows_on = q.shadows
	world.daylight.shadow_least_m = q.shadow_least_m
	RenderingServer.directional_shadow_atlas_set_size(int(q.shadow_px) * ATLAS_PER_CASCADE, true)
	RenderingServer.directional_soft_shadow_filter_set_quality(int(q.shadow_filter) as RenderingServer.ShadowQuality)
	world.daylight.shadow_side_px = float(q.shadow_px)
	world.daylight.set_filter(DataTables.game("settings").filter_radius[int(q.shadow_filter)])
	var temporal: int = int(q.temporal) if has_temporal() else TEMPORAL_NONE
	# one filter resolves the edges: where the frames do (TAA, FSR2), SMAA over them was a second full-screen pass that only softened
	var smaa: bool = q.smaa and has_smaa() and temporal == TEMPORAL_NONE
	viewport.msaa_3d = Viewport.MSAA_DISABLED if temporal == TEMPORAL_FSR2 else MSAA_MODES[maxi(int(q.msaa), FALLBACK_AA if q.smaa and not has_smaa() else 0)]
	# The engine precompiles every mesh's pipelines for the project's MSAA (Godot render_forward_clustered.cpp update():
	# _update_global_pipeline_data_requirements_from_project reads this setting each frame), never the viewport's: told the same here, or at a
	# tier with MSAA no precompiled pipeline fits the frame and each is made on the frame its mesh is first drawn.
	ProjectSettings.set_setting(MSAA_SETTING, viewport.msaa_3d)
	viewport.screen_space_aa = Viewport.SCREEN_SPACE_AA_SMAA if smaa else Viewport.SCREEN_SPACE_AA_DISABLED
	viewport.anisotropic_filtering_level = FILTER_LEVELS[int(q.anisotropy)]
	viewport.use_taa = temporal == TEMPORAL_TAA
	viewport.scaling_3d_mode = Viewport.SCALING_3D_MODE_FSR2 if temporal == TEMPORAL_FSR2 else Viewport.SCALING_3D_MODE_BILINEAR
	viewport.fsr_sharpness = float(q.temporal_sharpness)
	# FSR2 and TAA sharpen every texture by a negative mipmap bias, counting on the frames to resolve it; a tiled wall under a pixel turns to moire
	viewport.texture_mipmap_bias = float(q.temporal_mip_bias) if temporal != TEMPORAL_NONE else 0.0
	viewport.scaling_3d_scale = cap_scale(q.pixel_ratio) * dynamic_scale
	EffectMaterials.cover_by_frames(world.materials(), temporal != TEMPORAL_NONE)
	world.occlusion.turn(q.ssao)
	world.sky.clouds_drawn = q.sky_clouds or q.cloud_shadows
	world.environment.ssil_enabled = bool(q.ssil) # the light the walls and the street throw back on each other: shade stops being one flat blue
	CityStore.dispatch(&"set_detail_scale", [sqrt(float(q.triangle_share))])
	var lamps: bool = world.contact.available() # a mesh takes 8 lights in Compatibility
	CityStore.dispatch(&"set_lamp_lights", [int(q.lamp_lights) if lamps else 0, int(q.lamp_shadows) if lamps else 0, int(q.head_lights) if lamps else 0])
	CityStore.dispatch(&"set_vegetation", [float(q.vegetation)])
	CityStore.dispatch(&"set_agent_detail", [cabin_reach(q, viewport.get_visible_rect().size.y), int(q.vehicle_whole_cap),
			0 if q.pedestrian_detail > 0 else PEOPLE_FAR])
	CityStore.dispatch(&"set_population", [float(q.population)])
	CityStore.dispatch(&"set_upload_every", [int(q.upload_every)])
	CityStore.set_sim_switch("crowd", q.crowd)
	world.water.set_plain(q.water_plain)
	world.set_terrain_lod(int(q.terrain_lod))
