## The units the scripts convert between, written once (the core's are native/src/core/units.h): a second in milliseconds, a share
## as a percent, a mebibyte in bytes, a kibibyte in bytes, a minute in seconds, a kilometre in metres, a centimetre in metres, a metre
## a second in kilometres an hour.
class_name Units
extends RefCounted

const MS_PER_S := 1000.0
const PERCENT := 100.0
const BYTES_PER_MIB := 1048576.0
const BYTES_PER_KIB := 1024
const S_PER_MIN := 60.0
const METRES_PER_KM := 1000.0
const M_PER_CM := 0.01
const KMH_PER_M_S := 3.6
