// What a smooth dielectric (glass, a car's clear coat) reflects at a view angle: Schlick's approximation of Fresnel's
// equations (C. Schlick, "An Inexpensive BRDF Model for Physically-based Rendering", Computer Graphics Forum 13(3), 1994).
#ifndef FRESNEL_INC
#define FRESNEL_INC

const float DIELECTRIC_F0 = 0.04; // the reflectance head-on of index 1.5: ((1.5 - 1) / (1.5 + 1))^2
const float SCHLICK_POWER = 5.0; // Schlick 1994

// The share reflected by a surface of head-on reflectance `f0`, `cosv` the cosine between the view and the normal.
float schlick_f0(float f0, float cosv) { return f0 + (1.0 - f0) * pow(1.0 - cosv, SCHLICK_POWER); }

// The share an uncoated dielectric reflects.
float schlick(float cosv) { return schlick_f0(DIELECTRIC_F0, cosv); }

#endif
