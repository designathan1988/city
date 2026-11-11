// Calibration case (LAB_COPY): with no reference tree the gate cannot judge, so it refuses.
double minutes_for(double fixed, double per_gram, double grams) {
    return fixed + per_gram * grams;
}
