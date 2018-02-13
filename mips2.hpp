#ifndef SPECTRE_MIPS2_HPP
#define SPECTRE_MIPS2_HPP

#include <vector>
#include <string>
#include <memory>
#include <stack>
#include <algorithm>
#include <variant>
#include "mips_instruction_set.hpp"
#include "parser.hpp"
#include "config.hpp"

using std::vector;
using std::string;
using std::shared_ptr;
using std::stack;
using std::pair;
using std::variant;

namespace spectre {
	namespace mips2 {

		const static string label_prefix = "$L";
		const static string program_termination_hook = "_std_lib_exit";
		const static string program_new_hook = "_std_lib_malloc";

		class operand {
		public:
			enum class kind {
				KIND_REGISTER, KIND_MEMORY, KIND_INT_IMMEDIATE, KIND_CHAR_IMMEDIATE, KIND_LABEL, KIND_NONE
			};
			enum class offset_kind {
				KIND_TOP, KIND_BOTTOM, KIND_MIDDLE, KIND_TRUE, KIND_NONE
			};
		private:
			int _register_number;
			string _register_name;
			int _offset, _int_immediate;
			string _char_immediate;
			string _label_name;
			offset_kind _operand_offset_kind;
			kind _operand_kind;
			bool _double_precision, _single_precision;
			bool _on_stack;
			int _middle_stack_offset;
		public:
			operand(int r, string rn);
			operand(offset_kind ok, int o, int r, string rn);
			operand(int i);
			operand(bool c, string s);
			int register_number();
			string register_name();
			int offset();
			void set_offset(int o);
			int int_immediate();
			string char_immediate();
			string label_name();
			kind operand_kind();
			offset_kind operand_offset_kind();
			void set_operand_offset_kind(offset_kind ok);
			bool double_precision();
			bool single_precision();
			void set_single_precision(bool sp);
			void set_double_precision(bool dp);
			string raw_operand();
			bool on_stack();
			void set_on_stack(bool b);
			int middle_stack_offset();
			void set_middle_stack_offset(int i);
			bool is_immediate();
			void set_int_immediate(int i);
		};

		class insn {
		public:
			enum class type_kind {
				KIND_0, KIND_1, KIND_2, KIND_3, KIND_LABEL, KIND_RAW, KIND_NONE
			};
			enum class kind {
				KIND_ADD, KIND_ADDI, KIND_ADDU, KIND_ADDIU,
				KIND_AND, KIND_ANDI,
				KIND_BEQ, KIND_BGEZ, KIND_BGEZAL, KIND_BGTZ, KIND_BLEZ, KIND_BLTZ, KIND_BLTZAL, KIND_BNE,
				KIND_DIV, KIND_DIVU,
				KIND_JAL, KIND_J, KIND_JR, KIND_JALR,
				KIND_LA,
				KIND_LB, KIND_LH, KIND_LW, KIND_LHU, KIND_LBU,
				KIND_LUI,
				KIND_MFLO, KIND_MFHI, KIND_MULT, KIND_MULTU,
#if SYSTEM == 2
				KIND_REM, KIND_REMU,
#endif
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
		private:
			string _raw_instruction;
			shared_ptr<operand> _operand_1, _operand_2, _operand_3;
			type_kind _insn_type_kind;
			kind _insn_kind;
		public:
			insn(string ri);
			insn(kind ik);
			insn(kind ik, shared_ptr<operand> o1);
			insn(kind ik, shared_ptr<operand> o1, shared_ptr<operand> o2);
			insn(kind ik, shared_ptr<operand> o1, shared_ptr<operand> o2, shared_ptr<operand> o3);
			insn(shared_ptr<operand> o1);
			shared_ptr<operand> operand_1();
			shared_ptr<operand> operand_2();
			shared_ptr<operand> operand_3();
			type_kind insn_type_kind();
			kind insn_kind();
			string raw_instruction();
			string raw_insn();
		};

		class directive {
		public:
			enum class kind {
				KIND_HEADER, KIND_GLOBL, KIND_ALIGN, KIND_SPACE, KIND_FLOAT, KIND_DOUBLE, KIND_STRING, KIND_WORD, KIND_HALF, KIND_BYTE, KIND_NONE
			};
		private:
			string _label_name, _immediate_string;
			double _immediate_double;
			float _immediate_float;
			int _allocated_space, _alignment_boundary;
			string _header_name;
			kind _directive_kind;
			bool _word, _half, _byte;
			vector<shared_ptr<operand>> _word_list, _half_list, _byte_list;
			vector<double> _double_list;
			vector<float> _float_list;
		public:
			directive(bool h, string hn);
			directive(string ln, string is);
			directive(string ln, double id);
			directive(string ln, float _if);
			directive(string ln, int as, bool w = false);
			directive(string ln);
			directive(int ab);
			directive(kind k, string ln, vector<shared_ptr<operand>> ol);
			directive(string ln, vector<double> vd);
			directive(string ln, vector<float> vf);
			string label_name();
			string immediate_string();
			double immediate_double();
			float immediate_float();
			int allocated_space();
			int alignment_boundary();
			string header_name();
			kind directive_kind();
			string raw_directive();
			bool word();
			bool half();
			bool byte();
			vector<shared_ptr<operand>> word_list();
			vector<shared_ptr<operand>> half_list();
			vector<shared_ptr<operand>> byte_list();
			vector<double> double_list();
			vector<float> float_list();
		};

		class mips_code;

		class mips_frame {
		private:
			int _frame_size;
			bool _frame_size_set;
			vector<shared_ptr<insn>> _body_insn_list, _prologue_insn_list, _epilogue_insn_list;
			map<int, bool> _register_usage_map;
			map<int, bool> _saved_register_initially_stored_map;
			bool _main_function, _global_frame;
			bool _double_precision_return_value, _single_precision_return_value;
			map<string, shared_ptr<operand>> _variable_storage_map;
			int _middle_section_size;
			int _top_section_size;
			stack<shared_ptr<operand>> _break_stack, _continue_stack;
			vector<shared_ptr<operand>> _operand_list;
			int _overflow_register_counter, _fp_overflow_register_counter;
			shared_ptr<operand> _return_label;
			shared_ptr<type> _return_type;
			bool _return_label_set, _return_type_set;
		public:
			mips_frame(bool mf, bool gf, bool dprv, bool sprv);
			~mips_frame();

			int frame_size();
			void set_frame_size(int f);
			bool frame_size_set();
			void set_frame_size_set(bool f);

			vector<shared_ptr<insn>> body_insn_list();
			void add_insn_to_body(shared_ptr<insn> i, bool b = false);
			vector<shared_ptr<insn>> prologue_insn_list();
			void add_insn_to_prologue(shared_ptr<insn> i, bool b = false);
			vector<shared_ptr<insn>> epilogue_insn_list();
			void add_insn_to_epilogue(shared_ptr<insn> i, bool b = false);
			void calculate_true_insn_offsets();

			bool is_register_in_use(int r);
			void mark_register(int r);
			void unmark_register(int r);
			int current_overflow_register();
			int current_fp_overflow_register();

			bool is_saved_register_initially_stored(int r);
			void mark_saved_register_initially_stored(int r);

			bool main_function();
			bool global_frame();
			bool double_precision_return_value();
			bool single_precision_return_value();

			map<string, shared_ptr<operand>> variable_storage_map();
			void add_variable(string var, shared_ptr<operand> o);
			shared_ptr<operand> get_variable_offset(string var);
			void update_middle_section_size(int b);
			int middle_section_size();
			void update_top_section_size(int b);
			int top_section_size();

			void pop_break_stack();
			void pop_continue_stack();
			shared_ptr<operand> current_break();
			shared_ptr<operand> current_continue();
			void push_break_stack(shared_ptr<operand> o);
			void push_continue_stack(shared_ptr<operand> o);

			void add_operand(shared_ptr<operand> o);
			shared_ptr<operand> find_last_operand(int r);
			shared_ptr<operand> find_first_operand(int r);
			void restore_saved_registers(shared_ptr<mips_code> mc);

			shared_ptr<operand> return_label();
			void set_return_label(shared_ptr<operand> l);
			bool return_label_set();
			void set_return_label_set(bool s);

			shared_ptr<type> return_type();
			void set_return_type(shared_ptr<type> t);
			bool return_type_set();
			void set_return_type_set(bool s);
		};

		class mips_code {
		private:
			vector<string> _global_variable_name_list;
			vector<shared_ptr<insn>> _insn_list;
			vector<shared_ptr<directive>> _data_directive_list, _globl_directive_list;
			shared_ptr<spectre::parser::parser> _ast_parser;
			stack<shared_ptr<mips_frame>> _frame_stack;
			int _label_counter, _misc_counter;
			bool _inside_function;
		public:
			mips_code(shared_ptr<spectre::parser::parser> p);
			~mips_code();
			vector<shared_ptr<insn>> insn_list();
			vector<string> raw_insn_list();
			vector<shared_ptr<directive>> data_directive_list();
			vector<shared_ptr<directive>> globl_directive_list();
			void add_insn(shared_ptr<insn> i);
			void add_insn_to_beginning(shared_ptr<insn> i);
			void add_data_directive_to_beginning(shared_ptr<directive> d);
			void add_data_directive(shared_ptr<directive> d);
			void add_globl_directive_to_beginning(shared_ptr<directive> d);
			void add_globl_directive(shared_ptr<directive> d);
			void add_insn_batch(vector<shared_ptr<insn>> il);
			shared_ptr<mips_frame> current_frame();
			void push_frame(shared_ptr<mips_frame> mf);
			void pop_frame();
			pair<int, shared_ptr<operand>> next_label();
			bool is_name_global_variable(string var);
			void add_global_variable(string var);
			void report_internal(string msg, string fn, int ln, string fl);
			int next_misc_counter();
			shared_ptr<struct_symbol> find_struct_symbol(token n, int r);
			void set_inside_function(bool b);
			bool inside_function();
		};

		unsigned int calculate_type_size(shared_ptr<mips_code> mc, shared_ptr<type> t);
		shared_ptr<operand> allocate_general_purpose_register(shared_ptr<mips_code> mc);
		shared_ptr<operand> allocate_general_purpose_fp_register(shared_ptr<mips_code> mc);
		void free_general_purpose_register(shared_ptr<mips_code> mc, shared_ptr<operand> o);
		void free_general_purpose_fp_register(shared_ptr<mips_code> mc, shared_ptr<operand> o);
		void load_general_purpose_register_from_frame(shared_ptr<mips_code> mc, shared_ptr<operand> o);
		void load_general_purpose_fp_register_from_frame(shared_ptr<mips_code> mc, shared_ptr<operand> o);
		bool is_function_main(shared_ptr<mips_code> mc, shared_ptr<function_symbol> fsym);
		tuple<vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>> reorder_stmt_list(shared_ptr<mips_code> mc, vector<shared_ptr<stmt>> sl);
		shared_ptr<operand> load_value_into_register(shared_ptr<mips_code> mc, shared_ptr<operand> op, shared_ptr<type> t);
		shared_ptr<operand> cast_between_types(shared_ptr<mips_code> mc, shared_ptr<operand> src, shared_ptr<type> t1, shared_ptr<type> t2);
		pair<shared_ptr<operand>, shared_ptr<operand>> deconflict_registers(shared_ptr<mips_code> mc, shared_ptr<operand> o1, shared_ptr<operand> o2);
		shared_ptr<operand> binary_expression_helper_1(shared_ptr<mips_code> mc, shared_ptr<operand> o1, binary_expression::operator_kind ok, shared_ptr<operand> o2, shared_ptr<type> t, bool u);
		insn::kind load_store_insn_from_type(shared_ptr<mips_code> mc, shared_ptr<type> t, bool l);

		string type_2_string(shared_ptr<mips_code> mc, shared_ptr<type> t, bool spaces);
		string scope_2_string_helper(shared_ptr<mips_code> mc, shared_ptr<scope> s, string r);
		string scope_2_string(shared_ptr<mips_code> mc, shared_ptr<scope> s);
		string symbol_2_string_helper(shared_ptr<mips_code> mc, shared_ptr<symbol> s, string r);
		string symbol_2_string(shared_ptr<mips_code> mc, shared_ptr<symbol> s);

		string c_scope_2_string_helper(shared_ptr<mips_code> mc, shared_ptr<scope> s, string r);
		string c_scope_2_string(shared_ptr<mips_code> mc, shared_ptr<scope> s);
		string c_symbol_2_string_helper(shared_ptr<mips_code> mc, shared_ptr<symbol> s, string r);
		string c_symbol_2_string(shared_ptr<mips_code> mc, shared_ptr<symbol> s);

		shared_ptr<operand> literal_token_2_operand(shared_ptr<mips_code> mc, shared_ptr<primary_expression> pe);

		void generate_mips(shared_ptr<mips_code> mc, shared_ptr<spectre::parser::parser> p);
		void generate_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<stmt> s);
		shared_ptr<operand> generate_expression_mips(shared_ptr<mips_code> mc, shared_ptr<expression> e, bool lvalue);
		shared_ptr<operand> generate_assignment_expression_mips(shared_ptr<mips_code> mc, shared_ptr<assignment_expression> ae, bool lvalue);
		shared_ptr<operand> generate_ternary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<ternary_expression> te, bool lvalue);
		shared_ptr<operand> generate_binary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<binary_expression> be, bool lvalue);
		shared_ptr<operand> generate_unary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<unary_expression> ue, bool lvalue);
		shared_ptr<operand> generate_postfix_expression_mips(shared_ptr<mips_code> mc, shared_ptr<postfix_expression> pe, bool lvalue);
		shared_ptr<operand> generate_primary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<primary_expression> pe, bool lvalue);
		void generate_variable_declaration_mips(shared_ptr<mips_code> mc, shared_ptr<variable_declaration> vd);
		void generate_function_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<function_stmt> fs);
		void generate_return_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<return_stmt> rs);
		void generate_if_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<if_stmt> is);
		void generate_while_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<while_stmt> ws);
		void generate_block_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<block_stmt> bs);
		void generate_break_continue_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<break_continue_stmt> bcs);
		void generate_switch_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<switch_stmt> ss);
		void generate_case_default_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<case_default_stmt> cds);
		void generate_do_while_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<while_stmt> dws);
		void generate_for_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<for_stmt> fs);
		void generate_asm_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<asm_stmt> as);
		void generate_access_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<access_stmt> as);

		variant<bool, int, unsigned int, float, double, string> evaluate_constant_expression(shared_ptr<mips_code> mc, shared_ptr<assignment_expression> ae);
		template<typename C, typename TL, typename TR> auto raw_arithmetic_binary_expression_evaluator(shared_ptr<mips_code> mc, TL lhs, TR rhs, binary_expression::operator_kind ok);
		template<typename C, typename TL, typename TR> bool raw_logical_binary_expression_evaluator(shared_ptr<mips_code> mc, TL lhs, TR rhs, binary_expression::operator_kind ok);

		tuple<vector<int>, vector<int>, int> save_to_middle(shared_ptr<mips_code> mc);
		void restore_from_middle(shared_ptr<mips_code> mc, vector<int> which_to_store, vector<int> middle_offsets);
	}
}

#endif
