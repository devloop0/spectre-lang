#ifndef SPECTRE_PARSER_HPP
#define SPECTRE_PARSER_HPP

#include "expr.hpp"
#include "type.hpp"
#include "stmt.hpp"
#include "diagnostics.hpp"
#include "error.hpp"
#include <experimental/filesystem>
#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <algorithm>
#include <stack>

using spectre::ast::primary_expression;
using spectre::ast::type;
using spectre::ast::primitive_type;
using spectre::ast::stmt;
using spectre::ast::binary_expression;
using spectre::ast::value_kind;
using spectre::ast::postfix_expression;
using spectre::ast::unary_expression;
using spectre::ast::ternary_expression;
using spectre::ast::assignment_expression;
using spectre::ast::expression;
using spectre::ast::variable_declaration;
using spectre::ast::if_stmt;
using spectre::ast::function_type;
using spectre::ast::function_stmt;
using spectre::ast::return_stmt;
using spectre::ast::while_stmt;
using spectre::ast::block_stmt;
using spectre::ast::break_continue_stmt;
using spectre::ast::for_stmt;
using spectre::ast::case_default_stmt;
using spectre::ast::switch_stmt;
using spectre::ast::struct_stmt;
using spectre::ast::struct_type;
using spectre::ast::namespace_stmt;
using spectre::ast::using_stmt;
using spectre::ast::asm_stmt;
using spectre::ast::include_stmt;
using spectre::lexer::diagnostics;
using spectre::lexer::error;
using std::shared_ptr;
using std::vector;
using std::enable_shared_from_this;
using std::function;
using std::map;
using std::to_string;
using std::reverse;
using std::tuple;
using std::stack;

namespace filesystem = std::experimental::filesystem;
namespace fs = filesystem;

namespace spectre {
	namespace parser {

		class scope;
		class contained_variable_declaration;

		class symbol {
		public:
			enum class kind {
				KIND_VARIABLE, KIND_FUNCTION, KIND_STRUCT, KIND_NAMESPACE, KIND_NONE
			};
		private:
			kind _symbol_kind;
			shared_ptr<scope> _parent_scope;
			string _file_name;
		public:
			symbol(kind k, shared_ptr<scope> ps);
			~symbol();
			kind symbol_kind();
			shared_ptr<scope> parent_scope();
		};

		class variable_symbol : public symbol {
		private:
			token _variable_name;
			shared_ptr<type> _variable_type;
			vector<token> _stream;
		public:
			variable_symbol(shared_ptr<scope> ps, token vn, shared_ptr<type> vt, vector<token> s);
			~variable_symbol();
			shared_ptr<type> variable_type();
			token variable_name();
			vector<token> stream();
		};

		class function_symbol : public symbol {
		private:
			token _function_name;
			shared_ptr<function_type> _function_symbol_type;
			vector<token> _stream;
			function_stmt::defined_kind _function_symbol_defined_kind;
		public:
			function_symbol(shared_ptr<scope> ps, shared_ptr<function_type> fst, token fn, vector<token> s, function_stmt::defined_kind dk);
			~function_symbol();
			token function_name();
			shared_ptr<function_type> function_symbol_type();
			vector<token> stream();
			function_stmt::defined_kind function_symbol_defined_kind();
			void set_function_symbol_defined_kind(function_stmt::defined_kind dk);
		};

		class struct_symbol : public symbol {
		private:
			token _struct_name;
			vector<token> _stream;
			struct_stmt::defined_kind _struct_symbol_defined_kind;
			shared_ptr<struct_type> _struct_symbol_type;
			shared_ptr<scope> _struct_symbol_scope;
		public:
			struct_symbol(shared_ptr<scope> ps, shared_ptr<struct_type> sst, token sn, shared_ptr<scope> ss, vector<token> s, struct_stmt::defined_kind dk);
			~struct_symbol();
			vector<token> stream();
			struct_stmt::defined_kind struct_symbol_defined_kind();
			shared_ptr<struct_type> struct_symbol_type();
			token struct_name();
			void set_struct_symbol_defined_kind(struct_stmt::defined_kind dk);
			shared_ptr<scope> struct_symbol_scope();
		};

		class namespace_scope;

		class namespace_symbol : public symbol {
		private:
			token _namespace_symbol_name;
			vector<token> _stream;
			namespace_stmt::defined_kind _namespace_symbol_defined_kind;
			shared_ptr<namespace_scope> _namespace_symbol_scope;
		public:
			namespace_symbol(shared_ptr<scope> ps, token nsn, shared_ptr<namespace_scope> ns, vector<token> s, namespace_stmt::defined_kind dk);
			~namespace_symbol();
			vector<token> stream();
			namespace_stmt::defined_kind namespace_symbol_defined_kind();
			token namespace_symbol_name();
			void set_namespace_symbol_defined_kind(namespace_stmt::defined_kind dk);
			shared_ptr<namespace_scope> namespace_symbol_scope();
		};

		class scope {
		public:
			enum class kind {
				KIND_GLOBAL, KIND_STRUCT, KIND_BLOCK, KIND_FUNCTION, KIND_LOOP, KIND_SWITCH, KIND_NAMESPACE, KIND_NONE
			};
		private:
			shared_ptr<scope> _parent_scope;
			vector<shared_ptr<symbol>> _symbol_list;
			kind _scope_kind;
		public:
			scope(kind sk, shared_ptr<scope> ps);
			~scope();
			void add_symbol(shared_ptr<symbol> s);
			shared_ptr<symbol> find_symbol(token t, bool strict);
			kind scope_kind();
			shared_ptr<scope> parent_scope();
			vector<shared_ptr<symbol>> symbol_list();
		};

		class namespace_scope : public scope {
		private:
			token _namespace_name;
		public:
			namespace_scope(token nn, shared_ptr<scope> ps);
			~namespace_scope();
			token namespace_name();
		};

		class switch_scope : public scope {
		private:
			shared_ptr<expression> _parent_expression;
			bool _default_stmt_hit;
			vector<shared_ptr<case_default_stmt>> _case_default_stmt_list;
		public:
			switch_scope(shared_ptr<expression> pe, shared_ptr<scope> ps);
			~switch_scope();
			shared_ptr<expression> parent_expression();
			bool default_stmt_hit();
			void default_stmt_hit(bool b);
			vector<shared_ptr<case_default_stmt>> case_default_stmt_list();
			void add_case_default_stmt(shared_ptr<case_default_stmt> c);
		};

		class block_scope : public scope {
		private:
			int _block_number;
		public:
			block_scope(int bn, shared_ptr<scope> ps);
			~block_scope();
			int block_number();
		};

		class function_scope : public scope {
		private:
			token _function_name;
			shared_ptr<type> _return_type;
			type::const_kind _function_const_kind;
			type::static_kind _function_static_kind;
		public:
			function_scope(shared_ptr<scope> ps, type::const_kind ck, type::static_kind sk, shared_ptr<type> rt, token fn);
			~function_scope();
			token function_name();
			shared_ptr<type> return_type();
			type::const_kind function_const_kind();
			type::static_kind function_static_kind();
		};

		class parser : public enable_shared_from_this<parser> {
		private:
			vector<shared_ptr<stmt>> _stmt_list;
			buffer _source_buffer;
			shared_ptr<diagnostics> _diagnostics_reporter;
			int _pos, _block_scope_number;
			shared_ptr<scope> _global_scope, _current_scope;
			shared_ptr<scope> within_scope_helper(scope::kind sk, shared_ptr<scope> s);
			int _case_default_stmt_label_number_counter;
			int _struct_reference_number_counter;
			int _function_reference_number_counter;
			vector<shared_ptr<struct_symbol>> _struct_symbol_table;
			vector<shared_ptr<function_symbol>> _function_symbol_table;
			vector<token> _consumed_token_list;
			vector<shared_ptr<fs::path>> _import_list;
		public:
			parser(buffer sb);
			~parser();
			vector<shared_ptr<stmt>> stmt_list();

			void report(error err);
			static void report_internal(string msg, string fn, int ln, string fl);

			void add_symbol_to_current_scope(shared_ptr<symbol> sym);
			shared_ptr<symbol> find_symbol_in_current_scope(token t, bool strict);
			shared_ptr<scope> current_scope();
			shared_ptr<scope> global_scope();
			shared_ptr<scope> within_scope(scope::kind sk);
			void open_new_scope(shared_ptr<scope> s);
			void close_current_scope();

			void backtrack();
			void set_buffer_position(int pos);
			token pop();
			token peek();
			token get_token(int pos);
			int get_buffer_position();

			bool parse_stmt();
			int next_block_scope_number();
			int next_case_default_stmt_label_number();
			int next_struct_reference_number();
			int next_function_reference_number();

			void add_to_struct_symbol_table(shared_ptr<struct_symbol> s);
			shared_ptr<struct_symbol> find_struct_symbol(token n, int r);
			void add_to_function_symbol_table(shared_ptr<function_symbol> s);
			shared_ptr<function_symbol> find_function_symbol(token n, int r);
			void insert_token_list(int p, vector<token> vec);

			void add_to_import_list(shared_ptr<fs::path> f);
			bool in_import_list(shared_ptr<fs::path> f);
		};

		class primary_expression_parser {
		private:
			shared_ptr<primary_expression> _contained_primary_expression;
			bool _valid;
		public:
			primary_expression_parser(shared_ptr<parser> p);
			~primary_expression_parser();
			shared_ptr<primary_expression> contained_primary_expression();
			bool valid();
		};

		class stmt_parser {
		private:
			shared_ptr<stmt> _contained_stmt;
			bool _valid;
		public:
			stmt_parser(shared_ptr<parser> p);
			~stmt_parser();
			bool valid();
			shared_ptr<stmt> contained_stmt();
		};

		class postfix_expression_parser {
		private:
			shared_ptr<postfix_expression> _contained_postfix_expression;
			bool _valid;
		public:
			postfix_expression_parser(shared_ptr<parser> p);
			~postfix_expression_parser();
			shared_ptr<postfix_expression> contained_postfix_expression();
			bool valid();
		};

		class unary_expression_parser {
		private:
			shared_ptr<unary_expression> _contained_unary_expression;
			bool _valid;
		public:
			unary_expression_parser(shared_ptr<parser> p);
			~unary_expression_parser();
			shared_ptr<unary_expression> contained_unary_expression();
			bool valid();
		};

		class ternary_expression_parser {
		private:
			shared_ptr<ternary_expression> _contained_ternary_expression;
			bool _valid;
		public:
			ternary_expression_parser(shared_ptr<parser> p);
			~ternary_expression_parser();
			shared_ptr<ternary_expression> contained_ternary_expression();
			bool valid();
		};

		class expression_parser {
		private:
			shared_ptr<expression> _contained_expression;
			bool _valid;
		public:
			expression_parser(shared_ptr<parser> p);
			~expression_parser();
			shared_ptr<expression> contained_expression();
			bool valid();
		};

		namespace expression_parser_helper {

			shared_ptr<class binary_expression> parse_left_associative_binary_expression(shared_ptr<parser> p, function<shared_ptr<binary_expression>(shared_ptr<parser>)> base_func,
				map<token::kind, binary_expression::operator_kind> mapping);

			shared_ptr<binary_expression> parse_multiplicative_expression(shared_ptr<parser> p);
			shared_ptr<binary_expression> parse_additive_expression(shared_ptr<parser> p);
			shared_ptr<binary_expression> parse_bitwise_shift_expression(shared_ptr<parser> p);
			shared_ptr<binary_expression> parse_relational_comparative_expression(shared_ptr<parser> p);
			shared_ptr<binary_expression> parse_relational_equality_expression(shared_ptr<parser> p);
			shared_ptr<binary_expression> parse_bitwise_and_expression(shared_ptr<parser> p);
			shared_ptr<binary_expression> parse_bitwise_xor_expression(shared_ptr<parser> p);
			shared_ptr<binary_expression> parse_bitwise_or_expression(shared_ptr<parser> p);
			shared_ptr<binary_expression> parse_logical_and_expression(shared_ptr<parser> p);
			shared_ptr<binary_expression> parse_logical_or_expression(shared_ptr<parser> p);

			shared_ptr<assignment_expression> parse_assignment_expression(shared_ptr<parser> p);
		}

		shared_ptr<type> deduce_postfix_expression_type(shared_ptr<parser> p, shared_ptr<type> t, value_kind vk, token op, vector<token> stream);
		shared_ptr<type> deduce_unary_expression_type(shared_ptr<parser> p, shared_ptr<type> t, value_kind vk, unary_expression::kind uek, vector<token> stream);
		shared_ptr<type> deduce_binary_expression_type(shared_ptr<parser> p, shared_ptr<binary_expression> lhs, token op, shared_ptr<binary_expression> rhs);
		shared_ptr<type> deduce_ternary_expression_type(shared_ptr<parser> p, shared_ptr<binary_expression> cond, token q, shared_ptr<expression> true_path, token c,
			shared_ptr<ternary_expression> false_path);
		shared_ptr<type> deduce_assignment_expression_type(shared_ptr<parser> p, shared_ptr<unary_expression> lhs, token op, shared_ptr<assignment_expression> ae);

		value_kind deduce_binary_expression_value_kind(shared_ptr<parser> p, shared_ptr<binary_expression> lhs, binary_expression::operator_kind op, shared_ptr<binary_expression> rhs);
		value_kind deduce_unary_expression_value_kind(shared_ptr<parser> p, unary_expression::kind uek);
		value_kind deduce_postfix_expression_value_kind(shared_ptr<parser> p, shared_ptr<postfix_expression::postfix_type> pt);
		value_kind deduce_ternary_expression_value_kind(shared_ptr<parser> p, shared_ptr<binary_expression> cond, shared_ptr<expression> true_path,
			shared_ptr<ternary_expression> false_path);

		class type_parser {
		private:
			shared_ptr<type> _contained_type;
			bool _valid;
			vector<token> _stream;
		public:
			type_parser(shared_ptr<parser> p);
			~type_parser();
			shared_ptr<type> contained_type();
			bool valid();
			vector<token> stream();
		};

		class variable_declaration_parser {
		private:
			vector<shared_ptr<variable_declaration>> _contained_variable_declaration_list;
			bool _valid;
			vector<token> _stream;
		public:
			variable_declaration_parser(shared_ptr<parser> p);
			~variable_declaration_parser();
			vector<shared_ptr<variable_declaration>> contained_variable_declaration_list();
			vector<token> stream();
			bool valid();
		};

		bool check_declaration_type_is_correct(shared_ptr<parser> p, shared_ptr<type_parser> tp);
		shared_ptr<variable_declaration> check_declaration_initializer_is_correct(shared_ptr<parser> p, shared_ptr<variable_declaration> vd);

		class if_stmt_parser {
		private:
			shared_ptr<if_stmt> _contained_if_stmt;
			bool _valid;
		public:
			if_stmt_parser(shared_ptr<parser> p);
			~if_stmt_parser();
			shared_ptr<if_stmt> contained_if_stmt();
			bool valid();
		};

		class function_stmt_parser {
		private:
			shared_ptr<function_stmt> _contained_function_stmt;
			bool _valid;
		public:
			function_stmt_parser(shared_ptr<parser> p);
			~function_stmt_parser();
			shared_ptr<function_stmt> contained_function_stmt();
			bool valid();
		};

		bool primitive_types_equal(shared_ptr<parser> p, shared_ptr<type> t1, shared_ptr<type> t2);
		bool matching_function_symbol_and_partial_function_stmt(shared_ptr<parser> p, shared_ptr<function_symbol> fsym, shared_ptr<function_type> ft, token fn,
			vector<token> s);

		shared_ptr<type> deduce_array_initializer_type(shared_ptr<parser> p, vector<shared_ptr<assignment_expression>> ai, vector<token> s);

		class return_stmt_parser {
		private:
			shared_ptr<return_stmt> _contained_return_stmt;
			bool _valid;
		public:
			return_stmt_parser(shared_ptr<parser> p);
			~return_stmt_parser();
			shared_ptr<return_stmt> contained_return_stmt();
			bool valid();
		};

		class while_stmt_parser {
		private:
			shared_ptr<while_stmt> _contained_while_stmt;
			bool _valid;
		public:
			while_stmt_parser(shared_ptr<parser> p);
			~while_stmt_parser();
			shared_ptr<while_stmt> contained_while_stmt();
			bool valid();
		};

		bool valid_statement_inside_block_scope(shared_ptr<stmt> s);

		class block_stmt_parser {
		private:
			shared_ptr<block_stmt> _contained_block_stmt;
			bool _valid;
		public:
			block_stmt_parser(shared_ptr<parser> p);
			~block_stmt_parser();
			shared_ptr<block_stmt> contained_block_stmt();
			bool valid();
		};

		bool primitive_types_compatible(shared_ptr<parser> p, shared_ptr<type> t1, shared_ptr<type> t2);

		class break_continue_stmt_parser {
		private:
			shared_ptr<break_continue_stmt> _contained_break_continue_stmt;
			bool _valid;
		public:
			break_continue_stmt_parser(shared_ptr<parser> p);
			~break_continue_stmt_parser();
			shared_ptr<break_continue_stmt> contained_break_continue_stmt();
			bool valid();
		};

		class for_stmt_parser {
		private:
			shared_ptr<for_stmt> _contained_for_stmt;
			bool _valid;
		public:
			for_stmt_parser(shared_ptr<parser> p);
			~for_stmt_parser();
			shared_ptr<for_stmt> contained_for_stmt();
			bool valid();
		};

		class do_while_stmt_parser {
		private:
			shared_ptr<while_stmt> _contained_do_while_stmt;
			bool _valid;
		public:
			do_while_stmt_parser(shared_ptr<parser> p);
			~do_while_stmt_parser();
			shared_ptr<while_stmt> contained_do_while_stmt();
			bool valid();
		};

		class case_default_stmt_parser {
		private:
			shared_ptr<case_default_stmt> _contained_case_default_stmt;
			bool _valid;
		public:
			case_default_stmt_parser(shared_ptr<parser> p);
			~case_default_stmt_parser();
			shared_ptr<case_default_stmt> contained_case_default_stmt();
			bool valid();
		};

		class switch_stmt_parser {
		private:
			shared_ptr<switch_stmt> _contained_switch_stmt;
			bool _valid;
		public:
			switch_stmt_parser(shared_ptr<parser> p);
			~switch_stmt_parser();
			shared_ptr<switch_stmt> contained_switch_stmt();
			bool valid();
		};

		class struct_stmt_parser {
		private:
			shared_ptr<struct_stmt> _contained_struct_stmt;
			bool _valid;
		public:
			struct_stmt_parser(shared_ptr<parser> p);
			~struct_stmt_parser();
			shared_ptr<struct_stmt> contained_struct_stmt();
			bool valid();
		};

		class namespace_stmt_parser {
		private:
			shared_ptr<namespace_stmt> _contained_namespace_stmt;
			bool _valid;
		public:
			namespace_stmt_parser(shared_ptr<parser> p);
			~namespace_stmt_parser();
			shared_ptr<namespace_stmt> contained_namespace_stmt();
			bool valid();
		};

		tuple<bool, vector<token>, shared_ptr<symbol>> handle_qualified_identifier_name(shared_ptr<parser> p, bool disallow_namespace_symbol);

		class using_stmt_parser {
		private:
			shared_ptr<using_stmt> _contained_using_stmt;
			bool _valid;
		public:
			using_stmt_parser(shared_ptr<parser> p);
			~using_stmt_parser();
			shared_ptr<using_stmt> contained_using_stmt();
			bool valid();
		};

		class asm_stmt_parser {
		private:
			shared_ptr<asm_stmt> _contained_asm_stmt;
			bool _valid;
		public:
			asm_stmt_parser(shared_ptr<parser> p);
			~asm_stmt_parser();
			shared_ptr<asm_stmt> contained_asm_stmt();
			bool valid();
		};

		class include_stmt_parser {
		private:
			shared_ptr<include_stmt> _contained_include_stmt;
			bool _valid;
		public:
			include_stmt_parser(shared_ptr<parser> p);
			~include_stmt_parser();
			shared_ptr<include_stmt> contained_include_stmt();
			bool valid();
		};

		bool is_constant_expression(shared_ptr<parser> p, shared_ptr<assignment_expression> aexpr);
		bool matching_function_types(shared_ptr<parser> p, shared_ptr<type> t1, shared_ptr<type> t2, bool e = false);
		bool exact_type_match(shared_ptr<parser> p, shared_ptr<type> t1, shared_ptr<type> t2);
	}
}

#endif