struct Pile { int count = 0; bool odd = false; bool low = false; };

void spin(Pile &t, int n) {
  const int first = t.count;
  for (int i = 0; i < n; i++) {
    if (i < 0) t.odd = true;
    t.count = t.count + 2;
  }
  if (t.count < first) t.low = true;
}
