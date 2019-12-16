import "../test.sp"
import <"std/limits">

using namespace std::limits;

func void testc(unsigned char c, bool ok) {
	if (c >= 1 && c <= SCHAR_MAX) {
		if (!ok) fail();
	}
	else {
		if (ok) fail();
	}
}

func void tests(unsigned short s, bool ok) {
	if (s >= 1 && s <= SHRT_MAX) {
		if (!ok) fail();
	}
	else {
		if (ok) fail();
	}
}

func void testi(unsigned int i, bool ok) {
	if (i >= 1 && i <= INT_MAX) {
		if (!ok) fail();
	}
	else {
		if (ok) fail();
	}
}

func void testl(unsigned long l, bool ok) {
	if (l >= 1 && l <= LONG_MAX) {
		if (!ok) fail();
	}
	else {
		if (ok) fail();
	}
}

func int main () {
	testc (0, false);
	testc (1, true);
	testc (SCHAR_MAX, true);
	testc (SCHAR_MAX+1, false);
	testc (UCHAR_MAX, false);

	tests (0, false);
	tests (1, true);
	tests (SHRT_MAX, true);
	tests (SHRT_MAX+1, false);
	tests (USHRT_MAX, false);

	testi (0, false);
	testi (1, true);
	testi (INT_MAX, true);
	testi (INT_MAX+1_ui, false);
	testi (UINT_MAX, false);

	testl (0, false);
	testl (1, true);
	testl (LONG_MAX, true);
	testl (LONG_MAX+1_ul, false);
	testl (ULONG_MAX, false);

	unsigned char c = SCHAR_MAX, c2 = -(SCHAR_MIN + 1);
	if (!(c >= 1 && c <= SCHAR_MAX)) fail();
	if (!(c2 >= 1 && c2 <= SCHAR_MAX)) fail();

	success();
}
