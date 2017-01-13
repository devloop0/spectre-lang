#ifndef SPECTRE_MIPS_HPP
#define SPECTRE_MIPS_HPP

#include "token.hpp"
#include "buffer.hpp"
#include "error.hpp"
#include "diagnostics.hpp"
#include "expr.hpp"
#include "error.hpp"
#include "parser.hpp"
#include "stmt.hpp"
#include "type.hpp"
#include "mips_instruction_set.hpp"
#include <memory>
#include <string>
#include <vector>
#include <stack>

using namespace spectre::ast;
using namespace spectre::parser;
using namespace spectre::lexer;
using spectre::parser::parser;
using std::shared_ptr;
using std::string;
using std::vector;
using std::stack;
using std::pair;
using std::make_pair;
using std::tuple;

namespace spectre {
	namespace mips {

		class operand {
		public:
			enum class kind {
				KIND_REGISTER, KIND_IMMEDIATE, KIND_LABEL, KIND_NONE
			};
		private:
			kind _operand_kind;
		public:
			operand(kind k);
			~operand();
			kind operand_kind();
		};

		class register_operand : public operand {
		private:
			int _register_number;
			string _register_text;
		public:
			register_operand(int rn);
			register_operand(string rt);
			~register_operand();
			int register_number();
			string register_text();
			string raw_text();
		};

		class label_operand : public operand {
		private:
			string _label_text;
		public:
			label_operand(string lt);
			~label_operand();
			string label_text();
			string raw_text();
		};

		class immediate_operand : public operand {
		public:
			enum class kind {
				KIND_INTEGRAL, KIND_CHAR, KIND_NONE
			};
		private:
			int _integral_immediate;
			kind _immediate_operand_kind;
			string _immediate_operand_char;
		public:
			immediate_operand(int ii);
			immediate_operand(string c);
			~immediate_operand();
			int integral_immediate();
			string char_immediate();
			kind immediate_operand_kind();
			string raw_text();
		};

		class insn {
		public:
			enum class kind {
				KIND_ADD, KIND_ADDI, KIND_ADDU, KIND_ADDIU,
				KIND_AND, KIND_ANDI,
				KIND_BEQ, KIND_BGEZ, KIND_BGEZAL, KIND_BGTZ, KIND_BLEZ, KIND_BLTZ, KIND_BLTZAL, KIND_BNE,
				KIND_DIV, KIND_DIVU,
				KIND_JAL, KIND_J, KIND_JR,
				KIND_LA,
				KIND_LB, KIND_LH, KIND_LW, KIND_LHU, KIND_LBU,
				KIND_LUI,
				KIND_MFLO, KIND_MFHI, KIND_MULT, KIND_MULTU,
				KIND_NOOP,
				KIND_NOR, KIND_OR, KIND_ORI,
				KIND_SB, KIND_SH, KIND_SW,
				KIND_SLL, KIND_SLLV,
				KIND_SLT, KIND_SLTI, KIND_SLTIU, KIND_SLTU, KIND_SGT, KIND_SGTU, KIND_SLE, KIND_SLEU, KIND_SGE, KIND_SGEU, KIND_SEQ, KIND_SNE,
				KIND_SRA, KIND_SRAV,
				KIND_SRL, KIND_SRLV,
				KIND_SUB, KIND_SUBU,
				KIND_SYSCALL,
				KIND_XOR, KIND_XORI,
				KIND_ADD_S, KIND_ADD_D, KIND_SUB_S, KIND_SUB_D, KIND_MUL_S, KIND_MUL_D, KIND_DIV_S, KIND_DIV_D,
				KIND_LWC1, KIND_LDC1, KIND_SWC1, KIND_SDC1,
				KIND_L_S, KIND_L_D,
				KIND_BC1T, KIND_BC1F,
				KIND_CVT_D_S, KIND_CVT_D_W, KIND_CVT_S_D, KIND_CVT_S_W, KIND_CVT_W_D, KIND_CVT_W_S,
				KIND_C_EQ_S, KIND_C_EQ_D, KIND_C_GE_S, KIND_C_GE_D, KIND_C_GT_S, KIND_C_GT_D, KIND_C_LE_S, KIND_C_LE_D, KIND_C_LT_S, KIND_C_LT_D, KIND_C_NE_S, KIND_C_NE_D,
				KIND_MFC1, KIND_MTC1,
				KIND_MOV_S, KIND_MOV_D,
				KIND_NEG_S, KIND_NEG_D,
				KIND_NONE
			};
			enum class operand_number {
				KIND_0, KIND_1, KIND_2, KIND_3, KIND_NONE
			};
			enum class type {
				TYPE_REGULAR, TYPE_LABEL, TYPE_RAW, TYPE_NONE
			};
		private:
			shared_ptr<operand> _operand_1, _operand_2, _operand_3;
			kind _insn_kind;
			type _insn_type;
			operand_number _insn_operand_number;
			string _label_name;
			bool _tab;
		public:
			insn(kind ik, bool t = true);
			insn(kind ik, shared_ptr<operand> o1, bool t = true);
			insn(kind ik, shared_ptr<operand> o1, shared_ptr<operand> o2, bool t = true);
			insn(kind ik, shared_ptr<operand> o1, shared_ptr<operand> o2, shared_ptr<operand> o3, bool t = true);
			insn(string s);
			insn(type it, string s, bool t = true);
			insn();
			~insn();
			shared_ptr<operand> operand_1();
			shared_ptr<operand> operand_2();
			shared_ptr<operand> operand_3();
			kind insn_kind();
			operand_number insn_operand_number();
			string raw_text();
			type insn_type();
			string label_name();
			bool tab();
		};

		class data_directive {
		public:
			enum class directive_kind {
				KIND_SPACE, KIND_FLOAT, KIND_DOUBLE, KIND_STRING, KIND_NONE
			};
		private:
			shared_ptr<label_operand> _identifier;
			int _space;
			bool _tab;
			directive_kind _data_directive_kind;
			float _float_data;
			double _double_data;
			string _string_data;
		public:
			data_directive(bool t, shared_ptr<label_operand> i, int s);
			data_directive(bool t, shared_ptr<label_operand> i, float f);
			data_directive(bool t, shared_ptr<label_operand> i, double d);
			data_directive(bool t, shared_ptr<label_operand> i, string s);
			~data_directive();
			shared_ptr<label_operand> identifier();
			int space();
			bool tab();
			float float_data();
			double double_data();
			string string_data();
			string raw_text();
			directive_kind data_directive_kind();
		};

		class register_wrapper {
		public:
			enum class state_kind {
				KIND_ON_STACK, KIND_REGULAR, KIND_GLOBAL, KIND_NONE
			};
			enum class variable_kind {
				KIND_VARIABLE, KIND_NON_VARIABLE, KIND_NONE
			};
			enum class function_argument_kind {
				KIND_FUNCTION_ARGUMENT, KIND_NON_FUNCTION_ARGUMENT, KIND_NONE
			};
		private:
			shared_ptr<label_operand> _identifier;
			long _wrapper_id;
			int _offset_from_stack_ptr;
			shared_ptr<register_operand> _raw_register_operand;
			state_kind _register_wrapper_state_kind;
			variable_kind _register_wrapper_variable_kind;
			function_argument_kind _register_wrapper_function_argument_kind;
			bool _already_loaded, _double_precision;
		public:
			register_wrapper(long wid, shared_ptr<register_operand> rro);
			~register_wrapper();
			long wrapper_id();
			int offset_from_stack_ptr();
			void set_offset_from_stack_ptr(int o);
			void update_offset_from_stack_ptr(int o);
			shared_ptr<register_operand> raw_register_operand();
			state_kind register_wrapper_state_kind();
			void set_register_wrapper_state_kind(state_kind sk);
			void set_register_wrapper_variable_kind(variable_kind vk);
			variable_kind register_wrapper_variable_kind();
			shared_ptr<label_operand> identifier();
			void set_identifier(shared_ptr<label_operand> lo);
			void set_register_wrapper_function_argument_kind(function_argument_kind fak);
			function_argument_kind register_wrapper_function_argument_kind();
			bool already_loaded();
			void set_already_loaded(bool b);
			void set_double_precision(bool b);
			bool double_precision();
		};

		class operand_wrapper {
		public:
			enum class operand_kind {
				KIND_REGISTER, KIND_IMMEDIATE, KIND_LABEL, KIND_NONE
			};
		private:
			shared_ptr<register_wrapper> _contained_register_wrapper;
			shared_ptr<immediate_operand> _contained_immediate_operand;
			shared_ptr<label_operand> _contained_label_operand;
			operand_kind _wrapper_operand_kind;
		public:
			operand_wrapper(shared_ptr<register_wrapper> rw);
			operand_wrapper(shared_ptr<immediate_operand> io);
			operand_wrapper(shared_ptr<label_operand> lo);
			~operand_wrapper();
			shared_ptr<register_wrapper> contained_register_wrapper();
			shared_ptr<immediate_operand> contained_immediate_operand();
			shared_ptr<label_operand> contained_label_operand();
			operand_kind wrapper_operand_kind();
		};

		class mips_code;

		class mips_frame {
		private:
			int _local_stack_offset;
			vector<shared_ptr<register_wrapper>> _register_wrapper_list;
			map<int, bool> _register_usage, _s0_7_initial_store;
			map<string, long> _variable_name_2_id_map;
			map<long, string> _id_2_variable_name_map;
			map<int, int> _s_register_2_initial_offset_map;
			int _register_overflow_counter, _fp_register_overflow_counter;
			long& _register_wrapper_id_counter;
			bool _main_function;
			shared_ptr<mips_frame> _parent_frame;
			bool _fp_return_value, _dp_return_value;
		public:
			mips_frame(long& s, shared_ptr<mips_frame> pf, bool mf);
			~mips_frame();

			int local_stack_offset();
			void change_local_stack_offset(int o);

			int current_overflowed_register();
			int next_overflow_register();
			int current_fp_overflowed_register();
			int next_fp_overflow_register();

			bool is_register_marked(int r);
			void mark_register(int r);
			void unmark_register(int r);

			bool is_s_register_initially_stored(int r);
			void mark_s_register_initially_stored(int r);

			void remove_register_wrapper(long id);
			shared_ptr<register_wrapper> get_register_wrapper(long id);
			shared_ptr<register_wrapper> find_last_usage(int reg);
			shared_ptr<register_wrapper> generate_register_wrapper(shared_ptr<register_operand> ro);
			void restore_s_registers(shared_ptr<mips_code> mc);
			void set_s_register_initial_offset(int r, int o);
			void add_variable_id(string name, long id);
			long get_variable_id(string name);
			string get_variable(long id);
			bool main_function();
			shared_ptr<mips_frame> parent_frame();
			void set_fp_return_value(bool b);
			bool fp_return_value();
			bool dp_return_value();
			void set_dp_return_value(bool b);
		};

		class mips_code {
		private:
			vector<shared_ptr<insn>> _insn_list;
			vector<shared_ptr<data_directive>> _dd_list;
			shared_ptr<spectre::parser::parser> _ast_parser;
			int _label_number_counter;
			stack<shared_ptr<mips_frame>> _frame_stack;
			long _global_register_wrapper_counter;
			stack<shared_ptr<label_operand>> _loop_begin_stack, _loop_end_stack;
			stack<int> _initial_loop_offset_stack;
			int _misc_counter;
			vector<shared_ptr<label_operand>> _global_symbol_table;
		public:
			mips_code(shared_ptr<spectre::parser::parser> p);
			~mips_code();
			void report(error e);
			void report_internal(string msg, string fn, int l, string fl);
			vector<shared_ptr<insn>> insn_list();
			vector<string> raw_insn_list();
			shared_ptr<mips_frame> pop_frame();
			shared_ptr<mips_frame> current_frame();
			void add_frame(shared_ptr<mips_frame> mf);
			shared_ptr<mips_frame> generate_new_frame(bool mf);
			void add_insn(shared_ptr<insn> i);
			void add_insn_begin(shared_ptr<insn> i);
			tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> next_label_info();
			tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> generate_case_label_info(int c);
			shared_ptr<struct_symbol> find_struct_symbol(token n, int r);
			void push_loop_begin_label_operand(shared_ptr<label_operand> lo);
			void pop_loop_begin_label_operand();
			void push_loop_end_label_operand(shared_ptr<label_operand> lo);
			void pop_loop_end_label_operand();
			shared_ptr<label_operand> current_loop_begin_label_operand();
			shared_ptr<label_operand> current_loop_end_label_operand();
			void add_data_directive(shared_ptr<data_directive> dd, bool g);
			int current_misc_counter();
			int next_misc_counter();
			int current_initial_loop_offset();
			void push_initial_loop_offset(int i);
			void pop_initial_loop_offset();
			void add_to_global_symbol_table(shared_ptr<label_operand> lo);
		};

		string get_operand_text(shared_ptr<operand> o);
		shared_ptr<operand_wrapper> literal_token_2_operand_wrapper(shared_ptr<mips_code> mc, shared_ptr<primary_expression> pe);
		shared_ptr<register_wrapper> allocate_general_purpose_register(shared_ptr<mips_code> mc); // only call when the register is absolutely needed
		shared_ptr<register_wrapper> allocate_general_purpose_fp_register(shared_ptr<mips_code> mc);
		void free_general_purpose_register(shared_ptr<mips_code> mc, shared_ptr<register_wrapper> rw);
		shared_ptr<operand_wrapper> binary_expression_helper_1(shared_ptr<mips_code> mc, shared_ptr<operand_wrapper> lhs_op, binary_expression::operator_kind ok,
			shared_ptr<operand_wrapper> rhs_op); // handles addition, subtraction, multiplication, division, modulus, and bitwise operations
		void load_register_wrapper_from_stack(shared_ptr<mips_code> mc, shared_ptr<register_wrapper> rw);
		shared_ptr<operand_wrapper> binary_expression_helper_2(shared_ptr<mips_code> mc, shared_ptr<operand_wrapper> lhs_op, binary_expression::operator_kind ok,
			shared_ptr<operand_wrapper> rhs_op, bool u); // handles equality and comparisons
		shared_ptr<operand_wrapper> binary_expression_helper_3(shared_ptr<mips_code> mc, shared_ptr<binary_expression> lhs, binary_expression::operator_kind ok,
			shared_ptr<binary_expression> rhs); // handles logical and's and or's
		unsigned int calculate_type_size(shared_ptr<mips_code> mc, shared_ptr<type> t);

		enum class allocation_kind {
			KIND_STACK, KIND_STATIC, KIND_NONE
		};

		allocation_kind calculate_allocation_strategy(shared_ptr<mips_code> mc, shared_ptr<type> t, bool g);
		binary_expression::operator_kind convert_from_assignment_operator(shared_ptr<mips_code> mc, binary_expression::operator_kind op);
		shared_ptr<operand_wrapper> handle_primitive_assignment_operation(shared_ptr<mips_code> mc, shared_ptr<operand_wrapper> op1, binary_expression::operator_kind ok,
			shared_ptr<operand_wrapper> op2);
		bool is_function_main(shared_ptr<mips_code> mc, shared_ptr<function_symbol> fsym);

		string type_2_string(shared_ptr<mips_code> mc, shared_ptr<type> t, bool spaces);
		string scope_2_string_helper(shared_ptr<mips_code> mc, shared_ptr<scope> s, string r);
		string scope_2_string(shared_ptr<mips_code> mc, shared_ptr<scope> s);
		string symbol_2_string_helper(shared_ptr<mips_code> mc, shared_ptr<symbol> s, string r);
		string symbol_2_string(shared_ptr<mips_code> mc, shared_ptr<symbol> s);

		void generate_mips(shared_ptr<mips_code> mc, shared_ptr<spectre::parser::parser> p);
		void generate_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<stmt> s);
		shared_ptr<operand_wrapper> generate_expression_mips(shared_ptr<mips_code> mc, shared_ptr<expression> e, binary_expression::operator_kind assign_op,
			shared_ptr<operand_wrapper> assign_to, vector<unary_expression::kind> ukl, vector<shared_ptr<postfix_expression::postfix_type>> ptl);
		shared_ptr<operand_wrapper> generate_assignment_expression_mips(shared_ptr<mips_code> mc, shared_ptr<assignment_expression> ae,
			binary_expression::operator_kind assign_op, shared_ptr<operand_wrapper> assign_to, vector<unary_expression::kind> utl, vector<shared_ptr<postfix_expression::postfix_type>> ptl);
		shared_ptr<operand_wrapper> generate_ternary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<ternary_expression> te, 
			binary_expression::operator_kind assign_op, shared_ptr<operand_wrapper> assign_to, vector<unary_expression::kind> utl, vector<shared_ptr<postfix_expression::postfix_type>> ptl);
		shared_ptr<operand_wrapper> generate_binary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<binary_expression> be,
			binary_expression::operator_kind assign_op, shared_ptr<operand_wrapper> assign_to, vector<unary_expression::kind> utl, vector<shared_ptr<postfix_expression::postfix_type>> ptl);
		shared_ptr<operand_wrapper> generate_unary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<unary_expression> ue, 
			binary_expression::operator_kind assign_op, shared_ptr<operand_wrapper> assign_to, vector<unary_expression::kind> utl, vector<shared_ptr<postfix_expression::postfix_type>> ptl);
		shared_ptr<operand_wrapper> generate_postfix_expression_mips(shared_ptr<mips_code> mc, shared_ptr<postfix_expression> pe,
			binary_expression::operator_kind assign_op, shared_ptr<operand_wrapper> assign_to, vector<unary_expression::kind> utl, vector<shared_ptr<postfix_expression::postfix_type>> ptl);
		shared_ptr<operand_wrapper> generate_primary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<primary_expression> pe,
			binary_expression::operator_kind assign_op, shared_ptr<operand_wrapper> assign_to, vector<unary_expression::kind> utl, vector<shared_ptr<postfix_expression::postfix_type>> ptl);
		void generate_variable_declaration_mips(shared_ptr<mips_code> mc, shared_ptr<variable_declaration> vdecl);
		void generate_if_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<if_stmt> istmt);
		void generate_while_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<while_stmt> wstmt);
		void generate_block_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<block_stmt> bstmt);
		void generate_break_continue_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<break_continue_stmt> bcstmt);
		void generate_do_while_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<while_stmt> dwstmt);
		void generate_for_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<for_stmt> fstmt);
		void generate_switch_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<switch_stmt> sstmt);
		void generate_case_default_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<case_default_stmt> cdstmt);
		void generate_function_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<function_stmt> sstmt);
		void generate_return_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<return_stmt> rstmt);
		void generate_asm_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<asm_stmt> astmt);

		shared_ptr<register_wrapper> convert_operand_wrapper_2_fp_register_wrapper(shared_ptr<mips_code> mc, shared_ptr<operand_wrapper> src, bool dp);

		void generate_namespace_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<namespace_stmt> nstmt);
		vector<shared_ptr<stmt>> reorder_stmt_list(shared_ptr<mips_code> mc, vector<shared_ptr<stmt>> sl);
	}
}

#endif