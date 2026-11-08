// What a smooth dielectric (glass, a car's clear coat) reflects at a view angle: Schlick's approximation of Fresnel's
// equations (C. Schlick, "An Inexpensive BRDF Model for Physically-based Rendering", Computer Graphics Forum 13(3), 1994).
#ifndef FRESNEL_INC
#define FRESNEL_INC

const float DIELECTRIC_F0 = 0.04; // the reflectance head-on of index 1.5: ((1.5 - 1) / (1.5 + 1))^2
const float SCHLICK_POWER = 5.0; // Schlick 1994

// The share reflected, `cosv` the cosine between the view and the normal.
float schlick(float cosv) { return DIELECTRIC_F0 + (1.0 - DIELECTRIC_F0) * pow(1.0 - cosv, SCHLICK_POWER); }

#endif
