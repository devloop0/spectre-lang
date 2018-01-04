import <"std/math">
import <"std/cast">
import <"std/machine">

import "../test.sp"

func byte b(byte c) { return c; }

func int main() {
	using std::cast::b;
	using std::machine::big_endian;
	using std::machine::little_endian;

	byte* nan, inf;
	if(big_endian()) {
		nan = [ b(0x7f), b(0xf8), b(0x00), b(0x00),
			b(0x00), b(0x00), b(0x00), b(0x00) ];
	 	inf = [ b(0x7f), b(0xf0), b(0x00), b(0x00),
			b(0x00), b(0x00), b(0x00), b(0x00) ];
	}
	else if(little_endian()) {
		nan = [ b(0x00), b(0x00), b(0x00), b(0x00),
			b(0x00), b(0x00), b(0x7f), b(0xf8) ];
	 	inf = [ b(0x00), b(0x00), b(0x00), b(0x00),
			b(0x00), b(0x00), b(0xf0), b(0x7f) ];
	}
	else success(); # skip

	double i = std::math::infinity(), n = std::math::nan();
	for(int j = 0; j < 8; j++) {
		if((i$ as byte*)[j] != inf[j] || (n$ as byte*)[j] != nan[j])
			fail();
	}
	success();
}
