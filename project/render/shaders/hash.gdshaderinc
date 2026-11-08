// The integer hash every shader shares (PCG, M. O'Neill 2014, as Jarzynski and Olano, "Hash Functions for GPU Rendering", JCGT
// 2020, recommend it): exact on every GPU, unlike fract(sin(...)). A lattice cell's seed by the primes of Teschner et al., "Optimized
// Spatial Hashing for Collision Detection of Deformable Objects", VMV 2003.
#ifndef HASH_INC
#define HASH_INC

uint pcg(uint v) {
	uint s = v * 747796405u + 2891336453u;
	uint w = ((s >> ((s >> 28u) + 4u)) ^ s) * 277803737u;
	return (w >> 22u) ^ w;
}
float h01(uint v) { return float(pcg(v)) / 4294967295.0; }
uint spatial_hash(uint x, uint y) { return x * 73856093u ^ y * 19349663u; }

#endif
