import "../test.sp"

func int main() {
	auto i = 2, j = 3, k = 0;
	__asm__ (
		i "r0" :
		j "r1" :
		"ldr r0, [r0]" :
		"ldr r1, [r1]" :
		"add r0, r0, r1" :
		k "r2" :
		"str r0, [r2]"
	);

	if(k != 5) fail();
	else success();
}
