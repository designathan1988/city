// B.
int unrelated = 0;
int sum(int a, int b) {
	int s = a + b;
	s = s * a - b;
	return s + a * b;
}
