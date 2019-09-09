#ifndef SPECTRE_DIAGNOSTICS_HPP
#define SPECTRE_DIAGNOSTICS_HPP

#include <string>
#include <vector>
#include "core/error.hpp"

using std::string;
using std::vector;
using spectre::lexer::error;

namespace spectre {
	namespace lexer {
		class diagnostics {
		private:
			vector<error> _error_list;
			int _error_count, _warning_count, _note_count;
			int _limit;
		public:
			diagnostics(int l = 100);
			~diagnostics();
			vector<error> error_list() const;
			int error_count() const;
			int warning_count() const;
			int note_count() const;
			int limit() const;
			void report(error err);
			static void report_internal(string msg, string fn, int ln, string fl);
		};
	}
}

#endif
