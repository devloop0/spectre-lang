#include "diagnostics.hpp"
#include <iostream>

using std::cerr;
using std::exit;

namespace spectre {
	namespace lexer {
		diagnostics::diagnostics(int l) : _limit(l) {
			_error_count = 0, _warning_count = 0, _note_count = 0;
			_error_list = vector<error>();
		}

		diagnostics::~diagnostics() {

		}

		vector<error> diagnostics::error_list() const {
			return _error_list;
		}

		int diagnostics::error_count() const {
			return _error_count;
		}

		int diagnostics::warning_count() const {
			return _warning_count;
		}

		int diagnostics::note_count() const {
			return _note_count;
		}

		int diagnostics::limit() const {
			return _limit;
		}

		void diagnostics::report(error err) {
			if (err.error_kind() == error::kind::KIND_ERROR) {
				cerr << "Error: ";
				_error_count++;
			}
			else if (err.error_kind() == error::kind::KIND_NOTE) {
				cerr << "Note: ";
				_note_count++;
			}
			else {
				cerr << "Warning: ";
				_warning_count++;
			}
			cerr << "[" << (err.token_stream().size() == 0 ? "" : err.token_stream()[0].file_name()) << ":" << err.line_number() << ":" << err.start_column_number() << "-" <<
				err.end_column_number() << "] ";
			cerr << err.message() << "\n\n\t";
			int offset = 0;
			for (int i = 0; i < err.token_stream().size(); i++) {
				token tok = err.token_stream()[i];
				cerr << tok.raw_text();
				if (i < err.caret_index())
					offset += tok.raw_text().length() + 1;
				if (i != err.token_stream().size() - 1)
					cerr << " ";
			}
			cerr << "\n\t";
			for (int i = 0; i < offset; i++)
				cerr << " ";
			cerr << "^";
			cerr << "\n\n";
			if (_error_count > _limit) {
				cerr << "Error limit of " << _limit << " exceeded. Terminating...";
				cerr << "\n\n";
				exit(1);
			}
		}

		void diagnostics::report_internal(string msg, string fn, int ln, string fl) {
			cerr << "Internal Compiler Error: [" << fl << ":" << fn << ":" << ln << "] " << msg << "\n";
			exit(1);
		}
	}
}