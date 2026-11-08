// The glass of the near and mid levels, as data/render/materials.json's `glass` set gives it to every material that draws it
// (building, building_glass): one declaration for both bodies. The glass itself is a mirror at its glazing's reflectance
// (building_lamps.gdshaderinc glazing_f0); what is here is what a dark pane lets through of the room behind it.
uniform float dark_glass;
