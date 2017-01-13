#ifndef SPECTRE_LIR_HPP
#define SPECTRE_LIR_HPP

#include "parser.hpp"
#include "error.hpp"
#include "lir_instruction_set.hpp"
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <stack>

using std::string;
using std::vector;
using std::shared_ptr;
using spectre::lexer::error;
using namespace spectre::lir::insns;
using spectre::parser::symbol;
using spectre::parser::scope;
using spectre::parser::block_scope;
using std::map;
using std::stoi;
using std::stod;
using std::stof;
using std::stol;
using std::stoul;
using std::stack;

namespace spectre {
	namespace lir {

		class insn {
		public:
			enum class kind {
				KIND_LABEL, KIND_REGULAR, KIND_DECL, KIND_PROC_HEADER, KIND_PROC_FOOTER, KIND_ARRAY, KIND_RET, KIND_CALL, KIND_STRUCT_HEADER, KIND_STRUCT_FOOTER,
				KIND_MALLOC, KIND_NONE
			};
		private:
			kind _insn_kind;
		public:
			insn(kind k);
			~insn();
			kind insn_kind();
		};

		class operand {
		public:
			enum kind {
				KIND_VARIABLE, KIND_REGISTER, KIND_IMMEDIATE, KIND_NONE
			};
		private:
			kind _operand_kind;
		public:
			operand(kind ok);
			~operand();
			kind operand_kind();
		};

		class register_operand : public operand {
		private:
			int _register_number;
		public:
			register_operand(int n);
			~register_operand();
			int register_number();
			string raw_text();
		};

		class variable_operand : public operand {
		private:
			string _variable_name;
		public:
			variable_operand(string vn);
			~variable_operand();
			string variable_name();
			string raw_text();
		};

		class immediate_operand : public operand {
		public:
			enum class kind {
				KIND_BOOL,
				KIND_SIGNED_BYTE, KIND_UNSIGNED_BYTE,
				KIND_SIGNED_CHAR, KIND_UNSIGNED_CHAR,
				KIND_SIGNED_SHORT, KIND_UNSIGNED_SHORT,
				KIND_SIGNED_INT, KIND_UNSIGNED_INT,
				KIND_SIGNED_LONG, KIND_UNSIGNED_LONG,
				KIND_FLOAT,
				KIND_DOUBLE,
				KIND_STRING,
				KIND_NONE
			};
		private:
			bool _immediate_bool;
			signed char _immediate_signed_byte;
			unsigned char _immediate_unsigned_byte;
			signed char _immediate_signed_char;
			unsigned char _immediate_unsigned_char;
			signed short _immediate_signed_short;
			unsigned short _immediate_unsigned_short;
			signed int _immediate_signed_int;
			unsigned int _immediate_unsigned_int;
			signed long _immediate_signed_long;
			unsigned long _immediate_unsigned_long;
			float _immediate_float;
			double _immediate_double;
			string _immediate_string;
			kind _immediate_operand_kind;
		public:
			immediate_operand(bool b);
			immediate_operand(signed char b_c, bool is_char = true);
			immediate_operand(unsigned char b_c, bool is_char = true);
			immediate_operand(signed short s);
			immediate_operand(unsigned short s);
			immediate_operand(signed int i);
			immediate_operand(unsigned int i);
			immediate_operand(signed long l);
			immediate_operand(unsigned long l);
			immediate_operand(float f);
			immediate_operand(double d);
			immediate_operand(string s);
			~immediate_operand();
			kind immediate_operand_kind();
			bool immediate_bool();
			signed char immediate_signed_byte();
			unsigned char immediate_unsigned_byte();
			signed char immediate_signed_char();
			unsigned char immediate_unsigned_char();
			signed short immediate_signed_short();
			unsigned short immediate_unsigned_short();
			signed int immediate_signed_int();
			unsigned int immediate_unsigned_int();
			signed long immediate_signed_long();
			unsigned long immediate_unsigned_long();
			float immediate_float();
			double immediate_double();
			string immediate_string();
			string raw_text();
		};

		class array_insn : public insn {
		private:
			vector<shared_ptr<operand>> _array_initializer_list;
			shared_ptr<operand> _store;
		public:
			array_insn(shared_ptr<operand> s, vector<shared_ptr<operand>> ail);
			~array_insn();
			shared_ptr<operand> store();
			vector<shared_ptr<operand>> array_initializer_list();
			string raw_text();
		};

		class regular_insn : public insn {
		public:
			enum class operation_kind {
				KIND_MOV, KIND_ADD, KIND_SUB, KIND_MUL, KIND_DIV, KIND_MOD, KIND_SHL, KIND_SHR, KIND_BAND, KIND_BOR, KIND_BXOR, KIND_LT, KIND_GT, KIND_LTE, KIND_GTE, KIND_EQ,
				KIND_NEQ, KIND_NEG, KIND_POS, KIND_INC, KIND_DEC, KIND_LNOT, KIND_BNOT, KIND_JMP, KIND_JLT, KIND_JLTE, KIND_JGT, KIND_JGTE, KIND_JEQ, KIND_JNEQ, KIND_DMOV, 
				KIND_SUBSCR, KIND_NONE
			};
			enum class operation_number_kind {
				KIND_0, KIND_1, KIND_2, KIND_3, KIND_NONE
			};
			const map<operation_kind, string> operation_2_raw_instruction_map = {
				{ regular_insn::operation_kind::KIND_MOV, _mov },
				{ regular_insn::operation_kind::KIND_ADD, _add },
				{ regular_insn::operation_kind::KIND_SUB, _sub },
				{ regular_insn::operation_kind::KIND_MUL, _mul },
				{ regular_insn::operation_kind::KIND_DIV, _div },
				{ regular_insn::operation_kind::KIND_MOD, _mod },
				{ regular_insn::operation_kind::KIND_SHL, _shl },
				{ regular_insn::operation_kind::KIND_SHR, _shr },
				{ regular_insn::operation_kind::KIND_BAND, _band },
				{ regular_insn::operation_kind::KIND_BOR, _bor },
				{ regular_insn::operation_kind::KIND_BXOR, _bxor },
				{ regular_insn::operation_kind::KIND_LT, _lt },
				{ regular_insn::operation_kind::KIND_GT, _gt },
				{ regular_insn::operation_kind::KIND_LTE, _lte },
				{ regular_insn::operation_kind::KIND_GTE, _gte },
				{ regular_insn::operation_kind::KIND_EQ, _eq },
				{ regular_insn::operation_kind::KIND_NEQ, _neq },
				{ regular_insn::operation_kind::KIND_NEG, _neg },
				{ regular_insn::operation_kind::KIND_POS, _pos },
				{ regular_insn::operation_kind::KIND_INC, _inc },
				{ regular_insn::operation_kind::KIND_DEC, _dec },
				{ regular_insn::operation_kind::KIND_LNOT, _lnot },
				{ regular_insn::operation_kind::KIND_BNOT, _bnot },
				{ regular_insn::operation_kind::KIND_JMP, _jmp },
				{ regular_insn::operation_kind::KIND_JLT, _jlt },
				{ regular_insn::operation_kind::KIND_JGT, _jgt },
				{ regular_insn::operation_kind::KIND_JLTE, _jlte },
				{ regular_insn::operation_kind::KIND_JGTE, _jgte },
				{ regular_insn::operation_kind::KIND_JEQ, _jeq },
				{ regular_insn::operation_kind::KIND_JNEQ, _jneq },
				{ regular_insn::operation_kind::KIND_DMOV, _dmov },
				{ regular_insn::operation_kind::KIND_SUBSCR, _subscr },
				{ regular_insn::operation_kind::KIND_NONE, "" }
			};
		private:
			shared_ptr<operand> _first_operand, _second_operand, _third_operand;
			operation_kind _regular_insn_operation_kind;
			operation_number_kind _regular_insn_operation_number_kind;
		public:
			regular_insn(operation_kind ok);
			regular_insn(operation_kind ok, shared_ptr<operand> f);
			regular_insn(operation_kind ok, shared_ptr<operand> f, shared_ptr<operand> s);
			regular_insn(operation_kind ok, shared_ptr<operand> f, shared_ptr<operand> s, shared_ptr<operand> t);
			shared_ptr<operand> first_operand();
			shared_ptr<operand> second_operand();
			shared_ptr<operand> third_operand();
			operation_kind regular_insn_operation_kind();
			operation_number_kind regular_insn_operation_number_kind();
			string raw_text();
		};

		class label_insn : public insn {
		private:
			string _label_name;
		public:
			label_insn(string ln);
			~label_insn();
			string label_name();
			string raw_text();
		};

		class proc_header_insn : public insn {
		private:
			shared_ptr<variable_operand> _function_name;
			vector<shared_ptr<variable_operand>> _function_parameter_list;
			vector<string> _function_parameter_type_list;
			string _return_type;
		public:
			proc_header_insn(string rt, shared_ptr<variable_operand> fn, vector<shared_ptr<variable_operand>> fpl, vector<string> fptl);
			~proc_header_insn();
			shared_ptr<variable_operand> function_name();
			vector<shared_ptr<variable_operand>> function_parameter_list();
			vector<string> function_parameter_type_list();
			string return_type();
			string raw_text();
		};

		class proc_footer_insn : public insn {
		public:
			proc_footer_insn();
			~proc_footer_insn();
			string raw_text();
		};

		class ret_insn : public insn {
		private:
			shared_ptr<operand> _return_value;
		public:
			ret_insn(shared_ptr<operand> rv);
			ret_insn();
			~ret_insn();
			shared_ptr<operand> return_value();
			string raw_text();
		};

		class decl_insn : public insn {
		private:
			shared_ptr<variable_operand> _decl_variable_operand;
			shared_ptr<operand> _rhs;
			string _decl_type;
		public:
			decl_insn(string dt, shared_ptr<variable_operand> dvo, shared_ptr<operand> rhs);
			decl_insn(string dt, shared_ptr<variable_operand> dvo);
			~decl_insn();
			shared_ptr<variable_operand> decl_variable_operand();
			shared_ptr<operand> rhs();
			string decl_type();
			string raw_text();
		};

		class struct_header_insn : public insn {
		private:
			string _struct_name;
		public:
			struct_header_insn(string sn);
			~struct_header_insn();
			string struct_name();
			string raw_text();
		};

		class struct_footer_insn : public insn {
		public:
			struct_footer_insn();
			~struct_footer_insn();
			string raw_text();
		};

		class call_insn : public insn {
		private:
			shared_ptr<variable_operand> _function_operand;
			shared_ptr<operand> _result_operand;
			vector<shared_ptr<operand>> _argument_operand_list;
		public:
			call_insn(shared_ptr<operand> ro, shared_ptr<variable_operand> fo, vector<shared_ptr<operand>> aol);
			call_insn(shared_ptr<variable_operand> fo, vector<shared_ptr<operand>> aol);
			~call_insn();
			shared_ptr<variable_operand> function_operand();
			shared_ptr<operand> result_operand();
			vector<shared_ptr<operand>> argument_operand_list();
			string raw_text();
		};

		class malloc_insn : public insn {
		private:
			string _malloc_type;
			shared_ptr<operand> _destination;
			shared_ptr<operand> _number;
		public:
			malloc_insn(string mt, shared_ptr<operand> d, shared_ptr<operand> n);
			~malloc_insn();
			string malloc_type();
			shared_ptr<operand> destination();
			shared_ptr<operand> number();
			string raw_text();
		};

		class lir_code {
		private:
			vector<shared_ptr<insn>> _instruction_list;
			vector<shared_ptr<operand>> _post_increment_operand_list, _post_decrement_operand_list, _pre_decrement_operand_list,
				_pre_increment_operand_list;
			int _current_register_counter;
			vector<int> _pre_increment_decrement_flag_list;
			int _current_internal_label_counter;
			shared_ptr<spectre::parser::parser> _ast_parser;
			stack<shared_ptr<label_insn>> _loop_start_stack, _loop_end_stack;
		public:
			lir_code(shared_ptr<spectre::parser::parser> p);
			~lir_code();
			void add_instruction(shared_ptr<insn> i);
			void add_post_increment_operand(shared_ptr<operand> o);
			void add_post_decrement_operand(shared_ptr<operand> o);
			void add_pre_increment_operand(shared_ptr<operand> o);
			void add_pre_decrement_operand(shared_ptr<operand> o);
			void insert_pre_post_increment_decrement_operands();
			void flag_pre_increment_decrement_position();
			shared_ptr<register_operand> generate_next_register_operand();
			vector<shared_ptr<insn>> instruction_list();
			vector<string> raw_instruction_list();
			void report(error e);
			void report_internal(string msg, string fn, int ln, string fl);
			shared_ptr<label_insn> generate_next_internal_label_insn();
			void push_to_loop_start_stack(shared_ptr<label_insn> s);
			void push_to_loop_end_stack(shared_ptr<label_insn> s);
			shared_ptr<label_insn> loop_start_stack_top();
			shared_ptr<label_insn> loop_end_stack_top();
			void pop_loop_start_stack();
			void pop_loop_end_stack();
		};

		string get_operand_text(shared_ptr<operand> op);
		string get_insn_text(shared_ptr<insn> i);

		const static map<binary_expression::operator_kind, regular_insn::operation_kind> binary_operator_2_operation_map = {
			{ binary_expression::operator_kind::KIND_MULTIPLY, regular_insn::operation_kind::KIND_MUL },
			{ binary_expression::operator_kind::KIND_DIVIDE, regular_insn::operation_kind::KIND_DIV },
			{ binary_expression::operator_kind::KIND_MODULUS, regular_insn::operation_kind::KIND_MOD },
			{ binary_expression::operator_kind::KIND_ADD, regular_insn::operation_kind::KIND_ADD },
			{ binary_expression::operator_kind::KIND_SUBTRACT, regular_insn::operation_kind::KIND_SUB },
			{ binary_expression::operator_kind::KIND_SHIFT_LEFT, regular_insn::operation_kind::KIND_SHL },
			{ binary_expression::operator_kind::KIND_SHIFT_RIGHT, regular_insn::operation_kind::KIND_SHR },
			{ binary_expression::operator_kind::KIND_GREATER_THAN, regular_insn::operation_kind::KIND_GT },
			{ binary_expression::operator_kind::KIND_LESS_THAN, regular_insn::operation_kind::KIND_LT },
			{ binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO, regular_insn::operation_kind::KIND_GTE },
			{ binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO, regular_insn::operation_kind::KIND_LTE },
			{ binary_expression::operator_kind::KIND_EQUALS_EQUALS, regular_insn::operation_kind::KIND_EQ },
			{ binary_expression::operator_kind::KIND_NOT_EQUALS, regular_insn::operation_kind::KIND_NEQ },
			{ binary_expression::operator_kind::KIND_BITWISE_AND, regular_insn::operation_kind::KIND_BAND },
			{ binary_expression::operator_kind::KIND_BITWISE_OR, regular_insn::operation_kind::KIND_BOR },
			{ binary_expression::operator_kind::KIND_BITWISE_XOR, regular_insn::operation_kind::KIND_BXOR },
			{ binary_expression::operator_kind::KIND_EQUALS, regular_insn::operation_kind::KIND_MOV },
			{ binary_expression::operator_kind::KIND_MULTIPLY_EQUALS, regular_insn::operation_kind::KIND_MUL },
			{ binary_expression::operator_kind::KIND_DIVIDE_EQUALS, regular_insn::operation_kind::KIND_DIV },
			{ binary_expression::operator_kind::KIND_MODULUS_EQUALS, regular_insn::operation_kind::KIND_MOD },
			{ binary_expression::operator_kind::KIND_ADD_EQUALS, regular_insn::operation_kind::KIND_ADD },
			{ binary_expression::operator_kind::KIND_SUBTRACT_EQUALS, regular_insn::operation_kind::KIND_SUB },
			{ binary_expression::operator_kind::KIND_SHIFT_LEFT_EQUALS, regular_insn::operation_kind::KIND_SHL },
			{ binary_expression::operator_kind::KIND_SHIFT_RIGHT_EQUALS, regular_insn::operation_kind::KIND_SHR},
			{ binary_expression::operator_kind::KIND_BITWISE_AND_EQUALS, regular_insn::operation_kind::KIND_BAND },
			{ binary_expression::operator_kind::KIND_BITWISE_OR_EQUALS, regular_insn::operation_kind::KIND_BOR },
			{ binary_expression::operator_kind::KIND_BITWISE_XOR_EQUALS, regular_insn::operation_kind::KIND_BXOR },
			{ binary_expression::operator_kind::KIND_NONE, regular_insn::operation_kind::KIND_NONE }
		};

		const static map<unary_expression::kind, regular_insn::operation_kind> unary_operator_2_operation_map = {
			{ unary_expression::kind::KIND_BITWISE_NOT, regular_insn::operation_kind::KIND_BNOT },
			{ unary_expression::kind::KIND_DECREMENT, regular_insn::operation_kind::KIND_DEC },
			{ unary_expression::kind::KIND_INCREMENT, regular_insn::operation_kind::KIND_INC },
			{ unary_expression::kind::KIND_LOGICAL_NOT, regular_insn::operation_kind::KIND_LNOT },
			{ unary_expression::kind::KIND_MINUS, regular_insn::operation_kind::KIND_NEG },
			{ unary_expression::kind::KIND_NONE, regular_insn::operation_kind::KIND_NONE },
			{ unary_expression::kind::KIND_PLUS, regular_insn::operation_kind::KIND_POS }
		};

		shared_ptr<operand> literal_token_2_operand(shared_ptr<lir_code> lc, token lit, shared_ptr<type> t);
		string type_2_string(shared_ptr<lir_code> lc, shared_ptr<type> t, bool spaces);
		string scope_2_string_helper(shared_ptr<lir_code> lc, shared_ptr<scope> s, string r);
		string scope_2_string(shared_ptr<lir_code> lc, shared_ptr<scope> s);
		string symbol_2_string_helper(shared_ptr<lir_code> lc, shared_ptr<symbol> vs, string r);
		string symbol_2_string(shared_ptr<lir_code> lc, shared_ptr<symbol> vs);

		void generate_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<stmt> s);
		shared_ptr<operand> generate_expression_lir(shared_ptr<lir_code> lc, shared_ptr<expression> e, bool reference);
		shared_ptr<operand> generate_assignment_expression_lir(shared_ptr<lir_code> lc, shared_ptr<assignment_expression> ae, bool reference);
		shared_ptr<operand> generate_ternary_expression_lir(shared_ptr<lir_code> lc, shared_ptr<ternary_expression> te, bool reference);
		shared_ptr<operand> generate_binary_expression_lir(shared_ptr<lir_code> lc, shared_ptr<binary_expression> be, bool reference);
		shared_ptr<operand> generate_unary_expression_lir(shared_ptr<lir_code> lc, shared_ptr<unary_expression> ue, bool reference);
		shared_ptr<operand> generate_postfix_expression_lir(shared_ptr<lir_code> lc, shared_ptr<postfix_expression> pe, bool reference);
		shared_ptr<operand> generate_primary_expression_lir(shared_ptr<lir_code> lc, shared_ptr<primary_expression> pe, bool reference);
		shared_ptr<operand> generate_variable_declaration_lir(shared_ptr<lir_code> lc, shared_ptr<variable_declaration> vd, bool qualify);
		void generate_if_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<if_stmt> is);
		void generate_function_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<function_stmt> fstmt);
		void generate_return_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<return_stmt> rstmt);
		void generate_while_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<while_stmt> wstmt);
		void generate_block_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<block_stmt> bstmt);
		void generate_break_continue_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<break_continue_stmt> bcstmt);
		void generate_for_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<for_stmt> fstmt);
		void generate_do_while_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<while_stmt> wstmt);
		void generate_case_default_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<case_default_stmt> cdstmt);
		void generate_switch_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<switch_stmt> sstmt);
		void generate_struct_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<struct_stmt> sstmt);
	}
}

#endif