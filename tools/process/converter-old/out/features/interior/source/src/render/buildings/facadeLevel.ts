// The code of src/render/buildings/facadeLevel.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/facadeLevel.ts:33-67 slice of src/render/buildings/facadeLevel.ts#facadeFragmentPars (shared): the feature's statements; "…" marks code left out
function facadeFragmentPars(p: FacadePanes): string {
  return `
uniform float roomsAwake;
uniform float massingNight;
uniform float roomDaylight;
varying vec4 vFacadeGrid;
varying vec4 vFacadeKey;
varying vec4 vFacadeOpening;
varying vec4 vFacadeTrim;
varying vec3 vFacadeWorld;
// The pixel's shares: wall, frame, glass, door or shutter; the glass's darker pane; the room's light.
float facadeWall = 1.0;
float facadeFrame = 0.0;
float facadeGlass = 0.0;
float facadeSolid = 0.0;
float facadeDark = 0.0;
float facadeShutter = 0.0;
float facadeLit = 0.0;
float facadeStreak = 1.0;
vec3 facadeLamp = vec3(1.0);
${ROOM_LAMP_GLSL}
${PULSE_GLSL('facade')}
// buildingMesh.ts bayHash, to the bit: Math.imul is a uint product, >>> a uint shift.
uint facadeBayHash(int volume, int level, int side, int index, int x7, int y7) {
  uint h = (uint(volume + 1) * 0x27d4eb2du) ^ (uint(level + 7) * 0x165667b1u) ^ (uint(side + 3) * 0x3c6ef372u) ^ (uint(index + 11) * 0x85ebca6bu);
  h ^= (uint(x7) * 0x2c1b3c6du) ^ (uint(y7) * 0x297a2d39u);
  h = (h ^ (h >> 15u)) * 0x85ebca6bu;
  return h ^ (h >> 13u);
}
const vec3 FACADE_GLASS = ${vec3Of(p.glass.color)};
const vec3 FACADE_GLASS_DARK = ${vec3Of(p.glassDark.color)};
const vec3 FACADE_DOOR = ${vec3Of(p.door.color)};
const vec3 FACADE_SHUTTER = ${vec3Of(p.shutter.color)};
`;
}
