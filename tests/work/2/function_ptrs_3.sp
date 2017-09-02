fn int() ** GLOBAL = resv fn int()*(4);

func int f1() { return 5; }
func int f2() { return 6; }
func int f3() { return 3; }
func int f4() { return 4; }

func int main() {
	fn int() h1 = f1, h2 = f2, h3 = f3, h4 = f4;
	fn int() * p1 = h1$, p2 = h2$, p3 = h3$, p4 = h4$;
        GLOBAL[0] = p1;
        GLOBAL[1] = p2;
        GLOBAL[2] = p3;
        GLOBAL[3] = p4;

        int total = 0;
        for(int i = 0; i < 4; i++) total += GLOBAL[i]@();
        return total;
}
