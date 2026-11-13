void zorp();
void quab();
void plim(int) { zorp(); }
void plim(double) { quab(); }
struct Vend { void tock() { zorp(); } };
struct Wisk : Vend { void tock() { quab(); } };
Vend *made();
void zorp() {}
void quab() {}
void bramble() { plim(1); }
void frindle() { Wisk w; Vend &v = w; v.tock(); }
void frob() { made()->tock(); }
