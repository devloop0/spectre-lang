func fn int(int) choose(bool cond, fn int(int) f, fn int(int) g) {
	return cond ? f : g;
}

func int my1(int i) { return i * i; }
func int my2(int i) { return i + i; }

func int main() {
	return choose(true, my1, my2)(3);
}
