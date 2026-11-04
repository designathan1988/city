// The physics of one explosion, the only owner of how hard a blast hits at a distance (data/catalog/blast.json):
// Hopkinson-Cranz scaling and the Kingery-Bulmash surface-burst curves, and what an overpressure does to a building, a
// person, a vehicle or a pole. Pure functions of the parameters: no state, no randomness (callers draw with a hash).
#pragma once

#include "sim/catalog/blast_params.h"

namespace city::blast {

enum class Surface : uint8_t { EARTH = 0, HARD = 1 }; // what a crater is dug into

// TNT mass (kg) of the player's force `strength` (clamped to the data's range).
float charge_kg(const BlastParams &p, float strength);
// Scaled distance Z = r / W^(1/3), m per kg^(1/3).
float scaled_distance(float kg, float r);
// Incident overpressure (kPa) and impulse (kPa-ms) at ground distance `r` m from `kg` of TNT on the surface.
struct Airblast {
	float pressure_kpa = 0, impulse_kpa_ms = 0, duration_ms = 0;
};
Airblast airblast(const BlastParams &p, float kg, float r);
// The pressure a structure feels from that blow: the peak, in part when it is over before the structure answers.
float felt_pressure(const BlastParams &p, const Airblast &a);
// The farthest ground distance (m) where the overpressure still reaches `kpa`; 0 when even the nearest curve point is lower.
float range_at_pressure(const BlastParams &p, float kg, float kpa);
// Probability (0..1) of death, and of a ruptured eardrum, at an overpressure.
float lethality(const BlastParams &p, float pressure_kpa);
float eardrum(const BlastParams &p, float pressure_kpa);
// How far the bang still startles (140 dB re 20 uPa), capped by the data's window.
float startle_range(const BlastParams &p, float kg);
// Crater of a surface burst: radius and depth in metres.
float crater_radius(const BlastParams &p, float kg, Surface s);
float crater_depth(const BlastParams &p, float kg, Surface s);
// Standard normal cumulative distribution.
float normal_cdf(float z);

} // namespace city::blast
