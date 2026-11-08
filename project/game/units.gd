## The units the scripts convert between, written once (the core's are native/src/core/units.h): a second in milliseconds, a share
## as a percent, a mebibyte in bytes, a kibibyte in bytes, a minute in seconds, a kilometre in metres, a centimetre in metres, a metre
## a second in kilometres an hour.
class_name Units
extends RefCounted

const MS_PER_S := 1000.0 # the millisecond
const PERCENT := 100.0 # a share in hundredths
const BYTES_PER_MIB := 1048576.0 # the mebibyte: 2^20 bytes
const BYTES_PER_KIB := 1024 # the kibibyte: 2^10 bytes
const S_PER_MIN := 60.0 # the minute
const METRES_PER_KM := 1000.0 # the kilometre
const M_PER_CM := 0.01 # the centimetre
const KMH_PER_M_S := 3.6 # 3600 s an hour over 1000 m a kilometre
