#include "buffer.hpp"
#include "error.hpp"
#include "token_keywords.hpp"
#include <locale>
#include <iostream>

using spectre::lexer::error;
using spectre::lexer::bad_token;
using std::make_shared;
using namespace spectre::keywords;
using std::isdigit;
using std::cout;

namespace spectre {
	namespace lexer {

		buffer::buffer(string pd, string fn, shared_ptr<diagnostics> dr, string src) : _file_name(fn), _source_code(src), _diagnostics_reporter(dr), _parent_directory(pd) {
			_index = 0;
			_current_line_number = 1;
			_current_column_number = 1;
		}

		buffer::~buffer() {

		}

		int buffer::current_line_number() const {
			return _current_line_number;
		}

		int buffer::current_column_number() const {
			return _current_column_number;
		}

		string buffer::source_code() const {
			return _source_code;
		}

		string buffer::file_name() const {
			return _file_name;
		}

		shared_ptr<diagnostics> buffer::diagnostics_reporter() const {
			return _diagnostics_reporter;
		}

		vector<token> buffer::consumed_token_list() const {
			return _consumed_token_list;
		}

		string buffer::parent_directory() const {
			return _parent_directory;
		}

		token buffer::update(token tok) {
			_current_column_number += tok.raw_text().length();
			_index += tok.raw_text().length();
			_consumed_token_list.push_back(tok);
			return tok;
		}

		token buffer::pop() {
			while (_index < _source_code.length() && (_source_code[_index] == '\t' || _source_code[_index] == ' ' || _source_code[_index] == '\n' ||
				_source_code[_index] == '#')) {
				if (_source_code[_index] == '\n') {
					_index++;
					_current_column_number = 1;
					_current_line_number++;
				}
				else if (_source_code[_index] == '#') {
					while (_index < _source_code.length() && _source_code[_index] != '\n') {
						_index++;
						_current_column_number++;
					}
					if (_index < _source_code.length()) {
						_current_column_number = 1;
						_current_line_number++;
						_index++;
					}
				}
				else {
					_index++;
					_current_column_number++;
				}
			}
			if (_index >= _source_code.length())
				return token(_parent_directory, _file_name, _current_line_number + 1, 0, 0, token::kind::TOKEN_EOF, "(EOF)");
			switch (_source_code[_index]) {
			case '+': {
				if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_PLUS_EQUALS, "+="));
				else if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '+')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_INCREMENT, "++"));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_PLUS, "+"));
				return bad_token;
			}
					  break;
			case '-': {
				if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_MINUS_EQUALS, "-="));
				else if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '-')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_DECREMENT, "--"));
				else if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '>')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_ARROW, "->"));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_MINUS, "-"));
				return bad_token;
			}
					  break;
			case '*': {
				if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_MULTIPLY_EQUALS, "*="));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_STAR, "*"));
				return bad_token;
			}
					  break;
			case '/': {
				if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_DIVIDE_EQUALS, "/="));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_SLASH, "/"));
				return bad_token;
			}
					  break;
			case '%': {
				if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_MODULO_EQUALS, "%="));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_PERCENT, "%"));
				return bad_token;
			}
					  break;
			case '=': {
				if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_EQUALS_EQUALS, "=="));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_EQUALS, "="));
				return bad_token;
			}
					  break;
			case '!': {
				if (_index < _source_code.length() && _source_code[_index + 1] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_NOT_EQUALS, "!="));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_EXCALAMATION, "!"));
				return bad_token;
			}
					  break;
			case '<': {
				if (_index + 2 < _source_code.length() && _source_code[_index + 1] == '<' && _source_code[_index + 2] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 3, token::kind::TOKEN_SHIFT_LEFT_EQUALS, "<<="));
				else if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '<')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_SHIFT_LEFT, "<<"));
				else if (_index + 1, _source_code.length() && _source_code[_index + 1] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_LESS_THAN_OR_EQUAL_TO, "<="));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_LEFT_ANGLE_BRACKET, "<"));
				return bad_token;
			}
					  break;
			case '>': {
				if (_index + 2 < _source_code.length() && _source_code[_index + 1] == '>' && _source_code[_index + 2] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 3, token::kind::TOKEN_SHIFT_RIGHT_EQUALS, ">>="));
				else if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '>')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_SHIFT_RIGHT, ">>"));
				else if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_GREATER_THAN_OR_EQUAL_TO, ">="));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_RIGHT_ANGLE_BRACKET, ">"));
				return bad_token;
			}
					  break;
			case '&': {
				if (_index + 2 < _source_code.length() && _source_code[_index + 1] == '&' && _source_code[_index + 2] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 3, token::kind::TOKEN_LOGICAL_AND_EQUALS, "&&="));
				if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '&')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_LOGICAL_AND, "&&"));
				else if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_BITWISE_AND_EQUALS, "&="));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_AMPERSAND, "&"));
				return bad_token;
			}
					  break;
			case '|': {
				if (_index + 2 < _source_code.length() && _source_code[_index + 1] == '|' && _source_code[_index + 2] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 3, token::kind::TOKEN_LOGICAL_OR_EQUALS, "||="));
				else if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '|')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_LOGICAL_OR, "||"));
				else if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_BITWISE_OR_EQUALS, "|="));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_BAR, "|"));
				return bad_token;
			}
					  break;
			case '^': {
				if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '=')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_XOR_EQUALS, "^="));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_CARET, "^"));
				return bad_token;
			}
					  break;
			case '@': {
				return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_AT, "@"));
			}
					  break;
			case ':': {
				if (_index + 1 < _source_code.length() && _source_code[_index + 1] == ':')
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 2, token::kind::TOKEN_COLON_COLON, "::"));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_COLON, ":"));
			}
					  break;
			case '?': {
				return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_QUESTION_MARK, "?"));
			}
					  break;
			case '(': {
				return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_OPEN_PARENTHESIS, "("));
			}
					  break;
			case ')': {
				return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_CLOSE_PARENTHESIS, ")"));
			}
					  break;
			case '[': {
				return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_OPEN_BRACKET, "["));
			}
					  break;
			case ']': {
				return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_CLOSE_BRACKET, "]"));
			}
					  break;
			case '{': {
				return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_OPEN_BRACE, "{"));
			}
					  break;
			case '}': {
				return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_CLOSE_BRACE, "}"));
			}
					  break;
			case ';': {
				return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_SEMICOLON, ";"));
			}
					  break;
			case ',': {
				return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_COMMA, ","));
			}
					  break;
			case '~': {
				return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_TILDE, "~"));
			}
					  break;
			case '\'': {
				int start = _current_column_number;
				string character = string(1, _source_code[_index]);
				_index++;
				_current_column_number++;
				while (_index < _source_code.length() && _source_code[_index] != '\'' && _source_code[_index] != '\n') {
					if (_source_code[_index] == '\\') {
						if (_index + 1 < _source_code.length()) {
							character += _source_code[_index];
							_index++;
							_current_column_number++;
						}
					}
					character += _source_code[_index];
					_index++;
					_current_column_number++;
				}
				if (_index >= _source_code.length() || _source_code[_index] != '\'') {
					token temp(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_ERROR, character);
					_diagnostics_reporter->report(error(error::kind::KIND_ERROR, "Malformed character token.", vector<token> { temp }, 0));
					_consumed_token_list.push_back(temp);
					return temp;
				}
				character += _source_code[_index];
				_index++;
				_current_column_number++;
				token ret(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_CHARACTER, character);
				string trimmed_character = character.substr(1, character.length() - 2);
				bool good = true;
				if (trimmed_character.length() == 1);
				else if (trimmed_character.length() == 2) good = trimmed_character[0] == '\\';
				else good = false;
				if (!good) {
					token temp(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_ERROR, character);
					_diagnostics_reporter->report(error(error::kind::KIND_ERROR, "Malformed character token.", vector<token> { temp }, 0));
					_consumed_token_list.push_back(temp);
					return temp;
				}
				_consumed_token_list.push_back(ret);
				return ret;
			}
					   break;
			case '\"': {
				int start = _current_column_number;
				string quote = string(1, _source_code[_index]);
				_index++;
				_current_column_number++;
				while (_index < _source_code.length() && _source_code[_index] != '\"' && _source_code[_index] != '\n') {
					if (_source_code[_index] == '\\') {
						if (_index + 1 < _source_code.length() && _source_code[_index + 1] == '\"') {
							quote += _source_code[_index];
							_index++;
							_current_column_number++;
						}
					}
					quote += _source_code[_index];
					_index++;
					_current_column_number++;
				}
				if (_index >= _source_code.length() || _source_code[_index] != '\"') {
					token temp(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_ERROR, quote);
					_diagnostics_reporter->report(error(error::kind::KIND_ERROR, "Malformed string token.", vector<token> { temp }, 0));
					_consumed_token_list.push_back(temp);
					return temp;
				}
				quote += _source_code[_index];
				_index++;
				_current_column_number++;
				token ret(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_STRING, quote);
				_consumed_token_list.push_back(ret);
				return ret;
			}
					   break;
			case '.': {
				if (_index + 1 < _source_code.length() && isdigit(_source_code[_index + 1]));
				else
					return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_DOT, "."));
			}
			case '0': {
				if (_index + 1 < _source_code.length() && (_source_code[_index + 1] == 'x' || _source_code[_index + 1] == 'X')) {
					string num = "0" + string(1, _source_code[_index + 1]);
					token::prefix p = token::prefix::PREFIX_HEXADECIMAL;
					int start = _current_column_number;
					_index += 2;
					_current_column_number += 2;
					while (_index < _source_code.length() && (isdigit(_source_code[_index]) ||
						_source_code[_index] == 'a' || _source_code[_index] == 'A' || _source_code[_index] == 'B' || _source_code[_index] == 'b' ||
						_source_code[_index] == 'C' || _source_code[_index] == 'c' || _source_code[_index] == 'D' || _source_code[_index] == 'd' ||
						_source_code[_index] == 'E' || _source_code[_index] == 'e' || _source_code[_index] == 'F' || _source_code[_index] == 'f' ||
						_source_code[_index] == '\'')) {
						num += _source_code[_index];
						_index++;
						_current_column_number++;
					}
					token::suffix suff = token::suffix::SUFFIX_NONE;
					if (_source_code[_index] == '_') {
						num += _source_code[_index];
						_index++;
						_current_column_number++;
						if (_index >= _source_code.length()) {
							token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_INTEGER, num);
							error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
							_diagnostics_reporter->report(err);
							_consumed_token_list.push_back(b_token);
							return b_token;
						}
						num += _source_code[_index];
						_index++;
						_current_column_number++;
						if (_source_code[_index - 1] == 'f' || _source_code[_index - 1] == 'F')
							suff = token::suffix::SUFFIX_FLOAT;
						else if (_source_code[_index - 1] == 'l' || _source_code[_index - 1] == 'L')
							suff = token::suffix::SUFFIX_SIGNED_LONG;
						else if (_source_code[_index - 1] == 'd' || _source_code[_index - 1] == 'D')
							suff = token::suffix::SUFFIX_DOUBLE;
						else if (_source_code[_index - 1] == 's' || _source_code[_index - 1] == 'S')
							suff = token::suffix::SUFFIX_SIGNED_SHORT;
						else if (_source_code[_index - 1] == 'u' || _source_code[_index - 1] == 'U') {
							num += _source_code[_index];
							_index++;
							_current_column_number++;
							if (_source_code[_index - 1] == 'L' || _source_code[_index - 1] == 'l')
								suff = token::suffix::SUFFIX_UNSIGNED_LONG;
							else if (_source_code[_index - 1] == 's' || _source_code[_index - 1] == 'S')
								suff = token::suffix::SUFFIX_UNSIGNED_SHORT;
							else if (_source_code[_index - 1] == 'i' || _source_code[_index - 1] == 'I')
								suff = token::suffix::SUFFIX_UNSIGNED_INT;
						}
						else {
							token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_INTEGER, num);
							error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
							_diagnostics_reporter->report(err);
							_consumed_token_list.push_back(b_token);
							return b_token;
						}
					}
					if (_index < _source_code.length() && (isalnum(_source_code[_index]) || _source_code[_index] == '_')) {
						num += _source_code[_index];
						_index++;
						_current_column_number++;
						token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_INTEGER, num);
						error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
						_diagnostics_reporter->report(err);
						_consumed_token_list.push_back(b_token);
						return b_token;
					}
					token tok(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_INTEGER, num, p, suff);
					_consumed_token_list.push_back(tok);
					return tok;
				}
				else if (_index + 1 < _source_code.length() && (_source_code[_index + 1] == 'b' || _source_code[_index + 1] == 'B')) {
					string num = "0" + string(1, _source_code[_index + 1]);
					token::prefix p = token::prefix::PREFIX_BINARY;
					int start = _current_column_number;
					_index += 2;
					_current_column_number += 2;
					while (_index < _source_code.length() && (_source_code[_index] == '0' || _source_code[_index] == '1' || _source_code[_index] == '\'')) {
						num += _source_code[_index];
						_index++;
						_current_column_number++;
					}
					token::suffix suff = token::suffix::SUFFIX_NONE;
					if (_source_code[_index] == '_') {
						num += _source_code[_index];
						_index++;
						_current_column_number++;
						if (_index >= _source_code.length()) {
							token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_INTEGER, num);
							error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
							_diagnostics_reporter->report(err);
							_consumed_token_list.push_back(b_token);
							return b_token;
						}
						num += _source_code[_index];
						_index++;
						_current_column_number++;
						if (_source_code[_index - 1] == 'f' || _source_code[_index - 1] == 'F')
							suff = token::suffix::SUFFIX_FLOAT;
						else if (_source_code[_index - 1] == 'l' || _source_code[_index - 1] == 'L')
							suff = token::suffix::SUFFIX_SIGNED_LONG;
						else if (_source_code[_index - 1] == 'd' || _source_code[_index - 1] == 'D')
							suff = token::suffix::SUFFIX_DOUBLE;
						else if (_source_code[_index - 1] == 's' || _source_code[_index - 1] == 'S')
							suff = token::suffix::SUFFIX_SIGNED_SHORT;
						else if (_source_code[_index - 1] == 'u' || _source_code[_index - 1] == 'U') {
							num += _source_code[_index];
							_index++;
							_current_column_number++;
							if (_source_code[_index - 1] == 'L' || _source_code[_index - 1] == 'l')
								suff = token::suffix::SUFFIX_UNSIGNED_LONG;
							else if (_source_code[_index - 1] == 's' || _source_code[_index - 1] == 'S')
								suff = token::suffix::SUFFIX_UNSIGNED_SHORT;
							else if (_source_code[_index - 1] == 'i' || _source_code[_index - 1] == 'I')
								suff = token::suffix::SUFFIX_UNSIGNED_INT;
						}
						else {
							token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_INTEGER, num);
							error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
							_diagnostics_reporter->report(err);
							_consumed_token_list.push_back(b_token);
							return b_token;
						}
					}
					if (_index < _source_code.length() && (isalnum(_source_code[_index]) || _source_code[_index] == '_')) {
						num += _source_code[_index];
						_index++;
						_current_column_number++;
						token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_INTEGER, num);
						error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
						_diagnostics_reporter->report(err);
						_consumed_token_list.push_back(b_token);
						return b_token;
					}
					token tok(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_INTEGER, num, p, suff);
					_consumed_token_list.push_back(tok);
					return tok;
				}
				else if (_index + 1 < _source_code.length() && (_source_code[_index + 1] == 'o' || _source_code[_index + 1] == 'O')) {
					string num = "0" + string(1, _source_code[_index + 1]);
					token::prefix p = token::prefix::PREFIX_OCTAL;
					int start = _current_column_number;
					_index += 2;
					_current_column_number += 2;
					while (_index < _source_code.length() && (_source_code[_index] == '0' || _source_code[_index] == '1' ||
						_source_code[_index] == '2' || _source_code[_index] == '3' || _source_code[_index] == '4' ||
						_source_code[_index] == '5' || _source_code[_index] == '6' || _source_code[_index] == '7' || _source_code[_index] == '\'')) {
						num += _source_code[_index];
						_index++;
						_current_column_number++;
					}
					token::suffix suff = token::suffix::SUFFIX_NONE;
					if (_source_code[_index] == '_') {
						num += _source_code[_index];
						_index++;
						_current_column_number++;
						if (_index >= _source_code.length()) {
							token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_INTEGER, num);
							error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
							_diagnostics_reporter->report(err);
							_consumed_token_list.push_back(b_token);
							return b_token;
						}
						num += _source_code[_index];
						_index++;
						_current_column_number++;
						if (_source_code[_index - 1] == 'f' || _source_code[_index - 1] == 'F')
							suff = token::suffix::SUFFIX_FLOAT;
						else if (_source_code[_index - 1] == 'l' || _source_code[_index - 1] == 'L')
							suff = token::suffix::SUFFIX_SIGNED_LONG;
						else if (_source_code[_index - 1] == 'd' || _source_code[_index - 1] == 'D')
							suff = token::suffix::SUFFIX_DOUBLE;
						else if (_source_code[_index - 1] == 's' || _source_code[_index - 1] == 'S')
							suff = token::suffix::SUFFIX_SIGNED_SHORT;
						else if (_source_code[_index - 1] == 'u' || _source_code[_index - 1] == 'U') {
							num += _source_code[_index];
							_index++;
							_current_column_number++;
							if (_source_code[_index - 1] == 'L' || _source_code[_index - 1] == 'l')
								suff = token::suffix::SUFFIX_UNSIGNED_LONG;
							else if (_source_code[_index - 1] == 's' || _source_code[_index - 1] == 'S')
								suff = token::suffix::SUFFIX_UNSIGNED_SHORT;
							else if (_source_code[_index - 1] == 'i' || _source_code[_index - 1] == 'I')
								suff = token::suffix::SUFFIX_UNSIGNED_INT;
						}
						else {
							token b_token(_parent_directory, _file_name,_current_line_number, start, _current_column_number, token::kind::TOKEN_INTEGER, num);
							error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
							_diagnostics_reporter->report(err);
							_consumed_token_list.push_back(b_token);
							return b_token;
						}
					}
					if (_index < _source_code.length() && (isalnum(_source_code[_index]) || _source_code[_index] == '_')) {
						num += _source_code[_index];
						_index++;
						_current_column_number++;
						token b_token(_parent_directory, _file_name,_current_line_number, start, _current_column_number, token::kind::TOKEN_INTEGER, num);
						error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
						_diagnostics_reporter->report(err);
						_consumed_token_list.push_back(b_token);
						return b_token;
					}
					token tok(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_INTEGER, num, p, suff);
					_consumed_token_list.push_back(tok);
					return tok;
				}
			}
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				int start = _current_column_number;
				string num(1, _source_code[_index]);
				bool decimal_hit = _source_code[_index] == '.', e_hit = false;
				_index++;
				_current_column_number++;
				while (_index < _source_code.length() && (isdigit(_source_code[_index]) ||
					(!decimal_hit && _source_code[_index] == '.') || (!e_hit && (_source_code[_index] == 'e' || _source_code[_index] == 'E')) || _source_code[_index] == '\'')) {
					if (_source_code[_index] == '.') {
						if (e_hit) {
							num += _source_code[_index];
							_index++;
							_current_column_number++;
							token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number,
								decimal_hit || e_hit ? token::kind::TOKEN_DECIMAL : token::kind::TOKEN_INTEGER, num);
							error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
							_diagnostics_reporter->report(err);
							_consumed_token_list.push_back(b_token);
							return b_token;
						}
						decimal_hit = true;
					}
					else if (_source_code[_index] == 'e' || _source_code[_index] == 'E') {
						e_hit = true;
						if (_index < _source_code.length() && (_source_code[_index + 1] == '+' || _source_code[_index + 1] == '-')) {
							num += _source_code[_index];
							_index++;
							_current_column_number++;
						}
					}
					num += _source_code[_index];
					_index++;
					_current_column_number++;
				}
				if (_index < _source_code.length() && (_source_code[_index] == '.' || _source_code[_index] == 'e' || _source_code[_index] == 'E')) {
					num += _source_code[_index];
					_index++;
					_current_column_number++;
					token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number,
						decimal_hit || e_hit ? token::kind::TOKEN_DECIMAL : token::kind::TOKEN_INTEGER, num);
					error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
					_diagnostics_reporter->report(err);
					_consumed_token_list.push_back(b_token);
					return b_token;
				}
				if (_index < _source_code.length() && _source_code[_index] == '_') {
					num += _source_code[_index];
					_index++;
					_current_column_number++;
					if (_index >= _source_code.length()) {
						token b_token(_parent_directory, _file_name, _current_line_number, start,
							_current_column_number, decimal_hit || e_hit ? token::kind::TOKEN_DECIMAL : token::kind::TOKEN_INTEGER, num);
						error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
						_diagnostics_reporter->report(err);
						_consumed_token_list.push_back(b_token);
						return b_token;
					}
					token::suffix suff = token::suffix::SUFFIX_NONE;
					num += _source_code[_index];
					_index++;
					_current_column_number++;
					if (_source_code[_index - 1] == 'f' || _source_code[_index - 1] == 'F')
						suff = token::suffix::SUFFIX_FLOAT;
					else if (_source_code[_index - 1] == 'd' || _source_code[_index - 1] == 'D')
						suff = token::suffix::SUFFIX_DOUBLE;
					else if ((_source_code[_index - 1] == 'l' || _source_code[_index - 1] == 'L') && !(decimal_hit || e_hit))
						suff = token::suffix::SUFFIX_SIGNED_LONG;
					else if ((_source_code[_index - 1] == 's' || _source_code[_index - 1] == 'S') && !(decimal_hit || e_hit))
						suff = token::suffix::SUFFIX_SIGNED_SHORT;
					else if ((_source_code[_index - 1] == 'u' || _source_code[_index - 1] == 'U') && !(decimal_hit || e_hit)) {
						num += _source_code[_index];
						_index++;
						_current_column_number++;
						if (_source_code[_index - 1] == 'L' || _source_code[_index - 1] == 'l')
							suff = token::suffix::SUFFIX_UNSIGNED_LONG;
						else if (_source_code[_index - 1] == 's' || _source_code[_index - 1] == 'S')
							suff = token::suffix::SUFFIX_UNSIGNED_SHORT;
						else if (_source_code[_index - 1] == 'i' || _source_code[_index - 1] == 'I')
							suff = token::suffix::SUFFIX_UNSIGNED_INT;
						else {
							token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number,
								decimal_hit || e_hit ? token::kind::TOKEN_DECIMAL : token::kind::TOKEN_INTEGER, num);
							error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
							_diagnostics_reporter->report(err);
							_consumed_token_list.push_back(b_token);
							return b_token;
						}
					}
					else {
						token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number,
							decimal_hit || e_hit ? token::kind::TOKEN_DECIMAL : token::kind::TOKEN_INTEGER, num);
						error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
						_diagnostics_reporter->report(err);
						_consumed_token_list.push_back(b_token);
						return b_token;
					}
					if (_index < _source_code.length() && (isalnum(_source_code[_index]) || _source_code[_index] == '_')) {
						num += _source_code[_index];
						_index++;
						_current_column_number++;
						token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number,
							decimal_hit || e_hit ? token::kind::TOKEN_DECIMAL : token::kind::TOKEN_INTEGER, num);
						error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
						_diagnostics_reporter->report(err);
						_consumed_token_list.push_back(b_token);
						return b_token;
					}
					else {
						token tok(_parent_directory, _file_name, _current_line_number, start, _current_column_number,
							decimal_hit || e_hit ? token::kind::TOKEN_DECIMAL : token::kind::TOKEN_INTEGER,
							num, token::prefix::PREFIX_NONE, suff);
						_consumed_token_list.push_back(tok);
						return tok;
					}
				}
				else {
					if (num.length() != 0 && (num[num.length() - 1] == 'e' || num[num.length() - 1] == 'E' || num[num.length() - 1] == '+' || num[num.length() - 1] == '-')) {
						token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number,
							decimal_hit || e_hit ? token::kind::TOKEN_DECIMAL : token::kind::TOKEN_INTEGER, num);
						error err(error::kind::KIND_ERROR, "Malformed numeric literal token.", vector<token>{ b_token }, 0);
						_diagnostics_reporter->report(err);
						_consumed_token_list.push_back(b_token);
						return b_token;
					}
					token tok(_parent_directory, _file_name, _current_line_number, start, _current_column_number, decimal_hit || e_hit ? token::kind::TOKEN_DECIMAL : token::kind::TOKEN_INTEGER,
						num, token::prefix::PREFIX_NONE, token::suffix::SUFFIX_NONE);
					_consumed_token_list.push_back(tok);
					return tok;
				}
			}
					  break;
			case '$': {
				return update(token(_parent_directory, _file_name, _current_line_number, _current_column_number, _current_column_number + 1, token::kind::TOKEN_DOLLAR_SIGN, "$"));
			}
					  break;
			default: {
				int start = _current_column_number;
				string ident = string(1, _source_code[_index]);
				_index++;
				_current_column_number++;
				if (_source_code[_index - 1] == '`') {
						token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_IDENTIFIER, ident);
						error err(error::kind::KIND_ERROR, "Malformed identifier token.", vector<token>{ b_token }, 0);
						_diagnostics_reporter->report(err);
						_consumed_token_list.push_back(b_token);
						return b_token;
				}
				while (_index < _source_code.length() && (_source_code[_index] == '_' || isalnum(_source_code[_index]))) {
					ident += _source_code[_index];
					_index++;
					_current_column_number++;
				}
				if (_index < _source_code.length() && _source_code[_index] == '`') {
						ident += _source_code[_index];
						_index++;
						_current_column_number++;
						token b_token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_IDENTIFIER, ident);
						error err(error::kind::KIND_ERROR, "Malformed identifier token.", vector<token>{ b_token }, 0);
						_diagnostics_reporter->report(err);
						_consumed_token_list.push_back(b_token);
						return b_token;
				}
				token tok = bad_token;
				if (ident == keywords::_byte)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_BYTE, ident);
				else if (ident == keywords::_char)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_CHAR, ident);
				else if (ident == keywords::_double)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_DOUBLE, ident);
				else if (ident == keywords::_float)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_FLOAT, ident);
				else if (ident == keywords::_int)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_INT, ident);
				else if (ident == keywords::_long)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_LONG, ident);
				else if (ident == keywords::_short)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_SHORT, ident);
				else if (ident == keywords::_struct)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_STRUCT, ident);
				else if (ident == keywords::_bool)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_BOOL, ident);
				else if (ident == keywords::_true)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_TRUE, ident);
				else if (ident == keywords::_false)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_FALSE, ident);
				else if (ident == keywords::_signed)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_SIGNED, ident);
				else if (ident == keywords::_unsigned)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_UNSIGNED, ident);
				else if (ident == keywords::_const)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_CONST, ident);
				else if (ident == keywords::_static)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_STATIC, ident);
				else if (ident == keywords::_void)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_VOID, ident);
				else if (ident == keywords::_if)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_IF, ident);
				else if (ident == keywords::_else)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_ELSE, ident);
				else if (ident == keywords::_func)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_FUNC, ident);
				else if (ident == keywords::_return)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_RETURN, ident);
				else if (ident == keywords::_while)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_WHILE, ident);
				else if (ident == keywords::_continue)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_CONTINUE, ident);
				else if (ident == keywords::_break)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_BREAK, ident);
				else if (ident == keywords::_for)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_FOR, ident);
				else if (ident == keywords::_do)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_DO, ident);
				else if (ident == keywords::_switch)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_SWITCH, ident);
				else if (ident == keywords::_case)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_CASE, ident);
				else if (ident == keywords::_default)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_DEFAULT, ident);
				else if (ident == keywords::_struct)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_STRUCT, ident);
				else if (ident == keywords::_type)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_TYPE, ident);
				else if (ident == keywords::_new)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_NEW, ident);
				else if (ident == keywords::_sizeof)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_SIZEOF, ident);
				else if (ident == keywords::_as)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_AS, ident);
				else if (ident == keywords::_namespace)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_NAMESPACE, ident);
				else if (ident == keywords::_using)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_USING, ident);
				else if (ident == keywords::_asm_)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_ASM, ident);
				else if (ident == keywords::_import)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_IMPORT, ident);
				else if (ident == keywords::_include)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_INCLUDE, ident);
				else if (ident == keywords::_stk)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_STK, ident);
				else if (ident == keywords::_resv)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_RESV, ident);
				else if (ident == keywords::_fn)
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_FN, ident);
				else
					tok = token(_parent_directory, _file_name, _current_line_number, start, _current_column_number, token::kind::TOKEN_IDENTIFIER, ident);
				_consumed_token_list.push_back(tok);
				return tok;
			}
					 break;
			}
			_diagnostics_reporter->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return bad_token;
		}
	}
}