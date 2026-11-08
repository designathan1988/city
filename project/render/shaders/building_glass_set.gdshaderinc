// The glass of the near and mid levels, as data/render/materials.json's `glass` and `sky_in_glass` sets give it to every material
// that draws it (building, building_glass): one declaration for both bodies.
uniform float window_glow; // how bright a lit room's lamp shows through its glass
uniform float glass_reflect; // the least share of the sky the glass reflects
uniform vec3 glass_sky : source_color; // the sky the glass reflects (building_sky_glass.gdshaderinc)
uniform float glass_sky_floor; // what the glass reflects of it head-on, over its Fresnel share
uniform float dark_glass; // what a dark pane lets through of the room behind it
