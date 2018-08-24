import <"std/lib">

func char* ltoa(unsigned long l) {
	using std::lib::calloc;

	char* tmp = stk char(20), tmp_iter = tmp;
	char* ret = calloc(sizeof{char}, 20) as char*;
	unsigned int end = 0;
	if(l == 0) {
		ret@ = '0';
		return ret;
	}
	while(l as bool) {
		tmp_iter@ = (l % 10) + '0';
		tmp_iter = tmp_iter[1]$;
		end++, l /= 10;
	}
	for(int i = end - 1; i >= 0; i--, tmp = tmp[1]$)
		ret[i] = tmp@;
	return ret;
}
