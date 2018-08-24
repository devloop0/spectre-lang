import "../test.sp"

func int main() {
	true ? 1 : 1 / 0;
	false ? 1 / 0 : 1;
	success();
}
