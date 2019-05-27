#ifndef SPECTRE_EXPR_HPP
#define SPECTRE_EXPR_HPP

#include "buffer.hpp"
#include "token.hpp"
#include "type.hpp"

using spectre::lexer::bad_token;
using spectre::lexer::token;
using spectre::lexer::buffer;
using spectre::ast::type;

namespace spectre {
	namespace parser {
		class symbol;
	}
}

using spectre::parser::symbol;

namespace spectre {
	namespace ast {

		class expression;
		class assignment_expression;
		class block_stmt;

		enum class value_kind {
			VALUE_LVALUE, VALUE_RVALUE, VALUE_NONE
		};

		class primary_expression {
		public:
			enum class kind {
				KIND_IDENTIFIER, KIND_LITERAL, KIND_PARENTHESIZED_EXPRESSION, KIND_ARRAY_INITIALIZER, KIND_NEW, KIND_STK, KIND_RESV, KIND_SIZEOF_EXPRESSION,
				KIND_SIZEOF_TYPE, KIND_STMT_EXPR, KIND_NONE
			};
		private:
			kind _primary_expression_kind;
			token _literal_token;
			shared_ptr<type> _primary_expression_type;
			shared_ptr<expression> _parenthesized_expression;
			shared_ptr<block_stmt> _stmt_expression;
			vector<shared_ptr<assignment_expression>> _array_initializer;
			shared_ptr<symbol> _identifier_symbol;
			vector<token> _stream;
			bool _valid;
			value_kind _primary_expression_value_kind;
			shared_ptr<type> _mem_type, _sizeof_type;
		public:
			primary_expression(kind pek, token lt, shared_ptr<type> pet, bool v, vector<token> s, value_kind vk);
			primary_expression(kind pek, shared_ptr<expression> e, shared_ptr<type> pet, bool v, vector<token> s, value_kind vk);
			primary_expression(kind pek, token lt, shared_ptr<symbol> is, shared_ptr<type> pet, bool v, vector<token> s, value_kind vk);
			primary_expression(kind pek, vector<shared_ptr<assignment_expression>> ai, shared_ptr<type> pet, bool v, vector<token> s);
			primary_expression(kind pek, shared_ptr<type> nt, shared_ptr<expression> e, shared_ptr<type> pet, bool v, vector<token> s);
			primary_expression(kind pek, shared_ptr<type> t, shared_ptr<type> pet, bool v, vector<token> s);
			primary_expression(kind pek, shared_ptr<expression> e, shared_ptr<type> pet, bool v, vector<token> s);
			primary_expression(kind pek, shared_ptr<block_stmt> se, shared_ptr<type> pet, bool v, vector<token> s);
			primary_expression();
			~primary_expression();
			kind primary_expression_kind();
			token literal_token();
			shared_ptr<type> primary_expression_type();
			bool valid();
			vector<token> stream();
			shared_ptr<expression> parenthesized_expression();
			value_kind primary_expression_value_kind();
			shared_ptr<symbol> identifier_symbol();
			vector<shared_ptr<assignment_expression>> array_initializer();
			shared_ptr<type> mem_type();
			shared_ptr<type> sizeof_type();
			shared_ptr<block_stmt> stmt_expression();
		};

		class postfix_expression {
		public:
			enum class kind {
				KIND_INCREMENT, KIND_DECREMENT, KIND_FUNCTION_CALL, KIND_SUBSCRIPT, KIND_MEMBER, KIND_ARROW, KIND_ADDRESS, KIND_AT, KIND_AS, KIND_NONE
			};
			class postfix_type {
			private:
				kind _postfix_type_kind;
				shared_ptr<type> _function_type;
				vector<shared_ptr<assignment_expression>> _argument_list;
				shared_ptr<expression> _subscript;
				shared_ptr<type> _postfix_type_type;
				token _member;
			public:
				postfix_type(kind k, shared_ptr<type> t);
				postfix_type(shared_ptr<type> ft, vector<shared_ptr<assignment_expression>> al, shared_ptr<type> t);
				postfix_type(shared_ptr<expression> s, shared_ptr<type> t);
				postfix_type(kind k, token m, shared_ptr<type> t);
				~postfix_type();
				kind postfix_type_kind();
				vector<shared_ptr<assignment_expression>> argument_list();
				shared_ptr<expression> subscript();
				token member();
				shared_ptr<type> postfix_type_type();
				shared_ptr<type> function_type();
			};
		private:
			shared_ptr<primary_expression> _contained_primary_expression;
			shared_ptr<type> _postfix_expression_type;
			vector<token> _stream;
			bool _valid;
			value_kind _postfix_expression_value_kind;
			vector<shared_ptr<postfix_type>> _postfix_type_list;
		public:
			postfix_expression(vector<shared_ptr<postfix_type>> ptl, shared_ptr<primary_expression> cpe, shared_ptr<type> t, bool v,
				vector<token> s, value_kind vk);
			~postfix_expression();
			shared_ptr<primary_expression> contained_primary_expression();
			shared_ptr<type> postfix_expression_type();
			vector<token> stream();
			bool valid();
			value_kind postfix_expression_value_kind();
			vector<shared_ptr<postfix_type>> postfix_type_list();
		};

		class unary_expression {
		public:
			enum class kind {
				KIND_INCREMENT, KIND_DECREMENT, KIND_PLUS, KIND_MINUS, KIND_LOGICAL_NOT, KIND_BITWISE_NOT, KIND_NONE
			};
		private:
			shared_ptr<postfix_expression> _contained_postfix_expression;
			shared_ptr<type> _unary_expression_type;
			vector<token> _stream;
			bool _valid;
			value_kind _unary_expression_value_kind;
			vector<kind> _unary_expression_kind_list;
			kind _unary_expression_kind;
		public:
			unary_expression(vector<kind> uekl, shared_ptr<postfix_expression> cpe, shared_ptr<type> t, bool v, vector<token> s, value_kind vk, kind k);
			~unary_expression();
			shared_ptr<postfix_expression> contained_postfix_expression();
			shared_ptr<type> unary_expression_type();
			vector<token> stream();
			bool valid();
			value_kind unary_expression_value_kind();
			vector<kind> unary_expression_kind_list();
			kind unary_expression_kind();
		};

		class binary_expression {
		public:
			enum class operator_kind {
				KIND_NONE,
				KIND_MULTIPLY, KIND_DIVIDE, KIND_MODULUS,
				KIND_ADD, KIND_SUBTRACT,
				KIND_SHIFT_LEFT, KIND_SHIFT_RIGHT,
				KIND_GREATER_THAN, KIND_LESS_THAN, KIND_GREATER_THAN_OR_EQUAL_TO, KIND_LESS_THAN_OR_EQUAL_TO,
				KIND_EQUALS_EQUALS, KIND_NOT_EQUALS,
				KIND_BITWISE_AND,
				KIND_BITWISE_XOR,
				KIND_BITWISE_OR,
				KIND_LOGICAL_AND,
				KIND_LOGICAL_OR,

				KIND_EQUALS,
				KIND_MULTIPLY_EQUALS, KIND_DIVIDE_EQUALS, KIND_MODULUS_EQUALS,
				KIND_ADD_EQUALS, KIND_SUBTRACT_EQUALS,
				KIND_SHIFT_LEFT_EQUALS, KIND_SHIFT_RIGHT_EQUALS,
				KIND_BITWISE_AND_EQUALS, KIND_BITWISE_OR_EQUALS,
				KIND_BITWISE_XOR_EQUALS, KIND_LOGICAL_AND_EQUALS, KIND_LOGICAL_OR_EQUALS
			};
			enum class kind {
				KIND_UNARY_EXPRESSION, KIND_BINARY_EXPRESSION, KIND_NONE
			};
		private:
			shared_ptr<unary_expression> _single_lhs;
			shared_ptr<binary_expression> _lhs, _rhs;
			operator_kind _operator_kind;
			shared_ptr<type> _binary_expression_type;
			vector<token> _stream;
			bool _valid;
			kind _binary_expression_kind;
			value_kind _binary_expression_value_kind;
		public:
			binary_expression(shared_ptr<type> t, shared_ptr<unary_expression> sl, vector<token> s, bool v, value_kind vk);
			binary_expression(shared_ptr<type> t, shared_ptr<binary_expression> l, operator_kind ok, shared_ptr<binary_expression> r, vector<token> s, bool v, value_kind vk);
			~binary_expression();
			shared_ptr<unary_expression> single_lhs();
			shared_ptr<binary_expression> lhs();
			shared_ptr<binary_expression> rhs();
			operator_kind binary_expression_operator_kind();
			vector<token> stream();
			shared_ptr<type> binary_expression_type();
			bool valid();
			kind binary_expression_kind();
			value_kind binary_expression_value_kind();
		};

		class ternary_expression {
		public:
			enum class kind {
				KIND_TERNARY, KIND_BINARY
			};
		private:
			shared_ptr<binary_expression> _condition;
			shared_ptr<expression> _true_path;
			shared_ptr<ternary_expression> _false_path;
			kind _ternary_expression_kind;
			bool _valid;
			value_kind _ternary_expression_value_kind;
			shared_ptr<type> _ternary_expression_type;
			vector<token> _stream;
		public:
			ternary_expression(shared_ptr<type> t, shared_ptr<binary_expression> bexpr, vector<token> s, bool v, value_kind vk);
			ternary_expression(shared_ptr<type> t, shared_ptr<binary_expression> cond, shared_ptr<expression> tp, shared_ptr<ternary_expression> fp, vector<token> s, bool v,
				value_kind vk);
			~ternary_expression();
			shared_ptr<binary_expression> condition();
			shared_ptr<expression> true_path();
			shared_ptr<ternary_expression> false_path();
			bool valid();
			kind ternary_expression_kind();
			value_kind ternary_expression_value_kind();
			shared_ptr<type> ternary_expression_type();
			vector<token> stream();
		};

		class assignment_expression {
		public:
			enum class kind {
				KIND_ASSIGNMENT, KIND_TERNARY
			};
		private:
			shared_ptr<ternary_expression> _conditional_expression;
			shared_ptr<unary_expression> _lhs_assignment;
			binary_expression::operator_kind _assignment_operator_kind;
			shared_ptr<assignment_expression> _rhs_assignment;
			bool _valid;
			vector<token> _stream;
			shared_ptr<type> _assignment_expression_type;
			value_kind _assignment_expression_value_kind;
			kind _assignment_expression_kind;
		public:
			assignment_expression(shared_ptr<type> t, shared_ptr<ternary_expression> te, vector<token> s, bool v, value_kind vk);
			assignment_expression(shared_ptr<type> t, shared_ptr<unary_expression> ue, binary_expression::operator_kind op, shared_ptr<assignment_expression> ae, vector<token> s,
				bool v, value_kind vk);
			~assignment_expression();
			shared_ptr<ternary_expression> conditional_expression();
			shared_ptr<unary_expression> lhs_assignment();
			binary_expression::operator_kind assignment_operator_kind();
			shared_ptr<assignment_expression> rhs_assignment();
			bool valid();
			vector<token> stream();
			shared_ptr<type> assignment_expression_type();
			value_kind assignment_expression_value_kind();
			kind assignment_expression_kind();
		};

		class expression {
		private:
			vector<shared_ptr<assignment_expression>> _assignment_expression_list;
			shared_ptr<type> _expression_type;
			bool _valid;
			vector<token> _stream;
			value_kind _expression_value_kind;
		public:
			expression(shared_ptr<type> t, vector<shared_ptr<assignment_expression>> ael, vector<token> s, bool v, value_kind vk);
			~expression();
			bool valid();
			vector<token> stream();
			vector<shared_ptr<assignment_expression>> assignment_expression_list();
			shared_ptr<type> expression_type();
			value_kind expression_value_kind();
		};
	}
}

#endif