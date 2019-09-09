#ifndef SPECTRE_BUFFER_HPP
#define SPECTRE_BUFFER_HPP

#include <vector>
#include <string>
#include <memory>
#include "lexer/token.hpp"
#include "core/diagnostics.hpp"

using std::vector;
using std::string;
using std::shared_ptr;
using spectre::lexer::diagnostics;
using spectre::lexer::token;

namespace spectre {
	namespace lexer {

		const static token bad_token("", "", -1, -1, -1, token::kind::TOKEN_ERROR, "");
		
		class buffer {
			int _index, _current_line_number, _current_column_number;
			string _source_code, _file_name, _parent_directory;
			vector<token> _consumed_token_list;
			shared_ptr<diagnostics> _diagnostics_reporter;

			token update(token tok);
		public:
			buffer(string pd, string fn, shared_ptr<diagnostics> dr, string src);
			~buffer();
			int current_line_number() const;
			int current_column_number() const;
			string source_code() const;
			string file_name() const;
			string parent_directory() const;
			shared_ptr<diagnostics> diagnostics_reporter() const;
			vector<token> consumed_token_list() const;

			token pop();
		};
	}
}

#endif
