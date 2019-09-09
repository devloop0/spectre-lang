#ifndef SPECTRE_MIPS_INSTRUCTION_SET_HPP
#define SPECTRE_MIPS_INSTRUCTION_SET_HPP

#include <string>
#include <map>
#include "core/config.hpp"

using std::string;
using std::map;

namespace spectre {
	namespace mips {
		namespace register_file {
			
#if SYSTEM == 0 || SYSTEM == 1
			const static string _0 = "$0", _zero = _0;
			const static string _1 = "$1", _at = _1;
			const static string _2 = "$2", _v0 = _2;
			const static string _3 = "$3", _v1 = _3;
			const static string _4 = "$4", _a0 = _4;
			const static string _5 = "$5", _a1 = _5;
			const static string _6 = "$6", _a2 = _6;
			const static string _7 = "$7", _a3 = _7;
			const static string _8 = "$8", _t0 = _8;
			const static string _9 = "$9", _t1 = _9;
			const static string _10 = "$10", _t2 = _10;
			const static string _11 = "$11", _t3 = _11;
			const static string _12 = "$12", _t4 = _12;
			const static string _13 = "$13", _t5 = _13;
			const static string _14 = "$14", _t6 = _14;
			const static string _15 = "$15", _t7 = _15;
			const static string _16 = "$16", _s0 = _16;
			const static string _17 = "$17", _s1 = _17;
			const static string _18 = "$18", _s2 = _18;
			const static string _19 = "$19", _s3 = _19;
			const static string _20 = "$20", _s4 = _20;
			const static string _21 = "$21", _s5 = _21;
			const static string _22 = "$22", _s6 = _22;
			const static string _23 = "$23", _s7 = _23;
			const static string _24 = "$24", _t8 = _24;
			const static string _25 = "$25", _t9 = _25;
			const static string _26 = "$26", _k0 = _26;
			const static string _27 = "$27", _k1 = _27;
			const static string _28 = "$28", _gp = _28;
			const static string _29 = "$29", _sp = _29;
			const static string _30 = "$30", _fp = _30;
			const static string _31 = "$31", _ra = _31;
#elif SYSTEM == 2
			const static string _0 = "x0", _zero = _0;
			const static string _1 = "x1", _ra = _1;
			const static string _2 = "x2", _sp = _2;
			const static string _3 = "x3", _gp = _3;
			const static string _4 = "x4", _tp = _4;
			const static string _5 = "x5", _t0 = _5;
			const static string _6 = "x6", _t1 = _6;
			const static string _7 = "x7", _t2 = _7;
			const static string _8 = "x8", _s0 = _8, _fp = _s0;
			const static string _9 = "x9", _s1 = _9;
			const static string _10 = "x10", _a0 = _10;
			const static string _11 = "x11", _a1 = _11;
			const static string _12 = "x12", _a2 = _12;
			const static string _13 = "x13", _a3 = _13;
			const static string _14 = "x14", _a4 = _14;
			const static string _15 = "x15", _a5 = _15;
			const static string _16 = "x16", _a6 = _16;
			const static string _17 = "x17", _a7 = _17;
			const static string _18 = "x18", _s2 = _18;
			const static string _19 = "x19", _s3 = _19;
			const static string _20 = "x20", _s4 = _20;
			const static string _21 = "x21", _s5 = _21;
			const static string _22 = "x22", _s6 = _22;
			const static string _23 = "x23", _s7 = _23;
			const static string _24 = "x24", _s8 = _24;
			const static string _25 = "x25", _s9 = _25;
			const static string _26 = "x26", _s10 = _26;
			const static string _27 = "x27", _s11 = _27;
			const static string _28 = "x28", _t3 = _28;
			const static string _29 = "x29", _t4 = _29;
			const static string _30 = "x30", _t5 = _30;
			const static string _31 = "x31", _t6 = _31;
#endif

#if SYSTEM == 0 || SYSTEM == 1
			const static string _f0 = "$f0";
			const static string _f1 = "$f1";
			const static string _f2 = "$f2";
			const static string _f3 = "$f3";
			const static string _f4 = "$f4";
			const static string _f5 = "$f5";
			const static string _f6 = "$f6";
			const static string _f7 = "$f7";
			const static string _f8 = "$f8";
			const static string _f9 = "$f9";
			const static string _f10 = "$f10";
			const static string _f11 = "$f11";
			const static string _f12 = "$f12";
			const static string _f13 = "$f13";
			const static string _f14 = "$f14";
			const static string _f15 = "$f15";
			const static string _f16 = "$f16";
			const static string _f17 = "$f17";
			const static string _f18 = "$f18";
			const static string _f19 = "$f19";
			const static string _f20 = "$f20";
			const static string _f21 = "$f21";
			const static string _f22 = "$f22";
			const static string _f23 = "$f23";
			const static string _f24 = "$f24";
			const static string _f25 = "$f25";
			const static string _f26 = "$f26";
			const static string _f27 = "$f27";
			const static string _f28 = "$f28";
			const static string _f29 = "$f29";
			const static string _f30 = "$f30";
			const static string _f31 = "$f31";
#elif SYSTEM == 2
			const static string _f0 = "f0";
			const static string _f1 = "f1";
			const static string _f2 = "f2";
			const static string _f3 = "f3";
			const static string _f4 = "f4";
			const static string _f5 = "f5";
			const static string _f6 = "f6";
			const static string _f7 = "f7";
			const static string _f8 = "f8";
			const static string _f9 = "f9";
			const static string _f10 = "f10";
			const static string _f11 = "f11";
			const static string _f12 = "f12";
			const static string _f13 = "f13";
			const static string _f14 = "f14";
			const static string _f15 = "f15";
			const static string _f16 = "f16";
			const static string _f17 = "f17";
			const static string _f18 = "f18";
			const static string _f19 = "f19";
			const static string _f20 = "f20";
			const static string _f21 = "f21";
			const static string _f22 = "f22";
			const static string _f23 = "f23";
			const static string _f24 = "f24";
			const static string _f25 = "f25";
			const static string _f26 = "f26";
			const static string _f27 = "f27";
			const static string _f28 = "f28";
			const static string _f29 = "f29";
			const static string _f30 = "f30";
			const static string _f31 = "f31";
#endif

			const static map<string, int> register_2_int = {
				{ _0, 0 },
				{ _1, 1 },
				{ _2, 2 }, { _3, 3 },
				{ _4, 4 }, { _5, 5 }, { _6, 6 }, { _7, 7 },
				{ _8, 8 }, { _9, 9 }, { _10, 10 }, { _11, 11 }, { _12, 12 }, { _13, 13 }, { _14, 14 }, { _15, 15 },
				{ _16, 16 }, { _17, 17 }, { _18, 18 }, { _19, 19 }, { _20, 20 }, { _21, 21 }, { _22, 22 }, { _23, 23 },
				{ _24, 24 }, { _25, 25 },
				{ _26, 26 }, { _27, 27 },
				{ _28, 28 },
				{ _29, 29 },
				{ _30, 30 },
				{ _31, 31 },
				{ _f0, 32 }, { _f1, 33 }, { _f2, 34 }, { _f3, 35 },
				{ _f4, 36 }, { _f5, 37 }, { _f6, 38 }, { _f7, 39 }, { _f8, 40 }, { _f9, 41 }, { _f10, 42 },
				{ _f11, 43 },
				{ _f12, 44 }, { _f13, 45 }, { _f14, 46 },
				{ _f15, 47 },
				{ _f16, 48 }, { _f17, 49 }, { _f18, 50 },
				{ _f19, 51 },
				{ _f20, 52 }, { _f21, 53 }, { _f22, 54 }, { _f23, 55 }, { _f24, 56 }, { _f25, 57 }, { _f26, 58 }, { _f27, 59 }, { _f28, 60 }, { _f29, 61 }, { _f30, 62 },
				{ _f31, 63 }
			};

			const static map<int, string> int_2_register = {
				{ 0, _0 },
				{ 1, _1 },
				{ 2, _2 }, { 3, _3 },
				{ 4, _4 }, { 5, _5 }, { 6, _6 }, { 7, _7 },
				{ 8, _8 }, { 9, _9 }, { 10, _10 }, { 11, _11 }, { 12, _12 }, { 13, _13 }, { 14, _14 }, { 15, _15 },
				{ 16, _16 }, { 17, _17 }, { 18, _18 }, { 19, _19 }, { 20, _20 }, { 21, _21 }, { 22, _22 }, { 23, _23 },
				{ 24, _24 }, { 25, _25 },
				{ 26, _26 }, { 27, _27 },
				{ 28, _28 },
				{ 29, _29 },
				{ 30, _30 },
				{ 31, _31 },
				{ 32, _f0 }, { 33, _f1 }, { 34, _f2 }, { 35, _f3 },
				{ 36, _f4 }, { 37, _f5 }, { 38, _f6 }, { 39, _f7 }, { 40, _f8 }, { 41, _f9 }, { 42, _f10 },
				{ 43, _f11 },
				{ 44, _f12 }, { 45, _f13 }, { 46, _f14 },
				{ 47, _f15 },
				{ 48, _f16 }, { 49, _f17 }, { 50, _f18 },
				{ 51, _f19 },
				{ 52, _f20 }, { 53, _f21 }, { 54, _f22 }, { 55, _f23 }, { 56, _f24 }, { 57, _f25 }, { 58, _f26 }, { 59, _f27 }, { 60, _f28 }, { 61, _f29 }, { 62, _f30 },
				{ 63, _f31 }
			};
		}

		namespace insns {

#if SYSTEM == 0 || SYSTEM == 1
			const static string _add = "add";
			const static string _addi = "addi";
			const static string _addiu = "addiu";
			const static string _addu = "addu";
#elif SYSTEM == 2
			const static string _add = "add";
			const static string _addi = "addi";
			const static string _addiu = "addi";
			const static string _addu = "add";
#endif

			const static string _and = "and";
			const static string _andi = "andi";

			const static string _beq = "beq";
			const static string _bne = "bne";
			const static string _bgez = "bgez";
			const static string _bgezal = "bgezal";
			const static string _bgtz = "bgtz";
			const static string _blez = "blez";
			const static string _bltz = "bltz";
			const static string _bltzal = "bltzal";

			const static string _jal = "jal";
			const static string _j = "j";
			const static string _jr = "jr";
			const static string _jalr = "jalr";

			const static string _la = "la";

			const static string _lb = "lb";
			const static string _lbu = "lbu";
			const static string _lh = "lh";
			const static string _lhu = "lhu";
			const static string _lw = "lw";

			const static string _lui = "lui";

			const static string _mflo = "mflo";
			const static string _mfhi = "mfhi";
#if SYSTEM == 0 || SYSTEM == 1
			const static string _mult = "mult";
			const static string _multu = "multu";
			const static string _div = "div";
			const static string _divu = "divu";
#elif SYSTEM == 2
			const static string _mult = "mul";
			const static string _multu = "mul";
			const static string _div = "div";
			const static string _divu = "divu";
			const static string _rem = "rem";
			const static string _remu = "remu";
#endif

			const static string _noop = "noop";

			const static string _nor = "nor";
			const static string _or = "or";
			const static string _ori = "ori";

			const static string _sb = "sb";
			const static string _sh = "sh";
			const static string _sw = "sw";

#if SYSTEM == 0 || SYSTEM == 1
			const static string _sll = "sll";
			const static string _sllv = "sllv";
			const static string _sra = "sra";
			const static string _srav = "srav";
			const static string _srl = "srl";
			const static string _srlv = "srlv";
#elif SYSTEM == 2
			const static string _sll = "slli";
			const static string _sllv = "sll";
			const static string _sra = "srai";
			const static string _srav = "sra";
			const static string _srl = "srli";
			const static string _srlv = "srl";
#endif

			const static string _slt = "slt";
			const static string _slti = "slti";
			const static string _sltiu = "sltiu";
			const static string _sltu = "sltu";
			const static string _sgt = "sgt";
			const static string _sgtu = "sgtu";
			const static string _sle = "sle";
			const static string _sleu = "sleu";
			const static string _sge = "sge";
			const static string _sgeu = "sgeu";
			const static string _seq = "seq";
			const static string _sne = "sne";

#if SYSTEM == 0 || SYSTEM == 1
			const static string _sub = "sub";
			const static string _subu = "subu";
#elif SYSTEM == 2
			const static string _sub = "sub";
			const static string _subu = "sub";
#endif

#if SYSTEM == 0 || SYSTEM == 1
			const static string _syscall = "syscall";
#elif SYSTEM == 2
			const static string _syscall = "ecall";
#endif

			const static string _xor = "xor";
			const static string _xori = "xori";

#if SYSTEM == 0 || SYSTEM == 1
			const static string _add_s = "add.s";
			const static string _add_d = "add.d";
			const static string _sub_s = "sub.s";
			const static string _sub_d = "sub.d";
			const static string _mul_s = "mul.s";
			const static string _mul_d = "mul.d";
			const static string _div_s = "div.s";
			const static string _div_d = "div.d";
#elif SYSTEM == 2
			const static string _add_s = "fadd.s";
			const static string _add_d = "fadd.d";
			const static string _sub_s = "fsub.s";
			const static string _sub_d = "fsub.d";
			const static string _mul_s = "fmul.s";
			const static string _mul_d = "fmul.d";
			const static string _div_s = "fdiv.s";
			const static string _div_d = "fdiv.d";
#endif
			
#if SYSTEM == 0 || SYSTEM == 1
			const static string _lwc1 = "lwc1";
			const static string _ldc1 = "ldc1";
			const static string _swc1 = "swc1";
			const static string _sdc1 = "sdc1";
#elif SYSTEM == 2
			const static string _lwc1 = "flw";
			const static string _ldc1 = "fld";
			const static string _swc1 = "fsw";
			const static string _sdc1 = "fsd";
#endif

			const static string _l_s = "l.s";
			const static string _l_d = "l.d";

			const static string _bc1t = "bc1t";
			const static string _bc1f = "bc1f";

#if SYSTEM == 0 || SYSTEM == 1
			const static string _cvt_d_s = "cvt.d.s";
			const static string _cvt_d_w = "cvt.d.w";
			const static string _cvt_s_d = "cvt.s.d";
			const static string _cvt_s_w = "cvt.s.w";
			const static string _cvt_w_d = "cvt.w.d";
			const static string _cvt_w_s = "cvt.w.s";
#elif SYSTEM == 2
			const static string _cvt_d_s = "fcvt.d.s";
			const static string _cvt_d_w = "fcvt.d.w";
			const static string _cvt_s_d = "fcvt.s.d";
			const static string _cvt_s_w = "fcvt.s.w";
			const static string _cvt_w_d = "fcvt.w.d";
			const static string _cvt_w_s = "fcvt.w.s";
#endif

#if SYSTEM == 0 || SYSTEM == 1
			const static string _c_eq_s = "c.eq.s";
			const static string _c_eq_d = "c.eq.d";
			const static string _c_ge_s = "c.ge.s";
			const static string _c_ge_d = "c.ge.d";
			const static string _c_gt_s = "c.gt.s";
			const static string _c_gt_d = "c.gt.d";
			const static string _c_le_s = "c.le.s";
			const static string _c_le_d = "c.le.d";
			const static string _c_lt_s = "c.lt.s";
			const static string _c_lt_d = "c.lt.d";
			const static string _c_ne_s = "c.ne.s";
			const static string _c_ne_d = "c.ne.d";
#elif SYSTEM == 2
			const static string _c_eq_s = "feq.s";
			const static string _c_eq_d = "feq.d";
			const static string _c_ge_s = "fge.s";
			const static string _c_ge_d = "fge.d";
			const static string _c_gt_s = "fgt.s";
			const static string _c_gt_d = "fgt.d";
			const static string _c_le_s = "fle.s";
			const static string _c_le_d = "fle.d";
			const static string _c_lt_s = "flt.s";
			const static string _c_lt_d = "flt.d";
			const static string _c_ne_s = "fne.s";
			const static string _c_ne_d = "fne.d";
#endif

#if SYSTEM == 0 || SYSTEM == 1
			const static string _mfc1 = "mfc1";
			const static string _mtc1 = "mtc1";
#elif SYSTEM == 2
			const static string _mfc1 = "fmv.x.w";
			const static string _mtc1 = "fmv.w.x";
#endif

#if SYSTEM == 0 || SYSTEM == 1
			const static string _mov_d = "mov.d";
			const static string _mov_s = "mov.s";

			const static string _neg_s = "neg.s";
			const static string _neg_d = "neg.d";
#elif SYSTEM == 2
			const static string _mov_d = "fmv.d";
			const static string _mov_s = "fmv.s";

			const static string _neg_s = "fneg.s";
			const static string _neg_d = "fneg.d";
#endif
		}
	}
}

#endif
