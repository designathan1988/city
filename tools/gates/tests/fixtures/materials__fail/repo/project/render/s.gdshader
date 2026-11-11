// A shader whose uniforms the table's params must name; one more comes from its include.
shader_type spatial;
#include "res://render/parts.gdshaderinc"
uniform vec3 tint : source_color;
uniform float speed;
// uniform float hidden;
