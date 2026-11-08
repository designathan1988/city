## The eye's adaptation as the picture's pre-exposure (CameraAttributes.exposure_multiplier: both renderers apply it to the lights, the
## sky and its ambient, Forward+ to the emission too, render/shaders/emission_light.gdshaderinc), so a night keeps the darkness the eye
## sees in it and the picture's half floats keep its precision. Ward's contrast-based scale factor (Ward 1994, Graphics Gems IV, 'A
## contrast-based scalefactor for luminance display': the display luminance a world luminance takes so a just-noticeable difference in
## the world is one on the display) at the luminance the eye is adapted to, over the same at the day hour: the day keeps the exposure
## its grey card set (daylight.json exposure), and every other light is that far from it. render/daylight.gd meters the luminance.
class_name Eye
extends RefCounted

## Ward's fit of the threshold-versus-intensity curve (Blackwell's data, CIE 19/2.1, 1981): ((A + Lda^P) / (A + Lwa^P))^K.
const WARD_A := 1.219
const WARD_P := 0.4
const WARD_K := 2.5
const DISPLAY_ADAPTED := 0.5 # the display's adaptation, half its top (Ward)

var _display := 0.0 # cd/m^2
var _day := 1.0 # the scale factor at the day hour


func _init(eye: Dictionary) -> void:
	_display = float(eye.display_cd_m2) * DISPLAY_ADAPTED


## Ward's scale factor for a world adapted to `adapted` cd/m^2.
func scale(adapted: float) -> float:
	return pow((WARD_A + pow(_display, WARD_P)) / (WARD_A + pow(maxf(adapted, 0.0), WARD_P)), WARD_K)


## The day hour's adaptation, against which every other is taken.
func set_day(adapted: float) -> void:
	_day = scale(adapted)


## The pre-exposure for a world adapted to `adapted` cd/m^2.
func pre_exposure(adapted: float) -> float:
	return scale(adapted) / _day
