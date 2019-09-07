#ifndef SPECTRE_ARM32_HPP
#define SPECTRE_ARM32_HPP

#include "basic_block.hpp"
#include "middle_ir.hpp"
#include "parser.hpp"
#include "opt_passes/pass_manager.hpp"

#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <iomanip>
#include <limits>
#include <unordered_set>
#include <array>
#include <tuple>
#include <variant>
#include <optional>

using namespace spectre::ir;
using namespace spectre::opt;
using std::vector;
using std::string;
using std::shared_ptr;
using std::numeric_limits;
using std::to_string;
using std::stringstream;
using std::setprecision;
using std::fixed;
using std::unordered_set;
using std::array;
using std::tuple;
using std::make_tuple;
using std::variant;
using std::holds_alternative;
using std::get;

namespace spectre {
	namespace arm32 {
		namespace registers {
			const static string r0 = "r0",
				r1 = "r1",
				r2 = "r2",
				r3 = "r3",
				r4 = "r4",
				r5 = "r5",
				r6 = "r6",
				r7 = "r7",
				r8 = "r8",
				r9 = "r9",
				r10 = "r10",
				r11 = "r11",
				r12 = "r12",
				r13 = "r13",
				r14 = "r14",
				r15 = "r15";

			const static string f0 = "s0", f1 = "s1",
			      f2 = "s2", f3 = "s3",
			      f4 = "s4", f5 = "s5",
			      f6 = "s6", f7 = "s7",
			      f8 = "s8", f9 = "s9",
			      f10 = "s10", f11 = "s11",
			      f12 = "s12", f13 = "s13",
			      f14 = "s14", f15 = "s15",
			      f16 = "s16", f17 = "s17",
			      f18 = "s18", f19 = "s19",
			      f20 = "s20", f21 = "s21",
			      f22 = "s22", f23 = "s23",
			      f24 = "s24", f25 = "s25",
			      f26 = "s26", f27 = "s27",
			      f28 = "s28", f29 = "s29",
			      f30 = "s30", f31 = "s31";

			const static string a0 = r0,
				a1 = r1,
				a2 = r2,
				a3 = r3,
				t0 = r4,
				t1 = r5,
				t2 = r6,
				t3 = r7,
				t4 = r8,
				t5 = r9,
				s0 = r10,
				fp = r11,
				s1 = r12,
				sp = r13,
				lr = r14,
				pc = r15;

			static constexpr int num_argument_registers = 4,
				num_general_purpose_registers = 6,
				num_scratch_registers = 2,
				num_registers = 16;

			static constexpr int num_fp_argument_registers = 8,
				  num_fp_general_purpose_registers = 6,
				  num_fp_scratch_registers = 2,
				  num_fp_registers = 16;

			static constexpr array<bool, num_fp_registers * 2> fp_register_volatility_map = {
				true,
				true,
				true,
				true,
				true,
				true,
				true,
				true,
				true,
				true,
				true,
				true,
				true,
				true,
				true,
				true,
				false,
				false,
				false,
				false,
				false,
				false,
				false,
				false,
				false,
				false,
				false,
				false,
				false,
				false,
				false,
				false
			};

			static constexpr array<bool, num_registers> register_volatility_map = {
				true,
				true,
				true,
				true,
				false,
				false,
				false,
				false,
				false,
				false,
				false,
				false,
				true,
				false,
				false,
				false
			};
			
			static const array<string, num_argument_registers> argument_registers = { a0, a1, a2, a3 };
			static const array<string, num_general_purpose_registers> general_purpose_registers = { t0, t1, t2, t3, t4, t5 };
			static const array<string, num_scratch_registers> scratch_registers = { s0, s1 };

			static const array<string, num_fp_argument_registers> fp_argument_registers = { f0, f2, f4, f6, f8, f10, f12, f14 };
			static const array<string, num_fp_general_purpose_registers> fp_general_purpose_registers = { f16, f18, f20, f22, f24, f26 };
			static const array<string, num_fp_scratch_registers> fp_scratch_registers = { f28, f30 };

			static const unordered_map<int, string> fp_register_number_2_name = {
				{ 0, f0 },
				{ 1, f1 },
				{ 2, f2 },
				{ 3, f3 },
				{ 4, f4 },
				{ 5, f5 },
				{ 6, f6 },
				{ 7, f7 },
				{ 8, f8 },
				{ 9, f9 },
				{ 10, f10 },
				{ 11, f11 },
				{ 12, f12 },
				{ 13, f13 },
				{ 14, f14 },
				{ 15, f15 },
				{ 16, f16 },
				{ 17, f17 },
				{ 18, f18 },
				{ 19, f19 },
				{ 20, f20 },
				{ 21, f21 },
				{ 22, f22 },
				{ 23, f23 },
				{ 24, f24 },
				{ 25, f25 },
				{ 26, f26 },
				{ 27, f27 },
				{ 28, f28 },
				{ 29, f29 },
				{ 30, f30 },
				{ 31, f31 }
			};
			static const unordered_map<string, int> fp_register_name_2_number = {
				{ f0, 0 },
				{ f1, 1 },
				{ f2, 2 },
				{ f3, 3 },
				{ f4, 4 },
				{ f5, 5 },
				{ f6, 6 },
				{ f7, 7 },
				{ f8, 8 },
				{ f9, 9 },
				{ f10, 10 },
				{ f11, 11 },
				{ f12, 12 },
				{ f13, 13 },
				{ f14, 14 },
				{ f15, 15 },
				{ f16, 16 },
				{ f17, 17 },
				{ f18, 18 },
				{ f19, 19 },
				{ f20, 20 },
				{ f21, 21 },
				{ f22, 22 },
				{ f23, 23 },
				{ f24, 24 },
				{ f25, 25 },
				{ f26, 26 },
				{ f27, 27 },
				{ f28, 28 },
				{ f29, 29 },
				{ f30, 30 },
				{ f31, 31 }
			};

			static const unordered_map<int, string> register_number_2_name = {
				{ 0, r0 },
				{ 1, r1 },
				{ 2, r2 },
				{ 3, r3 },
				{ 4, r4 },
				{ 5, r5 },
				{ 6, r6 },
				{ 7, r7 },
				{ 8, r8 },
				{ 9, r9 },
				{ 10, r10 },
				{ 11, r11 },
				{ 12, r12 },
				{ 13, r13 },
				{ 14, r14 },
				{ 15, r15 }
			};
			static const unordered_map<string, int> register_name_2_number = {
				{ r0, 0 },
				{ r1, 1 },
				{ r2, 2 },
				{ r3, 3 },
				{ r4, 4 },
				{ r5, 5 },
				{ r6, 6 },
				{ r7, 7 },
				{ r8, 8 },
				{ r9, 9 },
				{ r10, 10 },
				{ r11, 11 },
				{ r12, 12 },
				{ r13, 13 },
				{ r14, 14 },
				{ r15, 15 }
			};
		}

		const static string TAB = "\t";
		const static string GLOBAL_SCRATCH_PREFIX = "__resv_scratch_";
		const static string FUNC_SCRATCH_PREFIX = "__resv_func_scratch_";
		const static string START_SYMBOL = "_start";
		const static string MAIN_SYMBOL = "_main";
		const static string ASM_BRANCH_PREFIX = ".B";
		static constexpr int STACK_ALIGNMENT = 8;
		static constexpr int EXIT_SYSCALL_NUMBER = 1;
		static constexpr int LITERAL_POOL_RANGE = (1 << 12) / (1 << 2),
			LITERAL_POOL_INSERTION_BEFORE = 20;
		const static string EXIT_HOOK = "_std_hooks__sp_exit";

		template<class T> T retrieve_immediate_operand(shared_ptr<basic_blocks> bbs, shared_ptr<immediate_operand> i) {
			T ret;
			switch (i->immediate_operand_kind()) {
			case immediate_operand::kind::KIND_CHAR:
				ret = i->immediate_char();
				break;
			case immediate_operand::kind::KIND_DOUBLE:
				ret = i->immediate_double();
				break;
			case immediate_operand::kind::KIND_FLOAT:
				ret = i->immediate_float();
				break;
			case immediate_operand::kind::KIND_INT:
				ret = i->immediate_int();
				break;
			case immediate_operand::kind::KIND_LONG:
				ret = i->immediate_long();
				break;
			case immediate_operand::kind::KIND_SHORT:
				ret = i->immediate_short();
				break;
			default:
				bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				break;
			}
			return ret;
		}

		template<class T> string fp_max_prec_to_string(T v) {
			stringstream ss;
			ss << fixed << setprecision(numeric_limits<T>::max_digits10) << v;
			return ss.str();
		}
		
		enum class immediate_operand_cast_kind {
			KIND_INTEGRAL, KIND_DOUBLE, KIND_FLOAT
		};

		template<class T> string cast_immediate_2_string(shared_ptr<basic_blocks> bbs, T t, immediate_operand_cast_kind iock) {
			switch (iock) {
			case immediate_operand_cast_kind::KIND_DOUBLE: {
				stringstream ss;
				ss << fixed << setprecision(numeric_limits<double>::max_digits10) << t;
				return ss.str();
			}
			case immediate_operand_cast_kind::KIND_FLOAT: {
				stringstream ss;
				ss << fixed << setprecision(numeric_limits<float>::max_digits10) << t;
				return ss.str();
			}
			case immediate_operand_cast_kind::KIND_INTEGRAL:
				return to_string(t);
			}
			bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return "";
		}

		struct stk_alloc_data {
			stk_alloc_data(shared_ptr<register_operand> r, int s, int a);
			~stk_alloc_data();

			shared_ptr<register_operand> reg;
			int size;
			int alignment;
		};

		int int_log2(shared_ptr<basic_blocks> bbs, int num);
		bool is_pow2(int num);

		struct arm_offset {
			enum class kind {
				KIND_TOP, KIND_FP
			};

			kind offset_kind;
			int immediate;
		};

		using arm_insn = vector<variant<string, arm_offset>>;
		string arm_insn_2_string(const arm_insn& ai, int prologue_size);

		struct arm_asm {
			vector<string> file_header;
			vector<string> file_data;
			vector<string> file_bss;
			vector<string> file_body;
		};

		void generate_start_hook(shared_ptr<arm_asm> aa, main_function_kind mfk);
		void handle_functions_asm(shared_ptr<basic_blocks> bbs, shared_ptr<arm_asm> aa,
			const unordered_set<int>& function_headers, const unordered_map<int, int>& coloring,
			unordered_map<int, string> g_var_2_sym_name_map, unordered_map<int, string> e_var_2_sym_name_map,
			pass_manager::debug_level dl);
		unordered_set<int> compute_reachable_blocks(shared_ptr<basic_blocks> bbs, int start);
		vector<string> generate_asm(shared_ptr<basic_blocks> bbs, shared_ptr<spectre::parser::parser> p, pass_manager::debug_level dl);
	}
}

#endif

