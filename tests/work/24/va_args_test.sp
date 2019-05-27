import <"std/arg">
import <"std/aliases">

import "../test.sp"

using namespace std::aliases;

func int sum(int count, ...) {
	type arg::va_list* args = arg::va_start(arg::align(sizeof(count)), count$ as byte*);

	int s = 0;
	for (int i = 0; i < count; i++) {
		int j;
		arg::va_arg(args, j$ as byte*, sizeof(j));
		s += j;
	}
	
	arg::va_end(args);
	return s;
}

func double favg(int count, ...) {
	type arg::va_list* args = arg::va_start(arg::align(sizeof(count)), count$ as byte*);

	double a = 0;
	for (int i = 0; i < count; i++) {
		double d;
		arg::va_arg(args, d$ as byte*, sizeof(d));
		a += d;
	}
	
	arg::va_end(args);
	return a / count;
}

func double fstdev(bool pop, int count, double mu, ...) {
	type arg::va_list* args = arg::va_start(arg::align(sizeof(pop)) + arg::align(sizeof(count)) + arg::align(sizeof(mu)), mu$ as byte*);

	double num = 0;
	for (int i = 0; i < count; i++) {
		double d;
		arg::va_arg(args, d$ as byte*, sizeof(d));
		num += (d - mu) * (d - mu);
	}

	arg::va_end(args);
	return pop ? num / count : num / (count - 1);
}

func int main() {
	if (sum(4, 1, 2, sum(2, 3, 4), sum(3, 5, sum(2, 6, 7), sum(1, 8))) != 36)
		fail();
	
	if (fstdev(true, 5, favg(5, 1., 2., 3., 4., 5.), 1., 2., 3., 4., 5.) != 2)
		fail();

	if (fstdev(false, 5, favg(5, 1., 2., 3., 4., 5.), 1., 2., 3., 4., 5.) != 2.5)
		fail();

	success();
}
