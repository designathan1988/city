// Integer hash shared by the building shaders (PCG, M. O'Neill 2014, as Jarzynski and Olano, "Hash Functions for GPU
// Rendering", JCGT 2020, recommend it).

uint pcg(uint v) {
	uint s = v * 747796405u + 2891336453u;
	uint w = ((s >> ((s >> 28u) + 4u)) ^ s) * 277803737u;
	return (w >> 22u) ^ w;
}
float h01(uint v) { return float(pcg(v)) / 4294967295.0; }
