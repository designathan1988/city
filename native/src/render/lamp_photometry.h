// A lamp's light in the renderer's unit (tuning.json render.light_unit_lx, the illuminance a light of energy 1 gives a surface
// square to it): one owner of the street lamp's and the headlamp's photometry, for their lights (render/props/lamp_lights.cpp,
// render/agents/head_lights.cpp), the pools the road draws past them and the lamp's own lens (render/props/prop_renderer.cpp).
// Godot multiplies a spot's energy by pi and divides the diffuse BRDF by pi (servers/rendering/renderer_rd/storage_rd/
// light_storage.cpp, 4.7.2), so a white surface square to a spot of energy e at d metres, decay 2, shows the radiance e / d^2: the
// illuminance I / d^2 of a lamp of intensity I is the energy I / unit. A radiance of 1 is a white diffuse surface under an energy
// of 1: the luminance unit / pi.
#pragma once

#include "core/math.h"
#include "core/units.h"
#include "sim/catalog/catalog.h"

#include <cmath>

namespace city {

// The solid angle of a cone of half angle `deg` (a spot's spot_angle): 2 pi (1 - cos).
inline double cone_solid_angle(double deg) { return 2.0 * PI_D * (1.0 - std::cos(deg * DEG_TO_RAD_D)); }

// The street lamp's flux spread evenly over its cone, in candela.
inline double lamp_intensity_cd(const Tuning &t) { return t.render_lamp_flux_lm / cone_solid_angle(t.render_lamp_cone_deg); }

// A spot's energy for an intensity in candela.
inline double light_energy(double cd, const Tuning &t) { return cd / t.render_light_unit_lx; }

// A luminance in cd/m^2 as the renderer's radiance.
inline double luminance_radiance(double cd_m2, const Tuning &t) { return cd_m2 * PI_D / t.render_light_unit_lx; }

// The illuminance under a street lamp `height_m` up, straight below it, as the radiance of a white floor: I / h^2.
inline double lamp_foot_light(const Tuning &t, double height_m) { return light_energy(lamp_intensity_cd(t), t) / (height_m * height_m); }

// The street lamp's lens as a light: its intensity over its lit area, as a radiance.
inline double lamp_lens_light(const Tuning &t) { return luminance_radiance(lamp_intensity_cd(t) / t.render_lamp_lens_m2, t); }

} // namespace city
