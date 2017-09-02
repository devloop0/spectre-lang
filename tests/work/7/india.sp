import <"std/syscall">

func void putchar(const char c) {
	std::syscall::direct_write(1, c$, 1);
}

func int main() {
	int count = 1;
	int b = 10, c = b, a;
	for( ; (a = "- FIGURE?, UMKC,XYZHello Folks,    TFy!QJu ROo TNn(ROo)SLq SLq ULo+    UHs UJq TNn*RPn/QPbEWS_JSWQAIJO^    NBELPeHBFHT}TnALVlBLOFAkHFOuFETp    HCStHAUFAgcEAelclcn^r^r\\tZvYxXy    T|S~Pn SPm SOn TNn ULo0ULo#ULo-W    Hq!WFs XDt!"[b+++21]) as bool; )
		for(; a --> 64;)
			putchar(++c == 'Z' ? c = c/ 9:33^b&1);
	return 0;
}
