#ifndef SPECTRE_ERROR_HPP
#define SPECTRE_ERROR_HPP

#include <string>
#include <vector>
#include "lexer/token.hpp"

using std::string;
using std::vector;
using spectre::lexer::token;

namespace spectre {
	namespace lexer {
		class error {
		public:
			enum class kind {
				KIND_ERROR, KIND_WARNING, KIND_NOTE
			};
		private:
			string _message;
			kind _error_kind;
			vector<token> _token_stream;
			int _line_number;
			int _start_column_number, _end_column_number;
			int _caret_index;
		public:
			error(kind ek, string m, vector<token> ts, int ci);
			~error();
			string message() const;
			kind error_kind() const;
			vector<token> token_stream() const;
			int line_number() const;
			int start_column_number() const;
			int end_column_number() const;
			int caret_index() const;
		};
	}
}

#endif
