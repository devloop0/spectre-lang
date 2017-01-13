#include "token.hpp"

namespace spectre {
	namespace lexer {
		token::token(string pd, string fn, int l, int scn, int ecn, token::kind tk, string rt, token::prefix pk, token::suffix sk) :
			_line_number(l), _start_column_number(scn), _end_column_number(ecn), _token_kind(tk), _parent_directory(pd), _file_name(fn),
			_raw_text(rt), _prefix_kind(pk), _suffix_kind(sk) {
			_valid = tk == token::kind::TOKEN_ERROR ? false : true;
		}

		token::~token() {

		}

		int token::line_number() const {
			return _line_number;
		}

		int token::start_column_number() const {
			return _start_column_number;
		}

		int token::end_column_number() const {
			return _end_column_number;
		}

		token::kind token::token_kind() const {
			return _token_kind;
		}

		string token::raw_text() const {
			return _raw_text;
		}

		bool token::valid() const {
			return _valid;
		}

		token::prefix token::prefix_kind() const {
			return _prefix_kind;
		}

		token::suffix token::suffix_kind() const {
			return _suffix_kind;
		}

		string token::file_name() const {
			return _file_name;
		}

		string token::parent_directory() const {
			return _parent_directory;
		}
	}
}