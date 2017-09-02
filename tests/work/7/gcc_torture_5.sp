import <"std/syscall">

func unsigned int bug(unsigned short value, unsigned short* buffer,
	unsigned short* bufend);

unsigned short* buf = [ 1 as unsigned short, 4 as unsigned short, 
			16 as unsigned short, 64 as unsigned short,
			256 as unsigned short];

func int main() {
	if(bug(512, buf, buf[3]$) != 491) {
		std::syscall::direct_write(1, "N\n", 2);	
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}

func unsigned int bug(unsigned short value, unsigned short* buffer,
	unsigned short* bufend) {
	unsigned short* tmp = buffer;
	for(; tmp as unsigned int < bufend as unsigned int;
		tmp = tmp[1]$)
		value -= tmp@;
	return value;
}
