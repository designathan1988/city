// The air of sky_model.gdshaderinc as the shader globals render/air.gd publishes, for a pass or a surface that looks through it.
// Needs sky_model.gdshaderinc.
#ifndef AIR_GLOBALS_INC
#define AIR_GLOBALS_INC

global uniform vec3 solar_direction;
global uniform vec3 solar_top;
global uniform vec4 air_rayleigh;
global uniform vec4 air_mie;
global uniform vec4 air_ozone;
global uniform vec3 air_ground;

Air air_now() {
	return Air(solar_direction, solar_top, air_rayleigh, air_mie, air_ozone, air_ground);
}

#endif
