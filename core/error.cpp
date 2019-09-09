#include "core/error.hpp"

namespace spectre {
	namespace lexer {
		error::error(error::kind ek, string m, vector<token> ts, int ci) :
			_error_kind(ek), _message(m), _token_stream(ts), _caret_index(ci) {
			if (_token_stream.size() == 0)
				_line_number = 0, _start_column_number = 0, _end_column_number = 0;
			else {
				token first = _token_stream[0], last = _token_stream[_token_stream.size() - 1];
				_line_number = first.line_number();
				_start_column_number = first.start_column_number();
				_end_column_number = last.end_column_number();
			}
		}

		error::~error() {

		}

		string error::message() const {
			return _message;
		}

		error::kind error::error_kind() const {
			return _error_kind;
		}

		vector<token> error::token_stream() const {
			return _token_stream;
		}

		int error::line_number() const {
			return _line_number;
		}

		int error::start_column_number() const {
			return _start_column_number;
		}

		int error::end_column_number() const {
			return _end_column_number;
		}

		int error::caret_index() const {
			return _caret_index;
		}
	}
}
