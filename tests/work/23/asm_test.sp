import "../test.sp"

func int main() {
	auto i = 2, j = 3, k = 0;
	__asm__ (
		i "$8" :
		j "$9" :
		"lw $8, 0($8)" :
		"lw $9, 0($9)" :
		"addu $8, $8, $9" :
		k "$10" :
		"sw $8, 0($10)"
	);

	if(k != 5) fail();
	else success();
}
