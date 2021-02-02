import <"std/lib">

// Implements the xorshift32 algorithm
namespace std {
	namespace lib {

		struct xorshift32_state {
			unsigned int a;
			bool initialized;
		}

		static type xorshift32_state x_state;

		func int rand() {
			if (!x_state.initialized) {
				x_state.initialized = true;
				x_state.a = 0xDEADBEEF_ui;
			}

			unsigned int x = x_state.a;
			x ^= x << 13;
			x ^= x >> 17;
			x ^= x << 5;
			return (x_state.a = x) >> 1; # Need the result to be between [0, RAND_MAX]
		}

		func void srand(unsigned int seed) {
			x_state.initialized = true;
			x_state.a = seed;
		}
	}
}
