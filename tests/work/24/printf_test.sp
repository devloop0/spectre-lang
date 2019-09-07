import <"std/syscall">
import <"std/string">
import <"std/limits">
import <"std/arg">
import <"std/aliases">
import <"std/io">

func int avg(int n, int weight, ...) {
	using namespace std::aliases;

	type arg::va_list* v = arg::va_start(weight$ as byte*, sizeof{int});
	
	int sum = 0;
	for (int i = 0; i < n; i++) {
		int i;
		arg::va_arg(v, i$ as byte*, sizeof{int}, alignof{int});
		sum += i;
	}
	arg::va_end(v);
	return sum / (n * weight);
}

func float f(float f1, int x2, double d3, float f4, int x5, double d6,
	float f7, int x8, double d9) {
	std::io::printf("%d %d %d %d %d %d %d %d %d\n", f1 as int, x2, d3 as int, f4 as int, x5, d6 as int, f7 as int, x8, d9 as int);
	return f1 * x2 * d3 + f4 * x5 * d6 + f7 * x8 * d9;
}

struct s {
	int i, j, k;
}

func int temp(type s ss) {
	std::io::printf("%d %d %d\n", ss.i, ss.j, ss.k);
}

func int main() {
	std::io::printf("%s from Spectre!\nMy name is %s %s.\n", "Hello World", "Nikhil", "Athreya");
	std::io::printf("Average: %d\n", avg(11, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11));
	float r = f(2, 3, 4, 5, 6, 7, 8, 9, 10);
	std::io::printf("%d\n", 1234567);
	type s ss; ss.i = 123, ss.j = 45, ss.k = 345;
	temp(ss);
}
