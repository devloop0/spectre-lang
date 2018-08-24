import <"stdx/string">
import <"std/string">

namespace stdx {
	namespace string_impl {

		func bool eq(const type string* lhs, const type string* rhs) {
			return std::string::strcmp(lhs->c_string, rhs->c_string) == 0;
		}

		func bool ceq(const char* lhs, const type string* rhs) {
			return std::string::strcmp(lhs, rhs->c_string) == 0;
		}

		func bool eqc(const type string* lhs, const char* rhs) {
			return std::string::strcmp(lhs->c_string, rhs) == 0;
		}

		func bool neq(const type string* lhs, const type string* rhs) {
			return !eq(lhs, rhs);
		}

		func bool cneq(const char* lhs, const type string* rhs) {
			return !ceq(lhs, rhs);
		}

		func bool neqc(const type string* lhs, const char* rhs) {
			return !eqc(lhs, rhs);
		}

		func bool lt(const type string* lhs, const type string* rhs) {
			return std::string::strcmp(lhs->c_string, rhs->c_string) < 0;
		}

		func bool clt(const char* lhs, const type string* rhs) {
			return std::string::strcmp(lhs, rhs->c_string) < 0;
		}

		func bool ltc(const type string* lhs, const char* rhs) {
			return std::string::strcmp(lhs->c_string, rhs) < 0;
		}

		func bool lte(const type string* lhs, const type string* rhs) {
			return std::string::strcmp(lhs->c_string, rhs->c_string) <= 0;
		}

		func bool clte(const char* lhs, const type string* rhs) {
			return std::string::strcmp(lhs, rhs->c_string) <= 0;
		}

		func bool ltec(const type string* lhs, const char* rhs) {
			return std::string::strcmp(lhs->c_string, rhs) <= 0;
		}

		func bool gt(const type string* lhs, const type string* rhs) {
			return std::string::strcmp(lhs->c_string, rhs->c_string) > 0;
		}

		func bool cgt(const char* lhs, const type string* rhs) {
			return std::string::strcmp(lhs, rhs->c_string) > 0;
		}

		func bool gtc(const type string* lhs, const char* rhs) {
			return std::string::strcmp(lhs->c_string, rhs) > 0;
		}

		func bool gte(const type string* lhs, const type string* rhs) {
			return std::string::strcmp(lhs->c_string, rhs->c_string) >= 0;
		}

		func bool cgte(const char* lhs, const type string* rhs) {
			return std::string::strcmp(lhs, rhs->c_string) >= 0;
		}

		func bool gtec(const type string* lhs, const char* rhs) {
			return std::string::strcmp(lhs->c_string, rhs) >= 0;
		}
	}
}
