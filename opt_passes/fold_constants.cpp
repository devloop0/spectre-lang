#include "fold_constants.hpp"
#include "cvt2ssa.hpp"

#include <unordered_set>
#include <memory>
#include <algorithm>
#include <iterator>
#include <variant>
#include <stack>

using std::static_pointer_cast;
using std::copy_if;
using std::make_pair;
using std::inserter;
using std::visit;
using std::to_string;
using std::stack;

namespace spectre {
	namespace opt {
		constant_data::constant_data(immediate_data v) : _immediate(v), _constant(true) {

		}

		constant_data::constant_data(char c) : _immediate(c), _constant(true) {

		}

		constant_data::constant_data(short s) : _immediate(s), _constant(true) {

		}

		constant_data::constant_data(int i) : _immediate(i), _constant(true) {

		}

		constant_data::constant_data(long l) : _immediate(l), _constant(true) {

		}

		constant_data::constant_data(float f) : _immediate(f), _constant(true) {

		}

		constant_data::constant_data(double d) : _immediate(d), _constant(true) {

		}

		constant_data::constant_data(string s) : _immediate(s), _constant(true) {

		}

		constant_data::constant_data() : _constant(false) {

		}

		bool constant_data::constant() {
			return _constant;
		}

		constant_data::immediate_data constant_data::immediate() {
			return _immediate;
		}

		template<class... Args> struct overload : Args... { using Args::operator()...;  };
		template<class... Args> overload(Args... a) -> overload<Args...>;

		unordered_map<int, unordered_map<int, shared_ptr<constant_data>>> fold_constants(shared_ptr<basic_blocks> bbs) {
			unordered_map<int, unordered_map<int, shared_ptr<constant_data>>> ret;
			int bb_counter = 0;
			for (shared_ptr<basic_block> bb : bbs->get_basic_blocks()) {
				unordered_map<int, shared_ptr<constant_data>> reg_2_constants;
				int insn_counter = 0;
				stack<int> to_remove;
				for (shared_ptr<insn> i : bb->get_insns(bbs)) {
					switch (i->insn_kind()) {
#define UNWRAP_OPERAND(WHICH_SRC, WHICH_I) \
	if ((WHICH_SRC)->operand_kind() == operand::kind::KIND_REGISTER) { \
		shared_ptr<register_operand> temp = static_pointer_cast<register_operand>(WHICH_SRC); \
		if (reg_2_constants.find(temp->virtual_register_number()) == reg_2_constants.end()) \
			break; \
		shared_ptr<constant_data> temp_cd = reg_2_constants[temp->virtual_register_number()]; \
		if (!temp_cd->constant()) \
			break; \
		(WHICH_I) = temp_cd->immediate(); \
	} \
	else { \
		if ((WHICH_SRC)->operand_kind() != operand::kind::KIND_IMMEDIATE) \
			bbs->report_internal("This should be unreacahable.", __FUNCTION__, __LINE__, __FILE__); \
		shared_ptr<immediate_operand> temp = static_pointer_cast<immediate_operand>(WHICH_SRC); \
		(WHICH_I) = temp->immediate(); \
	}
#define CHECK_DST(WHICH_DST) \
	if ((WHICH_DST)->operand_kind() != operand::kind::KIND_REGISTER) \
		break; \
	shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(WHICH_DST); \
	if (ro->dereference()) \
		break;
#define CHECK_SRC(WHICH_SRC) \
	if ((WHICH_SRC)->operand_kind() != operand::kind::KIND_REGISTER \
		&& (WHICH_SRC)->operand_kind() != operand::kind::KIND_IMMEDIATE) \
		break; \
	if ((WHICH_SRC)->operand_kind() == operand::kind::KIND_REGISTER) { \
		shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(WHICH_SRC); \
		if (ro->dereference()) \
			break; \
	}
					case insn::kind::KIND_ACCESS: {
						shared_ptr<access_insn> ai = static_pointer_cast<access_insn>(i);
						shared_ptr<operand> dst = ai->variable();
						if (dst->operand_kind() == operand::kind::KIND_REGISTER) {
							shared_ptr<register_operand> reg = static_pointer_cast<register_operand>(dst);
							if (!reg->dereference())
								reg_2_constants[reg->virtual_register_number()] = make_shared<constant_data>();
						}
					}
						break;
					case insn::kind::KIND_ALIGN:
					case insn::kind::KIND_ASM:
						break;
					case insn::kind::KIND_BINARY: {
						shared_ptr<binary_insn> bi = static_pointer_cast<binary_insn>(i);
						shared_ptr<operand> dsto = bi->dst_operand(),
							src1o = bi->src1_operand(),
							src2o = bi->src2_operand();
						constant_data::immediate_data i1, i2;

						CHECK_DST(dsto);
						CHECK_SRC(src1o);
						CHECK_SRC(src2o);
						UNWRAP_OPERAND(src1o, i1);
						UNWRAP_OPERAND(src2o, i2);

						shared_ptr<register_operand> dst = static_pointer_cast<register_operand>(dsto);
						shared_ptr<constant_data> cd = make_shared<constant_data>();
						unary_insn::kind which_mov = unary_insn::kind::KIND_NONE;
						switch (bi->binary_expr_kind()) {
#define GENERATE_VISITOR(RES_TYPE, TYPE, OP) \
	visit(overload{ \
		[] (char c1, char c2) { return make_shared<constant_data>((RES_TYPE) ((TYPE) c1 OP (TYPE) c2)); }, \
		[] (char c1, short s1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) c1 OP (TYPE) s1)); }, \
		[] (char c1, int i1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) c1 OP (TYPE) i1)); }, \
		[] (char c1, long l1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) c1 OP (TYPE) l1)); }, \
		[] (char c1, float f1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) c1 OP (TYPE) f1)); }, \
		[] (char c1, double d1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) c1 OP (TYPE) d1)); }, \
		[] (char c1, string s1) { return make_shared<constant_data>(); }, \
		[] (short s1, char c1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) s1 OP (TYPE) c1)); }, \
		[] (short s1, short s2) { return make_shared<constant_data>((RES_TYPE) ((TYPE) s1 OP (TYPE) s2)); }, \
		[] (short s1, int i1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) s1 OP (TYPE) i1)); }, \
		[] (short s1, long l1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) s1 OP (TYPE) l1)); }, \
		[] (short s1, float f1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) s1 OP (TYPE) f1)); }, \
		[] (short s1, double d1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) s1 OP (TYPE) d1)); }, \
		[] (short s1, string s2) { return make_shared<constant_data>(); }, \
		[] (int i1, char c1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) i1 OP (TYPE) c1)); }, \
		[] (int i1, short s1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) i1 OP (TYPE) s1)); }, \
		[] (int i1, int i2) { return make_shared<constant_data>((RES_TYPE) ((TYPE) i1 OP (TYPE) i2)); }, \
		[] (int i1, long l1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) i1 OP (TYPE) l1)); }, \
		[] (int i1, float f1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) i1 OP (TYPE) f1)); }, \
		[] (int i1, double d1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) i1 OP (TYPE) d1)); }, \
		[] (int i1, string s1) { return make_shared<constant_data>(); }, \
		[] (long l1, char c1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) l1 OP (TYPE) c1)); }, \
		[] (long l1, short s1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) l1 OP (TYPE) s1)); }, \
		[] (long l1, int i1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) l1 OP (TYPE) i1)); }, \
		[] (long l1, long l2) { return make_shared<constant_data>((RES_TYPE) ((TYPE) l1 OP (TYPE) l2)); }, \
		[] (long l1, float f1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) l1 OP (TYPE) f1)); }, \
		[] (long l1, double d1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) l1 OP (TYPE) d1)); }, \
		[] (long l1, string s1) { return make_shared<constant_data>(); }, \
		[] (float f1, char c1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) f1 OP (TYPE) c1)); }, \
		[] (float f1, short s1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) f1 OP (TYPE) s1)); }, \
		[] (float f1, int i1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) f1 OP (TYPE) i1)); }, \
		[] (float f1, long l1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) f1 OP (TYPE) l1)); }, \
		[] (float f1, float f2) { return make_shared<constant_data>((RES_TYPE) ((TYPE) f1 OP (TYPE) f2)); }, \
		[] (float f1, double d1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) f1 OP (TYPE) d1)); }, \
		[] (float f1, string s1) { return make_shared<constant_data>(); }, \
		[] (double d1, char c1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) d1 OP (TYPE) c1)); }, \
		[] (double d1, short s1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) d1 OP (TYPE) s1)); }, \
		[] (double d1, int i1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) d1 OP (TYPE) i1)); }, \
		[] (double d1, long l1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) d1 OP (TYPE) l1)); }, \
		[] (double d1, float f1) { return make_shared<constant_data>((RES_TYPE) ((TYPE) d1 OP (TYPE) f1)); }, \
		[] (double d1, double d2) { return make_shared<constant_data>((RES_TYPE) ((TYPE) d1 OP (TYPE) d2)); }, \
		[] (double d1, string s1) { return make_shared<constant_data>(); }, \
		[] (string s1, char c1) { return make_shared<constant_data>(); }, \
		[] (string s1, short s2) { return make_shared<constant_data>(); }, \
		[] (string s1, int i1) { return make_shared<constant_data>(); }, \
		[] (string s1, long l1) { return make_shared<constant_data>(); }, \
		[] (string s1, float f1) { return make_shared<constant_data>(); }, \
		[] (string s1, double d1) { return make_shared<constant_data>(); }, \
		[] (string s1, string s2) { return make_shared<constant_data>(); } \
	}, i1, i2)
#define BITWISE_EVAL(ASSIGN_PREFIX, PREFIX, RES_TYPE, TYPE) \
	case binary_insn::kind::KIND_ ## PREFIX ## BAND: \
		cd = GENERATE_VISITOR(RES_TYPE, TYPE, &); \
		which_mov = unary_insn::kind::KIND_ ## ASSIGN_PREFIX ## MOV; \
		break; \
	case binary_insn::kind::KIND_ ## PREFIX ## BOR: \
		cd = GENERATE_VISITOR(RES_TYPE, TYPE, |); \
		which_mov = unary_insn::kind::KIND_ ## ASSIGN_PREFIX ## MOV; \
		break; \
	case binary_insn::kind::KIND_ ## PREFIX ## BXOR: \
		cd = GENERATE_VISITOR(RES_TYPE, TYPE, ^); \
		which_mov = unary_insn::kind::KIND_ ## ASSIGN_PREFIX ## MOV; \
		break; \
	case binary_insn::kind::KIND_ ## PREFIX ## SHL: \
		cd = GENERATE_VISITOR(RES_TYPE, TYPE, <<); \
		which_mov = unary_insn::kind::KIND_ ## ASSIGN_PREFIX ## MOV; \
		break; \
	case binary_insn::kind::KIND_ ## PREFIX ## SHR: \
		cd = GENERATE_VISITOR(RES_TYPE, TYPE, >>); \
		which_mov = unary_insn::kind::KIND_ ## ASSIGN_PREFIX ## MOV; \
		break;
#define ORDERED_CMP_EVAL(ASSIGN_PREFIX, PREFIX, RES_TYPE, TYPE) \
	case binary_insn::kind::KIND_ ## PREFIX ## GE: \
		cd = GENERATE_VISITOR(char, TYPE, >=); \
		which_mov = unary_insn::kind::KIND_CMOV; \
		break; \
	case binary_insn::kind::KIND_ ## PREFIX ## GT: \
		cd = GENERATE_VISITOR(char, TYPE, >); \
		which_mov = unary_insn::kind::KIND_CMOV; \
		break; \
	case binary_insn::kind::KIND_ ## PREFIX ## LE: \
		cd = GENERATE_VISITOR(char, TYPE, <=); \
		which_mov = unary_insn::kind::KIND_CMOV; \
		break; \
	case binary_insn::kind::KIND_ ## PREFIX ## LT: \
		cd = GENERATE_VISITOR(char, TYPE, <); \
		which_mov = unary_insn::kind::KIND_CMOV; \
		break;
#define EQUALITY_CMP_EVAL(ASSIGN_PREFIX, PREFIX, RES_TYPE, TYPE) \
	case binary_insn::kind::KIND_ ## PREFIX ## EQ: \
		cd = GENERATE_VISITOR(char, TYPE, ==); \
		which_mov = unary_insn::kind::KIND_CMOV; \
		break; \
	case binary_insn::kind::KIND_ ## PREFIX ## NE: \
		cd = GENERATE_VISITOR(char, TYPE, !=); \
		which_mov = unary_insn::kind::KIND_CMOV; \
		break; 
#define CMP_EVAL(ASSIGN_PREFIX, PREFIX, RES_TYPE, TYPE) \
	EQUALITY_CMP_EVAL(ASSIGN_PREFIX, PREFIX, RES_TYPE, TYPE); \
	ORDERED_CMP_EVAL(ASSIGN_PREFIX, PREFIX, RES_TYPE, TYPE);
#define ADD_SUB_EVAL(ASSIGN_PREFIX, PREFIX, RES_TYPE, TYPE) \
	case binary_insn::kind::KIND_ ## PREFIX ## ADD: \
		cd = GENERATE_VISITOR(RES_TYPE, TYPE, +); \
		which_mov = unary_insn::kind::KIND_ ## ASSIGN_PREFIX ## MOV; \
		break; \
	case binary_insn::kind::KIND_ ## PREFIX ## SUB: \
		cd = GENERATE_VISITOR(RES_TYPE, TYPE, -); \
		which_mov = unary_insn::kind::KIND_ ## ASSIGN_PREFIX ## MOV; \
		break;
#define MULT_DIV_EVAL(ASSIGN_PREFIX, PREFIX, RES_TYPE, TYPE) \
	case binary_insn::kind::KIND_ ## PREFIX ## DIV: \
		cd = GENERATE_VISITOR(RES_TYPE, TYPE, /); \
		which_mov = unary_insn::kind::KIND_ ## ASSIGN_PREFIX ## MOV; \
		break; \
	case binary_insn::kind::KIND_ ## PREFIX ## MUL: \
		cd = GENERATE_VISITOR(RES_TYPE, TYPE, *); \
		which_mov = unary_insn::kind::KIND_ ## ASSIGN_PREFIX ## MOV; \
		break;
#define MOD_EVAL(ASSIGN_PREFIX, PREFIX, RES_TYPE, TYPE) \
	case binary_insn::kind::KIND_ ## PREFIX ## MOD: \
		cd = GENERATE_VISITOR(RES_TYPE, TYPE, %); \
		which_mov = unary_insn::kind::KIND_ ## ASSIGN_PREFIX ## MOV; \
		break;
#define ARITH_EVAL(ASSIGN_PREFIX, PREFIX, RES_TYPE, TYPE) \
	ADD_SUB_EVAL(ASSIGN_PREFIX, PREFIX, RES_TYPE, TYPE); \
	MULT_DIV_EVAL(ASSIGN_PREFIX, PREFIX, RES_TYPE, TYPE); \
	MOD_EVAL(ASSIGN_PREFIX, PREFIX, RES_TYPE, TYPE);
						ARITH_EVAL(C, C, char, char);
						CMP_EVAL(C, C, char, char);
						BITWISE_EVAL(C, C, char, char);

						ADD_SUB_EVAL(D, D, double, double);
						CMP_EVAL(C, D, char, double);

						ADD_SUB_EVAL(F, F, float, float);
						CMP_EVAL(C, F, char, float);

						ARITH_EVAL(I, I, int, int);
						CMP_EVAL(C, I, char, int);
						BITWISE_EVAL(I, I, int, int);

						ARITH_EVAL(L, L, long, long);
						CMP_EVAL(C, L, char, long);
						BITWISE_EVAL(L, L, long, long);

						ARITH_EVAL(S, S, short, short);
						CMP_EVAL(C, S, char, short);
						BITWISE_EVAL(S, S, short, short);

						MULT_DIV_EVAL(C, UC, char, unsigned char);
						ORDERED_CMP_EVAL(C, UC, char, unsigned char);

						MULT_DIV_EVAL(I, UI, int, unsigned int);
						ORDERED_CMP_EVAL(C, UI, int, unsigned int);

						MULT_DIV_EVAL(L, UL, long, unsigned long);
						ORDERED_CMP_EVAL(C, UL, long, unsigned long);

						MULT_DIV_EVAL(S, US, short, unsigned short);
						ORDERED_CMP_EVAL(C, US, short, unsigned short);
#undef ORDERED_CMP_EVAL
#undef EQUALITY_CMP_EVAL
#undef ADD_SUB_EVAL
#undef MULT_DIV_EVAL
#undef MOD_EVAL
#undef BITWISE_EVAL
#undef ARITH_EVAL
#undef CMP_EVAL
#undef GENERATE_VISITOR
						}
						if (reg_2_constants.find(dst->virtual_register_number()) != reg_2_constants.end())
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						reg_2_constants[dst->virtual_register_number()] = cd;
						if (!cd->constant())
							break;
						shared_ptr<type> reg_type = dst->register_type(), res_type = bi->result_type();
						shared_ptr<immediate_operand> new_imm = make_shared<immediate_operand>(reg_type, cd->immediate());
						shared_ptr<insn> reduced_insn = make_shared<unary_insn>(which_mov, dst, new_imm, res_type);
						bb->set_insn(insn_counter, reduced_insn);
					}
						break;
					case insn::kind::KIND_LABEL:
					case insn::kind::KIND_JUMP:
					case insn::kind::KIND_VAR:
					case insn::kind::KIND_RETURN:
						break;
					case insn::kind::KIND_FUNCTION:
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						break;
#define HANDLE_NON_CONSTANT(TYPE, DST_OPERAND) { \
	shared_ptr<TYPE> temp = static_pointer_cast<TYPE>(i); \
	shared_ptr<operand> dst = temp->DST_OPERAND(); \
	if (dst->operand_kind() != operand::kind::KIND_REGISTER) \
		break; \
	shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(dst); \
	if (ro->dereference()) \
		break; \
	if (reg_2_constants.find(ro->virtual_register_number()) != reg_2_constants.end()) \
		bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__); \
	reg_2_constants[ro->virtual_register_number()] = make_shared<constant_data>(); \
}
					case insn::kind::KIND_MEMCPY:
						HANDLE_NON_CONSTANT(memcpy_insn, destination);
						break;
					case insn::kind::KIND_PHI:
						HANDLE_NON_CONSTANT(phi_insn, dst);
						break;
					case insn::kind::KIND_CALL:
						HANDLE_NON_CONSTANT(call_insn, dest_operand);
						break;
#undef HANDLE_NON_CONSTANT
#define CAST_VISITOR(FROM, UNSIGNED, TO, SRC) \
	visit(overload{ \
		[] (char c) { return make_shared<constant_data>((TO) (UNSIGNED TO) (UNSIGNED FROM) (FROM) c); }, \
		[] (short s) { return make_shared<constant_data>((TO) (UNSIGNED TO) (UNSIGNED FROM) (FROM) s); }, \
		[] (int i) { return make_shared<constant_data>((TO) (UNSIGNED TO) (UNSIGNED FROM) (FROM) i); }, \
		[] (long l) { return make_shared<constant_data>((TO) (UNSIGNED TO) (UNSIGNED FROM) (FROM) l); }, \
		[] (float f) { return make_shared<constant_data>((TO) (UNSIGNED TO) (UNSIGNED FROM) (FROM) f); }, \
		[] (double d) { return make_shared<constant_data>((TO) (UNSIGNED TO) (UNSIGNED FROM) (FROM) d); }, \
		[] (string s) { return make_shared<constant_data>(); } \
	}, (SRC))
#define PERFORM_CAST(UNSIGNED, TO, FROM_TYPE, SRC, RES, MOV) do { \
	switch (TO) { \
	case 1: \
		(RES) = CAST_VISITOR(FROM_TYPE, UNSIGNED, char, (SRC)); \
		(MOV) = unary_insn::kind::KIND_CMOV; \
		break; \
	case 2: \
		(RES) = CAST_VISITOR(FROM_TYPE, UNSIGNED, short, (SRC)); \
		(MOV) = unary_insn::kind::KIND_SMOV; \
		break; \
	case 4: \
		(RES) = CAST_VISITOR(FROM_TYPE, UNSIGNED, int, (SRC)); \
		(MOV) = unary_insn::kind::KIND_IMOV; \
		break; \
	default: \
		bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__); \
		break; \
	} \
} while(0)
					case insn::kind::KIND_EXT: {
						shared_ptr<ext_insn> ei = static_pointer_cast<ext_insn>(i);
						shared_ptr<operand> dsto = ei->lhs(),
							srco = ei->rhs();
						int from = ei->from(), to = ei->to();
						if (to < from) {
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							break;
						}
						constant_data::immediate_data s;
						shared_ptr<constant_data> res = nullptr;
						bool signed_ext = ei->ext_kind() == ext_insn::kind::KIND_SEXT;

						CHECK_DST(dsto);
						CHECK_SRC(srco);
						UNWRAP_OPERAND(srco, s);
						unary_insn::kind which_mov = unary_insn::kind::KIND_NONE;
						switch (from) {
						case 1: {
							if (signed_ext)
								PERFORM_CAST(, to, char, s, res, which_mov);
							else
								PERFORM_CAST(unsigned, to, char, s, res, which_mov);
						}
							break;
						case 2: {
							if (signed_ext)
								PERFORM_CAST(, to, short, s, res, which_mov);
							else
								PERFORM_CAST(unsigned, to, short, s, res, which_mov);
						}
							break;
						case 4: {
							if (signed_ext)
								PERFORM_CAST(, to, int, s, res, which_mov);
							else
								PERFORM_CAST(unsigned, to, int, s, res, which_mov);
						}
							break;
						default:
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							break;
						}
						if (res == nullptr || which_mov == unary_insn::kind::KIND_NONE) {
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							break;
						}
						shared_ptr<register_operand> dst = static_pointer_cast<register_operand>(dsto);
						if (reg_2_constants.find(dst->virtual_register_number()) != reg_2_constants.end())
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						reg_2_constants[dst->virtual_register_number()] = res;
						if (!res->constant())
							break;
						shared_ptr<type> reg_type = dst->register_type(), res_type = ei->result_type();
						shared_ptr<immediate_operand> new_imm = make_shared<immediate_operand>(reg_type, res->immediate());
						shared_ptr<insn> reduced_insn = make_shared<unary_insn>(which_mov, dst, new_imm, res_type);
						bb->set_insn(insn_counter, reduced_insn);
					}
						break;
					case insn::kind::KIND_TRUNC: {
						shared_ptr<trunc_insn> ti = static_pointer_cast<trunc_insn>(i);
						shared_ptr<operand> dsto = ti->lhs(), srco = ti->rhs();
						int from = ti->from(), to = ti->to();
						if (to > from) {
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							break;
						}
						shared_ptr<constant_data> res = nullptr;
						constant_data::immediate_data s;

						CHECK_DST(dsto);
						CHECK_SRC(srco);
						UNWRAP_OPERAND(srco, s);
						unary_insn::kind which_mov = unary_insn::kind::KIND_NONE;
						switch(from) {
						case 1: {
							PERFORM_CAST(unsigned, to, char, s, res, which_mov);
						}
							break;
						case 2: {
							PERFORM_CAST(unsigned, to, short, s, res, which_mov);
						}
							break;
						case 4: {
							PERFORM_CAST(unsigned, to, int, s, res, which_mov);
						}
							break;
						default:
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							break;
						}
						if (res == nullptr || which_mov == unary_insn::kind::KIND_NONE) {
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							break;
						}
						shared_ptr<register_operand> dst = static_pointer_cast<register_operand>(dsto);
						if (reg_2_constants.find(dst->virtual_register_number()) != reg_2_constants.end())
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						reg_2_constants[dst->virtual_register_number()] = res;
						if (!res->constant())
							break;
						shared_ptr<type> reg_type = dst->register_type(), res_type = ti->result_type();
						shared_ptr<immediate_operand> new_imm = make_shared<immediate_operand>(reg_type, res->immediate());
						shared_ptr<insn> reduced_insn = make_shared<unary_insn>(which_mov, dst, new_imm, res_type);
						bb->set_insn(insn_counter, reduced_insn);
					}
						break;
#undef CAST_VISITOR
#undef PERFORM_EXT
#define UNARY_ANY_VISITOR(TYPE, OP, SRC) \
	visit(overload{ \
		[] (char c) { return make_shared<constant_data>((TYPE) OP c); }, \
		[] (short s) { return make_shared<constant_data>((TYPE) OP s); }, \
		[] (int i) { return make_shared<constant_data>((TYPE) OP i); }, \
		[] (long l) { return make_shared<constant_data>((TYPE) OP l); }, \
		[] (float f) { return make_shared<constant_data>((TYPE) OP f); }, \
		[] (double d) { return make_shared<constant_data>((TYPE) OP d); }, \
		[] (string s) { return make_shared<constant_data>(); }, \
	}, (SRC))
#define UNARY_INT_VISITOR(TYPE, OP, SRC) \
	visit(overload{ \
		[] (char c) { return make_shared<constant_data>((TYPE) OP c); }, \
		[] (short s) { return make_shared<constant_data>((TYPE) OP s); }, \
		[] (int i) { return make_shared<constant_data>((TYPE) OP i); }, \
		[] (long l) { return make_shared<constant_data>((TYPE) OP l); }, \
		[] (float f) { return make_shared<constant_data>(); }, \
		[] (double d) { return make_shared<constant_data>(); }, \
		[] (string s) { return make_shared<constant_data>(); }, \
	}, (SRC))
#define UNARY_CAST_VISITOR(FROM, TO, FINAL, SRC) \
	visit(overload{ \
		[] (char c) { return make_shared<constant_data>((FINAL) (TO) (FROM) c); }, \
		[] (short s) { return make_shared<constant_data>((FINAL) (TO) (FROM) s); }, \
		[] (int i) { return make_shared<constant_data>((FINAL) (TO) (FROM) i); }, \
		[] (long l) { return make_shared<constant_data>((FINAL) (TO) (FROM) l); }, \
		[] (float f) { return make_shared<constant_data>((FINAL) (TO) (FROM) f); }, \
		[] (double d) { return make_shared<constant_data>((FINAL) (TO) (FROM) d); }, \
		[] (string s) { return make_shared<constant_data>(); }, \
	}, (SRC))
#define UNARY_ALL_OP_CASES(OP_TEXT, OP, SRC, MOV, DST) { \
	case unary_insn::kind::KIND_L ## OP_TEXT: \
		(MOV) = unary_insn::kind::KIND_LMOV; \
		(DST) = UNARY_ANY_VISITOR(long, OP, (SRC)); \
		break; \
	case unary_insn::kind::KIND_I ## OP_TEXT: \
		(MOV) = unary_insn::kind::KIND_IMOV; \
		(DST) = UNARY_ANY_VISITOR(int, OP, (SRC)); \
		break; \
	case unary_insn::kind::KIND_S ## OP_TEXT: \
		(MOV) = unary_insn::kind::KIND_SMOV; \
		(DST) = UNARY_ANY_VISITOR(short, OP, (SRC)); \
		break; \
	case unary_insn::kind::KIND_C ## OP_TEXT: \
		(MOV) = unary_insn::kind::KIND_CMOV; \
		(DST) = UNARY_ANY_VISITOR(char, OP, (SRC)); \
		break; \
	case unary_insn::kind::KIND_D ## OP_TEXT: \
		(MOV) = unary_insn::kind::KIND_DMOV; \
		(DST) = UNARY_ANY_VISITOR(double, OP, (SRC)); \
		break; \
	case unary_insn::kind::KIND_F ## OP_TEXT: \
		(MOV) = unary_insn::kind::KIND_FMOV; \
		(DST) = UNARY_ANY_VISITOR(float, OP, (SRC)); \
		break; \
}
					case insn::kind::KIND_UNARY: {
						shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
						unary_insn::kind uk = ui->unary_expr_kind();
						shared_ptr<operand> dsto = ui->dst_operand(), srco = ui->src_operand();
						constant_data::immediate_data s;

						CHECK_DST(dsto);
						CHECK_SRC(srco);
						UNWRAP_OPERAND(srco, s);

						shared_ptr<register_operand> dst = static_pointer_cast<register_operand>(dsto);
						shared_ptr<constant_data> cd = nullptr;
						unary_insn::kind which_mov = unary_insn::kind::KIND_NONE;
						bool skip = false;
						switch (uk) {
						case unary_insn::kind::KIND_RESV:
						case unary_insn::kind::KIND_NEW:
						case unary_insn::kind::KIND_STK:
							skip = true;
							break;
						case unary_insn::kind::KIND_DELETE:
						case unary_insn::kind::KIND_LAT:
						case unary_insn::kind::KIND_IAT:
						case unary_insn::kind::KIND_SAT:
						case unary_insn::kind::KIND_CAT:
						case unary_insn::kind::KIND_FAT:
						case unary_insn::kind::KIND_DAT:
							cd = make_shared<constant_data>();
							break;
						UNARY_ALL_OP_CASES(PLUS, +, s, which_mov, cd);
						UNARY_ALL_OP_CASES(MINUS, -, s, which_mov, cd);
						case unary_insn::kind::KIND_LNOT:
							which_mov = unary_insn::kind::KIND_CMOV;
							cd = UNARY_ANY_VISITOR(char, !, s);
							break;
						case unary_insn::kind::KIND_LBNOT:
							which_mov = unary_insn::kind::KIND_LMOV;
							cd = UNARY_INT_VISITOR(long, ~, s);
							break;
						case unary_insn::kind::KIND_IBNOT:
							which_mov = unary_insn::kind::KIND_IMOV;
							cd = UNARY_INT_VISITOR(int, ~, s);
							break;
						case unary_insn::kind::KIND_SBNOT:
							which_mov = unary_insn::kind::KIND_SMOV;
							cd = UNARY_INT_VISITOR(short, ~, s);
							break;
						case unary_insn::kind::KIND_CBNOT:
							which_mov = unary_insn::kind::KIND_CMOV;
							cd = UNARY_INT_VISITOR(char, ~, s);
							break;
						case unary_insn::kind::KIND_CMOV:
							cd = make_shared<constant_data>(s);
							which_mov = unary_insn::kind::KIND_CMOV;
							break;
						case unary_insn::kind::KIND_SMOV:
							cd = make_shared<constant_data>(s);
							which_mov = unary_insn::kind::KIND_SMOV;
							break;
						case unary_insn::kind::KIND_IMOV:
							cd = make_shared<constant_data>(s);
							which_mov = unary_insn::kind::KIND_IMOV;
							break;
						case unary_insn::kind::KIND_LMOV:
							cd = make_shared<constant_data>(s);
							which_mov = unary_insn::kind::KIND_LMOV;
							break;
						case unary_insn::kind::KIND_DMOV:
							cd = make_shared<constant_data>(s);
							which_mov = unary_insn::kind::KIND_DMOV;
							break;
						case unary_insn::kind::KIND_FMOV:
							cd = make_shared<constant_data>(s);
							which_mov = unary_insn::kind::KIND_FMOV;
							break;
						case unary_insn::kind::KIND_FTOL:
							cd = UNARY_CAST_VISITOR(float, long, long, s);
							which_mov = unary_insn::kind::KIND_LMOV;
							break;
						case unary_insn::kind::KIND_DTOL:
							cd = UNARY_CAST_VISITOR(double, long, long, s);
							which_mov = unary_insn::kind::KIND_LMOV;
							break;
						case unary_insn::kind::KIND_LTOF:
							cd = UNARY_CAST_VISITOR(long, float, float, s);
							which_mov = unary_insn::kind::KIND_FMOV;
							break;
						case unary_insn::kind::KIND_LTOD:
							cd = UNARY_CAST_VISITOR(long, double, double, s);
							which_mov = unary_insn::kind::KIND_DMOV;
							break;
						case unary_insn::kind::KIND_FTOUL:
							cd = UNARY_CAST_VISITOR(float, unsigned long, long, s);
							which_mov = unary_insn::kind::KIND_LMOV;
							break;
						case unary_insn::kind::KIND_DTOUL:
							cd = UNARY_CAST_VISITOR(double, unsigned long, long, s);
							which_mov = unary_insn::kind::KIND_LMOV;
							break;
						case unary_insn::kind::KIND_ULTOF:
							cd = UNARY_CAST_VISITOR(unsigned long, float, float, s);
							which_mov = unary_insn::kind::KIND_FMOV;
							break;
						case unary_insn::kind::KIND_ULTOD:
							cd = UNARY_CAST_VISITOR(unsigned long, double, double, s);
							which_mov = unary_insn::kind::KIND_DMOV;
							break;
						case unary_insn::kind::KIND_FTOI:
							cd = UNARY_CAST_VISITOR(float, int, int, s);
							which_mov = unary_insn::kind::KIND_IMOV;
							break;
						case unary_insn::kind::KIND_DTOI:
							cd = UNARY_CAST_VISITOR(double, int, int, s);
							which_mov = unary_insn::kind::KIND_IMOV;
							break;
						case unary_insn::kind::KIND_ITOF:
							cd = UNARY_CAST_VISITOR(int, float, float, s);
							which_mov = unary_insn::kind::KIND_FMOV;
							break;
						case unary_insn::kind::KIND_ITOD:
							cd = UNARY_CAST_VISITOR(int, double, double, s);
							which_mov = unary_insn::kind::KIND_DMOV;
							break;
						case unary_insn::kind::KIND_FTOUI:
							cd = UNARY_CAST_VISITOR(float, unsigned int, int, s);
							which_mov = unary_insn::kind::KIND_IMOV;
							break;
						case unary_insn::kind::KIND_DTOUI:
							cd = UNARY_CAST_VISITOR(double, unsigned int, int, s);
							which_mov = unary_insn::kind::KIND_IMOV;
							break;
						case unary_insn::kind::KIND_UITOF:
							cd = UNARY_CAST_VISITOR(unsigned int, float, float, s);
							which_mov = unary_insn::kind::KIND_FMOV;
							break;
						case unary_insn::kind::KIND_UITOD:
							cd = UNARY_CAST_VISITOR(unsigned int, double, double, s);
							which_mov = unary_insn::kind::KIND_DMOV;
							break;
						case unary_insn::kind::KIND_FTOS:
							cd = UNARY_CAST_VISITOR(float, short, short, s);
							which_mov = unary_insn::kind::KIND_SMOV;
							break;
						case unary_insn::kind::KIND_DTOS:
							cd = UNARY_CAST_VISITOR(double, short, short, s);
							which_mov = unary_insn::kind::KIND_SMOV;
							break;
						case unary_insn::kind::KIND_STOF:
							cd = UNARY_CAST_VISITOR(short, float, float, s);
							which_mov = unary_insn::kind::KIND_FMOV;
							break;
						case unary_insn::kind::KIND_STOD:
							cd = UNARY_CAST_VISITOR(short, double, double, s);
							which_mov = unary_insn::kind::KIND_DMOV;
							break;
						case unary_insn::kind::KIND_FTOUS:
							cd = UNARY_CAST_VISITOR(float, unsigned short, short, s);
							which_mov = unary_insn::kind::KIND_SMOV;
							break;
						case unary_insn::kind::KIND_DTOUS:
							cd = UNARY_CAST_VISITOR(double, unsigned short, short, s);
							which_mov = unary_insn::kind::KIND_SMOV;
							break;
						case unary_insn::kind::KIND_USTOF:
							cd = UNARY_CAST_VISITOR(unsigned short, float, float, s);
							which_mov = unary_insn::kind::KIND_FMOV;
							break;
						case unary_insn::kind::KIND_USTOD:
							cd = UNARY_CAST_VISITOR(unsigned short, double, double, s);
							which_mov = unary_insn::kind::KIND_DMOV;
							break;
						case unary_insn::kind::KIND_FTOC:
							cd = UNARY_CAST_VISITOR(float, char, char, s);
							which_mov = unary_insn::kind::KIND_CMOV;
							break;
						case unary_insn::kind::KIND_DTOC:
							cd = UNARY_CAST_VISITOR(double, char, char, s);
							which_mov = unary_insn::kind::KIND_CMOV;
							break;
						case unary_insn::kind::KIND_CTOF:
							cd = UNARY_CAST_VISITOR(char, float, float, s);
							which_mov = unary_insn::kind::KIND_FMOV;
							break;
						case unary_insn::kind::KIND_CTOD:
							cd = UNARY_CAST_VISITOR(char, double, double, s);
							which_mov = unary_insn::kind::KIND_DMOV;
							break;
						case unary_insn::kind::KIND_FTOUC:
							cd = UNARY_CAST_VISITOR(float, unsigned char, char, s);
							which_mov = unary_insn::kind::KIND_CMOV;
							break;
						case unary_insn::kind::KIND_DTOUC:
							cd = UNARY_CAST_VISITOR(double, unsigned char, char, s);
							which_mov = unary_insn::kind::KIND_CMOV;
							break;
						case unary_insn::kind::KIND_UCTOF:
							cd = UNARY_CAST_VISITOR(unsigned char, float, float, s);
							which_mov = unary_insn::kind::KIND_FMOV;
							break;
						case unary_insn::kind::KIND_UCTOD:
							cd = UNARY_CAST_VISITOR(unsigned char, double, double, s);
							which_mov = unary_insn::kind::KIND_DMOV;
							break;
						case unary_insn::kind::KIND_FTOD:
							cd = UNARY_CAST_VISITOR(float, double, double, s);
							which_mov = unary_insn::kind::KIND_DMOV;
							break;
						case unary_insn::kind::KIND_DTOF:
							cd = UNARY_CAST_VISITOR(double, float, float, s);
							which_mov = unary_insn::kind::KIND_FMOV;
							break;
						default:
							skip = true;
							break;
						}
						if (skip)
							break;
						if (reg_2_constants.find(dst->virtual_register_number()) != reg_2_constants.end()
							|| cd == nullptr || which_mov == unary_insn::kind::KIND_NONE)
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						reg_2_constants[dst->virtual_register_number()] = cd;
						if (!cd->constant())
							break;
						shared_ptr<type> reg_type = dst->register_type(), res_type = ui->result_type();
						shared_ptr<immediate_operand> new_imm = make_shared<immediate_operand>(reg_type, cd->immediate());
						shared_ptr<insn> reduced_insn = make_shared<unary_insn>(which_mov, dst, new_imm, res_type);
						bb->set_insn(insn_counter, reduced_insn);
					}
#undef UNARY_ANY_VISITOR
#undef UNARY_INT_VISITOR
#undef UNARY_ALL_OP_CASES
						break;
					case insn::kind::KIND_CONDITIONAL: {
						shared_ptr<conditional_insn> ci = static_pointer_cast<conditional_insn>(i);
						shared_ptr<operand> srco = ci->src();
						shared_ptr<label_operand> lo = ci->branch();
						constant_data::immediate_data s;

						CHECK_SRC(srco);
						UNWRAP_OPERAND(srco, s);

						bool make_jump = visit(overload{
							[] (char c) { return (bool) c; },
							[] (short s) { return (bool) s; },
							[] (int i) { return (bool) i; },
							[] (long l) { return (bool) l; },
							[] (double d) { return (bool) d; },
							[] (float f) { return (bool) f; },
							[] (string s) { return true; }
						}, s);
						string lab_text = lo->label_text();
						int to_jump_bb = 0;
						for (shared_ptr<basic_block> bb : bbs->get_basic_blocks()) {
							vector<shared_ptr<insn>> bb_insns = bb->get_insns(bbs);
							if (!bb_insns.empty()) {
								shared_ptr<insn> i = bb_insns[0];
								if (i->insn_kind() == insn::kind::KIND_LABEL) {
									shared_ptr<label_insn> li = static_pointer_cast<label_insn>(i);
									if (li->label()->label_text() == lab_text)
										break;
								}
							}
							to_jump_bb++;
						}
						if (to_jump_bb == bbs->get_basic_blocks().size()) {
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							break;
						}
						if (make_jump) {
							bb->set_insn(insn_counter, make_shared<jump_insn>(lo));
							unordered_set<int> adjs = bbs->cfg().adjacent(bb_counter);
							for (const auto& v : adjs)
								bbs->cfg().remove_edge(bb_counter, v);
							bbs->cfg().add_edge(bb_counter, to_jump_bb);
						}
						else {
							to_remove.push(insn_counter);
							bbs->cfg().remove_edge(bb_counter, to_jump_bb);
						}
					}
						break;
#undef CHECK_SRC
#undef CHECK_DST
#undef UNWRAP_OPERAND
					}
					insn_counter++;
				}
				while (!to_remove.empty()) {
					int pos = to_remove.top();
					to_remove.pop();
					bb->remove_insn(pos);
				}
				ret[bb_counter] = reg_2_constants;
				bb_counter++;
			}
			return ret;
		}

		fold_constants_pass::fold_constants_pass(shared_ptr<pass_manager> pm) : pass("fold_constants", "Evaluates constant expressions", pm->next_pass_id(),
			unordered_set<string>{ "insns_in_bb" }) {

		}

		fold_constants_pass::fold_constants_pass(shared_ptr<pass_manager> pm, int counter, unordered_set<string> deps) :
			pass("fold_constants" + to_string(counter), "Evaluates constant expressions", pm->next_pass_id(), (deps.insert("insns_in_bb"), deps)) {

		}

		fold_constants_pass::~fold_constants_pass() {

		}

		shared_ptr<basic_blocks> fold_constants_pass::run_pass(shared_ptr<basic_blocks> bbs) {
			_data = fold_constants(bbs);
			return bbs;
		}

		unordered_map<int, unordered_map<int, shared_ptr<constant_data>>> fold_constants_pass::data() {
			return _data;
		}
	}
}
