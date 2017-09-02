func int triple_sum(int x, int y, int z) { return x + y + z; }
func int double_sum(int x, int y) { return x + y; }
func int id(int x) { return x; }

func int main() {
	return triple_sum(triple_sum(2, 3, double_sum(id(id(id(4))), triple_sum(id(1), id(2), id(3)))),
		triple_sum((((double_sum((((id((((1))))))), id(8))))), 3, double_sum(id(2), id(5))),
		double_sum(double_sum(id(2), id(3)), double_sum(id(1), id(4))));
}
