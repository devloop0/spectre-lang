#include "codegen/mips/mips2.hpp"
#include "codegen/mips/mips_instruction_set.hpp"
#include "parser/parser.hpp"
#include <memory>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <limits>
#include <sstream>
#include <utility>
#include <iomanip>
#include <cstdlib>

using std::make_shared;
using std::make_pair;
using std::to_string;
using std::find;
using std::atoi;
using std::make_tuple;
using std::stringstream;
using std::setprecision;
using std::static_pointer_cast;
using std::remove;
using std::stoul;
using std::get;
using std::log2;
using std::holds_alternative;
using std::visit;
using std::stol;
using std::reverse;
using std::max;
using namespace spectre::parser;

namespace spectre {
	namespace mips2 {

		namespace insns1 = spectre::mips::insns;
		namespace register_file1 = spectre::mips::register_file;

		const static map<insn::kind, string> insn_kind_2_string = {
			{ insn::kind::KIND_ADD, insns1::_add }, { insn::kind::KIND_ADDI, insns1::_addi }, { insn::kind::KIND_ADDIU, insns1::_addiu }, { insn::kind::KIND_ADDU, insns1::_addu },
			{ insn::kind::KIND_AND, insns1::_and }, { insn::kind::KIND_ANDI, insns1::_andi },
			{ insn::kind::KIND_BEQ, insns1::_beq }, { insn::kind::KIND_BGEZ, insns1::_bgez }, { insn::kind::KIND_BGEZAL, insns1::_bgezal }, { insn::kind::KIND_BGTZ, insns1::_bgtz },
				{ insn::kind::KIND_BLEZ, insns1::_blez }, { insn::kind::KIND_BLTZ, insns1::_bltz }, { insn::kind::KIND_BLTZAL, insns1::_bltzal }, { insn::kind::KIND_BNE, insns1::_bne },
			{ insn::kind::KIND_DIV, insns1::_div }, { insn::kind::KIND_DIVU, insns1::_divu },
			{ insn::kind::KIND_JAL, insns1::_jal }, { insn::kind::KIND_J, insns1::_j }, { insn::kind::KIND_JR, insns1::_jr }, { insn::kind::KIND_JALR, insns1::_jalr },
			{ insn::kind::KIND_LA, insns1::_la },
			{ insn::kind::KIND_LB, insns1::_lb }, { insn::kind::KIND_LH, insns1::_lh }, { insn::kind::KIND_LW, insns1::_lw },
			{ insn::kind::KIND_LHU, insns1::_lhu }, { insn::kind::KIND_LBU, insns1::_lbu },
			{ insn::kind::KIND_LUI, insns1::_lui },
			{ insn::kind::KIND_MFLO, insns1::_mflo }, { insn::kind::KIND_MFHI, insns1::_mfhi }, { insn::kind::KIND_MULT, insns1::_mult }, { insn::kind::KIND_MULTU, insns1::_multu },
#if SYSTEM == 2
			{ insn::kind::KIND_REM, insns1::_rem }, { insn::kind::KIND_REMU, insns1::_remu },
#endif
			{ insn::kind::KIND_NOOP, insns1::_noop },
			{ insn::kind::KIND_NOR, insns1::_nor }, { insn::kind::KIND_OR, insns1::_or }, { insn::kind::KIND_ORI, insns1::_ori },
			{ insn::kind::KIND_SB, insns1::_sb }, { insn::kind::KIND_SH, insns1::_sh }, { insn::kind::KIND_SW, insns1::_sw },
			{ insn::kind::KIND_SLL, insns1::_sll }, { insn::kind::KIND_SLLV, insns1::_sllv },
			{ insn::kind::KIND_SLT, insns1::_slt }, { insn::kind::KIND_SLTI, insns1::_slti }, { insn::kind::KIND_SLTIU, insns1::_sltiu }, { insn::kind::KIND_SLTU, insns1::_sltu },
			{ insn::kind::KIND_SGT, insns1::_sgt }, { insn::kind::KIND_SGTU, insns1::_sgtu }, { insn::kind::KIND_SLE, insns1::_sle }, { insn::kind::KIND_SLEU, insns1::_sleu },
			{ insn::kind::KIND_SGE, insns1::_sge }, { insn::kind::KIND_SGEU, insns1::_sgeu }, { insn::kind::KIND_SNE, insns1::_sne }, { insn::kind::KIND_SEQ, insns1::_seq },
			{ insn::kind::KIND_SRA, insns1::_sra }, { insn::kind::KIND_SRAV, insns1::_srav },
			{ insn::kind::KIND_SRL, insns1::_srl }, { insn::kind::KIND_SRLV, insns1::_srlv },
			{ insn::kind::KIND_SUB, insns1::_sub }, { insn::kind::KIND_SUBU, insns1::_subu },
			{ insn::kind::KIND_SYSCALL, insns1::_syscall },
			{ insn::kind::KIND_XOR, insns1::_xor }, { insn::kind::KIND_XORI, insns1::_xori },
			{ insn::kind::KIND_ADD_S, insns1::_add_s }, { insn::kind::KIND_ADD_D, insns1::_add_d },
			{ insn::kind::KIND_SUB_S, insns1::_sub_s }, { insn::kind::KIND_SUB_D, insns1::_sub_d },
			{ insn::kind::KIND_MUL_S, insns1::_mul_s }, { insn::kind::KIND_MUL_D, insns1::_mul_d },
			{ insn::kind::KIND_DIV_S, insns1::_div_s }, { insn::kind::KIND_DIV_D, insns1::_div_d },
			{ insn::kind::KIND_LWC1, insns1::_lwc1 }, { insn::kind::KIND_LDC1, insns1::_ldc1 },
			{ insn::kind::KIND_SWC1, insns1::_swc1 }, { insn::kind::KIND_SDC1, insns1::_sdc1 },
			{ insn::kind::KIND_L_S, insns1::_l_s }, { insn::kind::KIND_L_D, insns1::_l_d },
			{ insn::kind::KIND_BC1T, insns1::_bc1t }, { insn::kind::KIND_BC1F, insns1::_bc1f },
			{ insn::kind::KIND_CVT_D_S, insns1::_cvt_d_s }, { insn::kind::KIND_CVT_D_W, insns1::_cvt_d_w },
			{ insn::kind::KIND_CVT_S_D, insns1::_cvt_s_d }, { insn::kind::KIND_CVT_S_W, insns1::_cvt_s_w },
			{ insn::kind::KIND_CVT_W_D, insns1::_cvt_w_d }, { insn::kind::KIND_CVT_W_S, insns1::_cvt_w_s },
			{ insn::kind::KIND_C_EQ_D, insns1::_c_eq_d }, { insn::kind::KIND_C_EQ_S, insns1::_c_eq_s },
			{ insn::kind::KIND_C_GE_D, insns1::_c_ge_d }, { insn::kind::KIND_C_GE_S, insns1::_c_ge_s },
			{ insn::kind::KIND_C_GT_D, insns1::_c_gt_d }, { insn::kind::KIND_C_GT_S, insns1::_c_gt_s },
			{ insn::kind::KIND_C_LE_D, insns1::_c_le_d }, { insn::kind::KIND_C_LE_S, insns1::_c_le_s },
			{ insn::kind::KIND_C_LT_D, insns1::_c_lt_d }, { insn::kind::KIND_C_LT_S, insns1::_c_lt_s },
			{ insn::kind::KIND_C_NE_D, insns1::_c_ne_d }, { insn::kind::KIND_C_NE_S, insns1::_c_ne_s },
			{ insn::kind::KIND_MFC1, insns1::_mfc1 }, { insn::kind::KIND_MTC1, insns1::_mtc1 },
			{ insn::kind::KIND_MOV_D, insns1::_mov_d }, { insn::kind::KIND_MOV_S, insns1::_mov_s },
			{ insn::kind::KIND_NEG_D, insns1::_neg_d }, { insn::kind::KIND_NEG_S, insns1::_neg_s }
		};

		namespace register_file2 {

#if SYSTEM == 0 || SYSTEM == 1
			const static shared_ptr<operand> _zero_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_0), register_file1::_0);
			const static shared_ptr<operand> _at_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_1), register_file1::_1);
			const static shared_ptr<operand> _v0_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_2), register_file1::_2);
			const static shared_ptr<operand> _v1_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_3), register_file1::_3);
			const static shared_ptr<operand> _a0_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_4), register_file1::_4);
			const static shared_ptr<operand> _a1_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_5), register_file1::_5);
			const static shared_ptr<operand> _a2_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_6), register_file1::_6);
			const static shared_ptr<operand> _a3_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_7), register_file1::_7);
			const static shared_ptr<operand> _t0_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_8), register_file1::_8);
			const static shared_ptr<operand> _t1_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_9), register_file1::_9);
			const static shared_ptr<operand> _t2_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_10), register_file1::_10);
			const static shared_ptr<operand> _t3_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_11), register_file1::_11);
			const static shared_ptr<operand> _t4_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_12), register_file1::_12);
			const static shared_ptr<operand> _t5_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_13), register_file1::_13);
			const static shared_ptr<operand> _t6_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_14), register_file1::_14);
			const static shared_ptr<operand> _t7_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_15), register_file1::_15);
			const static shared_ptr<operand> _s0_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_16), register_file1::_16);
			const static shared_ptr<operand> _s1_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_17), register_file1::_17);
			const static shared_ptr<operand> _s2_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_18), register_file1::_18);
			const static shared_ptr<operand> _s3_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_19), register_file1::_19);
			const static shared_ptr<operand> _s4_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_20), register_file1::_20);
			const static shared_ptr<operand> _s5_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_21), register_file1::_21);
			const static shared_ptr<operand> _s6_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_22), register_file1::_22);
			const static shared_ptr<operand> _s7_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_23), register_file1::_23);
			const static shared_ptr<operand> _t8_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_24), register_file1::_24);
			const static shared_ptr<operand> _t9_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_25), register_file1::_25);
			const static shared_ptr<operand> _k0_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_26), register_file1::_26);
			const static shared_ptr<operand> _k1_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_27), register_file1::_27);
			const static shared_ptr<operand> _gp_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_28), register_file1::_28);
			const static shared_ptr<operand> _sp_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_29), register_file1::_29);
			const static shared_ptr<operand> _fp_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_30), register_file1::_30);
			const static shared_ptr<operand> _ra_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_31), register_file1::_31);
#elif SYSTEM == 2
			const static shared_ptr<operand> _zero_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_0), register_file1::_0);
			const static shared_ptr<operand> _ra_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_1), register_file1::_1);
			const static shared_ptr<operand> _sp_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_2), register_file1::_2);
			const static shared_ptr<operand> _gp_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_3), register_file1::_3);
			const static shared_ptr<operand> _tp_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_4), register_file1::_4);
			const static shared_ptr<operand> _t0_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_5), register_file1::_5);
			const static shared_ptr<operand> _t1_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_6), register_file1::_6);
			const static shared_ptr<operand> _t2_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_7), register_file1::_7);
			const static shared_ptr<operand> _s0_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_8), register_file1::_8), _fp_register = _s0_register;
			const static shared_ptr<operand> _s1_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_9), register_file1::_9);
			const static shared_ptr<operand> _a0_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_10), register_file1::_10);
			const static shared_ptr<operand> _a1_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_11), register_file1::_11);
			const static shared_ptr<operand> _a2_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_12), register_file1::_12);
			const static shared_ptr<operand> _a3_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_13), register_file1::_13);
			const static shared_ptr<operand> _a4_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_14), register_file1::_14);
			const static shared_ptr<operand> _a5_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_15), register_file1::_15);
			const static shared_ptr<operand> _a6_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_16), register_file1::_16);
			const static shared_ptr<operand> _a7_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_17), register_file1::_17);
			const static shared_ptr<operand> _s2_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_18), register_file1::_18);
			const static shared_ptr<operand> _s3_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_19), register_file1::_19);
			const static shared_ptr<operand> _s4_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_20), register_file1::_20);
			const static shared_ptr<operand> _s5_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_21), register_file1::_21);
			const static shared_ptr<operand> _s6_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_22), register_file1::_22);
			const static shared_ptr<operand> _s7_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_23), register_file1::_23);
			const static shared_ptr<operand> _s8_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_24), register_file1::_24);
			const static shared_ptr<operand> _s9_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_25), register_file1::_25);
			const static shared_ptr<operand> _s10_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_26), register_file1::_26);
			const static shared_ptr<operand> _s11_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_27), register_file1::_27);
			const static shared_ptr<operand> _t3_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_28), register_file1::_28);
			const static shared_ptr<operand> _t4_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_29), register_file1::_29);
			const static shared_ptr<operand> _t5_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_30), register_file1::_30);
			const static shared_ptr<operand> _t6_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_31), register_file1::_31);
#endif

			const static shared_ptr<operand> _f0_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f0), register_file1::_f0);
			const static shared_ptr<operand> _f1_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f1), register_file1::_f1);
			const static shared_ptr<operand> _f2_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f2), register_file1::_f2);
			const static shared_ptr<operand> _f3_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f3), register_file1::_f3);
			const static shared_ptr<operand> _f4_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f4), register_file1::_f4);
			const static shared_ptr<operand> _f5_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f5), register_file1::_f5);
			const static shared_ptr<operand> _f6_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f6), register_file1::_f6);
			const static shared_ptr<operand> _f7_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f7), register_file1::_f7);
			const static shared_ptr<operand> _f8_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f8), register_file1::_f8);
			const static shared_ptr<operand> _f9_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f9), register_file1::_f9);
			const static shared_ptr<operand> _f10_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f10), register_file1::_f10);
			const static shared_ptr<operand> _f11_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f11), register_file1::_f11);
			const static shared_ptr<operand> _f12_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f12), register_file1::_f12);
			const static shared_ptr<operand> _f13_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f13), register_file1::_f13);
			const static shared_ptr<operand> _f14_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f14), register_file1::_f14);
			const static shared_ptr<operand> _f15_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f15), register_file1::_f15);
			const static shared_ptr<operand> _f16_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f16), register_file1::_f16);
			const static shared_ptr<operand> _f17_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f17), register_file1::_f17);
			const static shared_ptr<operand> _f18_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f18), register_file1::_f18);
			const static shared_ptr<operand> _f19_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f19), register_file1::_f19);
			const static shared_ptr<operand> _f20_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f20), register_file1::_f20);
			const static shared_ptr<operand> _f21_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f21), register_file1::_f21);
			const static shared_ptr<operand> _f22_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f22), register_file1::_f22);
			const static shared_ptr<operand> _f23_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f23), register_file1::_f23);
			const static shared_ptr<operand> _f24_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f24), register_file1::_f24);
			const static shared_ptr<operand> _f25_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f25), register_file1::_f25);
			const static shared_ptr<operand> _f26_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f26), register_file1::_f26);
			const static shared_ptr<operand> _f27_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f27), register_file1::_f27);
			const static shared_ptr<operand> _f28_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f28), register_file1::_f28);
			const static shared_ptr<operand> _f29_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f29), register_file1::_f29);
			const static shared_ptr<operand> _f30_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f30), register_file1::_f30);
			const static shared_ptr<operand> _f31_register = make_shared<operand>(register_file1::register_2_int.at(register_file1::_f31), register_file1::_f31);

			map<int, shared_ptr<operand>> int_2_register_object = {
#if SYSTEM == 0 || SYSTEM == 1
				{ register_file1::register_2_int.at(register_file1::_0), _zero_register },
				{ register_file1::register_2_int.at(register_file1::_1), _at_register },
				{ register_file1::register_2_int.at(register_file1::_2), _v0_register },
				{ register_file1::register_2_int.at(register_file1::_3), _v1_register },
				{ register_file1::register_2_int.at(register_file1::_4), _a0_register },
				{ register_file1::register_2_int.at(register_file1::_5), _a1_register },
				{ register_file1::register_2_int.at(register_file1::_6), _a2_register },
				{ register_file1::register_2_int.at(register_file1::_7), _a3_register },
				{ register_file1::register_2_int.at(register_file1::_8), _t0_register },
				{ register_file1::register_2_int.at(register_file1::_9), _t1_register },
				{ register_file1::register_2_int.at(register_file1::_10), _t2_register },
				{ register_file1::register_2_int.at(register_file1::_11), _t3_register },
				{ register_file1::register_2_int.at(register_file1::_12), _t4_register },
				{ register_file1::register_2_int.at(register_file1::_13), _t5_register },
				{ register_file1::register_2_int.at(register_file1::_14), _t6_register },
				{ register_file1::register_2_int.at(register_file1::_15), _t7_register },
				{ register_file1::register_2_int.at(register_file1::_16), _s0_register },
				{ register_file1::register_2_int.at(register_file1::_17), _s1_register },
				{ register_file1::register_2_int.at(register_file1::_18), _s2_register },
				{ register_file1::register_2_int.at(register_file1::_19), _s3_register },
				{ register_file1::register_2_int.at(register_file1::_20), _s4_register },
				{ register_file1::register_2_int.at(register_file1::_21), _s5_register },
				{ register_file1::register_2_int.at(register_file1::_22), _s6_register },
				{ register_file1::register_2_int.at(register_file1::_23), _s7_register },
				{ register_file1::register_2_int.at(register_file1::_24), _t8_register },
				{ register_file1::register_2_int.at(register_file1::_25), _t9_register },
				{ register_file1::register_2_int.at(register_file1::_26), _k0_register },
				{ register_file1::register_2_int.at(register_file1::_27), _k1_register },
				{ register_file1::register_2_int.at(register_file1::_28), _gp_register },
				{ register_file1::register_2_int.at(register_file1::_29), _sp_register },
				{ register_file1::register_2_int.at(register_file1::_30), _fp_register },
				{ register_file1::register_2_int.at(register_file1::_31), _ra_register },
#elif SYSTEM == 2
				{ register_file1::register_2_int.at(register_file1::_0), _zero_register },
				{ register_file1::register_2_int.at(register_file1::_1), _ra_register },
				{ register_file1::register_2_int.at(register_file1::_2), _sp_register },
				{ register_file1::register_2_int.at(register_file1::_3), _gp_register },
				{ register_file1::register_2_int.at(register_file1::_4), _tp_register },
				{ register_file1::register_2_int.at(register_file1::_5), _t0_register },
				{ register_file1::register_2_int.at(register_file1::_6), _t1_register },
				{ register_file1::register_2_int.at(register_file1::_7), _t2_register },
				{ register_file1::register_2_int.at(register_file1::_8), _s0_register },
				{ register_file1::register_2_int.at(register_file1::_9), _s1_register },
				{ register_file1::register_2_int.at(register_file1::_10), _a0_register },
				{ register_file1::register_2_int.at(register_file1::_11), _a1_register },
				{ register_file1::register_2_int.at(register_file1::_12), _a2_register },
				{ register_file1::register_2_int.at(register_file1::_13), _a3_register },
				{ register_file1::register_2_int.at(register_file1::_14), _a4_register },
				{ register_file1::register_2_int.at(register_file1::_15), _a5_register },
				{ register_file1::register_2_int.at(register_file1::_16), _a6_register },
				{ register_file1::register_2_int.at(register_file1::_17), _a7_register },
				{ register_file1::register_2_int.at(register_file1::_18), _s2_register },
				{ register_file1::register_2_int.at(register_file1::_19), _s3_register },
				{ register_file1::register_2_int.at(register_file1::_20), _s4_register },
				{ register_file1::register_2_int.at(register_file1::_21), _s5_register },
				{ register_file1::register_2_int.at(register_file1::_22), _s6_register },
				{ register_file1::register_2_int.at(register_file1::_23), _s7_register },
				{ register_file1::register_2_int.at(register_file1::_24), _s8_register },
				{ register_file1::register_2_int.at(register_file1::_25), _s9_register },
				{ register_file1::register_2_int.at(register_file1::_26), _s10_register },
				{ register_file1::register_2_int.at(register_file1::_27), _s11_register },
				{ register_file1::register_2_int.at(register_file1::_28), _t3_register },
				{ register_file1::register_2_int.at(register_file1::_29), _t4_register },
				{ register_file1::register_2_int.at(register_file1::_30), _t5_register },
				{ register_file1::register_2_int.at(register_file1::_31), _t6_register },
#endif
				{ register_file1::register_2_int.at(register_file1::_f0), _f0_register },
				{ register_file1::register_2_int.at(register_file1::_f1), _f1_register },
				{ register_file1::register_2_int.at(register_file1::_f2), _f2_register },
				{ register_file1::register_2_int.at(register_file1::_f3), _f3_register },
				{ register_file1::register_2_int.at(register_file1::_f4), _f4_register },
				{ register_file1::register_2_int.at(register_file1::_f5), _f5_register },
				{ register_file1::register_2_int.at(register_file1::_f6), _f6_register },
				{ register_file1::register_2_int.at(register_file1::_f7), _f7_register },
				{ register_file1::register_2_int.at(register_file1::_f8), _f8_register },
				{ register_file1::register_2_int.at(register_file1::_f9), _f9_register },
				{ register_file1::register_2_int.at(register_file1::_f10), _f10_register },
				{ register_file1::register_2_int.at(register_file1::_f11), _f11_register },
				{ register_file1::register_2_int.at(register_file1::_f12), _f12_register },
				{ register_file1::register_2_int.at(register_file1::_f13), _f13_register },
				{ register_file1::register_2_int.at(register_file1::_f14), _f14_register },
				{ register_file1::register_2_int.at(register_file1::_f15), _f15_register },
				{ register_file1::register_2_int.at(register_file1::_f16), _f16_register },
				{ register_file1::register_2_int.at(register_file1::_f17), _f17_register },
				{ register_file1::register_2_int.at(register_file1::_f18), _f18_register },
				{ register_file1::register_2_int.at(register_file1::_f19), _f19_register },
				{ register_file1::register_2_int.at(register_file1::_f20), _f20_register },
				{ register_file1::register_2_int.at(register_file1::_f21), _f21_register },
				{ register_file1::register_2_int.at(register_file1::_f22), _f22_register },
				{ register_file1::register_2_int.at(register_file1::_f23), _f23_register },
				{ register_file1::register_2_int.at(register_file1::_f24), _f24_register },
				{ register_file1::register_2_int.at(register_file1::_f25), _f25_register },
				{ register_file1::register_2_int.at(register_file1::_f26), _f26_register },
				{ register_file1::register_2_int.at(register_file1::_f27), _f27_register },
				{ register_file1::register_2_int.at(register_file1::_f28), _f28_register },
				{ register_file1::register_2_int.at(register_file1::_f29), _f29_register },
				{ register_file1::register_2_int.at(register_file1::_f30), _f30_register },
				{ register_file1::register_2_int.at(register_file1::_f31), _f31_register }
			};

			map<string, shared_ptr<operand>> register_2_register_object = {
#if SYSTEM == 0 || SYSTEM == 1
				{ register_file1::_0, _zero_register },
				{ register_file1::_1, _at_register },
				{ register_file1::_2, _v0_register },
				{ register_file1::_3, _v1_register },
				{ register_file1::_4, _a0_register },
				{ register_file1::_5, _a1_register },
				{ register_file1::_6, _a2_register },
				{ register_file1::_7, _a3_register },
				{ register_file1::_8, _t0_register },
				{ register_file1::_9, _t1_register },
				{ register_file1::_10, _t2_register },
				{ register_file1::_11, _t3_register },
				{ register_file1::_12, _t4_register },
				{ register_file1::_13, _t5_register },
				{ register_file1::_14, _t6_register },
				{ register_file1::_15, _t7_register },
				{ register_file1::_16, _s0_register },
				{ register_file1::_17, _s1_register },
				{ register_file1::_18, _s2_register },
				{ register_file1::_19, _s3_register },
				{ register_file1::_20, _s4_register },
				{ register_file1::_21, _s5_register },
				{ register_file1::_22, _s6_register },
				{ register_file1::_23, _s7_register },
				{ register_file1::_24, _t8_register },
				{ register_file1::_25, _t9_register },
				{ register_file1::_26, _k0_register },
				{ register_file1::_27, _k1_register },
				{ register_file1::_28, _gp_register },
				{ register_file1::_29, _sp_register },
				{ register_file1::_30, _fp_register },
				{ register_file1::_31, _ra_register },
#elif SYSTEM == 2
				{ register_file1::_0, _zero_register },
				{ register_file1::_1, _ra_register },
				{ register_file1::_2, _sp_register },
				{ register_file1::_3, _gp_register },
				{ register_file1::_4, _tp_register },
				{ register_file1::_5, _t0_register },
				{ register_file1::_6, _t1_register },
				{ register_file1::_7, _t2_register },
				{ register_file1::_8, _s0_register },
				{ register_file1::_9, _s1_register },
				{ register_file1::_10, _a0_register },
				{ register_file1::_11, _a1_register },
				{ register_file1::_12, _a2_register },
				{ register_file1::_13, _a3_register },
				{ register_file1::_14, _a4_register },
				{ register_file1::_15, _a5_register },
				{ register_file1::_16, _a6_register },
				{ register_file1::_17, _a7_register },
				{ register_file1::_18, _s2_register },
				{ register_file1::_19, _s3_register },
				{ register_file1::_20, _s4_register },
				{ register_file1::_21, _s5_register },
				{ register_file1::_22, _s6_register },
				{ register_file1::_23, _s7_register },
				{ register_file1::_24, _s8_register },
				{ register_file1::_25, _s9_register },
				{ register_file1::_26, _s10_register },
				{ register_file1::_27, _s11_register },
				{ register_file1::_28, _t3_register },
				{ register_file1::_29, _t4_register },
				{ register_file1::_30, _t5_register },
				{ register_file1::_31, _t6_register },
#endif
				{ register_file1::_f0, _f0_register },
				{ register_file1::_f1, _f1_register },
				{ register_file1::_f2, _f2_register },
				{ register_file1::_f3, _f3_register },
				{ register_file1::_f4, _f4_register },
				{ register_file1::_f5, _f5_register },
				{ register_file1::_f6, _f6_register },
				{ register_file1::_f7, _f7_register },
				{ register_file1::_f8, _f8_register },
				{ register_file1::_f9, _f9_register },
				{ register_file1::_f10,_f10_register },
				{ register_file1::_f11, _f11_register },
				{ register_file1::_f12, _f12_register },
				{ register_file1::_f13, _f13_register },
				{ register_file1::_f14, _f14_register },
				{ register_file1::_f15, _f15_register },
				{ register_file1::_f16, _f16_register },
				{ register_file1::_f17, _f17_register },
				{ register_file1::_f18, _f18_register },
				{ register_file1::_f19, _f19_register },
				{ register_file1::_f20, _f20_register },
				{ register_file1::_f21, _f21_register },
				{ register_file1::_f22, _f22_register },
				{ register_file1::_f23, _f23_register },
				{ register_file1::_f24, _f24_register },
				{ register_file1::_f25, _f25_register },
				{ register_file1::_f26, _f26_register },
				{ register_file1::_f27, _f27_register },
				{ register_file1::_f28, _f28_register },
				{ register_file1::_f29, _f29_register },
				{ register_file1::_f30, _f30_register },
				{ register_file1::_f31, _f31_register }
			};
		}

		operand::operand(int r, string rn) : _register_number(r), _register_name(rn), _offset(0), _int_immediate(0), _char_immediate(""), _label_name(""),
			_operand_offset_kind(operand::offset_kind::KIND_NONE), _double_precision(false), _single_precision(false), _operand_kind(operand::kind::KIND_REGISTER), _on_stack(false),
			_middle_stack_offset(0) {

		}

		operand::operand(operand::offset_kind ok, int o, int r, string rn) : _register_number(r), _register_name(rn), _offset(o), _int_immediate(0), _label_name(""), _char_immediate(""),
			_operand_offset_kind(ok), _double_precision(false), _single_precision(false), _operand_kind(operand::kind::KIND_MEMORY), _on_stack(false), _middle_stack_offset(0) {

		}

		operand::operand(int i) : _register_number(0), _register_name(""), _offset(0), _int_immediate(i), _label_name(""), _operand_offset_kind(operand::offset_kind::KIND_NONE),
			_char_immediate(""), _double_precision(false), _single_precision(false), _operand_kind(operand::kind::KIND_INT_IMMEDIATE), _on_stack(false), _middle_stack_offset(0) {

		}

		operand::operand(bool c, string s) : _register_number(0), _register_name(""), _offset(0), _int_immediate(0), _operand_offset_kind(operand::offset_kind::KIND_NONE),
			_double_precision(false), _single_precision(false), _on_stack(false), _middle_stack_offset(0) {
			if (c)
				_char_immediate = s, _label_name = "", _operand_kind = operand::kind::KIND_CHAR_IMMEDIATE;
			else
				_char_immediate = "", _label_name = s, _operand_kind = operand::kind::KIND_LABEL;
		}

		int operand::register_number() {
			return _register_number;
		}

		string operand::register_name() {
			return _register_name;
		}

		int operand::offset() {
			return _offset;
		}

		void operand::set_offset(int o) {
			_offset = o;
		}

		void operand::set_int_immediate(int i) {
			_int_immediate = i;
		}

		int operand::int_immediate() {
			return _int_immediate;
		}

		string operand::char_immediate() {
			return _char_immediate;
		}

		string operand::label_name() {
			return _label_name;
		}

		operand::kind operand::operand_kind() {
			return _operand_kind;
		}

		operand::offset_kind operand::operand_offset_kind() {
			return _operand_offset_kind;
		}

		void operand::set_operand_offset_kind(operand::offset_kind ok) {
			_operand_offset_kind = ok;
		}

		bool operand::double_precision() {
			return _double_precision;
		}

		bool operand::single_precision() {
			return _single_precision;
		}

		void operand::set_single_precision(bool sp) {
			_single_precision = sp;
		}

		void operand::set_double_precision(bool dp) {
			_double_precision = dp;
		}

		string operand::raw_operand() {
			if (_operand_kind == operand::kind::KIND_NONE) return "";
			switch (_operand_kind) {
			case operand::kind::KIND_REGISTER:
				return _register_name;
				break;
			case operand::kind::KIND_MEMORY:
				return to_string(_offset) + "(" + _register_name + ")";
				break;
			case operand::kind::KIND_INT_IMMEDIATE:
				return to_string(_int_immediate);
				break;
			case operand::kind::KIND_CHAR_IMMEDIATE:
				return _char_immediate;
				break;
			case operand::kind::KIND_LABEL:
				return _label_name;
				break;
			default:
				return "";
				break;
			}
			return "";
		}

		bool operand::on_stack() {
			return _on_stack;
		}

		void operand::set_on_stack(bool b) {
			_on_stack = b;
		}

		int operand::middle_stack_offset() {
			return _middle_stack_offset;
		}

		void operand::set_middle_stack_offset(int i) {
			_middle_stack_offset = i;
		}

		bool operand::is_immediate() {
			return _operand_kind == operand::kind::KIND_CHAR_IMMEDIATE || _operand_kind == operand::kind::KIND_INT_IMMEDIATE;
		}

		insn::insn(string ri) : _raw_instruction(ri), _operand_1(nullptr), _operand_2(nullptr), _operand_3(nullptr), _insn_type_kind(insn::type_kind::KIND_RAW), _insn_kind(insn::kind::KIND_NONE) {

		}

		insn::insn(insn::kind ik) : _raw_instruction(""), _operand_1(nullptr), _operand_2(nullptr), _operand_3(nullptr), _insn_type_kind(insn::type_kind::KIND_0), _insn_kind(ik) {

		}

		insn::insn(insn::kind ik, shared_ptr<operand> o1) : _raw_instruction(""), _operand_1(o1), _operand_2(nullptr), _operand_3(nullptr), _insn_type_kind(insn::type_kind::KIND_1),
			_insn_kind(ik) {

		}

		insn::insn(insn::kind ik, shared_ptr<operand> o1, shared_ptr<operand> o2) : _raw_instruction(""), _operand_1(o1), _operand_2(o2), _operand_3(nullptr),
			_insn_type_kind(insn::type_kind::KIND_2), _insn_kind(ik) {

		}

		insn::insn(insn::kind ik, shared_ptr<operand> o1, shared_ptr<operand> o2, shared_ptr<operand> o3) : _raw_instruction(""), _operand_1(o1), _operand_2(o2), _operand_3(o3),
			_insn_type_kind(insn::type_kind::KIND_3), _insn_kind(ik) {

		}

		insn::insn(shared_ptr<operand> o1) : _raw_instruction(""), _operand_1(o1), _operand_2(nullptr), _operand_3(nullptr), _insn_type_kind(insn::type_kind::KIND_LABEL),
			_insn_kind(insn::kind::KIND_NONE) {

		}

		shared_ptr<operand> insn::operand_1() {
			return _operand_1;
		}

		shared_ptr<operand> insn::operand_2() {
			return _operand_2;
		}

		shared_ptr<operand> insn::operand_3() {
			return _operand_3;
		}

		insn::type_kind insn::insn_type_kind() {
			return _insn_type_kind;
		}

		insn::kind insn::insn_kind() {
			return _insn_kind;
		}

		string insn::raw_instruction() {
			return _raw_instruction;
		}

		string insn::raw_insn() {
			switch (_insn_type_kind) {
			case insn::type_kind::KIND_0:
				return "\t" + insn_kind_2_string.at(_insn_kind);
				break;
			case insn::type_kind::KIND_1:
				return "\t" + insn_kind_2_string.at(_insn_kind) + " " + _operand_1->raw_operand();
				break;
			case insn::type_kind::KIND_2:
				return "\t" + insn_kind_2_string.at(_insn_kind) + " " + _operand_1->raw_operand() + ", " + _operand_2->raw_operand();
				break;
			case insn::type_kind::KIND_3:
				return "\t" + insn_kind_2_string.at(_insn_kind) + " " + _operand_1->raw_operand() + ", " + _operand_2->raw_operand() + ", " + _operand_3->raw_operand();
				break;
			case insn::type_kind::KIND_LABEL:
				return _operand_1->raw_operand() + ":";
				break;
			case insn::type_kind::KIND_RAW:
				return "\t" + _raw_instruction;
				break;
			default:
				return "";
				break;
			}
			return "";
		}

		directive::directive(bool h, string s) : _immediate_string(""), _immediate_double(0), _immediate_float(0), _allocated_space(0), _alignment_boundary(0),
			_directive_kind(directive::kind::KIND_HEADER), _word(false), _half(false), _byte(false), _word_list(vector<shared_ptr<operand>>{}), _half_list(vector<shared_ptr<operand>>{}),
			_byte_list(vector<shared_ptr<operand>>{}), _double_list(vector<double>{}), _float_list(vector<float>{}) {
			if (h) _label_name = "", _header_name = s;
			else _label_name = s, _header_name = "";
		}

		directive::directive(string ln, string is) : _label_name(ln), _immediate_string(is), _immediate_double(0), _immediate_float(0), _allocated_space(0), _alignment_boundary(0),
			_header_name(""), _directive_kind(directive::kind::KIND_STRING), _word(false), _half(false), _byte(false), _word_list(vector<shared_ptr<operand>>{}),
			_half_list(vector<shared_ptr<operand>>{}), _byte_list(vector<shared_ptr<operand>>{}), _double_list(vector<double>{}), _float_list(vector<float>{}) {

		}

		directive::directive(string ln) : _label_name(ln), _immediate_string(""), _immediate_double(0), _immediate_float(0), _allocated_space(0), _alignment_boundary(0),
			_header_name(""), _directive_kind(directive::kind::KIND_GLOBL), _word(false), _half(false), _byte(false), _word_list(vector<shared_ptr<operand>>{}),
			_half_list(vector<shared_ptr<operand>>{}), _byte_list(vector<shared_ptr<operand>>{}), _double_list(vector<double>{}), _float_list(vector<float>{}) {

		}

		directive::directive(string ln, double id) : _label_name(ln), _immediate_string(""), _immediate_double(id), _immediate_float(0), _allocated_space(0), _alignment_boundary(0),
			_header_name(""), _directive_kind(directive::kind::KIND_DOUBLE), _word(false), _half(false), _byte(false), _word_list(vector<shared_ptr<operand>>{}),
			_half_list(vector<shared_ptr<operand>>{}), _byte_list(vector<shared_ptr<operand>>{}), _double_list(vector<double>{}), _float_list(vector<float>{}) {

		}

		directive::directive(string ln, float _if) : _label_name(ln), _immediate_string(""), _immediate_double(0), _immediate_float(_if), _allocated_space(0), _alignment_boundary(0),
			_header_name(""), _directive_kind(directive::kind::KIND_FLOAT), _word(false), _half(false), _byte(false), _word_list(vector<shared_ptr<operand>>{}),
			_half_list(vector<shared_ptr<operand>>{}), _byte_list(vector<shared_ptr<operand>>{}), _double_list(vector<double>{}), _float_list(vector<float>{}) {

		}

		directive::directive(string ln, int as, bool w) : _label_name(ln), _immediate_string(""), _immediate_double(0), _immediate_float(0), _allocated_space(as), _alignment_boundary(0),
			_header_name(""), _directive_kind(w ? directive::kind::KIND_WORD : directive::kind::KIND_SPACE), _word(w), _half(false), _byte(false), _word_list(vector<shared_ptr<operand>>{}),
			_half_list(vector<shared_ptr<operand>>{}), _byte_list(vector<shared_ptr<operand>>{}), _double_list(vector<double>{}), _float_list(vector<float>{}) {

		}

		directive::directive(int ab) : _label_name(""), _immediate_string(""), _immediate_double(0), _immediate_float(0), _allocated_space(0), _alignment_boundary(ab),
			_header_name(""), _directive_kind(directive::kind::KIND_ALIGN), _word(false), _half(false), _byte(false), _word_list(vector<shared_ptr<operand>>{}),
			_half_list(vector<shared_ptr<operand>>{}), _byte_list(vector<shared_ptr<operand>>{}), _double_list(vector<double>{}), _float_list(vector<float>{}) {

		}

		directive::directive(directive::kind k, string ln, vector<shared_ptr<operand>> ol) : _label_name(ln), _immediate_string(""), _immediate_double(0), _immediate_float(0), _allocated_space(0),
			_alignment_boundary(0), _header_name(""), _directive_kind(k), _double_list(vector<double>{}), _float_list(vector<float>{}) {
			_word = _directive_kind == directive::kind::KIND_WORD;
			_half = _directive_kind == directive::kind::KIND_HALF;
			_byte = _directive_kind == directive::kind::KIND_BYTE;
			if (_word)
				_word_list = ol;
			else if (_half)
				_half_list = ol;
			else if (_byte)
				_byte_list = ol;
		}

		directive::directive(string ln, vector<double> vd) : _label_name(ln), _immediate_string(""), _immediate_double(0), _immediate_float(0), _allocated_space(0),
			_alignment_boundary(0), _header_name(""), _directive_kind(directive::kind::KIND_DOUBLE), _double_list(vd), _float_list(vector<float>{}), _word(false), _half(false), _byte(false),
			_word_list(vector<shared_ptr<operand>>{}), _half_list(vector<shared_ptr<operand>>{}), _byte_list(vector<shared_ptr<operand>>{}) {

		}

		directive::directive(string ln, vector<float> vf) : _label_name(ln), _immediate_string(""), _immediate_double(0), _immediate_float(0), _allocated_space(0),
			_alignment_boundary(0), _header_name(""), _directive_kind(directive::kind::KIND_FLOAT), _double_list(vector<double>{}), _float_list(vf), _word(false), _half(false), _byte(false),
			_word_list(vector<shared_ptr<operand>>{}), _half_list(vector<shared_ptr<operand>>{}), _byte_list(vector<shared_ptr<operand>>{}) {

		}

		bool directive::word() {
			return _word;
		}

		bool directive::half() {
			return _half;
		}

		bool directive::byte() {
			return _byte;
		}

		vector<shared_ptr<operand>> directive::word_list() {
			return _word_list;
		}

		vector<shared_ptr<operand>> directive::half_list() {
			return _half_list;
		}

		vector<shared_ptr<operand>> directive::byte_list() {
			return _byte_list;
		}

		vector<double> directive::double_list() {
			return _double_list;
		}

		vector<float> directive::float_list() {
			return _float_list;
		}

		string directive::raw_directive() {
			if(_directive_kind == directive::kind::KIND_NONE) return "";
			switch(_directive_kind) {
				case directive::kind::KIND_HEADER:
					return _header_name;
					break;
				case directive::kind::KIND_GLOBL:
					return "\t.globl " + _label_name;
					break;
				case directive::kind::KIND_ALIGN:
					return "\t.align " + to_string(_alignment_boundary);
					break;
				case directive::kind::KIND_SPACE:
					return "\t" + _label_name + ": .space " + to_string(_allocated_space);
					break;
				case directive::kind::KIND_WORD: {
					if(_word_list.size() == 0)
						return "\t" + _label_name + ": .word " + to_string(_allocated_space);
					string ret = "\t" + _label_name + ": .word ";
					for (int i = 0; i < _word_list.size(); i++) {
						ret += _word_list[i]->raw_operand();
						if (i != _word_list.size() - 1) ret += ", ";
					}
					return ret;
				}
					break;
				case directive::kind::KIND_HALF: {
					string ret = "\t" + _label_name + ": .half ";
					for (int i = 0; i < _half_list.size(); i++) {
						ret += _half_list[i]->raw_operand();
						if (i != _half_list.size() - 1) ret += ", ";
					}
					return ret;
				}
					break;
				case directive::kind::KIND_BYTE: {
					string ret = "\t" + _label_name + ": .byte ";
					for (int i = 0; i < _byte_list.size(); i++) {
						ret += _byte_list[i]->raw_operand();
						if (i != _byte_list.size() - 1) ret += ", ";
					}
					return ret;
				}
					break;
				case directive::kind::KIND_FLOAT: {
					if (_float_list.size() == 0) {
						stringstream ss;
						ss << setprecision(std::numeric_limits<float>::max_digits10) << _immediate_float;
						string s;
						ss >> s;
						return "\t" + _label_name + ": .float " + s;
					}
					string ret = "\t" + _label_name + ": .float ";
					for (int i = 0; i < _float_list.size(); i++) {
						stringstream ss;
						ss << setprecision(std::numeric_limits<float>::max_digits10) << _float_list[i];
						string s;
						ss >> s;
						ret += s;
						if (i != _float_list.size() - 1)
							ret += ", ";
					}
					return ret;
				}
					break;
				case directive::kind::KIND_DOUBLE: {
					if (_double_list.size() == 0) {
						stringstream ss;
						ss << setprecision(std::numeric_limits<double>::max_digits10) << _immediate_double;
						string s;
						ss >> s;
						return "\t" + _label_name + ": .double " + s;
					}
					string ret = "\t" + _label_name + ": .double ";
					for (int i = 0; i < _double_list.size(); i++) {
						stringstream ss;
						ss << setprecision(std::numeric_limits<double>::max_digits10) << _double_list[i];
						string s;
						ss >> s;
						ret += s;
						if (i != _double_list.size() - 1) ret += ", ";
					}
					return ret;
				}
					break;
				case directive::kind::KIND_STRING:
					return "\t" + _label_name + ": .asciiz " + _immediate_string;
					break;
			}
			return "";
		}

		string directive::label_name() {
			return _label_name;
		}

		string directive::immediate_string() {
			return _immediate_string;
		}

		double directive::immediate_double() {
			return _immediate_double;
		}

		float directive::immediate_float() {
			return _immediate_float;
		}

		int directive::allocated_space() {
			return _allocated_space;
		}

		int directive::alignment_boundary() {
			return _alignment_boundary;
		}

		string directive::header_name() {
			return _header_name;
		}

		directive::kind directive::directive_kind() {
			return _directive_kind;
		}

		mips_frame::mips_frame(bool mf, bool gf, bool dprv, bool sprv) : _main_function(mf), _global_frame(gf), _double_precision_return_value(dprv), _single_precision_return_value(sprv),
			_frame_size(0), _frame_size_set(false), _body_insn_list(vector<shared_ptr<insn>>{}), _prologue_insn_list(vector<shared_ptr<insn>>{}), _epilogue_insn_list(vector<shared_ptr<insn>>{}),
		 	_variable_storage_map(map<string, shared_ptr<operand>>{}), _middle_section_size(0), _top_section_size(0), _overflow_register_counter(0), _fp_overflow_register_counter(0),
			_break_stack(stack<shared_ptr<operand>>{}), _continue_stack(stack<shared_ptr<operand>>{}), _operand_list(vector<shared_ptr<operand>>{}), _return_label_set(false),
			_return_label(nullptr), _return_type(nullptr) {
			_register_usage_map = map<int, bool>{
				{ 0, false }, { 1, false }, { 2, false }, { 3, false }, { 4, false }, { 5, false }, { 6, false }, { 7, false }, { 8, false }, { 9, false }, { 10, false },
				{ 11, false }, { 12, false }, { 13, false }, { 14, false }, { 15, false }, { 16, false }, { 17, false }, { 18, false }, { 19, false }, { 20, false },
				{ 21, false }, { 22, false }, { 23, false }, { 24, false }, { 25, false }, { 26, false }, { 27, false }, { 28, false }, { 29, false }, { 30, false },
				{ 31, false }, { 32, false }, { 33, false }, { 34, false }, { 35, false }, { 36, false }, { 37, false }, { 38, false }, { 39, false }, { 40, false },
				{ 41, false }, { 42, false }, { 43, false }, { 44, false }, { 45, false }, { 46, false }, { 47, false }, { 48, false }, { 49, false }, { 50, false },
				{ 51, false }, { 52, false }, { 53, false }, { 54, false }, { 55, false }, { 56, false }, { 57, false }, { 58, false }, { 59, false }, { 60, false },
				{ 61, false }, { 62, false }, { 63, false }
			};
#if SYSTEM == 0 || SYSTEM == 1
			_saved_register_initially_stored_map = map<int, bool>{
				{ 0, false }, { 1, false }, { 2, false }, { 3, false }, { 4, false }, { 5, false }, { 6, false }, { 7, false },
				{ 8, false }, { 9, false }, { 10, false }, { 11, false }, { 12, false }, { 13, false }
			};
#elif SYSTEM == 2
			_saved_register_initially_stored_map = map<int, bool>{
				{ 9, false }, { 18, false }, { 19, false }, { 20, false }, { 21, false }, { 22, false }, { 23, false }, { 24, false }, { 25, false }, { 26, false }, { 27, false },
				{ 40, false }, { 41, false }, { 50, false }, { 51, false }, { 52, false }, { 53, false }, { 54, false }, { 55, false }, { 56, false }, { 57, false }, { 58, false }, { 59, false }
			};
#endif
		}

		mips_frame::~mips_frame() {

		}

		shared_ptr<type> mips_frame::return_type() {
			return _return_type;
		}

		bool mips_frame::return_type_set() {
			return _return_type_set;
		}

		void mips_frame::set_return_type(shared_ptr<type> t) {
			_return_type = t;
		}

		void mips_frame::set_return_type_set(bool s) {
			_return_type_set = s;
		}

		map<string, shared_ptr<operand>> mips_frame::variable_storage_map() {
			return _variable_storage_map;
		}

		shared_ptr<operand> mips_frame::return_label() {
			return _return_label;
		}

		void mips_frame::set_return_label(shared_ptr<operand> l) {
			_return_label = l;
		}

		bool mips_frame::return_label_set() {
			return _return_label_set;
		}

		void mips_frame::set_return_label_set(bool s) {
			_return_label_set = s;
		}

		int mips_frame::frame_size() {
			return _frame_size;
		}

		void mips_frame::set_frame_size(int f) {
			_frame_size = f;
		}

		bool mips_frame::frame_size_set() {
			return _frame_size_set;
		}

		void mips_frame::set_frame_size_set(bool f) {
			_frame_size_set = f;
		}

		vector<shared_ptr<insn>> mips_frame::body_insn_list() {
			return _body_insn_list;
		}

		void mips_frame::add_insn_to_body(shared_ptr<insn> i, bool b) {
			if (b)
				_body_insn_list.insert(_body_insn_list.begin(), i);
			else
				_body_insn_list.push_back(i);
		}

		vector<shared_ptr<insn>> mips_frame::prologue_insn_list() {
			return _prologue_insn_list;
		}

		void mips_frame::add_insn_to_prologue(shared_ptr<insn> i, bool b) {
			if (b)
				_prologue_insn_list.insert(_prologue_insn_list.begin(), i);
			else
				_prologue_insn_list.push_back(i);
		}

		vector<shared_ptr<insn>> mips_frame::epilogue_insn_list() {
			return _epilogue_insn_list;
		}

		void mips_frame::add_insn_to_epilogue(shared_ptr<insn> i, bool b) {
			if (b)
				_epilogue_insn_list.insert(_epilogue_insn_list.begin(), i);
			else
				_epilogue_insn_list.push_back(i);
		}

		void mips_frame::calculate_true_insn_offsets() {
			if(!_frame_size_set) return;
			auto process_operand = [this](shared_ptr<operand> o) {
				if(o == nullptr || (o->operand_kind() != operand::kind::KIND_MEMORY && o->operand_kind() != operand::kind::KIND_INT_IMMEDIATE)) return;
				bool i = o->operand_kind() == operand::kind::KIND_INT_IMMEDIATE;
				if (i && o->operand_offset_kind() == operand::offset_kind::KIND_NONE) return;
				switch(o->operand_offset_kind()) {
					case operand::offset_kind::KIND_TOP:
						if (i)
							o->set_int_immediate(o->int_immediate() + _frame_size);
						else
							o->set_offset(o->offset() + _frame_size);
						break;
					case operand::offset_kind::KIND_BOTTOM:
					case operand::offset_kind::KIND_TRUE:
						break;
					case operand::offset_kind::KIND_MIDDLE:
						if (i)
							o->set_int_immediate(o->int_immediate() + _frame_size - _top_section_size);
						else
							o->set_offset(o->offset() + _frame_size - _top_section_size);
						break;
					default:
						break;
				}
				o->set_operand_offset_kind(operand::offset_kind::KIND_TRUE);
			};
			auto process_insn_list = [&process_operand](vector<shared_ptr<insn>>& vec) {
				for(shared_ptr<insn> i : vec) {
					if(i->operand_1() != nullptr)
						process_operand(i->operand_1());
					if(i->operand_2() != nullptr)
						process_operand(i->operand_2());
					if(i->operand_3() != nullptr)
						process_operand(i->operand_3());
				}
			};
			process_insn_list(_body_insn_list);
			process_insn_list(_prologue_insn_list);
			process_insn_list(_epilogue_insn_list);
		}

		bool mips_frame::is_register_in_use(int r) {
			if(_register_usage_map.find(r) == _register_usage_map.end()) return false;
			return _register_usage_map[r];
		}

		void mips_frame::mark_register(int r) {
			if(_register_usage_map.find(r) == _register_usage_map.end()) return;
			_register_usage_map[r] = true;
		}

		void mips_frame::unmark_register(int r) {
			if(_register_usage_map.find(r) == _register_usage_map.end()) return;
			_register_usage_map[r] = false;
		}

		bool mips_frame::is_saved_register_initially_stored(int r) {
			if(_saved_register_initially_stored_map.find(r) == _saved_register_initially_stored_map.end()) return false;
			return _saved_register_initially_stored_map[r];
		}

		void mips_frame::mark_saved_register_initially_stored(int r) {
			if(_saved_register_initially_stored_map.find(r) == _saved_register_initially_stored_map.end()) return;
			_saved_register_initially_stored_map[r] = true;
		}

		bool mips_frame::main_function() {
			return _main_function;
		}

		bool mips_frame::global_frame() {
			return _global_frame;
		}

		bool mips_frame::double_precision_return_value() {
			return _double_precision_return_value;
		}

		bool mips_frame::single_precision_return_value() {
			return _single_precision_return_value;
		}

		void mips_frame::add_variable(string var, shared_ptr<operand> o) {
			_variable_storage_map[var] = o;
		}

		shared_ptr<operand> mips_frame::get_variable_offset(string var) {
			if (_variable_storage_map.find(var) == _variable_storage_map.end()) return nullptr;
			return _variable_storage_map[var];
		}

		void mips_frame::update_middle_section_size(int b) {
			_middle_section_size += b;
		}

		int mips_frame::middle_section_size() {
			return _middle_section_size;
		}

		void mips_frame::update_top_section_size(int b) {
			_top_section_size += b;
		}

		int mips_frame::top_section_size() {
			return _top_section_size;
		}

		void mips_frame::pop_break_stack() {
			_break_stack.pop();
		}

		void mips_frame::pop_continue_stack() {
			_continue_stack.pop();
		}

		shared_ptr<operand> mips_frame::current_break() {
			return _break_stack.top();
		}

		shared_ptr<operand> mips_frame::current_continue() {
			return _continue_stack.top();
		}

		void mips_frame::push_break_stack(shared_ptr<operand> o) {
			_break_stack.push(o);
		}

		void mips_frame::push_continue_stack(shared_ptr<operand> o) {
			_continue_stack.push(o);
		}

		void mips_frame::add_operand(shared_ptr<operand> o) {
			_operand_list.push_back(o);
		}

		shared_ptr<operand> mips_frame::find_last_operand(int r) {
			for(int i = _operand_list.size() - 1; i >= 0; i--) {
				shared_ptr<operand> o = _operand_list[i];
				if(o->operand_kind() == operand::kind::KIND_REGISTER && o->register_number() == r)
					return o;
			}
			return nullptr;
		}

		shared_ptr<operand> mips_frame::find_first_operand(int r) {
			for(int i = 0; i < _operand_list.size(); i++) {
				shared_ptr<operand> o = _operand_list[i];
				if(o->operand_kind() == operand::kind::KIND_REGISTER && o->register_number() == r)
					return o;
			}
			return nullptr;
		}

		int mips_frame::current_overflow_register() {
#if SYSTEM == 0 || SYSTEM == 1
			return _overflow_register_counter++ % 24;
#elif SYSTEM == 2
			return _overflow_register_counter++ % 26;
#endif
		}

		int mips_frame::current_fp_overflow_register() {
#if SYSTEM == 0 || SYSTEM == 1
			return _fp_overflow_register_counter++ % 12;
#elif SYSTEM == 2
			return _fp_overflow_register_counter++ % 32;
#endif
		}

		void mips_frame::restore_saved_registers(shared_ptr<mips_code> mc) {
			if(!_frame_size_set) return;
			shared_ptr<operand> fp = register_file2::register_2_register_object.at(register_file1::_fp);
			string sfp = register_file1::_fp;
#if SYSTEM == 0 || SYSTEM == 1
			int s0 = register_file1::register_2_int.at(register_file1::_s0), f20 = register_file1::register_2_int.at(register_file1::_f20);
			for(int i = 0; i < 8; i++) {
#elif SYSTEM == 2
			int s1 = register_file1::register_2_int.at(register_file1::_s1);
			vector<int> stored_indices = { 9, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 },
				fstored_indices = { 40, 41, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59 };
			for(int i = 0; i < 11; i++) {
#endif
				if(_saved_register_initially_stored_map[i]) {
#if SYSTEM == 0 || SYSTEM == 1
					shared_ptr<operand> l = find_first_operand(s0 + i);
#elif SYSTEM == 2
					shared_ptr<operand> l = find_first_operand(stored_indices[i]);
#endif
					if(l == nullptr || !l->on_stack()) mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					int m = l->middle_stack_offset();
#if SYSTEM == 0 || SYSTEM == 1
					string s = register_file1::int_2_register.at(s0 + i);
					mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_LW, make_shared<operand>(s0 + i, s),
								make_shared<operand>(operand::offset_kind::KIND_MIDDLE, m, register_file1::register_2_int.at(sfp), sfp)));
					_saved_register_initially_stored_map[s0 + i] = false;
#elif SYSTEM == 2
					string s = register_file1::int_2_register.at(s1 + i);
					mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_LW, make_shared<operand>(stored_indices[i], s),
								make_shared<operand>(operand::offset_kind::KIND_MIDDLE, m, register_file1::register_2_int.at(sfp), sfp)));
					_saved_register_initially_stored_map[stored_indices[i]] = false;
#endif
				}
			}
#if SYSTEM == 0 || SYSTEM == 1
			for(int i = 8; i < 14; i++) {
				int curr = f20 + (i - 8) * 2;
				string scurr = register_file1::int_2_register.at(curr);
				if(_saved_register_initially_stored_map[i]) {
					shared_ptr<operand> l = find_first_operand(curr);
					if(l == nullptr || !l->on_stack()) mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					int m = l->middle_stack_offset();
					string s = register_file1::int_2_register.at(curr);
					mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_LDC1, make_shared<operand>(curr, scurr),
								make_shared<operand>(operand::offset_kind::KIND_MIDDLE, m, register_file1::register_2_int.at(sfp), sfp)));
					_saved_register_initially_stored_map[curr] = false;
				}
			}
#elif SYSTEM == 2
			for(int i = 0; i < 12; i++) {
				int curr = fstored_indices[i];
				string scurr = register_file1::int_2_register.at(curr);
				if(_saved_register_initially_stored_map[fstored_indices[i]]) {
					shared_ptr<operand> l = find_first_operand(curr);
					if(l == nullptr || !l->on_stack()) mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					int m = l->middle_stack_offset();
					string s = register_file1::int_2_register.at(curr);
					mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_LDC1, make_shared<operand>(curr, scurr),
								make_shared<operand>(operand::offset_kind::KIND_MIDDLE, m, register_file1::register_2_int.at(sfp), sfp)));
					_saved_register_initially_stored_map[curr] = false;
				}
			}
#endif
		}

		mips_code::mips_code(shared_ptr<spectre::parser::parser> p) : _insn_list(vector<shared_ptr<insn>>{}), _data_directive_list(vector<shared_ptr<directive>>{}), 
			_globl_directive_list(vector<shared_ptr<directive>>{}), _ast_parser(p), _label_counter(0), _frame_stack(stack<shared_ptr<mips_frame>>{}), _global_variable_name_list(vector<string>{}),
			_misc_counter(0), _inside_function(false) {
			_frame_stack.push(make_shared<mips_frame>(false, true, false, false));
		}

		mips_code::~mips_code() {

		}

		vector<shared_ptr<insn>> mips_code::insn_list() {
			return _insn_list;
		}

		vector<string> mips_code::raw_insn_list() {
			vector<string> ret;
			for(shared_ptr<directive> d : _data_directive_list)
				ret.push_back(d->raw_directive());
			for (shared_ptr<directive> d : _globl_directive_list)
				ret.push_back(d->raw_directive());
			for(shared_ptr<insn> i : _insn_list)
				ret.push_back(i->raw_insn());
			return ret;
		}

		vector<shared_ptr<directive>> mips_code::data_directive_list() {
			return _data_directive_list;
		}

		vector<shared_ptr<directive>> mips_code::globl_directive_list() {
			return _globl_directive_list;
		}

		void mips_code::add_insn(shared_ptr<insn> i) {
			_insn_list.push_back(i);
		}

		void mips_code::add_insn_to_beginning(shared_ptr<insn> i) {
			_insn_list.insert(_insn_list.begin(), i);
		}

		void mips_code::add_data_directive_to_beginning(shared_ptr<directive> d) {
			_data_directive_list.insert(_data_directive_list.begin(), d);
		}

		void mips_code::add_data_directive(shared_ptr<directive> d) {
			_data_directive_list.push_back(d);
		}

		void mips_code::add_globl_directive_to_beginning(shared_ptr<directive> d) {
			_globl_directive_list.insert(_globl_directive_list.begin(), d);
		}

		void mips_code::add_globl_directive(shared_ptr<directive> d) {
			_globl_directive_list.push_back(d);
		}

		void mips_code::add_insn_batch(vector<shared_ptr<insn>> il) {
			_insn_list.insert(_insn_list.end(), il.begin(), il.end());
		}

		shared_ptr<mips_frame> mips_code::current_frame() {
			return _frame_stack.top();
		}

		void mips_code::push_frame(shared_ptr<mips_frame> mf) {
			_frame_stack.push(mf);
		}

		void mips_code::pop_frame() {
			_frame_stack.pop();
		}

		pair<int, shared_ptr<operand>> mips_code::next_label() {
			int l = _label_counter++;
			return make_pair(l, make_shared<operand>(false, label_prefix + to_string(l)));
		}

		bool mips_code::is_name_global_variable(string var) {
			return find(_global_variable_name_list.begin(), _global_variable_name_list.end(), var) != _global_variable_name_list.end();
		}

		void mips_code::add_global_variable(string var) {
			_global_variable_name_list.push_back(var);
		}

		void mips_code::report_internal(string msg, string fn, int ln, string fl) {
			_ast_parser->report_internal(msg, fn, ln, fl);
		}

		int mips_code::next_misc_counter() {
			return _misc_counter++;
		}

		shared_ptr<struct_symbol> mips_code::find_struct_symbol(token n, int r) {
			return _ast_parser->find_struct_symbol(n, r);
		}

		void mips_code::set_inside_function(bool b) {
			_inside_function = b;
		}

		bool mips_code::inside_function() {
			return _inside_function;
		}

		void mips_code::add_constexpr_mapping(string k, variant<bool, int, unsigned int, float, double, string> v) {
			_constexpr_map[k] = v;
		}

		pair<bool, variant<bool, int, unsigned int, float, double, string>> mips_code::get_constexpr_mapping(string k) {
			if (_constexpr_map.find(k) == _constexpr_map.end())
				return make_pair(false, variant<bool, int, unsigned int, float, double, string>{});
			else
				return make_pair(true, _constexpr_map[k]);
		}

		bool mips_code::constexpr_mapping_exists(string k) {
			return _constexpr_map.find(k) != _constexpr_map.end();
		}

		shared_ptr<operand> allocate_general_purpose_register(shared_ptr<mips_code> mc) {
#if SYSTEM == 0 || SYSTEM == 1
			int t0 = register_file1::register_2_int.at(register_file1::_t0), t1 = register_file1::register_2_int.at(register_file1::_t1),
			    t2 = register_file1::register_2_int.at(register_file1::_t2), t3 = register_file1::register_2_int.at(register_file1::_t3),
			    t4 = register_file1::register_2_int.at(register_file1::_t4), t5 = register_file1::register_2_int.at(register_file1::_t5),
			    t6 = register_file1::register_2_int.at(register_file1::_t6), t7 = register_file1::register_2_int.at(register_file1::_t7),
			    t8 = register_file1::register_2_int.at(register_file1::_t8), t9 = register_file1::register_2_int.at(register_file1::_t9);
			int v0 = register_file1::register_2_int.at(register_file1::_v0), v1 = register_file1::register_2_int.at(register_file1::_v1);
			int a0 = register_file1::register_2_int.at(register_file1::_a0), a1 = register_file1::register_2_int.at(register_file1::_a1),
			    a2 = register_file1::register_2_int.at(register_file1::_a2), a3 = register_file1::register_2_int.at(register_file1::_a3);
			shared_ptr<operand> fp = register_file2::register_2_register_object.at(register_file1::_fp);
			string sfp = fp->register_name();
			for(int r : { t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, v0, v1, a0, a1, a2, a3 }) {
				if(!mc->current_frame()->is_register_in_use(r)) {
					shared_ptr<operand> o = make_shared<operand>(r, register_file1::int_2_register.at(r));
					mc->current_frame()->add_operand(o);
					mc->current_frame()->mark_register(r);
					return o;
				}
			}
#elif SYSTEM == 2
			int t0 = register_file1::register_2_int.at(register_file1::_t0), t1 = register_file1::register_2_int.at(register_file1::_t1),
				t2 = register_file1::register_2_int.at(register_file1::_t2), t3 = register_file1::register_2_int.at(register_file1::_t3),
				t4 = register_file1::register_2_int.at(register_file1::_t4), t5 = register_file1::register_2_int.at(register_file1::_t5),
				t6 = register_file1::register_2_int.at(register_file1::_t6);
			int a0 = register_file1::register_2_int.at(register_file1::_a0), a1 = register_file1::register_2_int.at(register_file1::_a1),
				a2 = register_file1::register_2_int.at(register_file1::_a2), a3 = register_file1::register_2_int.at(register_file1::_a3),
				a4 = register_file1::register_2_int.at(register_file1::_a4), a5 = register_file1::register_2_int.at(register_file1::_a5),
				a6 = register_file1::register_2_int.at(register_file1::_a6), a7 = register_file1::register_2_int.at(register_file1::_a7);
			shared_ptr<operand> fp = register_file2::register_2_register_object.at(register_file1::_fp);
			string sfp = fp->register_name();
			for(int r : { t0, t1, t2, t3, t4, t5, t6, a0, a1, a2, a3, a4, a5, a6, a7 }) {
				if(!mc->current_frame()->is_register_in_use(r)) {
					shared_ptr<operand> o = make_shared<operand>(r, register_file1::int_2_register.at(r));
					mc->current_frame()->add_operand(o);
					mc->current_frame()->mark_register(r);
					return o;
				}
			}
#endif
#if SYSTEM == 0 || SYSTEM == 1
			int s0 = register_file1::register_2_int.at(register_file1::_s0), s1 = register_file1::register_2_int.at(register_file1::_s1),
			    s2 = register_file1::register_2_int.at(register_file1::_s2), s3 = register_file1::register_2_int.at(register_file1::_s3),
			    s4 = register_file1::register_2_int.at(register_file1::_s4), s5 = register_file1::register_2_int.at(register_file1::_s5),
			    s6 = register_file1::register_2_int.at(register_file1::_s6), s7 = register_file1::register_2_int.at(register_file1::_s7);
			for(int r : { s0, s1, s2, s3, s4, s5, s6, s7 }) {
				if(mc->current_frame()->is_saved_register_initially_stored(r - s0) && !mc->current_frame()->is_register_in_use(r)) {
					shared_ptr<operand> o = make_shared<operand>(r, register_file1::int_2_register.at(r));
					mc->current_frame()->add_operand(o);
					mc->current_frame()->mark_saved_register_initially_stored(r - s0);
					mc->current_frame()->mark_register(r);
					return o;
				}
			}
#elif SYSTEM == 2
			int s1 = register_file1::register_2_int.at(register_file1::_s1), s2 = register_file1::register_2_int.at(register_file1::_s2),
				s3 = register_file1::register_2_int.at(register_file1::_s3), s4 = register_file1::register_2_int.at(register_file1::_s4),
				s5 = register_file1::register_2_int.at(register_file1::_s5), s6 = register_file1::register_2_int.at(register_file1::_s6),
				s7 = register_file1::register_2_int.at(register_file1::_s7), s8 = register_file1::register_2_int.at(register_file1::_s8),
				s9 = register_file1::register_2_int.at(register_file1::_s9), s10 = register_file1::register_2_int.at(register_file1::_s10),
				s11 = register_file1::register_2_int.at(register_file1::_s11);
			for(int r : { s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11 }) {
				if(mc->current_frame()->is_saved_register_initially_stored(r - s1) && !mc->current_frame()->is_register_in_use(r)) {
					shared_ptr<operand> o = make_shared<operand>(r, register_file1::int_2_register.at(r));
					mc->current_frame()->add_operand(o);
					mc->current_frame()->mark_saved_register_initially_stored(r);
					mc->current_frame()->mark_register(r);
					return o;
				}
			}
#endif
#if SYSTEM == 0 || SYSTEM == 1
			for(int r : { s0, s1, s2, s3, s4, s5, s6, s7 }) {
				if(!mc->current_frame()->is_saved_register_initially_stored(r - s0)) {
					shared_ptr<operand> l = mc->current_frame()->find_last_operand(r);
					if(l != nullptr) mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<operand> o = make_shared<operand>(r, register_file1::int_2_register.at(r));
					o->set_on_stack(true);
					o->set_middle_stack_offset(-(mc->current_frame()->middle_section_size() + 8));
					mc->current_frame()->update_middle_section_size(8);
					mc->current_frame()->mark_saved_register_initially_stored(r - s0);
					mc->current_frame()->add_operand(o);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, o, make_shared<operand>(operand::offset_kind::KIND_MIDDLE, o->middle_stack_offset(), fp->register_number(), sfp)));
					shared_ptr<operand> ret = make_shared<operand>(r, register_file1::int_2_register.at(r));
					mc->current_frame()->mark_register(r);
					mc->current_frame()->add_operand(ret);
					return ret;
				}
			}
#elif SYSTEM == 2
			for(int r : { s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11 }) {
				if(!mc->current_frame()->is_saved_register_initially_stored(r - s1)) {
					shared_ptr<operand> l = mc->current_frame()->find_last_operand(r);
					if(l != nullptr) mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<operand> o = make_shared<operand>(r, register_file1::int_2_register.at(r));
					o->set_on_stack(true);
					o->set_middle_stack_offset(-(mc->current_frame()->middle_section_size() + 8));
					mc->current_frame()->update_middle_section_size(8);
					mc->current_frame()->mark_saved_register_initially_stored(r);
					mc->current_frame()->add_operand(o);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, o, make_shared<operand>(operand::offset_kind::KIND_MIDDLE, o->middle_stack_offset(), fp->register_number(), sfp)));
					shared_ptr<operand> ret = make_shared<operand>(r, register_file1::int_2_register.at(r));
					mc->current_frame()->mark_register(r);
					mc->current_frame()->add_operand(ret);
					return ret;
				}
			}
#endif
			int curr = mc->current_frame()->current_overflow_register();
			int actual_register;
			shared_ptr<operand> l;
#if SYSTEM == 0 || SYSTEM == 1
			if(0 <= curr && curr <= 9)
				l = mc->current_frame()->find_last_operand(actual_register = curr + t0);
			else if(10 <= curr && curr <= 17)
				l = mc->current_frame()->find_last_operand(actual_register = (curr - 10) + s0);
			else if(curr == 18 || curr == 19)
				l = mc->current_frame()->find_last_operand(actual_register = (curr - 18) + v0);
			else
				l = mc->current_frame()->find_last_operand(actual_register = (curr - 20) + a0);
#elif SYSTEM == 2
			if(0 <= curr && curr <= 2)
				l = mc->current_frame()->find_last_operand(actual_register = curr + t0);
			else if(curr == 3)
				l = mc->current_frame()->find_last_operand(actual_register = (curr - 3) + s1);
			else if(4 <= curr && curr <= 11)
				l = mc->current_frame()->find_last_operand(actual_register = (curr - 4) + a0);
			else if(12 <= curr && curr <= 21)
				l = mc->current_frame()->find_last_operand(actual_register = (curr - 12) + a0); 
			else
				l = mc->current_frame()->find_last_operand(actual_register = (curr - 22) + a0); 
#endif
			if(l != nullptr && mc->current_frame()->is_register_in_use(l->register_number())) {
				l->set_on_stack(true);
				l->set_middle_stack_offset(-(mc->current_frame()->middle_section_size() + 8));
				mc->current_frame()->update_middle_section_size(8);
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, l, make_shared<operand>(operand::offset_kind::KIND_MIDDLE, l->middle_stack_offset(), fp->register_number(), sfp)));
			}
			shared_ptr<operand> ret = make_shared<operand>(actual_register, register_file1::int_2_register.at(actual_register));
			mc->current_frame()->mark_register(actual_register);
			mc->current_frame()->add_operand(ret);
			return ret;
		}

		shared_ptr<operand> allocate_general_purpose_fp_register(shared_ptr<mips_code> mc) {
#if SYSTEM == 0 || SYSTEM == 1
			int f4 = register_file1::register_2_int.at(register_file1::_f4), f6 = register_file1::register_2_int.at(register_file1::_f6),
			    f8 = register_file1::register_2_int.at(register_file1::_f8), f10 = register_file1::register_2_int.at(register_file1::_f10),
			    f16 = register_file1::register_2_int.at(register_file1::_f16), f18 = register_file1::register_2_int.at(register_file1::_f18);
			shared_ptr<operand> fp = register_file2::register_2_register_object.at(register_file1::_fp);
			string sfp = fp->register_name();
			for(int r : { f4, f6, f8, f10, f16, f18 }) {
				shared_ptr<operand> l = mc->current_frame()->find_last_operand(r);
				if(!mc->current_frame()->is_register_in_use(r)) {
					shared_ptr<operand> o = make_shared<operand>(r, register_file1::int_2_register.at(r));
					mc->current_frame()->add_operand(o);
					mc->current_frame()->mark_register(r);
					return o;
				}
			}
#elif SYSTEM == 2
			int f0 = register_file1::register_2_int.at(register_file1::_f0), f1 = register_file1::register_2_int.at(register_file1::_f1),
				f2 = register_file1::register_2_int.at(register_file1::_f2), f3 = register_file1::register_2_int.at(register_file1::_f3),
				f4 = register_file1::register_2_int.at(register_file1::_f4), f5 = register_file1::register_2_int.at(register_file1::_f5),
				f6 = register_file1::register_2_int.at(register_file1::_f6), f7 = register_file1::register_2_int.at(register_file1::_f7),
				f10 = register_file1::register_2_int.at(register_file1::_f10), f11 = register_file1::register_2_int.at(register_file1::_f11),
				f12 = register_file1::register_2_int.at(register_file1::_f12), f13 = register_file1::register_2_int.at(register_file1::_f13),
				f14 = register_file1::register_2_int.at(register_file1::_f14), f15 = register_file1::register_2_int.at(register_file1::_f15),
				f16 = register_file1::register_2_int.at(register_file1::_f16), f17 = register_file1::register_2_int.at(register_file1::_f17),
				f28 = register_file1::register_2_int.at(register_file1::_f28), f29 = register_file1::register_2_int.at(register_file1::_f29),
				f30 = register_file1::register_2_int.at(register_file1::_f30), f31 = register_file1::register_2_int.at(register_file1::_f31);
			shared_ptr<operand> fp = register_file2::register_2_register_object.at(register_file1::_fp);
			string sfp = fp->register_name();
			for(int r : { f0, f1, f2, f3, f4, f5, f6, f7, f28, f29, f30, f31, f10, f11, f12, f13, f14, f15, f16, f17 }) {
				shared_ptr<operand> l = mc->current_frame()->find_last_operand(r);
				if(!mc->current_frame()->is_register_in_use(r)) {
					shared_ptr<operand> o = make_shared<operand>(r, register_file1::int_2_register.at(r));
					mc->current_frame()->add_operand(o);
					mc->current_frame()->mark_register(r);
					return o;
				}
			}
#endif
#if SYSTEM == 0 || SYSTEM == 1
			int f20 = register_file1::register_2_int.at(register_file1::_f20), f22 = register_file1::register_2_int.at(register_file1::_f22),
			    f24 = register_file1::register_2_int.at(register_file1::_f24), f26 = register_file1::register_2_int.at(register_file1::_f26),
			    f28 = register_file1::register_2_int.at(register_file1::_f28), f30 = register_file1::register_2_int.at(register_file1::_f30);
			for(int r : { f20, f22, f24, f26, f28, f30 }) {
				if(mc->current_frame()->is_saved_register_initially_stored(8 + (r - f20) / 2) && !mc->current_frame()->is_register_in_use(r)) {
					shared_ptr<operand> o = make_shared<operand>(r, register_file1::int_2_register.at(r));
					mc->current_frame()->add_operand(o);
					mc->current_frame()->mark_register(r);
					return o;
				}
			}
#elif SYSTEM == 2
			int f8 = register_file1::register_2_int.at(register_file1::_f8), f9 = register_file1::register_2_int.at(register_file1::_f9),
				f18 = register_file1::register_2_int.at(register_file1::_f18), f19 = register_file1::register_2_int.at(register_file1::_f19),
				f20 = register_file1::register_2_int.at(register_file1::_f20), f21 = register_file1::register_2_int.at(register_file1::_f21),
				f22 = register_file1::register_2_int.at(register_file1::_f22), f23 = register_file1::register_2_int.at(register_file1::_f23),
				f24 = register_file1::register_2_int.at(register_file1::_f24), f25 = register_file1::register_2_int.at(register_file1::_f25),
				f26 = register_file1::register_2_int.at(register_file1::_f26), f27 = register_file1::register_2_int.at(register_file1::_f27);
			for(int r : { f8, f9, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27 }) {
				if(mc->current_frame()->is_saved_register_initially_stored(r) && !mc->current_frame()->is_register_in_use(r)) {
					shared_ptr<operand> o = make_shared<operand>(r, register_file1::int_2_register.at(r));
					mc->current_frame()->add_operand(o);
					mc->current_frame()->mark_register(r);
					return o;
				}
			}
#endif
#if SYSTEM == 0 || SYSTEM == 1
			for(int r : { f20, f22, f24, f26, f28, f30 }) {
				if(!mc->current_frame()->is_saved_register_initially_stored(8 + (r - f20) / 2)) {
					shared_ptr<operand> l = mc->current_frame()->find_last_operand(r);
					if(l != nullptr) mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<operand> o = make_shared<operand>(r, register_file1::int_2_register.at(r));
					o->set_on_stack(true);
					o->set_middle_stack_offset(-(mc->current_frame()->middle_section_size() + 8));
					mc->current_frame()->update_middle_section_size(8);
					mc->current_frame()->mark_saved_register_initially_stored(8 + (r - f20) / 2);
					mc->current_frame()->add_operand(o);
					shared_ptr<operand> ret = make_shared<operand>(r, register_file1::int_2_register.at(r));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SDC1, o, make_shared<operand>(operand::offset_kind::KIND_MIDDLE, o->middle_stack_offset(), fp->register_number(), sfp)));
					mc->current_frame()->mark_register(r);
					mc->current_frame()->add_operand(ret);
					return ret;
				}
			}
#elif SYSTEM == 2
			for(int r : { f8, f9, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27 }) {
				if(!mc->current_frame()->is_saved_register_initially_stored(r)) {
					shared_ptr<operand> l = mc->current_frame()->find_last_operand(r);
					if(l != nullptr) mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<operand> o = make_shared<operand>(r, register_file1::int_2_register.at(r));
					o->set_on_stack(true);
					o->set_middle_stack_offset(-(mc->current_frame()->middle_section_size() + 8));
					mc->current_frame()->update_middle_section_size(8);
					mc->current_frame()->mark_saved_register_initially_stored(r);
					mc->current_frame()->add_operand(o);
					shared_ptr<operand> ret = make_shared<operand>(r, register_file1::int_2_register.at(r));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SDC1, o, make_shared<operand>(operand::offset_kind::KIND_MIDDLE, o->middle_stack_offset(), fp->register_number(), sfp)));
					mc->current_frame()->mark_register(r);
					mc->current_frame()->add_operand(ret);
					return ret;
				}
			}
#endif
			int curr = mc->current_frame()->current_fp_overflow_register();
			int actual_register;
			shared_ptr<operand> l;
#if SYSTEM == 0 || SYSTEM == 1
			if(0 <= curr && curr <= 3)
				l = mc->current_frame()->find_last_operand(actual_register = curr * 2 + f4);
			else if(curr == 4 || curr == 5)
				l = mc->current_frame()->find_last_operand(actual_register = (curr - 4) * 2 + f16);
			else
				l = mc->current_frame()->find_last_operand(actual_register = (curr - 6) * 2 + f20);
#elif SYSTEM == 2
			l = mc->current_frame()->find_last_operand(actual_register = curr + f0);
#endif
			if(l != nullptr && mc->current_frame()->is_register_in_use(l->register_number())) {
				l->set_on_stack(true);
				l->set_middle_stack_offset(-(mc->current_frame()->middle_section_size() + 8));
				mc->current_frame()->update_middle_section_size(8);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SDC1, l, make_shared<operand>(operand::offset_kind::KIND_MIDDLE, l->middle_stack_offset(), fp->register_number(), sfp)));
			}
			shared_ptr<operand> ret = make_shared<operand>(actual_register, register_file1::int_2_register.at(actual_register));
			mc->current_frame()->mark_register(actual_register);
			mc->current_frame()->add_operand(ret);
			return ret;
		}

		void free_general_purpose_register(shared_ptr<mips_code> mc, shared_ptr<operand> o) {
			if(o == nullptr || o->register_number() > 31 || o->operand_kind() != operand::kind::KIND_REGISTER) return;
			mc->current_frame()->unmark_register(o->register_number());
		}

		void free_general_purpose_fp_register(shared_ptr<mips_code> mc, shared_ptr<operand> o) {
			if(o == nullptr || o->register_number() <= 31 || o->operand_kind() != operand::kind::KIND_REGISTER) return;
			mc->current_frame()->unmark_register(o->register_number());
		}

		void load_general_purpose_register_from_frame(shared_ptr<mips_code> mc, shared_ptr<operand> o) {
			shared_ptr<operand> fp = register_file2::register_2_register_object.at(register_file1::_fp);
			if(o == nullptr || o->register_number() > 31 || o->operand_kind() != operand::kind::KIND_REGISTER ||
					!o->on_stack()) return;
			mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, o, make_shared<operand>(operand::offset_kind::KIND_MIDDLE, o->middle_stack_offset(),
					fp->register_number(), fp->register_name())));
			o->set_on_stack(false);
			o->set_middle_stack_offset(0);
			mc->current_frame()->mark_register(o->register_number());
		}

		void load_general_purpose_fp_register_from_frame(shared_ptr<mips_code> mc, shared_ptr<operand> o) {
			shared_ptr<operand> fp = register_file2::register_2_register_object.at(register_file1::_fp);
			if(o == nullptr || o->register_number() <= 31 || o->operand_kind() != operand::kind::KIND_REGISTER ||
					!o->on_stack()) return;
			if(o->double_precision())
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LDC1, o, make_shared<operand>(operand::offset_kind::KIND_MIDDLE, o->middle_stack_offset(),
						fp->register_number(), fp->register_name())));
			else
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LWC1, o, make_shared<operand>(operand::offset_kind::KIND_MIDDLE, o->middle_stack_offset(),
						fp->register_number(), fp->register_name())));
			o->set_on_stack(false);
			o->set_middle_stack_offset(0);
			mc->current_frame()->mark_register(o->register_number());
		}

		void generate_mips(shared_ptr<mips_code> mc, shared_ptr<spectre::parser::parser> p) {
			mc->add_data_directive(make_shared<directive>(true, ".data"));
			mc->add_globl_directive(make_shared<directive>(true, ".text"));
			mc->add_data_directive(make_shared<directive>(2));
			mc->add_data_directive(make_shared<directive>("__fp_1", 1.f));
			mc->add_data_directive(make_shared<directive>(3));
			mc->add_data_directive(make_shared<directive>("__dp_1", 1.));
			mc->add_data_directive(make_shared<directive>(2));
			mc->add_data_directive(make_shared<directive>("__fp_m1", -1.f));
			mc->add_data_directive(make_shared<directive>(3));
			mc->add_data_directive(make_shared<directive>("__dp_m1", -1.));
			tuple<vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>> reordered = reorder_stmt_list(mc, p->stmt_list());
			vector<shared_ptr<stmt>> _vdecls = get<0>(reordered), _functions = get<1>(reordered), _main = get<2>(reordered), _rest = get<3>(reordered);
			for (shared_ptr<stmt> s : _vdecls) {
				if (s->stmt_kind() != stmt::kind::KIND_VARIABLE_DECLARATION && s->stmt_kind() != stmt::kind::KIND_ACCESS) {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return;
				}
				if (s->stmt_kind() == stmt::kind::KIND_VARIABLE_DECLARATION)
					for (shared_ptr<variable_declaration> vd : s->stmt_variable_declaration_list())
						generate_variable_declaration_mips(mc, vd);
				else if (s->stmt_kind() == stmt::kind::KIND_ACCESS)
					generate_access_stmt_mips(mc, s->stmt_access());
			}
			for (shared_ptr<stmt> s : _functions)
				generate_stmt_mips(mc, s);
			for (shared_ptr<stmt> s : _main)
				generate_stmt_mips(mc, s);
			for (shared_ptr<stmt> s : _rest)
				generate_stmt_mips(mc, s);
			mc->add_insn_batch(mc->current_frame()->prologue_insn_list());
			mc->add_insn_batch(mc->current_frame()->body_insn_list());
			mc->add_insn_batch(mc->current_frame()->epilogue_insn_list());
		}

		void generate_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<stmt> s) {
			if(s == nullptr) return;
			switch(s->stmt_kind()) {
				case stmt::kind::KIND_EXPRESSION: {
					shared_ptr<operand> op = generate_expression_mips(mc, s->stmt_expression(), false);
					op = load_value_into_register(mc, op, s->stmt_expression()->expression_type());
					if (op->operand_kind() == operand::kind::KIND_REGISTER) {
						if (op->register_number() > 31)
							free_general_purpose_fp_register(mc, op);
						else
							free_general_purpose_register(mc, op);
					}
				}
					break;
				case stmt::kind::KIND_VARIABLE_DECLARATION: {
					for (shared_ptr<variable_declaration> vd : s->stmt_variable_declaration_list())
						generate_variable_declaration_mips(mc, vd);
				}
					break;
				case stmt::kind::KIND_FUNCTION:
					return generate_function_stmt_mips(mc, s->stmt_function());
					break;
				case stmt::kind::KIND_RETURN:
					return generate_return_stmt_mips(mc, s->stmt_return());
					break;
				case stmt::kind::KIND_IF:
					return generate_if_stmt_mips(mc, s->stmt_if());
					break;
				case stmt::kind::KIND_WHILE:
					return generate_while_stmt_mips(mc, s->stmt_while());
					break;
				case stmt::kind::KIND_BLOCK:
					return generate_block_stmt_mips(mc, s->stmt_block());
					break;
				case stmt::kind::KIND_BREAK_CONTINUE:
					return generate_break_continue_stmt_mips(mc, s->stmt_break_continue());
					break;
				case stmt::kind::KIND_SWITCH:
					return generate_switch_stmt_mips(mc, s->stmt_switch());
					break;
				case stmt::kind::KIND_CASE_DEFAULT:
					return generate_case_default_stmt_mips(mc, s->stmt_case_default());
					break;
				case stmt::kind::KIND_DO_WHILE:
					return generate_do_while_stmt_mips(mc, s->stmt_while());
					break;
				case stmt::kind::KIND_FOR:
					return generate_for_stmt_mips(mc, s->stmt_for());
					break;
				case stmt::kind::KIND_EMPTY:
				case stmt::kind::KIND_STRUCT:
				case stmt::kind::KIND_USING:
				case stmt::kind::KIND_INCLUDE:
					break;
				case stmt::kind::KIND_ASM:
					return generate_asm_stmt_mips(mc, s->stmt_asm());
					break;
				case stmt::kind::KIND_DELETE:
					return generate_delete_stmt_mips(mc, s->stmt_delete());
				case stmt::kind::KIND_NAMESPACE:
					if(s->stmt_namespace()->namespace_stmt_kind() == namespace_stmt::kind::KIND_ALIAS)
						break;
				case stmt::kind::KIND_ACCESS:
				default:
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					break;
			}
		}

		shared_ptr<operand> load_value_into_register(shared_ptr<mips_code> mc, shared_ptr<operand> op, shared_ptr<type> t) {
			if (op == nullptr || t == nullptr) return nullptr;
			if (op->is_immediate()) {
				shared_ptr<operand> reg = allocate_general_purpose_register(mc);
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, reg, register_file2::_zero_register, op));
				return reg;
			}
			else if (op->operand_kind() == operand::kind::KIND_MEMORY || op->operand_kind() == operand::kind::KIND_LABEL) {
				if (t->type_kind() == type::kind::KIND_PRIMITIVE && t->type_array_kind() != type::array_kind::KIND_ARRAY) {
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_VOID)
						return op;
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_BOOL) {
						shared_ptr<operand> reg = allocate_general_purpose_register(mc);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LBU, reg, op));
						free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
						return reg;
					}
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_CHAR || pt->primitive_type_kind() == primitive_type::kind::KIND_BYTE) {
						shared_ptr<operand> reg = allocate_general_purpose_register(mc);
						if (pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_SIGNED)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LB, reg, op));
						else
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LBU, reg, op));
						free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
						return reg;
					}
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_SHORT) {
						shared_ptr<operand> reg = allocate_general_purpose_register(mc);
						if (pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_SIGNED)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LH, reg, op));
						else
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LHU, reg, op));
						free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
						return reg;
					}
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_INT || pt->primitive_type_kind() == primitive_type::kind::KIND_LONG) {
						shared_ptr<operand> reg = allocate_general_purpose_register(mc);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, reg, op));
						free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
						return reg;
					}
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
						shared_ptr<operand> reg = allocate_general_purpose_fp_register(mc);
						reg->set_double_precision(false);
						reg->set_single_precision(true);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LWC1, reg, op));
						free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
						return reg;
					}
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
						shared_ptr<operand> reg = allocate_general_purpose_fp_register(mc);
						reg->set_double_precision(true);
						reg->set_single_precision(false);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LDC1, reg, op));
						free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
						return reg;
					}
				}
				else if (t->type_kind() == type::kind::KIND_STRUCT && t->type_array_kind() != type::array_kind::KIND_ARRAY) {
					shared_ptr<operand> reg = allocate_general_purpose_register(mc);
					if (op->operand_kind() == operand::kind::KIND_MEMORY) {
						shared_ptr<operand> imm = make_shared<operand>(op->offset());
						imm->set_operand_offset_kind(op->operand_offset_kind());
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, reg, register_file2::int_2_register_object.at(op->register_number()), imm));
						free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
					}
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg, op));
					return reg;
				}
				else if (t->type_kind() == type::kind::KIND_FUNCTION && t->type_array_kind() != type::array_kind::KIND_ARRAY) {
					shared_ptr<operand> reg = allocate_general_purpose_register(mc);
					if (op->operand_kind() == operand::kind::KIND_MEMORY) {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, reg, op));
						free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
					}
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg, op));
					return reg;
				}
				else {
					shared_ptr<operand> reg = allocate_general_purpose_register(mc);
					if (op->operand_kind() == operand::kind::KIND_MEMORY) {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, reg, op));
						free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
					}
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, reg, op));
					return reg;
				}
			}
			else if (op->operand_kind() == operand::kind::KIND_REGISTER)
				return op;
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand> cast_between_types(shared_ptr<mips_code> mc, shared_ptr<operand> src, shared_ptr<type> t1, shared_ptr<type> t2) {
			if (src == nullptr || t1 == nullptr || t2 == nullptr) return nullptr;
			if (t1->type_kind() != type::kind::KIND_PRIMITIVE || t2->type_kind() != type::kind::KIND_PRIMITIVE) return src;
			if (src->operand_kind() != operand::kind::KIND_REGISTER) {
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return nullptr;
			}
			shared_ptr<primitive_type> pt1 = static_pointer_cast<primitive_type>(t1), pt2 = static_pointer_cast<primitive_type>(t2);
			if ((pt1->primitive_type_kind() != primitive_type::kind::KIND_FLOAT && pt1->primitive_type_kind() != primitive_type::kind::KIND_DOUBLE) &&
				(pt2->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE || pt2->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)) {
				if (src->register_number() > 31) {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
				}
				shared_ptr<operand> reg = allocate_general_purpose_fp_register(mc);
#if SYSTEM == 0 || SYSTEM == 1
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MTC1, src, reg));
#elif SYSTEM == 2
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MTC1, reg, src));
#endif
				if (pt2->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
					reg->set_double_precision(true);
					reg->set_single_precision(false);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_CVT_D_W, reg, reg));
				}
				else {
					reg->set_double_precision(false);
					reg->set_single_precision(true);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_CVT_S_W, reg, reg));
				}
				free_general_purpose_register(mc, src);
				return reg;
			}
			else if ((pt1->primitive_type_kind() == primitive_type::kind::KIND_FLOAT || pt1->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) &&
				(pt2->primitive_type_kind() != primitive_type::kind::KIND_DOUBLE && pt2->primitive_type_kind() != primitive_type::kind::KIND_FLOAT)) {
				if (src->register_number() <= 31) {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
				}
				shared_ptr<operand> reg = allocate_general_purpose_register(mc);
				if (pt1->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_CVT_W_D, src, src));
				else
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_CVT_W_S, src, src));
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MFC1, reg, src));
				free_general_purpose_fp_register(mc, src);
				return reg;
			}
			else if ((pt1->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE || pt1->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) &&
				(pt2->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE || pt2->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)) {
				if (pt1->primitive_type_kind() == pt2->primitive_type_kind())
					return src;
				if (src->register_number() <= 31) {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
				}
				if (pt1->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE && pt2->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
					src->set_double_precision(false);
					src->set_single_precision(true);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_CVT_S_D, src, src));
					return src;
				}
				else if (pt1->primitive_type_kind() == primitive_type::kind::KIND_FLOAT && pt2->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
					src->set_double_precision(true);
					src->set_single_precision(false);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_CVT_D_S, src, src));
					return src;
				}
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return nullptr;
			}
			else
				return src;
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand> generate_expression_mips(shared_ptr<mips_code> mc, shared_ptr<expression> e, bool lvalue) {
			if (e == nullptr) return nullptr;
			shared_ptr<operand> ret = nullptr;
			for (int i = 0; i < e->assignment_expression_list().size(); i++) {
				shared_ptr<assignment_expression> ae = e->assignment_expression_list()[i];
				if (i == e->assignment_expression_list().size() - 1)
					ret = generate_assignment_expression_mips(mc, ae, lvalue);
				else
					ret = generate_assignment_expression_mips(mc, ae, false);
				if (i != e->assignment_expression_list().size() - 1) {
					ret = load_value_into_register(mc, ret, ae->assignment_expression_type());
					if (ret->register_number() > 31)
						free_general_purpose_fp_register(mc, ret);
					else
						free_general_purpose_register(mc, ret);
				}
			}
			return ret;
		}

		shared_ptr<operand> generate_assignment_expression_mips(shared_ptr<mips_code> mc, shared_ptr<assignment_expression> ae, bool lvalue) {
			if (ae == nullptr) return nullptr;
			switch (ae->assignment_expression_kind()) {
			case assignment_expression::kind::KIND_ASSIGNMENT: {
				shared_ptr<unary_expression> lhs = ae->lhs_assignment();
				shared_ptr<assignment_expression> rhs = ae->rhs_assignment();
				shared_ptr<type> ae_type = ae->assignment_expression_type();
				if (ae_type->type_kind() == type::kind::KIND_PRIMITIVE && ae_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
					shared_ptr<primitive_type> ae_ptype = static_pointer_cast<primitive_type>(ae_type);
					shared_ptr<operand> l = generate_unary_expression_mips(mc, lhs, true);
					shared_ptr<operand> r = generate_assignment_expression_mips(mc, rhs, false);
					binary_expression::operator_kind ae_kind = ae->assignment_operator_kind();
					insn::kind s = load_store_insn_from_type(mc, ae_ptype, false);
					if (lhs->unary_expression_type()->type_kind() != type::kind::KIND_PRIMITIVE || rhs->assignment_expression_type()->type_kind() != type::kind::KIND_PRIMITIVE) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					shared_ptr<primitive_type> pl = static_pointer_cast<primitive_type>(lhs->unary_expression_type()), pr = static_pointer_cast<primitive_type>(rhs->assignment_expression_type());
					shared_ptr<type> t = nullptr;
					if (pl->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
						t = pl;
					else if (pr->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
						t = pr;
					else if (pl->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
						t = pl;
					else if (pr->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
						t = pr;
					else if (pl->primitive_type_kind() == primitive_type::kind::KIND_LONG && pl->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED)
						t = pl;
					else if (pr->primitive_type_kind() == primitive_type::kind::KIND_LONG && pr->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED)
						t = pr;
					else if (pl->primitive_type_kind() == primitive_type::kind::KIND_LONG)
						t = pl;
					else if (pr->primitive_type_kind() == primitive_type::kind::KIND_LONG)
						t = pr;
					else if (pl->primitive_type_kind() == primitive_type::kind::KIND_INT && pl->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED)
						t = pl;
					else if (pr->primitive_type_kind() == primitive_type::kind::KIND_INT && pr->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED)
						t = pr;
					else if (pl->primitive_type_kind() == primitive_type::kind::KIND_INT)
						t = pl;
					else if (pr->primitive_type_kind() == primitive_type::kind::KIND_INT)
						t = pr;
					else if (pl->primitive_type_kind() == primitive_type::kind::KIND_SHORT && pl->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED)
						t = pl;
					else if (pl->primitive_type_kind() == primitive_type::kind::KIND_SHORT && pr->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED)
						t = pr;
					else if (pl->primitive_type_kind() == primitive_type::kind::KIND_SHORT)
						t = pl;
					else if (pr->primitive_type_kind() == primitive_type::kind::KIND_SHORT)
						t = pr;
					else
						t = pl;
					if (ae_kind == binary_expression::operator_kind::KIND_EQUALS) {
						r = load_value_into_register(mc, r, rhs->assignment_expression_type());
						r = cast_between_types(mc, r, rhs->assignment_expression_type(), lhs->unary_expression_type());
						mc->current_frame()->add_insn_to_body(make_shared<insn>(s, r, l));
						if (r->register_number() > 31)
							free_general_purpose_fp_register(mc, r);
						else
							free_general_purpose_register(mc, r);
					}
					else {
						binary_expression::operator_kind to_use = binary_expression::operator_kind::KIND_NONE;
						switch (ae_kind) {
						case binary_expression::operator_kind::KIND_ADD_EQUALS:
							to_use = binary_expression::operator_kind::KIND_ADD;
							break;
						case binary_expression::operator_kind::KIND_SUBTRACT_EQUALS:
							to_use = binary_expression::operator_kind::KIND_SUBTRACT;
							break;
						case binary_expression::operator_kind::KIND_MULTIPLY_EQUALS:
							to_use = binary_expression::operator_kind::KIND_MULTIPLY;
							break;
						case binary_expression::operator_kind::KIND_DIVIDE_EQUALS:
							to_use = binary_expression::operator_kind::KIND_DIVIDE;
							break;
						case binary_expression::operator_kind::KIND_BITWISE_AND_EQUALS:
							to_use = binary_expression::operator_kind::KIND_BITWISE_AND;
							break;
						case binary_expression::operator_kind::KIND_BITWISE_OR_EQUALS:
							to_use = binary_expression::operator_kind::KIND_BITWISE_OR;
							break;
						case binary_expression::operator_kind::KIND_BITWISE_XOR_EQUALS:
							to_use = binary_expression::operator_kind::KIND_BITWISE_XOR;
							break;
						case binary_expression::operator_kind::KIND_LOGICAL_AND_EQUALS:
							to_use = binary_expression::operator_kind::KIND_LOGICAL_AND;
							break;
						case binary_expression::operator_kind::KIND_LOGICAL_OR_EQUALS:
							to_use = binary_expression::operator_kind::KIND_LOGICAL_OR;
							break;
						case binary_expression::operator_kind::KIND_MODULUS_EQUALS:
							to_use = binary_expression::operator_kind::KIND_MODULUS;
							break;
						case binary_expression::operator_kind::KIND_SHIFT_LEFT_EQUALS:
							to_use = binary_expression::operator_kind::KIND_SHIFT_LEFT;
							break;
						case binary_expression::operator_kind::KIND_SHIFT_RIGHT_EQUALS:
							to_use = binary_expression::operator_kind::KIND_SHIFT_RIGHT;
							break;
						default:
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return nullptr;
							break;
						}
						shared_ptr<operand> v = load_value_into_register(mc, l, lhs->unary_expression_type());
						if (l->operand_kind() == operand::kind::KIND_MEMORY || l->operand_kind() == operand::kind::KIND_REGISTER)
							mc->current_frame()->mark_register(l->register_number());
						r = load_value_into_register(mc, r, rhs->assignment_expression_type());
						shared_ptr<operand> res = binary_expression_helper_1(mc, v, to_use, r, t,
							pl->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED || pr->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED);
						res = cast_between_types(mc, res, t, lhs->unary_expression_type());
						mc->current_frame()->add_insn_to_body(make_shared<insn>(s, res, l));
						if (res->register_number() > 31)
							free_general_purpose_fp_register(mc, res);
						else
							free_general_purpose_register(mc, res);
					}
					return l;
				}
				else if (ae_type->type_kind() == type::kind::KIND_STRUCT && ae_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
					shared_ptr<operand> l = generate_unary_expression_mips(mc, lhs, true);
					shared_ptr<operand> r = generate_assignment_expression_mips(mc, rhs, false);
					binary_expression::operator_kind ae_kind = ae->assignment_operator_kind();
					insn::kind s = load_store_insn_from_type(mc, ae_type, false),
						ld = load_store_insn_from_type(mc, ae_type, true);
					if (ae_kind != binary_expression::operator_kind::KIND_EQUALS) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					r = load_value_into_register(mc, r, rhs->assignment_expression_type());
					r = cast_between_types(mc, r, rhs->assignment_expression_type(), lhs->unary_expression_type());
					int sz = (int)calculate_type_size(mc, lhs->unary_expression_type());
					shared_ptr<operand> reg = allocate_general_purpose_register(mc);
					shared_ptr<operand> reg2 = nullptr;
					if (l->operand_kind() == operand::kind::KIND_LABEL) {
						reg2 = allocate_general_purpose_register(mc);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg2, l));
					}
					else
						reg2 = l;
					for (int off = 0; off < sz; off += 4) {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(ld, reg, make_shared<operand>(operand::offset_kind::KIND_TRUE, off, r->register_number(), r->register_name())));
						if (l->operand_kind() == operand::kind::KIND_LABEL)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(s, reg, make_shared<operand>(operand::offset_kind::KIND_TRUE, off, reg2->register_number(),
								reg2->register_name())));
						else if (l->operand_kind() == operand::kind::KIND_MEMORY)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(s, reg, make_shared<operand>(l->operand_offset_kind(), l->offset() + off, l->register_number(),
								l->register_name())));
						else if (l->operand_kind() == operand::kind::KIND_REGISTER)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(s, reg, make_shared<operand>(operand::offset_kind::KIND_TRUE, off, l->register_number(),
								l->register_name())));
						else {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return nullptr;
						}
					}
					if (r->register_number() > 31)
						free_general_purpose_fp_register(mc, r);
					else
						free_general_purpose_register(mc, r);
					free_general_purpose_register(mc, reg);
					if (l->operand_kind() == operand::kind::KIND_LABEL)
						free_general_purpose_register(mc, reg2);
					return l;
				}
				else {
					shared_ptr<operand> l = generate_unary_expression_mips(mc, lhs, true);
					shared_ptr<operand> r = generate_assignment_expression_mips(mc, rhs, false);
					binary_expression::operator_kind ae_kind = ae->assignment_operator_kind();
					insn::kind s = load_store_insn_from_type(mc, ae_type, false);
					if (ae_kind != binary_expression::operator_kind::KIND_EQUALS) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					r = load_value_into_register(mc, r, rhs->assignment_expression_type());
					r = cast_between_types(mc, r, rhs->assignment_expression_type(), lhs->unary_expression_type());
					mc->current_frame()->add_insn_to_body(make_shared<insn>(s, r, l));
					if (r->register_number() > 31)
						free_general_purpose_fp_register(mc, r);
					else
						free_general_purpose_register(mc, r);
					return l;
				}
			}
				break;
			case assignment_expression::kind::KIND_TERNARY:
				return generate_ternary_expression_mips(mc, ae->conditional_expression(), lvalue);
				break;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand> generate_ternary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<ternary_expression> te, bool lvalue) {
			if (te == nullptr) return nullptr;
			switch (te->ternary_expression_kind()) {
			case ternary_expression::kind::KIND_TERNARY: {
				shared_ptr<operand> res = nullptr;
				if (te->ternary_expression_type()->type_kind() == type::kind::KIND_PRIMITIVE && te->ternary_expression_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY &&
					(static_pointer_cast<primitive_type>(te->ternary_expression_type())->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE ||
					static_pointer_cast<primitive_type>(te->ternary_expression_type())->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) && !lvalue) {
					res = allocate_general_purpose_fp_register(mc);
					res->set_double_precision(static_pointer_cast<primitive_type>(te->ternary_expression_type())->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE);
					res->set_single_precision(static_pointer_cast<primitive_type>(te->ternary_expression_type())->primitive_type_kind() == primitive_type::kind::KIND_FLOAT);
				}
				else
					res = allocate_general_purpose_register(mc);
				shared_ptr<binary_expression> cexpr = te->condition();
				shared_ptr<expression> texpr = te->true_path();
				shared_ptr<ternary_expression> fexpr = te->false_path();
				shared_ptr<operand> cond_op = generate_binary_expression_mips(mc, cexpr, false);
				shared_ptr<operand> cond_res = load_value_into_register(mc, cond_op, cexpr->binary_expression_type());
				pair<int, shared_ptr<operand>> lab_false = mc->next_label(), lab_done = mc->next_label();
				if (cond_res->register_number() > 31)
					free_general_purpose_fp_register(mc, cond_res);
				else
					free_general_purpose_register(mc, cond_res);
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_BEQ, cond_res, register_file2::_zero_register, get<1>(lab_false)));
				shared_ptr<operand> t = generate_expression_mips(mc, texpr, lvalue);
				if (lvalue) {
					if (t->operand_kind() != operand::kind::KIND_MEMORY && t->operand_kind() != operand::kind::KIND_LABEL && t->operand_kind() != operand::kind::KIND_REGISTER) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					if (t->operand_kind() == operand::kind::KIND_MEMORY) {
						operand::offset_kind ok = t->operand_offset_kind();
						shared_ptr<operand> to = make_shared<operand>(t->offset());
						to->set_operand_offset_kind(ok);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, res, register_file2::int_2_register_object.at(t->register_number()), to));
						free_general_purpose_register(mc, register_file2::int_2_register_object.at(t->register_number()));
					}
					else if (t->operand_kind() == operand::kind::KIND_REGISTER) {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, res, register_file2::_zero_register, t));
						free_general_purpose_register(mc, t);
					}
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, res, t));
				}
				else {
					t = load_value_into_register(mc, t, texpr->expression_type());
					t = cast_between_types(mc, t, texpr->expression_type(), te->ternary_expression_type());
					if (t->register_number() > 31 && res->register_number() > 31 && t->single_precision() == res->single_precision() && t->double_precision() == res->double_precision()) {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(res->double_precision() ? insn::kind::KIND_MOV_D : insn::kind::KIND_MOV_S, res, t));
						free_general_purpose_fp_register(mc, t);
					}
					else if (t->register_number() <= 31 && res->register_number() <= 31) {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, res, register_file2::_zero_register, t));
						free_general_purpose_register(mc, t);
					}
					else {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
				}
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_J, get<1>(lab_done)));
				mc->current_frame()->add_insn_to_body(make_shared<insn>(get<1>(lab_false)));
				shared_ptr<operand> f = generate_ternary_expression_mips(mc, fexpr, lvalue);
				if (lvalue) {
					if (f->operand_kind() != operand::kind::KIND_MEMORY && f->operand_kind() != operand::kind::KIND_LABEL && f->operand_kind() != operand::kind::KIND_REGISTER) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					if (f->operand_kind() == operand::kind::KIND_MEMORY) {
						operand::offset_kind ok = f->operand_offset_kind();
						shared_ptr<operand> fo = make_shared<operand>(f->offset());
						fo->set_operand_offset_kind(ok);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, res, register_file2::int_2_register_object.at(f->register_number()), fo));
						free_general_purpose_register(mc, register_file2::int_2_register_object.at(f->register_number()));
					}
					else if (f->operand_kind() == operand::kind::KIND_REGISTER) {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, res, register_file2::_zero_register, f));
						free_general_purpose_register(mc, f);
					}
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, res, f));
				}
				else {
					f = load_value_into_register(mc, f, fexpr->ternary_expression_type());
					f = cast_between_types(mc, f, fexpr->ternary_expression_type(), te->ternary_expression_type());
					if (f->register_number() > 31 && res->register_number() > 31 && f->single_precision() == res->single_precision() && f->double_precision() == res->double_precision()) {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(res->double_precision() ? insn::kind::KIND_MOV_D : insn::kind::KIND_MOV_S, res, f));
						free_general_purpose_fp_register(mc, f);
					}
					else if (f->register_number() <= 31 && res->register_number() <= 31) {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, res, register_file2::_zero_register, f));
						free_general_purpose_register(mc, f);
					}
					else {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
				}
				mc->current_frame()->add_insn_to_body(make_shared<insn>(get<1>(lab_done)));
				if (lvalue)
					return make_shared<operand>(operand::offset_kind::KIND_TRUE, 0, res->register_number(), res->register_name());
				else
					return res;
			}
				break;
			case ternary_expression::kind::KIND_BINARY:
				return generate_binary_expression_mips(mc, te->condition(), lvalue);
				break;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand> generate_binary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<binary_expression> be, bool lvalue) {
			if (be == nullptr) return nullptr;
			switch (be->binary_expression_kind()) {
			case binary_expression::kind::KIND_BINARY_EXPRESSION: {
				switch (be->binary_expression_operator_kind()) {
				case binary_expression::operator_kind::KIND_SUBTRACT:
				case binary_expression::operator_kind::KIND_BITWISE_XOR:
				case binary_expression::operator_kind::KIND_SHIFT_LEFT:
				case binary_expression::operator_kind::KIND_SHIFT_RIGHT:
				case binary_expression::operator_kind::KIND_BITWISE_AND:
				case binary_expression::operator_kind::KIND_BITWISE_OR:
				case binary_expression::operator_kind::KIND_LOGICAL_AND:
				case binary_expression::operator_kind::KIND_LOGICAL_OR:
				case binary_expression::operator_kind::KIND_ADD: {
					shared_ptr<operand> o1 = generate_binary_expression_mips(mc, be->lhs(), false), o2 = generate_binary_expression_mips(mc, be->rhs(), false);
					return binary_expression_helper_1(mc, o1, be->binary_expression_operator_kind(), o2, be->binary_expression_type(), false);
				}
					break;
				case binary_expression::operator_kind::KIND_MODULUS:
				case binary_expression::operator_kind::KIND_MULTIPLY:
				case binary_expression::operator_kind::KIND_DIVIDE:
				case binary_expression::operator_kind::KIND_EQUALS_EQUALS:
				case binary_expression::operator_kind::KIND_NOT_EQUALS:
				case binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO:
				case binary_expression::operator_kind::KIND_GREATER_THAN:
				case binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO:
				case binary_expression::operator_kind::KIND_LESS_THAN: {
					shared_ptr<operand> o1 = generate_binary_expression_mips(mc, be->lhs(), false), o2 = generate_binary_expression_mips(mc, be->rhs(), false);
					if ((be->lhs()->binary_expression_type()->type_kind() == type::kind::KIND_PRIMITIVE && be->lhs()->binary_expression_type()->type_array_kind() != type::array_kind::KIND_ARRAY) &&
						(be->rhs()->binary_expression_type()->type_kind() == type::kind::KIND_PRIMITIVE && be->rhs()->binary_expression_type()->type_array_kind() != type::array_kind::KIND_ARRAY)) {
						shared_ptr<primitive_type> pt_lhs = static_pointer_cast<primitive_type>(be->lhs()->binary_expression_type()),
							pt_rhs = static_pointer_cast<primitive_type>(be->rhs()->binary_expression_type());
						shared_ptr<type> d = nullptr;
						int sz_lhs = calculate_type_size(mc, pt_lhs), sz_rhs = calculate_type_size(mc, pt_rhs);
						if (sz_lhs > sz_rhs)
							d = pt_rhs->primitive_type_kind() == primitive_type::kind::KIND_FLOAT && pt_lhs->primitive_type_kind() != primitive_type::kind::KIND_DOUBLE ? pt_rhs : pt_lhs;
						else
							d = pt_lhs->primitive_type_kind() == primitive_type::kind::KIND_FLOAT && pt_rhs->primitive_type_kind() != primitive_type::kind::KIND_DOUBLE ? pt_lhs : pt_rhs;
						return binary_expression_helper_1(mc, o1, be->binary_expression_operator_kind(), o2, d, pt_lhs->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED ||
							pt_rhs->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED);
					}
					else
						return binary_expression_helper_1(mc, o1, be->binary_expression_operator_kind(), o2, be->lhs()->binary_expression_type(), false);
				}
					break;
				default:
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
					break;
				}
			}
				break;
			case binary_expression::kind::KIND_UNARY_EXPRESSION:
				return generate_unary_expression_mips(mc, be->single_lhs(), lvalue);
				break;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand> generate_unary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<unary_expression> ue, bool lvalue) {
			if (ue == nullptr) return nullptr;
			bool need_lvalue = ue->unary_expression_kind_list().size() > 0 && (ue->unary_expression_kind_list()[0] == unary_expression::kind::KIND_INCREMENT ||
				ue->unary_expression_kind_list()[0] == unary_expression::kind::KIND_DECREMENT);
			shared_ptr<operand> op = generate_postfix_expression_mips(mc, ue->contained_postfix_expression(), lvalue || need_lvalue);
			for (unary_expression::kind uek : ue->unary_expression_kind_list()) {
				switch (uek) {
				case unary_expression::kind::KIND_INCREMENT:
				case unary_expression::kind::KIND_DECREMENT: {
					bool inc = uek == unary_expression::kind::KIND_INCREMENT;
					shared_ptr<type> t = ue->unary_expression_type();
					shared_ptr<operand> n = nullptr;
					if(op->operand_kind() != operand::kind::KIND_REGISTER)
						n = load_value_into_register(mc, op, t);
					else {
						if (op->register_number() > 31) {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return nullptr;
						}
						n = allocate_general_purpose_register(mc);
						insn::kind l = load_store_insn_from_type(mc, t, true);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(l, n, make_shared<operand>(operand::offset_kind::KIND_TRUE, 0, op->register_number(), op->register_name())));
						free_general_purpose_register(mc, op);
					}
					if (t->type_kind() != type::kind::KIND_PRIMITIVE && op->operand_kind() != operand::kind::KIND_MEMORY && op->operand_kind() != operand::kind::KIND_LABEL) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					shared_ptr<primitive_type> prim = static_pointer_cast<primitive_type>(t);
					insn::kind st = load_store_insn_from_type(mc, t, false);
					if (prim->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
						shared_ptr<operand> d1 = allocate_general_purpose_fp_register(mc);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LDC1, d1, make_shared<operand>(false, inc ? "__dp_1" : "__dp_m1")));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADD_D, n, n, d1));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(st, n, op));
						free_general_purpose_fp_register(mc, d1);
						n->set_double_precision(true);
						n->set_single_precision(false);
					}
					else if (prim->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
						shared_ptr<operand> f1 = allocate_general_purpose_fp_register(mc);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LWC1, f1, make_shared<operand>(false, inc ? "__fp_1" : "__fp_m1")));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADD_S, n, n, f1));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(st, n, op));
						free_general_purpose_fp_register(mc, f1);
						n->set_single_precision(true);
						n->set_double_precision(false);
					}
					else {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, n, n, make_shared<operand>(inc ? 1 : -1)));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(st, n, op));
					}
					op = n;
				}
					break;
				case unary_expression::kind::KIND_PLUS:
					break;
				case unary_expression::kind::KIND_MINUS: {
					if (op->is_immediate()) {
						shared_ptr<operand> temp_reg = allocate_general_purpose_register(mc);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, temp_reg, register_file2::_zero_register, op));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SUBU, temp_reg, register_file2::_zero_register, temp_reg));
						op = temp_reg;
					}
					else {
						if (op->operand_kind() != operand::kind::KIND_REGISTER)
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						if (ue->unary_expression_type()->type_kind() != type::kind::KIND_PRIMITIVE && ue->unary_expression_type()->type_array_kind() != type::array_kind::KIND_NON_ARRAY) {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return nullptr;
						}
						shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(ue->unary_expression_type());
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
							if (!op->single_precision())
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_NEG_S, op, op));
							op->set_single_precision(true);
							op->set_double_precision(false);
						}
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
							if (!op->double_precision())
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_NEG_D, op, op));
							op->set_double_precision(true);
							op->set_single_precision(false);
						}
						else
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SUBU, op, register_file2::_zero_register, op));
					}
				}
					break;
				case unary_expression::kind::KIND_BITWISE_NOT: {
					op = load_value_into_register(mc, op, ue->unary_expression_type());
					if (op->register_number() > 31) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					shared_ptr<operand> temp = allocate_general_purpose_register(mc);
					pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, op, temp);
					op = p.first, temp = p.second;
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, temp, register_file2::_zero_register, make_shared<operand>(-1)));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_XOR, op, op, temp));
					free_general_purpose_register(mc, temp);
				}
					break;
				case unary_expression::kind::KIND_LOGICAL_NOT: {
					op = load_value_into_register(mc, op, ue->unary_expression_type());
					if (op->register_number() > 31) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SEQ, op, op, register_file2::_zero_register));
				}
					break;
				default:
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
					break;
				}
			}
			return op;
		}

		shared_ptr<operand> generate_postfix_expression_mips(shared_ptr<mips_code> mc, shared_ptr<postfix_expression> pe, bool lvalue) {
			if (pe == nullptr) return nullptr;
			bool pe_requires_lvalue = lvalue || (pe->postfix_type_list().size() > 0 &&
				(pe->postfix_type_list()[0]->postfix_type_kind() == postfix_expression::kind::KIND_INCREMENT ||
					pe->postfix_type_list()[0]->postfix_type_kind() == postfix_expression::kind::KIND_DECREMENT ||
					pe->postfix_type_list()[0]->postfix_type_kind() == postfix_expression::kind::KIND_ADDRESS));
			shared_ptr<operand> op = generate_primary_expression_mips(mc, pe->contained_primary_expression(), pe_requires_lvalue);
			bool already_loaded;
			if (pe->contained_primary_expression()->primary_expression_kind() == primary_expression::kind::KIND_PARENTHESIZED_EXPRESSION) {
				if (!pe_requires_lvalue)
					op = load_value_into_register(mc, op, pe->contained_primary_expression()->primary_expression_type());
				already_loaded = !pe_requires_lvalue;
			}
			else
				already_loaded = pe->contained_primary_expression()->primary_expression_value_kind() == value_kind::VALUE_RVALUE;
			shared_ptr<type> prev_type = pe->contained_primary_expression()->primary_expression_type();
			for (shared_ptr<postfix_expression::postfix_type> pt : pe->postfix_type_list()) {
				switch (pt->postfix_type_kind()) {
				case postfix_expression::kind::KIND_DECREMENT:
				case postfix_expression::kind::KIND_INCREMENT: {
					bool inc = pt->postfix_type_kind() == postfix_expression::kind::KIND_INCREMENT;
					shared_ptr<operand> n = nullptr;
					if(op->operand_kind() != operand::kind::KIND_REGISTER)
						n = load_value_into_register(mc, op, pt->postfix_type_type());
					else {
						if (n->register_number() > 31) {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return nullptr;
						}
						if (!already_loaded) {
							shared_ptr<operand> temp = allocate_general_purpose_register(mc);
							insn::kind l = load_store_insn_from_type(mc, pt->postfix_type_type(), true);
							mc->current_frame()->add_insn_to_body(make_shared<insn>(l, temp, make_shared<operand>(operand::offset_kind::KIND_TRUE, 0, n->register_number(), n->register_name())));
							free_general_purpose_register(mc, n);
							n = temp;
						}
					}
					shared_ptr<type> t = pt->postfix_type_type();
					if (t->type_kind() != type::kind::KIND_PRIMITIVE && op->operand_kind() != operand::kind::KIND_MEMORY && op->operand_kind() != operand::kind::KIND_LABEL) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					shared_ptr<primitive_type> prim = static_pointer_cast<primitive_type>(t);
					insn::kind st = load_store_insn_from_type(mc, pt->postfix_type_type(), false);
					if (prim->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
						shared_ptr<operand> d1 = allocate_general_purpose_fp_register(mc);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LDC1, d1, make_shared<operand>(false, inc ? "__dp_1" : "__dp_m1")));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADD_D, n, n, d1));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(st, n, op));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_NEG_D, d1, d1));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADD_D, n, n, d1));
						free_general_purpose_fp_register(mc, d1);
						n->set_double_precision(true);
						n->set_single_precision(false);
					}
					else if (prim->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
						shared_ptr<operand> f1 = allocate_general_purpose_fp_register(mc);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LWC1, f1, make_shared<operand>(false, inc ? "__fp_1" : "__fp_m1")));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADD_S, n, n, f1));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(st, n, op));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_NEG_S, f1, f1));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADD_S, n, n, f1));
						free_general_purpose_fp_register(mc, f1);
						n->set_double_precision(false);
						n->set_single_precision(true);
					}
					else {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, n, n, make_shared<operand>(inc ? 1 : -1)));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(st, n, op));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, n, n, make_shared<operand>(inc ? -1 : 1)));
					}
					op = n;
					already_loaded = true;
				}
					break;
				case postfix_expression::kind::KIND_AS: {
					shared_ptr<type> from_type = prev_type, to_type = pt->postfix_type_type();
					if (!already_loaded) {
						if (op->operand_kind() == operand::kind::KIND_REGISTER)
							op = make_shared<operand>(operand::offset_kind::KIND_TRUE, 0, op->register_number(), op->register_name());
					}
					else {
						if (op->operand_kind() == operand::kind::KIND_LABEL) {
							shared_ptr<operand> reg = allocate_general_purpose_register(mc);
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg, op));
							op = reg;
						}
						else if (op->operand_kind() == operand::kind::KIND_MEMORY) {
							shared_ptr<operand> reg = allocate_general_purpose_register(mc);
							shared_ptr<operand> imm = make_shared<operand>(op->offset());
							imm->set_operand_offset_kind(op->operand_offset_kind());
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, reg, register_file2::int_2_register_object.at(op->register_number()), imm));
							free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
							op = reg;
						}
					}
					op = load_value_into_register(mc, op, from_type);
					if (from_type->type_kind() == type::kind::KIND_PRIMITIVE && to_type->type_kind() == type::kind::KIND_PRIMITIVE &&
						from_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY && to_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
						op = cast_between_types(mc, op, from_type, to_type);
						if (static_pointer_cast<primitive_type>(to_type)->primitive_type_kind() == primitive_type::kind::KIND_BOOL)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SNE, op, op, register_file2::_zero_register));
					}
					else if ((from_type->type_kind() == type::kind::KIND_PRIMITIVE && from_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY &&
						to_type->type_array_kind() == type::array_kind::KIND_ARRAY))
						op = cast_between_types(mc, op, from_type, make_shared<primitive_type>(primitive_type::kind::KIND_INT, primitive_type::const_kind::KIND_CONST,
							primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_UNSIGNED));
					already_loaded = true;
				}
					break;
				case postfix_expression::kind::KIND_SUBSCRIPT: {
					shared_ptr<type> contained_type = pt->postfix_type_type();
					int sz = (int)calculate_type_size(mc, contained_type), adjusted_sz = sz;
					shared_ptr<operand> e = generate_expression_mips(mc, pt->subscript(), false);
					bool is_int_imm = e->operand_kind() == operand::kind::KIND_INT_IMMEDIATE;
					bool is_pow_2 = (adjusted_sz & (adjusted_sz - 1)) == 0 && adjusted_sz != 0;
					int l2 = adjusted_sz == 0 ? 0 : log2(adjusted_sz);
					if (e->operand_kind() != operand::kind::KIND_INT_IMMEDIATE)
						e = load_value_into_register(mc, e, pt->subscript()->expression_type());
					if (e->operand_kind() != operand::kind::KIND_REGISTER && e->operand_kind() != operand::kind::KIND_INT_IMMEDIATE) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					if (op->operand_kind() == operand::kind::KIND_MEMORY) {
						shared_ptr<operand> reg = register_file2::int_2_register_object.at(op->register_number());
						shared_ptr<operand> addr = allocate_general_purpose_register(mc);
						if (!already_loaded)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, addr, op));
						else {
							shared_ptr<operand> temp_imm = make_shared<operand>(op->offset());
							temp_imm->set_operand_offset_kind(op->operand_offset_kind());
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, addr, reg, temp_imm));
						}
						free_general_purpose_register(mc, reg);
						if (is_int_imm) {
							int offs = e->int_immediate() * adjusted_sz;
							op = make_shared<operand>(operand::offset_kind::KIND_TRUE, offs, addr->register_number(), addr->register_name());
						}
						else {
							if (e->operand_kind() != operand::kind::KIND_REGISTER) {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return nullptr;
							}
							if (is_pow_2) {
								if(l2 != 0)
									mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SLL, e, e, make_shared<operand>(l2)));
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, addr, addr, e));
							}
							else {
								shared_ptr<operand> t = allocate_general_purpose_register(mc);
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, t, register_file2::_zero_register, make_shared<operand>(adjusted_sz)));
#if SYSTEM == 0 || SYSTEM == 1
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MULTU, e, t));
								free_general_purpose_register(mc, t);
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MFLO, e));
#elif SYSTEM == 2
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MULTU, e, e, t));
								free_general_purpose_register(mc, t);
#endif
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, addr, addr, e));
							}
							free_general_purpose_register(mc, reg);
							op = make_shared<operand>(operand::offset_kind::KIND_TRUE, 0, addr->register_number(), addr->register_name());
						}
					}
					else if (op->operand_kind() == operand::kind::KIND_LABEL) {
						shared_ptr<operand> temp = allocate_general_purpose_register(mc);
						if (!already_loaded)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, temp, op));
						else
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, temp, op));
						if (is_int_imm) {
							int offs = e->int_immediate() * adjusted_sz;
							op = make_shared<operand>(operand::offset_kind::KIND_TRUE, offs, temp->register_number(), temp->register_name());
						}
						else {
							if (e->operand_kind() != operand::kind::KIND_REGISTER) {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return nullptr;
							}
							if (is_pow_2) {
								if(l2 != 0)
									mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SLL, e, e, make_shared<operand>(l2)));
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, temp, temp, e));
							}
							else {
								shared_ptr<operand> t = allocate_general_purpose_register(mc);
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, t, register_file2::_zero_register, make_shared<operand>(adjusted_sz)));
#if SYSTEM == 0 || SYSTEM == 1
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MULTU, e, t));
								free_general_purpose_register(mc, t);
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MFLO, e));
#elif SYSTEM == 2
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MULTU, e, e, t));
								free_general_purpose_register(mc, t);
#endif
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, temp, temp, e));
							}
							op = make_shared<operand>(operand::offset_kind::KIND_TRUE, 0, temp->register_number(), temp->register_name());
						}
					}
					else if (op->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<operand> reg = register_file2::int_2_register_object.at(op->register_number()), addr = allocate_general_purpose_register(mc);
						if (!already_loaded)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, addr, make_shared<operand>(operand::offset_kind::KIND_TRUE, 0, reg->register_number(),
								reg->register_name())));
						else
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, addr, reg, register_file2::_zero_register));
						free_general_purpose_register(mc, reg);
						if (is_int_imm) {
							int offs = e->int_immediate() * adjusted_sz;
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, addr, addr, make_shared<operand>(offs)));
							op = make_shared<operand>(operand::offset_kind::KIND_TRUE, 0, addr->register_number(), addr->register_name());
						}
						else {
							if (e->operand_kind() != operand::kind::KIND_REGISTER) {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return nullptr;
							}
							if (is_pow_2) {
								if(l2 != 0)
									mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SLL, e, e, make_shared<operand>(l2)));
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, addr, addr, e));
							}
							else {
								shared_ptr<operand> t = allocate_general_purpose_register(mc);
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, t, register_file2::_zero_register, make_shared<operand>(adjusted_sz)));
#if SYSTEM == 0 || SYSTEM == 1
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MULTU, e, t));
								free_general_purpose_register(mc, t);
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MFLO, e));
#elif SYSTEM == 2
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MULTU, e, e, t));
								free_general_purpose_register(mc, t);
#endif
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, addr, addr, e));
							}
							op = make_shared<operand>(operand::offset_kind::KIND_TRUE, 0, addr->register_number(), addr->register_name());
						}
					}
					else {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					if (e->register_number() > 31)
						free_general_purpose_fp_register(mc, register_file2::int_2_register_object.at(e->register_number()));
					else
						free_general_purpose_register(mc, register_file2::int_2_register_object.at(e->register_number()));
					already_loaded = false;
				}
					break;
				case postfix_expression::kind::KIND_ADDRESS: {
					if (op->operand_kind() == operand::kind::KIND_LABEL) {
						shared_ptr<operand> reg = allocate_general_purpose_register(mc);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg, op));
						op = reg;
					}
					else if (op->operand_kind() == operand::kind::KIND_MEMORY) {
						shared_ptr<operand> n = allocate_general_purpose_register(mc);
						shared_ptr<operand> reg = register_file2::int_2_register_object.at(op->register_number());
						shared_ptr<operand> imm = make_shared<operand>(op->offset());
						imm->set_operand_offset_kind(op->operand_offset_kind());
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, n, reg, imm));
						free_general_purpose_register(mc, reg);
						op = n;
					}
					else if (op->operand_kind() == operand::kind::KIND_REGISTER);
					else {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					already_loaded = true;
				}
					break;
				case postfix_expression::kind::KIND_MEMBER:
				case postfix_expression::kind::KIND_ARROW:
				case postfix_expression::kind::KIND_AT: {
					bool arrow_at = pt->postfix_type_kind() != postfix_expression::kind::KIND_MEMBER;
					bool arrow = pt->postfix_type_kind() == postfix_expression::kind::KIND_ARROW;
					bool member_arrow = pt->postfix_type_kind() != postfix_expression::kind::KIND_AT;
					if (arrow_at) {
						if (!already_loaded) {
							if (op->operand_kind() == operand::kind::KIND_LABEL) {
								shared_ptr<operand> reg = allocate_general_purpose_register(mc);
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, reg, op));
								op = reg;
							}
							else if (op->operand_kind() == operand::kind::KIND_MEMORY) {
								shared_ptr<operand> reg = register_file2::int_2_register_object.at(op->register_number());
								shared_ptr<operand> reg2 = allocate_general_purpose_register(mc);
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, reg2, op));
								free_general_purpose_register(mc, reg);
								op = reg2;
							}
							else if (op->operand_kind() == operand::kind::KIND_REGISTER) {
								shared_ptr<operand> reg = register_file2::int_2_register_object.at(op->register_number());
								shared_ptr<operand> reg2 = allocate_general_purpose_register(mc);
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, reg2, make_shared<operand>(operand::offset_kind::KIND_TRUE, 0, op->register_number(),
									op->register_name())));
								free_general_purpose_register(mc, reg);
								op = reg2;
							}
							else {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return nullptr;
							}
							if (op->operand_kind() != operand::kind::KIND_REGISTER) {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return nullptr;
							}
							op = make_shared<operand>(operand::offset_kind::KIND_TRUE, 0, op->register_number(), op->register_name());
						}
						else {
							if (op->operand_kind() == operand::kind::KIND_REGISTER)
								op = make_shared<operand>(operand::offset_kind::KIND_TRUE, 0, op->register_number(), op->register_name());
						}
					}
					if (member_arrow) {
						token mem = pt->member();
						if (prev_type->type_kind() != type::kind::KIND_STRUCT) {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return nullptr;
						}
						shared_ptr<struct_type> st = static_pointer_cast<struct_type>(prev_type);
						shared_ptr<struct_symbol> ss = mc->find_struct_symbol(st->struct_name(), st->struct_reference_number());
						shared_ptr<scope> s_scope = ss->struct_symbol_scope();
						int off = 0;
						if (!st->is_union()) {
							for (shared_ptr<symbol> sym : s_scope->symbol_list()) {
								if (sym->symbol_kind() == symbol::kind::KIND_VARIABLE) {
									shared_ptr<variable_symbol> vsym = static_pointer_cast<variable_symbol>(sym);
									if (vsym->variable_name().raw_text() != mem.raw_text()) {
										shared_ptr<type> vt = vsym->variable_type();
										int vt_sz = (int)calculate_type_size(mc, vt);
										off += vt_sz % 8 == 0 ? vt_sz : (vt_sz / 8 + 1) * 8;
									}
									else
										break;
								}
								else {
									mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									return nullptr;
								}
							}
						}
						if (op->operand_kind() == operand::kind::KIND_LABEL) {
							shared_ptr<operand> reg = allocate_general_purpose_register(mc);
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg, op));
							op = make_shared<operand>(operand::offset_kind::KIND_TRUE, off, reg->register_number(), reg->register_name());
						}
						else if (op->operand_kind() == operand::kind::KIND_MEMORY)
							op = make_shared<operand>(op->operand_offset_kind(), op->offset() + off, op->register_number(), op->register_name());
						else if (op->operand_kind() == operand::kind::KIND_REGISTER)
							op = make_shared<operand>(op->operand_offset_kind(), off, op->register_number(), op->register_name());
						else {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return nullptr;
						}
					}
					already_loaded = false;
				}
					break;
				case postfix_expression::kind::KIND_FUNCTION_CALL: {
					shared_ptr<operand> fp = register_file2::_fp_register, a0 = register_file2::_a0_register;
#if SYSTEM == 0 || SYSTEM == 1
					shared_ptr<operand> f12 = register_file2::_f12_register;
#elif SYSTEM == 2
					shared_ptr<operand> f10 = register_file2::_f10_register;
#endif
					shared_ptr<function_type> ftype = static_pointer_cast<function_type>(pt->function_type());
					shared_ptr<type> ret_type = ftype->return_type();
					vector<shared_ptr<variable_declaration>> vdecl_list = ftype->parameter_list();
					vector<shared_ptr<assignment_expression>> ae_list = pt->argument_list();
					if ((!ftype->variadic() && vdecl_list.size() != ae_list.size()) || (ftype->variadic() && ae_list.size() < vdecl_list.size())) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					int sz_to_offset = 0;
					tuple<vector<int>, vector<int>, int> res_s_tup = save_to_middle(mc);
					vector<int> which_to_store = get<0>(res_s_tup), middle_offsets = get<1>(res_s_tup);
					vector<int> register_args, fp_register_args;
					for (int i = 0; i < ae_list.size(); i++) {
						shared_ptr<type> vtype = nullptr;
						if (!ftype->variadic())
							vtype = vdecl_list[i]->variable_declaration_type();
						else {
							if (i < vdecl_list.size())
								vtype = vdecl_list[i]->variable_declaration_type();
							else
								vtype = ae_list[i]->assignment_expression_type();
						}
						bool is_prim = vtype->type_kind() == type::kind::KIND_PRIMITIVE && vtype->type_array_kind() == type::array_kind::KIND_NON_ARRAY;
						bool is_not_struct = !(vtype->type_kind() == type::kind::KIND_STRUCT && vtype->type_array_kind() == type::array_kind::KIND_NON_ARRAY);
						bool is_fp = static_pointer_cast<primitive_type>(vtype)->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE ||
							static_pointer_cast<primitive_type>(vtype)->primitive_type_kind() == primitive_type::kind::KIND_FLOAT;
#if SYSTEM == 0 || SYSTEM == 1
						if (fp_register_args.size() < 2 && is_prim && is_fp && !ftype->variadic())
							fp_register_args.push_back(i);
#elif SYSTEM == 2
						if (fp_register_args.size() < 8 && is_prim && is_fp && !ftype->variadic())
							fp_register_args.push_back(i);
#endif
#if SYSTEM == 0 || SYSTEM == 1
						else if (register_args.size() < 4 && is_not_struct && !(is_fp && is_prim) && !ftype->variadic())
#elif SYSTEM == 2
						else if (register_args.size() < 8 && is_not_struct && !(is_fp && is_prim) && !ftype->variadic())
#endif
							register_args.push_back(i);
						else {
							int sz = (int)calculate_type_size(mc, vtype);
							sz = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
							sz_to_offset += sz;
						}
					}
					if (sz_to_offset > mc->current_frame()->frame_size())
						mc->current_frame()->set_frame_size(sz_to_offset);
					int curr_off = 0;
					std::vector<std::tuple<int, int, std::vector<int>>> move_from_middle;
					for (int i = 0; i < ae_list.size(); i++) {
						int fp_index = find(fp_register_args.begin(), fp_register_args.end(), i) - fp_register_args.begin(),
							reg_index = find(register_args.begin(), register_args.end(), i) - register_args.begin();
						shared_ptr<assignment_expression> ae = ae_list[i];
						shared_ptr<type> arg_type = ae->assignment_expression_type();
						shared_ptr<type> parm_type = nullptr;
						if (!ftype->variadic()) {
							shared_ptr<variable_declaration> vdecl = vdecl_list[i];
							parm_type = vdecl->variable_declaration_type();
						}
						else {
							if (i < vdecl_list.size())
								parm_type = vdecl_list[i]->variable_declaration_type();
							else
								parm_type = ae->assignment_expression_type();
						}
						shared_ptr<operand> evaluated_arg = generate_assignment_expression_mips(mc, ae, false);
						evaluated_arg = load_value_into_register(mc, evaluated_arg, arg_type);
						evaluated_arg = cast_between_types(mc, evaluated_arg, arg_type, parm_type);
						if (fp_index < fp_register_args.size()) {
							if (ftype->variadic()) {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return nullptr;
							}
#if SYSTEM == 0 || SYSTEM == 1
							shared_ptr<operand> correct_reg = register_file2::int_2_register_object.at(f12->register_number() + fp_index * 2);
							mc->current_frame()->mark_register(f12->register_number() + fp_index * 2);
#elif SYSTEM == 2
							shared_ptr<operand> correct_reg = register_file2::int_2_register_object.at(f10->register_number() + fp_index);
							mc->current_frame()->mark_register(f10->register_number() + fp_index);
#endif
							if (parm_type->type_kind() != type::kind::KIND_PRIMITIVE && parm_type->type_array_kind() != type::array_kind::KIND_NON_ARRAY) {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return nullptr;
							}
							shared_ptr<primitive_type> parm_ptype = static_pointer_cast<primitive_type>(parm_type);
							if (parm_ptype->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MOV_D, correct_reg, evaluated_arg));
							else if (parm_ptype->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MOV_S, correct_reg, evaluated_arg));
							else {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return nullptr;
							}
						}
						else if (reg_index < register_args.size()) {
							if (ftype->variadic()) {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return nullptr;
							}
							shared_ptr<operand> correct_reg = register_file2::int_2_register_object.at(a0->register_number() + reg_index);
							mc->current_frame()->mark_register(a0->register_number() + reg_index);
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, correct_reg, evaluated_arg, register_file2::_zero_register));
						}
						else {
							int sz = (int)calculate_type_size(mc, parm_type);
							sz = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
							std::vector<int> middle_offsets;
							if (parm_type->type_kind() == type::kind::KIND_STRUCT && parm_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
								shared_ptr<operand> reg = allocate_general_purpose_register(mc);
								for (int o = 0; o < sz; o += 4) {
									int middle_offset = -(mc->current_frame()->middle_section_size() + 4);
									middle_offsets.push_back(middle_offset);
									mc->current_frame()->update_middle_section_size(4);
									mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, reg, make_shared<operand>(operand::offset_kind::KIND_TRUE, o,
										evaluated_arg->register_number(), evaluated_arg->register_name())));
									mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, reg, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
										middle_offset, fp->register_number(), fp->register_name())));
								}
								move_from_middle.push_back(make_tuple(curr_off, sz, middle_offsets));
								free_general_purpose_register(mc, reg);
							}
							else {
								insn::kind s = load_store_insn_from_type(mc, parm_type, false);
								int middle_offset = -(mc->current_frame()->middle_section_size() + 4);
								middle_offsets.push_back(middle_offset);
								mc->current_frame()->update_middle_section_size(4);
								int middle_offset2 = -(mc->current_frame()->middle_section_size() + 4);
								middle_offsets.push_back(middle_offset2);
								mc->current_frame()->update_middle_section_size(4);
								mc->current_frame()->add_insn_to_body(make_shared<insn>(s, evaluated_arg, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
									middle_offset2, fp->register_number(), fp->register_name())));
								std::reverse(middle_offsets.begin(), middle_offsets.end());
								move_from_middle.push_back(make_tuple(curr_off, sz, middle_offsets));
							}
							curr_off += sz;
						}
						if (evaluated_arg->register_number() > 31)
							free_general_purpose_fp_register(mc, evaluated_arg);
						else
							free_general_purpose_register(mc, evaluated_arg);
					}
					shared_ptr<operand> trans = allocate_general_purpose_register(mc);
					for (const std::tuple<int, int, std::vector<int>>& middle : move_from_middle) {
						int true_off = get<0>(middle), sz = get<1>(middle);
						std::vector<int> middle_offs = get<2>(middle);
						int curr = 0;
						for (int moff : middle_offs) {
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, trans, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
								moff, fp->register_number(), fp->register_name())));
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, trans, make_shared<operand>(operand::offset_kind::KIND_TRUE,
								true_off + curr, fp->register_number(), fp->register_name())));
							curr += 4;
						}
					}
					free_general_purpose_register(mc, trans);
					if (op->is_immediate()) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					if (op->operand_kind() == operand::kind::KIND_LABEL) {
						if(already_loaded)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_JAL, op));
						else {
							shared_ptr<operand> reg = allocate_general_purpose_register(mc);
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, reg, op));
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_JALR, reg));
							free_general_purpose_register(mc, reg);
						}
					}
					else if (op->operand_kind() == operand::kind::KIND_REGISTER) {
						if (already_loaded)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_JALR, op));
						else {
							shared_ptr<operand> reg = allocate_general_purpose_register(mc);
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, reg, make_shared<operand>(operand::offset_kind::KIND_TRUE, 0,
								op->register_number(), op->register_name())));
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_JALR, reg));
							free_general_purpose_register(mc, reg);
						}
					}
					else if (op->operand_kind() == operand::kind::KIND_MEMORY) {
						if (already_loaded) {
							shared_ptr<operand> reg = allocate_general_purpose_register(mc);
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, reg,
								register_file2::int_2_register_object.at(op->register_number()), make_shared<operand>(op->offset())));
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_JALR, reg));
							free_general_purpose_register(mc, reg);
						}
						else {
							shared_ptr<operand> reg = allocate_general_purpose_register(mc);
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, reg, op));
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_JALR, reg));
							free_general_purpose_register(mc, reg);
						}
					}
					else {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					int old_op_reg_num = op->register_number();
#if SYSTEM == 0 || SYSTEM == 1
					shared_ptr<operand> v0 = register_file2::_v0_register, f0 = register_file2::_f0_register;
#endif
					if (ret_type->type_kind() == type::kind::KIND_PRIMITIVE && ret_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
						shared_ptr<primitive_type> ret_ptype = static_pointer_cast<primitive_type>(ret_type);
						if (ret_ptype->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE || ret_ptype->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
							shared_ptr<operand> freg = allocate_general_purpose_fp_register(mc);
#if SYSTEM == 0 || SYSTEM == 1
							mc->current_frame()->add_insn_to_body(make_shared<insn>(ret_ptype->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE ? insn::kind::KIND_MOV_D :
								insn::kind::KIND_MOV_S, freg, f0));
#elif SYSTEM == 2
							mc->current_frame()->add_insn_to_body(make_shared<insn>(ret_ptype->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE ? insn::kind::KIND_MOV_D :
								insn::kind::KIND_MOV_S, freg, f10));
#endif
							op = freg;
							op->set_double_precision(ret_ptype->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE);
							op->set_single_precision(ret_ptype->primitive_type_kind() == primitive_type::kind::KIND_FLOAT);
						}
						else {
							shared_ptr<operand> reg = allocate_general_purpose_register(mc);
#if SYSTEM == 0 || SYSTEM == 1
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, reg, v0, register_file2::_zero_register));
#elif SYSTEM == 2
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, reg, a0, register_file2::_zero_register));
#endif
							op = reg;
						}
					}
					else {
						shared_ptr<operand> reg = allocate_general_purpose_register(mc);
#if SYSTEM == 0 || SYSTEM == 1
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, reg, v0, register_file2::_zero_register));
#elif SYSTEM == 2
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, reg, a0, register_file2::_zero_register));
#endif
						op = reg;
					}
					free_general_purpose_register(mc, register_file2::int_2_register_object.at(old_op_reg_num));
					restore_from_middle(mc, which_to_store, middle_offsets);
					already_loaded = true;
				}
					break;
				default:
					mc->report_internal("This is should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
					break;
				}
				prev_type = pt->postfix_type_type();
			}
			if (!lvalue && !already_loaded) {
				if (pe->postfix_expression_value_kind() == value_kind::VALUE_LVALUE) {
					if (op->is_immediate())
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<type> t = pe->postfix_expression_type();
					shared_ptr<operand> temp = nullptr;
					if (t->type_kind() == type::kind::KIND_PRIMITIVE && t->type_array_kind() != type::array_kind::KIND_ARRAY) {
						shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
						insn::kind ik = load_store_insn_from_type(mc, pt, true);
						auto which_free = pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE || pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT ?
							free_general_purpose_fp_register : free_general_purpose_register;
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE || pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
							temp = allocate_general_purpose_fp_register(mc);
							temp->set_single_precision(pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT);
							temp->set_double_precision(pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE);
						}
						else
							temp = allocate_general_purpose_register(mc);
						if (op->operand_kind() == operand::kind::KIND_LABEL)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(ik, temp, op));
						else if (op->operand_kind() == operand::kind::KIND_MEMORY) {
							pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, op, temp);
							op = p.first, temp = p.second;
							mc->current_frame()->add_insn_to_body(make_shared<insn>(ik, temp, op));
							free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
						}
						else if (op->operand_kind() == operand::kind::KIND_REGISTER) {
							pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, op, temp);
							op = p.first, temp = p.second;
							mc->current_frame()->add_insn_to_body(make_shared<insn>(ik, temp, make_shared<operand>(operand::offset_kind::KIND_TRUE, 0, op->register_number(), op->register_name())));
							free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
						}
						which_free(mc, op);
						op = temp;
					}
					else if (t->type_kind() == type::kind::KIND_STRUCT && t->type_array_kind() != type::array_kind::KIND_ARRAY) {
						shared_ptr<operand> temp = allocate_general_purpose_register(mc);
						if (op->register_number() > 31) {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return nullptr;
						}
						if (op->operand_kind() == operand::kind::KIND_LABEL)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, temp, op));
						else if (op->operand_kind() == operand::kind::KIND_MEMORY) {
							pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, op, temp);
							op = p.first, temp = p.second;
							shared_ptr<operand> imm = make_shared<operand>(op->offset());
							imm->set_operand_offset_kind(op->operand_offset_kind());
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, temp, register_file2::int_2_register_object.at(op->register_number()), imm));
							free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
						}
						op = temp;
					}
					else {
						shared_ptr<operand> temp = allocate_general_purpose_register(mc);
						if (op->register_number() > 31) {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return nullptr;
						}
						if (op->operand_kind() == operand::kind::KIND_LABEL)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, temp, op));
						else if (op->operand_kind() == operand::kind::KIND_MEMORY) {
							pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, op, temp);
							op = p.first, temp = p.second;
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, temp, op));
							free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
						}
						else if (op->operand_kind() == operand::kind::KIND_REGISTER) {
							pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, op, temp);
							op = p.first, temp = p.second;
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, temp, make_shared<operand>(operand::offset_kind::KIND_TRUE, 0,
								op->register_number(), op->register_name())));
							free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
						}
						op = temp;
					}
				}
				else {
					if (!op->is_immediate())
						op = load_value_into_register(mc, op, pe->postfix_expression_type());
				}
			}
			else if (!lvalue && already_loaded) {
				if (op->operand_kind() == operand::kind::KIND_MEMORY) {
					shared_ptr<operand> temp = allocate_general_purpose_register(mc);
					shared_ptr<operand> imm = make_shared<operand>(op->offset());
					imm->set_operand_offset_kind(op->operand_offset_kind());
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, temp, register_file2::int_2_register_object.at(op->register_number()), imm));
					free_general_purpose_register(mc, register_file2::int_2_register_object.at(op->register_number()));
					op = temp;
					if (pe->postfix_expression_type()->type_kind() == type::kind::KIND_PRIMITIVE && pe->postfix_expression_type()->type_array_kind() != type::array_kind::KIND_ARRAY) {
						shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(pe->postfix_expression_type());
						op->set_double_precision(pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE);
						op->set_single_precision(pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT);
					}
				}
				op = load_value_into_register(mc, op, pe->postfix_expression_type());
			}
			return op;
		}

		shared_ptr<operand> literal_token_2_operand(shared_ptr<mips_code> mc, shared_ptr<primary_expression> pe) {
			if (pe == nullptr || pe->primary_expression_kind() != primary_expression::kind::KIND_LITERAL) return nullptr;
			token lit = pe->literal_token();
			string raw_lit = lit.raw_text();
			switch (lit.token_kind()) {
			case token::kind::TOKEN_INTEGER: {
				raw_lit.erase(remove(raw_lit.begin(), raw_lit.end(), '\''), raw_lit.end());
				int base = 10;
				switch (lit.prefix_kind()) {
				case token::prefix::PREFIX_BINARY:
					base = 2;
					raw_lit = raw_lit.substr(2);
					break;
				case token::prefix::PREFIX_HEXADECIMAL:
					base = 16;
					raw_lit = raw_lit.substr(2);
					break;
				case token::prefix::PREFIX_OCTAL:
					base = 8;
					raw_lit = raw_lit.substr(2);
					break;
				}
				int val = stoul(raw_lit.c_str(), nullptr, base);
				shared_ptr<primitive_type> ipt = make_shared<primitive_type>(primitive_type::kind::KIND_INT, primitive_type::const_kind::KIND_CONST,
					primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_SIGNED);
				if (val <= INT16_MAX && val >= INT16_MIN) {
					shared_ptr<operand> imm = make_shared<operand>(val);
					if (lit.suffix_kind() == token::suffix::SUFFIX_DOUBLE || lit.suffix_kind() == token::suffix::SUFFIX_FLOAT) {
						shared_ptr<operand> src = allocate_general_purpose_register(mc);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, src, register_file2::_zero_register, imm));
						return cast_between_types(mc, src, ipt, pe->primary_expression_type());
					}
					else
						return imm;
				}
				else {
					shared_ptr<operand> reg = allocate_general_purpose_register(mc);
#if SYSTEM == 0 || SYSTEM == 1
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LUI, reg, make_shared<operand>((unsigned) ((val >> 16) & 0xffff))));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ORI, reg, reg, make_shared<operand>(val & 0xffff)));
#elif SYSTEM == 2
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LUI, reg, make_shared<operand>((unsigned) ((val >> 12) & 0xfffff))));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ORI, reg, reg, make_shared<operand>(val & 0xfff)));
#endif
					return cast_between_types(mc, reg, ipt, pe->primary_expression_type());
				}
			}
				break;
			case token::kind::TOKEN_CHARACTER:
				return make_shared<operand>(true, raw_lit);
				break;
			case token::kind::TOKEN_DECIMAL: {
				raw_lit.erase(remove(raw_lit.begin(), raw_lit.end(), '\''), raw_lit.end());
				shared_ptr<operand> freg = allocate_general_purpose_fp_register(mc);
				if (lit.suffix_kind() == token::suffix::SUFFIX_FLOAT) {
					string fp_lit = "__fp_lit_" + to_string(mc->next_misc_counter());
					mc->add_data_directive(make_shared<directive>(2));
					mc->add_data_directive(make_shared<directive>(fp_lit, (float) atof(raw_lit.c_str())));
					freg->set_double_precision(false);
					freg->set_single_precision(true);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LWC1, freg, make_shared<operand>(false, fp_lit)));
				}
				else {
					string dp_lit = "__dp_lit_" + to_string(mc->next_misc_counter());
					mc->add_data_directive(make_shared<directive>(3));
					mc->add_data_directive(make_shared<directive>(dp_lit, atof(raw_lit.c_str())));
					freg->set_double_precision(true);
					freg->set_single_precision(false);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LDC1, freg, make_shared<operand>(false, dp_lit)));
				}
				return freg;
			}
				break;
			case token::kind::TOKEN_STRING: {
				if (!mc->inside_function()) {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
				}
				int counter = mc->next_misc_counter();
				string s_lit = "__sp_lit_" + to_string(counter);
				raw_lit = "";
				for (token t : pe->stream())
					raw_lit += t.raw_text().substr(1, t.raw_text().length() - 2);
				shared_ptr<operand> fp = register_file2::_fp_register;
				vector<string> chars;
				for (int i = 0; i < raw_lit.length(); i++)
					if (raw_lit[i] == '\\')
						chars.push_back("'" + string(1, raw_lit[i]) + string(1, raw_lit[i + 1]) + "'"), i++;
					else
						chars.push_back("'" + string(1, raw_lit[i]) + "'");
				chars.push_back("0");
				mc->add_data_directive(make_shared<directive>(2));
				mc->add_data_directive(make_shared<directive>(s_lit, chars.size(), false));
				shared_ptr<operand> reg = allocate_general_purpose_register(mc);
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg, make_shared<operand>(false, s_lit)));
				int curr_off = 0;
				shared_ptr<operand> tmp = allocate_general_purpose_register(mc);
				for (string s : chars) {
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, tmp, register_file2::_zero_register, make_shared<operand>(false, s)));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SB, tmp, make_shared<operand>(operand::offset_kind::KIND_TRUE, curr_off, reg->register_number(),
							reg->register_name())));
					curr_off++;
				}
				free_general_purpose_register(mc, tmp);
				return reg;
			}
				break;
			case token::kind::TOKEN_TRUE:
			case token::kind::TOKEN_FALSE: {
				int b = lit.token_kind() == token::kind::TOKEN_TRUE;
				return make_shared<operand>(b);
			}
				break;
			default:
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return nullptr;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		pair<tuple<bool, bool, bool>, shared_ptr<operand>> generate_constexpr_identifier_mips(shared_ptr<mips_code> mc, shared_ptr<symbol> sym,
			pair<bool, variant<bool, int, unsigned int, float, double, string>> res, shared_ptr<operand> init_reg) {
			if (sym->symbol_kind() != symbol::kind::KIND_VARIABLE) {
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return make_pair(make_tuple(false, false, false), nullptr);
			}
			shared_ptr<variable_symbol> vsym = static_pointer_cast<variable_symbol>(sym);
			shared_ptr<type> vtype = vsym->variable_type();
			bool integral = false, singlep = false, doublep = false;
			if (vtype->type_array_kind() == type::array_kind::KIND_ARRAY)
				integral = true;
			else {
				if (vtype->type_kind() != type::kind::KIND_PRIMITIVE) {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return make_pair(make_tuple(false, false, false), nullptr);
				}
				shared_ptr<primitive_type> ptype = static_pointer_cast<primitive_type>(vtype);
				if (ptype->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
					doublep = true;
				else if (ptype->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
					singlep = true;
				else
					integral = true;
			}
			auto handle_integral = [&init_reg, &mc, &integral, &doublep, &singlep](variant<bool, int, unsigned int, float, double, string> var) -> shared_ptr<operand> {
				int val;
				if (holds_alternative<bool>(var))
					val = get<bool>(var);
				else if (holds_alternative<int>(var))
					val = get<int>(var);
				else if (holds_alternative<unsigned int>(var))
					val = get<unsigned int>(var);
				else if (holds_alternative<float>(var))
					val = get<float>(var);
				else if (holds_alternative<double>(var))
					val = get<double>(var);
				else
					return make_shared<operand>(false, get<string>(var));
				if (val <= INT16_MAX && val >= INT16_MIN) {
					if (init_reg != nullptr) {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, init_reg, register_file2::_zero_register, make_shared<operand>(val)));
						return init_reg;
					}
					else
						return make_shared<operand>(val);
				}
				else {
					shared_ptr<operand> reg = init_reg == nullptr ? allocate_general_purpose_register(mc) : init_reg;
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LUI, reg, make_shared<operand>((val >> 16) & 0xffff)));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ORI, reg, make_shared<operand>(val & 0xffff)));
					return reg;
				}
			};
			auto handle_non_integral = [&init_reg, &mc, &integral, &doublep, &singlep](variant<bool, int, unsigned int, float, double, string> var) -> shared_ptr<operand> {
				double val;
				if (holds_alternative<bool>(var))
					val = get<bool>(var);
				else if (holds_alternative<int>(var))
					val = get<int>(var);
				else if (holds_alternative<unsigned int>(var))
					val = get<unsigned int>(var);
				else if (holds_alternative<float>(var))
					val = get<float>(var);
				else if (holds_alternative<double>(var))
					val = get<double>(var);
				else
					return make_shared<operand>(false, get<string>(var));
				shared_ptr<operand> freg = init_reg == nullptr ? allocate_general_purpose_fp_register(mc) : init_reg;
				if (doublep) {
					string dp_lit = "__dp_lit_" + to_string(mc->next_misc_counter());
					mc->add_data_directive(make_shared<directive>(3));
					mc->add_data_directive(make_shared<directive>(dp_lit, val));
					freg->set_double_precision(true);
					freg->set_single_precision(false);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LDC1, freg, make_shared<operand>(false, dp_lit)));
					return freg;
				}
				else if (singlep) {
					string fp_lit = "__fp_lit_" + to_string(mc->next_misc_counter());
					mc->add_data_directive(make_shared<directive>(2));
					mc->add_data_directive(make_shared<directive>(fp_lit, (float)val));
					freg->set_double_precision(false);
					freg->set_single_precision(true);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LWC1, freg, make_shared<operand>(false, fp_lit)));
					return freg;
				}
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return nullptr;
			};
			if (integral) return make_pair(make_tuple(integral, singlep, doublep), handle_integral(res.second));
			else if (singlep || doublep) return make_pair(make_tuple(integral, singlep, doublep), handle_non_integral(res.second));
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return make_pair(make_tuple(false, false, false), nullptr);
		}

		shared_ptr<operand> generate_primary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<primary_expression> pe, bool lvalue) {
			if (pe == nullptr) return nullptr;
			switch (pe->primary_expression_kind()) {
			case primary_expression::kind::KIND_LITERAL:
				return literal_token_2_operand(mc, pe);
				break;
			case primary_expression::kind::KIND_PARENTHESIZED_EXPRESSION:
				return generate_expression_mips(mc, pe->parenthesized_expression(), lvalue);
				break;
			case primary_expression::kind::KIND_STMT_EXPR: {
				shared_ptr<block_stmt> block = pe->stmt_expression();
				shared_ptr<operand> ret = register_file2::_zero_register;
				for (int i = 0; i < block->stmt_list().size(); i++) {
					shared_ptr<stmt> s = block->stmt_list()[i];
					if (i == block->stmt_list().size() - 1 && s->stmt_kind() == stmt::kind::KIND_EXPRESSION) {
						shared_ptr<type> et = s->stmt_expression()->expression_type();
						shared_ptr<expression> e = s->stmt_expression();
						ret = generate_expression_mips(mc, e, false);
						ret = load_value_into_register(mc, ret, et);
					}
					else
						generate_stmt_mips(mc, s);
				}
				return ret;
			}
				break;
			case primary_expression::kind::KIND_SIZEOF_TYPE: {
				int sz = (int)calculate_type_size(mc, pe->sizeof_type());
				return make_shared<operand>(sz);
			}
				break;
			case primary_expression::kind::KIND_SIZEOF_EXPRESSION: {
				shared_ptr<expression> e = pe->parenthesized_expression();
				shared_ptr<operand> op = generate_expression_mips(mc, e, false);
				op = load_value_into_register(mc, op, e->expression_type());
				if (op->register_number() > 31)
					free_general_purpose_fp_register(mc, op);
				else
					free_general_purpose_register(mc, op);
				int sz = (int) calculate_type_size(mc, e->expression_type());
				return make_shared<operand>(sz);
			}
				break;
			case primary_expression::kind::KIND_IDENTIFIER: {
				string sym_string = c_symbol_2_string(mc, pe->identifier_symbol());
				shared_ptr<symbol> sym = pe->identifier_symbol();
				if (sym->symbol_kind() == symbol::kind::KIND_FUNCTION) {
					if (is_function_main(mc, static_pointer_cast<function_symbol>(sym)))
#if SYSTEM == 0
						return make_shared<operand>(false, "__start");
#elif SYSTEM == 1
						return make_shared<operand>(false, "main");
#elif SYSTEM == 2
						return make_shared<operand>(false, "main");
#endif
					else
						return make_shared<operand>(false, sym_string);
				}
				else {
					pair<bool, variant<bool, int, unsigned int, float, double, string>> res = mc->get_constexpr_mapping(sym_string);
					if(!res.first)
						return mc->current_frame()->get_variable_offset(sym_string);
					return generate_constexpr_identifier_mips(mc, sym, res, nullptr).second;
				}
			}
				break;
			case primary_expression::kind::KIND_ARRAY_INITIALIZER: {
				shared_ptr<operand> res = allocate_general_purpose_register(mc);
				vector<shared_ptr<assignment_expression>> arr_list = pe->array_initializer();
				int elem_sz = (int)calculate_type_size(mc, arr_list[0]->assignment_expression_type()),
					adjusted_elem_sz = elem_sz;
				int actual_sz = adjusted_elem_sz * arr_list.size();
				int counter = mc->next_misc_counter();
				if (!mc->inside_function()) {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
				}
				string lab = "__temp_arr_init_" + to_string(counter);
				shared_ptr<operand> lab_op = make_shared<operand>(false, lab);
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, res, lab_op));
				mc->add_data_directive(make_shared<directive>(3));
				mc->add_data_directive(make_shared<directive>(lab, actual_sz));
				int curr = 0;
				shared_ptr<type> parent_type = pe->primary_expression_type();
				if (parent_type->type_kind() == type::kind::KIND_PRIMITIVE) {
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(parent_type);
					parent_type = make_shared<primitive_type>(pt->primitive_type_kind(), pt->type_const_kind(), pt->type_static_kind(), pt->primitive_type_sign_kind(), pt->array_dimensions() - 1);
				}
				else if (parent_type->type_kind() == type::kind::KIND_STRUCT) {
					shared_ptr<struct_type> st = static_pointer_cast<struct_type>(parent_type);
					parent_type = make_shared<struct_type>(st->type_const_kind(), st->type_static_kind(), st->struct_name(), st->struct_reference_number(), st->array_dimensions() - 1,
						st->is_union());
				}
				else if (parent_type->type_kind() == type::kind::KIND_FUNCTION) {
					shared_ptr<function_type> ft = static_pointer_cast<function_type>(parent_type);
					parent_type = make_shared<function_type>(ft->type_const_kind(), ft->type_static_kind(), ft->return_type(), ft->parameter_list(), ft->function_reference_number(), ft->array_dimensions() - 1);
				}
				else {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
				}
				insn::kind s = load_store_insn_from_type(mc, parent_type, false);
				for (shared_ptr<assignment_expression> ae : arr_list) {
					shared_ptr<type> curr_type = ae->assignment_expression_type();
					shared_ptr<operand> cres = generate_assignment_expression_mips(mc, ae, false);
					cres = load_value_into_register(mc, cres, curr_type);
					if (curr_type->type_kind() == type::kind::KIND_PRIMITIVE || curr_type->type_array_kind() == type::array_kind::KIND_ARRAY) {
						cres = cast_between_types(mc, cres, curr_type, parent_type);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(s, cres, make_shared<operand>(operand::offset_kind::KIND_TRUE, curr, res->register_number(), res->register_name())));
					}
					else if (curr_type->type_kind() == type::kind::KIND_STRUCT && curr_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
						insn::kind l = load_store_insn_from_type(mc, parent_type, true);
						shared_ptr<operand> reg = allocate_general_purpose_register(mc);
						for (int off = 0; off < adjusted_elem_sz; off += 4) {
							mc->current_frame()->add_insn_to_body(make_shared<insn>(l, reg, make_shared<operand>(operand::offset_kind::KIND_TRUE, off, cres->register_number(),
								cres->register_name())));
							mc->current_frame()->add_insn_to_body(make_shared<insn>(s, reg, make_shared<operand>(operand::offset_kind::KIND_TRUE, curr + off, res->register_number(),
								res->register_name())));
						}
						free_general_purpose_register(mc, reg);
					}
					else if (curr_type->type_kind() == type::kind::KIND_FUNCTION && curr_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(s, cres, make_shared<operand>(operand::offset_kind::KIND_TRUE, curr, res->register_number(), res->register_name())));
					else {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					if (cres->register_number() > 31)
						free_general_purpose_fp_register(mc, cres);
					else
						free_general_purpose_register(mc, cres);
					curr += adjusted_elem_sz;
				}
				return res;
			}
				break;
			case primary_expression::kind::KIND_RESV:
			case primary_expression::kind::KIND_STK: {
				shared_ptr<type> t = pe->mem_type();
				int sz = (int)calculate_type_size(mc, t);
				variant<bool, int, unsigned int, float, double, string> res;
				for (shared_ptr<assignment_expression> ae : pe->parenthesized_expression()->assignment_expression_list())
					res = evaluate_constant_expression(mc, ae);
				if (holds_alternative<bool>(res))
					sz *= (int)get<bool>(res);
				else if (holds_alternative<int>(res))
					sz *= get<int>(res);
				else if (holds_alternative<unsigned int>(res))
					sz *= (int)get<unsigned int>(res);
				else if (holds_alternative<float>(res))
					sz *= (int)get<float>(res);
				else if (holds_alternative<double>(res))
					sz *= (int)get<double>(res);
				else {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				}
				shared_ptr<operand> reg = allocate_general_purpose_register(mc);
				sz = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
				if (pe->primary_expression_kind() == primary_expression::kind::KIND_STK) {
					int off = -(mc->current_frame()->middle_section_size() + sz);
					mc->current_frame()->update_middle_section_size(sz);
					shared_ptr<operand> imm = make_shared<operand>(off);
					imm->set_operand_offset_kind(operand::offset_kind::KIND_MIDDLE);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, reg, register_file2::_fp_register, imm));
				}
				else {
					string lab = "__temp_resv_" + to_string(mc->next_misc_counter());
					mc->add_data_directive(make_shared<directive>(3));
					mc->add_data_directive(make_shared<directive>(lab, sz));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg, make_shared<operand>(false, lab)));
				}
				return reg;
			}
				break;
			case primary_expression::kind::KIND_NEW: {
				shared_ptr<type> t = pe->mem_type();
				int sz = (int)calculate_type_size(mc, t);
				shared_ptr<operand> e = generate_expression_mips(mc, pe->parenthesized_expression(), false);
				if(e->operand_kind() != operand::kind::KIND_INT_IMMEDIATE)
					e = load_value_into_register(mc, e, pe->parenthesized_expression()->expression_type());
				bool is_pow_of_2 = (sz & (sz - 1)) == 0 && sz != 0;
				if (e->operand_kind() == operand::kind::KIND_INT_IMMEDIATE) {
					e = make_shared<operand>(e->int_immediate() * sz);
				}
				else {
					if (is_pow_of_2)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SLL, e, e, make_shared<operand>((int)log2(sz))));
					else {
						shared_ptr<operand> temp = allocate_general_purpose_register(mc);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, temp, register_file2::_zero_register, make_shared<operand>(sz)));
#if SYSTEM == 0 || SYSTEM == 1
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MULTU, e, temp));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MFLO, e));
#elif SYSTEM == 2
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MULTU, e, e, temp));
#endif
						free_general_purpose_register(mc, temp);
					}
				}
#if SYSTEM == 0 || SYSTEM == 1
				shared_ptr<operand> _4 = register_file2::_a0_register, _2 = register_file2::_v0_register, _30 = register_file2::_fp_register;
				vector<int> _4_2_store;
				bool _4_in_use = mc->current_frame()->is_register_in_use(4), _2_in_use = mc->current_frame()->is_register_in_use(2);
				if (_4_in_use) {
					int o = -(mc->current_frame()->middle_section_size() + 8);
					_4_2_store.push_back(o);
					mc->current_frame()->update_middle_section_size(8);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, _4, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
						o, _30->register_number(), _30->register_name())));
				}
				else
					_4_2_store.push_back(-1);
				if (_2_in_use) {
					int o = -(mc->current_frame()->middle_section_size() + 8);
					_4_2_store.push_back(o);
					mc->current_frame()->update_middle_section_size(8);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, _2, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
						o, _30->register_number(), _30->register_name())));
				}
				else
					_4_2_store.push_back(-1);
#endif
#if SYSTEM == 0
#if PROG_NEW == 0
				tuple<vector<int>, vector<int>, int> res_s_tup = save_to_middle(mc);
				vector<int> which_to_store = get<0>(res_s_tup), middle_offsets = get<1>(res_s_tup);
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, _2, register_file2::_zero_register, make_shared<operand>(4045)));
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, _4, register_file2::_zero_register, register_file2::_zero_register));
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SYSCALL));
				restore_from_middle(mc, which_to_store, middle_offsets);
				if(e->operand_kind() == operand::kind::KIND_INT_IMMEDIATE)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, _4, _2, e));
				else
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, _4, _2, e));
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, _2, register_file2::_zero_register, make_shared<operand>(4045)));
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SYSCALL));
				restore_from_middle(mc, which_to_store, middle_offsets);
				if (e->operand_kind() == operand::kind::KIND_INT_IMMEDIATE)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, _2, _2, make_shared<operand>(-e->int_immediate())));
				else
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SUBU, _2, _2, e));
#elif PROG_NEW == 1
				tuple<vector<int>, vector<int>, int> res_s_tup = save_to_middle(mc);
				vector<int> which_to_store = get<0>(res_s_tup), middle_offsets = get<1>(res_s_tup);
				mc->current_frame()->add_insn_to_body(make_shared<insn>(
					e->operand_kind() == operand::kind::KIND_INT_IMMEDIATE ? insn::kind::KIND_ADDIU : insn::kind::KIND_ADDU,
					_4, register_file2::_zero_register, e));
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_JAL, make_shared<operand>(false, program_new_hook)));
				restore_from_middle(mc, which_to_store, middle_offsets);
#endif
#elif SYSTEM == 1
				if(e->operand_kind() == operand::kind::KIND_INT_IMMEDIATE)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, _4, register_file2::_zero_register, e));
				else
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, _4, register_file2::_zero_register, e));
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, _2, register_file2::_zero_register, make_shared<operand>(9)));
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SYSCALL));
#elif SYSTEM == 2
				int a0_pos = -1, a1_pos = -1;
				shared_ptr<operand> a0 = register_file2::_a0_register, fp = register_file2::_fp_register, a1 = register_file2::_a1_register;
				bool _a0_in_use = mc->current_frame()->is_register_in_use(10), _a1_in_use = mc->current_frame()->is_register_in_use(11);
				if (_a0_in_use) {
					int o = -(mc->current_frame()->middle_section_size() + 8);
					a0_pos = o;
					mc->current_frame()->update_middle_section_size(8);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, a0, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
						o, fp->register_number(), fp->register_name())));
				}
				if (_a1_in_use) {
					int o = -(mc->current_frame()->middle_section_size() + 8);
					a1_pos = o;
					mc->current_frame()->update_middle_section_size(8);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, a1, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
						o, fp->register_number(), fp->register_name())));
				}
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, a0, register_file2::_zero_register, make_shared<operand>(9)));
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, a1, register_file2::_zero_register, e));
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SYSCALL));
#endif
				shared_ptr<operand> reg = allocate_general_purpose_register(mc);
#if SYSTEM == 0 || SYSTEM == 1
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, reg, register_file2::_zero_register, _2));
				if (_4_in_use)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, _4, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
						_4_2_store[0], _30->register_number(), _30->register_name())));
				if (_2_in_use)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, _2, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
						_4_2_store[1], _2->register_number(), _2->register_name())));
#elif SYSTEM == 2
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, reg, register_file2::_zero_register, a0));
				if (_a0_in_use)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, a0, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
						a0_pos, fp->register_number(), fp->register_name())));
				if (_a1_in_use)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, a1, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
						a1_pos, a0->register_number(), a1->register_name())));
#endif
				if (e->register_number() > 31)
					free_general_purpose_fp_register(mc, e);
				else
					free_general_purpose_register(mc, e);
				return reg;
			}
				break;
			default:
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return nullptr;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		pair<shared_ptr<operand>, shared_ptr<operand>> deconflict_registers(shared_ptr<mips_code> mc, shared_ptr<operand> o1, shared_ptr<operand> o2) {
			if (o1 == nullptr || o2 == nullptr) return make_pair(nullptr, nullptr);
			if (o1->operand_kind() != operand::kind::KIND_REGISTER || o2->operand_kind() != operand::kind::KIND_REGISTER)
				return make_pair(o1, o2);
			if (o1->register_number() != o2->register_number()) return make_pair(o1, o2);
			vector<shared_ptr<operand>> o_store;
			shared_ptr<operand> orig = o2;
			shared_ptr<operand> fp = register_file2::_fp_register;
			while (o2->register_number() == o1->register_number()) {
				o_store.push_back(o2);
				if (o1->register_number() > 31)
					o2 = allocate_general_purpose_fp_register(mc);
				else
					o2 = allocate_general_purpose_register(mc);
			}
			for (shared_ptr<operand> o : o_store) {
				if (o->register_number() > 31)
					free_general_purpose_fp_register(mc, o);
				else
					free_general_purpose_register(mc, o);
			}
			if (o1->register_number() > 31)
				load_general_purpose_fp_register_from_frame(mc, o1);
			else
				load_general_purpose_register_from_frame(mc, o1);
			if (orig->on_stack()) {
				if (orig->register_number() > 31) {
					if (orig->double_precision())
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LDC1, o2, make_shared<operand>(operand::offset_kind::KIND_MIDDLE, orig->middle_stack_offset(),
							fp->register_number(), fp->register_name())));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LWC1, o2, make_shared<operand>(operand::offset_kind::KIND_MIDDLE, orig->middle_stack_offset(),
							fp->register_number(), fp->register_name())));
					o2->set_double_precision(orig->double_precision());
					o2->set_single_precision(orig->single_precision());
				}
				else
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, o2, make_shared<operand>(operand::offset_kind::KIND_MIDDLE, orig->middle_stack_offset(),
						fp->register_number(), fp->register_name())));
			}
			else {
				if (orig->register_number() > 31) {
					if (orig->double_precision())
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MOV_D, o2, orig));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MOV_S, o2, orig));
					o2->set_double_precision(orig->double_precision());
					o2->set_single_precision(orig->single_precision());
				}
				else
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, o2, register_file2::_zero_register, orig));
			}
			return make_pair(o1, o2);
		}

		shared_ptr<operand> binary_expression_helper_1(shared_ptr<mips_code> mc, shared_ptr<operand> o1, binary_expression::operator_kind ok, shared_ptr<operand> o2, shared_ptr<type> t, bool u) {
			if (o1 == nullptr || o2 == nullptr) return nullptr;
			if((o1->operand_kind() == operand::kind::KIND_REGISTER && o1->register_number() > 31) || (o2->operand_kind() == operand::kind::KIND_REGISTER && o2->register_number() > 31)) {
				shared_ptr<operand> flhs = nullptr, frhs = nullptr;
				bool dp = false;
				if (o2->operand_kind() != operand::kind::KIND_REGISTER || o2->register_number() <= 31) {
					flhs = o1;
					frhs = load_value_into_register(mc, o2, t);
					dp = flhs->double_precision();
					if(frhs->register_number() <= 31) {
						shared_ptr<operand> temp = allocate_general_purpose_fp_register(mc);
#if SYSTEM == 0 || SYSTEM == 1
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MTC1, frhs, temp));
#elif SYSTEM == 2
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MTC1, temp, frhs));
#endif
						if (dp)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_CVT_D_W, temp, temp));
						else
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_CVT_S_W, temp, temp));
						free_general_purpose_register(mc, frhs);
						frhs = temp;
					}
					frhs->set_double_precision(flhs->double_precision());
					frhs->set_single_precision(flhs->single_precision());
					pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, flhs, frhs);
					flhs = p.first, frhs = p.second;
				}
				else if (o1->operand_kind() != operand::kind::KIND_REGISTER || o1->register_number() <= 31) {
					frhs = o2;
					flhs = load_value_into_register(mc, o1, t);
					dp = frhs->double_precision();
					if(flhs->register_number() <= 31) {
						shared_ptr<operand> temp = allocate_general_purpose_fp_register(mc);
#if SYSTEM == 0 || SYSTEM == 1
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MTC1, flhs, temp));
#elif SYSTEM == 2
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MTC1, temp, flhs));
#endif
						if (dp)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_CVT_D_W, temp, temp));
						else
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_CVT_S_W, temp, temp));
						free_general_purpose_register(mc, flhs);
						flhs = temp;
					}
					flhs->set_double_precision(frhs->double_precision());
					flhs->set_single_precision(frhs->single_precision());
					pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, flhs, frhs);
					flhs = p.first, frhs = p.second;
				}
				else {
					flhs = o1, frhs = o2;
					if (flhs->double_precision() && frhs->single_precision()) {
						shared_ptr<operand> temp = allocate_general_purpose_fp_register(mc);
						pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, frhs, temp);
						frhs = p.first, temp = p.second;
						temp->set_double_precision(true);
						temp->set_single_precision(false);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_CVT_D_S, temp, frhs));
						free_general_purpose_fp_register(mc, frhs);
						frhs = temp;
						dp = true;
					}
					else if (flhs->single_precision() && frhs->double_precision()) {
						shared_ptr<operand> temp = allocate_general_purpose_fp_register(mc);
						pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, flhs, temp);
						flhs = p.first, temp = p.second;
						temp->set_double_precision(true);
						temp->set_single_precision(false);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_CVT_D_S, temp, flhs));
						free_general_purpose_fp_register(mc, flhs);
						flhs = temp;
						dp = true;
					}
					else if (flhs->single_precision() && frhs->single_precision())
						dp = false;
					else
						dp = true;
					pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, flhs, frhs);
					flhs = p.first, frhs = p.second;
				}
				insn::kind fpb = insn::kind::KIND_NONE;
#if SYSTEM == 2
				shared_ptr<operand> res_reg = allocate_general_purpose_register(mc);
#endif
				switch (ok) {
				case binary_expression::operator_kind::KIND_ADD: {
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADD_D, flhs, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADD_S, flhs, flhs, frhs));
				}
					break;
				case binary_expression::operator_kind::KIND_SUBTRACT: {
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SUB_D, flhs, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SUB_S, flhs, flhs, frhs));
				}
					break;
				case binary_expression::operator_kind::KIND_MULTIPLY: {
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MUL_D, flhs, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MUL_S, flhs, flhs, frhs));
				}
					break;
				case binary_expression::operator_kind::KIND_DIVIDE: {
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_DIV_D, flhs, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_DIV_S, flhs, flhs, frhs));
				}
					break;
				case binary_expression::operator_kind::KIND_LESS_THAN: {
#if SYSTEM == 0 || SYSTEM == 1
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LT_D, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LT_S, flhs, frhs));
#elif SYSTEM == 2
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LT_D, res_reg, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LT_S, res_reg, flhs, frhs));
#endif
					fpb = insn::kind::KIND_BC1F;
				}
					break;
				case binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO: {
#if SYSTEM == 0 || SYSTEM == 1
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LE_D, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LE_S, flhs, frhs));
#elif SYSTEM == 2
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LE_D, res_reg, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LE_S, res_reg, flhs, frhs));
#endif
					fpb = insn::kind::KIND_BC1F;
				}
					break;
				case binary_expression::operator_kind::KIND_GREATER_THAN: {
#if SYSTEM == 0 || SYSTEM == 1
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LE_D, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LE_S, flhs, frhs));
#elif SYSTEM == 2
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LE_D, res_reg, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LE_S, res_reg, flhs, frhs));
#endif
					fpb = insn::kind::KIND_BC1T;
				}
					break;
				case binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO: {
#if SYSTEM == 0 || SYSTEM == 1
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LT_D, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LT_S, flhs, frhs));
#elif SYSTEM == 2
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LT_D, res_reg, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_LT_S, res_reg, flhs, frhs));
#endif
					fpb = insn::kind::KIND_BC1T;
				}
					break;
				case binary_expression::operator_kind::KIND_EQUALS_EQUALS: {
#if SYSTEM == 0 || SYSTEM == 1
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_EQ_D, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_EQ_S, flhs, frhs));
#elif SYSTEM == 2
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_EQ_D, res_reg, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_EQ_S, res_reg, flhs, frhs));
#endif
					fpb = insn::kind::KIND_BC1F;
				}
					break;
				case binary_expression::operator_kind::KIND_NOT_EQUALS: {
#if SYSTEM == 0 || SYSTEM == 1
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_EQ_D, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_EQ_S, flhs, frhs));
#elif SYSTEM == 2
					if (dp)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_EQ_D, res_reg, flhs, frhs));
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_C_EQ_S, res_reg, flhs, frhs));
#endif
					fpb = insn::kind::KIND_BC1T;
				}
					break;
				default:
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
					break;
				}
				if (fpb != insn::kind::KIND_NONE) {
					free_general_purpose_fp_register(mc, flhs);
					free_general_purpose_fp_register(mc, frhs);
#if SYSTEM == 0 || SYSTEM == 1
					shared_ptr<operand> reg = allocate_general_purpose_register(mc);
					pair<int, shared_ptr<operand>> lab = mc->next_label();
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, reg, register_file2::_zero_register, register_file2::_zero_register));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(fpb, lab.second));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, reg, register_file2::_zero_register, make_shared<operand>(1)));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(lab.second));
					return reg;
#elif SYSTEM == 2
					return res_reg;
#endif
				}
				else {
					free_general_purpose_fp_register(mc, frhs);
					return flhs;
				}
			}
			if ((o1->operand_kind() != operand::kind::KIND_REGISTER && !o1->is_immediate()) || (o2->operand_kind() != operand::kind::KIND_REGISTER && !o2->is_immediate()))
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			if (o1->operand_kind() == operand::kind::KIND_REGISTER && o2->operand_kind() == operand::kind::KIND_REGISTER) {
				pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, o1, o2);
				o1 = p.first, o2 = p.second;
			}
			if (ok == binary_expression::operator_kind::KIND_ADD || ok == binary_expression::operator_kind::KIND_BITWISE_XOR ||
				ok == binary_expression::operator_kind::KIND_BITWISE_AND || ok == binary_expression::operator_kind::KIND_BITWISE_OR) {
				insn::kind imm_insn = ok == binary_expression::operator_kind::KIND_ADD ? insn::kind::KIND_ADDIU :
					(ok == binary_expression::operator_kind::KIND_BITWISE_XOR ? insn::kind::KIND_XORI : (ok == binary_expression::operator_kind::KIND_BITWISE_AND ?
						insn::kind::KIND_ANDI : insn::kind::KIND_ORI));
				insn::kind reg_insn = ok == binary_expression::operator_kind::KIND_ADD ? insn::kind::KIND_ADDU :
					(ok == binary_expression::operator_kind::KIND_BITWISE_XOR ? insn::kind::KIND_XOR : (ok == binary_expression::operator_kind::KIND_BITWISE_AND ?
						insn::kind::KIND_AND : insn::kind::KIND_OR));
				if (o1->operand_kind() == operand::kind::KIND_REGISTER) {
					if (o2->is_immediate())
						mc->current_frame()->add_insn_to_body(make_shared<insn>(imm_insn, o1, o1, o2));
					else {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(reg_insn, o1, o1, o2));
						free_general_purpose_register(mc, o2);
					}
					return o1;
				}
				else if (o2->operand_kind() == operand::kind::KIND_REGISTER) {
					if (o1->operand_kind() == operand::kind::KIND_REGISTER)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(imm_insn, o2, o2, o1));
					return o2;
				}
				else {
					if (o2->operand_kind() == operand::kind::KIND_REGISTER || o1->operand_kind() == operand::kind::KIND_REGISTER)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					o1 = load_value_into_register(mc, o1, t);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(imm_insn, o1, o1, o2));
					return o1;
				}
			}
			else if (ok == binary_expression::operator_kind::KIND_SUBTRACT) {
				if (o1->operand_kind() == operand::kind::KIND_REGISTER) {
					if (o2->is_immediate()) {
						if (o2->operand_kind() == operand::kind::KIND_CHAR_IMMEDIATE) {
							shared_ptr<operand> temp = load_value_into_register(mc, o2, t);
							pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, o1, temp);
							o1 = p.first, temp = p.second;
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SUBU, o1, o1, temp));
							free_general_purpose_register(mc, temp);
							return o1;
						}
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, o1, o1, make_shared<operand>(-o2->int_immediate())));
						return o1;
					}
					else {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SUBU, o1, o1, o2));
						free_general_purpose_register(mc, o2);
						return o1;
					}
				}
				else if (o2->operand_kind() == operand::kind::KIND_REGISTER) {
					if (o1->operand_kind() == operand::kind::KIND_REGISTER)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SUBU, o2, register_file2::_zero_register, o2));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, o2, o2, o1));
					return o2;
				}
				else {
					if (o2->operand_kind() == operand::kind::KIND_REGISTER || o1->operand_kind() == operand::kind::KIND_REGISTER)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (o2->operand_kind() == operand::kind::KIND_INT_IMMEDIATE) {
						shared_ptr<operand> lhs = load_value_into_register(mc, o1, t);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, lhs, lhs, make_shared<operand>(-o2->int_immediate())));
						return lhs;
					}
					else if (o1->operand_kind() == operand::kind::KIND_INT_IMMEDIATE) {
						shared_ptr<operand> rhs = load_value_into_register(mc, o2, t);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SUBU, rhs, register_file2::_zero_register, rhs));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, rhs, rhs, o1));
						return rhs;
					}
					else {
						shared_ptr<operand> lhs = load_value_into_register(mc, o1, t);
						shared_ptr<operand> rhs = load_value_into_register(mc, o2, t);
						pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, lhs, rhs);
						lhs = p.first, rhs = p.second;
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SUBU, lhs, lhs, rhs));
						free_general_purpose_register(mc, rhs);
						return lhs;
					}
				}
			}
			else if (ok == binary_expression::operator_kind::KIND_MULTIPLY) {
				shared_ptr<operand> lhs = nullptr, rhs = nullptr;
				if (o1->operand_kind() != operand::kind::KIND_REGISTER)
					lhs = load_value_into_register(mc, o1, t);
				else
					lhs = o1;
				if (o2->operand_kind() != operand::kind::KIND_REGISTER)
					rhs = load_value_into_register(mc, o2, t);
				else
					rhs = o2;
				pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, lhs, rhs);
				lhs = p.first, rhs = p.second;
#if SYSTEM == 0 || SYSTEM == 1
				mc->current_frame()->add_insn_to_body(make_shared<insn>(u ? insn::kind::KIND_MULTU : insn::kind::KIND_MULT, lhs, rhs));
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MFLO, lhs));
#elif SYSTEM == 2
				mc->current_frame()->add_insn_to_body(make_shared<insn>(u ? insn::kind::KIND_MULTU : insn::kind::KIND_MULT, lhs, lhs, rhs));
#endif
				free_general_purpose_register(mc, rhs);
				return lhs;
			}
			else if (ok == binary_expression::operator_kind::KIND_DIVIDE || ok == binary_expression::operator_kind::KIND_MODULUS) {
				bool lo = ok == binary_expression::operator_kind::KIND_DIVIDE;
				shared_ptr<operand> lhs = nullptr, rhs = nullptr;
				if (o1->operand_kind() != operand::kind::KIND_REGISTER)
					lhs = load_value_into_register(mc, o1, t);
				else
					lhs = o1;
				if (o2->operand_kind() != operand::kind::KIND_REGISTER)
					rhs = load_value_into_register(mc, o2, t);
				else
					rhs = o2;
#if SYSTEM == 0 || SYSTEM == 1
				mc->current_frame()->add_insn_to_body(make_shared<insn>(u ? insn::kind::KIND_DIVU : insn::kind::KIND_DIV, lhs, rhs));
				mc->current_frame()->add_insn_to_body(make_shared<insn>(lo ? insn::kind::KIND_MFLO : insn::kind::KIND_MFHI, lhs));
#elif SYSTEM == 2
				if (lo)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(u ? insn::kind::KIND_DIVU : insn::kind::KIND_DIV, lhs, lhs, rhs));
				else
					mc->current_frame()->add_insn_to_body(make_shared<insn>(u ? insn::kind::KIND_REMU : insn::kind::KIND_REM, lhs, lhs, rhs));
#endif
				free_general_purpose_register(mc, rhs);
				return lhs;
			}
			else if (ok == binary_expression::operator_kind::KIND_SHIFT_LEFT || ok == binary_expression::operator_kind::KIND_SHIFT_RIGHT) {
				o1 = load_value_into_register(mc, o1, t);
				if (t->type_kind() != type::kind::KIND_PRIMITIVE || t->type_array_kind() == type::array_kind::KIND_ARRAY)
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
				if (pt->primitive_type_kind() == primitive_type::kind::KIND_BOOL || pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE ||
					pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				bool sl = ok == binary_expression::operator_kind::KIND_SHIFT_LEFT;
				if (o2->operand_kind() == operand::kind::KIND_INT_IMMEDIATE && (0 <= o2->int_immediate() && o2->int_immediate() <= 31)) {
					if (sl)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SLL, o1, o1, o2));
					else {
						if (pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_SIGNED)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SRA, o1, o1, o2));
						else
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SRL, o1, o1, o2));
					}
					return o1;
				}
				else {
					o2 = load_value_into_register(mc, o2, t);
					pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, o1, o2);
					o1 = p.first, o2 = p.second;
					if (sl)
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SLLV, o1, o1, o2));
					else {
						if (pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_SIGNED)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SRAV, o1, o1, o2));
						else
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SRLV, o1, o1, o2));
					}
					free_general_purpose_register(mc, o2);
					return o1;
				}
			}
			else if (ok == binary_expression::operator_kind::KIND_LESS_THAN || ok == binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO ||
				ok == binary_expression::operator_kind::KIND_GREATER_THAN || ok == binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO ||
				ok == binary_expression::operator_kind::KIND_EQUALS_EQUALS || ok == binary_expression::operator_kind::KIND_NOT_EQUALS) {
				shared_ptr<operand> lhs = load_value_into_register(mc, o1, t);
				if (ok == binary_expression::operator_kind::KIND_LESS_THAN) {
					if (o2->is_immediate()) {
						if (o2->int_immediate() >= (1 << 15)) {
							shared_ptr<operand> temp = allocate_general_purpose_register(mc);
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ORI, temp, register_file2::_zero_register, o2));
							mc->current_frame()->add_insn_to_body(make_shared<insn>(u ? insn::kind::KIND_SLTU : insn::kind::KIND_SLT, lhs, lhs, temp));
							free_general_purpose_register(mc, temp);
						}
						else
							mc->current_frame()->add_insn_to_body(make_shared<insn>(u ? insn::kind::KIND_SLTIU : insn::kind::KIND_SLTI, lhs, lhs, o2));
					}
					else {
						pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, o1, o2);
						o1 = p.first, o2 = p.second;
						mc->current_frame()->add_insn_to_body(make_shared<insn>(u ? insn::kind::KIND_SLTU : insn::kind::KIND_SLT, lhs, lhs, o2));
						free_general_purpose_register(mc, o2);
					}
					return lhs;
				}
				shared_ptr<operand> rhs = load_value_into_register(mc, o2, t);
				pair<shared_ptr<operand>, shared_ptr<operand>> p = deconflict_registers(mc, lhs, rhs);
				lhs = p.first, rhs = p.second;
				if (ok == binary_expression::operator_kind::KIND_GREATER_THAN)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(u ? insn::kind::KIND_SGTU : insn::kind::KIND_SGT, lhs, lhs, rhs));
				else if (ok == binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(u ? insn::kind::KIND_SGEU : insn::kind::KIND_SGE, lhs, lhs, rhs));
				else if (ok == binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(u ? insn::kind::KIND_SLEU : insn::kind::KIND_SLE, lhs, lhs, rhs));
				else if (ok == binary_expression::operator_kind::KIND_EQUALS_EQUALS)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SEQ, lhs, lhs, rhs));
				else if (ok == binary_expression::operator_kind::KIND_NOT_EQUALS)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SNE, lhs, lhs, rhs));
				else {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
				}
				free_general_purpose_register(mc, rhs);
				return lhs;
			}
			else if (ok == binary_expression::operator_kind::KIND_LOGICAL_AND || ok == binary_expression::operator_kind::KIND_LOGICAL_OR) {
				insn::kind imm_kind = ok == binary_expression::operator_kind::KIND_LOGICAL_AND ? insn::kind::KIND_ANDI : insn::kind::KIND_ORI;
				insn::kind ik = ok == binary_expression::operator_kind::KIND_LOGICAL_AND ? insn::kind::KIND_AND : insn::kind::KIND_OR;
				if (o1->operand_kind() == operand::kind::KIND_REGISTER) {
					if (o2->is_immediate())
						mc->current_frame()->add_insn_to_body(make_shared<insn>(imm_kind, o1, o1, o2));
					else {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(ik, o1, o1, o2));
						free_general_purpose_register(mc, o2);
					}
					return o1;
				}
				else if (o2->operand_kind() == operand::kind::KIND_REGISTER) {
					mc->current_frame()->add_insn_to_body(make_shared<insn>(ik, o2, o2, o1));
					return o1;
				}
				else {
					shared_ptr<operand> lhs_op = load_value_into_register(mc, o1, t);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(imm_kind, lhs_op, lhs_op, o2));
					return lhs_op;
				}
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		unsigned int calculate_type_size(shared_ptr<mips_code> mc, shared_ptr<type> t) {
			if (t == nullptr) return 0;
			if (t->type_array_kind() == type::array_kind::KIND_ARRAY)
				return 4;
			if (t->type_kind() == type::kind::KIND_PRIMITIVE) {
				shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
				switch (pt->primitive_type_kind()) {
				case primitive_type::kind::KIND_DOUBLE:
					return 8;
					break;
				case primitive_type::kind::KIND_BOOL:
				case primitive_type::kind::KIND_BYTE:
				case primitive_type::kind::KIND_CHAR:
					return 1;
					break;
				case primitive_type::kind::KIND_INT:
				case primitive_type::kind::KIND_FLOAT:
				case primitive_type::kind::KIND_LONG:
					return 4;
					break;
				case primitive_type::kind::KIND_SHORT:
					return 2;
					break;
				case primitive_type::kind::KIND_VOID:
					return 0;
					break;
				}
			}
			else if (t->type_kind() == type::kind::KIND_STRUCT) {
				shared_ptr<struct_type> st = static_pointer_cast<struct_type>(t);
				shared_ptr<struct_symbol> ss = mc->find_struct_symbol(st->struct_name(), st->struct_reference_number());
				if (ss == nullptr)
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				shared_ptr<scope> s_scope = ss->struct_symbol_scope();
				unsigned int sz = 0, max_mem_sz = 0;
				for (shared_ptr<symbol> sym : s_scope->symbol_list()) {
					if (sym->symbol_kind() == symbol::kind::KIND_STRUCT) {
						shared_ptr<struct_symbol> ssym = static_pointer_cast<struct_symbol>(sym);
						shared_ptr<type> t = ssym->struct_symbol_type();
						int sz1 = calculate_type_size(mc, t);
						max_mem_sz = max(max_mem_sz, (unsigned int) sz1);
						sz += sz1 % 8 == 0 ? sz1 : (sz1 / 8 + 1) * 8;
					}
					else if (sym->symbol_kind() == symbol::kind::KIND_VARIABLE) {
						shared_ptr<variable_symbol> vsym = static_pointer_cast<variable_symbol>(sym);
						shared_ptr<type> t = vsym->variable_type();
						int sz1 = calculate_type_size(mc, t);
						max_mem_sz = max(max_mem_sz, (unsigned int) sz1);
						sz += sz1 % 8 == 0 ? sz1 : (sz1 / 8 + 1) * 8;
					}
					else
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				}
				if (sz == 0) sz = 4;
				if (max_mem_sz == 0) max_mem_sz = 4;
				return st->is_union() ? max_mem_sz : sz;
			}
			else if (t->type_kind() == type::kind::KIND_FUNCTION)
				return 4;
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return 0;
		}

		string type_2_string(shared_ptr<mips_code> mc, shared_ptr<type> t, bool spaces) {
			if (t == nullptr || !t->valid())
				return "";
			string sep = spaces ? " " : "_";
			string arr = t->type_const_kind() == type::const_kind::KIND_CONST ? sep + "const" : "";
			arr += t->type_static_kind() == type::static_kind::KIND_STATIC ? sep + "static" : "";
			if (t->type_array_kind() == type::array_kind::KIND_ARRAY) {
				arr += sep;
				for(int i = 0; i < t->array_dimensions(); i++) {
					if (spaces) arr += "[]";
					else arr += "A";
				}
			}
			if (t->type_kind() == type::kind::KIND_PRIMITIVE) {
				shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
				string signed_unsigned = "";
				if (pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_SIGNED)
					signed_unsigned = "signed" + sep;
				else if (pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED)
					signed_unsigned = "unsigned" + sep;
				if (pt->primitive_type_kind() == primitive_type::kind::KIND_BOOL)
					return signed_unsigned + "bool" + arr;
				else if (pt->primitive_type_kind() == primitive_type::kind::KIND_BYTE)
					return signed_unsigned + "byte" + arr;
				else if (pt->primitive_type_kind() == primitive_type::kind::KIND_CHAR)
					return signed_unsigned + "char" + arr;
				else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
					return signed_unsigned + "double" + arr;
				else if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
					return signed_unsigned + "float" + arr;
				else if (pt->primitive_type_kind() == primitive_type::kind::KIND_INT)
					return signed_unsigned + "int" + arr;
				else if (pt->primitive_type_kind() == primitive_type::kind::KIND_LONG)
					return signed_unsigned + "long" + arr;
				else if (pt->primitive_type_kind() == primitive_type::kind::KIND_SHORT)
					return signed_unsigned + "short" + arr;
				else if (pt->primitive_type_kind() == primitive_type::kind::KIND_VOID)
					return signed_unsigned + "void" + arr;
			}
			else if (t->type_kind() == type::kind::KIND_STRUCT) {
				shared_ptr<struct_type> s = static_pointer_cast<struct_type>(t);
				return (s->is_union() ? "union" : "struct") + sep + s->struct_name().raw_text() + "_" + arr;
			}
			else if (t->type_kind() == type::kind::KIND_FUNCTION) {
				shared_ptr<function_type> ft = static_pointer_cast<function_type>(t);
				string ret = "fn" + sep + type_2_string(mc, ft->return_type(), spaces) + sep;
				for (shared_ptr<variable_declaration> vd : ft->parameter_list())
					ret += type_2_string(mc, vd->variable_declaration_type(), spaces) + sep;
				return ret + arr;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return "";
		}

		string scope_2_string_helper(shared_ptr<mips_code> mc, shared_ptr<scope> s, string r) {
			if (s == nullptr)
				return r;
			else if (s->parent_scope() == nullptr || s->scope_kind() == scope::kind::KIND_GLOBAL)
				return r + "global_";
			switch (s->scope_kind()) {
			case scope::kind::KIND_BLOCK:
				return scope_2_string_helper(mc, s->parent_scope(), r + "block" + to_string(static_pointer_cast<block_scope>(s)->block_number()) + "_");
				break;
			case scope::kind::KIND_LOOP:
				return scope_2_string_helper(mc, s->parent_scope(), r + "loop_");
				break;
			case scope::kind::KIND_SWITCH:
				return scope_2_string_helper(mc, s->parent_scope(), r + "switch_");
				break;
			case scope::kind::KIND_NAMESPACE:
				return scope_2_string_helper(mc, s->parent_scope(), r + "namespace_" + static_pointer_cast<namespace_scope>(s)->namespace_name().raw_text() + "_");
			case scope::kind::KIND_FUNCTION: {
				shared_ptr<function_scope> fs = static_pointer_cast<function_scope>(s);
				string c, st;
				if (fs->function_const_kind() == type::const_kind::KIND_CONST) c = "C";
				if (fs->function_static_kind() == type::static_kind::KIND_STATIC) st = "S";
				string rt = type_2_string(mc, fs->return_type(), false);
				return scope_2_string_helper(mc, s->parent_scope(), r + "function_" + c + st + "_" + rt + "_" + fs->function_name().raw_text() + "_");
			}
				break;
			};
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return "";
		}

		string scope_2_string(shared_ptr<mips_code> mc, shared_ptr<scope> s) {
			if (s == nullptr)
				return "";
			return "__" + scope_2_string_helper(mc, s, "");
		}

		string symbol_2_string_helper(shared_ptr<mips_code> mc, shared_ptr<symbol> s, string r) {
			string scope_string = scope_2_string(mc, s->parent_scope());
			if (s->symbol_kind() == symbol::kind::KIND_VARIABLE) {
				shared_ptr<type> t = static_pointer_cast<variable_symbol>(s)->variable_type();
				return scope_string + type_2_string(mc, t, false) + "_" + static_pointer_cast<variable_symbol>(s)->variable_name().raw_text();
			}
			if (s->symbol_kind() == symbol::kind::KIND_FUNCTION) {
				shared_ptr<function_symbol> fsym = static_pointer_cast<function_symbol>(s);
				shared_ptr<function_type> ft = fsym->function_symbol_type();
				string c, st;
				if (ft->type_const_kind() == type::const_kind::KIND_CONST) c = "C";
				if (ft->type_static_kind() == type::static_kind::KIND_STATIC) st = "S";
				return scope_string + "function_" + c + st + "_" + type_2_string(mc, ft->return_type(), false) + "_" + fsym->function_name().raw_text();
			}
			if (s->symbol_kind() == symbol::kind::KIND_STRUCT) {
				shared_ptr<struct_symbol> ss = static_pointer_cast<struct_symbol>(s);
				return scope_string + "struct_" + ss->struct_name().raw_text();
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return "";
		}

		string symbol_2_string(shared_ptr<mips_code> mc, shared_ptr<symbol> s) {
			if (s == nullptr)
				return "";
			return symbol_2_string_helper(mc, s, "");
		}

		string c_scope_2_string_helper(shared_ptr<mips_code> mc, shared_ptr<scope> s, string r) {
			if (s == nullptr)
				return r;
			else if (s->parent_scope() == nullptr || s->scope_kind() == scope::kind::KIND_GLOBAL)
				return r;
			switch (s->scope_kind()) {
			case scope::kind::KIND_BLOCK:
				return c_scope_2_string_helper(mc, s->parent_scope(), "b" + to_string(static_pointer_cast<block_scope>(s)->block_number()) + "$_" + r);
				break;
			case scope::kind::KIND_LOOP:
				return c_scope_2_string_helper(mc, s->parent_scope(), "l$_" + r);
				break;
			case scope::kind::KIND_SWITCH:
				return c_scope_2_string_helper(mc, s->parent_scope(), "s$_" + r);
				break;
			case scope::kind::KIND_NAMESPACE:
				return c_scope_2_string_helper(mc, s->parent_scope(), static_pointer_cast<namespace_scope>(s)->namespace_name().raw_text() + "_" + r);
			case scope::kind::KIND_FUNCTION: {
				shared_ptr<function_scope> fs = static_pointer_cast<function_scope>(s);
				return c_scope_2_string_helper(mc, s->parent_scope(), fs->function_name().raw_text() + "_" + r);
			}
				break;
			};
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return "";
		}

		string c_scope_2_string(shared_ptr<mips_code> mc, shared_ptr<scope> s) {
			if (s == nullptr)
				return "";
			return "_" + c_scope_2_string_helper(mc, s, "");
		}

		string c_symbol_2_string_helper(shared_ptr<mips_code> mc, shared_ptr<symbol> s, string r) {
			string scope_string = c_scope_2_string(mc, s->parent_scope());
			if (s->symbol_kind() == symbol::kind::KIND_VARIABLE) {
				shared_ptr<type> t = static_pointer_cast<variable_symbol>(s)->variable_type();
				return scope_string + static_pointer_cast<variable_symbol>(s)->variable_name().raw_text();
			}
			if (s->symbol_kind() == symbol::kind::KIND_FUNCTION) {
				shared_ptr<function_symbol> fsym = static_pointer_cast<function_symbol>(s);
				return scope_string + fsym->function_name().raw_text();
			}
			if (s->symbol_kind() == symbol::kind::KIND_STRUCT) {
				shared_ptr<struct_symbol> ss = static_pointer_cast<struct_symbol>(s);
				return scope_string + ss->struct_name().raw_text();
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return "";
		}

		string c_symbol_2_string(shared_ptr<mips_code> mc, shared_ptr<symbol> s) {
			if (s == nullptr)
				return "";
			return c_symbol_2_string_helper(mc, s, "");
		}

		void generate_variable_declaration_mips(shared_ptr<mips_code> mc, shared_ptr<variable_declaration> vd) {
			if (vd == nullptr) return;
			string sym_string = c_symbol_2_string(mc, vd->variable_declaration_symbol());
			shared_ptr<type> vd_type = vd->variable_declaration_type();
			bool on_stack = !(vd->variable_declaration_symbol()->parent_scope()->scope_kind() == scope::kind::KIND_GLOBAL || vd->variable_declaration_symbol()->parent_scope()->scope_kind() == scope::kind::KIND_NAMESPACE ||
					vd_type->type_static_kind() == type::static_kind::KIND_STATIC),
				global_variable = vd->variable_declaration_symbol()->parent_scope()->scope_kind() == scope::kind::KIND_GLOBAL ||
					vd->variable_declaration_symbol()->parent_scope()->scope_kind() == scope::kind::KIND_NAMESPACE,
				static_variable = vd_type->type_static_kind() == type::static_kind::KIND_STATIC;
			shared_ptr<operand> fp = register_file2::_fp_register;
			int sz = (int)calculate_type_size(mc, vd_type);
			int space = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
			shared_ptr<operand> o = nullptr;
			if (vd_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
				if (vd->variable_declaration_initialization_kind() != variable_declaration::initialization_kind::KIND_PRESENT) {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return;
				}
				variant<bool, int, unsigned int, float, double, string> res = evaluate_constant_expression(mc, vd->initialization());
				mc->add_constexpr_mapping(sym_string, res);
				return;
			}
			if (on_stack) {
				mc->current_frame()->update_top_section_size(space);
				o = make_shared<operand>(operand::offset_kind::KIND_TOP, -mc->current_frame()->top_section_size(), fp->register_number(), fp->register_name());
			}
			else {
				o = make_shared<operand>(false, sym_string);
				if (!global_variable || (global_variable && vd->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_NOT_PRESENT)) {
					mc->add_data_directive(make_shared<directive>(3));
					mc->add_data_directive(make_shared<directive>(sym_string, space));
				}
				if (!static_variable && global_variable)
					mc->add_globl_directive(make_shared<directive>(sym_string));
			}
			if (vd_type->type_kind() == type::kind::KIND_PRIMITIVE && vd_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
				shared_ptr<primitive_type> vd_ptype = static_pointer_cast<primitive_type>(vd_type);
				o->set_double_precision(vd_ptype->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE);
				o->set_single_precision(vd_ptype->primitive_type_kind() == primitive_type::kind::KIND_FLOAT);
			}
			mc->current_frame()->add_variable(sym_string, o);
			if (vd->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_PRESENT && !global_variable) {
				shared_ptr<operand> static_check_label = nullptr, initialization_end_label = nullptr;
				if (static_variable) {
					static_check_label = make_shared<operand>(false, sym_string + "_check");
					initialization_end_label = mc->next_label().second;
					mc->add_data_directive(make_shared<directive>(2));
					mc->add_data_directive(make_shared<directive>(sym_string + "_check", 0, true));
					shared_ptr<operand> op = allocate_general_purpose_register(mc);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, op, static_check_label));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_BNE, op, register_file2::_zero_register, initialization_end_label));
					free_general_purpose_register(mc, op);
				}
				shared_ptr<operand> e = generate_assignment_expression_mips(mc, vd->initialization(), false);
				e = load_value_into_register(mc, e, vd->initialization()->assignment_expression_type());
				e = cast_between_types(mc, e, vd->initialization()->assignment_expression_type(), vd->variable_declaration_type());
				insn::kind s = load_store_insn_from_type(mc, vd_type, false);
				if (vd_type->type_kind() == type::kind::KIND_STRUCT && vd_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
					insn::kind l = load_store_insn_from_type(mc, vd_type, true);
					shared_ptr<operand> reg = allocate_general_purpose_register(mc);
					shared_ptr<operand> reg2 = nullptr;
					if (o->operand_kind() == operand::kind::KIND_LABEL) {
						reg2 = allocate_general_purpose_register(mc);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg2, o));
					}
					else
						reg2 = o;
					for (int off = 0; off < sz; off += 4) {
						mc->current_frame()->add_insn_to_body(make_shared<insn>(l, reg, make_shared<operand>(operand::offset_kind::KIND_TRUE, off, e->register_number(), e->register_name())));
						if(o->operand_kind() == operand::kind::KIND_LABEL)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(s, reg, make_shared<operand>(operand::offset_kind::KIND_TRUE, off, reg2->register_number(),
								reg2->register_name())));
						else if (o->operand_kind() == operand::kind::KIND_MEMORY)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(s, reg, make_shared<operand>(o->operand_offset_kind(), o->offset() + off, o->register_number(),
								o->register_name())));
						else {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return;
						}
					}
					if (o->operand_kind() == operand::kind::KIND_LABEL)
						free_general_purpose_register(mc, reg2);
					free_general_purpose_register(mc, reg);
				}
				else
					mc->current_frame()->add_insn_to_body(make_shared<insn>(s, e, o));
				if (e->register_number() > 31)
					free_general_purpose_fp_register(mc, e);
				else
					free_general_purpose_register(mc, e);
				if (static_variable) {
					shared_ptr<operand> op = allocate_general_purpose_register(mc);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, op, register_file2::_zero_register, make_shared<operand>(1)));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, op, static_check_label));
					mc->current_frame()->add_insn_to_body(make_shared<insn>(initialization_end_label));
					free_general_purpose_register(mc, op);
				}
			}
			else if (vd->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_PRESENT && global_variable) {
				variant<bool, int, unsigned int, float, double, string> result = evaluate_constant_expression(mc, vd->initialization());
				shared_ptr<type> t = vd->variable_declaration_type();
				mc->add_data_directive(make_shared<directive>(3));
				if (t->type_array_kind() == type::array_kind::KIND_ARRAY) {
					if (holds_alternative<bool>(result))
						mc->add_data_directive(make_shared<directive>(sym_string, (int)get<bool>(result), true));
					else if (holds_alternative<int>(result))
						mc->add_data_directive(make_shared<directive>(sym_string, get<int>(result), true));
					else if (holds_alternative<unsigned int>(result))
						mc->add_data_directive(make_shared<directive>(sym_string, (int)get<unsigned int>(result), true));
					else if (holds_alternative<float>(result))
						mc->add_data_directive(make_shared<directive>(sym_string, (int)get<float>(result), true));
					else if (holds_alternative<double>(result))
						mc->add_data_directive(make_shared<directive>(sym_string, (int)get<double>(result), true));
					else
						mc->add_data_directive(make_shared<directive>(directive::kind::KIND_WORD, sym_string, vector<shared_ptr<operand>>{ make_shared<operand>(true, get<string>(result)) }));
				}
				else {
					if(t->type_kind() == type::kind::KIND_STRUCT)
						mc->add_data_directive(make_shared<directive>(sym_string, space));
					else if(t->type_kind() == type::kind::KIND_PRIMITIVE) {
						shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
							if (holds_alternative<bool>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, (double)get<bool>(result)));
							else if (holds_alternative<int>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, (double)get<int>(result)));
							else if (holds_alternative<unsigned int>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, (double)get<unsigned int>(result)));
							else if (holds_alternative<float>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, (double)get<float>(result)));
							else if (holds_alternative<double>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, get<double>(result)));
							else {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return;
							}
						}
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
							if (holds_alternative<bool>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, (float)get<bool>(result)));
							else if (holds_alternative<int>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, (float)get<int>(result)));
							else if (holds_alternative<unsigned int>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, (float)get<unsigned int>(result)));
							else if (holds_alternative<float>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, get<float>(result)));
							else if (holds_alternative<double>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, (float)get<double>(result)));
							else {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return;
							}
						}
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_LONG || pt->primitive_type_kind() == primitive_type::kind::KIND_INT) {
							if (holds_alternative<bool>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, (int)get<bool>(result), true));
							else if (holds_alternative<int>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, (int)get<int>(result), true));
							else if (holds_alternative<unsigned int>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, (int)get<unsigned int>(result), true));
							else if (holds_alternative<float>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, (int)get<int>(result), true));
							else if (holds_alternative<double>(result))
								mc->add_data_directive(make_shared<directive>(sym_string, (int)get<double>(result), true));
							else
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_WORD, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>(true, get<string>(result)) }));
						}
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_SHORT) {
							if (holds_alternative<bool>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_HALF, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((short) get<bool>(result)) }));
							else if (holds_alternative<int>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_HALF, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((short) get<int>(result)) }));
							else if (holds_alternative<unsigned int>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_HALF, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((short) get<unsigned int>(result)) }));
							else if (holds_alternative<float>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_HALF, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((short) get<float>(result)) }));
							else if (holds_alternative<double>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_HALF, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((short) get<double>(result)) }));
							else
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_HALF, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>(true, get<string>(result)) }));
						}
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_CHAR || pt->primitive_type_kind() == primitive_type::kind::KIND_BYTE) {
							if (holds_alternative<bool>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_BYTE, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((char) get<bool>(result)) }));
							else if (holds_alternative<int>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_BYTE, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((char) get<int>(result)) }));
							else if (holds_alternative<unsigned int>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_BYTE, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((char) get<unsigned int>(result)) }));
							else if (holds_alternative<float>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_BYTE, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((char) get<float>(result)) }));
							else if (holds_alternative<double>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_BYTE, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((char) get<double>(result)) }));
							else
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_BYTE, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>(true, get<string>(result)) }));
						}
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_BOOL) {
							if (holds_alternative<bool>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_BYTE, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((char) get<bool>(result) != 0) }));
							else if (holds_alternative<int>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_BYTE, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((char) get<int>(result) != 0) }));
							else if (holds_alternative<unsigned int>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_BYTE, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((char) get<unsigned int>(result) != 0) }));
							else if (holds_alternative<float>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_BYTE, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((char) get<float>(result) != 0) }));
							else if (holds_alternative<double>(result))
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_BYTE, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>((char) get<double>(result) != 0) }));
							else
								mc->add_data_directive(make_shared<directive>(directive::kind::KIND_BYTE, sym_string,
									vector<shared_ptr<operand>>{ make_shared<operand>(true, get<string>(result)) }));
						}
					}
					else {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return;
					}
				}
			}
		}

		insn::kind load_store_insn_from_type(shared_ptr<mips_code> mc, shared_ptr<type> t, bool l) {
			if (t == nullptr) return insn::kind::KIND_NONE;
			if (t->type_kind() == type::kind::KIND_FUNCTION && t->type_array_kind() == type::array_kind::KIND_NON_ARRAY)
				return l ? insn::kind::KIND_LW : insn::kind::KIND_SW;
			else if (t->type_kind() == type::kind::KIND_STRUCT && t->type_array_kind() == type::array_kind::KIND_NON_ARRAY)
				return l ? insn::kind::KIND_LW : insn::kind::KIND_SW;
			else if (t->type_kind() == type::kind::KIND_PRIMITIVE && t->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
				shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
				bool s = pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_SIGNED;
				switch (pt->primitive_type_kind()) {
				case primitive_type::kind::KIND_BOOL:
					return l ? insn::kind::KIND_LBU : insn::kind::KIND_SB;
					break;
				case primitive_type::kind::KIND_CHAR:
				case primitive_type::kind::KIND_BYTE:
					return l ? (s ? insn::kind::KIND_LB : insn::kind::KIND_LBU) : insn::kind::KIND_SB;
					break;
				case primitive_type::kind::KIND_SHORT:
					return l ? (s ? insn::kind::KIND_LH : insn::kind::KIND_LHU) : insn::kind::KIND_SH;
					break;
				case primitive_type::kind::KIND_INT:
				case primitive_type::kind::KIND_LONG:
					return l ? insn::kind::KIND_LW : insn::kind::KIND_SW;
					break;
				case primitive_type::kind::KIND_FLOAT:
					return l ? insn::kind::KIND_LWC1 : insn::kind::KIND_SWC1;
					break;
				case primitive_type::kind::KIND_DOUBLE:
					return l ? insn::kind::KIND_LDC1 : insn::kind::KIND_SDC1;
					break;
				default:
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return insn::kind::KIND_NONE;
					break;
				}
			}
			else if (t->type_array_kind() == type::array_kind::KIND_ARRAY)
				return l ? insn::kind::KIND_LW : insn::kind::KIND_SW;
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return insn::kind::KIND_NONE;
		}

		void generate_function_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<function_stmt> fs) {
			if (fs == nullptr || fs->function_stmt_defined_kind() != function_stmt::defined_kind::KIND_DEFINITION) return;
			pair<int, shared_ptr<operand>> ret_lab = mc->next_label();
			shared_ptr<operand> sp = register_file2::_sp_register, fp = register_file2::_fp_register, ra = register_file2::_ra_register;
			string fsym = "";
			bool main_function = is_function_main(mc, fs->function_stmt_symbol());
			if (main_function)
#if SYSTEM == 0
				fsym = "__start";
#elif SYSTEM == 1
				fsym = "main";
#elif SYSTEM == 2
				fsym = "main";
#endif
			else
				fsym = c_symbol_2_string(mc, fs->function_stmt_symbol());
			bool is_static = fs->function_stmt_type()->type_static_kind() == type::static_kind::KIND_STATIC;
			if (!is_static)
				mc->add_globl_directive(make_shared<directive>(fsym));
			shared_ptr<type> ret_type = fs->function_stmt_type()->return_type();
			bool ret_is_prim = ret_type->type_kind() == type::kind::KIND_PRIMITIVE && ret_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY;
			map<string, shared_ptr<operand>> old_map = mc->current_frame()->variable_storage_map();
			int old_frame_size = mc->current_frame()->top_section_size() + mc->current_frame()->middle_section_size() + mc->current_frame()->frame_size();
			mc->push_frame(make_shared<mips_frame>(main_function, false, ret_is_prim && static_pointer_cast<primitive_type>(ret_type)->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE,
				ret_is_prim && static_pointer_cast<primitive_type>(ret_type)->primitive_type_kind() == primitive_type::kind::KIND_FLOAT));
#if SYSTEM == 0 || SYSTEM == 1
			mc->current_frame()->mark_register(31);
#elif SYSTEM == 2
			mc->current_frame()->mark_register(1);
#endif
			mc->current_frame()->set_return_label(ret_lab.second);
			mc->current_frame()->set_return_label_set(true);
			mc->current_frame()->set_return_type(ret_type);
			mc->current_frame()->set_return_type_set(true);
			for (pair<string, shared_ptr<operand>> p : old_map)
				mc->current_frame()->add_variable(p.first, p.second);
			vector<int> register_args, fp_register_args;
			shared_ptr<operand> _a0 = register_file2::_a0_register;
#if SYSTEM == 0 || SYSTEM == 1
			shared_ptr<operand> _f12 = register_file2::_f12_register;
#elif SYSTEM == 2
			shared_ptr<operand> _f10 = register_file2::_f10_register;
#endif
			vector<shared_ptr<variable_declaration>> parm_list = fs->function_stmt_type()->parameter_list();
			mc->current_frame()->update_top_section_size(16);
			for (int i = 0; i < parm_list.size(); i++) {
				shared_ptr<variable_declaration> vdecl = parm_list[i];
				shared_ptr<type> curr_type = vdecl->variable_declaration_type();
				bool is_prim = curr_type->type_kind() == type::kind::KIND_PRIMITIVE && curr_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY;
				bool is_not_struct = !(curr_type->type_kind() == type::kind::KIND_STRUCT && curr_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY);
				bool is_fp = static_pointer_cast<primitive_type>(curr_type)->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE ||
					static_pointer_cast<primitive_type>(curr_type)->primitive_type_kind() == primitive_type::kind::KIND_FLOAT;
#if SYSTEM == 0 || SYSTEM == 1
				if (fp_register_args.size() < 2 && (is_prim && is_fp) && !fs->function_stmt_type()->variadic())
					fp_register_args.push_back(i);
#elif SYSTEM == 2
				if (fp_register_args.size() < 8 && (is_prim && is_fp) && !fs->function_stmt_type()->variadic())
					fp_register_args.push_back(i);
#endif
#if SYSTEM == 0 || SYSTEM == 1
				else if (register_args.size() < 4 && is_not_struct && !(is_fp && is_prim) && !fs->function_stmt_type()->variadic())
#elif SYSTEM == 2
				else if (register_args.size() < 8 && is_not_struct && !(is_fp && is_prim) && !fs->function_stmt_type()->variadic())
#endif
						register_args.push_back(i);
			}
			int curr_off = 0;
			for (int i = 0; i < parm_list.size(); i++) {
				shared_ptr<variable_declaration> vdecl = parm_list[i];
				shared_ptr<type> curr_type = vdecl->variable_declaration_type();
				string sym_name = c_symbol_2_string(mc, vdecl->variable_declaration_symbol());
				int fp_index = find(fp_register_args.begin(), fp_register_args.end(), i) - fp_register_args.begin(),
					reg_index = find(register_args.begin(), register_args.end(), i) - register_args.begin();
				if (fp_index < fp_register_args.size()) {
					if (fs->function_stmt_type()->variadic()) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return;
					}
					if (curr_type->type_kind() != type::kind::KIND_PRIMITIVE && curr_type->type_array_kind() != type::array_kind::KIND_NON_ARRAY) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return;
					}
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(curr_type);
#if SYSTEM == 0 || SYSTEM == 1
					shared_ptr<operand> correct_reg = register_file2::int_2_register_object.at(_f12->register_number() + fp_index * 2);
#elif SYSTEM == 2
					shared_ptr<operand> correct_reg = register_file2::int_2_register_object.at(_f10->register_number() + fp_index);
#endif
					if (pt->primitive_type_kind() != primitive_type::kind::KIND_DOUBLE && pt->primitive_type_kind() != primitive_type::kind::KIND_FLOAT) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return;
					}
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
						mc->current_frame()->update_top_section_size(8);
						shared_ptr<operand> dop = make_shared<operand>(operand::offset_kind::KIND_TOP, -mc->current_frame()->top_section_size(), fp->register_number(), fp->register_name());
						dop->set_double_precision(true);
						dop->set_single_precision(false);
						mc->current_frame()->add_variable(sym_name, dop);
						mc->current_frame()->add_insn_to_prologue(make_shared<insn>(insn::kind::KIND_SDC1, correct_reg, dop));
					}
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
						mc->current_frame()->update_top_section_size(8);
						shared_ptr<operand> fop = make_shared<operand>(operand::offset_kind::KIND_TOP, -mc->current_frame()->top_section_size(), fp->register_number(), fp->register_name());
						fop->set_double_precision(false);
						fop->set_single_precision(true);
						mc->current_frame()->add_variable(sym_name, fop);
						mc->current_frame()->add_insn_to_prologue(make_shared<insn>(insn::kind::KIND_SWC1, correct_reg, fop));
					}
					else {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return;
					}
				}
				else if (reg_index < register_args.size()) {
					if (fs->function_stmt_type()->variadic()) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return;
					}
					int sz = (int)calculate_type_size(mc, curr_type);
					sz = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
					mc->current_frame()->update_top_section_size(sz);
					shared_ptr<operand> mem = make_shared<operand>(operand::offset_kind::KIND_TOP, -mc->current_frame()->top_section_size(), fp->register_number(), fp->register_name());
					mc->current_frame()->add_variable(sym_name, mem);
					insn::kind s = load_store_insn_from_type(mc, curr_type, false);
					shared_ptr<operand> correct_reg = register_file2::int_2_register_object.at(_a0->register_number() + reg_index);
					mc->current_frame()->add_insn_to_prologue(make_shared<insn>(s, correct_reg, mem));
				}
				else {
					int sz = (int)calculate_type_size(mc, curr_type);
					sz = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
					mc->current_frame()->update_top_section_size(sz);
					shared_ptr<operand> mem = make_shared<operand>(operand::offset_kind::KIND_TOP, -mc->current_frame()->top_section_size(), fp->register_number(), fp->register_name());
					bool is_sp = curr_type->type_kind() == type::kind::KIND_PRIMITIVE && curr_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY &&
						static_pointer_cast<primitive_type>(curr_type)->primitive_type_kind() == primitive_type::kind::KIND_FLOAT,
						is_dp = curr_type->type_kind() == type::kind::KIND_PRIMITIVE && curr_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY &&
						static_pointer_cast<primitive_type>(curr_type)->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE;
					mem->set_double_precision(is_dp);
					mem->set_single_precision(is_sp);
					mc->current_frame()->add_variable(sym_name, mem);
					if (curr_type->type_kind() == type::kind::KIND_STRUCT && curr_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
						shared_ptr<operand> temp = allocate_general_purpose_register(mc);
						for (int o = 0; o < sz; o += 4) {
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, temp, make_shared<operand>(operand::offset_kind::KIND_TOP, curr_off + o,
								fp->register_number(), fp->register_name())));
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, temp, make_shared<operand>(operand::offset_kind::KIND_TOP,
								-mc->current_frame()->top_section_size() + o, fp->register_number(), fp->register_name())));
						}
						free_general_purpose_register(mc, temp);
						curr_off += sz;
					}
					else {
						insn::kind s = load_store_insn_from_type(mc, curr_type, false),
							l = load_store_insn_from_type(mc, curr_type, true);
						shared_ptr<operand> temp = nullptr;
						if (is_sp || is_dp)
							temp = allocate_general_purpose_fp_register(mc);
						else
							temp = allocate_general_purpose_register(mc);
						if (is_sp) {
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LWC1, temp, make_shared<operand>(operand::offset_kind::KIND_TOP, curr_off,
								fp->register_number(), fp->register_name())));
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SWC1, temp, mem));
						}
						else if (is_dp) {
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LDC1, temp, make_shared<operand>(operand::offset_kind::KIND_TOP, curr_off,
								fp->register_number(), fp->register_name())));
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SDC1, temp, mem));
						}
						else {
							mc->current_frame()->add_insn_to_body(make_shared<insn>(l, temp, make_shared<operand>(operand::offset_kind::KIND_TOP, curr_off, fp->register_number(),
								fp->register_name())));
							mc->current_frame()->add_insn_to_body(make_shared<insn>(s, temp, mem));
						}
						if (is_sp || is_dp)
							free_general_purpose_fp_register(mc, temp);
						else
							free_general_purpose_register(mc, temp);
						curr_off += sz;
					}
				}
			}
			mc->set_inside_function(true);
			for (shared_ptr<stmt> s : fs->function_body())
				generate_stmt_mips(mc, s);
			mc->set_inside_function(false);
			mc->current_frame()->set_frame_size_set(true);
			mc->current_frame()->set_frame_size(mc->current_frame()->top_section_size() + mc->current_frame()->middle_section_size() + mc->current_frame()->frame_size());
			mc->current_frame()->restore_saved_registers(mc);
			if (main_function && parm_list.size() == 2) {
				shared_ptr<operand> argv_location = make_shared<operand>(4);
				argv_location->set_operand_offset_kind(operand::offset_kind::KIND_TOP);
				mc->current_frame()->add_insn_to_prologue(make_shared<insn>(insn::kind::KIND_ADDIU, register_file2::_a1_register, fp, argv_location), true);
				mc->current_frame()->add_insn_to_prologue(make_shared<insn>(insn::kind::KIND_LW, register_file2::_a0_register, make_shared<operand>(operand::offset_kind::KIND_TOP, 0, fp->register_number(), fp->register_name())), true);
			}
			mc->current_frame()->add_insn_to_prologue(make_shared<insn>(insn::kind::KIND_ADDU, fp, sp, register_file2::_zero_register), true);
			mc->current_frame()->add_insn_to_prologue(make_shared<insn>(insn::kind::KIND_SW, ra, make_shared<operand>(operand::offset_kind::KIND_TOP, -16, sp->register_number(),
				sp->register_name())), true);
			mc->current_frame()->add_insn_to_prologue(make_shared<insn>(insn::kind::KIND_SW, fp, make_shared<operand>(operand::offset_kind::KIND_TOP, -8, sp->register_number(),
				sp->register_name())), true);
			mc->current_frame()->add_insn_to_prologue(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<operand>(-mc->current_frame()->frame_size())), true);
			if (main_function)
#if SYSTEM == 0
				;
#elif SYSTEM == 1
				mc->current_frame()->add_insn_to_prologue(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<operand>(-4)), true);
#elif SYSTEM == 2
				;
#endif
				mc->current_frame()->add_insn_to_prologue(make_shared<insn>(make_shared<operand>(false, fsym)), true);
			mc->current_frame()->add_insn_to_prologue(make_shared<insn>(""), true);
			mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(ret_lab.second));
			mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_LW, ra, make_shared<operand>(operand::offset_kind::KIND_TOP, -16, fp->register_number(),
				fp->register_name())));
			mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_LW, fp, make_shared<operand>(operand::offset_kind::KIND_TOP, -8, fp->register_number(),
				fp->register_name())));
			mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<operand>(mc->current_frame()->frame_size())));
			if (main_function) {
#if SYSTEM == 0 || SYSTEM == 1
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, register_file2::_v0_register, register_file2::_zero_register, register_file2::_zero_register));
#elif SYSTEM == 2
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, register_file2::_a0_register, register_file2::_zero_register, register_file2::_zero_register));
#endif
#if SYSTEM == 1
				mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<operand>(4)));
#endif
#if SYSTEM == 0 || SYSTEM == 1
				mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_ADDU, register_file2::_a0_register, register_file2::_v0_register, register_file2::_zero_register));
#endif
#if PROG_TERM == 0
#if SYSTEM == 0
				mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_ADDIU, register_file2::_v0_register, register_file2::_zero_register, make_shared<operand>(4001)));
#elif SYSTEM == 1
				mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_ADDIU, register_file2::_v0_register, register_file2::_zero_register, make_shared<operand>(17)));
#endif
#if SYSTEM == 0 || SYSTEM == 1
				mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_SYSCALL));
#elif SYSTEM == 2
				mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_ADDU, register_file2::_a1_register, register_file2::_zero_register, register_file2::_a0_register));
				mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_ADDIU, register_file2::_a0_register, register_file2::_zero_register, make_shared<operand>(10)));
				mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_SYSCALL));
#endif
#elif PROG_TERM == 1
				mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_JAL, make_shared<operand>(false, program_termination_hook)));
#endif
				mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(""));
			}
			else
				mc->current_frame()->add_insn_to_epilogue(make_shared<insn>(insn::kind::KIND_JR, ra));
			mc->current_frame()->calculate_true_insn_offsets();
#if SYSTEM == 0 || SYSTEM == 1
			mc->current_frame()->unmark_register(31);
#elif SYSTEM == 2
			mc->current_frame()->unmark_register(1);
#endif
			vector<shared_ptr<insn>> fnal, pre = mc->current_frame()->prologue_insn_list(), bod = mc->current_frame()->body_insn_list(), post = mc->current_frame()->epilogue_insn_list();
			mc->pop_frame();
			fnal.insert(fnal.end(), pre.begin(), pre.end());
			fnal.insert(fnal.end(), bod.begin(), bod.end());
			fnal.insert(fnal.end(), post.begin(), post.end());
			for (shared_ptr<insn> i : fnal)
				mc->current_frame()->add_insn_to_body(i);
		}

		bool is_function_main(shared_ptr<mips_code> mc, shared_ptr<function_symbol> fsym) {
			if (fsym == nullptr) return false;
			shared_ptr<function_type> ft = fsym->function_symbol_type();
			token name = fsym->function_name();
			if (name.raw_text() == "main" && fsym->parent_scope()->scope_kind() == scope::kind::KIND_GLOBAL) {
				shared_ptr<type> rt = ft->return_type();
				if (rt->type_kind() == type::kind::KIND_PRIMITIVE && rt->type_array_kind() == type::array_kind::KIND_NON_ARRAY &&
					rt->type_const_kind() == type::const_kind::KIND_NON_CONST && rt->type_static_kind() == type::static_kind::KIND_NON_STATIC &&
					ft->parameter_list().size() == 0 && ft->type_const_kind() == type::const_kind::KIND_NON_CONST && ft->type_static_kind() == type::static_kind::KIND_NON_STATIC) {
					shared_ptr<primitive_type> prt = static_pointer_cast<primitive_type>(rt);
					if (prt->primitive_type_kind() == primitive_type::kind::KIND_INT && prt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_SIGNED) return true;
				}
				else if (rt->type_kind() == type::kind::KIND_PRIMITIVE && rt->type_array_kind() == type::array_kind::KIND_NON_ARRAY &&
					rt->type_const_kind() == type::const_kind::KIND_NON_CONST && rt->type_static_kind() == type::static_kind::KIND_NON_STATIC &&
					ft->parameter_list().size() == 2 && ft->type_const_kind() == type::const_kind::KIND_NON_CONST && ft->type_static_kind() == type::static_kind::KIND_NON_STATIC) {
					shared_ptr<primitive_type> prt = static_pointer_cast<primitive_type>(rt);
					if (prt->primitive_type_kind() != primitive_type::kind::KIND_INT && prt->primitive_type_sign_kind() != primitive_type::sign_kind::KIND_SIGNED) return false;
					shared_ptr<variable_declaration> potential_argc = ft->parameter_list()[0];
					shared_ptr<variable_declaration> potential_argv = ft->parameter_list()[1];
					shared_ptr<type> potential_argc_type = potential_argc->variable_declaration_type(),
						potential_argv_type = potential_argv->variable_declaration_type();
					if (potential_argc_type->type_kind() != type::kind::KIND_PRIMITIVE || potential_argc_type->array_dimensions() != 0 || potential_argc_type->type_array_kind() != type::array_kind::KIND_NON_ARRAY ||
						potential_argc_type->type_static_kind() != type::static_kind::KIND_NON_STATIC) return false;
					if (potential_argv_type->type_kind() != type::kind::KIND_PRIMITIVE || potential_argv_type->array_dimensions() != 2 || potential_argv_type->type_array_kind() != type::array_kind::KIND_ARRAY ||
						potential_argv_type->type_static_kind() != type::static_kind::KIND_NON_STATIC) return false;
					shared_ptr<primitive_type> potential_argc_prim_type = static_pointer_cast<primitive_type>(potential_argc_type),
						potential_argv_prim_type = static_pointer_cast<primitive_type>(potential_argv_type);
					if (potential_argc_prim_type->primitive_type_kind() == primitive_type::kind::KIND_INT && potential_argv_prim_type->primitive_type_kind() == primitive_type::kind::KIND_CHAR) return true;
				}
			}
			return false;
		}

		void generate_return_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<return_stmt> rs) {
			if (rs == nullptr) return;
			if (!mc->current_frame()->return_label_set() || !mc->current_frame()->return_type_set()) {
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return;
			}
			if (rs->return_stmt_value_kind() == return_stmt::value_kind::KIND_EXPRESSION) {
				shared_ptr<expression> e = rs->return_value();
				shared_ptr<operand> op = generate_expression_mips(mc, e, false);
				op = load_value_into_register(mc, op, e->expression_type());
				op = cast_between_types(mc, op, e->expression_type(), mc->current_frame()->return_type());
#if SYSTEM == 0 || SYSTEM == 1
				shared_ptr<operand> f0 = register_file2::_f0_register, v0 = register_file2::_v0_register;
#elif SYSTEM == 2
				shared_ptr<operand> f10 = register_file2::_f0_register, v0 = register_file2::_a0_register;
#endif
				shared_ptr<type> ret_type = mc->current_frame()->return_type();
				if (ret_type->type_kind() == type::kind::KIND_PRIMITIVE && ret_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
					shared_ptr<primitive_type> ret_ptype = static_pointer_cast<primitive_type>(ret_type);
					if (ret_ptype->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE || ret_ptype->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
#if SYSTEM == 0 || SYSTEM == 1
						if (ret_ptype->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MOV_D, f0, op));
						else
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MOV_S, f0, op));
#elif SYSTEM == 2
						if (ret_ptype->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MOV_D, f10, op));
						else
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_MOV_S, f10, op));
#endif
					}
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, v0, op, register_file2::_zero_register));
				}
				else
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, v0, op, register_file2::_zero_register));
				if (op->register_number() > 31)
					free_general_purpose_fp_register(mc, op);
				else
					free_general_purpose_register(mc, op);
			}
			mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_J, mc->current_frame()->return_label()));
		}

		void generate_if_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<if_stmt> is) {
			if (is == nullptr) return;
			if (is->if_stmt_init_decl_kind() == if_stmt::init_decl_kind::KIND_INIT_DECL_PRESENT) {
				vector<shared_ptr<variable_declaration>> vdecl_list = is->init_decl_list();
				for (shared_ptr<variable_declaration> vdecl : vdecl_list)
					generate_variable_declaration_mips(mc, vdecl);
			}
			shared_ptr<expression> cond_expr = is->condition();
			shared_ptr<operand> cond = generate_expression_mips(mc, cond_expr, false);
			cond = load_value_into_register(mc, cond, cond_expr->expression_type());
			pair<int, shared_ptr<operand>> false_path_lab = mc->next_label(), done_path_label = mc->next_label();
			mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_BEQ, cond, register_file2::_zero_register, false_path_lab.second));
			if (cond->register_number() > 31)
				free_general_purpose_fp_register(mc, cond);
			else
				free_general_purpose_register(mc, cond);
			generate_stmt_mips(mc, is->true_path());
			mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_J, done_path_label.second));
			mc->current_frame()->add_insn_to_body(make_shared<insn>(false_path_lab.second));
			if (is->if_stmt_else_kind() == if_stmt::else_kind::KIND_ELSE_PRESENT)
				generate_stmt_mips(mc, is->false_path());
			mc->current_frame()->add_insn_to_body(make_shared<insn>(done_path_label.second));
		}

		void generate_while_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<while_stmt> ws) {
			if (ws == nullptr || ws->while_stmt_kind() != while_stmt::while_kind::KIND_WHILE) return;
			pair<int, shared_ptr<operand>> start_lab = mc->next_label(), end_lab = mc->next_label();
			mc->current_frame()->add_insn_to_body(make_shared<insn>(start_lab.second));
			shared_ptr<expression> cond_expr = ws->condition();
			shared_ptr<operand> op = generate_expression_mips(mc, cond_expr, false);
			op = load_value_into_register(mc, op, cond_expr->expression_type());
			mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_BEQ, op, register_file2::_zero_register, end_lab.second));
			free_general_purpose_register(mc, op);
			mc->current_frame()->push_break_stack(end_lab.second);
			mc->current_frame()->push_continue_stack(start_lab.second);
			generate_stmt_mips(mc, ws->while_body());
			mc->current_frame()->pop_continue_stack();
			mc->current_frame()->pop_break_stack();
			mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_J, start_lab.second));
			mc->current_frame()->add_insn_to_body(make_shared<insn>(end_lab.second));
		}

		void generate_block_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<block_stmt> bs) {
			if (bs == nullptr) return;
			for (shared_ptr<stmt> s : bs->stmt_list())
				generate_stmt_mips(mc, s);
		}

		void generate_break_continue_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<break_continue_stmt> bcs) {
			if (bcs == nullptr) return;
			if (bcs->break_continue_stmt_kind() == break_continue_stmt::kind::KIND_BREAK)
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_J, mc->current_frame()->current_break()));
			else if (bcs->break_continue_stmt_kind() == break_continue_stmt::kind::KIND_CONTINUE)
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_J, mc->current_frame()->current_continue()));
			else {
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return;
			}
		}

		void generate_switch_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<switch_stmt> ss) {
			if (ss == nullptr) return;
			shared_ptr<expression> base_expr = ss->parent_expression();
			shared_ptr<type> base_type = ss->parent_expression()->expression_type();
			bool dp_eq = base_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY && base_type->type_kind() == type::kind::KIND_PRIMITIVE &&
				static_pointer_cast<primitive_type>(base_type)->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE,
				sp_eq = base_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY && base_type->type_kind() == type::kind::KIND_PRIMITIVE &&
				static_pointer_cast<primitive_type>(base_type)->primitive_type_kind() == primitive_type::kind::KIND_FLOAT;
			shared_ptr<operand> op = generate_expression_mips(mc, base_expr, false);
			op = load_value_into_register(mc, op, base_type);
			pair<int, shared_ptr<operand>> default_label = make_pair(-1, nullptr), end_label = mc->next_label();
			mc->current_frame()->push_break_stack(end_label.second);
			for (shared_ptr<case_default_stmt> cds : ss->switch_stmt_scope()->case_default_stmt_list()) {
				if (cds->case_default_stmt_kind() == case_default_stmt::kind::KIND_CASE) {
					shared_ptr<operand> case_label = make_shared<operand>(false, "$C" + to_string(cds->case_default_stmt_label_number()));
					shared_ptr<expression> child_expr = cds->case_expression();
					shared_ptr<type> child_type = cds->case_expression()->expression_type();
					shared_ptr<operand> case_cond = generate_expression_mips(mc, child_expr, false);
					case_cond = load_value_into_register(mc, case_cond, child_type);
					case_cond = cast_between_types(mc, case_cond, child_type, base_type);
					if (dp_eq || sp_eq) {
#if SYSTEM == 0 || SYSTEM == 1
						mc->current_frame()->add_insn_to_body(make_shared<insn>(dp_eq ? insn::kind::KIND_C_EQ_D : insn::kind::KIND_C_EQ_S, op, case_cond));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_BC1T, case_label));
#elif SYSTEM == 2
						shared_ptr<operand> temp = allocate_general_purpose_register(mc);
						mc->current_frame()->add_insn_to_body(make_shared<insn>(dp_eq ? insn::kind::KIND_C_EQ_D : insn::kind::KIND_C_EQ_S, temp, op, case_cond));
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_BNE, temp, register_file2::_zero_register, case_label));
						free_general_purpose_register(mc, temp);
#endif
					}
					else
						mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_BEQ, op, case_cond, case_label));
					if (case_cond->register_number() > 31)
						free_general_purpose_fp_register(mc, case_cond);
					else
						free_general_purpose_register(mc, case_cond);
				}
				else if (cds->case_default_stmt_kind() == case_default_stmt::kind::KIND_DEFAULT)
					default_label = make_pair(cds->case_default_stmt_label_number(), make_shared<operand>(false, "$D" + to_string(cds->case_default_stmt_label_number())));
			}
			if (ss->switch_stmt_scope()->default_stmt_hit())
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_J, default_label.second));
			mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_J, end_label.second));
			if (op->register_number() > 31)
				free_general_purpose_fp_register(mc, op);
			else
				free_general_purpose_register(mc, op);
			generate_stmt_mips(mc, ss->switch_stmt_body());
			mc->current_frame()->add_insn_to_body(make_shared<insn>(end_label.second));
			mc->current_frame()->pop_break_stack();
		}

		void generate_case_default_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<case_default_stmt> cds) {
			if (cds == nullptr) return;
			mc->current_frame()->add_insn_to_body(make_shared<insn>(make_shared<operand>(false, (cds->case_default_stmt_kind() == case_default_stmt::kind::KIND_CASE ? "$C" : "$D") +
				to_string(cds->case_default_stmt_label_number()))));
			generate_stmt_mips(mc, cds->case_default_stmt_body());
		}

		void generate_do_while_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<while_stmt> dws) {
			if (dws == nullptr || dws->while_stmt_kind() != while_stmt::while_kind::KIND_DO_WHILE) return;
			pair<int, shared_ptr<operand>> start_label = mc->next_label(), end_label = mc->next_label();
			mc->current_frame()->add_insn_to_body(make_shared<insn>(start_label.second));
			mc->current_frame()->push_break_stack(end_label.second);
			mc->current_frame()->push_continue_stack(start_label.second);
			generate_stmt_mips(mc, dws->while_body());
			mc->current_frame()->pop_continue_stack();
			mc->current_frame()->pop_break_stack();
			shared_ptr<operand> cond = generate_expression_mips(mc, dws->condition(), false);
			cond = load_value_into_register(mc, cond, dws->condition()->expression_type());
			mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_BEQ, cond, register_file2::_zero_register, end_label.second));
			if (cond->register_number() > 31)
				free_general_purpose_fp_register(mc, cond);
			else
				free_general_purpose_register(mc, cond);
			mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_J, start_label.second));
			mc->current_frame()->add_insn_to_body(make_shared<insn>(end_label.second));
		}

		tuple<vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>> reorder_stmt_list(shared_ptr<mips_code> mc, vector<shared_ptr<stmt>> sl) {
			vector<shared_ptr<stmt>> reordered;
			vector<shared_ptr<stmt>> _vdecls, _functions, _main, _rest;
			for (shared_ptr<stmt> s : sl) {
				if (s->stmt_kind() == stmt::kind::KIND_VARIABLE_DECLARATION || s->stmt_kind() == stmt::kind::KIND_ACCESS)
					_vdecls.push_back(s);
				else if (s->stmt_kind() == stmt::kind::KIND_FUNCTION) {
					if (is_function_main(mc, s->stmt_function()->function_stmt_symbol()))
						_main.push_back(s);
					else
						_functions.push_back(s);
				}
				else if (s->stmt_kind() == stmt::kind::KIND_NAMESPACE) {
					tuple<vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>> reordered_namespace = reorder_stmt_list(mc,
						s->stmt_namespace()->namespace_stmt_list());
					_vdecls.insert(_vdecls.end(), get<0>(reordered_namespace).begin(), get<0>(reordered_namespace).end());
					_functions.insert(_functions.end(), get<1>(reordered_namespace).begin(), get<1>(reordered_namespace).end());
					_main.insert(_main.end(), get<2>(reordered_namespace).begin(), get<2>(reordered_namespace).end());
					_rest.insert(_rest.end(), get<3>(reordered_namespace).begin(), get<3>(reordered_namespace).end());
				}
				else if (s->stmt_kind() == stmt::kind::KIND_STRUCT || s->stmt_kind() == stmt::kind::KIND_EMPTY || s->stmt_kind() == stmt::kind::KIND_USING ||
					s->stmt_kind() == stmt::kind::KIND_INCLUDE)
					_rest.push_back(s);
				else {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return make_tuple(_vdecls, _functions, _main, _rest);
				}
			}
			return make_tuple(_vdecls, _functions, _main, _rest);
		}

		void generate_for_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<for_stmt> fs) {
			if (fs == nullptr) return;
			pair<int, shared_ptr<operand>> start_label = mc->next_label(), end_label = mc->next_label();
			if (fs->for_stmt_initializer_present_kind() == for_stmt::initializer_present_kind::INITIALIZER_PRESENT) {
				vector<shared_ptr<variable_declaration>> vdecl_list = fs->initializer();
				for (shared_ptr<variable_declaration> vdecl : vdecl_list)
					generate_variable_declaration_mips(mc, vdecl);
			}
			mc->current_frame()->add_insn_to_body(make_shared<insn>(start_label.second));
			if (fs->for_stmt_condition_present_kind() == for_stmt::condition_present_kind::CONDITION_PRESENT) {
				shared_ptr<expression> cond_expr = fs->condition();
				shared_ptr<type> cond_type = cond_expr->expression_type();
				shared_ptr<operand> cond_op = generate_expression_mips(mc, cond_expr, false);
				cond_op = load_value_into_register(mc, cond_op, cond_type);
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_BEQ, cond_op, register_file2::_zero_register, end_label.second));
				if (cond_op->register_number() > 31)
					free_general_purpose_fp_register(mc, cond_op);
				else
					free_general_purpose_register(mc, cond_op);
			}
			mc->current_frame()->push_break_stack(end_label.second);
			mc->current_frame()->push_continue_stack(start_label.second);
			generate_stmt_mips(mc, fs->for_stmt_body());
			mc->current_frame()->pop_break_stack();
			mc->current_frame()->pop_continue_stack();
			if (fs->for_stmt_update_present_kind() == for_stmt::update_present_kind::UPDATE_PRESENT) {
				shared_ptr<expression> update_expr = fs->update();
				shared_ptr<type> update_type = update_expr->expression_type();
				shared_ptr<operand> update_op = generate_expression_mips(mc, update_expr, false);
				update_op = load_value_into_register(mc, update_op, update_type);
				if (update_op->register_number() > 31)
					free_general_purpose_fp_register(mc, update_op);
				else
					free_general_purpose_register(mc, update_op);
			}
			mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_J, start_label.second));
			mc->current_frame()->add_insn_to_body(make_shared<insn>(end_label.second));
		}

		void generate_asm_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<asm_stmt> as) {
			if (as == nullptr) return;
			for (shared_ptr<asm_stmt::asm_type> at : as->asm_type_list()) {
				if (at->asm_type_kind() == asm_stmt::kind::KIND_RAW_STRING) {
					string raw_stmt;
					for (token t : at->raw_stmt())
						raw_stmt += t.raw_text().substr(1, t.raw_text().length() - 2);
					mc->current_frame()->add_insn_to_body(make_shared<insn>(raw_stmt));
				}
				else if (at->asm_type_kind() == asm_stmt::kind::KIND_LA) {
					string raw_reg;
					for (token t : at->raw_register())
						raw_reg += t.raw_text().substr(1, t.raw_text().length() - 2);
					shared_ptr<operand> reg_label = make_shared<operand>(false, raw_reg);
					string sym_string = c_symbol_2_string(mc, at->identifier_symbol());
					if (at->identifier_symbol()->symbol_kind() == symbol::kind::KIND_FUNCTION) {
						if (is_function_main(mc, static_pointer_cast<function_symbol>(at->identifier_symbol())))
#if SYSTEM == 0
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg_label, make_shared<operand>(false, "__start")));
#elif SYSTEM == 1
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg_label, make_shared<operand>(false, "main")));
#elif SYSTEM == 2
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg_label, make_shared<operand>(false, "main")));
#endif
						else
							mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg_label, make_shared<operand>(false, sym_string)));
					}
					else if (at->identifier_symbol()->symbol_kind() == symbol::kind::KIND_VARIABLE) {
						pair<bool, variant<bool, int, unsigned int, float, double, string>> check = mc->get_constexpr_mapping(sym_string);
						if (!check.first) {
							shared_ptr<operand> location = mc->current_frame()->get_variable_offset(sym_string);
							if (location->operand_kind() == operand::kind::KIND_LABEL)
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LA, reg_label, location));
							else if (location->operand_kind() == operand::kind::KIND_MEMORY) {
								shared_ptr<operand> imm = make_shared<operand>(location->offset());
								imm->set_operand_offset_kind(location->operand_offset_kind());
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDIU, reg_label, register_file2::int_2_register_object.at(location->register_number()), imm));
							}
							else if (location->operand_kind() == operand::kind::KIND_REGISTER)
								mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_ADDU, reg_label, register_file2::_zero_register, reg_label));
							else {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return;
							}
						}
						else
							generate_constexpr_identifier_mips(mc, at->identifier_symbol(), check, reg_label);
					}
					else {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return;
					}
				}
				else {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return;
				}
			}
		}

		template<typename C, typename TL, typename TR> auto raw_arithmetic_binary_expression_evaluator(shared_ptr<mips_code> mc, TL lhs, TR rhs, binary_expression::operator_kind ok) {
			switch (ok) {
			case binary_expression::operator_kind::KIND_ADD:
				return (C)lhs + (C)rhs;
				break;
			case binary_expression::operator_kind::KIND_BITWISE_AND:
				return (C)((int)lhs & (int)rhs);
				break;
			case binary_expression::operator_kind::KIND_BITWISE_OR:
				return (C)((int)lhs | (int)rhs);
				break;
			case binary_expression::operator_kind::KIND_BITWISE_XOR:
				return (C)((int)lhs ^ (int)rhs);
				break;
			case binary_expression::operator_kind::KIND_DIVIDE:
				return (C)lhs / (C)rhs;
				break;
			case binary_expression::operator_kind::KIND_MODULUS:
				return (C)((int)lhs % (int)rhs);
				break;
			case binary_expression::operator_kind::KIND_MULTIPLY:
				return (C)lhs * (C)rhs;
				break;
			case binary_expression::operator_kind::KIND_SHIFT_LEFT:
				return (C)((int)lhs << (unsigned int)rhs);
				break;
			case binary_expression::operator_kind::KIND_SHIFT_RIGHT:
				return (C)((int)lhs >> (unsigned int)rhs);
				break;
			case binary_expression::operator_kind::KIND_SUBTRACT:
				return (C)lhs - (C)rhs;
				break;
			default:
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return (C)0;
				break;
			}
		}

		template<typename C, typename TL, typename TR> bool raw_logical_binary_expression_evaluator(shared_ptr<mips_code> mc, TL lhs, TR rhs, binary_expression::operator_kind ok) {
			switch (ok) {
			case binary_expression::operator_kind::KIND_EQUALS_EQUALS:
				return (C)lhs == (C)rhs;
				break;
			case binary_expression::operator_kind::KIND_GREATER_THAN:
				return (C)lhs > (C)rhs;
				break;
			case binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO:
				return (C)lhs >= (C)rhs;
				break;
			case binary_expression::operator_kind::KIND_LESS_THAN:
				return (C)lhs < (C)rhs;
				break;
			case binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO:
				return (C)lhs <= (C)rhs;
				break;
			case binary_expression::operator_kind::KIND_LOGICAL_AND:
				return (C)lhs && (C)rhs;
				break;
			case binary_expression::operator_kind::KIND_LOGICAL_OR:
				return (C)lhs || (C)rhs;
				break;
			case binary_expression::operator_kind::KIND_NOT_EQUALS:
				return (C)lhs != (C)rhs;
				break;
			default:
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return false;
				break;
			}
		}

		variant<bool, int, unsigned int, float, double, string> evaluate_constant_expression(shared_ptr<mips_code> mc, shared_ptr<assignment_expression> ae) {
			function<variant<bool, int, unsigned int, float, double, string>(shared_ptr<expression>)> evaluate_expression;
			function<variant<bool, int, unsigned int, float, double, string>(shared_ptr<assignment_expression>)> evaluate_assignment_expression;
			function<variant<bool, int, unsigned int, float, double, string>(shared_ptr<ternary_expression>)> evaluate_ternary_expression;
			function<variant<bool, int, unsigned int, float, double, string>(shared_ptr<binary_expression>)> evaluate_binary_expression;
			auto evaluate_primary_expression = [&](shared_ptr<primary_expression> pexpr) {
				variant<bool, int, unsigned int, float, double, string> dummy, result;
				if (pexpr->primary_expression_kind() == primary_expression::kind::KIND_SIZEOF_TYPE) {
					result = (unsigned int)calculate_type_size(mc, pexpr->sizeof_type());
					return result;
				}
				else if (pexpr->primary_expression_kind() == primary_expression::kind::KIND_SIZEOF_EXPRESSION) {
					evaluate_expression(pexpr->parenthesized_expression());
					result = (unsigned int)calculate_type_size(mc, pexpr->parenthesized_expression()->expression_type());
					return result;
				}
				else if (pexpr->primary_expression_kind() == primary_expression::kind::KIND_PARENTHESIZED_EXPRESSION) {
					result = evaluate_expression(pexpr->parenthesized_expression());
					return result;
				}
				else if (pexpr->primary_expression_kind() == primary_expression::kind::KIND_RESV) {
					variant<bool, int, unsigned int, float, double, string> par = evaluate_expression(pexpr->parenthesized_expression());
					int sz = (int)calculate_type_size(mc, pexpr->mem_type());
					if (holds_alternative<bool>(par)) sz *= (int)get<bool>(par);
					else if (holds_alternative<int>(par)) sz *= get<int>(par);
					else if (holds_alternative<unsigned int>(par)) sz *= (int)get<unsigned int>(par);
					else if (holds_alternative<float>(par)) sz *= (int)get<float>(par);
					else if (holds_alternative<double>(par)) sz *= (int)get<double>(par);
					else {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return dummy;
					}
					sz = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
					string lab = "__resv_const_" + to_string(mc->next_misc_counter());
					mc->add_data_directive(make_shared<directive>(3));
					mc->add_data_directive(make_shared<directive>(lab, sz));
					result = lab;
					return result;
				}
				else if (pexpr->primary_expression_kind() == primary_expression::kind::KIND_ARRAY_INITIALIZER) {
					vector<shared_ptr<operand>> vec;
					vector<double> dlist;
					vector<float> flist;
					directive::kind dk;
					shared_ptr<type> arr_type = pexpr->primary_expression_type();
					string lab = "__temp_const_array_" + to_string(mc->next_misc_counter());
					if (arr_type->type_array_kind() == type::array_kind::KIND_ARRAY && arr_type->array_dimensions() > 1) dk = directive::kind::KIND_WORD;
					else {
						if (arr_type->type_kind() != type::kind::KIND_PRIMITIVE) {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return dummy;
						}
						shared_ptr<primitive_type> arr_ptype = static_pointer_cast<primitive_type>(arr_type);
						if (arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_BOOL || arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_BYTE ||
							arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_CHAR)
							dk = directive::kind::KIND_BYTE;
						else if (arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_SHORT)
							dk = directive::kind::KIND_HALF;
						else if (arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_LONG || arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_INT)
							dk = directive::kind::KIND_WORD;
						else if (arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
							dk = directive::kind::KIND_FLOAT;
						else if (arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
							dk = directive::kind::KIND_DOUBLE;
						else {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return dummy;
						}
					}
					for (shared_ptr<assignment_expression> ae : pexpr->array_initializer()) {
						variant<bool, int, unsigned int, float, double, string> el = evaluate_assignment_expression(ae);
						if (dk == directive::kind::KIND_DOUBLE) {
							if (holds_alternative<string>(el)) {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return dummy;
							}
							double to_put;
							if (holds_alternative<bool>(el)) to_put = (double)get<bool>(el);
							else if (holds_alternative<int>(el)) to_put = (double)get<int>(el);
							else if (holds_alternative<unsigned int>(el)) to_put = (double)get<unsigned int>(el);
							else if (holds_alternative<float>(el)) to_put = (double)get<float>(el);
							else if (holds_alternative<double>(el)) to_put = get<double>(el);
							else {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return dummy;
							}
							dlist.push_back(to_put);
						}
						else if (dk == directive::kind::KIND_FLOAT) {
							if (holds_alternative<string>(el)) {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return dummy;
							}
							float to_put;
							if (holds_alternative<bool>(el)) to_put = (float)get<bool>(el);
							else if (holds_alternative<int>(el)) to_put = (float)get<int>(el);
							else if (holds_alternative<unsigned int>(el)) to_put = (float)get<unsigned int>(el);
							else if (holds_alternative<float>(el)) to_put = get<float>(el);
							else if (holds_alternative<double>(el)) to_put = (float) get<double>(el);
							else {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return dummy;
							}
							flist.push_back(to_put);
						}
						else if (dk == directive::kind::KIND_WORD) {
							if (holds_alternative<string>(el)) vec.push_back(make_shared<operand>(false, get<string>(el)));
							else if (holds_alternative<bool>(el)) vec.push_back(make_shared<operand>((int) get<bool>(el)));
							else if (holds_alternative<int>(el)) vec.push_back(make_shared<operand>(get<int>(el)));
							else if (holds_alternative<unsigned int>(el)) vec.push_back(make_shared<operand>((int)get<unsigned int>(el)));
							else if (holds_alternative<float>(el)) vec.push_back(make_shared<operand>((int)get<float>(el)));
							else if (holds_alternative<double>(el)) vec.push_back(make_shared<operand>((int)get<double>(el)));
							else {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return dummy;
							}
						}
						else if (dk == directive::kind::KIND_HALF) {
							if (holds_alternative<string>(el)) vec.push_back(make_shared<operand>(false, get<string>(el)));
							else if (holds_alternative<bool>(el)) vec.push_back(make_shared<operand>((short) get<bool>(el)));
							else if (holds_alternative<int>(el)) vec.push_back(make_shared<operand>((short) get<int>(el)));
							else if (holds_alternative<unsigned int>(el)) vec.push_back(make_shared<operand>((short)get<unsigned int>(el)));
							else if (holds_alternative<float>(el)) vec.push_back(make_shared<operand>((short)get<float>(el)));
							else if (holds_alternative<double>(el)) vec.push_back(make_shared<operand>((short)get<double>(el)));
							else {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return dummy;
							}
						}
						else if (dk == directive::kind::KIND_BYTE) {
							if (holds_alternative<string>(el)) vec.push_back(make_shared<operand>(false, get<string>(el)));
							else if (holds_alternative<bool>(el)) vec.push_back(make_shared<operand>((char) get<bool>(el)));
							else if (holds_alternative<int>(el)) vec.push_back(make_shared<operand>((char) get<int>(el)));
							else if (holds_alternative<unsigned int>(el)) vec.push_back(make_shared<operand>((char)get<unsigned int>(el)));
							else if (holds_alternative<float>(el)) vec.push_back(make_shared<operand>((char)get<float>(el)));
							else if (holds_alternative<double>(el)) vec.push_back(make_shared<operand>((char)get<double>(el)));
							else {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return dummy;
							}
						}
					}
					if (dk == directive::kind::KIND_DOUBLE) {
						mc->add_data_directive(make_shared<directive>(3));
						mc->add_data_directive(make_shared<directive>(lab, dlist));
					}
					else if (dk == directive::kind::KIND_FLOAT) {
						mc->add_data_directive(make_shared<directive>(2));
						mc->add_data_directive(make_shared<directive>(lab, flist));
					}
					else {
						if (dk == directive::kind::KIND_WORD || dk == directive::kind::KIND_BYTE)
							mc->add_data_directive(make_shared<directive>(2));
						else if (dk == directive::kind::KIND_HALF)
							mc->add_data_directive(make_shared<directive>(1));
						else {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return dummy;
						}
						mc->add_data_directive(make_shared<directive>(dk, lab, vec));
					}
					result = lab;
					return result;
				}
				else if (pexpr->primary_expression_kind() == primary_expression::kind::KIND_LITERAL) {
					token lit = pexpr->literal_token();
					string raw_lit = lit.raw_text();
					switch (lit.token_kind()) {
					case token::kind::TOKEN_INTEGER: {
						raw_lit.erase(remove(raw_lit.begin(), raw_lit.end(), '\''), raw_lit.end());
						int base = 10;
						switch (lit.prefix_kind()) {
						case token::prefix::PREFIX_BINARY:
							base = 2;
							raw_lit = raw_lit.substr(2);
							break;
						case token::prefix::PREFIX_HEXADECIMAL:
							base = 16;
							raw_lit = raw_lit.substr(2);
							break;
						case token::prefix::PREFIX_OCTAL:
							base = 8;
							raw_lit = raw_lit.substr(2);
							break;
						}
						int val = stoul(raw_lit.c_str(), nullptr, base);
						if (lit.suffix_kind() == token::suffix::SUFFIX_DOUBLE)
							result = (double)val;
						else if (lit.suffix_kind() == token::suffix::SUFFIX_FLOAT)
							result = (float)val;
						else if (lit.suffix_kind() == token::suffix::SUFFIX_SIGNED_LONG || lit.suffix_kind() == token::suffix::SUFFIX_SIGNED_SHORT)
							result = (int)val;
						else if (lit.suffix_kind() == token::suffix::SUFFIX_UNSIGNED_INT || lit.suffix_kind() == token::suffix::SUFFIX_UNSIGNED_LONG ||
							lit.suffix_kind() == token::suffix::SUFFIX_UNSIGNED_SHORT)
							result = (unsigned int)val;
						else {
							result = (int)val;
						}
					}
						break;
					case token::kind::TOKEN_DECIMAL: {
						raw_lit.erase(remove(raw_lit.begin(), raw_lit.end(), '\''), raw_lit.end());
						if (lit.suffix_kind() == token::suffix::SUFFIX_DOUBLE)
							result = (double)atof(raw_lit.c_str());
						else if (lit.suffix_kind() == token::suffix::SUFFIX_FLOAT)
							result = (float)atof(raw_lit.c_str());
						else
							result = (double)atof(raw_lit.c_str());
					}
						break;
					case token::kind::TOKEN_CHARACTER: {
						raw_lit = raw_lit.substr(1, raw_lit.size() - 2);
						if (raw_lit[0] != '\\')
							result = (unsigned int)raw_lit[0];
						else {
							if (raw_lit.size() == 1) {
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return dummy;
							}
							switch (raw_lit[1]) {
							case 'a':
								result = (unsigned int) '\a';
								break;
							case 'b':
								result = (unsigned int) '\b';
								break;
							case 'f':
								result = (unsigned int) '\f';
								break;
							case 'n':
								result = (unsigned int) '\n';
								break;
							case 'r':
								result = (unsigned int) '\r';
								break;
							case 't':
								result = (unsigned int) '\t';
								break;
							case 'v':
								result = (unsigned int) '\v';
								break;
							case '\\':
								result = (unsigned int) '\\';
								break;
							case '\'':
								result = (unsigned int) '\'';
								break;
							case '\"':
								result = (unsigned int) '\"';
								break;
							case '\?':
								result = (unsigned int) '\?';
								break;
							default:
								result = (unsigned int) '\0';
								break;
							}
						}
					}
						break;
					case token::kind::TOKEN_TRUE:
					case token::kind::TOKEN_FALSE: {
						result = (bool)(lit.token_kind() == token::kind::TOKEN_TRUE);
					}
						break;
					case token::kind::TOKEN_STRING: {
						int counter = mc->next_misc_counter();
						string s_lit = "__sp_lit_" + to_string(counter);
						mc->add_data_directive(make_shared<directive>(2));
						string concated_raw_lit;
						for (token t : pexpr->stream())
							concated_raw_lit += t.raw_text().substr(1, t.raw_text().length() - 2);
						mc->add_data_directive(make_shared<directive>(s_lit, "\"" + concated_raw_lit + "\""));
						result = s_lit;
					}
						break;
					}
					return result;
				}
				else if (pexpr->primary_expression_kind() == primary_expression::kind::KIND_IDENTIFIER) {
					shared_ptr<symbol> sym = pexpr->identifier_symbol();
					string sym_string = c_symbol_2_string(mc, sym);
					pair<bool, variant<bool, int, unsigned int, float, double, string>> check = mc->get_constexpr_mapping(sym_string);
					if (!check.first) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return dummy;
					}
					result = check.second;
					return result;
				}
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return dummy;
			};
			auto evaluate_postfix_expression = [&](shared_ptr<postfix_expression> pexpr) {
				variant<bool, int, unsigned int, float, double, string> dummy, result = evaluate_primary_expression(pexpr->contained_primary_expression());
				shared_ptr<type> prev_type = pexpr->contained_primary_expression()->primary_expression_type();
				for (shared_ptr<postfix_expression::postfix_type> pt : pexpr->postfix_type_list()) {
					if (pt->postfix_type_kind() != postfix_expression::kind::KIND_AS) {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return dummy;
					}
					shared_ptr<type> to_type = pt->postfix_type_type();
					if (to_type->type_array_kind() != type::array_kind::KIND_ARRAY) {
						if (prev_type->type_array_kind() == type::array_kind::KIND_ARRAY || holds_alternative<string>(result) || to_type->type_kind() != type::kind::KIND_PRIMITIVE) {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return dummy;
						}
						shared_ptr<primitive_type> to_prim = static_pointer_cast<primitive_type>(to_type);

#define SPECTRE_CAST_BODY(T) \
	if(to_prim->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) \
		result = (double) get<T>(result); \
	else if(to_prim->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) \
		result = (float) get<T>(result); \
	else if(to_prim->primitive_type_kind() == primitive_type::kind::KIND_BOOL) \
		result = (bool) get<T>(result); \
	else if(to_prim->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED) \
		result = (unsigned int) get<T>(result); \
	else \
		result = (int) get<T>(result)

						if (holds_alternative<bool>(result)) { SPECTRE_CAST_BODY(bool); }
						else if (holds_alternative<int>(result)) { SPECTRE_CAST_BODY(int); }
						else if (holds_alternative<unsigned int>(result)) { SPECTRE_CAST_BODY(unsigned int); }
						else if (holds_alternative<float>(result)) { SPECTRE_CAST_BODY(float); }
						else if (holds_alternative<double>(result)) { SPECTRE_CAST_BODY(double); }
						else {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return dummy;
						}

#undef SPECTRE_CAST_BODY
					}
					else {
						if (holds_alternative<bool>(result))
							result = (unsigned int)get<bool>(result);
						else if (holds_alternative<int>(result))
							result = (unsigned int)get<int>(result);
						else if (holds_alternative<unsigned int>(result));
						else if (holds_alternative<float>(result))
							result = (unsigned int)get<float>(result);
						else if (holds_alternative<double>(result))
							result = (unsigned int)get<double>(result);
						else;
					}

					prev_type = to_type;
				}
				return result;
			};
			auto evaluate_unary_expression = [&](shared_ptr<unary_expression> uexpr) {
				variant<bool, int, unsigned int, float, double, string> dummy, result = evaluate_postfix_expression(uexpr->contained_postfix_expression());
				for (unary_expression::kind uek : uexpr->unary_expression_kind_list()) {
					switch (uek) {
					case unary_expression::kind::KIND_INCREMENT:
					case unary_expression::kind::KIND_DECREMENT:
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return dummy;
						break;
					case unary_expression::kind::KIND_BITWISE_NOT: {
						if (holds_alternative<bool>(result))
							result = (unsigned int)(~(unsigned int)get<bool>(result));
						else if (holds_alternative<int>(result))
							result = (int)~get<int>(result);
						else if (holds_alternative<unsigned int>(result))
							result = (unsigned int)~get<unsigned int>(result);
						else {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return dummy;
						}
					}
						break;
					case unary_expression::kind::KIND_LOGICAL_NOT: {
						if (holds_alternative<bool>(result))
							result = (bool)!get<bool>(result);
						else if (holds_alternative<int>(result))
							result = (bool)!get<int>(result);
						else if (holds_alternative<unsigned int>(result))
							result = (bool)!get<unsigned int>(result);
						else if (holds_alternative<float>(result))
							result = (bool)!get<float>(result);
						else if (holds_alternative<double>(result))
							result = (bool)!get<double>(result);
						else {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return dummy;
						}
					}
						break;
					case unary_expression::kind::KIND_PLUS:
					case unary_expression::kind::KIND_MINUS: {
						bool plus = uek == unary_expression::kind::KIND_PLUS;
						if (holds_alternative<bool>(result)) {
							int temp = plus ? (unsigned int)+(unsigned int)get<bool>(result) : -(int)get<bool>(result);
							result = (unsigned int)temp;
						}
						else if (holds_alternative<int>(result))
							result = plus ? (int)+get<int>(result) : (int)-get<int>(result);
						else if (holds_alternative<unsigned int>(result)) {
							int temp = plus ? (unsigned int)+get<unsigned int>(result) : (int)- (int) get<unsigned int>(result);
							result = (unsigned int)temp;
						}
						else if (holds_alternative<float>(result))
							result = plus ? (float)+get<float>(result) : (float)-get<float>(result);
						else if (holds_alternative<double>(result))
							result = plus ? (double)+get<double>(result) : (double)-get<double>(result);
						else {
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return dummy;
						}
					}
						break;
					default: {
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return dummy;
					}
						break;
					}
				}
				return result;
			};
			evaluate_binary_expression = [&](shared_ptr<binary_expression> bexpr) {
				if (bexpr->binary_expression_kind() == binary_expression::kind::KIND_UNARY_EXPRESSION)
					return evaluate_unary_expression(bexpr->single_lhs());
				variant<bool, int, unsigned int, float, double, string> dummy, result;
				variant<bool, int, unsigned int, float, double, string> lhs_result = evaluate_binary_expression(bexpr->lhs()),
					rhs_result = evaluate_binary_expression(bexpr->rhs());
				if (holds_alternative<string>(lhs_result) || holds_alternative<string>(rhs_result)) return dummy;

				// sigh...
				using result_variant_type = variant<bool, int, unsigned int, float, double>;
				result_variant_type temp_lhs_result, temp_rhs_result;
				struct collapsing_variant_assignment_visitor {
				private:
					shared_ptr<mips_code> _mc;
					result_variant_type& _res;
				public:
					collapsing_variant_assignment_visitor(shared_ptr<mips_code> mc, result_variant_type& r) : _mc(mc), _res(r) {}

					void operator()(bool b) { _res = b; }
					void operator()(int i) { _res = i; }
					void operator()(unsigned int u) { _res = u; }
					void operator()(float f) { _res = f; }
					void operator()(double d) { _res = d; }
					void operator()(string s) { _mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__); }
				};
				visit(collapsing_variant_assignment_visitor{ mc, temp_lhs_result }, lhs_result);
				visit(collapsing_variant_assignment_visitor{ mc, temp_rhs_result }, rhs_result);

				// SIGH...
#define SPECTRE_OPERATOR_BODY(T) \
	result_variant_type result; \
	if(_logical) \
		result = raw_logical_binary_expression_evaluator<T>(_mc, lhs, rhs, _operator_kind); \
	else \
		result = raw_arithmetic_binary_expression_evaluator<T>(_mc, lhs, rhs, _operator_kind); \
	return result

				struct custom_variant_visitor {
				private:
					binary_expression::operator_kind _operator_kind;
					shared_ptr<mips_code> _mc;
					bool _logical;
				public:
					custom_variant_visitor(shared_ptr<mips_code> mc, binary_expression::operator_kind ok) : _mc(mc), _operator_kind(ok),
						_logical(ok == binary_expression::operator_kind::KIND_EQUALS_EQUALS || ok == binary_expression::operator_kind::KIND_GREATER_THAN ||
							ok == binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO || ok == binary_expression::operator_kind::KIND_LESS_THAN ||
							ok == binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO || ok == binary_expression::operator_kind::KIND_LOGICAL_AND ||
							ok == binary_expression::operator_kind::KIND_LOGICAL_OR || ok == binary_expression::operator_kind::KIND_NOT_EQUALS) {}
					result_variant_type operator()(bool lhs, bool rhs) { SPECTRE_OPERATOR_BODY(unsigned int); }
					result_variant_type operator()(bool lhs, int rhs) { SPECTRE_OPERATOR_BODY(int); }
					result_variant_type operator()(bool lhs, unsigned int rhs) { SPECTRE_OPERATOR_BODY(unsigned int); }
					result_variant_type operator()(bool lhs, float rhs) { SPECTRE_OPERATOR_BODY(float); }
					result_variant_type operator()(bool lhs, double rhs) { SPECTRE_OPERATOR_BODY(double); }

					result_variant_type operator()(int lhs, bool rhs) { SPECTRE_OPERATOR_BODY(int); }
					result_variant_type operator()(int lhs, int rhs) { SPECTRE_OPERATOR_BODY(int); }
					result_variant_type operator()(int lhs, unsigned int rhs) { SPECTRE_OPERATOR_BODY(unsigned int); }
					result_variant_type operator()(int lhs, float rhs) { SPECTRE_OPERATOR_BODY(float); }
					result_variant_type operator()(int lhs, double rhs) { SPECTRE_OPERATOR_BODY(double); }

					result_variant_type operator()(unsigned int lhs, bool rhs) { SPECTRE_OPERATOR_BODY(unsigned int); }
					result_variant_type operator()(unsigned int lhs, int rhs) { SPECTRE_OPERATOR_BODY(unsigned int); }
					result_variant_type operator()(unsigned int lhs, unsigned int rhs) { SPECTRE_OPERATOR_BODY(unsigned int); }
					result_variant_type operator()(unsigned int lhs, float rhs) { SPECTRE_OPERATOR_BODY(float); }
					result_variant_type operator()(unsigned int lhs, double rhs) { SPECTRE_OPERATOR_BODY(double); }

					result_variant_type operator()(float lhs, bool rhs) { SPECTRE_OPERATOR_BODY(float); }
					result_variant_type operator()(float lhs, int rhs) { SPECTRE_OPERATOR_BODY(float); }
					result_variant_type operator()(float lhs, unsigned int rhs) { SPECTRE_OPERATOR_BODY(float); }
					result_variant_type operator()(float lhs, float rhs) { SPECTRE_OPERATOR_BODY(float); }
					result_variant_type operator()(float lhs, double rhs) { SPECTRE_OPERATOR_BODY(double); }

					result_variant_type operator()(double lhs, bool rhs) { SPECTRE_OPERATOR_BODY(double); }
					result_variant_type operator()(double lhs, int rhs) { SPECTRE_OPERATOR_BODY(double); }
					result_variant_type operator()(double lhs, unsigned int rhs) { SPECTRE_OPERATOR_BODY(double); }
					result_variant_type operator()(double lhs, float rhs) { SPECTRE_OPERATOR_BODY(double); }
					result_variant_type operator()(double lhs, double rhs) { SPECTRE_OPERATOR_BODY(double); }
				};
#undef SPECTRE_OPERATOR_BODY

				result_variant_type temp_result = visit(custom_variant_visitor{ mc, bexpr->binary_expression_operator_kind() }, temp_lhs_result, temp_rhs_result);
				visit([&result](auto t) { result = t; }, temp_result);
				return result;
			};
			evaluate_ternary_expression = [&](shared_ptr<ternary_expression> texpr) {
				variant<bool, int, unsigned int, float, double, string> dummy;
				variant<bool, int, unsigned int, float, double, string> cond_result = evaluate_binary_expression(texpr->condition());
				if (texpr->ternary_expression_kind() == ternary_expression::kind::KIND_BINARY)
					return cond_result;
				if (!holds_alternative<bool>(cond_result)) {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return dummy;
				}
				if (get<bool>(cond_result))
					return evaluate_expression(texpr->true_path());
				else
					return evaluate_ternary_expression(texpr->false_path());
				return dummy;
			};
			evaluate_assignment_expression = [&](shared_ptr<assignment_expression> aexpr) {
				variant<bool, int, unsigned int, float, double, string> result;
				if (aexpr->assignment_expression_kind() == assignment_expression::kind::KIND_TERNARY)
					return evaluate_ternary_expression(aexpr->conditional_expression());
				else {
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return result;
				}
			};
			evaluate_expression = [&](shared_ptr<expression> expr) {
				variant<bool, int, unsigned int, float, double, string> result;
				for (int i = 0; i < expr->assignment_expression_list().size(); i++) {
					result = evaluate_assignment_expression(expr->assignment_expression_list()[i]);
				}
				return result;
			};
			return evaluate_assignment_expression(ae);
		}

		tuple<vector<int>, vector<int>, int> save_to_middle(shared_ptr<mips_code> mc) {
			shared_ptr<operand> fp = register_file2::_fp_register;
			vector<int> which_to_store;
			vector<int> middle_offsets;
			int pre_offset = 0;
#if SYSTEM == 0 || SYSTEM == 1
			int _t0 = register_file2::_t0_register->register_number(), _t1 = register_file2::_t1_register->register_number(),
				_t2 = register_file2::_t2_register->register_number(), _t3 = register_file2::_t3_register->register_number(),
				_t4 = register_file2::_t4_register->register_number(), _t5 = register_file2::_t5_register->register_number(),
				_t6 = register_file2::_t6_register->register_number(), _t7 = register_file2::_t7_register->register_number(),
				_f4 = register_file2::_f4_register->register_number(), _f6 = register_file2::_f6_register->register_number(),
				_f8 = register_file2::_f8_register->register_number(), _f10 = register_file2::_f10_register->register_number(),
				_f12 = register_file2::_f12_register->register_number(), _f14 = register_file2::_f14_register->register_number(),
				_v0 = register_file2::_v0_register->register_number(), _a0 = register_file2::_a0_register->register_number(),
				_a1 = register_file2::_a1_register->register_number(), _a2 = register_file2::_a2_register->register_number(),
				_a3 = register_file2::_a3_register->register_number();
			for (int r : { _t0, _t1, _t2, _t3, _t4, _t5, _t6, _t7, _v0, _a0, _a1, _a2, _a3, _f4, _f6, _f8, _f10, _f12, _f14 })
#elif SYSTEM == 2
			int _t0 = register_file2::_t0_register->register_number(), _t1 = register_file2::_t1_register->register_number(),
				_t2 = register_file2::_t2_register->register_number(), _t3 = register_file2::_t3_register->register_number(),
				_t5 = register_file2::_t5_register->register_number(), _t6 = register_file2::_t6_register->register_number(),
				_t4 = register_file2::_t4_register->register_number(), _a5 = register_file2::_a5_register->register_number(),
				_a6 = register_file2::_a6_register->register_number(), _a7 = register_file2::_a7_register->register_number(),
				_a0 = register_file2::_a0_register->register_number();
			int _a1 = register_file2::_a1_register->register_number(), _a2 = register_file2::_a2_register->register_number(),
				_a3 = register_file2::_a3_register->register_number(), _a4 = register_file2::_a4_register->register_number();
			int f0 = register_file1::register_2_int.at(register_file1::_f0), f1 = register_file1::register_2_int.at(register_file1::_f1),
				f2 = register_file1::register_2_int.at(register_file1::_f2), f3 = register_file1::register_2_int.at(register_file1::_f3),
				f4 = register_file1::register_2_int.at(register_file1::_f4), f5 = register_file1::register_2_int.at(register_file1::_f5),
				f6 = register_file1::register_2_int.at(register_file1::_f6), f7 = register_file1::register_2_int.at(register_file1::_f7),
				f10 = register_file1::register_2_int.at(register_file1::_f10), f11 = register_file1::register_2_int.at(register_file1::_f11),
				f12 = register_file1::register_2_int.at(register_file1::_f12), f13 = register_file1::register_2_int.at(register_file1::_f13),
				f14 = register_file1::register_2_int.at(register_file1::_f14), f15 = register_file1::register_2_int.at(register_file1::_f15),
				f16 = register_file1::register_2_int.at(register_file1::_f16), f17 = register_file1::register_2_int.at(register_file1::_f17),
				f28 = register_file1::register_2_int.at(register_file1::_f28), f29 = register_file1::register_2_int.at(register_file1::_f29),
				f30 = register_file1::register_2_int.at(register_file1::_f30), f31 = register_file1::register_2_int.at(register_file1::_f31);
			for (int r : { _t0, _t1, _t2, _t3, _t4, _a5, _a6, _a7, _t5, _a0, _a1, _a2, _a3, _a4, _t5, _t6,
							f0, f1, f2, f3, f4, f5, f6, f7, f10, f11, f12, f13, f14, f15, f16, f17, f28, f29, f30, f31 })
#endif
				if (mc->current_frame()->is_register_in_use(r)) {
					if (mc->current_frame()->is_register_in_use(r) && (r > 31))
						which_to_store.push_back(r);
					else if (mc->current_frame()->is_register_in_use(r))
						which_to_store.push_back(r);
					pre_offset += 8;
				}
			for (int i = 0, curr = 0; i < which_to_store.size(); i++) {
				int r = which_to_store[i];
				middle_offsets.push_back(-(mc->current_frame()->middle_section_size() + 8));
				mc->current_frame()->update_middle_section_size(8);
				if (r > 31) {
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SDC1, register_file2::int_2_register_object.at(which_to_store[i]),
						make_shared<operand>(operand::offset_kind::KIND_MIDDLE, middle_offsets[i], fp->register_number(), fp->register_name())));
				}
				else
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, register_file2::int_2_register_object.at(which_to_store[i]),
						make_shared<operand>(operand::offset_kind::KIND_MIDDLE, middle_offsets[i], fp->register_number(), fp->register_name())));
				curr += 8;
			}
			return make_tuple(which_to_store, middle_offsets, pre_offset);
		}

		void restore_from_middle(shared_ptr<mips_code> mc, vector<int> which_to_store, vector<int> middle_offsets) {
			shared_ptr<operand> fp = register_file2::_fp_register;
			for (int i = 0, curr = 0; i < which_to_store.size(); i++) {
				if (which_to_store[i] > 31)
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LDC1, register_file2::int_2_register_object.at(which_to_store[i]),
						make_shared<operand>(operand::offset_kind::KIND_MIDDLE, middle_offsets[i], fp->register_number(), fp->register_name())));
				else
					mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, register_file2::int_2_register_object.at(which_to_store[i]),
						make_shared<operand>(operand::offset_kind::KIND_MIDDLE, middle_offsets[i], fp->register_number(), fp->register_name())));
				curr += 8;
			}
		}

		void generate_access_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<access_stmt> as) {
			if (as == nullptr || !as->valid()) return;
			for (shared_ptr<symbol> s : as->declared_symbol_list()) {
				string sym_name = c_symbol_2_string(mc, s);
				mc->current_frame()->add_variable(sym_name, make_shared<operand>(false, sym_name));
			}
		}

		void generate_delete_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<delete_stmt> ds) {
			if (ds == nullptr || !ds->valid()) return;
			if (SYSTEM != 0)
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<operand> e = generate_expression_mips(mc, ds->expr(), false);
			e = load_value_into_register(mc, e, ds->expr()->expression_type());
			shared_ptr<operand> _4 = register_file2::_a0_register, _2 = register_file2::_v0_register, _30 = register_file2::_fp_register;
			vector<int> _4_2_store;
			bool _4_in_use = mc->current_frame()->is_register_in_use(4), _2_in_use = mc->current_frame()->is_register_in_use(2);
			if (_4_in_use) {
				int o = -(mc->current_frame()->middle_section_size() + 8);
				_4_2_store.push_back(o);
				mc->current_frame()->update_middle_section_size(8);
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, _4, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
					o, _30->register_number(), _30->register_name())));
			}
			else
				_4_2_store.push_back(-1);
			if (_2_in_use) {
				int o = -(mc->current_frame()->middle_section_size() + 8);
				_4_2_store.push_back(o);
				mc->current_frame()->update_middle_section_size(8);
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_SW, _2, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
					o, _30->register_number(), _30->register_name())));
			}
			else
				_4_2_store.push_back(-1);
			tuple<vector<int>, vector<int>, int> res_s_tup = save_to_middle(mc);
			vector<int> which_to_store = get<0>(res_s_tup), middle_offsets = get<1>(res_s_tup);
			mc->current_frame()->add_insn_to_body(make_shared<insn>(
				e->operand_kind() == operand::kind::KIND_INT_IMMEDIATE ? insn::kind::KIND_ADDIU : insn::kind::KIND_ADDU,
				_4, register_file2::_zero_register, e));
			mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_JAL, make_shared<operand>(false, program_delete_hook)));
			restore_from_middle(mc, which_to_store, middle_offsets);
			if (_4_in_use)
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, _4, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
					_4_2_store[0], _30->register_number(), _30->register_name())));
			if (_2_in_use)
				mc->current_frame()->add_insn_to_body(make_shared<insn>(insn::kind::KIND_LW, _2, make_shared<operand>(operand::offset_kind::KIND_MIDDLE,
					_4_2_store[1], _2->register_number(), _2->register_name())));
			if (e->register_number() > 31)
				free_general_purpose_fp_register(mc, e);
			else
				free_general_purpose_register(mc, e);
		}
	}
}
