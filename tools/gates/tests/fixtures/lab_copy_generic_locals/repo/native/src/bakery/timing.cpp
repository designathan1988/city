// Calibration case (LAB_COPY): locals named as the reference's locals are no copy; what a program defines for others is.
struct Clock {
    double fixed;
    double per_gram;
    double factor;
};

double minutes_for(const Clock& clock, double grams, double cooled) {
    const double base_minutes = clock.fixed + clock.per_gram * grams;
    const double freed_slots = cooled;
    return base_minutes * clock.factor - freed_slots;
}
