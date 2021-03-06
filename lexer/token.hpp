#ifndef SPECTRE_TOKEN_HPP
#define SPECTRE_TOKEN_HPP

#include <string>

using std::string;

namespace spectre {
	namespace lexer {
		class token {
		public:
			enum class kind {
				TOKEN_IDENTIFIER, TOKEN_BYTE, TOKEN_SHORT, TOKEN_LONG, TOKEN_INT, TOKEN_CHAR, TOKEN_FLOAT, TOKEN_DOUBLE,
				TOKEN_BOOL, TOKEN_TRUE, TOKEN_FALSE, TOKEN_SIGNED, TOKEN_UNSIGNED, TOKEN_CONST, TOKEN_STATIC, TOKEN_NAMESPACE,
				TOKEN_VOID, TOKEN_IF, TOKEN_ELSE, TOKEN_FUNC, TOKEN_RETURN, TOKEN_WHILE, TOKEN_BREAK, TOKEN_CONTINUE, TOKEN_FOR, TOKEN_DO,
				TOKEN_SWITCH, TOKEN_DEFAULT, TOKEN_CASE, TOKEN_STRUCT, TOKEN_TYPE, TOKEN_NEW, TOKEN_SIZEOF, TOKEN_AS,
				TOKEN_USING, TOKEN_ASM, TOKEN_IMPORT, TOKEN_INCLUDE, TOKEN_STK, TOKEN_RESV, TOKEN_FN, TOKEN_ACCESS, TOKEN_CONSTEXPR,
				TOKEN_AUTO, TOKEN_DELETE, TOKEN_UNION, TOKEN_ALIGNOF,

				TOKEN_INTEGER, TOKEN_DECIMAL, TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_PERCENT,
				TOKEN_SHIFT_LEFT, TOKEN_SHIFT_RIGHT, TOKEN_AMPERSAND, TOKEN_BAR, TOKEN_CARET, TOKEN_LEFT_ANGLE_BRACKET,
				TOKEN_RIGHT_ANGLE_BRACKET, TOKEN_GREATER_THAN_OR_EQUAL_TO, TOKEN_LESS_THAN_OR_EQUAL_TO, TOKEN_LOGICAL_AND,
				TOKEN_LOGICAL_OR, TOKEN_EQUALS_EQUALS, TOKEN_NOT_EQUALS, TOKEN_EQUALS, TOKEN_PLUS_EQUALS, TOKEN_MINUS_EQUALS,
				TOKEN_MULTIPLY_EQUALS, TOKEN_DIVIDE_EQUALS, TOKEN_MODULO_EQUALS, TOKEN_SHIFT_LEFT_EQUALS, TOKEN_SHIFT_RIGHT_EQUALS,
				TOKEN_BITWISE_AND_EQUALS, TOKEN_BITWISE_OR_EQUALS, TOKEN_XOR_EQUALS, TOKEN_DOT, TOKEN_COLON, TOKEN_QUESTION_MARK,
				TOKEN_COMMA, TOKEN_OPEN_BRACE, TOKEN_CLOSE_BRACE, TOKEN_STRING, TOKEN_CHARACTER, TOKEN_EXCALAMATION, TOKEN_TILDE,
				TOKEN_SEMICOLON, TOKEN_OPEN_BRACKET, TOKEN_CLOSE_BRACKET, TOKEN_OPEN_PARENTHESIS, TOKEN_CLOSE_PARENTHESIS, TOKEN_INCREMENT,
				TOKEN_DECREMENT, TOKEN_LOGICAL_AND_EQUALS, TOKEN_LOGICAL_OR_EQUALS, TOKEN_AT, TOKEN_ARROW, TOKEN_COLON_COLON, TOKEN_DOLLAR_SIGN,
				TOKEN_ELLIPSIS,

				TOKEN_ERROR, TOKEN_EOF
			};
			enum class prefix {
				PREFIX_BINARY, PREFIX_HEXADECIMAL, PREFIX_OCTAL, PREFIX_NONE
			};
			enum class suffix {
				SUFFIX_FLOAT, SUFFIX_DOUBLE, SUFFIX_SIGNED_LONG, SUFFIX_UNSIGNED_LONG, SUFFIX_SIGNED_SHORT, SUFFIX_UNSIGNED_SHORT, SUFFIX_UNSIGNED_INT, SUFFIX_NONE
			};
		private:
			string _raw_text, _parent_directory, _file_name;
			kind _token_kind;
			prefix _prefix_kind;
			suffix _suffix_kind;
			int _start_column_number, _end_column_number, _line_number;
			bool _valid;
		public:
			token(string pd, string fn, int l, int scn, int ecn, kind tk, string rt, prefix pk = prefix::PREFIX_NONE, suffix sk = suffix::SUFFIX_NONE);
			~token();
			int line_number() const;
			int start_column_number() const;
			int end_column_number() const;
			kind token_kind() const;
			string raw_text() const;
			bool valid() const;
			prefix prefix_kind() const;
			suffix suffix_kind() const;
			string file_name() const;
			string parent_directory() const;
		};
	}
}

#endif
