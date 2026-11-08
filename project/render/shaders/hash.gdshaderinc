// The integer hash every shader shares (PCG, M. O'Neill 2014, as Jarzynski and Olano, "Hash Functions for GPU Rendering", JCGT
// 2020, recommend it): exact on every GPU, unlike fract(sin(...)). A lattice cell's seed by the primes of Teschner et al., "Optimized
// Spatial Hashing for Collision Detection of Deformable Objects", VMV 2003.
#ifndef HASH_INC
#define HASH_INC

// PCG's 32-bit RXS-M-XS output (O'Neill 2014, as Jarzynski and Olano give it): the LCG's multiplier and increment, the
// output's shift, multiplier and last shift; a word's top, 2^32 - 1; Teschner's two primes.
const uint PCG_MUL = 747796405u, PCG_INC = 2891336453u, PCG_SHIFT = 28u, PCG_OUT_MUL = 277803737u, PCG_OUT_SHIFT = 22u;
const float WORD_TOP = 4294967295.0;
const uint KNUTH_HASH = 2654435761u; // Knuth's multiplicative hash: 2^32 over the golden ratio
// The common sin hash (Rey 1998): fract(sin(dot(p, k)) * scale); three directions for a vec3, the first two for a vec2.
const vec3 SIN_HASH = vec3(12.9898, 78.233, 37.719);
const float SIN_HASH_SCALE = 43758.5453;
const uint TESCHNER_X = 73856093u, TESCHNER_Y = 19349663u;

uint pcg(uint v) {
	uint s = v * PCG_MUL + PCG_INC;
	uint w = ((s >> ((s >> PCG_SHIFT) + 4u)) ^ s) * PCG_OUT_MUL;
	return (w >> PCG_OUT_SHIFT) ^ w;
}
float h01(uint v) { return float(pcg(v)) / WORD_TOP; }
uint spatial_hash(uint x, uint y) { return x * TESCHNER_X ^ y * TESCHNER_Y; }

#endif
