// BUG!!!: __rfuncarg__ function arguments as lvalues in ternary assignment expressions do not work.
// BUG!!!: register spilling in either branch of a ternary expression will fail.
// -- both of these mean -> avoid complex ternary expressions for now.

#include "mips.hpp"
#include <algorithm>
#include <memory>
#include <limits>
#include <iostream>
#include <functional>
#include <iomanip>
#include <sstream>
#include <cmath>

using std::to_string;
using std::static_pointer_cast;
using std::make_shared;
using std::stoi;
using std::numeric_limits;
using std::make_tuple;
using std::get;
using std::function;
using std::find;
using std::stod;
using std::ostringstream;
using std::setprecision;
using std::log2;

namespace spectre {
	namespace mips {

		namespace register_file {
			namespace register_objects {

				const static shared_ptr<register_operand> _zero_register = make_shared<register_operand>(register_2_int.at(_0));
				const static shared_ptr<register_operand> _at_register = make_shared<register_operand>(register_2_int.at(_at));
				const static shared_ptr<register_operand> _v0_register = make_shared<register_operand>(register_2_int.at(_v0));
				const static shared_ptr<register_operand> _v1_register = make_shared<register_operand>(register_2_int.at(_v1));
				const static shared_ptr<register_operand> _a0_register = make_shared<register_operand>(register_2_int.at(_a0));
				const static shared_ptr<register_operand> _a1_register = make_shared<register_operand>(register_2_int.at(_a1));
				const static shared_ptr<register_operand> _a2_register = make_shared<register_operand>(register_2_int.at(_a2));
				const static shared_ptr<register_operand> _a3_register = make_shared<register_operand>(register_2_int.at(_a3));
				const static shared_ptr<register_operand> _t0_register = make_shared<register_operand>(register_2_int.at(_t0));
				const static shared_ptr<register_operand> _t1_register = make_shared<register_operand>(register_2_int.at(_t1));
				const static shared_ptr<register_operand> _t2_register = make_shared<register_operand>(register_2_int.at(_t2));
				const static shared_ptr<register_operand> _t3_register = make_shared<register_operand>(register_2_int.at(_t3));
				const static shared_ptr<register_operand> _t4_register = make_shared<register_operand>(register_2_int.at(_t4));
				const static shared_ptr<register_operand> _t5_register = make_shared<register_operand>(register_2_int.at(_t5));
				const static shared_ptr<register_operand> _t6_register = make_shared<register_operand>(register_2_int.at(_t6));
				const static shared_ptr<register_operand> _t7_register = make_shared<register_operand>(register_2_int.at(_t7));
				const static shared_ptr<register_operand> _s0_register = make_shared<register_operand>(register_2_int.at(_s0));
				const static shared_ptr<register_operand> _s1_register = make_shared<register_operand>(register_2_int.at(_s1));
				const static shared_ptr<register_operand> _s2_register = make_shared<register_operand>(register_2_int.at(_s2));
				const static shared_ptr<register_operand> _s3_register = make_shared<register_operand>(register_2_int.at(_s3));
				const static shared_ptr<register_operand> _s4_register = make_shared<register_operand>(register_2_int.at(_s4));
				const static shared_ptr<register_operand> _s5_register = make_shared<register_operand>(register_2_int.at(_s5));
				const static shared_ptr<register_operand> _s6_register = make_shared<register_operand>(register_2_int.at(_s6));
				const static shared_ptr<register_operand> _s7_register = make_shared<register_operand>(register_2_int.at(_s7));
				const static shared_ptr<register_operand> _t8_register = make_shared<register_operand>(register_2_int.at(_t8));
				const static shared_ptr<register_operand> _t9_register = make_shared<register_operand>(register_2_int.at(_t9));
				const static shared_ptr<register_operand> _k0_register = make_shared<register_operand>(register_2_int.at(_k0));
				const static shared_ptr<register_operand> _k1_register = make_shared<register_operand>(register_2_int.at(_k1));
				const static shared_ptr<register_operand> _gp_register = make_shared<register_operand>(register_2_int.at(_gp));
				const static shared_ptr<register_operand> _sp_register = make_shared<register_operand>(register_2_int.at(_sp));
				const static shared_ptr<register_operand> _fp_register = make_shared<register_operand>(register_2_int.at(_fp));
				const static shared_ptr<register_operand> _ra_register = make_shared<register_operand>(register_2_int.at(_ra));

				const static shared_ptr<register_operand> _f0_register = make_shared<register_operand>(register_2_int.at(_f0));
				const static shared_ptr<register_operand> _f1_register = make_shared<register_operand>(register_2_int.at(_f1));
				const static shared_ptr<register_operand> _f2_register = make_shared<register_operand>(register_2_int.at(_f2));
				const static shared_ptr<register_operand> _f3_register = make_shared<register_operand>(register_2_int.at(_f3));
				const static shared_ptr<register_operand> _f4_register = make_shared<register_operand>(register_2_int.at(_f4));
				const static shared_ptr<register_operand> _f5_register = make_shared<register_operand>(register_2_int.at(_f5));
				const static shared_ptr<register_operand> _f6_register = make_shared<register_operand>(register_2_int.at(_f6));
				const static shared_ptr<register_operand> _f7_register = make_shared<register_operand>(register_2_int.at(_f7));
				const static shared_ptr<register_operand> _f8_register = make_shared<register_operand>(register_2_int.at(_f8));
				const static shared_ptr<register_operand> _f9_register = make_shared<register_operand>(register_2_int.at(_f9));
				const static shared_ptr<register_operand> _f10_register = make_shared<register_operand>(register_2_int.at(_f10));
				const static shared_ptr<register_operand> _f11_register = make_shared<register_operand>(register_2_int.at(_f11));
				const static shared_ptr<register_operand> _f12_register = make_shared<register_operand>(register_2_int.at(_f12));
				const static shared_ptr<register_operand> _f13_register = make_shared<register_operand>(register_2_int.at(_f13));
				const static shared_ptr<register_operand> _f14_register = make_shared<register_operand>(register_2_int.at(_f14));
				const static shared_ptr<register_operand> _f15_register = make_shared<register_operand>(register_2_int.at(_f15));
				const static shared_ptr<register_operand> _f16_register = make_shared<register_operand>(register_2_int.at(_f16));
				const static shared_ptr<register_operand> _f17_register = make_shared<register_operand>(register_2_int.at(_f17));
				const static shared_ptr<register_operand> _f18_register = make_shared<register_operand>(register_2_int.at(_f18));
				const static shared_ptr<register_operand> _f19_register = make_shared<register_operand>(register_2_int.at(_f19));
				const static shared_ptr<register_operand> _f20_register = make_shared<register_operand>(register_2_int.at(_f20));
				const static shared_ptr<register_operand> _f21_register = make_shared<register_operand>(register_2_int.at(_f21));
				const static shared_ptr<register_operand> _f22_register = make_shared<register_operand>(register_2_int.at(_f22));
				const static shared_ptr<register_operand> _f23_register = make_shared<register_operand>(register_2_int.at(_f23));
				const static shared_ptr<register_operand> _f24_register = make_shared<register_operand>(register_2_int.at(_f24));
				const static shared_ptr<register_operand> _f25_register = make_shared<register_operand>(register_2_int.at(_f25));
				const static shared_ptr<register_operand> _f26_register = make_shared<register_operand>(register_2_int.at(_f26));
				const static shared_ptr<register_operand> _f27_register = make_shared<register_operand>(register_2_int.at(_f27));
				const static shared_ptr<register_operand> _f28_register = make_shared<register_operand>(register_2_int.at(_f28));
				const static shared_ptr<register_operand> _f29_register = make_shared<register_operand>(register_2_int.at(_f29));
				const static shared_ptr<register_operand> _f30_register = make_shared<register_operand>(register_2_int.at(_f30));
				const static shared_ptr<register_operand> _f31_register = make_shared<register_operand>(register_2_int.at(_f31));


				map<int, shared_ptr<register_operand>> int_2_register_object = {
					{ register_2_int.at(_0), _zero_register },
					{ register_2_int.at(_1), _at_register },
					{ register_2_int.at(_2), _v0_register },
					{ register_2_int.at(_3), _v1_register },
					{ register_2_int.at(_4), _a0_register },
					{ register_2_int.at(_5), _a1_register },
					{ register_2_int.at(_6), _a2_register },
					{ register_2_int.at(_7), _a3_register },
					{ register_2_int.at(_8), _t0_register },
					{ register_2_int.at(_9), _t1_register },
					{ register_2_int.at(_10), _t2_register },
					{ register_2_int.at(_11), _t3_register },
					{ register_2_int.at(_12), _t4_register },
					{ register_2_int.at(_13), _t5_register },
					{ register_2_int.at(_14), _t6_register },
					{ register_2_int.at(_15), _t7_register },
					{ register_2_int.at(_16), _s0_register },
					{ register_2_int.at(_17), _s1_register },
					{ register_2_int.at(_18), _s2_register },
					{ register_2_int.at(_19), _s3_register },
					{ register_2_int.at(_20), _s4_register },
					{ register_2_int.at(_21), _s5_register },
					{ register_2_int.at(_22), _s6_register },
					{ register_2_int.at(_23), _s7_register },
					{ register_2_int.at(_24), _t8_register },
					{ register_2_int.at(_25), _t9_register },
					{ register_2_int.at(_26), _k0_register },
					{ register_2_int.at(_27), _k1_register },
					{ register_2_int.at(_28), _gp_register },
					{ register_2_int.at(_29), _sp_register },
					{ register_2_int.at(_30), _fp_register },
					{ register_2_int.at(_31), _ra_register },
					{ register_2_int.at(_f0), _f0_register },
					{ register_2_int.at(_f1), _f1_register },
					{ register_2_int.at(_f2), _f2_register },
					{ register_2_int.at(_f3), _f3_register },
					{ register_2_int.at(_f4), _f4_register },
					{ register_2_int.at(_f5), _f5_register },
					{ register_2_int.at(_f6), _f6_register },
					{ register_2_int.at(_f7), _f7_register },
					{ register_2_int.at(_f8), _f8_register },
					{ register_2_int.at(_f9), _f9_register },
					{ register_2_int.at(_f10), _f10_register },
					{ register_2_int.at(_f11), _f11_register },
					{ register_2_int.at(_f12), _f12_register },
					{ register_2_int.at(_f13), _f13_register },
					{ register_2_int.at(_f14), _f14_register },
					{ register_2_int.at(_f15), _f15_register },
					{ register_2_int.at(_f16), _f16_register },
					{ register_2_int.at(_f17), _f17_register },
					{ register_2_int.at(_f18), _f18_register },
					{ register_2_int.at(_f19), _f19_register },
					{ register_2_int.at(_f20), _f20_register },
					{ register_2_int.at(_f21), _f21_register },
					{ register_2_int.at(_f22), _f22_register },
					{ register_2_int.at(_f23), _f23_register },
					{ register_2_int.at(_f24), _f24_register },
					{ register_2_int.at(_f25), _f25_register },
					{ register_2_int.at(_f26), _f26_register },
					{ register_2_int.at(_f27), _f27_register },
					{ register_2_int.at(_f28), _f28_register },
					{ register_2_int.at(_f29), _f29_register },
					{ register_2_int.at(_f30), _f30_register },
					{ register_2_int.at(_f31), _f31_register }
				};

				map<string, shared_ptr<register_operand>> register_2_register_object = {
					{ _0, _zero_register },
					{ _1, _at_register },
					{ _2, _v0_register },
					{ _3, _v1_register },
					{ _4, _a0_register },
					{ _5, _a1_register },
					{ _6, _a2_register },
					{ _7, _a3_register },
					{ _8, _t0_register },
					{ _9, _t1_register },
					{ _10, _t2_register },
					{ _11, _t3_register },
					{ _12, _t4_register },
					{ _13, _t5_register },
					{ _14, _t6_register },
					{ _15, _t7_register },
					{ _16, _s0_register },
					{ _17, _s1_register },
					{ _18, _s2_register },
					{ _19, _s3_register },
					{ _20, _s4_register },
					{ _21, _s5_register },
					{ _22, _s6_register },
					{ _23, _s7_register },
					{ _24, _t8_register },
					{ _25, _t9_register },
					{ _26, _k0_register },
					{ _27, _k1_register },
					{ _28, _gp_register },
					{ _29, _sp_register },
					{ _30, _fp_register },
					{ _31, _ra_register },
					{ _f0, _f0_register },
					{ _f1, _f1_register },
					{ _f2, _f2_register },
					{ _f3, _f3_register },
					{ _f4, _f4_register },
					{ _f5, _f5_register },
					{ _f6, _f6_register },
					{ _f7, _f7_register },
					{ _f8, _f8_register },
					{ _f9, _f9_register },
					{ _f10, _f10_register },
					{ _f11, _f11_register },
					{ _f12, _f12_register },
					{ _f13, _f13_register },
					{ _f14, _f14_register },
					{ _f15, _f15_register },
					{ _f16, _f16_register },
					{ _f17, _f17_register },
					{ _f18, _f18_register },
					{ _f19, _f19_register },
					{ _f20, _f20_register },
					{ _f21, _f21_register },
					{ _f22, _f22_register },
					{ _f23, _f23_register },
					{ _f24, _f24_register },
					{ _f25, _f25_register },
					{ _f26, _f26_register },
					{ _f27, _f27_register },
					{ _f28, _f28_register },
					{ _f29, _f29_register },
					{ _f30, _f30_register },
					{ _f31, _f31_register }
				};
			}
		}

		const static map<insn::kind, string> insn_kind_2_string = {
			{ insn::kind::KIND_ADD, insns::_add }, { insn::kind::KIND_ADDI, insns::_addi }, { insn::kind::KIND_ADDIU, insns::_addiu }, { insn::kind::KIND_ADDU, insns::_addu },
			{ insn::kind::KIND_AND, insns::_and }, { insn::kind::KIND_ANDI, insns::_andi },
			{ insn::kind::KIND_BEQ, insns::_beq }, { insn::kind::KIND_BGEZ, insns::_bgez }, { insn::kind::KIND_BGEZAL, insns::_bgezal }, { insn::kind::KIND_BGTZ, insns::_bgtz },
				{ insn::kind::KIND_BLEZ, insns::_blez }, { insn::kind::KIND_BLTZ, insns::_bltz }, { insn::kind::KIND_BLTZAL, insns::_bltzal }, { insn::kind::KIND_BNE, insns::_bne },
			{ insn::kind::KIND_DIV, insns::_div }, { insn::kind::KIND_DIVU, insns::_divu },
			{ insn::kind::KIND_JAL, insns::_jal }, { insn::kind::KIND_J, insns::_j }, { insn::kind::KIND_JR, insns::_jr },
			{ insn::kind::KIND_LA, insns::_la },
			{ insn::kind::KIND_LB, insns::_lb }, { insn::kind::KIND_LH, insns::_lh }, { insn::kind::KIND_LW, insns::_lw },
			{ insn::kind::KIND_LHU, insns::_lhu }, { insn::kind::KIND_LBU, insns::_lbu },
			{ insn::kind::KIND_LUI, insns::_lui },
			{ insn::kind::KIND_MFLO, insns::_mflo }, { insn::kind::KIND_MFHI, insns::_mfhi }, { insn::kind::KIND_MULT, insns::_mult }, { insn::kind::KIND_MULTU, insns::_multu },
			{ insn::kind::KIND_NOOP, insns::_noop },
			{ insn::kind::KIND_NOR, insns::_nor }, { insn::kind::KIND_OR, insns::_or }, { insn::kind::KIND_ORI, insns::_ori },
			{ insn::kind::KIND_SB, insns::_sb }, { insn::kind::KIND_SH, insns::_sh }, { insn::kind::KIND_SW, insns::_sw },
			{ insn::kind::KIND_SLL, insns::_sll }, { insn::kind::KIND_SLLV, insns::_sllv },
			{ insn::kind::KIND_SLT, insns::_slt }, { insn::kind::KIND_SLTI, insns::_slti }, { insn::kind::KIND_SLTIU, insns::_sltiu }, { insn::kind::KIND_SLTU, insns::_sltu },
			{ insn::kind::KIND_SGT, insns::_sgt }, { insn::kind::KIND_SGTU, insns::_sgtu }, { insn::kind::KIND_SLE, insns::_sle }, { insn::kind::KIND_SLEU, insns::_sleu },
			{ insn::kind::KIND_SGE, insns::_sge }, { insn::kind::KIND_SGEU, insns::_sgeu }, { insn::kind::KIND_SNE, insns::_sne }, { insn::kind::KIND_SEQ, insns::_seq },
			{ insn::kind::KIND_SRA, insns::_sra }, { insn::kind::KIND_SRAV, insns::_srav },
			{ insn::kind::KIND_SRL, insns::_srl }, { insn::kind::KIND_SRLV, insns::_srlv },
			{ insn::kind::KIND_SUB, insns::_sub }, { insn::kind::KIND_SUBU, insns::_subu },
			{ insn::kind::KIND_SYSCALL, insns::_syscall },
			{ insn::kind::KIND_XOR, insns::_xor }, { insn::kind::KIND_XORI, insns::_xori },
			{ insn::kind::KIND_ADD_S, insns::_add_s }, { insn::kind::KIND_ADD_D, insns::_add_d },
			{ insn::kind::KIND_SUB_S, insns::_sub_s }, { insn::kind::KIND_SUB_D, insns::_sub_d },
			{ insn::kind::KIND_MUL_S, insns::_mul_s }, { insn::kind::KIND_MUL_D, insns::_mul_d },
			{ insn::kind::KIND_DIV_S, insns::_div_s }, { insn::kind::KIND_DIV_D, insns::_div_d },
			{ insn::kind::KIND_LWC1, insns::_lwc1 }, { insn::kind::KIND_LDC1, insns::_ldc1 },
			{ insn::kind::KIND_SWC1, insns::_swc1 }, { insn::kind::KIND_SDC1, insns::_sdc1 },
			{ insn::kind::KIND_L_S, insns::_l_s }, { insn::kind::KIND_L_D, insns::_l_d },
			{ insn::kind::KIND_BC1T, insns::_bc1t }, { insn::kind::KIND_BC1F, insns::_bc1f },
			{ insn::kind::KIND_CVT_D_S, insns::_cvt_d_s }, { insn::kind::KIND_CVT_D_W, insns::_cvt_d_w },
			{ insn::kind::KIND_CVT_S_D, insns::_cvt_s_d }, { insn::kind::KIND_CVT_S_W, insns::_cvt_s_w },
			{ insn::kind::KIND_CVT_W_D, insns::_cvt_w_d }, { insn::kind::KIND_CVT_W_S, insns::_cvt_w_s },
			{ insn::kind::KIND_C_EQ_D, insns::_c_eq_d }, { insn::kind::KIND_C_EQ_S, insns::_c_eq_s },
			{ insn::kind::KIND_C_GE_D, insns::_c_ge_d }, { insn::kind::KIND_C_GE_S, insns::_c_ge_s },
			{ insn::kind::KIND_C_GT_D, insns::_c_gt_d }, { insn::kind::KIND_C_GT_S, insns::_c_gt_s },
			{ insn::kind::KIND_C_LE_D, insns::_c_le_d }, { insn::kind::KIND_C_LE_S, insns::_c_le_s },
			{ insn::kind::KIND_C_LT_D, insns::_c_lt_d }, { insn::kind::KIND_C_LT_S, insns::_c_lt_s },
			{ insn::kind::KIND_C_NE_D, insns::_c_ne_d }, { insn::kind::KIND_C_NE_S, insns::_c_ne_s },
			{ insn::kind::KIND_MFC1, insns::_mfc1 }, { insn::kind::KIND_MTC1, insns::_mtc1 },
			{ insn::kind::KIND_MOV_D, insns::_mov_d }, { insn::kind::KIND_MOV_S, insns::_mov_s },
			{ insn::kind::KIND_NEG_D, insns::_neg_d }, { insn::kind::KIND_NEG_S, insns::_neg_s }
		};

		const static map<string, insn::kind> string_2_insn_kind = {
			{ insns::_add, insn::kind::KIND_ADD }, { insns::_addi, insn::kind::KIND_ADDI }, { insns::_addiu, insn::kind::KIND_ADDIU }, { insns::_addu, insn::kind::KIND_ADDU },
			{ insns::_and, insn::kind::KIND_AND }, { insns::_andi, insn::kind::KIND_ANDI },
			{ insns::_beq, insn::kind::KIND_BEQ }, { insns::_bgez, insn::kind::KIND_BGEZ }, { insns::_bgezal, insn::kind::KIND_BGEZAL }, {insns::_bgtz, insn::kind::KIND_BGTZ },
				{ insns::_blez, insn::kind::KIND_BLEZ }, { insns::_bltz, insn::kind::KIND_BLTZ }, { insns::_bltzal, insn::kind::KIND_BLTZAL }, { insns::_bne, insn::kind::KIND_BNE },
			{ insns::_div, insn::kind::KIND_DIV }, { insns::_divu, insn::kind::KIND_DIVU },
			{ insns::_jal, insn::kind::KIND_JAL }, { insns::_j, insn::kind::KIND_J }, { insns::_jr, insn::kind::KIND_JR },
			{ insns::_la, insn::kind::KIND_LA },
			{ insns::_lb, insn::kind::KIND_LB }, { insns::_lh, insn::kind::KIND_LH }, { insns::_lw, insn::kind::KIND_LW },
			{ insns::_lui, insn::kind::KIND_LUI },
			{ insns::_mflo, insn::kind::KIND_MFLO }, { insns::_mfhi, insn::kind::KIND_MFHI }, { insns::_mult, insn::kind::KIND_MULT }, { insns::_multu, insn::kind::KIND_MULTU },
			{ insns::_noop, insn::kind::KIND_NOOP },
			{ insns::_nor, insn::kind::KIND_NOR }, { insns::_or, insn::kind::KIND_OR }, { insns::_ori, insn::kind::KIND_ORI },
			{ insns::_sb, insn::kind::KIND_SB }, { insns::_sh, insn::kind::KIND_SH }, { insns::_sw, insn::kind::KIND_SW },
			{ insns::_sll, insn::kind::KIND_SLL }, { insns::_sllv, insn::kind::KIND_SLLV },
			{ insns::_slt, insn::kind::KIND_SLT }, { insns::_slti, insn::kind::KIND_SLTI }, { insns::_sltiu, insn::kind::KIND_SLTIU }, { insns::_sltu, insn::kind::KIND_SLTU },
			{ insns::_sgt, insn::kind::KIND_SGT }, { insns::_sgtu, insn::kind::KIND_SGTU }, { insns::_sle, insn::kind::KIND_SLE }, { insns::_sleu, insn::kind::KIND_SLEU },
			{ insns::_sge, insn::kind::KIND_SGE }, { insns::_sgeu, insn::kind::KIND_SGEU }, { insns::_sne, insn::kind::KIND_SNE }, { insns::_seq, insn::kind::KIND_SEQ },
			{ insns::_sra, insn::kind::KIND_SRA }, { insns::_srav, insn::kind::KIND_SRAV },
			{ insns::_srl, insn::kind::KIND_SRL }, { insns::_srlv, insn::kind::KIND_SRLV },
			{ insns::_sub, insn::kind::KIND_SUB }, { insns::_subu, insn::kind::KIND_SUBU },
			{ insns::_syscall, insn::kind::KIND_SYSCALL },
			{ insns::_xor, insn::kind::KIND_XOR }, { insns::_xori, insn::kind::KIND_XORI },
			{ insns::_add_s, insn::kind::KIND_ADD_S }, { insns::_add_d, insn::kind::KIND_ADD_D },
			{ insns::_sub_s, insn::kind::KIND_SUB_S }, { insns::_sub_d, insn::kind::KIND_SUB_D },
			{ insns::_mul_s, insn::kind::KIND_MUL_S }, { insns::_mul_d, insn::kind::KIND_MUL_D },
			{ insns::_div_s, insn::kind::KIND_DIV_S }, { insns::_div_d, insn::kind::KIND_DIV_D },
			{ insns::_lwc1, insn::kind::KIND_LWC1 }, { insns::_ldc1, insn::kind::KIND_LDC1 },
			{ insns::_swc1, insn::kind::KIND_SWC1 }, { insns::_sdc1, insn::kind::KIND_SDC1 },
			{ insns::_l_s, insn::kind::KIND_L_S }, { insns::_l_d, insn::kind::KIND_L_D },
			{ insns::_bc1t, insn::kind::KIND_BC1T }, { insns::_bc1f, insn::kind::KIND_BC1F },
			{ insns::_cvt_d_s, insn::kind::KIND_CVT_D_S }, { insns::_cvt_d_w, insn::kind::KIND_CVT_D_W },
			{ insns::_cvt_s_d, insn::kind::KIND_CVT_S_D }, { insns::_cvt_s_w, insn::kind::KIND_CVT_S_W },
			{ insns::_cvt_w_d, insn::kind::KIND_CVT_W_D }, { insns::_cvt_w_s, insn::kind::KIND_CVT_W_S },
			{ insns::_c_eq_d, insn::kind::KIND_C_EQ_D }, { insns::_c_eq_s, insn::kind::KIND_C_EQ_S },
			{ insns::_c_ge_d, insn::kind::KIND_C_GE_D }, { insns::_c_ge_s, insn::kind::KIND_C_GE_S },
			{ insns::_c_gt_d, insn::kind::KIND_C_GT_D }, { insns::_c_gt_s, insn::kind::KIND_C_GT_S },
			{ insns::_c_le_d, insn::kind::KIND_C_LE_D }, { insns::_c_le_s, insn::kind::KIND_C_LE_S },
			{ insns::_c_lt_d, insn::kind::KIND_C_LT_D }, { insns::_c_lt_s, insn::kind::KIND_C_LT_S },
			{ insns::_c_ne_d, insn::kind::KIND_C_NE_D }, { insns::_c_ne_s, insn::kind::KIND_C_NE_S },
			{ insns::_mfc1, insn::kind::KIND_MFC1 }, { insns::_mtc1, insn::kind::KIND_MTC1 },
			{ insns::_mov_d, insn::kind::KIND_MOV_D }, { insns::_mov_s, insn::kind::KIND_MOV_S },
			{ insns::_neg_d, insn::kind::KIND_NEG_D }, { insns::_neg_s, insn::kind::KIND_NEG_S }
		};

		operand::operand(kind k) : _operand_kind(k) {

		}

		operand::~operand() {

		}

		operand::kind operand::operand_kind() {
			return _operand_kind;
		}

		register_operand::register_operand(int rn) : operand(operand::kind::KIND_REGISTER), _register_number(rn), _register_text(register_file::int_2_register.at(rn)) {

		}

		register_operand::register_operand(string rt) : operand(operand::kind::KIND_REGISTER), _register_text(rt), _register_number(register_file::register_2_int.at(rt)) {

		}

		register_operand::~register_operand() {

		}

		int register_operand::register_number() {
			return _register_number;
		}

		string register_operand::register_text() {
			return _register_text;
		}

		string register_operand::raw_text() {
			return _register_text;
		}

		label_operand::label_operand(string lt) : operand(operand::kind::KIND_LABEL), _label_text(lt) {

		}

		label_operand::~label_operand() {

		}

		string label_operand::label_text() {
			return _label_text;
		}

		string label_operand::raw_text() {
			return _label_text;
		}

		immediate_operand::immediate_operand(int ii) : operand(operand::kind::KIND_IMMEDIATE), _integral_immediate(ii), _immediate_operand_kind(immediate_operand::kind::KIND_INTEGRAL) {

		}

		immediate_operand::immediate_operand(string c) : operand(operand::kind::KIND_IMMEDIATE), _immediate_operand_char(c),
			_immediate_operand_kind(immediate_operand::kind::KIND_CHAR) {

		}

		immediate_operand::~immediate_operand() {

		}

		int immediate_operand::integral_immediate() {
			return _integral_immediate;
		}

		string immediate_operand::char_immediate() {
			return _immediate_operand_char;
		}

		immediate_operand::kind immediate_operand::immediate_operand_kind() {
			return _immediate_operand_kind;
		}

		string immediate_operand::raw_text() {
			if (_immediate_operand_kind == immediate_operand::kind::KIND_INTEGRAL)
				return to_string(_integral_immediate);
			else if (_immediate_operand_kind == immediate_operand::kind::KIND_CHAR)
				return _immediate_operand_char;
			return "";
		}

		insn::insn(insn::kind ik, bool t) : _insn_kind(ik), _insn_operand_number(insn::operand_number::KIND_0), _insn_type(insn::type::TYPE_REGULAR), _tab(t) {

		}

		insn::insn(insn::kind ik, shared_ptr<operand> o1, bool t) : _insn_kind(ik), _insn_operand_number(insn::operand_number::KIND_1), _operand_1(o1), _insn_type(insn::type::TYPE_REGULAR),
			_tab(t) {

		}

		insn::insn(insn::kind ik, shared_ptr<operand> o1, shared_ptr<operand> o2, bool t) : _insn_kind(ik), _insn_operand_number(insn::operand_number::KIND_2), _operand_1(o1), _operand_2(o2),
			_tab(t), _insn_type(insn::type::TYPE_REGULAR) {

		}

		insn::insn(insn::kind ik, shared_ptr<operand> o1, shared_ptr<operand> o2, shared_ptr<operand> o3, bool t) : _insn_kind(ik), _insn_operand_number(insn::operand_number::KIND_3), _tab(t),
			_operand_1(o1), _operand_2(o2), _operand_3(o3), _insn_type(insn::type::TYPE_REGULAR) {

		}

		insn::insn(string s) : _label_name(s), _insn_type(insn::type::TYPE_LABEL), _insn_operand_number(insn::operand_number::KIND_0), _tab(false) {

		}

		insn::insn(insn::type it, string s, bool t) : _label_name(s), _insn_type(it), _insn_operand_number(insn::operand_number::KIND_0), _tab(t) {

		}
		
		insn::insn() : _insn_type(insn::type::TYPE_NONE), _insn_operand_number(insn::operand_number::KIND_NONE), _tab(false) {

		}

		insn::~insn() {

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

		insn::kind insn::insn_kind() {
			return _insn_kind;
		}

		insn::operand_number insn::insn_operand_number() {
			return _insn_operand_number;
		}

		insn::type insn::insn_type() {
			return _insn_type;
		}

		string insn::label_name() {
			return _label_name;
		}

		string insn::raw_text() {
			string to_use = _tab ? "\t" : "";
			if (_insn_operand_number == insn::operand_number::KIND_0 && _insn_type == insn::type::TYPE_NONE)
				return to_use;
			else if (_insn_operand_number == insn::operand_number::KIND_0) {
				if (_insn_type == insn::type::TYPE_REGULAR)
					return to_use + insn_kind_2_string.at(_insn_kind);
				else if (_insn_type == insn::type::TYPE_RAW)
					return _label_name;
				else
					return _label_name + ":";
			}
			else if (_insn_operand_number == insn::operand_number::KIND_1)
				return to_use + insn_kind_2_string.at(_insn_kind) + " " + get_operand_text(_operand_1);
			else if (_insn_operand_number == insn::operand_number::KIND_2)
				return  to_use + insn_kind_2_string.at(_insn_kind) + " " + get_operand_text(_operand_1) + ", " + get_operand_text(_operand_2);
			else if (_insn_operand_number == insn::operand_number::KIND_3) {
				if (_insn_kind == insn::kind::KIND_LW || _insn_kind == insn::kind::KIND_LH || _insn_kind == insn::kind::KIND_LB || _insn_kind == insn::kind::KIND_LBU ||
					_insn_kind == insn::kind::KIND_SW || _insn_kind == insn::kind::KIND_SB || _insn_kind == insn::kind::KIND_SH || _insn_kind == insn::kind::KIND_LHU ||
					_insn_kind == insn::kind::KIND_LWC1 || _insn_kind == insn::kind::KIND_LDC1 || _insn_kind == insn::kind::KIND_SWC1 || _insn_kind == insn::kind::KIND_SDC1)
					return to_use + insn_kind_2_string.at(_insn_kind) + " " + get_operand_text(_operand_1) + ", " + get_operand_text(_operand_2) + "(" + get_operand_text(_operand_3) + ")";
				else
					return to_use + insn_kind_2_string.at(_insn_kind) + " " + get_operand_text(_operand_1) + ", " + get_operand_text(_operand_2) + ", " + get_operand_text(_operand_3);
			}
			return "";
		}

		bool insn::tab() {
			return _tab;
		}

		data_directive::data_directive(bool t, shared_ptr<label_operand> i, int s) : _identifier(i), _space(s), _tab(t), 
			_data_directive_kind(data_directive::directive_kind::KIND_SPACE) {

		}

		data_directive::data_directive(bool t, shared_ptr<label_operand> i, float f) : _identifier(i), _float_data(f), _tab(t),
			_data_directive_kind(data_directive::directive_kind::KIND_FLOAT) {

		}

		data_directive::data_directive(bool t, shared_ptr<label_operand> i, double d) : _identifier(i), _double_data(d), _tab(t),
			_data_directive_kind(data_directive::directive_kind::KIND_DOUBLE) {

		}

		data_directive::data_directive(bool t, shared_ptr<label_operand> i, string s) : _identifier(i), _string_data(s), _tab(t),
			_data_directive_kind(data_directive::directive_kind::KIND_STRING) {

		}

		data_directive::~data_directive() {

		}

		shared_ptr<label_operand> data_directive::identifier() {
			return _identifier;
		}

		int data_directive::space() {
			return _space;
		}

		bool data_directive::tab() {
			return _tab;
		}

		data_directive::directive_kind data_directive::data_directive_kind() {
			return _data_directive_kind;
		}

		float data_directive::float_data() {
			return _float_data;
		}

		double data_directive::double_data() {
			return _double_data;
		}
		
		string data_directive::string_data() {
			return _string_data;
		}

		string data_directive::raw_text() {
			string prefix = _tab ? "\t" : "";
			if (_data_directive_kind == data_directive::directive_kind::KIND_SPACE)
				return prefix + ".align 8\n" + prefix + get_operand_text(_identifier) + ": .space " + to_string(_space);
			else if (_data_directive_kind == data_directive::directive_kind::KIND_DOUBLE) {
				ostringstream oss;
				oss << setprecision(numeric_limits<double>::max_digits10 - 1) << _double_data;
				return prefix + get_operand_text(_identifier) + ": .double " + oss.str();
			}
			else if (_data_directive_kind == data_directive::directive_kind::KIND_STRING)
				return prefix + ".align 8\n" + prefix + get_operand_text(_identifier) + ": .asciiz " + _string_data;
			else {
				ostringstream oss;
				oss << setprecision(numeric_limits<float>::max_digits10 - 1) << _float_data;
				return prefix + ".align 8\n" + prefix + get_operand_text(_identifier) + ": .float " + oss.str();
			}
		}

		string get_operand_text(shared_ptr<operand> o) {
			if (o == nullptr) return "";
			if (o->operand_kind() == operand::kind::KIND_IMMEDIATE)
				return static_pointer_cast<immediate_operand>(o)->raw_text();
			else if (o->operand_kind() == operand::kind::KIND_LABEL)
				return static_pointer_cast<label_operand>(o)->raw_text();
			else if (o->operand_kind() == operand::kind::KIND_REGISTER)
				return static_pointer_cast<register_operand>(o)->raw_text();
			return "";
		}

		register_wrapper::register_wrapper(long wid, shared_ptr<register_operand> rro) : _wrapper_id(wid), _offset_from_stack_ptr(0), _raw_register_operand(rro),
			_register_wrapper_state_kind(register_wrapper::state_kind::KIND_REGULAR), _register_wrapper_variable_kind(register_wrapper::variable_kind::KIND_NON_VARIABLE),
			_identifier(nullptr), _register_wrapper_function_argument_kind(register_wrapper::function_argument_kind::KIND_NON_FUNCTION_ARGUMENT), _already_loaded(false),
			_double_precision(false) {

		}

		register_wrapper::~register_wrapper() {

		}

		long register_wrapper::wrapper_id() {
			return _wrapper_id;
		}

		int register_wrapper::offset_from_stack_ptr() {
			return _offset_from_stack_ptr;
		}

		void register_wrapper::set_offset_from_stack_ptr(int o) {
			_offset_from_stack_ptr = o;
		}

		void register_wrapper::update_offset_from_stack_ptr(int o) {
			_offset_from_stack_ptr += o;
		}

		shared_ptr<register_operand> register_wrapper::raw_register_operand() {
			return _raw_register_operand;
		}

		register_wrapper::state_kind register_wrapper::register_wrapper_state_kind() {
			return _register_wrapper_state_kind;
		}

		void register_wrapper::set_register_wrapper_state_kind(register_wrapper::state_kind sk) {
			_register_wrapper_state_kind = sk;
		}

		void register_wrapper::set_register_wrapper_variable_kind(register_wrapper::variable_kind vk) {
			_register_wrapper_variable_kind = vk;
		}

		register_wrapper::variable_kind register_wrapper::register_wrapper_variable_kind() {
			return _register_wrapper_variable_kind;
		}

		shared_ptr<label_operand> register_wrapper::identifier() {
			return _identifier;
		}

		void register_wrapper::set_identifier(shared_ptr<label_operand> lo) {
			_identifier = lo;
		}

		void register_wrapper::set_register_wrapper_function_argument_kind(register_wrapper::function_argument_kind fak) {
			_register_wrapper_function_argument_kind = fak;
		}

		register_wrapper::function_argument_kind register_wrapper::register_wrapper_function_argument_kind() {
			return _register_wrapper_function_argument_kind;
		}

		bool register_wrapper::already_loaded() {
			return _already_loaded;
		}

		void register_wrapper::set_already_loaded(bool b) {
			_already_loaded = b;
		}
		
		void register_wrapper::set_double_precision(bool b) {
			_double_precision = b;
		}

		bool register_wrapper::double_precision() {
			return _double_precision;
		}

		operand_wrapper::operand_wrapper(shared_ptr<register_wrapper> rw) : _wrapper_operand_kind(operand_wrapper::operand_kind::KIND_REGISTER),
			_contained_register_wrapper(rw) {

		}

		operand_wrapper::operand_wrapper(shared_ptr<immediate_operand> io) : _wrapper_operand_kind(operand_wrapper::operand_kind::KIND_IMMEDIATE),
			_contained_immediate_operand(io) {

		}

		operand_wrapper::operand_wrapper(shared_ptr<label_operand> lo) : _wrapper_operand_kind(operand_wrapper::operand_kind::KIND_LABEL),
			_contained_label_operand(lo){

		}

		operand_wrapper::~operand_wrapper() {

		}

		shared_ptr<register_wrapper> operand_wrapper::contained_register_wrapper() {
			return _contained_register_wrapper;
		}

		shared_ptr<immediate_operand> operand_wrapper::contained_immediate_operand() {
			return _contained_immediate_operand;
		}

		shared_ptr<label_operand> operand_wrapper::contained_label_operand() {
			return _contained_label_operand;
		}

		operand_wrapper::operand_kind operand_wrapper::wrapper_operand_kind() {
			return _wrapper_operand_kind;
		}

		mips_frame::mips_frame(long& s, shared_ptr<mips_frame> pf, bool mf) : _local_stack_offset(0), _register_wrapper_list(vector<shared_ptr<register_wrapper>>{}), _register_overflow_counter(0),
			_register_wrapper_id_counter(s), _s_register_2_initial_offset_map(map<int, int>{}), _variable_name_2_id_map(map<string, long>{}), _id_2_variable_name_map(map<long, string>{}),
			_main_function(mf), _parent_frame(pf), _fp_register_overflow_counter(0), _fp_return_value(false), _dp_return_value(false) {
			_register_usage = map<int, bool>{
				{ 0, false }, { 1, false }, { 2, false }, { 3, false }, { 4, false }, { 5, false }, { 6, false }, { 7, false }, { 8, false }, { 9, false }, { 10, false },
				{ 11, false }, { 12, false }, { 13, false }, { 14, false }, { 15, false }, { 16, false }, { 17, false }, { 18, false }, { 19, false }, { 20, false },
				{ 21, false }, { 22, false }, { 23, false }, { 24, false }, { 25, false }, { 26, false }, { 27, false }, { 28, false }, { 29, false }, { 30, false },
				{ 31, false }, { 32, false }, { 33, false }, { 34, false }, { 35, false }, { 36, false }, { 37, false }, { 38, false }, { 39, false }, { 40, false },
				{ 41, false }, { 42, false }, { 43, false }, { 44, false }, { 45, false }, { 46, false }, { 47, false }, { 48, false }, { 49, false }, { 50, false },
				{ 51, false }, { 52, false }, { 53, false }, { 54, false }, { 55, false }, { 56, false }, { 57, false }, { 58, false }, { 59, false }, { 60, false },
				{ 61, false }, { 62, false }, { 63, false }
			};
			_s0_7_initial_store = map<int, bool>{
				{ 0, false }, { 1, false }, { 2, false }, { 3, false }, { 4, false }, { 5, false }, { 6, false }, { 7, false },
				{ 8, false }, { 9, false }, { 10, false }, { 11, false }, { 12, false }, { 13, false }
			};
			_s_register_2_initial_offset_map = map<int, int>{
				{ 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 }, { 5, 0 }, { 6, 0 }, { 7, 0 },
				{ 8, 0 }, { 9, 0 }, { 10, 0 }, { 11, 0 }, { 12, 0 }, { 13, 0 }
			};
			_variable_name_2_id_map = map<string, long>{};
			_id_2_variable_name_map = map<long, string>{};
		}

		mips_frame::~mips_frame() {

		}

		int mips_frame::local_stack_offset() {
			return _local_stack_offset;
		}

		void mips_frame::change_local_stack_offset(int o) {
			_local_stack_offset += o;
			for (shared_ptr<register_wrapper>& rw : _register_wrapper_list)
				if (rw->register_wrapper_state_kind() == register_wrapper::state_kind::KIND_ON_STACK)
					rw->update_offset_from_stack_ptr(-o);
			for (int i = 0; i < 8; i++)
				if (_s0_7_initial_store[i])
					_s_register_2_initial_offset_map[i] += -o;
			for (int i = 8; i < 14; i++)
				if (_s0_7_initial_store[i])
					_s_register_2_initial_offset_map[i] += -o;
		}

		int mips_frame::current_overflowed_register() {
			return _register_overflow_counter;
		}

		int mips_frame::next_overflow_register() {
			return _register_overflow_counter++ % 16;
		}

		int mips_frame::current_fp_overflowed_register() {
			return _fp_register_overflow_counter;
		}

		int mips_frame::next_fp_overflow_register() {
			return _fp_register_overflow_counter++ % 10;
		}

		bool mips_frame::is_register_marked(int r) {
			if (_register_usage.find(r) == _register_usage.end()) return false;
			return _register_usage[r];
		}

		void mips_frame::mark_register(int r) {
			if (_register_usage.find(r) == _register_usage.end()) return;
			_register_usage[r] = true;
		}

		void mips_frame::unmark_register(int r) {
			if (_register_usage.find(r) == _register_usage.end()) return;
			_register_usage[r] = false;
		}

		bool mips_frame::is_s_register_initially_stored(int r) {
			if (_s0_7_initial_store.find(r) == _s0_7_initial_store.end()) return false;
			return _s0_7_initial_store[r];
		}

		void mips_frame::mark_s_register_initially_stored(int r) {
			if (_s0_7_initial_store.find(r) == _s0_7_initial_store.end()) return;
			_s0_7_initial_store[r] = true;
		}

		void mips_frame::remove_register_wrapper(long id) {
			for (int i = 0; i < _register_wrapper_list.size(); i++)
				if (_register_wrapper_list[i]->wrapper_id() == id)
					_register_wrapper_list.erase(_register_wrapper_list.begin() + i, _register_wrapper_list.begin() + i + 1);
		}

		shared_ptr<register_wrapper> mips_frame::find_last_usage(int reg) {
			for (int i = _register_wrapper_list.size() - 1; i >= 0; i--) {
				shared_ptr<register_wrapper> rw = _register_wrapper_list[i];
				if (rw->raw_register_operand()->register_number() == reg && rw->register_wrapper_variable_kind() != register_wrapper::variable_kind::KIND_VARIABLE)
					return rw;
			}
			return nullptr;
		}

		shared_ptr<register_wrapper> mips_frame::get_register_wrapper(long id) {
			for (shared_ptr<register_wrapper> rw : _register_wrapper_list)
				if (rw->wrapper_id() == id)
					return rw;
			if (_parent_frame != nullptr)
				return _parent_frame->get_register_wrapper(id);
			return make_shared<register_wrapper>(-1, register_file::register_objects::_zero_register);
		}

		shared_ptr<register_wrapper> mips_frame::generate_register_wrapper(shared_ptr<register_operand> ro) {
			shared_ptr<register_wrapper> rw = make_shared<register_wrapper>(_register_wrapper_id_counter++, ro);;
			_register_wrapper_list.push_back(rw);
			return rw;
		}

		void mips_frame::set_s_register_initial_offset(int r, int o) {
			if (_s_register_2_initial_offset_map.find(r) == _s_register_2_initial_offset_map.end()) return;
			_s_register_2_initial_offset_map[r] = o;
			return;
		}

		void mips_frame::restore_s_registers(shared_ptr<mips_code> mc) {
			shared_ptr<register_operand> sp = register_file::register_objects::register_2_register_object.at(register_file::_sp);
			for (int i = 0; i < 8; i++) {
				if (_s0_7_initial_store[i]) {
					int r = register_file::register_2_int.at(register_file::_s0) + i;
					shared_ptr<register_operand> curr_reg = register_file::register_objects::int_2_register_object.at(r);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, curr_reg, make_shared<immediate_operand>(_s_register_2_initial_offset_map[i]), sp));
				}
				_s_register_2_initial_offset_map[i] = 0;
				_s0_7_initial_store[i] = false;
			}
			for (int i = 8; i < 14; i++) {
				if (_s0_7_initial_store[i]) {
					int r = register_file::register_2_int.at(register_file::_f20) + (i - 8) * 2;
					shared_ptr<register_operand> curr_reg = register_file::register_objects::int_2_register_object.at(r);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_LDC1, curr_reg, make_shared<immediate_operand>(_s_register_2_initial_offset_map[i]), sp));
				}
				_s_register_2_initial_offset_map[i] = 0;
				_s0_7_initial_store[i] = false;
			}
		}

		void mips_frame::add_variable_id(string name, long id) {
			_variable_name_2_id_map[name] = id;
			_id_2_variable_name_map[id] = name;
		}

		long mips_frame::get_variable_id(string name) {
			if (_variable_name_2_id_map.find(name) == _variable_name_2_id_map.end()) {
				if (_parent_frame != nullptr)
					return _parent_frame->get_variable_id(name);
				else
					return -1;
			}
			return _variable_name_2_id_map[name];
		}

		string mips_frame::get_variable(long id) {
			if (_id_2_variable_name_map.find(id) == _id_2_variable_name_map.end()) {
				if (_parent_frame != nullptr)
					return _parent_frame->get_variable(id);
				else
					return "";
			}
			return _id_2_variable_name_map[id];
		}
		
		bool mips_frame::main_function() {
			return _main_function;
		}

		shared_ptr<mips_frame> mips_frame::parent_frame() {
			return _parent_frame;
		}

		void mips_frame::set_fp_return_value(bool b) {
			_fp_return_value = b;
		}

		bool mips_frame::fp_return_value() {
			return _fp_return_value;
		}

		bool mips_frame::dp_return_value() {
			return _dp_return_value;
		}

		void mips_frame::set_dp_return_value(bool b) {
			_dp_return_value = b;
		}

		mips_code::mips_code(shared_ptr<spectre::parser::parser> p) : _insn_list(vector<shared_ptr<insn>>{}), _ast_parser(p), _label_number_counter(0),
			_frame_stack(stack<shared_ptr<mips_frame>>{}), _global_register_wrapper_counter(0), _loop_begin_stack(stack<shared_ptr<label_operand>>{}),
			_loop_end_stack(stack<shared_ptr<label_operand>>{}), _misc_counter(0), _initial_loop_offset_stack(stack<int>{}), _global_symbol_table(vector<shared_ptr<label_operand>>{}) {
			_frame_stack.push(make_shared<mips_frame>(_global_register_wrapper_counter, nullptr, false));
		}

		mips_code::~mips_code() {

		}

		void mips_code::report(error e) {
			_ast_parser->report(e);
		}

		void mips_code::report_internal(string msg, string fn, int ln, string fl) {
			_ast_parser->report_internal(msg, fn, ln, fl);
		}

		vector<shared_ptr<insn>> mips_code::insn_list() {
			return _insn_list;
		}

		vector<string> mips_code::raw_insn_list() {
			vector<string> ret;
			ret.push_back(".data");
			for (shared_ptr<data_directive> dd : _dd_list)
				ret.push_back(dd->raw_text());
			ret.push_back(".text");
			for (shared_ptr<label_operand> lo : _global_symbol_table)
				ret.push_back("\t.globl " + lo->raw_text());
			ret.push_back(make_shared<insn>()->raw_text());
			for (shared_ptr<insn> i : _insn_list)
				ret.push_back(i->raw_text());
			return ret;
		}

		shared_ptr<mips_frame> mips_code::pop_frame() {
			shared_ptr<mips_frame> mf = _frame_stack.top();
			_frame_stack.pop();
			return mf;
		}

		shared_ptr<mips_frame> mips_code::current_frame() {
			return _frame_stack.top();
		}

		void mips_code::add_frame(shared_ptr<mips_frame> mf) {
			_frame_stack.push(mf);
		}

		shared_ptr<mips_frame> mips_code::generate_new_frame(bool mf) {
			_global_register_wrapper_counter++;
			return make_shared<mips_frame>(_global_register_wrapper_counter, _frame_stack.top(), mf);
		}

		void mips_code::add_insn(shared_ptr<insn> i) {
			_insn_list.push_back(i);
		}

		void mips_code::add_insn_begin(shared_ptr<insn> i) {
			_insn_list.insert(_insn_list.begin(), i);
		}

		void mips_code::add_data_directive(shared_ptr<data_directive> dd, bool g) {
			if (g)
				_global_symbol_table.push_back(dd->identifier());
			_dd_list.push_back(dd);
		}

		tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> mips_code::next_label_info() {
			int ln = _label_number_counter++;
			string l = ".L" + to_string(ln);
			return make_tuple(ln, l, make_shared<label_operand>(l), make_shared<insn>(l));
		}

		tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> mips_code::generate_case_label_info(int c) {
			string l = ".C" + to_string(c);
			return make_tuple(c, l, make_shared<label_operand>(l), make_shared<insn>(l));
		}

		shared_ptr<struct_symbol> mips_code::find_struct_symbol(token n, int r) {
			return _ast_parser->find_struct_symbol(n, r);
		}

		void mips_code::push_loop_begin_label_operand(shared_ptr<label_operand> lo) {
			_loop_begin_stack.push(lo);
		}

		void mips_code::push_loop_end_label_operand(shared_ptr<label_operand> lo) {
			_loop_end_stack.push(lo);
		}

		void mips_code::pop_loop_begin_label_operand() {
			_loop_begin_stack.pop();
		}

		void mips_code::pop_loop_end_label_operand() {
			_loop_end_stack.pop();
		}

		shared_ptr<label_operand> mips_code::current_loop_end_label_operand() {
			return _loop_end_stack.top();
		}

		shared_ptr<label_operand> mips_code::current_loop_begin_label_operand() {
			return _loop_begin_stack.top();
		}

		int mips_code::current_misc_counter() {
			return _misc_counter;
		}

		int mips_code::next_misc_counter() {
			return _misc_counter++;
		}

		int mips_code::current_initial_loop_offset() {
			if (_initial_loop_offset_stack.size() == 0) return -1;
			return _initial_loop_offset_stack.top();
		}

		void mips_code::push_initial_loop_offset(int i) {
			_initial_loop_offset_stack.push(i);
		}

		void mips_code::pop_initial_loop_offset() {
			_initial_loop_offset_stack.pop();
		}

		void mips_code::add_to_global_symbol_table(shared_ptr<label_operand> lo) {
			_global_symbol_table.push_back(lo);
		}

		void generate_mips(shared_ptr<mips_code> mc, shared_ptr<spectre::parser::parser> p) {
			mc->add_data_directive(make_shared<data_directive>(true, make_shared<label_operand>("__fp_1"), (float) 1.0), false);
			mc->add_data_directive(make_shared<data_directive>(true, make_shared<label_operand>("__fp_m1"), (float) -1.0), false);
			mc->add_data_directive(make_shared<data_directive>(true, make_shared<label_operand>("__dp_1"), (double) 1.0), false);
			mc->add_data_directive(make_shared<data_directive>(true, make_shared<label_operand>("__dp_m1"), (double) -1.0), false);
			mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, register_file::register_objects::_sp_register, register_file::register_objects::_sp_register,
				make_shared<immediate_operand>(-4)));
			mc->current_frame()->change_local_stack_offset(-4);
			vector<shared_ptr<stmt>> reordered = reorder_stmt_list(mc, p->stmt_list());
			for (shared_ptr<stmt> s : reordered)
				generate_stmt_mips(mc, s);
			shared_ptr<register_operand> sp = register_file::register_objects::register_2_register_object.at(register_file::_sp);
			if (mc->current_frame()->local_stack_offset() != 0)
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-mc->current_frame()->local_stack_offset())));
		}

		void generate_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<stmt> s) {
			if (s == nullptr)
				return;
			switch (s->stmt_kind()) {
			case stmt::kind::KIND_EXPRESSION: {
				int pre_off = mc->current_frame()->local_stack_offset();
				shared_ptr<operand_wrapper> ow = generate_expression_mips(mc, s->stmt_expression(), binary_expression::operator_kind::KIND_NONE, nullptr,
					vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
				if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, ow->contained_immediate_operand()));
					free_general_purpose_register(mc, rw);
				}
				else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
					if(ow->contained_register_wrapper()->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
						free_general_purpose_register(mc, ow->contained_register_wrapper());
				}
				int post_off = mc->current_frame()->local_stack_offset();
				mc->current_frame()->restore_s_registers(mc);
				if (pre_off != post_off) {
					shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(pre_off - post_off)));
					mc->current_frame()->change_local_stack_offset(pre_off - post_off);
				}
				return;
			}
				break;
			case stmt::kind::KIND_VARIABLE_DECLARATION: {
				for (shared_ptr<variable_declaration> vdecl : s->stmt_variable_declaration_list())
					generate_variable_declaration_mips(mc, vdecl);
			}
				break;
			case stmt::kind::KIND_IF: {
				generate_if_stmt_mips(mc, s->stmt_if());
			}
				break;
			case stmt::kind::KIND_WHILE: {
				generate_while_stmt_mips(mc, s->stmt_while());
			}
				break;
			case stmt::kind::KIND_BLOCK: {
				generate_block_stmt_mips(mc, s->stmt_block());
			}
				break;
			case stmt::kind::KIND_BREAK_CONTINUE: {
				generate_break_continue_stmt_mips(mc, s->stmt_break_continue());
			}
				break;
			case stmt::kind::KIND_DO_WHILE: {
				generate_do_while_stmt_mips(mc, s->stmt_while());
			}
				break;
			case stmt::kind::KIND_FOR: {
				generate_for_stmt_mips(mc, s->stmt_for());
			}
				break;
			case stmt::kind::KIND_SWITCH: {
				generate_switch_stmt_mips(mc, s->stmt_switch());
			}
				break;
			case stmt::kind::KIND_CASE_DEFAULT: {
				generate_case_default_stmt_mips(mc, s->stmt_case_default());
			}
				break;
			case stmt::kind::KIND_FUNCTION: {
				generate_function_stmt_mips(mc, s->stmt_function());
			}
				break;
			case stmt::kind::KIND_RETURN: {
				generate_return_stmt_mips(mc, s->stmt_return());
			}
				break;
			case stmt::kind::KIND_STRUCT:
			case stmt::kind::KIND_USING:
			case stmt::kind::KIND_INCLUDE:
			case stmt::kind::KIND_IMPORT:
			case stmt::kind::KIND_EMPTY:
				return;
				break;
			case stmt::kind::KIND_NAMESPACE:
				generate_namespace_stmt_mips(mc, s->stmt_namespace());
				break;
			case stmt::kind::KIND_ASM:
				generate_asm_stmt_mips(mc, s->stmt_asm());
				break;
			}
			mc->current_frame()->restore_s_registers(mc);
		}

		shared_ptr<operand_wrapper> generate_expression_mips(shared_ptr<mips_code> mc, shared_ptr<expression> e, binary_expression::operator_kind assign_op, shared_ptr<operand_wrapper> assign_to,
			vector<unary_expression::kind> utl, vector<shared_ptr<postfix_expression::postfix_type>> ptl) {
			if (e == nullptr || !e->valid())
				return nullptr;
			shared_ptr<operand_wrapper> ret = nullptr;
			int counter = 0;
			for (shared_ptr<assignment_expression> ae : e->assignment_expression_list()) {
				if (counter == e->assignment_expression_list().size() - 1)
					ret = generate_assignment_expression_mips(mc, ae, assign_op, assign_to, utl, ptl);
				else
					ret = generate_assignment_expression_mips(mc, ae, binary_expression::operator_kind::KIND_NONE, nullptr, vector<unary_expression::kind>{},
						vector<shared_ptr<postfix_expression::postfix_type>>{});
				if (counter != e->assignment_expression_list().size() - 1) {
					if (ret->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, ret->contained_immediate_operand()));
						free_general_purpose_register(mc, rw);
					}
					else if (ret->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
						if(ret->contained_register_wrapper()->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
							free_general_purpose_register(mc, ret->contained_register_wrapper());
					}
				}
				counter++;
			}
			return ret;
		}

		shared_ptr<operand_wrapper> generate_assignment_expression_mips(shared_ptr<mips_code> mc, shared_ptr<assignment_expression> ae, binary_expression::operator_kind assign_op,
			shared_ptr<operand_wrapper> assign_to, vector<unary_expression::kind> utl, vector<shared_ptr<postfix_expression::postfix_type>> ptl) {
			if (ae == nullptr || !ae->valid())
				return nullptr;
			switch (ae->assignment_expression_kind()) {
			case assignment_expression::kind::KIND_ASSIGNMENT: {
				shared_ptr<unary_expression> lhs = ae->lhs_assignment();
				shared_ptr<assignment_expression> rhs = ae->rhs_assignment();
				shared_ptr<operand_wrapper> ow = generate_assignment_expression_mips(mc, rhs, binary_expression::operator_kind::KIND_NONE, nullptr, vector<unary_expression::kind>{},
					vector<shared_ptr<postfix_expression::postfix_type>>{});
				shared_ptr<register_wrapper> rhs_op = nullptr;
				if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					rhs_op = allocate_general_purpose_register(mc);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rhs_op->raw_register_operand(), register_file::register_objects::_zero_register,
						ow->contained_immediate_operand()));
				}
				else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
					rhs_op = ow->contained_register_wrapper();
				else
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				shared_ptr<operand_wrapper> lhs_op = generate_unary_expression_mips(mc, lhs, ae->assignment_operator_kind(), ow, utl, ptl);
				if (lhs_op->wrapper_operand_kind() != operand_wrapper::operand_kind::KIND_REGISTER)
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				if (ae->assignment_expression_type()->type_kind() == type::kind::KIND_PRIMITIVE && ae->assignment_expression_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
					int sz = (int)calculate_type_size(mc, ae->assignment_expression_type());
					shared_ptr<primitive_type> ae_pt = static_pointer_cast<primitive_type>(ae->assignment_expression_type());
					if (ae->assignment_operator_kind() == binary_expression::operator_kind::KIND_EQUALS) {
						if (lhs_op->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
							if (rhs_op->raw_register_operand()->register_number() > 31 && lhs_op->contained_register_wrapper()->raw_register_operand()->register_number() > 31) {
								if (lhs_op->contained_register_wrapper()->double_precision() && rhs_op->double_precision())
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, lhs_op->contained_register_wrapper()->raw_register_operand(), rhs_op->raw_register_operand()));
								else if (!lhs_op->contained_register_wrapper()->double_precision() && !rhs_op->double_precision())
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, lhs_op->contained_register_wrapper()->raw_register_operand(), rhs_op->raw_register_operand()));
								else if (!lhs_op->contained_register_wrapper()->double_precision() && rhs_op->double_precision()) {
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, lhs_op->contained_register_wrapper()->raw_register_operand(), rhs_op->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_D, lhs_op->contained_register_wrapper()->raw_register_operand(),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
								}
								else {
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, lhs_op->contained_register_wrapper()->raw_register_operand(), rhs_op->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, lhs_op->contained_register_wrapper()->raw_register_operand(),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
								}
							}
							else if(lhs_op->contained_register_wrapper()->raw_register_operand()->register_number() <= 31 && rhs_op->raw_register_operand()->register_number() <= 31)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, lhs_op->contained_register_wrapper()->raw_register_operand(), rhs_op->raw_register_operand(),
									make_shared<immediate_operand>(0)));
							else if (lhs_op->contained_register_wrapper()->raw_register_operand()->register_number() > 31 && rhs_op->raw_register_operand()->register_number() <= 31) {
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rhs_op->raw_register_operand(), lhs_op->contained_register_wrapper()->raw_register_operand()));
								if (lhs_op->contained_register_wrapper()->double_precision())
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, lhs_op->contained_register_wrapper()->raw_register_operand(),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
								else
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, lhs_op->contained_register_wrapper()->raw_register_operand(),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
							}
							else if(lhs_op->contained_register_wrapper()->raw_register_operand()->register_number() <= 31 && rhs_op->raw_register_operand()->register_number() > 31) {
								if (rhs_op->double_precision())
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_D, rhs_op->raw_register_operand(), rhs_op->raw_register_operand()));
								else
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_S, rhs_op->raw_register_operand(), rhs_op->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MFC1, lhs_op->contained_register_wrapper()->raw_register_operand(), rhs_op->raw_register_operand()));
								if (rhs_op->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
									if (rhs_op->double_precision())
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, rhs_op->raw_register_operand(), rhs_op->raw_register_operand()));
									else
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, rhs_op->raw_register_operand(), rhs_op->raw_register_operand()));
								}
							}
						}
						else {
							if (ae_pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
								if (rhs_op->raw_register_operand()->register_number() > 31) {
									if (rhs_op->double_precision()) {
										shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_D, frw->raw_register_operand(), rhs_op->raw_register_operand()));
										mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, frw->raw_register_operand(), make_shared<immediate_operand>(0),
											lhs_op->contained_register_wrapper()->raw_register_operand()));
										free_general_purpose_register(mc, frw);
									}
									else
										mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, rhs_op->raw_register_operand(), make_shared<immediate_operand>(0),
											lhs_op->contained_register_wrapper()->raw_register_operand()));
								}
								else {
									shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rhs_op->raw_register_operand(), frw->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, frw->raw_register_operand(), frw->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, frw->raw_register_operand(), make_shared<immediate_operand>(0),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
									free_general_purpose_register(mc, frw);
								}
							}
							else if (ae_pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
								if (rhs_op->raw_register_operand()->register_number() > 31) {
									if (rhs_op->double_precision())
										mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, rhs_op->raw_register_operand(), make_shared<immediate_operand>(0),
											lhs_op->contained_register_wrapper()->raw_register_operand()));
									else {
										shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, frw->raw_register_operand(), rhs_op->raw_register_operand()));
										mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, frw->raw_register_operand(), make_shared<immediate_operand>(0),
											lhs_op->contained_register_wrapper()->raw_register_operand()));
										free_general_purpose_register(mc, frw);
									}
								}
								else {
									shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rhs_op->raw_register_operand(), frw->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, frw->raw_register_operand(), frw->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, frw->raw_register_operand(), make_shared<immediate_operand>(0),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
									free_general_purpose_register(mc, frw);
								}
							}
							else {
								if (rhs_op->raw_register_operand()->register_number() > 31) {
									shared_ptr<register_wrapper> temp = allocate_general_purpose_register(mc);
									if(rhs_op->double_precision())
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, rhs_op->raw_register_operand(), rhs_op->raw_register_operand()));
									else
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_S, rhs_op->raw_register_operand(), rhs_op->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MFC1, temp->raw_register_operand(), rhs_op->raw_register_operand()));
									if (rhs_op->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
										if(rhs_op->double_precision())
											mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, rhs_op->raw_register_operand(), rhs_op->raw_register_operand()));
										else
											mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, rhs_op->raw_register_operand(), rhs_op->raw_register_operand()));
									}
									else
										free_general_purpose_register(mc, rhs_op);
									rhs_op = temp;
								}
								if (sz == 4)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, rhs_op->raw_register_operand(), make_shared<immediate_operand>(0),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
								else if (sz == 2)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SH, rhs_op->raw_register_operand(), make_shared<immediate_operand>(0),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
								else if (sz == 1)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SB, rhs_op->raw_register_operand(), make_shared<immediate_operand>(0),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
							}
						}
					}
					else {
						if (lhs->unary_expression_type()->type_kind() != type::kind::KIND_PRIMITIVE)
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						shared_ptr<primitive_type> lhs_pt = static_pointer_cast<primitive_type>(lhs->unary_expression_type());
						shared_ptr<register_wrapper> rw = nullptr;
						if (lhs_op->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
							if (lhs_op->contained_register_wrapper()->raw_register_operand()->register_number() <= 31) {
								rw = allocate_general_purpose_register(mc);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, rw->raw_register_operand(), register_file::register_objects::_zero_register,
									lhs_op->contained_register_wrapper()->raw_register_operand()));
							}
							else {
								rw = allocate_general_purpose_fp_register(mc);
								if(lhs_op->contained_register_wrapper()->double_precision())
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, rw->raw_register_operand(), lhs_op->contained_register_wrapper()->raw_register_operand()));
								else
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, rw->raw_register_operand(), lhs_op->contained_register_wrapper()->raw_register_operand()));
								rw->set_double_precision(lhs_op->contained_register_wrapper()->double_precision());
							}
						}
						else {
							if (lhs_pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
								rw = allocate_general_purpose_fp_register(mc);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_LWC1, rw->raw_register_operand(), make_shared<immediate_operand>(0),
									lhs_op->contained_register_wrapper()->raw_register_operand()));
							}
							else if (lhs_pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
								rw = allocate_general_purpose_fp_register(mc);
								rw->set_double_precision(true);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_LDC1, rw->raw_register_operand(), make_shared<immediate_operand>(0),
									lhs_op->contained_register_wrapper()->raw_register_operand()));
							}
							else {
								bool u = lhs_pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED;
								rw = allocate_general_purpose_register(mc);
								if (sz == 4)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, rw->raw_register_operand(), make_shared<immediate_operand>(0),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
								else if (sz == 2)
									mc->add_insn(make_shared<insn>(u ? insn::kind::KIND_LHU : insn::kind::KIND_LH, rw->raw_register_operand(), make_shared<immediate_operand>(0),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
								else if (sz == 1)
									mc->add_insn(make_shared<insn>(u ? insn::kind::KIND_LBU : insn::kind::KIND_LB, rw->raw_register_operand(), make_shared<immediate_operand>(0),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
							}
						}
						shared_ptr<operand_wrapper> ret = handle_primitive_assignment_operation(mc, make_shared<operand_wrapper>(rw), ae->assignment_operator_kind(),
							make_shared<operand_wrapper>(rhs_op));
						free_general_purpose_register(mc, rw);
						shared_ptr<register_wrapper> rw2 = nullptr;
						if (ret->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
							rw2 = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw2->raw_register_operand(), register_file::register_objects::_zero_register,
								ret->contained_immediate_operand()));
						}
						else if (ret->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
							rw2 = ret->contained_register_wrapper();
						else
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						if (lhs_op->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
							if (lhs_op->contained_register_wrapper()->raw_register_operand()->register_number() <= 31) {
								if(rw2->raw_register_operand()->register_number() <= 31)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, lhs_op->contained_register_wrapper()->raw_register_operand(),
										register_file::register_objects::_zero_register, rw2->raw_register_operand()));
								else {
									shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
									if (rw2->double_precision())
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_D, frw->raw_register_operand(), rw2->raw_register_operand()));
									else
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_S, frw->raw_register_operand(), rw2->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MFC1, lhs_op->contained_register_wrapper()->raw_register_operand(), frw->raw_register_operand()));
									free_general_purpose_register(mc, frw);
								}
							}
							else {
								if (rw2->raw_register_operand()->register_number() <= 31) {
									shared_ptr<register_wrapper> reg = allocate_general_purpose_fp_register(mc);
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rw2->raw_register_operand(), reg->raw_register_operand()));
									if (lhs_op->contained_register_wrapper()->double_precision()) {
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, reg->raw_register_operand(), reg->raw_register_operand()));
										mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, lhs_op->contained_register_wrapper()->raw_register_operand(), reg->raw_register_operand()));
									}
									else {
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, reg->raw_register_operand(), reg->raw_register_operand()));
										mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, lhs_op->contained_register_wrapper()->raw_register_operand(), reg->raw_register_operand()));
									}
									free_general_purpose_register(mc, reg);
								}
								else {
									if (lhs_op->contained_register_wrapper()->double_precision() && rw2->double_precision())
										mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, lhs_op->contained_register_wrapper()->raw_register_operand(), rw2->raw_register_operand()));
									else if (!lhs_op->contained_register_wrapper()->double_precision() && !rw2->double_precision())
										mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, lhs_op->contained_register_wrapper()->raw_register_operand(), rw2->raw_register_operand()));
									else if (lhs_op->contained_register_wrapper()->double_precision() && !rw2->double_precision())
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, lhs_op->contained_register_wrapper()->raw_register_operand(), rw2->raw_register_operand()));
									else
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_D, lhs_op->contained_register_wrapper()->raw_register_operand(), rw2->raw_register_operand()));
								}
							}
						}
						else {
							if (ae_pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
								if (rw2->raw_register_operand()->register_number() > 31) {
									if (rw2->double_precision()) {
										shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_D, frw->raw_register_operand(), rw2->raw_register_operand()));
										mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, frw->raw_register_operand(), make_shared<immediate_operand>(0),
											lhs_op->contained_register_wrapper()->raw_register_operand()));
										free_general_purpose_register(mc, frw);
									}
									else
										mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, rw2->raw_register_operand(), make_shared<immediate_operand>(0),
											lhs_op->contained_register_wrapper()->raw_register_operand()));
								}
								else {
									shared_ptr<register_wrapper> temp = allocate_general_purpose_fp_register(mc);
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rw2->raw_register_operand(), temp->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, temp->raw_register_operand(), temp->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, temp->raw_register_operand(), make_shared<immediate_operand>(0),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
									if(rw2->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
										free_general_purpose_register(mc, rw2);
									rw2 = temp;
								}
							}
							else if (ae_pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
								if (rw2->raw_register_operand()->register_number() > 31) {
									if (!rw2->double_precision()) {
										shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, frw->raw_register_operand(), rw2->raw_register_operand()));
										mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, frw->raw_register_operand(), make_shared<immediate_operand>(0),
											lhs_op->contained_register_wrapper()->raw_register_operand()));
										free_general_purpose_register(mc, frw);
									}
									else
										mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, rw2->raw_register_operand(), make_shared<immediate_operand>(0),
											lhs_op->contained_register_wrapper()->raw_register_operand()));
								}
								else {
									shared_ptr<register_wrapper> temp = allocate_general_purpose_fp_register(mc);
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rw2->raw_register_operand(), temp->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, temp->raw_register_operand(), temp->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, temp->raw_register_operand(), make_shared<immediate_operand>(0),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
									if(rw2->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
										free_general_purpose_register(mc, rw2);
									temp->set_double_precision(true);
									rw2 = temp;
								}
							}
							else {
								if (rw2->raw_register_operand()->register_number() > 31) {
									shared_ptr<register_wrapper> temp = allocate_general_purpose_register(mc);
									if (rw2->double_precision())
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_D, rw2->raw_register_operand(), rw2->raw_register_operand()));
									else
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_S, rw2->raw_register_operand(), rw2->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MFC1, temp->raw_register_operand(), rw2->raw_register_operand()));
									if (rw2->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
										if (rw2->double_precision())
											mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, rw2->raw_register_operand(), rw2->raw_register_operand()));
										else
											mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, rw2->raw_register_operand(), rw2->raw_register_operand()));
									}
									else
										free_general_purpose_register(mc, rw2);
									rw2 = temp;
								}
								if (sz == 4)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, rw2->raw_register_operand(), make_shared<immediate_operand>(0),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
								else if (sz == 2)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SH, rw2->raw_register_operand(), make_shared<immediate_operand>(0),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
								else if (sz == 1)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SB, rw2->raw_register_operand(), make_shared<immediate_operand>(0),
										lhs_op->contained_register_wrapper()->raw_register_operand()));
							}
						}
						free_general_purpose_register(mc, rw2);
					}
					free_general_purpose_register(mc, rhs_op);
				}
				else if (ae->assignment_expression_type()->type_kind() == type::kind::KIND_STRUCT && ae->assignment_expression_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
					int sz = (int)calculate_type_size(mc, ae->assignment_expression_type());
					if (ae->assignment_operator_kind() != binary_expression::operator_kind::KIND_EQUALS)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<register_wrapper> temp = allocate_general_purpose_register(mc);
					for (int curr = 0; curr < sz; curr += 4) {
						mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, temp->raw_register_operand(), make_shared<immediate_operand>(curr), rhs_op->raw_register_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, temp->raw_register_operand(), make_shared<immediate_operand>(curr),
							lhs_op->contained_register_wrapper()->raw_register_operand()));
					}
					free_general_purpose_register(mc, temp);
					free_general_purpose_register(mc, rhs_op);
				}
				else {
					if (ae->assignment_expression_type()->type_array_kind() != type::array_kind::KIND_ARRAY)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (lhs_op->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, lhs_op->contained_register_wrapper()->raw_register_operand(), rhs_op->raw_register_operand(),
							make_shared<immediate_operand>(0)));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, rhs_op->raw_register_operand(), make_shared<immediate_operand>(0),
							lhs_op->contained_register_wrapper()->raw_register_operand()));
					free_general_purpose_register(mc, rhs_op);
				}
				if (assign_op == binary_expression::operator_kind::KIND_NONE && ptl.size() == 0 && (utl.size() == 0 || (utl.size() != 0 && 
					utl[0] != unary_expression::kind::KIND_INCREMENT && utl[0] != unary_expression::kind::KIND_DECREMENT))) {
					if (ae->assignment_expression_value_kind() == value_kind::VALUE_LVALUE) {
						if (lhs_op->wrapper_operand_kind() != operand_wrapper::operand_kind::KIND_REGISTER)
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						shared_ptr<register_wrapper> to_use = lhs_op->contained_register_wrapper();
						if (ae->assignment_expression_type()->type_kind() == type::kind::KIND_PRIMITIVE &&
							to_use->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT &&
								ae->assignment_expression_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY && !to_use->already_loaded()) {
							shared_ptr<primitive_type> ae_pt = static_pointer_cast<primitive_type>(ae->assignment_expression_type());
							int sz = (int)calculate_type_size(mc, ae->assignment_expression_type());
							shared_ptr<register_wrapper> temp = nullptr;
							if (ae_pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
								temp = allocate_general_purpose_fp_register(mc);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_LWC1, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
							}
							else if (ae_pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
								temp = allocate_general_purpose_fp_register(mc);
								temp->set_double_precision(true);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_LDC1, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
							}
							else {
								bool u = ae_pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED;
								temp = allocate_general_purpose_register(mc);
								if (sz == 4)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
								else if (sz == 2)
									mc->add_insn(make_shared<insn>(u ? insn::kind::KIND_LHU : insn::kind::KIND_LH, temp->raw_register_operand(), make_shared<immediate_operand>(0),
										to_use->raw_register_operand()));
								else if (sz == 1)
									mc->add_insn(make_shared<insn>(u ? insn::kind::KIND_LBU : insn::kind::KIND_LB, temp->raw_register_operand(), make_shared<immediate_operand>(0),
										to_use->raw_register_operand()));
							}
							free_general_purpose_register(mc, to_use);
							to_use = temp;
							to_use->set_already_loaded(true);
							return make_shared<operand_wrapper>(to_use);
						}
						else if (to_use->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT &&
							ae->assignment_expression_type()->type_array_kind() == type::array_kind::KIND_ARRAY && !to_use->already_loaded()) {
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, to_use->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
							to_use->set_already_loaded(true);
							return make_shared<operand_wrapper>(to_use);
						}
					}
				}
				return lhs_op;
			}
				break;
			case assignment_expression::kind::KIND_TERNARY:
				return generate_ternary_expression_mips(mc, ae->conditional_expression(), assign_op, assign_to, utl, ptl);
				break;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand_wrapper> generate_ternary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<ternary_expression> te, binary_expression::operator_kind assign_op,
			shared_ptr<operand_wrapper> assign_to, vector<unary_expression::kind> utl, vector<shared_ptr<postfix_expression::postfix_type>> ptl) {
			if (te == nullptr || !te->valid())
				return nullptr;
			switch (te->ternary_expression_kind()) {
			case ternary_expression::kind::KIND_TERNARY: {
				shared_ptr<register_wrapper> res = nullptr;
				bool more_to_do = false;
				if (assign_op == binary_expression::operator_kind::KIND_NONE && ptl.size() == 0 && (utl.size() == 0 || (utl.size() != 0 && utl[0] != unary_expression::kind::KIND_INCREMENT &&
					utl[0] != unary_expression::kind::KIND_DECREMENT))) {
					if (te->ternary_expression_type()->type_kind() == type::kind::KIND_PRIMITIVE && te->ternary_expression_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY &&
						(static_pointer_cast<primitive_type>(te->ternary_expression_type())->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE ||
						(static_pointer_cast<primitive_type>(te->ternary_expression_type())->primitive_type_kind() == primitive_type::kind::KIND_FLOAT))) {
						res = allocate_general_purpose_fp_register(mc);
						res->set_double_precision(static_pointer_cast<primitive_type>(te->ternary_expression_type())->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE);
					}
					else
						res = allocate_general_purpose_register(mc);
					res->set_already_loaded(true);
				}
				else {
					res = allocate_general_purpose_register(mc);
					more_to_do = true;
				}
				shared_ptr<binary_expression> cexpr = te->condition();
				shared_ptr<expression> texpr = te->true_path();
				shared_ptr<ternary_expression> fexpr = te->false_path();
				shared_ptr<operand_wrapper> cond = generate_binary_expression_mips(mc, cexpr, binary_expression::operator_kind::KIND_NONE, nullptr, vector<unary_expression::kind>{},
					vector<shared_ptr<postfix_expression::postfix_type>>{});
				shared_ptr<register_wrapper> rw = nullptr;
				tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> lab_false_path = mc->next_label_info(), lab_done = mc->next_label_info();
				if (cond->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					rw = allocate_general_purpose_register(mc);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, cond->contained_immediate_operand()));
				}
				else if (cond->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
					rw = cond->contained_register_wrapper();
				mc->add_insn(make_shared<insn>(insn::kind::KIND_BEQ, rw->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_false_path)));
				shared_ptr<operand_wrapper> tp = generate_expression_mips(mc, texpr, assign_op, assign_to, utl, ptl);
				if (tp->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					if (res->raw_register_operand()->register_number() > 31) {
						shared_ptr<register_wrapper> reg = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, reg->raw_register_operand(), register_file::register_objects::_zero_register, tp->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, reg->raw_register_operand(), res->raw_register_operand()));
						if (res->double_precision())
							mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, res->raw_register_operand(), res->raw_register_operand()));
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, res->raw_register_operand(), res->raw_register_operand()));
						free_general_purpose_register(mc, reg);
					}
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, res->raw_register_operand(), register_file::register_objects::_zero_register, tp->contained_immediate_operand()));
				}
				else if (tp->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
					if(!res->already_loaded())
						res->set_already_loaded(tp->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT &&
							more_to_do);
					if (res->raw_register_operand()->register_number() > 31) {
						if (tp->contained_register_wrapper()->raw_register_operand()->register_number() <= 31) {
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, tp->contained_register_wrapper()->raw_register_operand(), res->raw_register_operand()));
							if (res->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, res->raw_register_operand(), res->raw_register_operand()));
							else
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, res->raw_register_operand(), res->raw_register_operand()));
						}
						else {
							if (res->double_precision() && tp->contained_register_wrapper()->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, res->raw_register_operand(), tp->contained_register_wrapper()->raw_register_operand()));
							else if (!res->double_precision() && !tp->contained_register_wrapper()->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, res->raw_register_operand(), tp->contained_register_wrapper()->raw_register_operand()));
							else if (!res->double_precision() && tp->contained_register_wrapper()->raw_register_operand())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_D, res->raw_register_operand(), tp->contained_register_wrapper()->raw_register_operand()));
							else
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, res->raw_register_operand(), tp->contained_register_wrapper()->raw_register_operand()));
						}
						if (tp->contained_register_wrapper()->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
							free_general_purpose_register(mc, tp->contained_register_wrapper());
					}
					else 
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, res->raw_register_operand(), register_file::register_objects::_zero_register,
							tp->contained_register_wrapper()->raw_register_operand()));
					if (tp->contained_register_wrapper()->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
						free_general_purpose_register(mc, tp->contained_register_wrapper());
				}
				mc->add_insn(make_shared<insn>(insn::kind::KIND_J, get<2>(lab_done)));
				mc->add_insn(get<3>(lab_false_path));
				shared_ptr<operand_wrapper> fp = generate_ternary_expression_mips(mc, fexpr, assign_op, assign_to, utl, ptl);
				if (fp->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					if (res->raw_register_operand()->register_number() > 31) {
						shared_ptr<register_wrapper> reg = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, reg->raw_register_operand(), register_file::register_objects::_zero_register, fp->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, reg->raw_register_operand(), res->raw_register_operand()));
						if (res->double_precision())
							mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, res->raw_register_operand(), res->raw_register_operand()));
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, res->raw_register_operand(), res->raw_register_operand()));
						free_general_purpose_register(mc, reg);
					}
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, res->raw_register_operand(), register_file::register_objects::_zero_register, fp->contained_immediate_operand()));
				}
				else if (fp->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
					if(!res->already_loaded())
						res->set_already_loaded(tp->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT &&
							more_to_do);
					if (res->raw_register_operand()->register_number() > 31) {
						if (fp->contained_register_wrapper()->raw_register_operand()->register_number() <= 31) {
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, fp->contained_register_wrapper()->raw_register_operand(), res->raw_register_operand()));
							if (res->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, res->raw_register_operand(), res->raw_register_operand()));
							else
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, res->raw_register_operand(), res->raw_register_operand()));
						}
						else {
							if (res->double_precision() && fp->contained_register_wrapper()->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, res->raw_register_operand(), fp->contained_register_wrapper()->raw_register_operand()));
							else if (!res->double_precision() && !fp->contained_register_wrapper()->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, res->raw_register_operand(), fp->contained_register_wrapper()->raw_register_operand()));
							else if (!res->double_precision() && fp->contained_register_wrapper()->raw_register_operand())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_D, res->raw_register_operand(), fp->contained_register_wrapper()->raw_register_operand()));
							else
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, res->raw_register_operand(), fp->contained_register_wrapper()->raw_register_operand()));
						}
					}
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, res->raw_register_operand(), register_file::register_objects::_zero_register,
							fp->contained_register_wrapper()->raw_register_operand()));
					if (fp->contained_register_wrapper()->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
						free_general_purpose_register(mc, fp->contained_register_wrapper());
				}
				free_general_purpose_register(mc, rw);
				mc->add_insn(get<3>(lab_done));
				return make_shared<operand_wrapper>(res);
			}
				break;
			case ternary_expression::kind::KIND_BINARY:
				return generate_binary_expression_mips(mc, te->condition(), assign_op, assign_to, utl, ptl);
				break;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand_wrapper> generate_binary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<binary_expression> be, binary_expression::operator_kind assign_op,
			shared_ptr<operand_wrapper> assign_to, vector<unary_expression::kind> utl, vector<shared_ptr<postfix_expression::postfix_type>> ptl) {
			if (be == nullptr || !be->valid())
				return nullptr;
			switch (be->binary_expression_kind()) {
			case binary_expression::kind::KIND_BINARY_EXPRESSION: {
				if (assign_to != nullptr)
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				switch (be->binary_expression_operator_kind()) {
				case binary_expression::operator_kind::KIND_ADD:
				case binary_expression::operator_kind::KIND_SUBTRACT:
				case binary_expression::operator_kind::KIND_MULTIPLY:
				case binary_expression::operator_kind::KIND_DIVIDE:
				case binary_expression::operator_kind::KIND_MODULUS:
				case binary_expression::operator_kind::KIND_SHIFT_LEFT:
				case binary_expression::operator_kind::KIND_SHIFT_RIGHT:
				case binary_expression::operator_kind::KIND_BITWISE_XOR:
				case binary_expression::operator_kind::KIND_BITWISE_AND:
				case binary_expression::operator_kind::KIND_BITWISE_OR: {
					shared_ptr<operand_wrapper> lhs_op = generate_binary_expression_mips(mc, be->lhs(), binary_expression::operator_kind::KIND_NONE, nullptr,
						vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{}),
						rhs_op = generate_binary_expression_mips(mc, be->rhs(), binary_expression::operator_kind::KIND_NONE, nullptr, vector<unary_expression::kind>{},
							vector<shared_ptr<postfix_expression::postfix_type>>{});
					if (lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER &&
						lhs_op->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
						shared_ptr<register_wrapper> to_use_lhs = nullptr;
						if (lhs_op->contained_register_wrapper()->raw_register_operand()->register_number() <= 31) {
							to_use_lhs = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, to_use_lhs->raw_register_operand(), register_file::register_objects::_zero_register,
								lhs_op->contained_register_wrapper()->raw_register_operand()));
						}
						else {
							to_use_lhs = allocate_general_purpose_fp_register(mc);
							if(lhs_op->contained_register_wrapper()->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, to_use_lhs->raw_register_operand(), lhs_op->contained_register_wrapper()->raw_register_operand()));
							else
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, to_use_lhs->raw_register_operand(), lhs_op->contained_register_wrapper()->raw_register_operand()));
							to_use_lhs->set_double_precision(lhs_op->contained_register_wrapper()->double_precision());
						}
						lhs_op = make_shared<operand_wrapper>(to_use_lhs);
					}
					if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER &&
						rhs_op->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
						shared_ptr<register_wrapper> to_use_rhs = nullptr;
						if (rhs_op->contained_register_wrapper()->raw_register_operand()->register_number() <= 31) {
							to_use_rhs = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, to_use_rhs->raw_register_operand(), register_file::register_objects::_zero_register,
								rhs_op->contained_register_wrapper()->raw_register_operand()));
						}
						else {
							to_use_rhs = allocate_general_purpose_fp_register(mc);
							if(rhs_op->contained_register_wrapper()->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, to_use_rhs->raw_register_operand(), rhs_op->contained_register_wrapper()->raw_register_operand()));
							else
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, to_use_rhs->raw_register_operand(), rhs_op->contained_register_wrapper()->raw_register_operand()));
							to_use_rhs->set_double_precision(rhs_op->contained_register_wrapper()->double_precision());
						}
						rhs_op = make_shared<operand_wrapper>(to_use_rhs);
					}
					shared_ptr<operand_wrapper> ret = binary_expression_helper_1(mc, lhs_op, be->binary_expression_operator_kind(), rhs_op);
					return ret;
				}
					break;
				case binary_expression::operator_kind::KIND_EQUALS_EQUALS:
				case binary_expression::operator_kind::KIND_NOT_EQUALS:
				case binary_expression::operator_kind::KIND_LESS_THAN:
				case binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO:
				case binary_expression::operator_kind::KIND_GREATER_THAN:
				case binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO: {
					shared_ptr<operand_wrapper> lhs_op = generate_binary_expression_mips(mc, be->lhs(), binary_expression::operator_kind::KIND_NONE, nullptr,
						vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{}),
						rhs_op = generate_binary_expression_mips(mc, be->rhs(), binary_expression::operator_kind::KIND_NONE, nullptr, vector<unary_expression::kind>{},
							vector<shared_ptr<postfix_expression::postfix_type>>{});
					if (lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER &&
						lhs_op->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
						shared_ptr<register_wrapper> to_use_lhs = nullptr;
						if (lhs_op->contained_register_wrapper()->raw_register_operand()->register_number() <= 31) {
							to_use_lhs = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, to_use_lhs->raw_register_operand(), register_file::register_objects::_zero_register,
								lhs_op->contained_register_wrapper()->raw_register_operand()));
						}
						else {
							to_use_lhs = allocate_general_purpose_fp_register(mc);
							if(lhs_op->contained_register_wrapper()->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, to_use_lhs->raw_register_operand(), lhs_op->contained_register_wrapper()->raw_register_operand()));
							else
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, to_use_lhs->raw_register_operand(), lhs_op->contained_register_wrapper()->raw_register_operand()));
							to_use_lhs->set_double_precision(lhs_op->contained_register_wrapper()->double_precision());
						}
						lhs_op = make_shared<operand_wrapper>(to_use_lhs);
					}
					if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER &&
						rhs_op->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
						shared_ptr<register_wrapper> to_use_rhs = nullptr;
						if (rhs_op->contained_register_wrapper()->raw_register_operand()->register_number() <= 31) {
							to_use_rhs = allocate_general_purpose_register(mc); 
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, to_use_rhs->raw_register_operand(), register_file::register_objects::_zero_register,
								rhs_op->contained_register_wrapper()->raw_register_operand()));
						}
						else {
							to_use_rhs = allocate_general_purpose_fp_register(mc);
							if(rhs_op->contained_register_wrapper()->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, to_use_rhs->raw_register_operand(), rhs_op->contained_register_wrapper()->raw_register_operand()));
							else
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, to_use_rhs->raw_register_operand(), rhs_op->contained_register_wrapper()->raw_register_operand()));
							to_use_rhs->set_double_precision(rhs_op->contained_register_wrapper()->double_precision());
						}
						rhs_op = make_shared<operand_wrapper>(to_use_rhs);
					}
					if (be->lhs()->binary_expression_type()->type_kind() != type::kind::KIND_PRIMITIVE || be->rhs()->binary_expression_type()->type_kind() != type::kind::KIND_PRIMITIVE ||
						be->lhs()->binary_expression_type()->array_dimensions() > 0 || be->rhs()->binary_expression_type()->array_dimensions() > 0 ||
						be->lhs()->binary_expression_type()->type_array_kind() != type::array_kind::KIND_NON_ARRAY ||
						be->rhs()->binary_expression_type()->type_array_kind() != type::array_kind::KIND_NON_ARRAY)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					bool u = static_pointer_cast<primitive_type>(be->lhs()->binary_expression_type())->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED ||
						static_pointer_cast<primitive_type>(be->rhs()->binary_expression_type())->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED;
					shared_ptr<operand_wrapper> ret = binary_expression_helper_2(mc, lhs_op, be->binary_expression_operator_kind(), rhs_op, u);
					if (ret->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
						ret->contained_register_wrapper()->set_register_wrapper_function_argument_kind(register_wrapper::function_argument_kind::KIND_NON_FUNCTION_ARGUMENT);
					return ret;
				}
					break;
				case binary_expression::operator_kind::KIND_LOGICAL_AND:
				case binary_expression::operator_kind::KIND_LOGICAL_OR:
					shared_ptr<operand_wrapper> ret = binary_expression_helper_3(mc, be->lhs(), be->binary_expression_operator_kind(), be->rhs());
					if (ret->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
						ret->contained_register_wrapper()->set_register_wrapper_function_argument_kind(register_wrapper::function_argument_kind::KIND_NON_FUNCTION_ARGUMENT);
					return ret;
				}
					break;
			}
				break;
			case binary_expression::kind::KIND_UNARY_EXPRESSION:
				return generate_unary_expression_mips(mc, be->single_lhs(), assign_op, assign_to, utl, ptl);
				break;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand_wrapper> generate_unary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<unary_expression> ue, binary_expression::operator_kind assign_op, 
			shared_ptr<operand_wrapper> assign_to, vector<unary_expression::kind> utl, vector<shared_ptr<postfix_expression::postfix_type>> ptl) {
			if (ue == nullptr || !ue->valid())
				return nullptr;
			shared_ptr<operand_wrapper> op = generate_postfix_expression_mips(mc, ue->contained_postfix_expression(), assign_op, assign_to, ue->unary_expression_kind_list(),
				vector<shared_ptr<postfix_expression::postfix_type>>{});
			shared_ptr<register_wrapper> to_use = nullptr;
			if (ue->unary_expression_kind_list().size() > 0) {
				if (op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					to_use = allocate_general_purpose_register(mc);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, to_use->raw_register_operand(), register_file::register_objects::_zero_register, op->contained_immediate_operand()));
				}
				else if (op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
					to_use = op->contained_register_wrapper();
			}
			else return op;
			for(int i = 0; i < ue->unary_expression_kind_list().size(); i++) {
				unary_expression::kind uek = ue->unary_expression_kind_list()[i];
				if (uek != unary_expression::kind::KIND_INCREMENT && uek != unary_expression::kind::KIND_DECREMENT) {
					if (to_use->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
						if (ue->unary_expression_type()->type_kind() == type::kind::KIND_PRIMITIVE &&
							static_pointer_cast<primitive_type>(ue->unary_expression_type())->primitive_type_kind() == primitive_type::kind::KIND_FLOAT &&
							ue->unary_expression_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
							shared_ptr<register_wrapper> temp = allocate_general_purpose_fp_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, temp->raw_register_operand(), to_use->raw_register_operand()));
							to_use = temp;
						}
						else if (ue->unary_expression_type()->type_kind() == type::kind::KIND_PRIMITIVE &&
							static_pointer_cast<primitive_type>(ue->unary_expression_type())->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE &&
							ue->unary_expression_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
							shared_ptr<register_wrapper> temp = allocate_general_purpose_fp_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, temp->raw_register_operand(), to_use->raw_register_operand()));
							to_use = temp;
						}
						else {
							shared_ptr<register_wrapper> temp = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, temp->raw_register_operand(), to_use->raw_register_operand(), make_shared<immediate_operand>(0)));
							to_use = temp;
						}
					}
				}
				switch (uek) {
				case unary_expression::kind::KIND_PLUS:
					to_use->set_register_wrapper_function_argument_kind(register_wrapper::function_argument_kind::KIND_NON_FUNCTION_ARGUMENT);
					break;
				case unary_expression::kind::KIND_MINUS: {
					if (ue->unary_expression_type()->type_kind() != type::kind::KIND_PRIMITIVE)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(ue->unary_expression_type());
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_NEG_S, to_use->raw_register_operand(), to_use->raw_register_operand()));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_NEG_D, to_use->raw_register_operand(), to_use->raw_register_operand()));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_SUBU, to_use->raw_register_operand(), register_file::register_objects::_zero_register, to_use->raw_register_operand()));
					to_use->set_register_wrapper_function_argument_kind(register_wrapper::function_argument_kind::KIND_NON_FUNCTION_ARGUMENT);
				}
					break;
				case unary_expression::kind::KIND_LOGICAL_NOT: {
					tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> lab_false_path = mc->next_label_info(), lab_done = mc->next_label_info();
					mc->add_insn(make_shared<insn>(insn::kind::KIND_BEQ, to_use->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_false_path)));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, to_use->raw_register_operand(), register_file::register_objects::_zero_register,
						register_file::register_objects::_zero_register));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_J, get<2>(lab_done)));
					mc->add_insn(get<3>(lab_false_path));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, to_use->raw_register_operand(), register_file::register_objects::_zero_register, make_shared<immediate_operand>(1)));
					mc->add_insn(get<3>(lab_done));
					to_use->set_register_wrapper_function_argument_kind(register_wrapper::function_argument_kind::KIND_NON_FUNCTION_ARGUMENT);
				}
					break;
				case unary_expression::kind::KIND_BITWISE_NOT:
					mc->add_insn(make_shared<insn>(insn::kind::KIND_XORI, to_use->raw_register_operand(), to_use->raw_register_operand(), make_shared<immediate_operand>(0)));
					to_use->set_register_wrapper_function_argument_kind(register_wrapper::function_argument_kind::KIND_NON_FUNCTION_ARGUMENT);
					break;
				case unary_expression::kind::KIND_DECREMENT:
				case unary_expression::kind::KIND_INCREMENT: {
					int to_add = uek == unary_expression::kind::KIND_INCREMENT ? 1 : -1;
					if (i != 0 || ue->contained_postfix_expression()->postfix_expression_type()->type_kind() != type::kind::KIND_PRIMITIVE ||
						ue->unary_expression_type()->type_kind() != type::kind::KIND_PRIMITIVE ||
						op->wrapper_operand_kind() != operand_wrapper::operand_kind::KIND_REGISTER)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<register_wrapper> rw = op->contained_register_wrapper();
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(ue->unary_expression_type());
					shared_ptr<register_wrapper> temp = nullptr;
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
						temp = allocate_general_purpose_fp_register(mc);
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
						temp = allocate_general_purpose_fp_register(mc);
						temp->set_double_precision(true);
					}
					else
						temp = allocate_general_purpose_register(mc);
					int sz = (int)calculate_type_size(mc, ue->contained_postfix_expression()->postfix_expression_type());
					if (to_use->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, temp->raw_register_operand(), to_use->raw_register_operand()));
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, temp->raw_register_operand(), to_use->raw_register_operand()));
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, temp->raw_register_operand(), to_use->raw_register_operand(), make_shared<immediate_operand>(0)));
					}
					else {
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LWC1, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LDC1, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
						else {
							bool u = pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED;
							if (sz == 4)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
							else if (sz == 2)
								mc->add_insn(make_shared<insn>(u ? insn::kind::KIND_LHU : insn::kind::KIND_LH, temp->raw_register_operand(), make_shared<immediate_operand>(0),
									to_use->raw_register_operand()));
							else if (sz == 1)
								mc->add_insn(make_shared<insn>(u ? insn::kind::KIND_LBU : insn::kind::KIND_LB, temp->raw_register_operand(), make_shared<immediate_operand>(0), 
									to_use->raw_register_operand()));
						}
					}
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
						shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_L_S, frw->raw_register_operand(), make_shared<label_operand>(to_add == 1 ? "__fp_1" : "__fp_m1")));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADD_S, temp->raw_register_operand(), temp->raw_register_operand(), frw->raw_register_operand()));
						free_general_purpose_register(mc, frw);
					}
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
						shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_L_D, frw->raw_register_operand(), make_shared<label_operand>(to_add == 1 ? "__dp_1" : "__dp_m1")));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADD_D, temp->raw_register_operand(), temp->raw_register_operand(), frw->raw_register_operand()));
						free_general_purpose_register(mc, frw);
					}
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, temp->raw_register_operand(), temp->raw_register_operand(), make_shared<immediate_operand>(to_add)));
					if (to_use->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, to_use->raw_register_operand(), temp->raw_register_operand()));
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, to_use->raw_register_operand(), temp->raw_register_operand()));
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, to_use->raw_register_operand(), temp->raw_register_operand(), make_shared<immediate_operand>(0)));
					}
					else {
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
						else {
							if (sz == 4)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
							else if (sz == 2)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SH, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
							else if (sz == 1)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SB, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
						}
					}
					free_general_purpose_register(mc, to_use);
					to_use = temp;
					to_use->set_already_loaded(true);
				}
					break;
				}
			}
			return make_shared<operand_wrapper>(to_use);
		}

		shared_ptr<operand_wrapper> generate_postfix_expression_mips(shared_ptr<mips_code> mc, shared_ptr<postfix_expression> pe, binary_expression::operator_kind assign_op,
			shared_ptr<operand_wrapper> assign_to, vector<unary_expression::kind> utl, vector<shared_ptr<postfix_expression::postfix_type>> ptl) {
			if (pe == nullptr || !pe->valid())
				return nullptr;
			shared_ptr<operand_wrapper> op = generate_primary_expression_mips(mc, pe->contained_primary_expression(), assign_op, assign_to, utl, pe->postfix_type_list());
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
			shared_ptr<type> prev_type = pe->contained_primary_expression()->primary_expression_type();
			for(int i = 0; i < pe->postfix_type_list().size(); i++) {
				shared_ptr<postfix_expression::postfix_type> pt = pe->postfix_type_list()[i];
				switch (pt->postfix_type_kind()) {
				case postfix_expression::kind::KIND_ADDRESS: {
					if (op->wrapper_operand_kind() != operand_wrapper::operand_kind::KIND_REGISTER)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					op->contained_register_wrapper()->set_already_loaded(true);
				}
					break;
				case postfix_expression::kind::KIND_FUNCTION_CALL: {
					vector<int> which_to_store;
					int t0 = register_file::register_2_int.at(register_file::_t0), t1 = register_file::register_2_int.at(register_file::_t1),
						t2 = register_file::register_2_int.at(register_file::_t2), t3 = register_file::register_2_int.at(register_file::_t3),
						t4 = register_file::register_2_int.at(register_file::_t4), t5 = register_file::register_2_int.at(register_file::_t5),
						t6 = register_file::register_2_int.at(register_file::_t6), t7 = register_file::register_2_int.at(register_file::_t7),
						f4 = register_file::register_2_int.at(register_file::_f4), f6 = register_file::register_2_int.at(register_file::_f6),
						f8 = register_file::register_2_int.at(register_file::_f8), f10 = register_file::register_2_int.at(register_file::_f10),
						f12 = register_file::register_2_int.at(register_file::_f12), f14 = register_file::register_2_int.at(register_file::_f14),
						v0 = register_file::register_2_int.at(register_file::_v0), a0 = register_file::register_2_int.at(register_file::_a0),
						a1 = register_file::register_2_int.at(register_file::_a1), a2 = register_file::register_2_int.at(register_file::_a2),
						a3 = register_file::register_2_int.at(register_file::_a3), ra = register_file::register_2_int.at(register_file::_ra);
					int pre_offset = 0;
					for (int r : { t0, t1, t2, t3, t4, t5, t6, t7, v0, ra, a0, a1, a2, a3, f4, f6, f8, f10, f12, f14 })
						if (mc->current_frame()->is_register_marked(r)) {
							if (mc->current_frame()->is_register_marked(r) && (r == f4 || r == f6 || r == f8 || r == f10))
								which_to_store.push_back(r), pre_offset += 8;
							else if (mc->current_frame()->is_register_marked(r))
								which_to_store.push_back(r), pre_offset += 8;
						}
					if (pre_offset != 0) {
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-pre_offset)));
						mc->current_frame()->change_local_stack_offset(-pre_offset);
					}
					for (int i = 0, curr = 0; i < which_to_store.size(); i++) {
						int r = which_to_store[i];
						if (r == f4 || r == f6 || r == f8 || r == f10 || r == f12 || r == f14) {
							mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, make_shared<register_operand>(which_to_store[i]), make_shared<immediate_operand>(curr), sp));
						}
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, make_shared<register_operand>(which_to_store[i]), make_shared<immediate_operand>(curr), sp));
						curr += 8;
					}
					if (op->wrapper_operand_kind() != operand_wrapper::operand_kind::KIND_LABEL || prev_type->type_kind() != type::kind::KIND_FUNCTION)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<function_type> ft = static_pointer_cast<function_type>(prev_type);
					int offset = 0;
					vector<pair<shared_ptr<register_wrapper>, shared_ptr<operand_wrapper>>> arg_list;
					vector<int> fp_reg_args, reg_args;
					for (int i = 0; i < ft->parameter_list().size(); i++) {
						shared_ptr<type> t = ft->parameter_list()[i]->variable_declaration_type();
						if (t->type_kind() == type::kind::KIND_PRIMITIVE && t->type_array_kind() == type::array_kind::KIND_NON_ARRAY &&
							(static_pointer_cast<primitive_type>(t)->primitive_type_kind() == primitive_type::kind::KIND_FLOAT ||
								static_pointer_cast<primitive_type>(t)->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) && fp_reg_args.size() < 2 &&
							ft->parameter_rfuncarg_list()[i])
							fp_reg_args.push_back(i);
						else if (reg_args.size() < 4 && !(t->type_kind() == type::kind::KIND_PRIMITIVE && t->type_array_kind() == type::array_kind::KIND_NON_ARRAY &&
							(static_pointer_cast<primitive_type>(t)->primitive_type_kind() == primitive_type::kind::KIND_FLOAT || 
								static_pointer_cast<primitive_type>(t)->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)) && ft->parameter_rfuncarg_list()[i])
							reg_args.push_back(i);
					}
					if (ft->parameter_list().size() != pt->argument_list().size())
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					vector<int> fp_reg_args_ref = fp_reg_args, reg_args_ref = reg_args;
					int reg_arg_counter = register_file::register_2_int.at(register_file::_a0),
						fp_reg_arg_counter = register_file::register_2_int.at(register_file::_f12);
					for (int i = 0; i < pt->argument_list().size(); i++) {
						shared_ptr<assignment_expression> ae = pt->argument_list()[i];
						shared_ptr<register_wrapper> rw = nullptr;
						shared_ptr<operand_wrapper> ow = generate_assignment_expression_mips(mc, ae, binary_expression::operator_kind::KIND_NONE, nullptr,
							vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
						int sz = calculate_type_size(mc, ae->assignment_expression_type());
						if (reg_args.size() > 0 && reg_args[0] == i) {
							int to_use = reg_arg_counter++;
							reg_args.erase(reg_args.begin(), reg_args.begin() + 1);
							bool in_use = mc->current_frame()->is_register_marked(to_use);
							rw = mc->current_frame()->generate_register_wrapper(make_shared<register_operand>(to_use));
							if (!in_use) mc->current_frame()->unmark_register(to_use);
							if (ae->assignment_expression_type()->type_kind() == type::kind::KIND_STRUCT &&
								ae->assignment_expression_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
								rw->set_offset_from_stack_ptr(offset);
								offset += sz;
							}
						}
						else if (fp_reg_args.size() > 0 && fp_reg_args[0] == i) {
							int to_use = fp_reg_arg_counter;
							fp_reg_arg_counter += 2;
							fp_reg_args.erase(fp_reg_args.begin(), fp_reg_args.begin() + 1);
							bool in_use = mc->current_frame()->is_register_marked(to_use);
							rw = mc->current_frame()->generate_register_wrapper(make_shared<register_operand>(to_use));
							if (ft->parameter_list()[i]->variable_declaration_type()->type_kind() != type::kind::KIND_PRIMITIVE &&
								ft->parameter_list()[i]->variable_declaration_type()->type_array_kind() != type::array_kind::KIND_NON_ARRAY)
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(ft->parameter_list()[i]->variable_declaration_type());
							if (pt->primitive_type_kind() != primitive_type::kind::KIND_DOUBLE && pt->primitive_type_kind() != primitive_type::kind::KIND_FLOAT)
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							rw->set_double_precision(pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE);
							if (!in_use) mc->current_frame()->unmark_register(to_use);
						}
						else {
							int actual_size = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
							rw = allocate_general_purpose_register(mc);
							mc->current_frame()->unmark_register(rw->raw_register_operand()->register_number());
							rw->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_ON_STACK);
							rw->set_register_wrapper_variable_kind(register_wrapper::variable_kind::KIND_VARIABLE);
							rw->set_offset_from_stack_ptr(offset);
							offset += actual_size;
						}
						arg_list.push_back(make_pair(rw, ow));
					}
					if(offset != 0)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-offset)));
					for (int i = 0; i < arg_list.size(); i++) {
						pair<shared_ptr<register_wrapper>, shared_ptr<operand_wrapper>> p = arg_list[i];
						int sz = (int)calculate_type_size(mc, pt->argument_list()[i]->assignment_expression_type());
						if (find(reg_args_ref.begin(), reg_args_ref.end(), i) != reg_args_ref.end()) {
							if (pt->argument_list()[i]->assignment_expression_type()->type_kind() == type::kind::KIND_STRUCT &&
								pt->argument_list()[i]->assignment_expression_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
								mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, p.first->raw_register_operand(), sp, make_shared<immediate_operand>(p.first->offset_from_stack_ptr())));
								shared_ptr<register_wrapper> to_copy = nullptr;
								if (p.second->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
									to_copy = allocate_general_purpose_register(mc);
									mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, to_copy->raw_register_operand(), register_file::register_objects::_zero_register,
										p.second->contained_immediate_operand()));
								}
								else if (p.second->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
									to_copy = p.second->contained_register_wrapper();
								else
									mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								shared_ptr<register_wrapper> loop_iterator = allocate_general_purpose_register(mc);
								for (int curr = 0; curr < sz; curr += 4) {
									mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, loop_iterator->raw_register_operand(), make_shared<immediate_operand>(curr),
										to_copy->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, loop_iterator->raw_register_operand(), make_shared<immediate_operand>(curr),
										p.first->raw_register_operand()));
								}
								free_general_purpose_register(mc, loop_iterator);
								if(to_copy->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
									free_general_purpose_register(mc, to_copy);
							}
							else {
								if (p.second->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, p.first->raw_register_operand(), register_file::register_objects::_zero_register,
										p.second->contained_immediate_operand()));
								else if (p.second->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
									if (p.second->contained_register_wrapper()->raw_register_operand()->register_number() <= 31) {
										mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, p.first->raw_register_operand(), register_file::register_objects::_zero_register,
											p.second->contained_register_wrapper()->raw_register_operand()));
									}
									else {
										if (!p.second->contained_register_wrapper()->double_precision()) {
											mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_S, p.second->contained_register_wrapper()->raw_register_operand(),
												p.second->contained_register_wrapper()->raw_register_operand()));
											mc->add_insn(make_shared<insn>(insn::kind::KIND_MFC1, p.first->raw_register_operand(), p.second->contained_register_wrapper()->raw_register_operand()));
											if (p.second->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
												mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, p.second->contained_register_wrapper()->raw_register_operand(),
													p.second->contained_register_wrapper()->raw_register_operand()));
										}
										else {
											mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_D, p.second->contained_register_wrapper()->raw_register_operand(),
												p.second->contained_register_wrapper()->raw_register_operand()));
											mc->add_insn(make_shared<insn>(insn::kind::KIND_MFC1, p.first->raw_register_operand(), p.second->contained_register_wrapper()->raw_register_operand()));
											if (p.second->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
												mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, p.second->contained_register_wrapper()->raw_register_operand(),
													p.second->contained_register_wrapper()->raw_register_operand()));
										}
									}
									if(p.second->contained_register_wrapper()->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
										free_general_purpose_register(mc, p.second->contained_register_wrapper());
								}
								else
									mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}
						}
						else if (find(fp_reg_args_ref.begin(), fp_reg_args_ref.end(), i) != fp_reg_args_ref.end()) {
							shared_ptr<register_wrapper> tmp = nullptr;
							if (p.second->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
								tmp = allocate_general_purpose_register(mc);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, tmp->raw_register_operand(), register_file::register_objects::_zero_register,
									p.second->contained_immediate_operand()));
							}
							else if (p.second->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
								tmp = p.second->contained_register_wrapper();
							else
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__,  __FILE__);
							if (tmp->raw_register_operand()->register_number() <= 31) {
								if(p.first->double_precision()) {
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, tmp->raw_register_operand(), p.first->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, p.first->raw_register_operand(), p.first->raw_register_operand()));
								}
								else {
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, tmp->raw_register_operand(), p.first->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, p.first->raw_register_operand(), p.first->raw_register_operand()));
								}
							}
							else {
								if (p.first->double_precision() && tmp->double_precision())
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, p.first->raw_register_operand(), tmp->raw_register_operand()));
								else if (!p.first->double_precision() && !tmp->double_precision())
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, p.first->raw_register_operand(), tmp->raw_register_operand()));
								else if (p.first->double_precision() && !tmp->double_precision())
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, p.first->raw_register_operand(), tmp->raw_register_operand()));
								else
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_D, p.first->raw_register_operand(), tmp->raw_register_operand()));
							}
							if(tmp->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
								free_general_purpose_register(mc, tmp);
						}
						else {
							shared_ptr<register_wrapper> tmp = nullptr;
							if (p.second->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
								tmp = allocate_general_purpose_register(mc);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, tmp->raw_register_operand(), register_file::register_objects::_zero_register,
									p.second->contained_immediate_operand()));
							}
							else if (p.second->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
								tmp = p.second->contained_register_wrapper();
							else
								mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__,  __FILE__);
							if (pt->argument_list()[i]->assignment_expression_type()->type_kind() == type::kind::KIND_STRUCT &&
								pt->argument_list()[i]->assignment_expression_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
								shared_ptr<register_wrapper> to_copy = tmp;
								shared_ptr<register_wrapper> loop_iterator = allocate_general_purpose_register(mc);
								for (int curr = 0; curr < sz; curr += 4) {
									mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, loop_iterator->raw_register_operand(), make_shared<immediate_operand>(curr),
										to_copy->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, loop_iterator->raw_register_operand(),
										make_shared<immediate_operand>(curr + p.first->offset_from_stack_ptr()), sp));
								}
								free_general_purpose_register(mc, loop_iterator);
							}
							else {
								shared_ptr<type> to_test = ft->parameter_list()[i]->variable_declaration_type();
								if (to_test->type_kind() == type::kind::KIND_PRIMITIVE && static_pointer_cast<primitive_type>(to_test)->primitive_type_kind() == primitive_type::kind::KIND_FLOAT &&
									to_test->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
									if (tmp->raw_register_operand()->register_number() <= 31) {
										shared_ptr<register_wrapper> ntemp = allocate_general_purpose_fp_register(mc);
										mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, tmp->raw_register_operand(), ntemp->raw_register_operand()));
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, ntemp->raw_register_operand(), ntemp->raw_register_operand()));
										mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, ntemp->raw_register_operand(), make_shared<immediate_operand>(p.first->offset_from_stack_ptr()), sp));
										free_general_purpose_register(mc, ntemp);
									}
									else {
										if (tmp->double_precision()) {
											mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_D, tmp->raw_register_operand(), tmp->raw_register_operand()));
											mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, tmp->raw_register_operand(), make_shared<immediate_operand>(p.first->offset_from_stack_ptr()), sp));
											if (tmp->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
												mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, tmp->raw_register_operand(), tmp->raw_register_operand()));
										}
										else
											mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, tmp->raw_register_operand(), make_shared<immediate_operand>(p.first->offset_from_stack_ptr()), sp));
									}
								}
								else if (to_test->type_kind() == type::kind::KIND_PRIMITIVE &&
									static_pointer_cast<primitive_type>(to_test)->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE &&
									to_test->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
									if (tmp->raw_register_operand()->register_number() <= 31) {
										shared_ptr<register_wrapper> ntemp = allocate_general_purpose_fp_register(mc);
										mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, tmp->raw_register_operand(), ntemp->raw_register_operand()));
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, ntemp->raw_register_operand(), ntemp->raw_register_operand()));
										mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, ntemp->raw_register_operand(), make_shared<immediate_operand>(p.first->offset_from_stack_ptr()), sp));
										free_general_purpose_register(mc, ntemp);
									}
									else {
										if (tmp->double_precision())
											mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, tmp->raw_register_operand(), make_shared<immediate_operand>(p.first->offset_from_stack_ptr()),
												sp));
										else {
											mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, tmp->raw_register_operand(), tmp->raw_register_operand()));
											mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, tmp->raw_register_operand(), make_shared<immediate_operand>(p.first->offset_from_stack_ptr()),
												sp));
											if (tmp->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
												mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_D, tmp->raw_register_operand(), tmp->raw_register_operand()));
										}
									}
								}
								else {
									if (tmp->raw_register_operand()->register_number() > 31) {
										if (!tmp->double_precision()) {
											shared_ptr<register_wrapper> ntemp = allocate_general_purpose_register(mc);
											mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_S, tmp->raw_register_operand(), tmp->raw_register_operand()));
											mc->add_insn(make_shared<insn>(insn::kind::KIND_MFC1, ntemp->raw_register_operand(), tmp->raw_register_operand()));
											mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, ntemp->raw_register_operand(), make_shared<immediate_operand>(p.first->offset_from_stack_ptr()), sp));
											if (tmp->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
												mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, tmp->raw_register_operand(), tmp->raw_register_operand()));
											free_general_purpose_register(mc, ntemp);
										}
										else {
											shared_ptr<register_wrapper> ntemp = allocate_general_purpose_register(mc);
											mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_D, tmp->raw_register_operand(), tmp->raw_register_operand()));
											mc->add_insn(make_shared<insn>(insn::kind::KIND_MFC1, ntemp->raw_register_operand(), tmp->raw_register_operand()));
											mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, ntemp->raw_register_operand(), make_shared<immediate_operand>(p.first->offset_from_stack_ptr()), sp));
											if (tmp->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
												mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, tmp->raw_register_operand(), tmp->raw_register_operand()));
											free_general_purpose_register(mc, ntemp);
										}
									}
									else {
										if (sz == 4)
											mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, tmp->raw_register_operand(), make_shared<immediate_operand>(p.first->offset_from_stack_ptr()), sp));
										else if (sz == 2)
											mc->add_insn(make_shared<insn>(insn::kind::KIND_SH, tmp->raw_register_operand(), make_shared<immediate_operand>(p.first->offset_from_stack_ptr()), sp));
										else if (sz == 1)
											mc->add_insn(make_shared<insn>(insn::kind::KIND_SB, tmp->raw_register_operand(), make_shared<immediate_operand>(p.first->offset_from_stack_ptr()), sp));
									}
								}
							}
							if(tmp->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
								free_general_purpose_register(mc, tmp);
						}
						mc->current_frame()->restore_s_registers(mc);
					}
					mc->add_insn(make_shared<insn>(insn::kind::KIND_JAL, op->contained_label_operand()));
					shared_ptr<type> t = pt->postfix_type_type();
					shared_ptr<register_wrapper> rw = nullptr;
					if (t->type_kind() == type::kind::KIND_PRIMITIVE && static_pointer_cast<primitive_type>(t)->primitive_type_kind() == primitive_type::kind::KIND_FLOAT &&
						t->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
						rw = allocate_general_purpose_fp_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, rw->raw_register_operand(), register_file::register_objects::_f0_register));
					}
					else if (t->type_kind() == type::kind::KIND_PRIMITIVE && static_pointer_cast<primitive_type>(t)->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE &&
						t->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
						rw = allocate_general_purpose_fp_register(mc);
						rw->set_double_precision(true);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, rw->raw_register_operand(), register_file::register_objects::_f0_register));
					}
					else {
						rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, rw->raw_register_operand(), register_file::register_objects::_zero_register,
							register_file::register_objects::_v0_register));
					}
					rw->set_already_loaded(true);
					op = make_shared<operand_wrapper>(rw);
					if(offset != 0)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(offset)));
					for (int i = 0, curr = 0; i < which_to_store.size(); i++) {
						if (which_to_store[i] > 31) {
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LDC1, make_shared<register_operand>(which_to_store[i]), make_shared<immediate_operand>(curr), sp));
						}
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, make_shared<register_operand>(which_to_store[i]), make_shared<immediate_operand>(curr), sp));
						curr += 8;
					}
					if (pre_offset != 0) {
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(pre_offset)));
						mc->current_frame()->change_local_stack_offset(pre_offset);
					}
				}
					break;
				case postfix_expression::kind::KIND_ARROW:
				case postfix_expression::kind::KIND_MEMBER: {
					if (prev_type->type_kind() != type::kind::KIND_STRUCT)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<struct_type> st = static_pointer_cast<struct_type>(prev_type);
					shared_ptr<struct_symbol> s_sym = mc->find_struct_symbol(st->struct_name(), st->struct_reference_number());
					vector<shared_ptr<symbol>> s_sym_list = s_sym->struct_symbol_scope()->symbol_list();
					token member_name = pt->member();
					int member_offset = 0;
					bool hit = false;
					for (shared_ptr<symbol> s : s_sym_list) {
						if (s->symbol_kind() == symbol::kind::KIND_VARIABLE) {
							shared_ptr<variable_symbol> vsym = static_pointer_cast<variable_symbol>(s);
							if (vsym->variable_name().raw_text() == member_name.raw_text()) {
								hit = true;
								break;
							}
							int sz_member = calculate_type_size(mc, vsym->variable_type());
							int actual_sz_member = sz_member % 8 == 0 ? sz_member : (sz_member / 8 + 1) * 8;
							member_offset += actual_sz_member;
						}
						else
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					}
					if (!hit)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<register_wrapper> to_use = nullptr;
					bool fn_arg = false;
					if (op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
						to_use = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, to_use->raw_register_operand(), register_file::register_objects::_zero_register,
							op->contained_immediate_operand()));
					}
					else if (op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
						if (op->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
							fn_arg = true;
							to_use = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, to_use->raw_register_operand(), op->contained_register_wrapper()->raw_register_operand(),
								make_shared<immediate_operand>(0)));
						}
						else
							to_use = op->contained_register_wrapper();
					}
					else
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (pt->postfix_type_kind() == postfix_expression::kind::KIND_ARROW && !to_use->already_loaded() && !fn_arg)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, to_use->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, to_use->raw_register_operand(), to_use->raw_register_operand(), make_shared<immediate_operand>(member_offset)));
					to_use->set_register_wrapper_function_argument_kind(register_wrapper::function_argument_kind::KIND_NON_FUNCTION_ARGUMENT);
					if (to_use->already_loaded()) to_use->set_already_loaded(false);
					op = make_shared<operand_wrapper>(to_use);
				}
					break;
				case postfix_expression::kind::KIND_INCREMENT:
				case postfix_expression::kind::KIND_DECREMENT: {
					int to_add = pt->postfix_type_kind() == postfix_expression::kind::KIND_INCREMENT ? 1 : -1;
					if (prev_type->type_kind() != type::kind::KIND_PRIMITIVE || op->wrapper_operand_kind() != operand_wrapper::operand_kind::KIND_REGISTER)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<register_wrapper> to_use = op->contained_register_wrapper();
					int sz = (int)calculate_type_size(mc, prev_type);
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(prev_type);
					shared_ptr<register_wrapper> temp = nullptr;
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT || pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
						temp = allocate_general_purpose_fp_register(mc);
						temp->set_double_precision(pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE);
					}
					else
						temp = allocate_general_purpose_register(mc);
					if (to_use->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, temp->raw_register_operand(), to_use->raw_register_operand()));
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, temp->raw_register_operand(), to_use->raw_register_operand()));
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, temp->raw_register_operand(), to_use->raw_register_operand(), make_shared<immediate_operand>(0)));
					}
					else {
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LWC1, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LDC1, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
						else {
							bool u = pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED;
							if (sz == 4)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
							else if (sz == 2)
								mc->add_insn(make_shared<insn>(u ? insn::kind::KIND_LHU : insn::kind::KIND_LH, temp->raw_register_operand(), make_shared<immediate_operand>(0),
									to_use->raw_register_operand()));
							else if (sz == 1)
								mc->add_insn(make_shared<insn>(u ? insn::kind::KIND_LBU : insn::kind::KIND_LB, temp->raw_register_operand(), make_shared<immediate_operand>(0),
									to_use->raw_register_operand()));
						}
					}
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
						shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_L_S, frw->raw_register_operand(), make_shared<label_operand>(to_add == 1 ? "__fp_1" : "__fp_m1")));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADD_S, temp->raw_register_operand(), temp->raw_register_operand(), frw->raw_register_operand()));
						free_general_purpose_register(mc, frw);
					}
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
						shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_L_D, frw->raw_register_operand(), make_shared<label_operand>(to_add == 1 ? "__dp_1" : "__dp_m1")));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADD_D, temp->raw_register_operand(), temp->raw_register_operand(), frw->raw_register_operand()));
						free_general_purpose_register(mc, frw);
					}
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, temp->raw_register_operand(), temp->raw_register_operand(), make_shared<immediate_operand>(to_add)));
					if (to_use->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, temp->raw_register_operand(), to_use->raw_register_operand()));
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, temp->raw_register_operand(), to_use->raw_register_operand()));
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, to_use->raw_register_operand(), temp->raw_register_operand(), make_shared<immediate_operand>(0)));
					}
					else {
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
						else {
							if (sz == 4)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
							else if (sz == 2)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SH, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
							else if (sz == 1)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SB, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
						}
					}
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
						shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_L_S, frw->raw_register_operand(), make_shared<label_operand>(to_add == 1 ? "__fp_m1" : "__fp_1")));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADD_S, temp->raw_register_operand(), temp->raw_register_operand(), frw->raw_register_operand()));
						free_general_purpose_register(mc, frw);
					}
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
						shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_L_D, frw->raw_register_operand(), make_shared<label_operand>(to_add == 1 ? "__dp_m1" : "__dp_1")));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADD_D, temp->raw_register_operand(), temp->raw_register_operand(), frw->raw_register_operand()));
						free_general_purpose_register(mc, frw);
					}
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, temp->raw_register_operand(), temp->raw_register_operand(), make_shared<immediate_operand>(-to_add)));
					free_general_purpose_register(mc, to_use);
					temp->set_already_loaded(true);
					op = make_shared<operand_wrapper>(temp);
				}
					break;
				case postfix_expression::kind::KIND_AT: {
					shared_ptr<register_wrapper> lhs_reg = nullptr;
					bool needs_loading = true;
					if (op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
						lhs_reg = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, lhs_reg->raw_register_operand(), register_file::register_objects::_zero_register,
							op->contained_immediate_operand()));
					}
					else if (op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
						if (op->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
							needs_loading = false;
							lhs_reg = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, lhs_reg->raw_register_operand(), op->contained_register_wrapper()->raw_register_operand(),
								make_shared<immediate_operand>(0)));
						}
						else
							lhs_reg = op->contained_register_wrapper();
					}
					else
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if(needs_loading && !lhs_reg->already_loaded())
						mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, lhs_reg->raw_register_operand(), make_shared<immediate_operand>(0), lhs_reg->raw_register_operand()));
					lhs_reg->set_register_wrapper_function_argument_kind(register_wrapper::function_argument_kind::KIND_NON_FUNCTION_ARGUMENT);
					if (lhs_reg->already_loaded()) lhs_reg->set_already_loaded(false);
					op = make_shared<operand_wrapper>(lhs_reg);
				}
					break;
				case postfix_expression::kind::KIND_SUBSCRIPT: {
					shared_ptr<operand_wrapper> ow = generate_expression_mips(mc, pt->subscript(), binary_expression::operator_kind::KIND_NONE, nullptr,
						vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
					shared_ptr<register_wrapper> rhs_reg = nullptr;
					if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
						rhs_reg = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rhs_reg->raw_register_operand(), register_file::register_objects::_zero_register,
							ow->contained_immediate_operand()));
					}
					else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
						rhs_reg = ow->contained_register_wrapper();
					else
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<register_wrapper> lhs_reg = nullptr;
					bool needs_loading = true;
					if (op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
						lhs_reg = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, lhs_reg->raw_register_operand(), register_file::register_objects::_zero_register,
							op->contained_immediate_operand()));
					}
					else if (op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
						if (op->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
							needs_loading = false;
							lhs_reg = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, lhs_reg->raw_register_operand(), op->contained_register_wrapper()->raw_register_operand(),
								make_shared<immediate_operand>(0)));
						}
						else
							lhs_reg = op->contained_register_wrapper();
					}
					else
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if(needs_loading && !lhs_reg->already_loaded())
						mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, lhs_reg->raw_register_operand(), make_shared<immediate_operand>(0), lhs_reg->raw_register_operand()));
					shared_ptr<type> subscripted_type = pt->postfix_type_type();
					int contained_type_size = calculate_type_size(mc, subscripted_type);
					if (contained_type_size != 0 && ((contained_type_size & (contained_type_size - 1)) == 0)) {
						int sll_amount = log2(contained_type_size);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_SLL, rhs_reg->raw_register_operand(), rhs_reg->raw_register_operand(), make_shared<immediate_operand>(sll_amount)));
					}
					else {
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register,
							make_shared<immediate_operand>(contained_type_size)));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_MULTU, rhs_reg->raw_register_operand(), rw->raw_register_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_MFLO, rhs_reg->raw_register_operand()));
						free_general_purpose_register(mc, rw);
					}
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, lhs_reg->raw_register_operand(), lhs_reg->raw_register_operand(), rhs_reg->raw_register_operand()));
					free_general_purpose_register(mc, rhs_reg);
					lhs_reg->set_register_wrapper_function_argument_kind(register_wrapper::function_argument_kind::KIND_NON_FUNCTION_ARGUMENT);
					if (lhs_reg->already_loaded()) lhs_reg->set_already_loaded(false);
					op = make_shared<operand_wrapper>(lhs_reg);
				}
					break;
				default:
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
					break;
				}
				prev_type = pt->postfix_type_type();
			}
			if (assign_op == binary_expression::operator_kind::KIND_NONE && ptl.size() == 0 && (utl.size() == 0 || (utl.size() != 0 && 
				utl[0] != unary_expression::kind::KIND_INCREMENT && utl[0] != unary_expression::kind::KIND_DECREMENT))) {
				if (pe->postfix_expression_value_kind() == value_kind::VALUE_LVALUE) {
					if (op->wrapper_operand_kind() != operand_wrapper::operand_kind::KIND_REGISTER)
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<register_wrapper> to_use = op->contained_register_wrapper();
					if (pe->postfix_expression_type()->type_kind() == type::kind::KIND_PRIMITIVE &&
						to_use->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT &&
						pe->postfix_expression_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY && !to_use->already_loaded()) {
						int sz = (int)calculate_type_size(mc, pe->postfix_expression_type());
						shared_ptr<register_wrapper> temp = nullptr;
						if (static_pointer_cast<primitive_type>(pe->postfix_expression_type())->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
							temp = allocate_general_purpose_fp_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LWC1, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
						}
						else if (static_pointer_cast<primitive_type>(pe->postfix_expression_type())->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
							temp = allocate_general_purpose_fp_register(mc);
							temp->set_double_precision(true);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LDC1, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
						}
						else {
							bool u = static_pointer_cast<primitive_type>(pe->postfix_expression_type())->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED;
							temp = allocate_general_purpose_register(mc);
							if (sz == 4)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, temp->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
							else if (sz == 2)
								mc->add_insn(make_shared<insn>(u ? insn::kind::KIND_LHU : insn::kind::KIND_LH, temp->raw_register_operand(), make_shared<immediate_operand>(0),
									to_use->raw_register_operand()));
							else if (sz == 1)
								mc->add_insn(make_shared<insn>(u ? insn::kind::KIND_LBU : insn::kind::KIND_LB, temp->raw_register_operand(), make_shared<immediate_operand>(0),
									to_use->raw_register_operand()));
						}
						free_general_purpose_register(mc, to_use);
						to_use = temp;
						to_use->set_already_loaded(true);
						return make_shared<operand_wrapper>(to_use);
					}
					else if (to_use->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT &&
						pe->postfix_expression_type()->type_array_kind() == type::array_kind::KIND_ARRAY && !to_use->already_loaded()) {
						mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, to_use->raw_register_operand(), make_shared<immediate_operand>(0), to_use->raw_register_operand()));
						to_use->set_already_loaded(true);
						return make_shared<operand_wrapper>(to_use);
					}
				}
			}
			return op;
		}

		shared_ptr<operand_wrapper> literal_token_2_operand_wrapper(shared_ptr<mips_code> mc, shared_ptr<primary_expression> pe) {
			auto replace_all = [](string str, string to_replace, string to_place) {
				int index = 0;
				while (true) {
					index = str.find(to_replace, index);
					if (index == string::npos) break;
					str.replace(index, to_replace.size(), to_place);
					index += to_place.size();
				}
				return str;
			};
			token lit = pe->literal_token();
			switch (lit.token_kind()) {
			case token::kind::TOKEN_STRING: {
				string lname = "__s_lit_" + to_string(mc->next_misc_counter());
				shared_ptr<label_operand> lo = make_shared<label_operand>(lname);
				shared_ptr<register_wrapper> reg = allocate_general_purpose_register(mc);
				string actual_text;
				for (token t : pe->stream())
					actual_text += t.raw_text().substr(1, t.raw_text().size() - 2);
				mc->add_data_directive(make_shared<data_directive>(true, lo, "\"" + actual_text + "\""), false);
				mc->add_insn(make_shared<insn>(insn::kind::KIND_LA, reg->raw_register_operand(), lo));
				reg->set_already_loaded(true);
				return make_shared<operand_wrapper>(reg);
			}
				break;
			case token::kind::TOKEN_INTEGER: {
				string raw = lit.raw_text();
				raw = replace_all(raw, "'", "");
				int base = 10;
				switch (lit.prefix_kind()) {
				case token::prefix::PREFIX_BINARY:
					base = 2;
					raw = raw.substr(2);
					break;
				case token::prefix::PREFIX_HEXADECIMAL:
					base = 16;
					raw = raw.substr(2);
					break;
				case token::prefix::PREFIX_OCTAL:
					base = 8;
					raw = raw.substr(2);
					break;
				}
				int val = stoul(raw, nullptr, base);
				if (val < INT16_MAX && val > INT16_MIN) {
					shared_ptr<immediate_operand> io = make_shared<immediate_operand>(val);
					if (lit.suffix_kind() == token::suffix::SUFFIX_FLOAT) {
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc), frw = allocate_general_purpose_fp_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, io));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rw->raw_register_operand(), frw->raw_register_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, frw->raw_register_operand(), frw->raw_register_operand()));
						free_general_purpose_register(mc, rw);
						return make_shared<operand_wrapper>(frw);
					}
					else if (lit.suffix_kind() == token::suffix::SUFFIX_DOUBLE) {
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc), frw = allocate_general_purpose_fp_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, io));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rw->raw_register_operand(), frw->raw_register_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, frw->raw_register_operand(), frw->raw_register_operand()));
						free_general_purpose_register(mc, rw);
						frw->set_double_precision(true);
						return make_shared<operand_wrapper>(frw);
					}
					else
						return make_shared<operand_wrapper>(io);
				}
				else {
					shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_LUI, rw->raw_register_operand(), make_shared<immediate_operand>((unsigned int) val >> 16)));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ORI, rw->raw_register_operand(), make_shared<immediate_operand>(val & 0xFFFF)));
					if (lit.suffix_kind() == token::suffix::SUFFIX_FLOAT) {
						shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rw->raw_register_operand(), frw->raw_register_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, frw->raw_register_operand(), frw->raw_register_operand()));
						free_general_purpose_register(mc, rw);
						return make_shared<operand_wrapper>(frw);
					}
					else if (lit.suffix_kind() == token::suffix::SUFFIX_DOUBLE) {
						shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rw->raw_register_operand(), frw->raw_register_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, frw->raw_register_operand(), frw->raw_register_operand()));
						frw->set_double_precision(true);
						free_general_purpose_register(mc, rw);
						return make_shared<operand_wrapper>(frw);
					}
					else
						return make_shared<operand_wrapper>(rw);
				}
			}
				break;
			case token::kind::TOKEN_TRUE:
			case token::kind::TOKEN_FALSE: {
				int i = lit.token_kind() == token::kind::TOKEN_TRUE;
				return make_shared<operand_wrapper>(make_shared<immediate_operand>(i));
			}
				break;
			case token::kind::TOKEN_CHARACTER: {
				string raw = lit.raw_text();
				return make_shared<operand_wrapper>(make_shared<immediate_operand>(raw));
			}
				break;
			case token::kind::TOKEN_DECIMAL: {
				if (lit.suffix_kind() == token::suffix::SUFFIX_FLOAT) {
					float f = stof(replace_all(lit.raw_text(), "'", ""));
					string s = "__fp_lit_" + to_string(mc->next_misc_counter());
					shared_ptr<label_operand> lo = make_shared<label_operand>(s);
					mc->add_data_directive(make_shared<data_directive>(true, lo, f), false);
					shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_L_S, frw->raw_register_operand(), lo));
					return make_shared<operand_wrapper>(frw);
				}
				else {
					double d = stod(replace_all(lit.raw_text(), "'", ""));
					string s = "__dp_lit_" + to_string(mc->next_misc_counter());
					shared_ptr<label_operand> lo = make_shared<label_operand>(s);
					mc->add_data_directive(make_shared<data_directive>(true, lo, d), false);
					shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_L_D, frw->raw_register_operand(), lo));
					frw->set_double_precision(true);
					return make_shared<operand_wrapper>(frw);
				}
			}
				break;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand_wrapper> generate_primary_expression_mips(shared_ptr<mips_code> mc, shared_ptr<primary_expression> pe, binary_expression::operator_kind assign_op,
			shared_ptr<operand_wrapper> assign_to, vector<unary_expression::kind> utl, vector<shared_ptr<postfix_expression::postfix_type>> ptl) {
			if (pe == nullptr || !pe->valid())
				return nullptr;
			switch (pe->primary_expression_kind()) {
			case primary_expression::kind::KIND_LITERAL: {
				return literal_token_2_operand_wrapper(mc, pe);
			}
				break;
			case primary_expression::kind::KIND_SIZEOF_EXPRESSION: {
				shared_ptr<operand_wrapper> ow = generate_expression_mips(mc, pe->parenthesized_expression(), binary_expression::operator_kind::KIND_NONE, nullptr,
					vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
				int pre_off = mc->current_frame()->local_stack_offset();
				if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, ow->contained_immediate_operand()));
					free_general_purpose_register(mc, rw);
				}
				else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
					if(ow->contained_register_wrapper()->register_wrapper_function_argument_kind() != register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
						free_general_purpose_register(mc, ow->contained_register_wrapper());
				}
				int post_off = mc->current_frame()->local_stack_offset();
				if (pre_off != post_off) {
					shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(pre_off - post_off)));
					mc->current_frame()->change_local_stack_offset(pre_off - post_off);
				}
				unsigned int sz = (unsigned int)calculate_type_size(mc, pe->parenthesized_expression()->expression_type());
				return make_shared<operand_wrapper>(make_shared<immediate_operand>(sz));
			}
				break;
			case primary_expression::kind::KIND_SIZEOF_TYPE: {
				unsigned int sz = (unsigned int)calculate_type_size(mc, pe->sizeof_type());
				return make_shared<operand_wrapper>(make_shared<immediate_operand>(sz));
			}
				break;
			case primary_expression::kind::KIND_PARENTHESIZED_EXPRESSION:
				return generate_expression_mips(mc, pe->parenthesized_expression(), assign_op, assign_to, utl, ptl);
				break;
			case primary_expression::kind::KIND_IDENTIFIER: {
				shared_ptr<symbol> s = pe->identifier_symbol();
				string sym_string = symbol_2_string(mc, s);
				long id = mc->current_frame()->get_variable_id(sym_string);
				shared_ptr<register_wrapper> rw = mc->current_frame()->get_register_wrapper(id);
				shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
				if (s->symbol_kind() == symbol::kind::KIND_VARIABLE) {
					shared_ptr<variable_symbol> vsym = static_pointer_cast<variable_symbol>(s);
					int sz = (int)calculate_type_size(mc, vsym->variable_type());
					shared_ptr<register_wrapper> ret = nullptr;
					if (rw->register_wrapper_state_kind() == register_wrapper::state_kind::KIND_REGULAR &&
						((rw->raw_register_operand()->register_number() >= register_file::register_2_int.at(register_file::_a0) &&
						rw->raw_register_operand()->register_number() <= register_file::register_2_int.at(register_file::_a3)) ||
							rw->raw_register_operand()->register_number() == register_file::register_2_int.at(register_file::_f12) ||
							rw->raw_register_operand()->register_number() == register_file::register_2_int.at(register_file::_f14)))
						return make_shared<operand_wrapper>(rw);
					else if (vsym->variable_type()->type_kind() == type::kind::KIND_PRIMITIVE && vsym->variable_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
						if (sz == 0)
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(vsym->variable_type());
						ret = allocate_general_purpose_register(mc);
						if (rw->register_wrapper_state_kind() == register_wrapper::state_kind::KIND_GLOBAL)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LA, ret->raw_register_operand(), rw->identifier()));
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, ret->raw_register_operand(), sp, make_shared<immediate_operand>(rw->offset_from_stack_ptr())));
					}
					else if (vsym->variable_type()->type_kind() == type::kind::KIND_STRUCT && vsym->variable_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
						if (sz == 0)
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						ret = allocate_general_purpose_register(mc);
						if (rw->register_wrapper_state_kind() == register_wrapper::state_kind::KIND_GLOBAL)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LA, ret->raw_register_operand(), rw->identifier()));
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, ret->raw_register_operand(), sp, make_shared<immediate_operand>(rw->offset_from_stack_ptr())));
					}
					else {
						if (vsym->variable_type()->type_array_kind() != type::array_kind::KIND_ARRAY)
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						ret = allocate_general_purpose_register(mc);
						if (rw->register_wrapper_state_kind() == register_wrapper::state_kind::KIND_GLOBAL)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LA, ret->raw_register_operand(), rw->identifier()));
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, ret->raw_register_operand(), sp, make_shared<immediate_operand>(rw->offset_from_stack_ptr())));
					}
					return make_shared<operand_wrapper>(ret);
				}
				else if (s->symbol_kind() == symbol::kind::KIND_FUNCTION) {
					shared_ptr<function_symbol> fsym = static_pointer_cast<function_symbol>(s);
					bool main_function = is_function_main(mc, fsym);
					string sym_string = "";
					if (main_function)
						sym_string = "main";
					else
						sym_string = symbol_2_string(mc, s);
					shared_ptr<operand_wrapper> ret = make_shared<operand_wrapper>(make_shared<label_operand>(sym_string));
					return ret;
				}
				else
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
				break;
			case primary_expression::kind::KIND_NEW: {
				shared_ptr<type> t = pe->new_type();
				int contained_type_size = (int)calculate_type_size(mc, t);
				shared_ptr<operand_wrapper> ow = generate_expression_mips(mc, pe->parenthesized_expression(), binary_expression::operator_kind::KIND_NONE, nullptr,
					vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
				shared_ptr<register_operand> sp = register_file::register_objects::_sp_register, a0 = register_file::register_objects::_a0_register,
					v0 = register_file::register_objects::_v0_register;
				shared_ptr<register_wrapper> offset_rw = nullptr;
				if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					offset_rw = allocate_general_purpose_register(mc);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, offset_rw->raw_register_operand(), register_file::register_objects::_zero_register,
						ow->contained_immediate_operand()));
				}
				else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
					offset_rw = ow->contained_register_wrapper();
				else
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				if (contained_type_size != 0 && ((contained_type_size & (contained_type_size - 1)) == 0)) {
					int sll_amount = log2(contained_type_size);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_SLL, offset_rw->raw_register_operand(), offset_rw->raw_register_operand(), make_shared<immediate_operand>(sll_amount)));
				}
				else {
					shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register,
						make_shared<immediate_operand>(contained_type_size)));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_MULTU, offset_rw->raw_register_operand(), rw->raw_register_operand()));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_MFLO, offset_rw->raw_register_operand()));
					free_general_purpose_register(mc, rw);
				}
				bool is_in_use = mc->current_frame()->is_register_marked(register_file::register_2_int.at(register_file::_a0));
				if (is_in_use) {
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-8)));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, a0, make_shared<immediate_operand>(0), sp));
				}
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, a0, register_file::register_objects::_zero_register, offset_rw->raw_register_operand()));
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, v0, register_file::register_objects::_zero_register, make_shared<immediate_operand>(9)));
				mc->add_insn(make_shared<insn>(insn::kind::KIND_SYSCALL));
				if (is_in_use) {
					mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, a0, make_shared<immediate_operand>(0), sp));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(8)));
				}
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, offset_rw->raw_register_operand(), register_file::register_objects::_zero_register,
					v0));
				offset_rw->set_already_loaded(true);
				return make_shared<operand_wrapper>(offset_rw);
			}
				break;
			case primary_expression::kind::KIND_ARRAY_INITIALIZER: {
				shared_ptr<register_wrapper> ret = allocate_general_purpose_register(mc);
				int sz = (int)calculate_type_size(mc, pe->array_initializer()[0]->assignment_expression_type());
				int adjusted_sz = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
				int actual_size = adjusted_sz * pe->array_initializer().size();
				string lab = "__temp_arr_init_" + to_string(mc->next_misc_counter());
				shared_ptr<label_operand> lo = make_shared<label_operand>(lab);
				mc->add_data_directive(make_shared<data_directive>(true, lo, actual_size), false);
				mc->add_insn(make_shared<insn>(insn::kind::KIND_LA, ret->raw_register_operand(), lo));
				shared_ptr<type> parent_type = pe->array_initializer()[0]->assignment_expression_type();
				int curr = 0;
				for (shared_ptr<assignment_expression> ae : pe->array_initializer()) {
					shared_ptr<register_wrapper> rw = nullptr;
					shared_ptr<operand_wrapper> ow = generate_assignment_expression_mips(mc, ae, binary_expression::operator_kind::KIND_NONE, nullptr, vector<unary_expression::kind>{},
						vector<shared_ptr<postfix_expression::postfix_type>>{});
					if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
						rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register,
							ow->contained_immediate_operand()));
					}
					else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
						rw = ow->contained_register_wrapper();
					else
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (parent_type->type_kind() == type::kind::KIND_PRIMITIVE && parent_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
						shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(parent_type);
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
							if (rw->raw_register_operand()->register_number() > 31)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, rw->raw_register_operand(), make_shared<immediate_operand>(curr), ret->raw_register_operand()));
							else {
								shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rw->raw_register_operand(), frw->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, frw->raw_register_operand(), frw->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, frw->raw_register_operand(), make_shared<immediate_operand>(curr), ret->raw_register_operand()));
								free_general_purpose_register(mc, frw);
							}
						}
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
							if (rw->raw_register_operand()->register_number() > 31)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, rw->raw_register_operand(), make_shared<immediate_operand>(curr), ret->raw_register_operand()));
							else {
								shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rw->raw_register_operand(), frw->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, frw->raw_register_operand(), frw->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, frw->raw_register_operand(), make_shared<immediate_operand>(curr), ret->raw_register_operand()));
								free_general_purpose_register(mc, frw);
							}
						}
						else {
							if (rw->raw_register_operand()->register_number() > 31) {
								if (!rw->double_precision()) {
									shared_ptr<register_wrapper> new_rw = allocate_general_purpose_register(mc);
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_S, rw->raw_register_operand(), rw->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MFC1, new_rw->raw_register_operand(), rw->raw_register_operand()));
									if (rw->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, rw->raw_register_operand(), rw->raw_register_operand()));
									else
										free_general_purpose_register(mc, rw);
									rw = new_rw;
								}
								else {
									shared_ptr<register_wrapper> new_rw = allocate_general_purpose_register(mc);
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_D, rw->raw_register_operand(), rw->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_MFC1, new_rw->raw_register_operand(), rw->raw_register_operand()));
									if (rw->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, rw->raw_register_operand(), rw->raw_register_operand()));
									else
										free_general_purpose_register(mc, rw);
									rw = new_rw;
								}
							}
							if (sz == 4)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, rw->raw_register_operand(), make_shared<immediate_operand>(curr), ret->raw_register_operand()));
							else if (sz == 2)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SH, rw->raw_register_operand(), make_shared<immediate_operand>(curr), ret->raw_register_operand()));
							else if (sz == 1)
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SB, rw->raw_register_operand(), make_shared<immediate_operand>(curr), ret->raw_register_operand()));
						}
						curr += 4;
					}
					else if (parent_type->type_kind() == type::kind::KIND_STRUCT && parent_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
						shared_ptr<register_wrapper> loop_iterator = allocate_general_purpose_register(mc);
						for (int count = 0; count < adjusted_sz; count += 4, curr += 4) {
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, loop_iterator->raw_register_operand(), make_shared<immediate_operand>(count), rw->raw_register_operand()));
							mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, loop_iterator->raw_register_operand(), make_shared<immediate_operand>(curr), ret->raw_register_operand()));
						}
						free_general_purpose_register(mc, loop_iterator);
					}
					else {
						if (ae->assignment_expression_type()->type_array_kind() != type::array_kind::KIND_ARRAY)
							mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, rw->raw_register_operand(), make_shared<immediate_operand>(curr), ret->raw_register_operand()));
						curr += 4;
					}
					free_general_purpose_register(mc, rw);
				}
				ret->set_already_loaded(true);
				return make_shared<operand_wrapper>(ret);
			}
				break;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<register_wrapper> allocate_general_purpose_register(shared_ptr<mips_code> mc) {
			int t0 = register_file::register_2_int.at(register_file::_t0), t1 = register_file::register_2_int.at(register_file::_t1),
				t2 = register_file::register_2_int.at(register_file::_t2), t3 = register_file::register_2_int.at(register_file::_t3),
				t4 = register_file::register_2_int.at(register_file::_t4), t5 = register_file::register_2_int.at(register_file::_t5),
				t6 = register_file::register_2_int.at(register_file::_t6), t7 = register_file::register_2_int.at(register_file::_t7);
			for (int r : { t0, t1, t2, t3, t4, t5, t6, t7 })
				if (!mc->current_frame()->is_register_marked(r)) {
					mc->current_frame()->mark_register(r);
					return mc->current_frame()->generate_register_wrapper(register_file::register_objects::int_2_register_object.at(r));
				}
			int s0 = register_file::register_2_int.at(register_file::_s0), s1 = register_file::register_2_int.at(register_file::_s1),
				s2 = register_file::register_2_int.at(register_file::_s2), s3 = register_file::register_2_int.at(register_file::_s3),
				s4 = register_file::register_2_int.at(register_file::_s4), s5 = register_file::register_2_int.at(register_file::_s5),
				s6 = register_file::register_2_int.at(register_file::_s6), s7 = register_file::register_2_int.at(register_file::_s7);
			for (int r : { s0, s1, s2, s3, s4, s5, s6, s7 }) {
				int offset = r - s0;
				if (mc->current_frame()->is_s_register_initially_stored(offset) && !mc->current_frame()->is_register_marked(r)) {
					mc->current_frame()->mark_register(r);
					return mc->current_frame()->generate_register_wrapper(register_file::register_objects::int_2_register_object.at(r));
				}
			}
			for (int r : { s0, s1, s2, s3, s4, s5, s6, s7 }) {
				int offset = r - s0;
				if (!mc->current_frame()->is_s_register_initially_stored(offset)) {
					mc->current_frame()->change_local_stack_offset(-8);
					shared_ptr<register_operand> sp = register_file::register_objects::register_2_register_object.at(register_file::_sp),
						curr = register_file::register_objects::int_2_register_object.at(r);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-8)));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, curr, make_shared<immediate_operand>(0), sp));
					mc->current_frame()->mark_register(r);
					mc->current_frame()->set_s_register_initial_offset(offset, 0);
					mc->current_frame()->mark_s_register_initially_stored(offset);
					return mc->current_frame()->generate_register_wrapper(register_file::register_objects::int_2_register_object.at(r));
				}
			}
			int curr_overflow = mc->current_frame()->next_overflow_register();
			bool t_reg = curr_overflow <= 7;
			int reg_offset = t_reg ? t0 : s0;
			int reg_to_use = reg_offset + curr_overflow;
			mc->current_frame()->change_local_stack_offset(-8);
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register,
				curr = register_file::register_objects::int_2_register_object.at(reg_to_use);
			mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-8)));
			mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, curr, make_shared<immediate_operand>(0), sp));
			shared_ptr<register_wrapper> old = mc->current_frame()->find_last_usage(reg_to_use);
			if (old == nullptr)
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			old->set_offset_from_stack_ptr(0);
			old->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_ON_STACK);
			mc->current_frame()->mark_register(reg_to_use);
			shared_ptr<register_wrapper> gen = mc->current_frame()->generate_register_wrapper(register_file::register_objects::int_2_register_object.at(reg_to_use));
			return gen;
		}

		shared_ptr<register_wrapper> allocate_general_purpose_fp_register(shared_ptr<mips_code> mc) {
			int f4 = register_file::register_2_int.at(register_file::_f4), f6 = register_file::register_2_int.at(register_file::_f6),
				f8 = register_file::register_2_int.at(register_file::_f8), f10 = register_file::register_2_int.at(register_file::_f10);
			for (int r : { f4, f6, f8, f10 })
				if (!mc->current_frame()->is_register_marked(r)) {
					mc->current_frame()->mark_register(r);
					shared_ptr<register_wrapper> rw = mc->current_frame()->generate_register_wrapper(register_file::register_objects::int_2_register_object.at(r));
					return rw;
				}
			int f20 = register_file::register_2_int.at(register_file::_f20), f22 = register_file::register_2_int.at(register_file::_f22),
				f24 = register_file::register_2_int.at(register_file::_f24), f26 = register_file::register_2_int.at(register_file::_f26),
				f28 = register_file::register_2_int.at(register_file::_f28), f30 = register_file::register_2_int.at(register_file::_f30);
			for (int r : { f20, f22, f24, f26, f28, f30 }) {
				int offset = 8 + (r - f20) / 2;
				if (mc->current_frame()->is_s_register_initially_stored(offset) && !mc->current_frame()->is_register_marked(r)) {
					mc->current_frame()->mark_register(r);
					shared_ptr<register_wrapper> rw = mc->current_frame()->generate_register_wrapper(register_file::register_objects::int_2_register_object.at(r));
					return rw;
				}
			}
			for (int r : { f20, f22, f24, f26, f28, f30 }) {
				int offset = 8 + (r - f20) / 2;
				if (!mc->current_frame()->is_s_register_initially_stored(offset)) {
					mc->current_frame()->change_local_stack_offset(-8);
					shared_ptr<register_operand> sp = register_file::register_objects::register_2_register_object.at(register_file::_sp),
						curr = register_file::register_objects::int_2_register_object.at(r);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-8)));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, curr, make_shared<immediate_operand>(0), sp));
					mc->current_frame()->mark_register(r);
					mc->current_frame()->set_s_register_initial_offset(offset, 0);
					mc->current_frame()->mark_s_register_initially_stored(offset);
					return mc->current_frame()->generate_register_wrapper(register_file::register_objects::int_2_register_object.at(r));
				}
			}
			int curr_overflow = mc->current_frame()->next_fp_overflow_register();
			bool t_reg = curr_overflow <= 3;
			int reg_offset = t_reg ? f4 : f20;
			int reg_to_use = reg_offset + t_reg ? curr_overflow * 2 : (curr_overflow - 4) * 2;
			mc->current_frame()->change_local_stack_offset(-8);
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register,
				curr = register_file::register_objects::int_2_register_object.at(reg_to_use);
			mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-8)));
			mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, curr, make_shared<immediate_operand>(0), sp));
			shared_ptr<register_wrapper> old = mc->current_frame()->find_last_usage(reg_to_use);
			if (old == nullptr)
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			old->set_offset_from_stack_ptr(0);
			old->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_ON_STACK);
			mc->current_frame()->mark_register(reg_to_use);
			shared_ptr<register_wrapper> gen = mc->current_frame()->generate_register_wrapper(register_file::register_objects::int_2_register_object.at(reg_to_use));
			return gen;
		}

		void free_general_purpose_register(shared_ptr<mips_code> mc, shared_ptr<register_wrapper> rw) {
			if (rw->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
				return;
			bool fp = rw->raw_register_operand()->register_number() >= 31;
			mc->current_frame()->remove_register_wrapper(rw->wrapper_id());
			shared_ptr<register_wrapper> last_usage = mc->current_frame()->find_last_usage(rw->raw_register_operand()->register_number());
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
			if (last_usage == nullptr)
				mc->current_frame()->unmark_register(rw->raw_register_operand()->register_number());
			else if (last_usage->register_wrapper_state_kind() != register_wrapper::state_kind::KIND_ON_STACK)
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			else {
				last_usage->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_REGULAR);
				mc->add_insn(make_shared<insn>(fp ? insn::kind::KIND_LDC1 : insn::kind::KIND_LW, last_usage->raw_register_operand(),
					make_shared<immediate_operand>(last_usage->offset_from_stack_ptr()), sp));
			}
		}

		shared_ptr<operand_wrapper> binary_expression_helper_1(shared_ptr<mips_code> mc, shared_ptr<operand_wrapper> lhs_op, binary_expression::operator_kind ok,
			shared_ptr<operand_wrapper> rhs_op) {
			if (lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
				load_register_wrapper_from_stack(mc, lhs_op->contained_register_wrapper());
			if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
				load_register_wrapper_from_stack(mc, rhs_op->contained_register_wrapper());
			if ((rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER && rhs_op->contained_register_wrapper()->raw_register_operand()->register_number() > 31) ||
				(lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER && lhs_op->contained_register_wrapper()->raw_register_operand()->register_number() > 31)) {
				bool dp = (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER && rhs_op->contained_register_wrapper()->double_precision()) ||
					(lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER && lhs_op->contained_register_wrapper()->double_precision());
				shared_ptr<register_wrapper> frhs = convert_operand_wrapper_2_fp_register_wrapper(mc, rhs_op, dp),
					flhs = convert_operand_wrapper_2_fp_register_wrapper(mc, lhs_op, dp);
				switch (ok) {
				case binary_expression::operator_kind::KIND_ADD:
					if (!dp)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADD_S, flhs->raw_register_operand(), flhs->raw_register_operand(), frhs->raw_register_operand()));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADD_D, flhs->raw_register_operand(), flhs->raw_register_operand(), frhs->raw_register_operand()));
					break;
				case binary_expression::operator_kind::KIND_SUBTRACT:
					if (!dp)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_SUB_S, flhs->raw_register_operand(), flhs->raw_register_operand(), frhs->raw_register_operand()));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_SUB_D, flhs->raw_register_operand(), flhs->raw_register_operand(), frhs->raw_register_operand()));
					break;
				case binary_expression::operator_kind::KIND_MULTIPLY:
					if (!dp)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_MUL_S, flhs->raw_register_operand(), flhs->raw_register_operand(), frhs->raw_register_operand()));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_MUL_D, flhs->raw_register_operand(), flhs->raw_register_operand(), frhs->raw_register_operand()));
					break;
				case binary_expression::operator_kind::KIND_DIVIDE:
					if (!dp)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_DIV_S, flhs->raw_register_operand(), flhs->raw_register_operand(), frhs->raw_register_operand()));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_DIV_D, flhs->raw_register_operand(), flhs->raw_register_operand(), frhs->raw_register_operand()));
					break;
				default:
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					break;
				}
				free_general_purpose_register(mc, frhs);
				return make_shared<operand_wrapper>(flhs);
			}
			if (lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
				if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
					switch (ok) {
					case binary_expression::operator_kind::KIND_ADD: {
						// addition is commutative, just do rhs_op + lhs_op
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rhs_op->contained_register_wrapper()->raw_register_operand(),
							rhs_op->contained_register_wrapper()->raw_register_operand(), lhs_op->contained_immediate_operand()));
						return rhs_op;
					}
						break;
					case binary_expression::operator_kind::KIND_SUBTRACT: {
						mc->add_insn(make_shared<insn>(insn::kind::KIND_SUBU, rhs_op->contained_register_wrapper()->raw_register_operand(), register_file::register_objects::_zero_register,
							rhs_op->contained_register_wrapper()->raw_register_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rhs_op->contained_register_wrapper()->raw_register_operand(),
							rhs_op->contained_register_wrapper()->raw_register_operand(), lhs_op->contained_immediate_operand()));
						return rhs_op;
					}
						break;
					case binary_expression::operator_kind::KIND_MODULUS:
					case binary_expression::operator_kind::KIND_DIVIDE:
					case binary_expression::operator_kind::KIND_MULTIPLY: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_MULTIPLY ? insn::kind::KIND_MULTU : insn::kind::KIND_DIVU,
							ik2 = ok == binary_expression::operator_kind::KIND_MODULUS ? insn::kind::KIND_MFHI : insn::kind::KIND_MFLO;
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, lhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(ik, rw->raw_register_operand(), rhs_op->contained_register_wrapper()->raw_register_operand()));
						mc->add_insn(make_shared<insn>(ik2, rw->raw_register_operand()));
						free_general_purpose_register(mc, rhs_op->contained_register_wrapper());
						return make_shared<operand_wrapper>(rw);
					}
						break;
					case binary_expression::operator_kind::KIND_SHIFT_LEFT:
					case binary_expression::operator_kind::KIND_SHIFT_RIGHT: {
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						insn::kind ik = ok == binary_expression::operator_kind::KIND_SHIFT_LEFT ? insn::kind::KIND_SLLV : insn::kind::KIND_SRLV;
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, lhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(ik, rw->raw_register_operand(), rw->raw_register_operand(), rhs_op->contained_register_wrapper()->raw_register_operand()));
						free_general_purpose_register(mc, rhs_op->contained_register_wrapper());
						return make_shared<operand_wrapper>(rw);
					}
						break;
					case binary_expression::operator_kind::KIND_BITWISE_AND:
					case binary_expression::operator_kind::KIND_BITWISE_OR:
					case binary_expression::operator_kind::KIND_BITWISE_XOR: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_BITWISE_AND ? insn::kind::KIND_ANDI :
							(ok == binary_expression::operator_kind::KIND_BITWISE_OR ? insn::kind::KIND_ORI : insn::kind::KIND_XORI);
						mc->add_insn(make_shared<insn>(ik, rhs_op->contained_register_wrapper()->raw_register_operand(),
							rhs_op->contained_register_wrapper()->raw_register_operand(), lhs_op->contained_immediate_operand()));
						return rhs_op;
					}
						break;
					}
				}
				else if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					switch (ok) {
					case binary_expression::operator_kind::KIND_ADD: {
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, lhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), rw->raw_register_operand(), rhs_op->contained_immediate_operand()));
						return make_shared<operand_wrapper>(rw);
					}
						break;
					case binary_expression::operator_kind::KIND_SUBTRACT: {
						shared_ptr<immediate_operand> io_lhs = lhs_op->contained_immediate_operand(),
							io_rhs = rhs_op->contained_immediate_operand();
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, io_lhs));
						if (io_rhs->immediate_operand_kind() == immediate_operand::kind::KIND_CHAR) {
							shared_ptr<register_wrapper> rw2 = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw2->raw_register_operand(), register_file::register_objects::_zero_register, io_rhs));
							mc->add_insn(make_shared<insn>(insn::kind::KIND_SUBU, rw->raw_register_operand(), rw->raw_register_operand(), rw2->raw_register_operand()));
							free_general_purpose_register(mc, rw2);
						}
						else if (io_rhs->immediate_operand_kind() == immediate_operand::kind::KIND_INTEGRAL)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), rw->raw_register_operand(),
								make_shared<immediate_operand>(-io_rhs->integral_immediate())));
						return make_shared<operand_wrapper>(rw);
					}
						break;
					case binary_expression::operator_kind::KIND_MODULUS:
					case binary_expression::operator_kind::KIND_DIVIDE:
					case binary_expression::operator_kind::KIND_MULTIPLY: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_MULTIPLY ? insn::kind::KIND_MULTU : insn::kind::KIND_DIVU,
							ik2 = ok == binary_expression::operator_kind::KIND_MODULUS ? insn::kind::KIND_MFHI : insn::kind::KIND_MFLO;
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc),
							rw2 = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, lhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw2->raw_register_operand(), register_file::register_objects::_zero_register, rhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(ik, rw->raw_register_operand(), rw2->raw_register_operand()));
						mc->add_insn(make_shared<insn>(ik2, rw->raw_register_operand()));
						free_general_purpose_register(mc, rw2);
						return make_shared<operand_wrapper>(rw);
					}
						break;
					case binary_expression::operator_kind::KIND_SHIFT_LEFT:
					case binary_expression::operator_kind::KIND_SHIFT_RIGHT: {
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, lhs_op->contained_immediate_operand()));
						if (rhs_op->contained_immediate_operand()->immediate_operand_kind() == immediate_operand::kind::KIND_CHAR) {
							shared_ptr<register_wrapper> rw2 = allocate_general_purpose_register(mc);
							insn::kind ik = ok == binary_expression::operator_kind::KIND_SHIFT_LEFT ? insn::kind::KIND_SLLV : insn::kind::KIND_SRLV;
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw2->raw_register_operand(), register_file::register_objects::_zero_register,
								rhs_op->contained_immediate_operand()));
							mc->add_insn(make_shared<insn>(ik, rw->raw_register_operand(), rw->raw_register_operand(), rw2->raw_register_operand()));
							free_general_purpose_register(mc, rw2);
							return make_shared<operand_wrapper>(rw);
						}
						else if (rhs_op->contained_immediate_operand()->immediate_operand_kind() == immediate_operand::kind::KIND_INTEGRAL) {
							if (rhs_op->contained_immediate_operand()->integral_immediate() >= 0 && rhs_op->contained_immediate_operand()->integral_immediate() < 32) {
								insn::kind ik = ok == binary_expression::operator_kind::KIND_SHIFT_LEFT ? insn::kind::KIND_SLL : insn::kind::KIND_SRL;
								mc->add_insn(make_shared<insn>(ik, rw->raw_register_operand(), rw->raw_register_operand(), rhs_op->contained_immediate_operand()));
								return make_shared<operand_wrapper>(rw);
							}
							else {
								insn::kind ik = ok == binary_expression::operator_kind::KIND_SHIFT_LEFT ? insn::kind::KIND_SLLV : insn::kind::KIND_SRLV;
								shared_ptr<register_wrapper> rw2 = allocate_general_purpose_register(mc);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw2->raw_register_operand(), register_file::register_objects::_zero_register,
									rhs_op->contained_immediate_operand()));
								mc->add_insn(make_shared<insn>(ik, rw->raw_register_operand(), rw->raw_register_operand(), rw2->raw_register_operand()));
								free_general_purpose_register(mc, rw2);
								return make_shared<operand_wrapper>(rw);
							}
						}
					}
						break;
					case binary_expression::operator_kind::KIND_BITWISE_AND:
					case binary_expression::operator_kind::KIND_BITWISE_OR:
					case binary_expression::operator_kind::KIND_BITWISE_XOR: {
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, lhs_op->contained_immediate_operand()));
						insn::kind ik = ok == binary_expression::operator_kind::KIND_BITWISE_AND ? insn::kind::KIND_ANDI :
							(ok == binary_expression::operator_kind::KIND_BITWISE_OR ? insn::kind::KIND_ORI : insn::kind::KIND_XORI);
						mc->add_insn(make_shared<insn>(ik, rw->raw_register_operand(), rw->raw_register_operand(), rhs_op->contained_immediate_operand()));
						return make_shared<operand_wrapper>(rw);
					}
						break;
					}
				}
				else
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			else if (lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
				if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					switch (ok) {
					case binary_expression::operator_kind::KIND_ADD: {
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, lhs_op->contained_register_wrapper()->raw_register_operand(),
							lhs_op->contained_register_wrapper()->raw_register_operand(), rhs_op->contained_immediate_operand()));
						return lhs_op;
					}
						break;
					case binary_expression::operator_kind::KIND_SUBTRACT: {
						shared_ptr<immediate_operand> io_rhs = rhs_op->contained_immediate_operand();
						if (io_rhs->immediate_operand_kind() == immediate_operand::kind::KIND_CHAR) {
							shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register,
								io_rhs));
							mc->add_insn(make_shared<insn>(insn::kind::KIND_SUBU, lhs_op->contained_register_wrapper()->raw_register_operand(),
								lhs_op->contained_register_wrapper()->raw_register_operand(), rw->raw_register_operand()));
							free_general_purpose_register(mc, rw);
						}
						else if (io_rhs->immediate_operand_kind() == immediate_operand::kind::KIND_INTEGRAL)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, lhs_op->contained_register_wrapper()->raw_register_operand(),
								lhs_op->contained_register_wrapper()->raw_register_operand(), make_shared<immediate_operand>(-io_rhs->integral_immediate())));
						return lhs_op;
					}
						break;
					case binary_expression::operator_kind::KIND_MODULUS:
					case binary_expression::operator_kind::KIND_DIVIDE:
					case binary_expression::operator_kind::KIND_MULTIPLY: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_MULTIPLY ? insn::kind::KIND_MULTU : insn::kind::KIND_DIVU,
							ik2 = ok == binary_expression::operator_kind::KIND_MODULUS ? insn::kind::KIND_MFHI : insn::kind::KIND_MFLO;
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, rhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(ik, lhs_op->contained_register_wrapper()->raw_register_operand(), rw->raw_register_operand()));
						mc->add_insn(make_shared<insn>(ik2, lhs_op->contained_register_wrapper()->raw_register_operand()));
						free_general_purpose_register(mc, rw);
						return lhs_op;
					}
						break;
					case binary_expression::operator_kind::KIND_SHIFT_LEFT:
					case binary_expression::operator_kind::KIND_SHIFT_RIGHT: {
						shared_ptr<immediate_operand> io = rhs_op->contained_immediate_operand();
						if (io->immediate_operand_kind() == immediate_operand::kind::KIND_CHAR) {
							insn::kind ik = ok == binary_expression::operator_kind::KIND_SHIFT_LEFT ? insn::kind::KIND_SLLV : insn::kind::KIND_SRLV;
							shared_ptr<register_wrapper> rw2 = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw2->raw_register_operand(), register_file::register_objects::_zero_register, io));
							mc->add_insn(make_shared<insn>(ik, lhs_op->contained_register_wrapper()->raw_register_operand(),
								lhs_op->contained_register_wrapper()->raw_register_operand(), rw2->raw_register_operand()));
							free_general_purpose_register(mc, rw2);
							return lhs_op;
						}
						else if (io->immediate_operand_kind() == immediate_operand::kind::KIND_INTEGRAL) {
							if (io->integral_immediate() >= 0 && io->integral_immediate() < 32) {
								insn::kind ik = ok == binary_expression::operator_kind::KIND_SHIFT_LEFT ? insn::kind::KIND_SLL : insn::kind::KIND_SRL;
								mc->add_insn(make_shared<insn>(ik, lhs_op->contained_register_wrapper()->raw_register_operand(),
									lhs_op->contained_register_wrapper()->raw_register_operand(), io));
								return lhs_op;
							}
							else {
								shared_ptr<register_wrapper> rw2 = allocate_general_purpose_register(mc);
								insn::kind ik = ok == binary_expression::operator_kind::KIND_SHIFT_LEFT ? insn::kind::KIND_SLLV : insn::kind::KIND_SRLV;
								mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw2->raw_register_operand(), register_file::register_objects::_zero_register, io));
								mc->add_insn(make_shared<insn>(ik, lhs_op->contained_register_wrapper()->raw_register_operand(),
									lhs_op->contained_register_wrapper()->raw_register_operand(), rw2->raw_register_operand()));
								free_general_purpose_register(mc, rw2);
								return lhs_op;
							}
						}
					}
						break;
					case binary_expression::operator_kind::KIND_BITWISE_AND:
					case binary_expression::operator_kind::KIND_BITWISE_OR:
					case binary_expression::operator_kind::KIND_BITWISE_XOR: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_BITWISE_AND ? insn::kind::KIND_ANDI :
							(ok == binary_expression::operator_kind::KIND_BITWISE_OR ? insn::kind::KIND_ORI : insn::kind::KIND_XORI);
						mc->add_insn(make_shared<insn>(ik, lhs_op->contained_register_wrapper()->raw_register_operand(),
							lhs_op->contained_register_wrapper()->raw_register_operand(), rhs_op->contained_immediate_operand()));
						return lhs_op;
					}
						break;
					}
				}
				else if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
					switch (ok) {
					case binary_expression::operator_kind::KIND_ADD: {
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, lhs_op->contained_register_wrapper()->raw_register_operand(),
							lhs_op->contained_register_wrapper()->raw_register_operand(), rhs_op->contained_register_wrapper()->raw_register_operand()));
						free_general_purpose_register(mc, rhs_op->contained_register_wrapper());
						return lhs_op;
					}
						break;
					case binary_expression::operator_kind::KIND_SUBTRACT: {
						mc->add_insn(make_shared<insn>(insn::kind::KIND_SUBU, lhs_op->contained_register_wrapper()->raw_register_operand(),
							lhs_op->contained_register_wrapper()->raw_register_operand(), rhs_op->contained_register_wrapper()->raw_register_operand()));
						free_general_purpose_register(mc, rhs_op->contained_register_wrapper());
						return lhs_op;
					}
						break;
					case binary_expression::operator_kind::KIND_MODULUS:
					case binary_expression::operator_kind::KIND_DIVIDE:
					case binary_expression::operator_kind::KIND_MULTIPLY: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_MULTIPLY ? insn::kind::KIND_MULTU : insn::kind::KIND_DIVU,
							ik2 = ok == binary_expression::operator_kind::KIND_MODULUS ? insn::kind::KIND_MFHI : insn::kind::KIND_MFLO;
						mc->add_insn(make_shared<insn>(ik, lhs_op->contained_register_wrapper()->raw_register_operand(),
							rhs_op->contained_register_wrapper()->raw_register_operand()));
						mc->add_insn(make_shared<insn>(ik2, lhs_op->contained_register_wrapper()->raw_register_operand()));
						free_general_purpose_register(mc, rhs_op->contained_register_wrapper());
						return lhs_op;
					}
						break;
					case binary_expression::operator_kind::KIND_SHIFT_LEFT:
					case binary_expression::operator_kind::KIND_SHIFT_RIGHT: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_SHIFT_LEFT ? insn::kind::KIND_SLLV : insn::kind::KIND_SRLV;
						mc->add_insn(make_shared<insn>(ik, lhs_op->contained_register_wrapper()->raw_register_operand(),
							lhs_op->contained_register_wrapper()->raw_register_operand(), rhs_op->contained_register_wrapper()->raw_register_operand()));
						free_general_purpose_register(mc, rhs_op->contained_register_wrapper());
						return lhs_op;
					}
						break;
					case binary_expression::operator_kind::KIND_BITWISE_AND:
					case binary_expression::operator_kind::KIND_BITWISE_OR:
					case binary_expression::operator_kind::KIND_BITWISE_XOR: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_BITWISE_AND ? insn::kind::KIND_AND :
							(ok == binary_expression::operator_kind::KIND_BITWISE_OR ? insn::kind::KIND_OR : insn::kind::KIND_XOR);
						mc->add_insn(make_shared<insn>(ik, lhs_op->contained_register_wrapper()->raw_register_operand(),
							lhs_op->contained_register_wrapper()->raw_register_operand(), rhs_op->contained_register_wrapper()->raw_register_operand()));
						free_general_purpose_register(mc, rhs_op->contained_register_wrapper());
						return lhs_op;
					}
						break;
					}
				}
				else
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		void load_register_wrapper_from_stack(shared_ptr<mips_code> mc, shared_ptr<register_wrapper> rw) {
			if (rw == nullptr) return;
			shared_ptr<register_wrapper> last_usage = mc->current_frame()->find_last_usage(rw->raw_register_operand()->register_number());
			if (last_usage == nullptr || last_usage->wrapper_id() == rw->wrapper_id() ||
				rw->register_wrapper_state_kind() == register_wrapper::state_kind::KIND_REGULAR)
				return;
			mc->current_frame()->change_local_stack_offset(-8);
			if (rw->raw_register_operand()->register_number() > 31)
				mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, last_usage->raw_register_operand(), make_shared<immediate_operand>(0), register_file::register_objects::_sp_register));
			else
				mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, last_usage->raw_register_operand(), make_shared<immediate_operand>(0), register_file::register_objects::_sp_register));
			last_usage->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_ON_STACK);
			if (rw->raw_register_operand()->register_number() > 31)
				mc->add_insn(make_shared<insn>(insn::kind::KIND_LWC1, rw->raw_register_operand(), make_shared<immediate_operand>(rw->offset_from_stack_ptr()),
					register_file::register_objects::_sp_register));
			else
				mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, rw->raw_register_operand(), make_shared<immediate_operand>(rw->offset_from_stack_ptr()),
					register_file::register_objects::_sp_register));
			rw->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_REGULAR);
		}

		shared_ptr<operand_wrapper> binary_expression_helper_2(shared_ptr<mips_code> mc, shared_ptr<operand_wrapper> lhs_op, binary_expression::operator_kind ok,
			shared_ptr<operand_wrapper> rhs_op, bool u) {
			if (lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
				load_register_wrapper_from_stack(mc, lhs_op->contained_register_wrapper());
			if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
				load_register_wrapper_from_stack(mc, rhs_op->contained_register_wrapper());
			if ((rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER && rhs_op->contained_register_wrapper()->raw_register_operand()->register_number() > 31) ||
				(lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER && lhs_op->contained_register_wrapper()->raw_register_operand()->register_number() > 31)) {
				bool dp = (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER && rhs_op->contained_register_wrapper()->double_precision()) ||
					(lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER && lhs_op->contained_register_wrapper()->double_precision());
				shared_ptr<register_wrapper> frhs = convert_operand_wrapper_2_fp_register_wrapper(mc, rhs_op, dp),
					flhs = convert_operand_wrapper_2_fp_register_wrapper(mc, lhs_op, dp);
				shared_ptr<register_wrapper> result = allocate_general_purpose_register(mc);
				tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> false_path = mc->next_label_info(), done_path = mc->next_label_info();
				insn::kind to_use = insn::kind::KIND_NONE;
				switch (ok) {
				case binary_expression::operator_kind::KIND_EQUALS_EQUALS:
					if (!dp)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_C_EQ_S, flhs->raw_register_operand(), frhs->raw_register_operand()));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_C_EQ_D, flhs->raw_register_operand(), frhs->raw_register_operand()));
					to_use = insn::kind::KIND_BC1F;
					break;
				case binary_expression::operator_kind::KIND_NOT_EQUALS:
					if (!dp)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_C_EQ_S, flhs->raw_register_operand(), frhs->raw_register_operand()));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_C_EQ_D, flhs->raw_register_operand(), frhs->raw_register_operand()));
					to_use = insn::kind::KIND_BC1T;
					break;
				case binary_expression::operator_kind::KIND_GREATER_THAN:
					if (!dp)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_C_LE_S, flhs->raw_register_operand(), frhs->raw_register_operand()));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_C_LE_D, flhs->raw_register_operand(), frhs->raw_register_operand()));
					to_use = insn::kind::KIND_BC1T;
					break;
				case binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO:
					if (!dp)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_C_LT_S, flhs->raw_register_operand(), frhs->raw_register_operand()));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_C_LT_D, flhs->raw_register_operand(), frhs->raw_register_operand()));
					to_use = insn::kind::KIND_BC1T;
					break;
				case binary_expression::operator_kind::KIND_LESS_THAN:
					if (!dp)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_C_LT_S, flhs->raw_register_operand(), frhs->raw_register_operand()));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_C_LT_D, flhs->raw_register_operand(), frhs->raw_register_operand()));
					to_use = insn::kind::KIND_BC1F;
					break;
				case binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO:
					if (!dp)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_C_LE_S, flhs->raw_register_operand(), frhs->raw_register_operand()));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_C_LE_D, flhs->raw_register_operand(), frhs->raw_register_operand()));
					to_use = insn::kind::KIND_BC1F;
					break;
				default:
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					break;
				}
				free_general_purpose_register(mc, flhs);
				free_general_purpose_register(mc, frhs);
				mc->add_insn(make_shared<insn>(to_use, get<2>(false_path)));
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, result->raw_register_operand(), register_file::register_objects::_zero_register, make_shared<immediate_operand>(1)));
				mc->add_insn(make_shared<insn>(insn::kind::KIND_J, get<2>(done_path)));
				mc->add_insn(get<3>(false_path));
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, result->raw_register_operand(), register_file::register_objects::_zero_register, make_shared<immediate_operand>(0)));
				mc->add_insn(get<3>(done_path));
				return make_shared<operand_wrapper>(result);
			}
			if (lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
				if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
					switch (ok) {
					case binary_expression::operator_kind::KIND_EQUALS_EQUALS:
					case binary_expression::operator_kind::KIND_NOT_EQUALS: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_EQUALS_EQUALS ? insn::kind::KIND_SEQ : insn::kind::KIND_SNE;
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, lhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(ik, rw->raw_register_operand(), rw->raw_register_operand(), rhs_op->contained_register_wrapper()->raw_register_operand()));
						free_general_purpose_register(mc, rhs_op->contained_register_wrapper());
						return make_shared<operand_wrapper>(rw);
					}
						break;
					case binary_expression::operator_kind::KIND_LESS_THAN:
					case binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO:
					case binary_expression::operator_kind::KIND_GREATER_THAN:
					case binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_LESS_THAN ? (u ? insn::kind::KIND_SLTU : insn::kind::KIND_SLT) :
							(ok == binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO ? (u ? insn::kind::KIND_SLEU : insn::kind::KIND_SLE) :
							(ok == binary_expression::operator_kind::KIND_GREATER_THAN ? (u ? insn::kind::KIND_SGTU : insn::kind::KIND_SGT) : (u ? insn::kind::KIND_SGEU : insn::kind::KIND_SGE)));
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, lhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(ik, rw->raw_register_operand(), rw->raw_register_operand(), rhs_op->contained_register_wrapper()->raw_register_operand()));
						free_general_purpose_register(mc, rhs_op->contained_register_wrapper());
						return make_shared<operand_wrapper>(rw);
					}
						break;
					}
				}
				else if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					switch (ok) {
					case binary_expression::operator_kind::KIND_EQUALS_EQUALS:
					case binary_expression::operator_kind::KIND_NOT_EQUALS: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_EQUALS_EQUALS ? insn::kind::KIND_SEQ : insn::kind::KIND_SNE;
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc), rw2 = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, lhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw2->raw_register_operand(), register_file::register_objects::_zero_register, rhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(ik, rw->raw_register_operand(), rw->raw_register_operand(), rw2->raw_register_operand()));
						free_general_purpose_register(mc, rw2);
						return make_shared<operand_wrapper>(rw);
					}
						break;
					case binary_expression::operator_kind::KIND_LESS_THAN: {
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, lhs_op->contained_immediate_operand()));
						insn::kind ik = u ? insn::kind::KIND_SLTIU : insn::kind::KIND_SLTI;
						mc->add_insn(make_shared<insn>(ik, rw->raw_register_operand(), rw->raw_register_operand(), rhs_op->contained_immediate_operand()));
						return make_shared<operand_wrapper>(rw);
					}
						break;
					case binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO:
					case binary_expression::operator_kind::KIND_GREATER_THAN:
					case binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO ? (u ? insn::kind::KIND_SLEU : insn::kind::KIND_SLE) :
							(ok == binary_expression::operator_kind::KIND_GREATER_THAN ? (u ? insn::kind::KIND_SGTU : insn::kind::KIND_SGT) : (u ? insn::kind::KIND_SGEU : insn::kind::KIND_SGE));
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc),
							rw2 = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, lhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw2->raw_register_operand(), register_file::register_objects::_zero_register, rhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(ik, rw->raw_register_operand(), rw->raw_register_operand(), rw2->raw_register_operand()));
						free_general_purpose_register(mc, rw2);
						return make_shared<operand_wrapper>(rw);
					}
						break;
					}
				}
				else
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			else if (lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
				if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					switch (ok) {
					case binary_expression::operator_kind::KIND_EQUALS_EQUALS:
					case binary_expression::operator_kind::KIND_NOT_EQUALS: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_EQUALS_EQUALS ? insn::kind::KIND_SEQ : insn::kind::KIND_SNE;
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, rhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(ik, lhs_op->contained_register_wrapper()->raw_register_operand(), lhs_op->contained_register_wrapper()->raw_register_operand(),
							rw->raw_register_operand()));
						free_general_purpose_register(mc, rw);
						return lhs_op;
					}
						break;
					case binary_expression::operator_kind::KIND_LESS_THAN: {
						insn::kind ik = u ? insn::kind::KIND_SLTIU : insn::kind::KIND_SLTI;
						mc->add_insn(make_shared<insn>(ik, lhs_op->contained_register_wrapper()->raw_register_operand(), lhs_op->contained_register_wrapper()->raw_register_operand(),
							rhs_op->contained_immediate_operand()));
						return lhs_op;
					}
						break;
					case binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO:
					case binary_expression::operator_kind::KIND_GREATER_THAN:
					case binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO ? (u ? insn::kind::KIND_SLEU : insn::kind::KIND_SLE) :
							(ok == binary_expression::operator_kind::KIND_GREATER_THAN ? (u ? insn::kind::KIND_SGTU : insn::kind::KIND_SGT) : (u ? insn::kind::KIND_SGEU : insn::kind::KIND_SGE));
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, rhs_op->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(ik, lhs_op->contained_register_wrapper()->raw_register_operand(), lhs_op->contained_register_wrapper()->raw_register_operand(),
							rw->raw_register_operand()));
						free_general_purpose_register(mc, rw);
						return lhs_op;
					}
						break;
					}
				}
				else if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
					switch (ok) {
					case binary_expression::operator_kind::KIND_EQUALS_EQUALS:
					case binary_expression::operator_kind::KIND_NOT_EQUALS: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_EQUALS_EQUALS ? insn::kind::KIND_SEQ : insn::kind::KIND_SNE;
						mc->add_insn(make_shared<insn>(ik, lhs_op->contained_register_wrapper()->raw_register_operand(), lhs_op->contained_register_wrapper()->raw_register_operand(),
							rhs_op->contained_register_wrapper()->raw_register_operand()));
						free_general_purpose_register(mc, rhs_op->contained_register_wrapper());
						return lhs_op;
					}
						break;
					case binary_expression::operator_kind::KIND_LESS_THAN:
					case binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO:
					case binary_expression::operator_kind::KIND_GREATER_THAN:
					case binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO: {
						insn::kind ik = ok == binary_expression::operator_kind::KIND_LESS_THAN ? (u ? insn::kind::KIND_SLTU : insn::kind::KIND_SLT) :
							(ok == binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO ? (u ? insn::kind::KIND_SLEU : insn::kind::KIND_SLE) :
							(ok == binary_expression::operator_kind::KIND_GREATER_THAN ? (u ? insn::kind::KIND_SGTU : insn::kind::KIND_SGT) : (u ? insn::kind::KIND_SGEU : insn::kind::KIND_SGE)));
						mc->add_insn(make_shared<insn>(ik, lhs_op->contained_register_wrapper()->raw_register_operand(), lhs_op->contained_register_wrapper()->raw_register_operand(),
							rhs_op->contained_register_wrapper()->raw_register_operand()));
						free_general_purpose_register(mc, rhs_op->contained_register_wrapper());
						return lhs_op;
					}
						break;
					}	
				}
				else
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand_wrapper> binary_expression_helper_3(shared_ptr<mips_code> mc, shared_ptr<binary_expression> lhs, binary_expression::operator_kind ok,
			shared_ptr<binary_expression> rhs) {
			shared_ptr<operand_wrapper> lhs_op = generate_binary_expression_mips(mc, lhs, binary_expression::operator_kind::KIND_NONE, nullptr,
				vector<unary_expression::kind> {}, vector<shared_ptr<postfix_expression::postfix_type>>{});
			shared_ptr<operand_wrapper> rhs_op = generate_binary_expression_mips(mc, rhs, binary_expression::operator_kind::KIND_NONE, nullptr,
				vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
			if (lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER &&
				lhs_op->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
				if (lhs_op->contained_register_wrapper()->raw_register_operand()->register_number() > 31)
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				shared_ptr<register_wrapper> to_use_lhs = allocate_general_purpose_register(mc);
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, to_use_lhs->raw_register_operand(), register_file::register_objects::_zero_register,
					lhs_op->contained_register_wrapper()->raw_register_operand()));
				lhs_op = make_shared<operand_wrapper>(to_use_lhs);
			}
			tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> lab_jump = mc->next_label_info(),
				lab_done = mc->next_label_info();
			shared_ptr<register_wrapper> rw = nullptr;
			if (lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
				rw = allocate_general_purpose_register(mc);
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, lhs_op->contained_immediate_operand()));
			}
			else if (lhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
				rw = lhs_op->contained_register_wrapper();
			else {
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return nullptr;
			}
			switch (ok) {
			case binary_expression::operator_kind::KIND_LOGICAL_AND: {
				mc->add_insn(make_shared<insn>(insn::kind::KIND_BEQ, rw->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_jump)));
			}
				break;
			case binary_expression::operator_kind::KIND_LOGICAL_OR: {
				mc->add_insn(make_shared<insn>(insn::kind::KIND_BNE, rw->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_jump)));
			}
				break;
			}
			if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER &&
				rhs_op->contained_register_wrapper()->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
				if (rhs_op->contained_register_wrapper()->raw_register_operand()->register_number() > 31)
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				shared_ptr<register_wrapper> to_use_rhs = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, to_use_rhs->raw_register_operand(), register_file::register_objects::_zero_register,
					rhs_op->contained_register_wrapper()->raw_register_operand()));
				rhs_op = make_shared<operand_wrapper>(to_use_rhs);
			}
			shared_ptr<register_wrapper> rw2 = nullptr;
			if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
				rw2 = allocate_general_purpose_register(mc);
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw2->raw_register_operand(), register_file::register_objects::_zero_register, rhs_op->contained_immediate_operand()));
			}
			else if (rhs_op->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
				rw2 = rhs_op->contained_register_wrapper();
			else {
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return nullptr;
			}
			switch (ok) {
			case binary_expression::operator_kind::KIND_LOGICAL_AND: {
				mc->add_insn(make_shared<insn>(insn::kind::KIND_BEQ, rw2->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_jump)));
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, make_shared<immediate_operand>(1)));
			}
				break;
			case binary_expression::operator_kind::KIND_LOGICAL_OR: {
				mc->add_insn(make_shared<insn>(insn::kind::KIND_BNE, rw2->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_jump)));
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, rw->raw_register_operand(), register_file::register_objects::_zero_register, register_file::register_objects::_zero_register));
			}
				break;
			}
			free_general_purpose_register(mc, rw2);
			mc->add_insn(make_shared<insn>(insn::kind::KIND_J, get<2>(lab_done)));
			mc->add_insn(get<3>(lab_jump));
			switch (ok) {
			case binary_expression::operator_kind::KIND_LOGICAL_AND: {
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, rw->raw_register_operand(), register_file::register_objects::_zero_register,
					register_file::register_objects::_zero_register));
			}
				break;
			case binary_expression::operator_kind::KIND_LOGICAL_OR: {
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register,
					make_shared<immediate_operand>(1)));
			}
				break;
			}
			mc->add_insn(get<3>(lab_done));
			return make_shared<operand_wrapper>(rw);
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
				return "struct" + sep + s->struct_name().raw_text() + "_" + arr;
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
				unsigned int sz = 0;
				for (shared_ptr<symbol> sym : s_scope->symbol_list()) {
					if (sym->symbol_kind() == symbol::kind::KIND_STRUCT) {
						shared_ptr<struct_symbol> ssym = static_pointer_cast<struct_symbol>(sym);
						shared_ptr<type> t = ssym->struct_symbol_type();
						int sz1 = calculate_type_size(mc, t);
						sz += sz1 % 8 == 0 ? sz1 : (sz1 / 8 + 1) * 8;
					}
					else if (sym->symbol_kind() == symbol::kind::KIND_VARIABLE) {
						shared_ptr<variable_symbol> vsym = static_pointer_cast<variable_symbol>(sym);
						shared_ptr<type> t = vsym->variable_type();
						int sz1 = calculate_type_size(mc, t);
						sz += sz1 % 8 == 0 ? sz1 : (sz1 / 8 + 1) * 8;
					}
					else
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				}
				if (sz == 0) sz = 4;
				return sz;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return 0;
		}

		allocation_kind calculate_allocation_strategy(shared_ptr<mips_code> mc, shared_ptr<type> t, bool g) {
			if (t == nullptr) return allocation_kind::KIND_NONE;
			if (g || t->type_static_kind() == type::static_kind::KIND_STATIC) return allocation_kind::KIND_STATIC;
			return allocation_kind::KIND_STACK;
		}

		void generate_variable_declaration_mips(shared_ptr<mips_code> mc, shared_ptr<variable_declaration> vdecl) {
			string sym_string = symbol_2_string(mc, vdecl->variable_declaration_symbol());
			shared_ptr<type> t = vdecl->variable_declaration_type();
			allocation_kind strat = calculate_allocation_strategy(mc, t, vdecl->variable_declaration_symbol()->parent_scope()->scope_kind() == scope::kind::KIND_GLOBAL ||
			vdecl->variable_declaration_symbol()->parent_scope()->scope_kind() == scope::kind::KIND_NAMESPACE);
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
			bool static_variable = t->type_static_kind() == type::static_kind::KIND_STATIC,
				global_variable = vdecl->variable_declaration_symbol()->parent_scope()->scope_kind() == scope::kind::KIND_GLOBAL ||
				vdecl->variable_declaration_symbol()->parent_scope()->scope_kind() == scope::kind::KIND_NAMESPACE;
			if (t->type_kind() == type::kind::KIND_FUNCTION)
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			if (strat == allocation_kind::KIND_STACK && t->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
				int sz = (int) calculate_type_size(mc, t);
				if (t->type_kind() == type::kind::KIND_PRIMITIVE) {
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
					int offset = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
					mc->current_frame()->change_local_stack_offset(-offset);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-offset)));
					shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
					rw->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_ON_STACK);
					rw->set_offset_from_stack_ptr(0);
					rw->set_register_wrapper_variable_kind(register_wrapper::variable_kind::KIND_VARIABLE);
					mc->current_frame()->add_variable_id(sym_string, rw->wrapper_id());
					mc->current_frame()->unmark_register(rw->raw_register_operand()->register_number());
					if (vdecl->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_PRESENT) {
						shared_ptr<operand_wrapper> ow = generate_assignment_expression_mips(mc, vdecl->initialization(), binary_expression::operator_kind::KIND_NONE, nullptr,
							vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
						shared_ptr<register_wrapper> temp = nullptr;
						if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
							temp = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, temp->raw_register_operand(), register_file::register_objects::_zero_register, ow->contained_immediate_operand()));
						}
						else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
							temp = ow->contained_register_wrapper();
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
							if (temp->raw_register_operand()->register_number() > 31) {
								if (temp->double_precision()) {
									shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_D, frw->raw_register_operand(), temp->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, frw->raw_register_operand(), make_shared<immediate_operand>(rw->offset_from_stack_ptr()), sp));
									free_general_purpose_register(mc, frw);
								}
								else
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, temp->raw_register_operand(), make_shared<immediate_operand>(rw->offset_from_stack_ptr()), sp));
							}
							else {
								shared_ptr<register_wrapper> ftemp = allocate_general_purpose_fp_register(mc);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, temp->raw_register_operand(), ftemp->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, ftemp->raw_register_operand(), ftemp->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, ftemp->raw_register_operand(), make_shared<immediate_operand>(rw->offset_from_stack_ptr()), sp));
								free_general_purpose_register(mc, ftemp);
							}
							free_general_purpose_register(mc, temp);
						}
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
							if (temp->raw_register_operand()->register_number() > 31) {
								if (!temp->double_precision()) {
									shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, frw->raw_register_operand(), temp->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, frw->raw_register_operand(), make_shared<immediate_operand>(rw->offset_from_stack_ptr()), sp));
									free_general_purpose_register(mc, frw);
								}
								else
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, temp->raw_register_operand(), make_shared<immediate_operand>(rw->offset_from_stack_ptr()), sp));
							}
							else {
								shared_ptr<register_wrapper> ftemp = allocate_general_purpose_fp_register(mc);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, temp->raw_register_operand(), ftemp->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, ftemp->raw_register_operand(), ftemp->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, ftemp->raw_register_operand(), make_shared<immediate_operand>(rw->offset_from_stack_ptr()), sp));
								free_general_purpose_register(mc, ftemp);
							}
							free_general_purpose_register(mc, temp);
						}
						else {
							if (temp->raw_register_operand()->register_number() > 31) {
								shared_ptr<register_wrapper> temp2 = allocate_general_purpose_register(mc);
								if(temp->double_precision())
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_D, temp->raw_register_operand(), temp->raw_register_operand()));
								else
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_S, temp->raw_register_operand(), temp->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MFC1, temp2->raw_register_operand(), temp->raw_register_operand()));
								if (temp->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
									if(temp->double_precision())
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, temp->raw_register_operand(), temp->raw_register_operand()));
									else
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, temp->raw_register_operand(), temp->raw_register_operand()));
								}
								else
									free_general_purpose_register(mc, temp);
								temp = temp2;
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, temp->raw_register_operand(), make_shared<immediate_operand>(rw->offset_from_stack_ptr()), sp));
							}
							else {
								if (sz == 4)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, temp->raw_register_operand(), make_shared<immediate_operand>(rw->offset_from_stack_ptr()), sp));
								else if (sz == 2)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SH, temp->raw_register_operand(), make_shared<immediate_operand>(rw->offset_from_stack_ptr()), sp));
								else if (sz == 1)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SB, temp->raw_register_operand(), make_shared<immediate_operand>(rw->offset_from_stack_ptr()), sp));
							}
							free_general_purpose_register(mc, temp);
						}
					}
					mc->current_frame()->restore_s_registers(mc);
					return;
				}
				else if (t->type_kind() == type::kind::KIND_STRUCT) {
					int offset = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
					mc->current_frame()->change_local_stack_offset(-offset);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-offset)));
					shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
					rw->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_ON_STACK);
					rw->set_offset_from_stack_ptr(0);
					rw->set_register_wrapper_variable_kind(register_wrapper::variable_kind::KIND_VARIABLE);
					mc->current_frame()->add_variable_id(sym_string, rw->wrapper_id());
					mc->current_frame()->unmark_register(rw->raw_register_operand()->register_number());
					if (vdecl->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_PRESENT) {
						shared_ptr<operand_wrapper> ow = generate_assignment_expression_mips(mc, vdecl->initialization(), binary_expression::operator_kind::KIND_NONE, nullptr,
							vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
						shared_ptr<register_wrapper> temp = nullptr;
						if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
							temp = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, temp->raw_register_operand(), register_file::register_objects::_zero_register, ow->contained_immediate_operand()));
						}
						else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
							temp = ow->contained_register_wrapper();
						shared_ptr<register_wrapper> loop_iterator = allocate_general_purpose_register(mc);
						for (int curr = 0; curr < offset; curr += 4) {
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, loop_iterator->raw_register_operand(), make_shared<immediate_operand>(curr),
								temp->raw_register_operand()));
							mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, loop_iterator->raw_register_operand(), make_shared<immediate_operand>(curr), sp));
						}
						free_general_purpose_register(mc, loop_iterator);
						free_general_purpose_register(mc, temp);
					}
					mc->current_frame()->restore_s_registers(mc);
					return;
				}
				else
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			else if (strat == allocation_kind::KIND_STATIC && t->type_array_kind() == type::array_kind::KIND_NON_ARRAY) {
				int sz = (int) calculate_type_size(mc, t);
				string sym_name = symbol_2_string(mc, vdecl->variable_declaration_symbol());
				shared_ptr<label_operand> sym_name_lo;
				tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> stat_lo;
				if (static_variable && !global_variable) {
					stat_lo = mc->next_label_info();
					string initialization_check = sym_name + "_initialization_check";
					sym_name_lo = make_shared<label_operand>(initialization_check);
					shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
					mc->add_data_directive(make_shared<data_directive>(true, sym_name_lo, 8), false);
					mc->add_insn_begin(make_shared<insn>(insn::kind::KIND_SW, register_file::register_objects::_t0_register, sym_name_lo));
					mc->add_insn_begin(make_shared<insn>(insn::kind::KIND_ADDIU, register_file::register_objects::_t0_register, register_file::register_objects::_zero_register,
						make_shared<immediate_operand>(1)));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, rw->raw_register_operand(), sym_name_lo));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_BEQ, rw->raw_register_operand(), register_file::register_objects::_zero_register,
						get<2>(stat_lo)));
					free_general_purpose_register(mc, rw);
				}
				if (t->type_kind() == type::kind::KIND_PRIMITIVE) {
					shared_ptr<label_operand> lo = make_shared<label_operand>(sym_name);
					mc->add_data_directive(make_shared<data_directive>(true, lo, (sz % 8 ==0  ? sz : (sz / 8 + 1) * 8)), !static_variable);
					shared_ptr<register_wrapper> rw = mc->current_frame()->generate_register_wrapper(register_file::register_objects::_gp_register);
					rw->set_identifier(lo);
					rw->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_GLOBAL);
					rw->set_offset_from_stack_ptr(0);
					rw->set_register_wrapper_variable_kind(register_wrapper::variable_kind::KIND_VARIABLE);
					mc->current_frame()->add_variable_id(sym_name, rw->wrapper_id());
					mc->current_frame()->unmark_register(rw->raw_register_operand()->register_number());
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
					if (vdecl->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_PRESENT) {
						shared_ptr<operand_wrapper> ow = generate_assignment_expression_mips(mc, vdecl->initialization(), binary_expression::operator_kind::KIND_NONE, nullptr,
							vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
						shared_ptr<register_wrapper> temp = nullptr;
						if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
							temp = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, temp->raw_register_operand(), register_file::register_objects::_zero_register, ow->contained_immediate_operand()));
						}
						else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
							temp = ow->contained_register_wrapper();
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
							if (temp->raw_register_operand()->register_number() > 31) {
								if (temp->double_precision()) {
									shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_D, frw->raw_register_operand(), temp->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, frw->raw_register_operand(), lo));
									free_general_purpose_register(mc, frw);
								}
								else
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, temp->raw_register_operand(), lo));
							}
							else {
								shared_ptr<register_wrapper> ftemp = allocate_general_purpose_fp_register(mc);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, temp->raw_register_operand(), ftemp->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, ftemp->raw_register_operand(), ftemp->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SWC1, ftemp->raw_register_operand(), lo));
								free_general_purpose_register(mc, ftemp);
							}
						}
						else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) {
							if (temp->raw_register_operand()->register_number() > 31) {
								if (!temp->double_precision()) {
									shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, frw->raw_register_operand(), temp->raw_register_operand()));
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, frw->raw_register_operand(), lo));
									free_general_purpose_register(mc, frw);
								}
								else
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, temp->raw_register_operand(), lo));
							}
							else {
								shared_ptr<register_wrapper> ftemp = allocate_general_purpose_fp_register(mc);
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, temp->raw_register_operand(), ftemp->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, ftemp->raw_register_operand(), ftemp->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SDC1, ftemp->raw_register_operand(), lo));
								free_general_purpose_register(mc, ftemp);
							}
						}
						else {
							if (temp->raw_register_operand()->register_number() > 31) {
								shared_ptr<register_wrapper> temp2 = allocate_general_purpose_register(mc);
								if (temp->double_precision())
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_D, temp->raw_register_operand(), temp->raw_register_operand()));
								else
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_S, temp->raw_register_operand(), temp->raw_register_operand()));
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MFC1, temp2->raw_register_operand(), temp->raw_register_operand()));
								if (temp->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
									if(temp->double_precision())
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, temp->raw_register_operand(), temp->raw_register_operand()));
									else
										mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, temp->raw_register_operand(), temp->raw_register_operand()));
								}
								else
									free_general_purpose_register(mc, temp);
								temp = temp2;
								mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, temp->raw_register_operand(), lo));
							}
							else {
								if (sz == 4)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, temp->raw_register_operand(), lo));
								else if (sz == 2)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SH, temp->raw_register_operand(), lo));
								else if (sz == 1)
									mc->add_insn(make_shared<insn>(insn::kind::KIND_SB, temp->raw_register_operand(), lo));
							}
						}
						free_general_purpose_register(mc, temp);
					}
					mc->current_frame()->restore_s_registers(mc);
				}
				else if (t->type_kind() == type::kind::KIND_STRUCT) {
					int actual_size = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
					shared_ptr<label_operand> lo = make_shared<label_operand>(sym_name);
					mc->add_data_directive(make_shared<data_directive>(true, lo, actual_size), !static_variable);
					shared_ptr<register_wrapper> rw = mc->current_frame()->generate_register_wrapper(register_file::register_objects::_gp_register);
					rw->set_identifier(lo);
					rw->set_register_wrapper_variable_kind(register_wrapper::variable_kind::KIND_VARIABLE);
					rw->set_offset_from_stack_ptr(0);
					rw->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_GLOBAL);
					mc->current_frame()->add_variable_id(sym_name, rw->wrapper_id());
					mc->current_frame()->unmark_register(rw->raw_register_operand()->register_number());
					if (vdecl->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_PRESENT) {
						shared_ptr<operand_wrapper> ow = generate_assignment_expression_mips(mc, vdecl->initialization(), binary_expression::operator_kind::KIND_NONE, nullptr,
							vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
						shared_ptr<register_wrapper> temp = nullptr;
						if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
							temp = allocate_general_purpose_register(mc);
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, temp->raw_register_operand(), register_file::register_objects::_zero_register, ow->contained_immediate_operand()));
						}
						else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
							temp = ow->contained_register_wrapper();
						shared_ptr<register_wrapper> ref = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_LA, ref->raw_register_operand(), lo));
						shared_ptr<register_wrapper> loop_iterator = allocate_general_purpose_register(mc);
						for (int curr = 0; curr < actual_size; curr += 4) {
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, loop_iterator->raw_register_operand(), make_shared<immediate_operand>(curr),
								temp->raw_register_operand()));
							mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, loop_iterator->raw_register_operand(), make_shared<immediate_operand>(curr), ref->raw_register_operand()));
						}
						free_general_purpose_register(mc, loop_iterator);
						free_general_purpose_register(mc, temp);
						free_general_purpose_register(mc, ref);
					}
					mc->current_frame()->restore_s_registers(mc);
				}
				if (static_variable && !global_variable) {
					mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, register_file::register_objects::_zero_register, sym_name_lo));
					mc->add_insn(get<3>(stat_lo));
				}
				return;
			}
			else {
				if (t->type_array_kind() != type::array_kind::KIND_ARRAY)
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				string sym_name = symbol_2_string(mc, vdecl->variable_declaration_symbol());
				int sz = (int) calculate_type_size(mc, t);
				shared_ptr<register_wrapper> rw = nullptr;
				shared_ptr<label_operand> lo = nullptr;
				shared_ptr<label_operand> sym_name_lo;
				tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> stat_lo;
				if (strat == allocation_kind::KIND_STACK) {
					int offset = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
					rw = allocate_general_purpose_register(mc);
					mc->current_frame()->change_local_stack_offset(-offset);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-offset)));
					rw->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_ON_STACK);
					rw->set_offset_from_stack_ptr(0);
					rw->set_register_wrapper_variable_kind(register_wrapper::variable_kind::KIND_VARIABLE);
					mc->current_frame()->add_variable_id(sym_string, rw->wrapper_id());
					mc->current_frame()->unmark_register(rw->raw_register_operand()->register_number());
				}
				else {
					if (static_variable && !global_variable) {
						stat_lo = mc->next_label_info();
						string initialization_check = sym_name + "_initialization_check";
						sym_name_lo = make_shared<label_operand>(initialization_check);
						shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
						mc->add_data_directive(make_shared<data_directive>(true, sym_name_lo, 8), false);
						mc->add_insn_begin(make_shared<insn>(insn::kind::KIND_SW, register_file::register_objects::_t0_register, sym_name_lo));
						mc->add_insn_begin(make_shared<insn>(insn::kind::KIND_ADDIU, register_file::register_objects::_t0_register, register_file::register_objects::_zero_register,
							make_shared<immediate_operand>(1)));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_LW, rw->raw_register_operand(), sym_name_lo));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_BEQ, rw->raw_register_operand(), register_file::register_objects::_zero_register,
							get<2>(stat_lo)));
						free_general_purpose_register(mc, rw);
					}
					int actual_size = sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
					lo = make_shared<label_operand>(sym_name);
					mc->add_data_directive(make_shared<data_directive>(true, lo, actual_size), !static_variable);
					rw = mc->current_frame()->generate_register_wrapper(register_file::register_objects::_gp_register);
					rw->set_identifier(lo);
					rw->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_GLOBAL);
					rw->set_offset_from_stack_ptr(0);
					rw->set_register_wrapper_variable_kind(register_wrapper::variable_kind::KIND_VARIABLE);
					mc->current_frame()->add_variable_id(sym_name, rw->wrapper_id());
					mc->current_frame()->unmark_register(rw->raw_register_operand()->register_number());
				}
				if (vdecl->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_PRESENT) {
					shared_ptr<operand_wrapper> ow = generate_assignment_expression_mips(mc, vdecl->initialization(), binary_expression::operator_kind::KIND_NONE, nullptr,
						vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
					shared_ptr<register_wrapper> temp = nullptr;
					if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
						temp = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, temp->raw_register_operand(), register_file::register_objects::_zero_register, ow->contained_immediate_operand()));
					}
					else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
						temp = ow->contained_register_wrapper();
					else
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (strat == allocation_kind::KIND_STACK)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, temp->raw_register_operand(), make_shared<immediate_operand>(rw->offset_from_stack_ptr()), sp));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, temp->raw_register_operand(), lo));
					free_general_purpose_register(mc, temp);
				}
				if (static_variable && !global_variable) {
					mc->add_insn(make_shared<insn>(insn::kind::KIND_SW, register_file::register_objects::_zero_register, sym_name_lo));
					mc->add_insn(get<3>(stat_lo));
				}
				mc->current_frame()->restore_s_registers(mc);
				return;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
		}

		void generate_if_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<if_stmt> istmt) {
			if (istmt->if_stmt_init_decl_kind() == if_stmt::init_decl_kind::KIND_INIT_DECL_PRESENT) {
				vector<shared_ptr<variable_declaration>> vdecl_list = istmt->init_decl_list();
				for (shared_ptr<variable_declaration> vdecl : vdecl_list)
					generate_variable_declaration_mips(mc, vdecl);
			}
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
			shared_ptr<expression> cond = istmt->condition();
			shared_ptr<stmt> tstmt = istmt->true_path(), fstmt = istmt->false_path();
			shared_ptr<operand_wrapper> ow_cond = generate_expression_mips(mc, cond, binary_expression::operator_kind::KIND_NONE, nullptr,
				vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
			shared_ptr<register_wrapper> rw = nullptr;
			tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> lab_false_path = mc->next_label_info(),
				lab_done = mc->next_label_info();
			if (ow_cond->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
				rw = allocate_general_purpose_register(mc);
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, ow_cond->contained_immediate_operand()));
			}
			else if (ow_cond->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
				rw = ow_cond->contained_register_wrapper();
			mc->add_insn(make_shared<insn>(insn::kind::KIND_BEQ, rw->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_false_path)));
			int pre_off = mc->current_frame()->local_stack_offset();
			generate_stmt_mips(mc, tstmt);
			int post_off = mc->current_frame()->local_stack_offset();
			if (pre_off != post_off) {
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(pre_off - post_off)));
				mc->current_frame()->change_local_stack_offset(pre_off - post_off);
			}
			mc->add_insn(make_shared<insn>(insn::kind::KIND_J, get<2>(lab_done)));
			mc->add_insn(get<3>(lab_false_path));
			pre_off = mc->current_frame()->local_stack_offset();
			generate_stmt_mips(mc, fstmt);
			post_off = mc->current_frame()->local_stack_offset();
			if (pre_off != post_off) {
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(pre_off - post_off)));
				mc->current_frame()->change_local_stack_offset(pre_off - post_off);
			}
			mc->add_insn(get<3>(lab_done));
			free_general_purpose_register(mc, rw);
			mc->current_frame()->restore_s_registers(mc);
		}

		binary_expression::operator_kind convert_from_assignment_operator(shared_ptr<mips_code> mc, binary_expression::operator_kind op) {
			switch (op) {
			case binary_expression::operator_kind::KIND_ADD_EQUALS:
				return binary_expression::operator_kind::KIND_ADD;
				break;
			case binary_expression::operator_kind::KIND_BITWISE_AND_EQUALS:
				return binary_expression::operator_kind::KIND_BITWISE_AND;
				break;
			case binary_expression::operator_kind::KIND_BITWISE_OR_EQUALS:
				return binary_expression::operator_kind::KIND_BITWISE_OR;
				break;
			case binary_expression::operator_kind::KIND_BITWISE_XOR_EQUALS:
				return binary_expression::operator_kind::KIND_BITWISE_XOR;
				break;
			case binary_expression::operator_kind::KIND_DIVIDE_EQUALS:
				return binary_expression::operator_kind::KIND_DIVIDE;
				break;
			case binary_expression::operator_kind::KIND_LOGICAL_AND_EQUALS:
				return binary_expression::operator_kind::KIND_LOGICAL_AND;
				break;
			case binary_expression::operator_kind::KIND_LOGICAL_OR_EQUALS:
				return binary_expression::operator_kind::KIND_LOGICAL_OR;
				break;
			case binary_expression::operator_kind::KIND_MODULUS_EQUALS:
				return binary_expression::operator_kind::KIND_MODULUS;
				break;
			case binary_expression::operator_kind::KIND_MULTIPLY_EQUALS:
				return binary_expression::operator_kind::KIND_MULTIPLY;
				break;
			case binary_expression::operator_kind::KIND_SHIFT_LEFT_EQUALS:
				return binary_expression::operator_kind::KIND_SHIFT_LEFT;
				break;
			case binary_expression::operator_kind::KIND_SHIFT_RIGHT_EQUALS:
				return binary_expression::operator_kind::KIND_SHIFT_RIGHT;
				break;
			case binary_expression::operator_kind::KIND_SUBTRACT_EQUALS:
				return binary_expression::operator_kind::KIND_SUBTRACT;
				break;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return binary_expression::operator_kind::KIND_NONE;
		}

		shared_ptr<operand_wrapper> handle_primitive_assignment_operation(shared_ptr<mips_code> mc, shared_ptr<operand_wrapper> op1, binary_expression::operator_kind ok,
			shared_ptr<operand_wrapper> op2) {
			binary_expression::operator_kind converted = binary_expression::operator_kind::KIND_NONE;
			switch (ok) {
			case binary_expression::operator_kind::KIND_ADD_EQUALS:
				converted = binary_expression::operator_kind::KIND_ADD;
				break;
			case binary_expression::operator_kind::KIND_BITWISE_AND_EQUALS:
				converted = binary_expression::operator_kind::KIND_BITWISE_AND;
				break;
			case binary_expression::operator_kind::KIND_BITWISE_OR_EQUALS:
				converted = binary_expression::operator_kind::KIND_BITWISE_OR;
				break;
			case binary_expression::operator_kind::KIND_BITWISE_XOR_EQUALS:
				converted = binary_expression::operator_kind::KIND_BITWISE_XOR;
				break;
			case binary_expression::operator_kind::KIND_DIVIDE_EQUALS:
				converted = binary_expression::operator_kind::KIND_DIVIDE;
				break;
			case binary_expression::operator_kind::KIND_MODULUS_EQUALS:
				converted = binary_expression::operator_kind::KIND_MODULUS;
				break;
			case binary_expression::operator_kind::KIND_MULTIPLY_EQUALS:
				converted = binary_expression::operator_kind::KIND_MULTIPLY;
				break;
			case binary_expression::operator_kind::KIND_SHIFT_LEFT_EQUALS:
				converted = binary_expression::operator_kind::KIND_SHIFT_LEFT;
				break;
			case binary_expression::operator_kind::KIND_SHIFT_RIGHT_EQUALS:
				converted = binary_expression::operator_kind::KIND_SHIFT_RIGHT;
				break;
			case binary_expression::operator_kind::KIND_SUBTRACT_EQUALS:
				converted = binary_expression::operator_kind::KIND_SUBTRACT;
				break;
			case binary_expression::operator_kind::KIND_LOGICAL_AND_EQUALS:
				converted = binary_expression::operator_kind::KIND_LOGICAL_AND;
				break;
			case binary_expression::operator_kind::KIND_LOGICAL_OR_EQUALS:
				converted = binary_expression::operator_kind::KIND_LOGICAL_OR;
				break;
			}
			switch (converted) {
				case binary_expression::operator_kind::KIND_ADD:
				case binary_expression::operator_kind::KIND_BITWISE_AND:
				case binary_expression::operator_kind::KIND_BITWISE_OR:
				case binary_expression::operator_kind::KIND_BITWISE_XOR:
				case binary_expression::operator_kind::KIND_DIVIDE:
				case binary_expression::operator_kind::KIND_MODULUS:
				case binary_expression::operator_kind::KIND_MULTIPLY:
				case binary_expression::operator_kind::KIND_SHIFT_LEFT:
				case binary_expression::operator_kind::KIND_SHIFT_RIGHT:
				case binary_expression::operator_kind::KIND_SUBTRACT:
					return binary_expression_helper_1(mc, op1, converted, op2);
					break;
				case binary_expression::operator_kind::KIND_LOGICAL_AND:
				case binary_expression::operator_kind::KIND_LOGICAL_OR: {
					shared_ptr<register_wrapper> rw_op1 = nullptr, rw_op2 = nullptr;
					tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> lab_jump = mc->next_label_info(),
						lab_done = mc->next_label_info();
					if (op1->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
						load_register_wrapper_from_stack(mc, op1->contained_register_wrapper());
						rw_op1 = op1->contained_register_wrapper();
					}
					else if (op1->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
						rw_op1 = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw_op1->raw_register_operand(), register_file::register_objects::_zero_register, op1->contained_immediate_operand()));
					}
					else
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (op2->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
						load_register_wrapper_from_stack(mc, op2->contained_register_wrapper());
						rw_op2 = op2->contained_register_wrapper();
					}
					else if (op2->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
						rw_op2 = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw_op2->raw_register_operand(), register_file::register_objects::_zero_register, op2->contained_immediate_operand()));
					}
					else
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					switch (converted) {
					case binary_expression::operator_kind::KIND_LOGICAL_AND:
						mc->add_insn(make_shared<insn>(insn::kind::KIND_BEQ, rw_op1->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_jump)));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_BEQ, rw_op2->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_jump)));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw_op1->raw_register_operand(), register_file::register_objects::_zero_register,
							make_shared<immediate_operand>(1)));
						break;
					case binary_expression::operator_kind::KIND_LOGICAL_OR:
						mc->add_insn(make_shared<insn>(insn::kind::KIND_BNE, rw_op1->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_jump)));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_BNE, rw_op2->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_jump)));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, rw_op1->raw_register_operand(), register_file::register_objects::_zero_register,
							register_file::register_objects::_zero_register));
						break;
					}
					mc->add_insn(make_shared<insn>(insn::kind::KIND_J, get<2>(lab_done)));
					mc->add_insn(get<3>(lab_jump));
					switch (converted) {
					case binary_expression::operator_kind::KIND_LOGICAL_AND:
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, rw_op1->raw_register_operand(), register_file::register_objects::_zero_register,
							register_file::register_objects::_zero_register));
						break;
					case binary_expression::operator_kind::KIND_LOGICAL_OR:
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw_op1->raw_register_operand(), register_file::register_objects::_zero_register,
							make_shared<immediate_operand>(1)));
						break;
					}
					mc->add_insn(get<3>(lab_done));
					free_general_purpose_register(mc, rw_op2);
					return make_shared<operand_wrapper>(rw_op1);
				}
					break;
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		void generate_while_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<while_stmt> wstmt) {
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
			shared_ptr<expression> cond = wstmt->condition();
			tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> lab_begin = mc->next_label_info(),
				lab_end = mc->next_label_info();
			mc->add_insn(get<3>(lab_begin));
			mc->push_loop_begin_label_operand(get<2>(lab_begin));
			mc->push_loop_end_label_operand(get<2>(lab_end));
			shared_ptr<operand_wrapper> ow = generate_expression_mips(mc, cond, binary_expression::operator_kind::KIND_NONE, nullptr, vector<unary_expression::kind>{},
				vector<shared_ptr<postfix_expression::postfix_type>>{});
			shared_ptr<register_wrapper> to_use = nullptr;
			if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
				to_use = allocate_general_purpose_register(mc);
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, to_use->raw_register_operand(), register_file::register_objects::_zero_register, ow->contained_immediate_operand()));
			}
			else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
				to_use = ow->contained_register_wrapper();
			mc->add_insn(make_shared<insn>(insn::kind::KIND_BEQ, to_use->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_end)));
			int pre_off = mc->current_frame()->local_stack_offset();
			mc->push_initial_loop_offset(pre_off);
			generate_stmt_mips(mc, wstmt->while_body());
			mc->pop_initial_loop_offset();
			int post_off = mc->current_frame()->local_stack_offset();
			if (pre_off != post_off) {
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(pre_off - post_off)));
				mc->current_frame()->change_local_stack_offset(pre_off - post_off);
			}
			mc->pop_loop_begin_label_operand();
			mc->pop_loop_end_label_operand();
			mc->add_insn(make_shared<insn>(insn::kind::KIND_J, get<2>(lab_begin)));
			mc->add_insn(get<3>(lab_end));
			free_general_purpose_register(mc, to_use);
			mc->current_frame()->restore_s_registers(mc);
		}

		void generate_block_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<block_stmt> bstmt) {
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
			int pre_off = mc->current_frame()->local_stack_offset();
			for (shared_ptr<stmt> s : bstmt->stmt_list())
				generate_stmt_mips(mc, s);
			int post_off = mc->current_frame()->local_stack_offset();
			if (pre_off != post_off) {
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(pre_off - post_off)));
				mc->current_frame()->change_local_stack_offset(pre_off - post_off);
			}
		}

		void generate_break_continue_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<break_continue_stmt> bcstmt) {
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
			if(mc->current_initial_loop_offset() != -1 && mc->current_initial_loop_offset() - mc->current_frame()->local_stack_offset() != 0)
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(mc->current_initial_loop_offset() - mc->current_frame()->local_stack_offset())));
			if (bcstmt->break_continue_stmt_kind() == break_continue_stmt::kind::KIND_BREAK)
				mc->add_insn(make_shared<insn>(insn::kind::KIND_J, mc->current_loop_end_label_operand()));
			else if (bcstmt->break_continue_stmt_kind() == break_continue_stmt::kind::KIND_CONTINUE)
				mc->add_insn(make_shared<insn>(insn::kind::KIND_J, mc->current_loop_begin_label_operand()));
			else
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
		}

		void generate_do_while_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<while_stmt> dwstmt) {
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
			tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> lab_begin = mc->next_label_info(),
				lab_done = mc->next_label_info();
			mc->push_loop_begin_label_operand(get<2>(lab_begin));
			mc->push_loop_end_label_operand(get<2>(lab_done));
			mc->add_insn(get<3>(lab_begin));
			int pre_off = mc->current_frame()->local_stack_offset();
			mc->push_initial_loop_offset(pre_off);
			generate_stmt_mips(mc, dwstmt->while_body());
			mc->pop_initial_loop_offset();
			int post_off = mc->current_frame()->local_stack_offset();
			if (pre_off != post_off) {
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(pre_off - post_off)));
				mc->current_frame()->change_local_stack_offset(pre_off - post_off);
			}
			shared_ptr<operand_wrapper> ow = generate_expression_mips(mc, dwstmt->condition(), binary_expression::operator_kind::KIND_NONE, nullptr, vector<unary_expression::kind>{},
				vector<shared_ptr<postfix_expression::postfix_type>>{});
			shared_ptr<register_wrapper> rw = nullptr;
			if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
				rw = allocate_general_purpose_register(mc);
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, ow->contained_immediate_operand()));
			}
			else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
				rw = ow->contained_register_wrapper();
			else
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			mc->add_insn(make_shared<insn>(insn::kind::KIND_BNE, rw->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_begin)));
			mc->add_insn(get<3>(lab_done));
			mc->pop_loop_begin_label_operand();
			mc->pop_loop_end_label_operand();
			free_general_purpose_register(mc, rw);
			mc->current_frame()->restore_s_registers(mc);
		}

		void generate_for_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<for_stmt> fstmt) {
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
			tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> lab_begin = mc->next_label_info(),
				lab_end = mc->next_label_info();
			if (fstmt->for_stmt_initializer_present_kind() == for_stmt::initializer_present_kind::INITIALIZER_PRESENT)
				for (shared_ptr<variable_declaration> vdecl : fstmt->initializer())
					generate_variable_declaration_mips(mc, vdecl);
			mc->push_loop_begin_label_operand(get<2>(lab_begin));
			mc->push_loop_end_label_operand(get<2>(lab_end));
			mc->add_insn(get<3>(lab_begin));
			shared_ptr<register_wrapper> to_use = nullptr;
			if (fstmt->for_stmt_condition_present_kind() == for_stmt::condition_present_kind::CONDITION_PRESENT) {
				shared_ptr<operand_wrapper> ow = generate_expression_mips(mc, fstmt->condition(), binary_expression::operator_kind::KIND_NONE, nullptr, vector<unary_expression::kind>{},
					vector<shared_ptr<postfix_expression::postfix_type>>{});
				if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					to_use = allocate_general_purpose_register(mc);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, to_use->raw_register_operand(), register_file::register_objects::_zero_register, ow->contained_immediate_operand()));
				}
				else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
					to_use = ow->contained_register_wrapper();
				else
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				mc->add_insn(make_shared<insn>(insn::kind::KIND_BEQ, to_use->raw_register_operand(), register_file::register_objects::_zero_register, get<2>(lab_end)));
			}
			int pre_off = mc->current_frame()->local_stack_offset();
			mc->push_initial_loop_offset(pre_off);
			generate_stmt_mips(mc, fstmt->for_stmt_body());
			mc->pop_initial_loop_offset();
			if (fstmt->for_stmt_update_present_kind() == for_stmt::update_present_kind::UPDATE_PRESENT) {
				shared_ptr<operand_wrapper> update_ow = generate_expression_mips(mc, fstmt->update(), binary_expression::operator_kind::KIND_NONE, nullptr, vector<unary_expression::kind>{},
					vector<shared_ptr<postfix_expression::postfix_type>>{});
				if (update_ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
					free_general_purpose_register(mc, update_ow->contained_register_wrapper());
			}
			int post_off = mc->current_frame()->local_stack_offset();
			if (pre_off != post_off) {
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADD, sp, sp, make_shared<immediate_operand>(pre_off - post_off)));
				mc->current_frame()->change_local_stack_offset(pre_off - post_off);
			}
			mc->add_insn(make_shared<insn>(insn::kind::KIND_J, get<2>(lab_begin)));
			mc->add_insn(get<3>(lab_end));
			mc->pop_loop_begin_label_operand();
			mc->pop_loop_end_label_operand();
			if (fstmt->for_stmt_condition_present_kind() == for_stmt::condition_present_kind::CONDITION_PRESENT)
				free_general_purpose_register(mc, to_use);
			mc->current_frame()->restore_s_registers(mc);
		}

		void generate_switch_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<switch_stmt> sstmt) {
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
			shared_ptr<expression> expr = sstmt->parent_expression();
			shared_ptr<operand_wrapper> ow = generate_expression_mips(mc, expr, binary_expression::operator_kind::KIND_NONE, nullptr,
				vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
			shared_ptr<register_wrapper> comp = nullptr;
			if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
				comp = allocate_general_purpose_register(mc);
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, comp->raw_register_operand(), register_file::register_objects::_zero_register, ow->contained_immediate_operand()));
			}
			else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
				comp = ow->contained_register_wrapper();
			else
				mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> lab_end = mc->next_label_info(),
				default_lab = make_tuple(-1, "", nullptr, nullptr);
			mc->push_loop_end_label_operand(get<2>(lab_end));
			vector<shared_ptr<register_wrapper>> vec;
			for (shared_ptr<case_default_stmt> cdstmt : sstmt->switch_stmt_scope()->case_default_stmt_list()) {
				tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> clab = mc->generate_case_label_info(cdstmt->case_default_stmt_label_number());
				shared_ptr<register_wrapper> c_rw = nullptr;
				if (cdstmt->case_default_stmt_kind() == case_default_stmt::kind::KIND_CASE) {
					shared_ptr<expression> c_expr = cdstmt->case_expression();
					shared_ptr<operand_wrapper> c_ow = generate_expression_mips(mc, c_expr, binary_expression::operator_kind::KIND_NONE, nullptr,
						vector<unary_expression::kind>{}, vector<shared_ptr<postfix_expression::postfix_type>>{});
					if (c_ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
						c_rw = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, c_rw->raw_register_operand(), register_file::register_objects::_zero_register, c_ow->contained_immediate_operand()));
					}
					else if (c_ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER)
						c_rw = c_ow->contained_register_wrapper();
					else
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					vec.push_back(c_rw);
					load_register_wrapper_from_stack(mc, comp);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_BEQ, c_rw->raw_register_operand(), comp->raw_register_operand(), get<2>(clab)));
				}
				else if (cdstmt->case_default_stmt_kind() == case_default_stmt::kind::KIND_DEFAULT)
					default_lab = mc->generate_case_label_info(cdstmt->case_default_stmt_label_number());
				else
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			if (get<0>(default_lab) != -1)
				mc->add_insn(make_shared<insn>(insn::kind::KIND_J, get<2>(default_lab)));
			int pre_off = mc->current_frame()->local_stack_offset();
			generate_stmt_mips(mc, sstmt->switch_stmt_body());
			int post_off = mc->current_frame()->local_stack_offset();
			mc->add_insn(get<3>(lab_end));
			if (pre_off != post_off) {
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(pre_off - post_off)));
				mc->current_frame()->change_local_stack_offset(pre_off - post_off);
			}
			mc->pop_loop_end_label_operand();
			for (shared_ptr<register_wrapper> r : vec)
				free_general_purpose_register(mc, r);
			free_general_purpose_register(mc, comp);
			mc->current_frame()->restore_s_registers(mc);
		}

		void generate_case_default_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<case_default_stmt> cdstmt) {
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
			tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> cd_lab = mc->generate_case_label_info(cdstmt->case_default_stmt_label_number());
			mc->add_insn(get<3>(cd_lab));
			generate_stmt_mips(mc, cdstmt->case_default_stmt_body());
		}

		void generate_function_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<function_stmt> fstmt) {
			if (fstmt->function_stmt_defined_kind() == function_stmt::defined_kind::KIND_DECLARATION) return;
			shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
			tuple<int, string, shared_ptr<label_operand>, shared_ptr<insn>> lab = mc->next_label_info();
			string fsym = "";
			bool main_function = is_function_main(mc, fstmt->function_stmt_symbol());
			if (main_function)
				fsym = "main";
			else
				fsym = symbol_2_string(mc, fstmt->function_stmt_symbol());
			if (fstmt->function_stmt_symbol()->function_symbol_type()->type_static_kind() == type::static_kind::KIND_NON_STATIC)
				mc->add_to_global_symbol_table(make_shared<label_operand>(fsym));
			mc->add_insn(make_shared<insn>());
			if (!main_function)
				mc->add_insn(make_shared<insn>(insn::kind::KIND_J, get<2>(lab)));
			mc->add_insn(make_shared<insn>(fsym));
			mc->add_frame(mc->generate_new_frame(main_function));
			mc->current_frame()->mark_register(register_file::register_2_int.at(register_file::_ra));
			shared_ptr<type> t = fstmt->function_stmt_type()->return_type();
			if (t->type_kind() == type::kind::KIND_PRIMITIVE && (static_pointer_cast<primitive_type>(t)->primitive_type_kind() == primitive_type::kind::KIND_FLOAT ||
				static_pointer_cast<primitive_type>(t)->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) &&
				t->type_array_kind() == type::array_kind::KIND_NON_ARRAY)
				mc->current_frame()->set_fp_return_value(true);
			if (t->type_kind() == type::kind::KIND_PRIMITIVE && static_pointer_cast<primitive_type>(t)->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE &&
				t->type_array_kind() == type::array_kind::KIND_NON_ARRAY)
				mc->current_frame()->set_dp_return_value(true);
			vector<shared_ptr<variable_declaration>> parm_list = fstmt->function_stmt_type()->parameter_list();
			int a0 = register_file::register_2_int.at(register_file::_a0), f12 = register_file::register_2_int.at(register_file::_f12);
			int offset_from_stack = 0;
			vector<int> reg_args, fp_reg_args;
			for (int i = 0; i < parm_list.size(); i++) {
				shared_ptr<variable_declaration> vdecl = parm_list[i];
				shared_ptr<type> t = vdecl->variable_declaration_type();
				if (fp_reg_args.size() < 2 && (t->type_kind() == type::kind::KIND_PRIMITIVE && (static_pointer_cast<primitive_type>(t)->primitive_type_kind() == primitive_type::kind::KIND_FLOAT ||
					static_pointer_cast<primitive_type>(t)->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
					&& t->type_array_kind() == type::array_kind::KIND_NON_ARRAY) && fstmt->function_stmt_type()->parameter_rfuncarg_list()[i])
					fp_reg_args.push_back(i);
				else if (reg_args.size() < 4 && !(t->type_kind() == type::kind::KIND_PRIMITIVE &&
					(static_pointer_cast<primitive_type>(t)->primitive_type_kind() == primitive_type::kind::KIND_FLOAT ||
						static_pointer_cast<primitive_type>(t)->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) &&  t->type_array_kind() == type::array_kind::KIND_NON_ARRAY)
					&& fstmt->function_stmt_type()->parameter_rfuncarg_list()[i])
					reg_args.push_back(i);
			}
			for (int i = 0; i < parm_list.size(); i++) {
				shared_ptr<variable_declaration> parm = parm_list[i];
				string vsym = symbol_2_string(mc, parm->variable_declaration_symbol());
				int sz = calculate_type_size(mc, parm->variable_declaration_type());
				if (find(reg_args.begin(), reg_args.end(), i) != reg_args.end()) {
					if (parm->variable_declaration_type()->type_kind() == type::kind::KIND_STRUCT &&
						parm->variable_declaration_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY)
						offset_from_stack += sz;
					int offset = find(reg_args.begin(), reg_args.end(), i) - reg_args.begin();
					shared_ptr<register_operand> reg = register_file::register_objects::int_2_register_object.at(a0 + offset);
					mc->current_frame()->mark_register(a0 + offset);
					shared_ptr<register_wrapper> rw = mc->current_frame()->generate_register_wrapper(reg);
					rw->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_REGULAR);
					rw->set_register_wrapper_variable_kind(register_wrapper::variable_kind::KIND_VARIABLE);
					rw->set_register_wrapper_function_argument_kind(register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT);
					mc->current_frame()->add_variable_id(vsym, rw->wrapper_id());
				}
				else if (find(fp_reg_args.begin(), fp_reg_args.end(), i) != fp_reg_args.end()) {
					int offset = find(fp_reg_args.begin(), fp_reg_args.end(), i) - fp_reg_args.begin();
					shared_ptr<register_operand> reg = register_file::register_objects::int_2_register_object.at(f12 + offset * 2);
					mc->current_frame()->mark_register(f12 + offset * 2);
					shared_ptr<register_wrapper> rw = mc->current_frame()->generate_register_wrapper(reg);
					rw->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_REGULAR);
					rw->set_register_wrapper_variable_kind(register_wrapper::variable_kind::KIND_VARIABLE);
					rw->set_register_wrapper_function_argument_kind(register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT);
					rw->set_double_precision(static_pointer_cast<primitive_type>(parm->variable_declaration_type())->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE);
					mc->current_frame()->add_variable_id(vsym, rw->wrapper_id());
				}
				else {
					shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc);
					mc->current_frame()->unmark_register(rw->raw_register_operand()->register_number());
					rw->set_register_wrapper_state_kind(register_wrapper::state_kind::KIND_ON_STACK);
					rw->set_register_wrapper_variable_kind(register_wrapper::variable_kind::KIND_VARIABLE);
					rw->set_offset_from_stack_ptr(offset_from_stack);
					mc->current_frame()->add_variable_id(vsym, rw->wrapper_id());
					offset_from_stack += sz % 8 == 0 ? sz : (sz / 8 + 1) * 8;
				}
			}
			vector<shared_ptr<stmt>> body = fstmt->function_body();
			stmt::kind check = stmt::kind::KIND_NONE;
			for (shared_ptr<stmt> s : body) {
				check = s->stmt_kind();
				generate_stmt_mips(mc, s);
			}
			if (check != stmt::kind::KIND_RETURN) {
				if (!mc->current_frame()->main_function()) {
					if(mc->current_frame()->local_stack_offset() != 0)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-mc->current_frame()->local_stack_offset())));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_JR, register_file::register_objects::_ra_register));
				}
				else {
					if (mc->current_frame()->parent_frame() != nullptr && mc->current_frame()->parent_frame()->local_stack_offset() != 0)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-mc->current_frame()->parent_frame()->local_stack_offset() -
							mc->current_frame()->local_stack_offset())));
					else if(mc->current_frame()->local_stack_offset() != 0)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-mc->current_frame()->local_stack_offset())));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, register_file::register_objects::_v0_register, register_file::register_objects::_zero_register,
						make_shared<immediate_operand>(17)));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, register_file::register_objects::_a0_register, register_file::register_objects::_zero_register,
						register_file::register_objects::_zero_register));
					mc->add_insn(make_shared<insn>(insn::kind::KIND_SYSCALL));
				}
			}
			if (!main_function)
				mc->add_insn(get<3>(lab));
			mc->pop_frame();
			mc->add_insn(make_shared<insn>());
		}

		void generate_return_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<return_stmt> rstmt) {
			shared_ptr<expression> rv = rstmt->return_value();
			shared_ptr<register_operand> to_use = nullptr, sp = register_file::register_objects::_sp_register;
			if (!mc->current_frame()->main_function()) {
				if (mc->current_frame()->fp_return_value() || mc->current_frame()->dp_return_value())
					to_use = register_file::register_objects::_f0_register;
				else
					to_use = register_file::register_objects::_v0_register;
			}
			else
				to_use = register_file::register_objects::_a0_register;
			if (rstmt->return_stmt_value_kind() == return_stmt::value_kind::KIND_EXPRESSION) {
				shared_ptr<operand_wrapper> ow = generate_expression_mips(mc, rv, binary_expression::operator_kind::KIND_NONE, nullptr, vector<unary_expression::kind>{},
					vector<shared_ptr<postfix_expression::postfix_type>>{});
				if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
					if(to_use->register_number() <= 31)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, to_use, register_file::register_objects::_zero_register, ow->contained_immediate_operand()));
					else {
						shared_ptr<register_wrapper> reg = allocate_general_purpose_register(mc);
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, to_use, register_file::register_objects::_zero_register, ow->contained_immediate_operand()));
						mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, reg->raw_register_operand(), to_use));
						if (mc->current_frame()->dp_return_value())
							mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, to_use, to_use));
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, to_use, to_use));
					}
				}
				else if (ow->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
					shared_ptr<register_wrapper> rv = ow->contained_register_wrapper();
					if (mc->current_frame()->fp_return_value()) {
						if (rv->raw_register_operand()->register_number() > 31) {
							if (mc->current_frame()->dp_return_value() && rv->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, to_use, rv->raw_register_operand()));
							else if (!mc->current_frame()->dp_return_value() && !rv->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, to_use, rv->raw_register_operand()));
							else if (mc->current_frame()->dp_return_value() && !rv->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, to_use, rv->raw_register_operand()));
							else
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_D, to_use, rv->raw_register_operand()));
						}
						else {
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rv->raw_register_operand(), to_use));
							if(mc->current_frame()->dp_return_value())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, to_use, to_use));
							else
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, to_use, to_use));
						}
					}
					else {
						if (rv->raw_register_operand()->register_number() > 31) {
							if(rv->double_precision())
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_D, rv->raw_register_operand(), rv->raw_register_operand()));
							else
								mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_W_S, rv->raw_register_operand(), rv->raw_register_operand()));
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MFC1, to_use, rv->raw_register_operand()));
							if (rv->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT) {
								if(rv->double_precision())
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, rv->raw_register_operand(), rv->raw_register_operand()));
								else
									mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, rv->raw_register_operand(), rv->raw_register_operand()));
							}
						}
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, to_use, register_file::register_objects::_zero_register, rv->raw_register_operand()));
					}
					free_general_purpose_register(mc, rv);
				}
				else
					mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			mc->current_frame()->restore_s_registers(mc);
			if (!mc->current_frame()->main_function()) {
				if(mc->current_frame()->local_stack_offset() != 0)
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-mc->current_frame()->local_stack_offset())));
				mc->add_insn(make_shared<insn>(insn::kind::KIND_JR, register_file::register_objects::_ra_register));
			}
			else {
				if (mc->current_frame()->parent_frame() != nullptr && mc->current_frame()->parent_frame()->local_stack_offset() != 0)
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-mc->current_frame()->parent_frame()->local_stack_offset() -
						mc->current_frame()->local_stack_offset())));
				else if(mc->current_frame()->local_stack_offset() != 0)
					mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, sp, sp, make_shared<immediate_operand>(-mc->current_frame()->local_stack_offset())));
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, register_file::register_objects::_v0_register, register_file::register_objects::_zero_register,
					make_shared<immediate_operand>(17)));
				mc->add_insn(make_shared<insn>(insn::kind::KIND_SYSCALL));
			}
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
			}
			return false;
		}

		shared_ptr<register_wrapper> convert_operand_wrapper_2_fp_register_wrapper(shared_ptr<mips_code> mc, shared_ptr<operand_wrapper> src, bool dp) {
			if (src->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_IMMEDIATE) {
				shared_ptr<register_wrapper> rw = allocate_general_purpose_register(mc), frw = allocate_general_purpose_fp_register(mc);
				mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, rw->raw_register_operand(), register_file::register_objects::_zero_register, src->contained_immediate_operand()));
				mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rw->raw_register_operand(), frw->raw_register_operand()));
				free_general_purpose_register(mc, rw);
				if (!dp)
					mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, frw->raw_register_operand(), frw->raw_register_operand()));
				else
					mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, frw->raw_register_operand(), frw->raw_register_operand()));
				frw->set_double_precision(dp);
				return frw;
			}
			else if (src->wrapper_operand_kind() == operand_wrapper::operand_kind::KIND_REGISTER) {
				shared_ptr<register_wrapper> rw = src->contained_register_wrapper();
				if (rw->raw_register_operand()->register_number() > 31) {
					if (rw->double_precision() == dp);
					else if (rw->double_precision()) {
						shared_ptr<register_wrapper> to_use = nullptr;
						if (rw->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
							to_use = allocate_general_purpose_fp_register(mc);
						else
							to_use = rw;
						mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_D, to_use->raw_register_operand(), to_use->raw_register_operand()));
					}
					else {
						shared_ptr<register_wrapper> to_use = nullptr;
						if (rw->register_wrapper_function_argument_kind() == register_wrapper::function_argument_kind::KIND_FUNCTION_ARGUMENT)
							to_use = allocate_general_purpose_fp_register(mc);
						else
							to_use = rw;
						mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_S, to_use->raw_register_operand(), to_use->raw_register_operand()));
					}
					rw->set_double_precision(dp);
					return rw;
				}
				else {
					shared_ptr<register_wrapper> frw = allocate_general_purpose_fp_register(mc);
					mc->add_insn(make_shared<insn>(insn::kind::KIND_MTC1, rw->raw_register_operand(), frw->raw_register_operand()));
					free_general_purpose_register(mc, rw);
					if (!dp)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_S_W, frw->raw_register_operand(), frw->raw_register_operand()));
					else
						mc->add_insn(make_shared<insn>(insn::kind::KIND_CVT_D_W, frw->raw_register_operand(), frw->raw_register_operand()));
					frw->set_double_precision(dp);
					return frw;
				}
			}
			mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		void generate_namespace_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<namespace_stmt> nstmt) {
			vector<shared_ptr<stmt>> sl = reorder_stmt_list(mc, nstmt->namespace_stmt_list());
			for (shared_ptr<stmt> s : sl)
				generate_stmt_mips(mc, s);
		}

		void generate_asm_stmt_mips(shared_ptr<mips_code> mc, shared_ptr<asm_stmt> astmt) {
			for (shared_ptr<asm_stmt::asm_type> at : astmt->asm_type_list()) {
				if (at->asm_type_kind() == asm_stmt::kind::KIND_LA) {
					shared_ptr<symbol> s = at->identifier_symbol();
					string name = symbol_2_string(mc, s);
					shared_ptr<label_operand> lo = make_shared<label_operand>(name);
					string raw_reg;
					for (token t : at->raw_register())
						raw_reg += t.raw_text().substr(1, t.raw_text().length() - 2);
					shared_ptr<label_operand> ro = make_shared<label_operand>(raw_reg);
					long id = mc->current_frame()->get_variable_id(name);
					shared_ptr<register_wrapper> rw = mc->current_frame()->get_register_wrapper(id);
					shared_ptr<register_operand> sp = register_file::register_objects::_sp_register;
					if (rw->register_wrapper_state_kind() == register_wrapper::state_kind::KIND_REGULAR &&
						((rw->raw_register_operand()->register_number() >= register_file::register_2_int.at(register_file::_a0) &&
							rw->raw_register_operand()->register_number() <= register_file::register_2_int.at(register_file::_a3))))
						mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDU, ro, register_file::register_objects::_zero_register, rw->raw_register_operand()));
					else if (rw->register_wrapper_state_kind() == register_wrapper::state_kind::KIND_REGULAR &&
						(rw->raw_register_operand()->register_number() == register_file::register_2_int.at(register_file::_f12) ||
							rw->raw_register_operand()->register_number() == register_file::register_2_int.at(register_file::_f14))) {
						if (rw->double_precision())
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_D, ro, rw->raw_register_operand()));
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_MOV_S, ro, rw->raw_register_operand()));
					}
					else if (s->symbol_kind() == symbol::kind::KIND_VARIABLE) {
						if (rw->register_wrapper_state_kind() == register_wrapper::state_kind::KIND_GLOBAL)
							mc->add_insn(make_shared<insn>(insn::kind::KIND_LA, ro, rw->identifier()));
						else
							mc->add_insn(make_shared<insn>(insn::kind::KIND_ADDIU, ro, sp, make_shared<immediate_operand>(rw->offset_from_stack_ptr())));
					}
					else if (s->symbol_kind() == symbol::kind::KIND_FUNCTION)
						mc->add_insn(make_shared<insn>(insn::kind::KIND_LA, ro, lo));
					else
						mc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				}
				else {
					string raw_stmt;
					for (token t : at->raw_stmt())
						raw_stmt += t.raw_text().substr(1, t.raw_text().length() - 2);
					mc->add_insn(make_shared<insn>(insn::type::TYPE_RAW, raw_stmt));
				}
			}
			int t0 = register_file::register_2_int.at(register_file::_t0), s0 = register_file::register_2_int.at(register_file::_s0),
				f4 = register_file::register_2_int.at(register_file::_f4), f20 = register_file::register_2_int.at(register_file::_f20);
			for (int i = 0; i <= 7; i++) mc->current_frame()->unmark_register(t0 + i);
			for (int i = 0; i <= 7; i++) mc->current_frame()->unmark_register(s0 + i);
			for (int i = 0; i <= 3; i++) mc->current_frame()->unmark_register(f4 + i * 2);
			for (int i = 0; i <= 5; i++) mc->current_frame()->unmark_register(f20 + i * 2);
		}

		vector<shared_ptr<stmt>> reorder_stmt_list(shared_ptr<mips_code> mc, vector<shared_ptr<stmt>> sl) {
			vector<shared_ptr<stmt>> reordered;
			vector<shared_ptr<stmt>> _vdecls_exprs_namespaces_asms, _functions, _main, _rest;
			for (shared_ptr<stmt> s : sl) {
				if (s->stmt_kind() == stmt::kind::KIND_EMPTY);
				else if (s->stmt_kind() == stmt::kind::KIND_VARIABLE_DECLARATION || s->stmt_kind() == stmt::kind::KIND_EXPRESSION || s->stmt_kind() == stmt::kind::KIND_NAMESPACE ||
					s->stmt_kind() == stmt::kind::KIND_ASM)
					_vdecls_exprs_namespaces_asms.push_back(s);
				else if (s->stmt_kind() == stmt::kind::KIND_FUNCTION) {
					if (is_function_main(mc, s->stmt_function()->function_stmt_symbol()))
						_main.push_back(s);
					else
						_functions.push_back(s);
				}
				else
					_rest.push_back(s);
			}
			reordered.insert(reordered.end(), _vdecls_exprs_namespaces_asms.begin(), _vdecls_exprs_namespaces_asms.end());
			reordered.insert(reordered.end(), _functions.begin(), _functions.end());
			reordered.insert(reordered.end(), _main.begin(), _main.end());
			reordered.insert(reordered.end(), _rest.begin(), _rest.end());
			return reordered;
		}
	}
}