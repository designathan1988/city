// What a mesh carries as bytes in a colour channel or a custom attribute.
#ifndef BYTES_INC
#define BYTES_INC

const float BYTE_TOP = 255.0; // an 8-bit UNORM channel reads byte / 255
const int BYTE_SPAN = 256, TWO_BYTES_SPAN = 65536; // the bytes of an id run over 2^8 and 2^16
const float BYTE_STEPS = 256.0; // a byte packed in a float's fraction reads fraction * 256
const int BYTE_BITS = 8, BYTE_MASK = 255; // a byte's bits, and all of them set
const uint BYTE_SHIFT = 8u, BYTE_MASK_U = 255u; // the same as uints

#endif
