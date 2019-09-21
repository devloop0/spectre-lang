#include "ir/middle_ir.hpp"
#include "parser/parser.hpp"
#include <algorithm>
#include <memory>
#include <iostream>
#include <tuple>
#include <sstream>
#include <iomanip>
#include <limits>

using std::get;
using spectre::parser::block_scope;
using std::static_pointer_cast;
using std::make_shared;
using std::max_element;
using std::stoi;
using std::make_pair;
using std::make_tuple;
using std::holds_alternative;
using std::stoul;
using std::stringstream;
using std::setprecision;
using std::fixed;
using std::numeric_limits;
using std::remove;

namespace spectre {
	namespace ir {

		insn::insn(insn::kind k) : _insn_kind(k) {

		}

		insn::~insn() {

		}

		insn::kind insn::insn_kind() {
			return _insn_kind;
		}

		global_array_insn::global_array_insn(directive_kind dk, shared_ptr<label_operand> l, vector<shared_ptr<operand>> d, shared_ptr<type> t) :
			insn(insn::kind::KIND_GLOBAL_ARRAY), _directive(dk), _label(l), _data(d), _array_type(t) {

		}

		global_array_insn::~global_array_insn() {

		}

		shared_ptr<label_operand> global_array_insn::label() {
			return _label;
		}

		shared_ptr<type> global_array_insn::array_type() {
			return _array_type;
		}

		vector<shared_ptr<operand>> global_array_insn::data() {
			return _data;
		}

		global_array_insn::directive_kind global_array_insn::directive() {
			return _directive;
		}

		string global_array_insn::to_string() {
			string ret;
			switch(_directive) {
			case directive_kind::KIND_BYTE:
				ret += "arr_byte ";
				break;
			case directive_kind::KIND_HALF:
				ret += "arr_half ";
				break;
			case directive_kind::KIND_WORD:
				ret += "arr_word ";
				break;
			case directive_kind::KIND_FLOAT:
				ret += "arr_float ";
				break;
			case directive_kind::KIND_DOUBLE:
				ret += "arr_double ";
				break;
			}

			ret += _label->to_string() + " ";
			for (int i = 0; i < _data.size(); i++) {
				ret += _data[i]->to_string();
				if (i != _data.size() - 1)
					ret += ", ";
			}
			return ret;
		}

		void global_array_insn::deduplicate() {
			for (auto& o : _data)
				DEDUP(o);
		}

		asm_insn::asm_insn(string d) : insn(insn::kind::KIND_ASM), _data(d), _asm_insn_kind(asm_insn::kind::KIND_RAW_STRING) {

		}

		asm_insn::asm_insn(pair<string, shared_ptr<register_operand>> p) : insn(insn::kind::KIND_ASM), _data(p), _asm_insn_kind(asm_insn::kind::KIND_LA) {

		}

		asm_insn::~asm_insn() {

		}

		string asm_insn::raw_string() {
			return _asm_insn_kind == asm_insn::kind::KIND_RAW_STRING ? get<string>(_data) : "";
		}

		pair<string, shared_ptr<register_operand>> asm_insn::la() {
			return _asm_insn_kind == asm_insn::kind::KIND_LA ? get<pair<string, shared_ptr<register_operand>>>(_data) : make_pair("", nullptr);
		}

		asm_insn::kind asm_insn::asm_insn_kind() {
			return _asm_insn_kind;
		}

		string asm_insn::to_string() {
			switch (_asm_insn_kind) {
			case asm_insn::kind::KIND_LA: {
				pair<string, shared_ptr<register_operand>> d = get<pair<string, shared_ptr<register_operand>>>(_data);
				return "__asm__ ( \"" + d.first + "\" " + d.second->to_string() + " )";
			}
				break;
			case asm_insn::kind::KIND_RAW_STRING:
				return "__asm__ ( \"" + get<string>(_data) + "\" )";
			default:
				return "";
			}
			return "";
		}

		void asm_insn::deduplicate() {
			return;
		}

		jump_insn::jump_insn(shared_ptr<label_operand> l) : insn(insn::kind::KIND_JUMP), _label(l) {

		}

		jump_insn::~jump_insn() {

		}

		string jump_insn::to_string() {
			return "j " + _label->to_string();
		}

		void jump_insn::deduplicate() {
			return;
		}

		shared_ptr<label_operand> jump_insn::label() {
			return _label;
		}

		void jump_insn::set_label(shared_ptr<label_operand> l) {
			_label = l;
		}

		access_insn::access_insn(string s, shared_ptr<operand> v) : insn(insn::kind::KIND_ACCESS), _variable(v), _symbol(s) {

		}

		access_insn::~access_insn() {

		}

		string access_insn::to_string() {
			return "access " + _symbol + ", " + _variable->to_string();
		}

		void access_insn::deduplicate() {
			DEDUP(_variable);
		}

		shared_ptr<operand> access_insn::variable() {
			return _variable;
		}

		string access_insn::symbol() {
			return _symbol;
		}

		return_insn::return_insn(shared_ptr<operand> e) : insn(insn::kind::KIND_RETURN), _expr(e), _return_kind(return_insn::kind::KIND_REGULAR) {

		}

		return_insn::return_insn() : insn(insn::kind::KIND_RETURN), _expr(nullptr), _return_kind(return_insn::kind::KIND_VOID) {

		}

		return_insn::~return_insn() {

		}

		shared_ptr<operand> return_insn::expr() {
			return _expr;
		}

		return_insn::kind return_insn::return_kind() {
			return _return_kind;
		}

		string return_insn::to_string() {
			return "ret " + _expr->to_string();
		}
		
		void return_insn::deduplicate() {
			DEDUP(_expr);
		}

		label_insn::label_insn(shared_ptr<label_operand> l) : insn(insn::kind::KIND_LABEL), _label(l), _func_param_regs(vector<int>{}), _func_return_reg(-1),
		       _orig_func_insn(nullptr) {

		}

		label_insn::label_insn(shared_ptr<label_operand> l, vector<int> fpr, int frr, shared_ptr<function_insn> ofi) : insn(insn::kind::KIND_LABEL),
			_label(l), _func_param_regs(fpr), _func_return_reg(frr), _orig_func_insn(ofi) {

		}

		label_insn::~label_insn() {

		}

		void label_insn::set_orig_func_insn(shared_ptr<function_insn> ofi) {
			_orig_func_insn = ofi;
		}

		shared_ptr<function_insn> label_insn::orig_func_insn() {
			return _orig_func_insn;
		}

		int label_insn::func_return_reg() {
			return _func_return_reg;
		}

		void label_insn::set_func_return_reg(int i) {
			_func_return_reg = i;
		}

		vector<int> label_insn::func_param_regs() {
			return _func_param_regs;
		}

		void label_insn::set_func_param_regs(vector<int> regs) {
			_func_param_regs = regs;
		}

		shared_ptr<label_operand> label_insn::label() {
			return _label;
		}

		string label_insn::to_string() {
			return _label->to_string() + ":";
		}

		void label_insn::deduplicate() {
			return;
		}

		conditional_insn::conditional_insn(shared_ptr<operand> s, shared_ptr<label_operand> b) : insn(insn::kind::KIND_CONDITIONAL), _src(s), _branch(b) {

		}

		conditional_insn::~conditional_insn() {

		}

		shared_ptr<operand> conditional_insn::src() {
			return _src;
		}

		shared_ptr<label_operand> conditional_insn::branch() {
			return _branch;
		}

		void conditional_insn::set_branch(shared_ptr<label_operand> l) {
			_branch = l;
		}

		string conditional_insn::to_string() {
			return "cond " + _src->to_string() + ", " + _branch->to_string();
		}

		void conditional_insn::deduplicate() {
			DEDUP(_src);
		}

		call_insn::call_insn(shared_ptr<type> rt, shared_ptr<operand> d, shared_ptr<operand> fo,
			vector<shared_ptr<operand>> al, shared_ptr<function_type> ft)
			: insn(insn::kind::KIND_CALL), _result_type(rt), _function_operand(fo), _argument_list(al), _is_variable(false),
			_variable_decl(nullptr), _dest_operand(d), _called_function_type(ft) {

		}

		call_insn::call_insn(shared_ptr<type> rt, shared_ptr<operand> d, shared_ptr<operand> fo, vector<shared_ptr<operand>> al,
			shared_ptr<variable_declaration> vd, shared_ptr<function_type> ft) : insn(insn::kind::KIND_CALL), _result_type(rt),
			_function_operand(fo), _argument_list(al), _is_variable(true), _variable_decl(vd), _dest_operand(d), _called_function_type(ft) {

		}

		call_insn::~call_insn() {

		}

		shared_ptr<function_type> call_insn::called_function_type() {
			return _called_function_type;
		}

		shared_ptr<type> call_insn::result_type() {
			return _result_type;
		}

		shared_ptr<operand> call_insn::function_operand() {
			return _function_operand;
		}

		shared_ptr<operand> call_insn::dest_operand() {
			return _dest_operand;
		}

		vector<shared_ptr<operand>> call_insn::argument_list() {
			return _argument_list;
		}

		bool call_insn::is_variable() {
			return _is_variable;
		}

		shared_ptr<variable_declaration> call_insn::variable_decl() {
			return _variable_decl;
		}

		string call_insn::to_string() {
			string ret = "call " + _dest_operand->to_string() + ", " + _function_operand->to_string();
			if (!_argument_list.empty()) ret += ", ";
			unsigned int i = 0;
			for (shared_ptr<operand> op : _argument_list) {
				ret += op->to_string();
				if (i != _argument_list.size() - 1)
					ret += ", ";
				i++;
			}
			return ret;
		}

		void call_insn::deduplicate() {
			DEDUP(_dest_operand);
			DEDUP(_function_operand);
			for (auto& d : _argument_list)
				DEDUP(d);
		}

		binary_insn::binary_insn(binary_insn::kind k, shared_ptr<operand> d, shared_ptr<operand> s1, shared_ptr<operand> s2, shared_ptr<type> r) : insn(insn::kind::KIND_BINARY),
			_src1_operand(s1), _src2_operand(s2), _dst_operand(d), _result_type(r), _variable_decl(nullptr), _binary_expr_kind(k), _is_variable_decl(false) {

		}

		binary_insn::binary_insn(binary_insn::kind k, shared_ptr<operand> d, shared_ptr<operand> s1, shared_ptr<operand> s2, shared_ptr<type> r, shared_ptr<variable_declaration> vdecl) : insn(insn::kind::KIND_BINARY),
			_src1_operand(s1), _src2_operand(s2), _dst_operand(d), _result_type(r), _variable_decl(vdecl), _binary_expr_kind(k), _is_variable_decl(true) {

		}

		binary_insn::~binary_insn() {

		}

		shared_ptr<operand> binary_insn::src1_operand() {
			return _src1_operand;
		}

		shared_ptr<operand> binary_insn::src2_operand() {
			return _src2_operand;
		}

		shared_ptr<operand> binary_insn::dst_operand() {
			return _dst_operand;
		}

		binary_insn::kind binary_insn::binary_expr_kind() {
			return _binary_expr_kind;
		}

		shared_ptr<type> binary_insn::result_type() {
			return _result_type;
		}

		bool binary_insn::is_variable_decl() {
			return _is_variable_decl;
		}

		shared_ptr<variable_declaration> binary_insn::variable_decl() {
			return _variable_decl;
		}

		string binary_insn::kind_to_string(binary_insn::kind k) {
			switch (k) {
			case binary_insn::kind::KIND_CADD:
				return "cadd";
			case binary_insn::kind::KIND_CBAND:
				return "cband";
			case binary_insn::kind::KIND_CBOR:
				return "cbor";
			case binary_insn::kind::KIND_CBXOR:
				return "cbxor";
			case binary_insn::kind::KIND_CDIV:
				return "cdiv";
			case binary_insn::kind::KIND_CEQ:
				return "ceq";
			case binary_insn::kind::KIND_CGE:
				return "cge";
			case binary_insn::kind::KIND_CGT:
				return "cgt";
			case binary_insn::kind::KIND_CLE:
				return "cle";
			case binary_insn::kind::KIND_CLT:
				return "clt";
			case binary_insn::kind::KIND_CMOD:
				return "cmod";
			case binary_insn::kind::KIND_CMUL:
				return "cmul";
			case binary_insn::kind::KIND_CNE:
				return "cne";
			case binary_insn::kind::KIND_CSHL:
				return "cshl";
			case binary_insn::kind::KIND_CSHR:
				return "cshr";
			case binary_insn::kind::KIND_CSHRA:
				return "cshra";
			case binary_insn::kind::KIND_CSUB:
				return "csub";
			case binary_insn::kind::KIND_DADD:
				return "dadd";
			case binary_insn::kind::KIND_DDIV:
				return "ddiv";
			case binary_insn::kind::KIND_DEQ:
				return "deq";
			case binary_insn::kind::KIND_DGE:
				return "dge";
			case binary_insn::kind::KIND_DGT:
				return "dgt";
			case binary_insn::kind::KIND_DLE:
				return "dle";
			case binary_insn::kind::KIND_DLT:
				return "dlt";
			case binary_insn::kind::KIND_DMUL:
				return "dmul";
			case binary_insn::kind::KIND_DNE:
				return "dne";
			case binary_insn::kind::KIND_DSUB:
				return "dsub";
			case binary_insn::kind::KIND_FADD:
				return "fadd";
			case binary_insn::kind::KIND_FDIV:
				return "fdiv";
			case binary_insn::kind::KIND_FEQ:
				return "feq";
			case binary_insn::kind::KIND_FGE:
				return "fge";
			case binary_insn::kind::KIND_FGT:
				return "fgt";
			case binary_insn::kind::KIND_FLE:
				return "fle";
			case binary_insn::kind::KIND_FLT:
				return "flt";
			case binary_insn::kind::KIND_FMUL:
				return "fmul";
			case binary_insn::kind::KIND_FNE:
				return "fne";
			case binary_insn::kind::KIND_FSUB:
				return "fsub";
			case binary_insn::kind::KIND_IADD:
				return "iadd";
			case binary_insn::kind::KIND_IBAND:
				return "iband";
			case binary_insn::kind::KIND_IBOR:
				return "ibor";
			case binary_insn::kind::KIND_IBXOR:
				return "ibxor";
			case binary_insn::kind::KIND_IDIV:
				return "idiv";
			case binary_insn::kind::KIND_IEQ:
				return "ieq";
			case binary_insn::kind::KIND_IGE:
				return "ige";
			case binary_insn::kind::KIND_IGT:
				return "igt";
			case binary_insn::kind::KIND_ILE:
				return "ile";
			case binary_insn::kind::KIND_ILT:
				return "ilt";
			case binary_insn::kind::KIND_IMOD:
				return "imod";
			case binary_insn::kind::KIND_IMUL:
				return "imul";
			case binary_insn::kind::KIND_INE:
				return "ine";
			case binary_insn::kind::KIND_ISHL:
				return "ishl";
			case binary_insn::kind::KIND_ISHR:
				return "ishr";
			case binary_insn::kind::KIND_ISHRA:
				return "ishra";
			case binary_insn::kind::KIND_ISUB:
				return "isub";
			case binary_insn::kind::KIND_LADD:
				return "ladd";
			case binary_insn::kind::KIND_LBAND:
				return "lband";
			case binary_insn::kind::KIND_LBOR:
				return "lbor";
			case binary_insn::kind::KIND_LBXOR:
				return "lbxor";
			case binary_insn::kind::KIND_LDIV:
				return "ldiv";
			case binary_insn::kind::KIND_LEQ:
				return "leq";
			case binary_insn::kind::KIND_LGE:
				return "lge";
			case binary_insn::kind::KIND_LGT:
				return "lgt";
			case binary_insn::kind::KIND_LLE:
				return "lle";
			case binary_insn::kind::KIND_LLT:
				return "llt";
			case binary_insn::kind::KIND_LMOD:
				return "lmod";
			case binary_insn::kind::KIND_LMUL:
				return "lmul";
			case binary_insn::kind::KIND_LNE:
				return "lne";
			case binary_insn::kind::KIND_LSHL:
				return "lshl";
			case binary_insn::kind::KIND_LSHR:
				return "lshr";
			case binary_insn::kind::KIND_LSHRA:
				return "lshra";
			case binary_insn::kind::KIND_LSUB:
				return "lsub";
			case binary_insn::kind::KIND_NONE:
				return "";
			case binary_insn::kind::KIND_SADD:
				return "sadd";
			case binary_insn::kind::KIND_SBAND:
				return "sband";
			case binary_insn::kind::KIND_SBOR:
				return "sbor";
			case binary_insn::kind::KIND_SBXOR:
				return "sbxor";
			case binary_insn::kind::KIND_SDIV:
				return "sdiv";
			case binary_insn::kind::KIND_SEQ:
				return "seq";
			case binary_insn::kind::KIND_SGE:
				return "sge";
			case binary_insn::kind::KIND_SGT:
				return "sgt";
			case binary_insn::kind::KIND_SLE:
				return "sle";
			case binary_insn::kind::KIND_SLT:
				return "slt";
			case binary_insn::kind::KIND_SMOD:
				return "smod";
			case binary_insn::kind::KIND_SMUL:
				return "smul";
			case binary_insn::kind::KIND_SNE:
				return "sne";
			case binary_insn::kind::KIND_SSHL:
				return "sshl";
			case binary_insn::kind::KIND_SSHR:
				return "sshr";
			case binary_insn::kind::KIND_SSHRA:
				return "sshra";
			case binary_insn::kind::KIND_SSUB:
				return "ssub";
			case binary_insn::kind::KIND_UCDIV:
				return "ucdiv";
			case binary_insn::kind::KIND_UCGE:
				return "ucge";
			case binary_insn::kind::KIND_UCGT:
				return "ucgt";
			case binary_insn::kind::KIND_UCLE:
				return "ucle";
			case binary_insn::kind::KIND_UCLT:
				return "uclt";
			case binary_insn::kind::KIND_UCMOD:
				return "ucmod";
			case binary_insn::kind::KIND_UCMUL:
				return "ucmul";
			case binary_insn::kind::KIND_UIDIV:
				return "uidiv";
			case binary_insn::kind::KIND_UIGE:
				return "uige";
			case binary_insn::kind::KIND_UIGT:
				return "uigt";
			case binary_insn::kind::KIND_UILE:
				return "uile";
			case binary_insn::kind::KIND_UILT:
				return "uilt";
			case binary_insn::kind::KIND_UIMOD:
				return "uimod";
			case binary_insn::kind::KIND_UIMUL:
				return "uimul";
			case binary_insn::kind::KIND_ULDIV:
				return "uldiv";
			case binary_insn::kind::KIND_ULGE:
				return "ulge";
			case binary_insn::kind::KIND_ULGT:
				return "ulgt";
			case binary_insn::kind::KIND_ULLE:
				return "ulle";
			case binary_insn::kind::KIND_ULLT:
				return "ullt";
			case binary_insn::kind::KIND_ULMOD:
				return "ulmod";
			case binary_insn::kind::KIND_ULMUL:
				return "ulmul";
			case binary_insn::kind::KIND_USDIV:
				return "usdiv";
			case binary_insn::kind::KIND_USGE:
				return "usge";
			case binary_insn::kind::KIND_USGT:
				return "usgt";
			case binary_insn::kind::KIND_USLE:
				return "usle";
			case binary_insn::kind::KIND_USLT:
				return "uslt";
			case binary_insn::kind::KIND_USMOD:
				return "usmod";
			case binary_insn::kind::KIND_USMUL:
				return "usmul";
			default:
				return "";
			}
			return "";
		}

		void binary_insn::deduplicate() {
			DEDUP(_dst_operand);
			DEDUP(_src1_operand);
			DEDUP(_src2_operand);
		}

		string binary_insn::to_string() {
			return kind_to_string(_binary_expr_kind) + " " + _dst_operand->to_string() + ", " + _src1_operand->to_string() + ", " + _src2_operand->to_string();
		}

		function_insn::function_insn(shared_ptr<function_type> ft, shared_ptr<function_symbol> fs, shared_ptr<function_scope> fsc, shared_ptr<register_operand> r)
			: insn(insn::kind::KIND_FUNCTION), _func_type(ft), _func_symbol(fs), _func_scope(fsc), _insn_list(vector<shared_ptr<insn>>{}), _return_register(r) {

		}

		function_insn::~function_insn() {

		}

		shared_ptr<function_type> function_insn::func_type() {
			return _func_type;
		}

		shared_ptr<function_symbol> function_insn::func_symbol() {
			return _func_symbol;
		}

		shared_ptr<function_scope> function_insn::func_scope() {
			return _func_scope;
		}

		vector<shared_ptr<insn>> function_insn::insn_list() {
			return _insn_list;
		}

		void function_insn::add_insn(shared_ptr<insn> i) {
			_insn_list.push_back(i);
		}

		shared_ptr<register_operand> function_insn::return_register() {
			return _return_register;
		}

		string function_insn::to_string() {
			string ret = "\n";
			for (shared_ptr<insn> i : _insn_list)
				ret += i->to_string() + "\n";
			return ret + "\n";
		}

		void function_insn::deduplicate() {
			for (auto& i : _insn_list)
				i->deduplicate();
		}

		unary_insn::unary_insn(unary_insn::kind k, shared_ptr<operand> d, shared_ptr<operand> s, shared_ptr<type> r) : insn(insn::kind::KIND_UNARY), _result_type(r), _unary_expr_kind(k), _src_operand(s), _dst_operand(d), _is_variable_decl(false),
			_variable_decl(nullptr) {

		}

		unary_insn::unary_insn(unary_insn::kind k, shared_ptr<operand> d, shared_ptr<operand> s, shared_ptr<type> r, shared_ptr<variable_declaration> vdecl) : insn(insn::kind::KIND_UNARY), _result_type(r), _unary_expr_kind(k), _src_operand(s),
			_dst_operand(d), _is_variable_decl(true), _variable_decl(vdecl) {

		}

		unary_insn::~unary_insn() {

		}

		shared_ptr<type> unary_insn::result_type() {
			return _result_type;
		}

		unary_insn::kind unary_insn::unary_expr_kind() {
			return _unary_expr_kind;
		}

		shared_ptr<operand> unary_insn::src_operand() {
			return _src_operand;
		}

		shared_ptr<operand> unary_insn::dst_operand() {
			return _dst_operand;
		}

		shared_ptr<variable_declaration> unary_insn::variable_decl() {
			return _variable_decl;
		}

		bool unary_insn::is_variable_decl() {
			return _is_variable_decl;
		}

		string unary_insn::kind_to_string(unary_insn::kind k) {
			switch (k) {
			case unary_insn::kind::KIND_LPLUS:
				return "lplus";
			case unary_insn::kind::KIND_IPLUS:
				return "iplus";
			case unary_insn::kind::KIND_SPLUS:
				return "splus";
			case unary_insn::kind::KIND_CPLUS:
				return "cplus";
			case unary_insn::kind::KIND_DPLUS:
				return "dplus";
			case unary_insn::kind::KIND_FPLUS:
				return "fplus";
			case unary_insn::kind::KIND_LMINUS:
				return "lminus";
			case unary_insn::kind::KIND_IMINUS:
				return "iminus";
			case unary_insn::kind::KIND_SMINUS:
				return "sminus";
			case unary_insn::kind::KIND_CMINUS:
				return "cminus";
			case unary_insn::kind::KIND_DMINUS:
				return "dminus";
			case unary_insn::kind::KIND_FMINUS:
				return "fminus";
			case unary_insn::kind::KIND_LNOT:
				return "lnot";
			case unary_insn::kind::KIND_LBNOT:
				return "lbnot";
			case unary_insn::kind::KIND_IBNOT:
				return "ibnot";
			case unary_insn::kind::KIND_SBNOT:
				return "sbnot";
			case unary_insn::kind::KIND_CBNOT:
				return "cbnot";
			case unary_insn::kind::KIND_LMOV:
				return "lmov";
			case unary_insn::kind::KIND_IMOV:
				return "imov";
			case unary_insn::kind::KIND_SMOV:
				return "smov";
			case unary_insn::kind::KIND_CMOV:
				return "cmov";
			case unary_insn::kind::KIND_DMOV:
				return "dmov";
			case unary_insn::kind::KIND_FMOV:
				return "fmov";
			case unary_insn::kind::KIND_RESV:
				return "resv";
			case unary_insn::kind::KIND_STK:
				return "stk";
			case unary_insn::kind::KIND_LTOD:
				return "ltod";
			case unary_insn::kind::KIND_LTOF:
				return "ltof";
			case unary_insn::kind::KIND_ULTOD:
				return "ultod";
			case unary_insn::kind::KIND_ULTOF:
				return "ultof";
			case unary_insn::kind::KIND_ITOD:
				return "itod";
			case unary_insn::kind::KIND_ITOF:
				return "itof";
			case unary_insn::kind::KIND_UITOD:
				return "uitod";
			case unary_insn::kind::KIND_UITOF:
				return "uitof";
			case unary_insn::kind::KIND_STOD:
				return "stod";
			case unary_insn::kind::KIND_STOF:
				return "stof";
			case unary_insn::kind::KIND_USTOD:
				return "ustod";
			case unary_insn::kind::KIND_USTOF:
				return "ustof";
			case unary_insn::kind::KIND_CTOD:
				return "ctod";
			case unary_insn::kind::KIND_CTOF:
				return "ctof";
			case unary_insn::kind::KIND_UCTOD:
				return "uctod";
			case unary_insn::kind::KIND_UCTOF:
				return "uctof";
			case unary_insn::kind::KIND_DTOL:
				return "dtol";
			case unary_insn::kind::KIND_DTOUL:
				return "dtoul";
			case unary_insn::kind::KIND_DTOI:
				return "dtoi";
			case unary_insn::kind::KIND_DTOUI:
				return "dtoui";
			case unary_insn::kind::KIND_DTOS:
				return "dtos";
			case unary_insn::kind::KIND_DTOUS:
				return "dtous";
			case unary_insn::kind::KIND_DTOC:
				return "dtoc";
			case unary_insn::kind::KIND_DTOUC:
				return "dtouc";
			case unary_insn::kind::KIND_DTOF:
				return "dtof";
			case unary_insn::kind::KIND_FTOL:
				return "ftol";
			case unary_insn::kind::KIND_FTOUL:
				return "ftoul";
			case unary_insn::kind::KIND_FTOI:
				return "ftoi";
			case unary_insn::kind::KIND_FTOUI:
				return "ftoui";
			case unary_insn::kind::KIND_FTOS:
				return "ftos";
			case unary_insn::kind::KIND_FTOUS:
				return "ftous";
			case unary_insn::kind::KIND_FTOC:
				return "ftoc";
			case unary_insn::kind::KIND_FTOUC:
				return "ftouc";
			case unary_insn::kind::KIND_FTOD:
				return "ftod";
			case unary_insn::kind::KIND_NONE:
				return "";
			default:
				return "";
			}
			return "";
		}

		void unary_insn::deduplicate() {
			DEDUP(_dst_operand);
			DEDUP(_src_operand);
		}

		string unary_insn::to_string() {
			return kind_to_string(_unary_expr_kind) + " " + _dst_operand->to_string() + ", " + _src_operand->to_string();
		}

		align_insn::align_insn(unsigned int a) : insn(insn::kind::KIND_ALIGN), _alignment(a) {

		}

		align_insn::~align_insn() {

		}

		unsigned int align_insn::alignment() {
			return _alignment;
		}

		string align_insn::to_string() {
			return "align " + std::to_string(_alignment);
		}

		void align_insn::deduplicate() {
			return;
		}

		memcpy_insn::memcpy_insn(shared_ptr<operand> d, shared_ptr<operand> s, unsigned int sz, shared_ptr<type> r) : insn(insn::kind::KIND_MEMCPY),
			_destination(d), _source(s), _size(sz), _result_type(r) {

		}

		memcpy_insn::~memcpy_insn() {

		}

		shared_ptr<operand> memcpy_insn::destination() {
			return _destination;
		}

		shared_ptr<operand> memcpy_insn::source() {
			return _source;
		}

		unsigned int memcpy_insn::size() {
			return _size;
		}

		shared_ptr<type> memcpy_insn::result_type() {
			return _result_type;
		}

		string memcpy_insn::to_string() {
			return "memcpy " + _destination->to_string() + ", " + _source->to_string() + ", " + std::to_string(_size);
		}

		void memcpy_insn::deduplicate() {
			DEDUP(_destination);
			DEDUP(_source);
		}

		ext_insn::ext_insn(kind k, shared_ptr<operand> lhs, shared_ptr<operand> rhs, unsigned int f, unsigned int t, shared_ptr<type> r) :
			insn(insn::kind::KIND_EXT), _ext_kind(k), _lhs(lhs), _rhs(rhs), _from(f), _to(t), _result_type(r) {

		}

		ext_insn::~ext_insn() {

		}

		shared_ptr<operand> ext_insn::lhs() {
			return _lhs;
		}

		shared_ptr<operand> ext_insn::rhs() {
			return _rhs;
		}

		unsigned int ext_insn::from() {
			return _from;
		}

		unsigned int ext_insn::to() {
			return _to;
		}

		shared_ptr<type> ext_insn::result_type() {
			return _result_type;
		}

		ext_insn::kind ext_insn::ext_kind() {
			return _ext_kind;
		}

		string ext_insn::to_string() {
			string which;
			switch (_ext_kind) {
			case ext_insn::kind::KIND_NONE:
				which = "";
				break;
			case ext_insn::kind::KIND_SEXT:
				which = "sext";
				break;
			case ext_insn::kind::KIND_ZEXT:
				which = "zext";
				break;
			default:
				which = "";
				break;
			}
			return which + " " + _lhs->to_string() + ", " + _rhs->to_string() + ", " + std::to_string(_from) + ", " + std::to_string(_to);
		}

		void ext_insn::deduplicate() {
			DEDUP(_lhs);
			DEDUP(_rhs);
		}

		trunc_insn::trunc_insn(shared_ptr<operand> lhs, shared_ptr<operand> rhs, unsigned int f, unsigned int t, shared_ptr<type> r) :
			insn(insn::kind::KIND_TRUNC), _lhs(lhs), _rhs(rhs), _from(f), _to(t), _result_type(r) {

		}

		trunc_insn::~trunc_insn() {

		}

		shared_ptr<operand> trunc_insn::lhs() {
			return _lhs;
		}

		shared_ptr<operand> trunc_insn::rhs() {
			return _rhs;
		}

		unsigned int trunc_insn::from() {
			return _from;
		}

		unsigned int trunc_insn::to() {
			return _to;
		}

		shared_ptr<type> trunc_insn::result_type() {
			return _result_type;
		}

		string trunc_insn::to_string() {
			return "trunc " + _lhs->to_string() + ", " + _rhs->to_string() + ", " + std::to_string(_from) + ", " + std::to_string(_to);
		}

		void trunc_insn::deduplicate() {
			DEDUP(_lhs);
			DEDUP(_rhs);
		}

		operand::operand(operand::kind k) : _operand_kind(k) {

		}

		operand::~operand() {

		}

		operand::kind operand::operand_kind() {
			return _operand_kind;
		}

		label_operand::label_operand(string lt, shared_ptr<type> t) : operand(operand::kind::KIND_LABEL), _label_text(lt), _result_type(t) {

		}

		label_operand::~label_operand() {

		}

		string label_operand::label_text() {
			return _label_text;
		}

		shared_ptr<type> label_operand::result_type() {
			return _result_type;
		}

		string label_operand::to_string() {
			return _label_text;
		}

		immediate_operand::immediate_operand(shared_ptr<type> t, variant<char, short, int, long, float, double, string> i) : operand(operand::kind::KIND_IMMEDIATE), _immediate(i), _immediate_type(t) {
			if (holds_alternative<char>(i))
				_immediate_operand_kind = immediate_operand::kind::KIND_CHAR;
			else if (holds_alternative<short>(i))
				_immediate_operand_kind = immediate_operand::kind::KIND_SHORT;
			else if (holds_alternative<int>(i))
				_immediate_operand_kind = immediate_operand::kind::KIND_INT;
			else if (holds_alternative<long>(i))
				_immediate_operand_kind = immediate_operand::kind::KIND_LONG;
			else if (holds_alternative<float>(i))
				_immediate_operand_kind = immediate_operand::kind::KIND_FLOAT;
			else if (holds_alternative<double>(i))
				_immediate_operand_kind = immediate_operand::kind::KIND_DOUBLE;
			else if (holds_alternative<string>(i))
				_immediate_operand_kind = immediate_operand::kind::KIND_STRING;
			else
				_immediate_operand_kind = immediate_operand::kind::KIND_NONE;
		}

		immediate_operand::immediate_operand(shared_ptr<type> t, char c) : operand(operand::kind::KIND_IMMEDIATE), _immediate(c), _immediate_operand_kind(immediate_operand::kind::KIND_CHAR), _immediate_type(t) {

		}

		immediate_operand::immediate_operand(shared_ptr<type> t, short s) : operand(operand::kind::KIND_IMMEDIATE), _immediate(s), _immediate_operand_kind(immediate_operand::kind::KIND_SHORT), _immediate_type(t) {

		}

		immediate_operand::immediate_operand(shared_ptr<type> t, int i) : operand(operand::kind::KIND_IMMEDIATE), _immediate(i), _immediate_operand_kind(immediate_operand::kind::KIND_INT), _immediate_type(t) {

		}

		immediate_operand::immediate_operand(shared_ptr<type> t, long l) : operand(operand::kind::KIND_IMMEDIATE), _immediate(l), _immediate_operand_kind(immediate_operand::kind::KIND_LONG), _immediate_type(t) {

		}

		immediate_operand::immediate_operand(shared_ptr<type> t, float f) : operand(operand::kind::KIND_IMMEDIATE), _immediate(f), _immediate_operand_kind(immediate_operand::kind::KIND_FLOAT), _immediate_type(t) {

		}

		immediate_operand::immediate_operand(shared_ptr<type> t, double d) : operand(operand::kind::KIND_IMMEDIATE), _immediate(d), _immediate_operand_kind(immediate_operand::kind::KIND_DOUBLE), _immediate_type(t) {

		}

		immediate_operand::immediate_operand(shared_ptr<type> t, string s) : operand(operand::kind::KIND_IMMEDIATE), _immediate(s), _immediate_operand_kind(immediate_operand::kind::KIND_STRING), _immediate_type(t) {

		}

		immediate_operand::~immediate_operand() {

		}

		char immediate_operand::immediate_char() {
			return get<char>(_immediate);
		}

		short immediate_operand::immediate_short() {
			return get<short>(_immediate);
		}

		int immediate_operand::immediate_int() {
			return get<int>(_immediate);
		}

		long immediate_operand::immediate_long() {
			return get<long>(_immediate);
		}

		float immediate_operand::immediate_float() {
			return get<float>(_immediate);
		}

		double immediate_operand::immediate_double() {
			return get<double>(_immediate);
		}

		string immediate_operand::immediate_string() {
			return get<string>(_immediate);
		}

		immediate_operand::kind immediate_operand::immediate_operand_kind() {
			return _immediate_operand_kind;
		}

		shared_ptr<type> immediate_operand::immediate_type() {
			return _immediate_type;
		}


		variant<char, short, int, long, float, double, string> immediate_operand::immediate() {
			return _immediate;
		}

		string immediate_operand::to_string() {
			switch (_immediate_operand_kind) {
			case immediate_operand::kind::KIND_CHAR:
				return std::to_string(get<char>(_immediate));
			case immediate_operand::kind::KIND_DOUBLE: {
				stringstream ss;
				ss << fixed << setprecision(numeric_limits<double>::max_digits10) << get<double>(_immediate);
				return ss.str();
			}
			case immediate_operand::kind::KIND_FLOAT: {
				stringstream ss;
				ss << fixed << setprecision(numeric_limits<double>::max_digits10) << get<float>(_immediate);
				return ss.str();
			}
			case immediate_operand::kind::KIND_INT:
				return std::to_string(get<int>(_immediate));
			case immediate_operand::kind::KIND_LONG:
				return std::to_string(get<long>(_immediate));
			case immediate_operand::kind::KIND_NONE:
				return "";
			case immediate_operand::kind::KIND_SHORT:
				return std::to_string(get<short>(_immediate));
			case immediate_operand::kind::KIND_STRING:
				return '"' + get<string>(_immediate) + '"';
			default:
				return "";
			}
			return "";
		}

		register_operand::register_operand(bool d, int vrn, shared_ptr<type> rt) : operand(operand::kind::KIND_REGISTER), _dereference(d),
			_virtual_register_number(vrn), _register_type(rt), _original_virtual_register_number(vrn), _base_virtual_register_number(vrn) {

		}

		register_operand::~register_operand() {

		}

		int register_operand::virtual_register_number() {
			return _virtual_register_number;
		}

		void register_operand::set_virtual_register_number(int vrn, bool overwrite) {
			_virtual_register_number = vrn;
			if (overwrite)
				_original_virtual_register_number = _virtual_register_number;
		}

		void register_operand::set_base_virtual_register_number(int vrn) {
			_base_virtual_register_number = vrn;
		}

		int register_operand::original_virtual_register_number() {
			return _original_virtual_register_number;
		}

		int register_operand::base_virtual_register_number() {
			return _base_virtual_register_number;
		}

		shared_ptr<type> register_operand::register_type() {
			return _register_type;
		}

		void register_operand::set_register_type(shared_ptr<type> t) {
			_register_type = t;
		}

		bool register_operand::dereference() {
			return _dereference;
		}

		void register_operand::set_dereference(bool b) {
			_dereference = b;
		}

		string register_operand::to_string() {
			return string(_dereference ? "*" : "") + "r" + std::to_string(_virtual_register_number);
		}

		middle_ir::middle_ir(shared_ptr<spectre::parser::parser> p) : _ast_parser(p), _mangled_name_2_ast_map(map<string, shared_ptr<variable_declaration>>{}), _insn_list(vector<shared_ptr<insn>>{}), _virtual_register_counter(0), _misc_counter(0),
			_label_counter(0), _loop_start_stack(stack<shared_ptr<label_operand>>{}), _loop_end_stack(stack<shared_ptr<label_operand>>{}), _function_ctx(nullptr),
			_function_ctx_set(false), _mangled_name_2_register_map(map<string, shared_ptr<register_operand>>{}), _return_label(nullptr), _return_register(nullptr),
			_global_insn_list(vector<shared_ptr<insn>>{}), _global_ctx_set(false), _mangled_name_2_constexpr_value_map(map<string, variant<bool, int, unsigned int, float, double, string>>{}),
			_function_insn_list(vector<shared_ptr<function_insn>>{}), _register_2_type(unordered_map<int, shared_ptr<type>>{}) {

		}

		middle_ir::~middle_ir() {

		}

		void middle_ir::add_register_type(int j, shared_ptr<type> t) {
			if (_register_2_type.find(j) != _register_2_type.end())
				return;
			_register_2_type[j] = t;
		}

		shared_ptr<type> middle_ir::get_register_type(int j) {
			if (_register_2_type.find(j) == _register_2_type.end())
				return nullptr;
			return _register_2_type[j];
		}

		void middle_ir::add_function_insn(shared_ptr<function_insn> fi) {
			_function_insn_list.push_back(fi);
		}

		vector<shared_ptr<function_insn>> middle_ir::function_insn_list() {
			return _function_insn_list;
		}

		shared_ptr<struct_symbol> middle_ir::find_struct_symbol(token t, int r) {
			return _ast_parser->find_struct_symbol(t, r);
		}

		vector<shared_ptr<insn>> middle_ir::insn_list() {
			return _insn_list;
		}

		void middle_ir::set_insn_list(vector<shared_ptr<insn>> il) {
			_insn_list = il;
		}

		void middle_ir::set_global_insn_list(vector<shared_ptr<insn>> il) {
			_global_insn_list = il;
		}

		void middle_ir::clear_insn_list() {
			_insn_list.clear();
		}

		void middle_ir::clear_global_insn_list() {
			_global_insn_list.clear();
		}

		shared_ptr<variable_declaration> middle_ir::lookup_variable(string mangled_name) {
			if (_mangled_name_2_ast_map.find(mangled_name) == _mangled_name_2_ast_map.end()) return nullptr;
			return _mangled_name_2_ast_map[mangled_name];
		}

		void middle_ir::add_variable(string mangled_name, shared_ptr<variable_declaration> vdecl) {
			_mangled_name_2_ast_map[mangled_name] = vdecl;
		}

		void middle_ir::report_internal(string msg, string fn, int ln, string fl) {
			_ast_parser->report_internal(msg, fn, ln, fl);
		}

		void middle_ir::report(error e) {
			_ast_parser->report(e);
		}

		void middle_ir::push_loop_start_stack(shared_ptr<label_operand> l) {
			_loop_start_stack.push(l);
		}

		void middle_ir::push_loop_end_stack(shared_ptr<label_operand> l) {
			_loop_end_stack.push(l);
		}

		void middle_ir::pop_loop_end_stack() {
			_loop_end_stack.pop();
		}

		void middle_ir::pop_loop_start_stack() {
			_loop_start_stack.pop();
		}

		shared_ptr<label_operand> middle_ir::loop_start() {
			return _loop_start_stack.top();
		}

		shared_ptr<label_operand> middle_ir::loop_end() {
			return _loop_end_stack.top();
		}

		void middle_ir::set_function_ctx(shared_ptr<function_insn> f) {
			_function_ctx = f;
			_function_ctx_set = true;
		}

		void middle_ir::unset_function_ctx() {
			_function_ctx = nullptr;
			_function_ctx_set = false;
		}

		shared_ptr<function_insn> middle_ir::get_function_ctx() {
			return _function_ctx;
		}

		bool middle_ir::function_ctx_set() {
			return _function_ctx_set;
		}

		void middle_ir::set_return_label(shared_ptr<label_operand> l) {
			_return_label = l;
		}

		shared_ptr<label_operand> middle_ir::get_return_label() {
			return _return_label;
		}

		void middle_ir::set_return_register(shared_ptr<register_operand> l) {
			_return_register = l;
		}

		shared_ptr<register_operand> middle_ir::get_return_register() {
			return _return_register;
		}

		void middle_ir::add_insn(shared_ptr<insn> i) {
			if (_function_ctx_set) _function_ctx->add_insn(i);
			else _global_ctx_set ? _global_insn_list.push_back(i) : _insn_list.push_back(i);
		}

		void middle_ir::map_variable_register(string mangled_name, shared_ptr<register_operand> op) {
			_mangled_name_2_register_map[mangled_name] = op;
		}

		shared_ptr<register_operand> middle_ir::lookup_variable_register(string mangled_name) {
			if (_mangled_name_2_register_map.find(mangled_name) == _mangled_name_2_register_map.end())
				return nullptr;
			return _mangled_name_2_register_map[mangled_name];
		}

		pair<bool, variant<bool, int, unsigned int, float, double, string>> middle_ir::lookup_constexpr_mapping(string mangled_name) {
			variant<bool, int, unsigned int, float, double, string> dummy;
			if (_mangled_name_2_constexpr_value_map.find(mangled_name) == _mangled_name_2_constexpr_value_map.end())
				return make_pair(false, dummy);
			return make_pair(true, _mangled_name_2_constexpr_value_map[mangled_name]);
		}

		void middle_ir::add_constexpr_mapping(string mangled_name, variant<bool, int, unsigned int, float, double, string> c) {
			_mangled_name_2_constexpr_value_map[mangled_name] = c;
		}

		shared_ptr<register_operand> middle_ir::generate_next_register(shared_ptr<type> t) {
			shared_ptr<register_operand> ro = make_shared<register_operand>(false, _virtual_register_counter++, t);
			_register_2_type[ro->virtual_register_number()] = t;
			return ro;
		}

		shared_ptr<label_operand> middle_ir::generate_next_label_operand() {
			return make_shared<label_operand>(LABEL_PREFIX + to_string(_label_counter++), raw_pointer_type);
		}

		void middle_ir::add_global_insn(shared_ptr<insn> i) {
			_global_insn_list.push_back(i);
		}

		vector<shared_ptr<insn>> middle_ir::global_insn_list() {
			return _global_insn_list;
		}

		void middle_ir::set_global_ctx_set(bool b) {
			_global_ctx_set = b;
		}

		bool middle_ir::get_global_ctx_set() {
			return _global_ctx_set;
		}

		int middle_ir::get_num_allocated_registers() {
			return _virtual_register_counter;
		}

		vector<string> middle_ir::variable_names() {
			vector<string> ret;
			for (auto[k, v] : _mangled_name_2_register_map)
				ret.push_back(k);
			return ret;
		}

		vector<string> middle_ir::constexpr_variable_names() {
			vector<string> ret;
			for (auto[k, v] : _mangled_name_2_constexpr_value_map)
				ret.push_back(k);
			return ret;
		}

		int middle_ir::next_virtual_register_number() {
			return _virtual_register_counter++;
		}

		int middle_ir::next_misc_counter() {
			return _misc_counter++;
		}

		int middle_ir::get_virtual_register_number() {
			return _virtual_register_counter;
		}

		string c_scope_2_string_helper(shared_ptr<middle_ir> mi, shared_ptr<scope> s, string r) {
			if (s == nullptr) return r;
			switch (s->scope_kind()) {
			case scope::kind::KIND_GLOBAL:
				return GLOBAL_SCOPE_PREFIX + r;
				break;
			case scope::kind::KIND_BLOCK:
				return c_scope_2_string_helper(mi, s->parent_scope(), "b" + to_string(static_pointer_cast<block_scope>(s)->block_number()) + "$_" + r);
				break;
			case scope::kind::KIND_FUNCTION:
				return c_scope_2_string_helper(mi, s->parent_scope(), static_pointer_cast<function_scope>(s)->function_name().raw_text() + "_" + r);
				break;
			case scope::kind::KIND_LOOP:
				return c_scope_2_string_helper(mi, s->parent_scope(), "l$_" + r);
				break;
			case scope::kind::KIND_NAMESPACE:
				return c_scope_2_string_helper(mi, s->parent_scope(), static_pointer_cast<namespace_scope>(s)->namespace_name().raw_text() + "_" + r);
				break;
			case scope::kind::KIND_SWITCH:
				return c_scope_2_string_helper(mi, s->parent_scope(), "s$_" + r);
				break;
			}
			mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return "";
		}

		string c_scope_2_string(shared_ptr<middle_ir> mi, shared_ptr<scope> s) {
			return c_scope_2_string_helper(mi, s, "");
		}

		string c_symbol_2_string(shared_ptr<middle_ir> mi, shared_ptr<symbol> s) {
			if (s == nullptr) return "";
			string parent_scope_string = c_scope_2_string(mi, s->parent_scope());
			string scope_prefix = parent_scope_string == "_" ? "_" : parent_scope_string + "_";
			switch (s->symbol_kind()) {
			case symbol::kind::KIND_FUNCTION:
				return scope_prefix + static_pointer_cast<function_symbol>(s)->function_name().raw_text();
				break;
			case symbol::kind::KIND_NAMESPACE:
				return scope_prefix + static_pointer_cast<namespace_symbol>(s)->namespace_symbol_name().raw_text();
				break;
			case symbol::kind::KIND_STRUCT:
				return scope_prefix + static_pointer_cast<struct_symbol>(s)->struct_name().raw_text();
				break;
			case symbol::kind::KIND_VARIABLE:
				return scope_prefix + static_pointer_cast<variable_symbol>(s)->variable_name().raw_text();
				break;
			}
			mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return "";
		}

		bool is_double_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			if (t->type_array_kind() == type::array_kind::KIND_ARRAY || t->type_kind() != type::kind::KIND_PRIMITIVE)
				return false;
			shared_ptr<primitive_type> pr = static_pointer_cast<primitive_type>(t);
			return pr->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE;
		}

		bool is_float_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			if (t->type_array_kind() == type::array_kind::KIND_ARRAY || t->type_kind() != type::kind::KIND_PRIMITIVE)
				return false;
			shared_ptr<primitive_type> pr = static_pointer_cast<primitive_type>(t);
			return pr->primitive_type_kind() == primitive_type::kind::KIND_FLOAT;
		}

		bool is_floating_point_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return is_double_type(mi, t) || is_float_type(mi, t);
		}

		bool is_struct_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return t->type_array_kind() != type::array_kind::KIND_ARRAY && t->type_kind() == type::kind::KIND_STRUCT;
		}

		bool is_function_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return t->type_array_kind() != type::array_kind::KIND_ARRAY && t->type_kind() == type::kind::KIND_FUNCTION;
		}

		bool is_void_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<primitive_type> pr = static_pointer_cast<primitive_type>(t);
			return pr->type_array_kind() != type::array_kind::KIND_ARRAY && pr->primitive_type_kind() == primitive_type::kind::KIND_VOID;
		}

		bool is_bool_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<primitive_type> pr = static_pointer_cast<primitive_type>(t);
			return pr->type_array_kind() != type::array_kind::KIND_ARRAY && pr->primitive_type_kind() == primitive_type::kind::KIND_BOOL;
		}

		bool is_char_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<primitive_type> pr = static_pointer_cast<primitive_type>(t);
			return pr->type_array_kind() != type::array_kind::KIND_ARRAY && pr->primitive_type_kind() == primitive_type::kind::KIND_CHAR;
		}

		bool is_byte_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<primitive_type> pr = static_pointer_cast<primitive_type>(t);
			return pr->type_array_kind() != type::array_kind::KIND_ARRAY && pr->primitive_type_kind() == primitive_type::kind::KIND_BYTE;
		}

		bool is_short_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<primitive_type> pr = static_pointer_cast<primitive_type>(t);
			return pr->type_array_kind() != type::array_kind::KIND_ARRAY && pr->primitive_type_kind() == primitive_type::kind::KIND_SHORT;
		}

		bool is_int_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<primitive_type> pr = static_pointer_cast<primitive_type>(t);
			return pr->type_array_kind() != type::array_kind::KIND_ARRAY && pr->primitive_type_kind() == primitive_type::kind::KIND_INT;
		}

		bool is_long_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<primitive_type> pr = static_pointer_cast<primitive_type>(t);
			return pr->type_array_kind() != type::array_kind::KIND_ARRAY && pr->primitive_type_kind() == primitive_type::kind::KIND_LONG;
		}

		bool is_pointer_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return t->type_array_kind() == type::array_kind::KIND_ARRAY || t->type_kind() == type::kind::KIND_FUNCTION;
		}

		bool is_integral_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return is_bool_type(mi, t) || is_char_type(mi, t) || is_short_type(mi, t) || is_int_type(mi, t) || is_long_type(mi, t)
				|| is_pointer_type(mi, t);
		}

		bool is_signed_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			if (t->type_kind() != type::kind::KIND_PRIMITIVE || t->type_array_kind() == type::array_kind::KIND_ARRAY)
				return false;
			return static_pointer_cast<primitive_type>(t)->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_SIGNED;
		}

		bool is_unsigned_type(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			if (t->type_kind() != type::kind::KIND_PRIMITIVE || t->type_array_kind() == type::array_kind::KIND_ARRAY)
				return false;
			return static_pointer_cast<primitive_type>(t)->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED;
		}

		shared_ptr<register_operand> to_register_operand(shared_ptr<middle_ir> mi, shared_ptr<operand> i) {
			if (i == nullptr || i->operand_kind() == operand::kind::KIND_NONE)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			if (i->operand_kind() == operand::kind::KIND_REGISTER)
				return static_pointer_cast<register_operand>(i);
			else if (i->operand_kind() == operand::kind::KIND_LABEL) {
				shared_ptr<label_operand> lab = static_pointer_cast<label_operand>(i);
				shared_ptr<register_operand> reg = mi->generate_next_register(lab->result_type());
				mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_IMOV, reg, lab, lab->result_type()));
				return reg;
			}
			shared_ptr<immediate_operand> io = static_pointer_cast<immediate_operand>(i);
			shared_ptr<type> imm_type = io->immediate_type();
			shared_ptr<register_operand> reg = mi->generate_next_register(imm_type);
			unary_insn::kind uk = unary_insn::kind::KIND_NONE;
			if (is_double_type(mi, imm_type))
				uk = unary_insn::kind::KIND_DMOV;
			else if (is_float_type(mi, imm_type))
				uk = unary_insn::kind::KIND_FMOV;
			else if (is_long_type(mi, imm_type))
				uk = unary_insn::kind::KIND_LMOV;
			else if (is_int_type(mi, imm_type) || is_pointer_type(mi, imm_type))
				uk = unary_insn::kind::KIND_IMOV;
			else if (is_short_type(mi, imm_type))
				uk = unary_insn::kind::KIND_SMOV;
			else if (is_bool_type(mi, imm_type) || is_char_type(mi, imm_type) || is_byte_type(mi, imm_type))
				uk = unary_insn::kind::KIND_CMOV;
			else
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			mi->add_insn(make_shared<unary_insn>(uk, reg, io, imm_type));
			return reg;
		}

		shared_ptr<size_data> spectre_sizeof(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			if (t->type_array_kind() == type::array_kind::KIND_ARRAY || t->type_kind() == type::kind::KIND_FUNCTION)
				return make_shared<size_data>(size_data{ SIZEOF_POINTER, SIZEOF_POINTER });
			if (t->type_kind() == type::kind::KIND_PRIMITIVE) {
				shared_ptr<primitive_type> pr = static_pointer_cast<primitive_type>(t);
				switch (pr->primitive_type_kind()) {
				case primitive_type::kind::KIND_VOID:
					return make_shared<size_data>(size_data{ SIZEOF_VOID, SIZEOF_VOID });
				case primitive_type::kind::KIND_BYTE:
					return make_shared<size_data>(size_data{ SIZEOF_BYTE, SIZEOF_BYTE });
				case primitive_type::kind::KIND_CHAR:
					return make_shared<size_data>(size_data{ SIZEOF_CHAR, SIZEOF_CHAR });
				case primitive_type::kind::KIND_DOUBLE:
					return make_shared<size_data>(size_data{ SIZEOF_DOUBLE, SIZEOF_DOUBLE });
				case primitive_type::kind::KIND_FLOAT:
					return make_shared<size_data>(size_data{ SIZEOF_FLOAT, SIZEOF_FLOAT });
				case primitive_type::kind::KIND_INT:
					return make_shared<size_data>(size_data{ SIZEOF_INT, SIZEOF_INT });
				case primitive_type::kind::KIND_LONG:
					return make_shared<size_data>(size_data{ SIZEOF_LONG, SIZEOF_LONG });
				case primitive_type::kind::KIND_SHORT:
					return make_shared<size_data>(size_data{ SIZEOF_SHORT, SIZEOF_SHORT });
				case primitive_type::kind::KIND_BOOL:
					return make_shared<size_data>(size_data{ SIZEOF_BOOL, SIZEOF_BOOL });
				default:
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					break;
				}
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			else if (t->type_kind() == type::kind::KIND_STRUCT) {
				shared_ptr<struct_type> st = static_pointer_cast<struct_type>(t);
				shared_ptr<struct_symbol> ss = mi->find_struct_symbol(st->struct_name(), st->struct_reference_number());
				vector<shared_ptr<size_data>> member_data;
				unsigned int size = 0;
				for (const shared_ptr<symbol> s : ss->struct_symbol_scope()->symbol_list()) {
					switch (s->symbol_kind()) {
					case symbol::kind::KIND_VARIABLE: {
						shared_ptr<variable_symbol> varsym = static_pointer_cast<variable_symbol>(s);
						shared_ptr<size_data> memsize = spectre_sizeof(mi, varsym->variable_type());
						if (size % memsize->alignment != 0)
							size += memsize->alignment - (size % memsize->alignment); // padding
						size += memsize->size;
						member_data.push_back(memsize);
					}
						break;
					default:
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
				}
				if (!member_data.empty()) {
					unsigned int alignment = (*max_element(member_data.begin(), member_data.end(),
						[](const shared_ptr<size_data>& sd1, const shared_ptr<size_data>& sd2) {
						return sd1->alignment < sd2->alignment;
					}))->alignment;
					if (st->is_union())
						return (*max_element(member_data.begin(), member_data.end(),
							[](const shared_ptr<size_data>& sd1, const shared_ptr<size_data>& sd2) {
							return sd1->size < sd2->size;
						}));
					else {
						if (size % alignment != 0)
							size += alignment - (size % alignment); // padding at the end
						return make_shared<size_data>(size_data{ size, alignment });
					}
				}
				else
					return make_shared<size_data>(size_data{ 1, 1 }); // empty structs and unions take up a byte and are byte-aligned
			}
			mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		unordered_map<string, unsigned int> member_offsets(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			if (t == nullptr || t->type_kind() != type::kind::KIND_STRUCT)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			unordered_map<string, unsigned int> ret;
			shared_ptr<struct_type> st = static_pointer_cast<struct_type>(t);
			shared_ptr<struct_symbol> ss = mi->find_struct_symbol(st->struct_name(), st->struct_reference_number());
			unsigned int size = 0;
			for (const shared_ptr<symbol>& memsym : ss->struct_symbol_scope()->symbol_list()) {
				switch (memsym->symbol_kind()) {
				case symbol::kind::KIND_VARIABLE: {
					shared_ptr<variable_symbol> varsym = static_pointer_cast<variable_symbol>(memsym);
					shared_ptr<size_data> sd = spectre_sizeof(mi, varsym->variable_type());
					if (size % sd->alignment != 0)
						size += sd->alignment - (size % sd->alignment);
					ret[varsym->variable_name().raw_text()] = size;
					size += sd->size;
				}
					break;
				default:
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					break;
				}
			}
			return ret;
		}

		shared_ptr<operand> ir_cast(shared_ptr<middle_ir> mi, shared_ptr<operand> src, shared_ptr<type> from, shared_ptr<type> to) {
			if (src == nullptr || from == nullptr || to == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			auto generate_conversion = [&](unary_insn::kind op) {
				shared_ptr<operand> cvt = mi->generate_next_register(to);
				mi->add_insn(make_shared<unary_insn>(op, cvt, src, to));
				return cvt;
			};
			auto generate_trunc = [&](unsigned int f, unsigned int t) {
				shared_ptr<operand> cvt = mi->generate_next_register(to);
				mi->add_insn(make_shared<trunc_insn>(cvt, src, f, t, to));
				return cvt;
			};
			auto generate_ext = [&](ext_insn::kind ek, unsigned int f, unsigned int t) {
				shared_ptr<operand> cvt = mi->generate_next_register(to);
				mi->add_insn(make_shared<ext_insn>(ek, cvt, src, f, t, to));
				return cvt;
			};
			auto generate_bool_test = [&](binary_insn::kind op, shared_ptr<immediate_operand> imm) {
				shared_ptr<operand> cvt = mi->generate_next_register(bool_type);
				mi->add_insn(make_shared<binary_insn>(op, cvt, src, imm, bool_type));
				return cvt;
			};
			auto copy_src_with_final_type = [&] () {
				if (src->operand_kind() == operand::kind::KIND_REGISTER) {
					shared_ptr<register_operand> src_reg = static_pointer_cast<register_operand>(src);
					src_reg = make_shared<register_operand>(*src_reg);
					src_reg->set_register_type(to);
					src = src_reg;
				}
				return src;
			};

			if (is_double_type(mi, from) || is_float_type(mi, from)) {
				bool f = is_float_type(mi, from);
				if (is_double_type(mi, to)) {
					if (f) return generate_conversion(unary_insn::kind::KIND_FTOD);
					else return copy_src_with_final_type();
				}
				else if (is_float_type(mi, to)) {
					if (f) return copy_src_with_final_type();
					else return generate_conversion(unary_insn::kind::KIND_DTOF);
				}
				else if (is_long_type(mi, to)) {
					bool u = is_unsigned_type(mi, to);
					if (f) return generate_conversion(u ? unary_insn::kind::KIND_FTOUL : unary_insn::kind::KIND_FTOL);
					else return generate_conversion(u ? unary_insn::kind::KIND_DTOUL : unary_insn::kind::KIND_DTOL);
				}
				else if (is_int_type(mi, to) || is_pointer_type(mi, to) || is_struct_type(mi, to)) {
					bool u = (is_pointer_type(mi, to) || is_struct_type(mi, to)) || is_unsigned_type(mi, to);
					if (f) return generate_conversion(u ? unary_insn::kind::KIND_FTOUI : unary_insn::kind::KIND_FTOI);
					else return generate_conversion(u ? unary_insn::kind::KIND_DTOUI : unary_insn::kind::KIND_DTOI);
				}
				else if (is_short_type(mi, to)) {
					bool u = is_unsigned_type(mi, to);
					if (f) return generate_conversion(u ? unary_insn::kind::KIND_FTOUS : unary_insn::kind::KIND_FTOS);
					else return generate_conversion(u ? unary_insn::kind::KIND_DTOUS : unary_insn::kind::KIND_DTOS);
				}
				else if (is_char_type(mi, to) || is_byte_type(mi, to)) {
					bool u = is_unsigned_type(mi, to);
					if (f) return generate_conversion(u ? unary_insn::kind::KIND_FTOUC : unary_insn::kind::KIND_FTOC);
					else return generate_conversion(u ? unary_insn::kind::KIND_DTOUC : unary_insn::kind::KIND_DTOC);
				}
				else if (is_bool_type(mi, to)) {
					if (f) return generate_bool_test(binary_insn::kind::KIND_FNE, make_shared<immediate_operand>(from, (float)0));
					else return generate_bool_test(binary_insn::kind::KIND_DNE, make_shared<immediate_operand>(from, (double)0));
				}
				else if (is_void_type(mi, to))
					return copy_src_with_final_type();
				else
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			else if (is_long_type(mi, from)) {
				bool u = is_unsigned_type(mi, from);
				if (is_double_type(mi, to))
					return generate_conversion(u ? unary_insn::kind::KIND_ULTOD : unary_insn::kind::KIND_LTOD);
				else if (is_float_type(mi, to))
					return generate_conversion(u ? unary_insn::kind::KIND_ULTOF : unary_insn::kind::KIND_LTOF);
				else if (is_long_type(mi, to))
					return copy_src_with_final_type();
				else if (is_int_type(mi, to) || is_pointer_type(mi, to) || is_struct_type(mi, to))
					return generate_trunc(SIZEOF_LONG, SIZEOF_INT);
				else if (is_short_type(mi, to))
					return generate_trunc(SIZEOF_LONG, SIZEOF_SHORT);
				else if (is_char_type(mi, to) || is_byte_type(mi, to))
					return generate_trunc(SIZEOF_LONG, SIZEOF_CHAR);
				else if (is_bool_type(mi, to))
					return generate_bool_test(binary_insn::kind::KIND_LNE, make_shared<immediate_operand>(from, (long)0));
				else if (is_void_type(mi, to))
					return copy_src_with_final_type();
				else
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			else if (is_int_type(mi, from) || is_pointer_type(mi, from) || is_struct_type(mi, from)) {
				bool fu = (is_struct_type(mi, from) || is_pointer_type(mi, from)) || is_unsigned_type(mi, from);
				if (is_double_type(mi, to))
					return generate_conversion(fu ? unary_insn::kind::KIND_UITOD : unary_insn::kind::KIND_ITOD);
				else if (is_float_type(mi, to))
					return generate_conversion(fu ? unary_insn::kind::KIND_UITOF : unary_insn::kind::KIND_ITOF);
				else if (is_long_type(mi, to)) {
					bool tu = is_unsigned_type(mi, to);
					return generate_ext(tu ? ext_insn::kind::KIND_ZEXT : ext_insn::kind::KIND_SEXT, SIZEOF_INT, SIZEOF_LONG);
				}
				else if (is_int_type(mi, to) || is_struct_type(mi, to) || is_pointer_type(mi, to))
					return copy_src_with_final_type();
				else if (is_short_type(mi, to))
					return generate_trunc(SIZEOF_INT, SIZEOF_SHORT);
				else if (is_char_type(mi, to) || is_byte_type(mi, to))
					return generate_trunc(SIZEOF_INT, SIZEOF_CHAR);
				else if (is_void_type(mi, to))
					return copy_src_with_final_type();
				else if (is_bool_type(mi, to))
					return generate_bool_test(binary_insn::kind::KIND_INE, make_shared<immediate_operand>(from, (int)0));
				else
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			else if (is_short_type(mi, from)) {
				bool fu = is_unsigned_type(mi, from);
				if (is_double_type(mi, to))
					return generate_conversion(fu ? unary_insn::kind::KIND_USTOD : unary_insn::kind::KIND_STOD);
				else if (is_float_type(mi, from))
					return generate_conversion(fu ? unary_insn::kind::KIND_USTOF : unary_insn::kind::KIND_STOF);
				else if (is_long_type(mi, to)) {
					bool tu = is_unsigned_type(mi, to);
					return generate_ext(tu ? ext_insn::kind::KIND_ZEXT : ext_insn::kind::KIND_SEXT, SIZEOF_SHORT, SIZEOF_LONG);
				}
				else if (is_int_type(mi, to) || is_pointer_type(mi, to) || is_struct_type(mi, to)) {
					bool tu = (is_pointer_type(mi, to) || is_struct_type(mi, to)) || is_unsigned_type(mi, to);
					return generate_ext(tu ? ext_insn::kind::KIND_ZEXT : ext_insn::kind::KIND_SEXT, SIZEOF_SHORT, SIZEOF_INT);
				}
				else if (is_short_type(mi, to))
					return copy_src_with_final_type();
				else if (is_char_type(mi, to) || is_byte_type(mi, to))
					return generate_trunc(SIZEOF_SHORT, SIZEOF_CHAR);
				else if (is_void_type(mi, to))
					return copy_src_with_final_type();
				else if (is_bool_type(mi, to))
					return generate_bool_test(binary_insn::kind::KIND_SNE, make_shared<immediate_operand>(from, (short)0));
				else
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			else if (is_char_type(mi, from) || is_byte_type(mi, from) || is_bool_type(mi, from)) {
				bool fu = is_bool_type(mi, from) || is_unsigned_type(mi, from);
				if (is_double_type(mi, to))
					return generate_conversion(fu ? unary_insn::kind::KIND_UCTOD : unary_insn::kind::KIND_CTOD);
				else if (is_float_type(mi, to))
					return generate_conversion(fu ? unary_insn::kind::KIND_UCTOF : unary_insn::kind::KIND_CTOF);
				else if (is_long_type(mi, to)) {
					bool tu = is_unsigned_type(mi, to);
					return generate_ext(tu ? ext_insn::kind::KIND_ZEXT : ext_insn::kind::KIND_SEXT, SIZEOF_CHAR, SIZEOF_LONG);
				}
				else if (is_int_type(mi, to) || is_pointer_type(mi, to) || is_struct_type(mi, to)) {
					bool tu = (is_pointer_type(mi, to) || is_struct_type(mi, to)) || is_unsigned_type(mi, to);
					return generate_ext(tu ? ext_insn::kind::KIND_ZEXT : ext_insn::kind::KIND_SEXT, SIZEOF_CHAR, SIZEOF_INT);
				}
				else if (is_short_type(mi, to)) {
					bool tu = is_unsigned_type(mi, to);
					return generate_ext(tu ? ext_insn::kind::KIND_ZEXT : ext_insn::kind::KIND_SEXT, SIZEOF_CHAR, SIZEOF_SHORT);
				}
				else if (is_char_type(mi, to) || is_byte_type(mi, to))
					return copy_src_with_final_type();
				else if (is_void_type(mi, to))
					return copy_src_with_final_type();
				else if (is_bool_type(mi, to)) {
					if (is_char_type(mi, from) || is_byte_type(mi, from))
						return generate_bool_test(binary_insn::kind::KIND_CNE, make_shared<immediate_operand>(from, (char)0));
					else
						return copy_src_with_final_type();
				}
				else
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			else if (is_void_type(mi, from))
				return copy_src_with_final_type();
			else
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<register_operand> load_lvalue_to_register(shared_ptr<middle_ir> mi, shared_ptr<operand> op) {
			if (op == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<register_operand> ret = nullptr;
			switch (op->operand_kind()) {
			case operand::kind::KIND_REGISTER:
				ret = make_shared<register_operand>(*static_pointer_cast<register_operand>(op));
				ret->set_dereference(true);
				ret->set_register_type(dereference(ret->register_type()));
				break;
			case operand::kind::KIND_IMMEDIATE:
				ret = to_register_operand(mi, op);
				break;
			case operand::kind::KIND_LABEL:
				ret = to_register_operand(mi, op);
				ret->set_dereference(true);
				ret->set_register_type(dereference(ret->register_type()));
				break;
			default:
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				break;
			}
			return ret;
		}

		bool is_function_main(shared_ptr<middle_ir> mi, shared_ptr<function_symbol> fsym) {
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

		tuple<vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>> reorder_stmt_list(shared_ptr<middle_ir> mi, vector<shared_ptr<stmt>> sl) {
			vector<shared_ptr<stmt>> reordered;
			vector<shared_ptr<stmt>> _vdecls, _functions, _main, _rest;
			for (shared_ptr<stmt> s : sl) {
				if (s->stmt_kind() == stmt::kind::KIND_VARIABLE_DECLARATION || s->stmt_kind() == stmt::kind::KIND_ACCESS)
					_vdecls.push_back(s);
				else if (s->stmt_kind() == stmt::kind::KIND_FUNCTION) {
					if (is_function_main(mi, s->stmt_function()->function_stmt_symbol()))
						_main.push_back(s);
					else
						_functions.push_back(s);
				}
				else if (s->stmt_kind() == stmt::kind::KIND_NAMESPACE) {
					tuple<vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>> reordered_namespace
						= reorder_stmt_list(mi, s->stmt_namespace()->namespace_stmt_list());
					_vdecls.insert(_vdecls.end(), get<0>(reordered_namespace).begin(), get<0>(reordered_namespace).end());
					_functions.insert(_functions.end(), get<1>(reordered_namespace).begin(), get<1>(reordered_namespace).end());
					_main.insert(_main.end(), get<2>(reordered_namespace).begin(), get<2>(reordered_namespace).end());
					_rest.insert(_rest.end(), get<3>(reordered_namespace).begin(), get<3>(reordered_namespace).end());
				}
				else if (s->stmt_kind() == stmt::kind::KIND_STRUCT || s->stmt_kind() == stmt::kind::KIND_EMPTY || s->stmt_kind() == stmt::kind::KIND_USING ||
					s->stmt_kind() == stmt::kind::KIND_INCLUDE)
					_rest.push_back(s);
				else {
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return make_tuple(_vdecls, _functions, _main, _rest);
				}
			}
			return make_tuple(_vdecls, _functions, _main, _rest);
		}

		void generate_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<spectre::parser::parser> p) {
			tuple<vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>> reordered = reorder_stmt_list(mi, p->stmt_list());
			vector<shared_ptr<stmt>> _vdecls = get<0>(reordered), _functions = get<1>(reordered), _main = get<2>(reordered), _rest = get<3>(reordered);
			mi->add_global_insn(make_shared<label_insn>(
				make_shared<label_operand>(global_header, void_type)));
			for (shared_ptr<stmt> s : _vdecls) {
				if (s->stmt_kind() != stmt::kind::KIND_VARIABLE_DECLARATION && s->stmt_kind() != stmt::kind::KIND_ACCESS) {
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return;
				}
				if (s->stmt_kind() == stmt::kind::KIND_VARIABLE_DECLARATION)
					for (shared_ptr<variable_declaration> vd : s->stmt_variable_declaration_list())
						generate_variable_declaration_middle_ir(mi, vd);
				else if (s->stmt_kind() == stmt::kind::KIND_ACCESS)
					generate_access_stmt_middle_ir(mi, s->stmt_access());
			}
			for (shared_ptr<stmt> s : _functions)
				generate_stmt_middle_ir(mi, s);
			for (shared_ptr<stmt> s : _main)
				generate_stmt_middle_ir(mi, s);
			for (shared_ptr<stmt> s : _rest)
				generate_stmt_middle_ir(mi, s);
		}

		void generate_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<stmt> s) {
			if (s == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			switch (s->stmt_kind()) {
			case stmt::kind::KIND_EXPRESSION:
				generate_expression_middle_ir(mi, s->stmt_expression(), false);
				break;
			case stmt::kind::KIND_FUNCTION:
				generate_function_stmt_middle_ir(mi, s->stmt_function());
				break;
			case stmt::kind::KIND_VARIABLE_DECLARATION:
				for (shared_ptr<variable_declaration> vd : s->stmt_variable_declaration_list())
					generate_variable_declaration_middle_ir(mi, vd);
				break;
			case stmt::kind::KIND_RETURN:
				generate_return_stmt_middle_ir(mi, s->stmt_return());
				break;
			case stmt::kind::KIND_IF:
				generate_if_stmt_middle_ir(mi, s->stmt_if());
				break;
			case stmt::kind::KIND_DO_WHILE:
			case stmt::kind::KIND_WHILE:
				generate_while_stmt_middle_ir(mi, s->stmt_while());
				break;
			case stmt::kind::KIND_BLOCK:
				generate_block_stmt_middle_ir(mi, s->stmt_block());
				break;
			case stmt::kind::KIND_BREAK_CONTINUE:
				generate_break_continue_stmt_middle_ir(mi, s->stmt_break_continue());
				break;
			case stmt::kind::KIND_SWITCH:
				generate_switch_stmt_middle_ir(mi, s->stmt_switch());
				break;
			case stmt::kind::KIND_CASE_DEFAULT:
				generate_case_default_stmt_middle_ir(mi, s->stmt_case_default());
				break;
			case stmt::kind::KIND_FOR:
				generate_for_stmt_middle_ir(mi, s->stmt_for());
				break;
			case stmt::kind::KIND_EMPTY:
			case stmt::kind::KIND_STRUCT:
			case stmt::kind::KIND_INCLUDE:
			case stmt::kind::KIND_USING:
				break;
			case stmt::kind::KIND_ASM:
				generate_asm_stmt_middle_ir(mi, s->stmt_asm());
				break;
			case stmt::kind::KIND_DELETE:
				generate_delete_stmt_middle_ir(mi, s->stmt_delete());
				break;
			case stmt::kind::KIND_NAMESPACE:
				if (s->stmt_namespace()->namespace_stmt_kind() == namespace_stmt::kind::KIND_ALIAS)
					break;
			case stmt::kind::KIND_ACCESS:
			default:
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				break;
			}
		}

		shared_ptr<operand> generate_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<expression> e, bool lvalue) {
			if (e == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<operand> ret = nullptr;
			for (int i = 0; i < e->assignment_expression_list().size(); i++) {
				shared_ptr<assignment_expression> ae = e->assignment_expression_list()[i];
				shared_ptr<operand> op = nullptr;
				if (i != e->assignment_expression_list().size() - 1)
					op = generate_assignment_expression_middle_ir(mi, ae, false);
				else {
					op = generate_assignment_expression_middle_ir(mi, ae, lvalue);
					ret = op;
				}
			}
			return ret;
		}

		shared_ptr<type> copy_type(shared_ptr<type> t) {
			switch (t->type_kind()) {
			case type::kind::KIND_PRIMITIVE:
				return make_shared<primitive_type>(*static_pointer_cast<primitive_type>(t));
			case type::kind::KIND_STRUCT:
				return make_shared<struct_type>(*static_pointer_cast<struct_type>(t));
			case type::kind::KIND_FUNCTION:
				return make_shared<function_type>(*static_pointer_cast<function_type>(t));
			case type::kind::KIND_AUTO:
				return make_shared<auto_type>(*static_pointer_cast<auto_type>(t));
			}
			return make_shared<type>(*t);
		}
		
		shared_ptr<type> add_indirection(shared_ptr<type> t) {
			shared_ptr<type> c = copy_type(t);
			c->set_array_dimensions(c->array_dimensions() + 1);
			c->set_type_array_kind(type::array_kind::KIND_ARRAY);
			return c;
		}

		shared_ptr<type> dereference(shared_ptr<type> t) {
			shared_ptr<type> c = copy_type(t);
			if (c->array_dimensions() != 0) {
				c->set_array_dimensions(c->array_dimensions() - 1);
				c->set_type_array_kind(c->array_dimensions() == 0 ? type::array_kind::KIND_NON_ARRAY : type::array_kind::KIND_ARRAY);
			}
			return c;
		}

		shared_ptr<type> dominant_type(shared_ptr<middle_ir> mi, shared_ptr<type> t1, shared_ptr<type> t2) {
			if (t1 == nullptr || t2 == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			if (is_struct_type(mi, t1))
				return t1;
			else if (is_struct_type(mi, t2))
				return t2;
			else if (is_pointer_type(mi, t1))
				return t1;
			else if (is_pointer_type(mi, t2))
				return t2;
			else if (is_double_type(mi, t1))
				return t1;
			else if (is_double_type(mi, t2))
				return t2;
			else if (is_float_type(mi, t1))
				return t1;
			else if (is_float_type(mi, t2))
				return t2;
			else if (is_long_type(mi, t1)) {
				if (is_unsigned_type(mi, t2) || is_unsigned_type(mi, t1))
					return ulong_type;
				else
					return t1;
			}
			else if (is_long_type(mi, t2)) {
				if (is_unsigned_type(mi, t1) || is_unsigned_type(mi, t2))
					return ulong_type;
				else
					return t2;
			}
			else if (is_int_type(mi, t1)) {
				if (is_unsigned_type(mi, t2) || is_unsigned_type(mi, t1))
					return uint_type;
				else
					return t1;
			}
			else if (is_int_type(mi, t2)) {
				if (is_unsigned_type(mi, t1) || is_unsigned_type(mi, t2))
					return uint_type;
				else
					return t2;
			}
			else if (is_short_type(mi, t1)) {
				if (is_unsigned_type(mi, t2) || is_unsigned_type(mi, t1))
					return ushort_type;
				else
					return t1;
			}
			else if (is_short_type(mi, t2)) {
				if (is_unsigned_type(mi, t1) || is_unsigned_type(mi, t2))
					return ushort_type;
				else
					return t2;
			}
			else if (is_char_type(mi, t1) || is_byte_type(mi, t1) || is_bool_type(mi, t1)) {
				if (is_unsigned_type(mi, t2) || is_unsigned_type(mi, t1)) {
					if (is_char_type(mi, t1))
						return uchar_type;
					else if (is_byte_type(mi, t1))
						return ubyte_type;
					else
						return bool_type;
				}
				else
					return t1;
			}
			else if (is_char_type(mi, t2) || is_byte_type(mi, t2) || is_bool_type(mi, t2)) {
				if (is_unsigned_type(mi, t1) || is_unsigned_type(mi, t2)) {
					if (is_char_type(mi, t2))
						return uchar_type;
					else if (is_byte_type(mi, t2))
						return ubyte_type;
					else
						return bool_type;
				}
				else
					return t2;
			}
			else if (is_void_type(mi, t1))
				return t1;
			else if (is_void_type(mi, t2))
				return t2;
			else
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		pair<bool, unary_insn::kind> which_mov_insn(shared_ptr<middle_ir> mi, shared_ptr<type> t) {
			bool is_struct = false;
			unary_insn::kind umov = unary_insn::kind::KIND_NONE;
			if (is_struct_type(mi, t))
				is_struct = true;
			else if (is_double_type(mi, t))
				umov = unary_insn::kind::KIND_DMOV;
			else if (is_float_type(mi, t))
				umov = unary_insn::kind::KIND_FMOV;
			else if (is_long_type(mi, t))
				umov = unary_insn::kind::KIND_LMOV;
			else if (is_int_type(mi, t) || is_pointer_type(mi, t))
				umov = unary_insn::kind::KIND_IMOV;
			else if (is_short_type(mi, t))
				umov = unary_insn::kind::KIND_SMOV;
			else if (is_bool_type(mi, t) || is_char_type(mi, t) || is_byte_type(mi, t) || is_void_type(mi, t))
				umov = unary_insn::kind::KIND_CMOV;
			else
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return make_pair(is_struct, umov);
		}

		shared_ptr<operand> generate_assignment_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<assignment_expression> ae, bool lvalue) {
			if (ae == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			switch (ae->assignment_expression_kind()) {
			case assignment_expression::kind::KIND_ASSIGNMENT: {
				shared_ptr<operand> rhs = generate_assignment_expression_middle_ir(mi, ae->rhs_assignment(), false);
				shared_ptr<operand> lhs = to_register_operand(mi, generate_unary_expression_middle_ir(mi, ae->lhs_assignment(), true));
				shared_ptr<register_operand> reg_lhs = static_pointer_cast<register_operand>(lhs);
				shared_ptr<register_operand> deref_reg_lhs = make_shared<register_operand>(*reg_lhs);
				deref_reg_lhs->set_dereference(true);
				shared_ptr<type> lhs_type = ae->lhs_assignment()->unary_expression_type(),
					rhs_type = ae->rhs_assignment()->assignment_expression_type();
				deref_reg_lhs->set_register_type(lhs_type);
				if (rhs->operand_kind() == operand::kind::KIND_REGISTER) {
					shared_ptr<register_operand> rhs_reg = static_pointer_cast<register_operand>(rhs);
					rhs_reg = make_shared<register_operand>(*rhs_reg);
					rhs_reg->set_register_type(rhs_type);
					rhs = rhs_reg;
				}
				shared_ptr<operand> casted_rhs = ir_cast(mi, rhs, rhs_type, lhs_type);
				if (casted_rhs->operand_kind() == operand::kind::KIND_REGISTER) {
					shared_ptr<register_operand> casted_rhs_reg = static_pointer_cast<register_operand>(casted_rhs);
					casted_rhs_reg = make_shared<register_operand>(*casted_rhs_reg);
					casted_rhs_reg->set_register_type(lhs_type);
					casted_rhs = casted_rhs_reg;
				}
				if (is_double_type(mi, lhs_type) || is_float_type(mi, lhs_type) || is_long_type(mi, lhs_type) || is_int_type(mi, lhs_type)
					|| is_short_type(mi, lhs_type) || is_char_type(mi, lhs_type) || is_byte_type(mi, lhs_type)
					|| is_bool_type(mi, lhs_type)) {
					binary_insn::kind bop = binary_insn::kind::KIND_NONE;
					unary_insn::kind uop = unary_insn::kind::KIND_NONE;
					switch (ae->assignment_operator_kind()) {
					case binary_expression::operator_kind::KIND_EQUALS:
						if (is_double_type(mi, lhs_type))
							uop = unary_insn::kind::KIND_DMOV;
						else if (is_float_type(mi, lhs_type))
							uop = unary_insn::kind::KIND_FMOV;
						else if (is_int_type(mi, lhs_type) || is_long_type(mi, lhs_type))
							uop = unary_insn::kind::KIND_IMOV;
						else if (is_short_type(mi, lhs_type))
							uop = unary_insn::kind::KIND_SMOV;
						else if (is_char_type(mi, lhs_type) || is_byte_type(mi, lhs_type) || is_bool_type(mi, lhs_type))
							uop = unary_insn::kind::KIND_CMOV;
						else
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						break;
#define ARITH_OPGEN(OP) \
					if (is_double_type(mi, lhs_type)) \
						bop = binary_insn::kind::KIND_D ## OP; \
					else if (is_float_type(mi, lhs_type)) \
						bop = binary_insn::kind::KIND_F ## OP; \
					else if (is_long_type(mi, lhs_type)) \
						bop = binary_insn::kind::KIND_L ## OP; \
					else if (is_int_type(mi, lhs_type)) \
						bop = binary_insn::kind::KIND_I ## OP; \
					else if (is_short_type(mi, lhs_type)) \
						bop = binary_insn::kind::KIND_S ## OP; \
					else if (is_char_type(mi, lhs_type) || is_byte_type(mi, lhs_type)) \
						bop = binary_insn::kind::KIND_C ## OP; \
					else \
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					case binary_expression::operator_kind::KIND_ADD_EQUALS:
						ARITH_OPGEN(ADD)
							break;
					case binary_expression::operator_kind::KIND_SUBTRACT_EQUALS:
						ARITH_OPGEN(SUB)
							break;
					case binary_expression::operator_kind::KIND_MULTIPLY_EQUALS:
						ARITH_OPGEN(MUL)
							break;
#undef ARITH_OPGEN
					case binary_expression::operator_kind::KIND_DIVIDE_EQUALS: {
						if (is_double_type(mi, lhs_type))
							bop = binary_insn::kind::KIND_DDIV;
						else if (is_float_type(mi, lhs_type))
							bop = binary_insn::kind::KIND_FDIV;
						else if (is_long_type(mi, lhs_type)) {
							if (is_unsigned_type(mi, lhs_type))
								bop = binary_insn::kind::KIND_ULDIV;
							else
								bop = binary_insn::kind::KIND_LDIV;
						}
						else if (is_int_type(mi, lhs_type)) {
							if (is_unsigned_type(mi, lhs_type))
								bop = binary_insn::kind::KIND_UIDIV;
							else
								bop = binary_insn::kind::KIND_IDIV;
						}
						else if (is_short_type(mi, lhs_type)) {
							if (is_unsigned_type(mi, lhs_type))
								bop = binary_insn::kind::KIND_USDIV;
							else
								bop = binary_insn::kind::KIND_SDIV;
						}
						else if (is_char_type(mi, lhs_type) || is_byte_type(mi, lhs_type)){
							if (is_unsigned_type(mi, lhs_type))
								bop = binary_insn::kind::KIND_UCDIV;
							else
								bop = binary_insn::kind::KIND_CDIV;
						}
						else
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					}
						break;
					case binary_expression::operator_kind::KIND_MODULUS_EQUALS: {
						if (is_long_type(mi, lhs_type)) {
							if (is_unsigned_type(mi, lhs_type))
								bop = binary_insn::kind::KIND_ULMOD;
							else
								bop = binary_insn::kind::KIND_LMOD;
						}
						else if (is_int_type(mi, lhs_type)) {
							if (is_unsigned_type(mi, lhs_type))
								bop = binary_insn::kind::KIND_UIMOD;
							else
								bop = binary_insn::kind::KIND_IMOD;
						}
						else if (is_short_type(mi, lhs_type)) {
							if (is_unsigned_type(mi, lhs_type))
								bop = binary_insn::kind::KIND_USMOD;
							else
								bop = binary_insn::kind::KIND_SMOD;
						}
						else if (is_char_type(mi, lhs_type) || is_byte_type(mi, lhs_type)){
							if (is_unsigned_type(mi, lhs_type))
								bop = binary_insn::kind::KIND_UCMOD;
							else
								bop = binary_insn::kind::KIND_CMOD;
						}
						else
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					}
						break;
#define INTEGRAL_ARITH_OPGEN(OP) \
					if (is_long_type(mi, lhs_type)) \
						bop = binary_insn::kind::KIND_L ## OP; \
					else if (is_int_type(mi, lhs_type)) \
						bop = binary_insn::kind::KIND_I ## OP; \
					else if (is_short_type(mi, lhs_type)) \
						bop = binary_insn::kind::KIND_S ## OP; \
					else if (is_char_type(mi, lhs_type) || is_byte_type(mi, lhs_type)) \
						bop = binary_insn::kind::KIND_C ## OP; \
					else \
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					case binary_expression::operator_kind::KIND_SHIFT_LEFT_EQUALS:
						INTEGRAL_ARITH_OPGEN(SHL);
						break;
					case binary_expression::operator_kind::KIND_SHIFT_RIGHT_EQUALS: {
						if (is_unsigned_type(mi, lhs_type)) {
							INTEGRAL_ARITH_OPGEN(SHR);
						}
						else {
							INTEGRAL_ARITH_OPGEN(SHRA);
						}
					}
						break;
					case binary_expression::operator_kind::KIND_BITWISE_AND_EQUALS:
						INTEGRAL_ARITH_OPGEN(BAND);
						break;
					case binary_expression::operator_kind::KIND_BITWISE_OR_EQUALS:
						INTEGRAL_ARITH_OPGEN(BOR);
						break;
					case binary_expression::operator_kind::KIND_BITWISE_XOR_EQUALS:
						INTEGRAL_ARITH_OPGEN(BXOR);
						break;
#undef INTEGRAL_ARITH_OPGEN
					case binary_expression::operator_kind::KIND_LOGICAL_AND_EQUALS:
						bop = binary_insn::kind::KIND_CBAND;
						break;
					case binary_expression::operator_kind::KIND_LOGICAL_OR_EQUALS:
						bop = binary_insn::kind::KIND_CBOR;
						break;
					default:
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						break;
					}
					if (bop == binary_insn::kind::KIND_NONE && uop != unary_insn::kind::KIND_NONE)
						mi->add_insn(make_shared<unary_insn>(uop, deref_reg_lhs, casted_rhs, lhs_type));
					else if (uop == unary_insn::kind::KIND_NONE && bop != binary_insn::kind::KIND_NONE) {
						if (bop == binary_insn::kind::KIND_LDIV
							|| bop == binary_insn::kind::KIND_IDIV
							|| bop == binary_insn::kind::KIND_SDIV
							|| bop == binary_insn::kind::KIND_CDIV) {
							mi->add_insn(make_shared<call_insn>(idiv_type->return_type(), deref_reg_lhs,
								make_shared<label_operand>(idiv_symbol_name, idiv_type), vector<shared_ptr<operand>>{
									deref_reg_lhs, casted_rhs
								}, idiv_type));
						}
						else if (bop == binary_insn::kind::KIND_ULDIV
							|| bop == binary_insn::kind::KIND_UIDIV
							|| bop == binary_insn::kind::KIND_USDIV
							|| bop == binary_insn::kind::KIND_UCDIV) {
							mi->add_insn(make_shared<call_insn>(uidiv_type->return_type(), deref_reg_lhs,
								make_shared<label_operand>(uidiv_symbol_name, uidiv_type), vector<shared_ptr<operand>>{
									deref_reg_lhs, casted_rhs
								}, uidiv_type));
						}
						else if (bop == binary_insn::kind::KIND_LMOD
							|| bop == binary_insn::kind::KIND_IMOD
							|| bop == binary_insn::kind::KIND_SMOD
							|| bop == binary_insn::kind::KIND_CMOD) {
							mi->add_insn(make_shared<call_insn>(imod_type->return_type(), deref_reg_lhs,
								make_shared<label_operand>(imod_symbol_name, imod_type), vector<shared_ptr<operand>>{
									deref_reg_lhs, casted_rhs
								}, imod_type));
						}
						else if (bop == binary_insn::kind::KIND_ULMOD
							|| bop == binary_insn::kind::KIND_UIMOD
							|| bop == binary_insn::kind::KIND_USMOD
							|| bop == binary_insn::kind::KIND_UCMOD) {
							mi->add_insn(make_shared<call_insn>(uimod_type->return_type(), deref_reg_lhs,
								make_shared<label_operand>(uimod_symbol_name, uimod_type), vector<shared_ptr<operand>>{
									deref_reg_lhs, casted_rhs
								}, uimod_type));
						}
						else
							mi->add_insn(make_shared<binary_insn>(bop, deref_reg_lhs, deref_reg_lhs, casted_rhs, lhs_type));
					}
					else
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				}
				else if (is_pointer_type(mi, lhs_type)) {
					if (ae->assignment_operator_kind() != binary_expression::operator_kind::KIND_EQUALS)
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_IMOV, deref_reg_lhs, casted_rhs, lhs_type));
				}
				else if (is_struct_type(mi, lhs_type)) {
					if (ae->assignment_operator_kind() != binary_expression::operator_kind::KIND_EQUALS)
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<size_data> sd = spectre_sizeof(mi, lhs_type);
					mi->add_insn(make_shared<memcpy_insn>(reg_lhs, casted_rhs, sd->size, lhs_type));
				}
				else
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return (is_struct_type(mi, lhs_type) || lvalue) ? lhs : deref_reg_lhs;
			}
				break;
			case assignment_expression::kind::KIND_TERNARY:
				return generate_ternary_expression_middle_ir(mi, ae->conditional_expression(), lvalue);
				break;
			}
			mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand> generate_ternary_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<ternary_expression> te, bool lvalue) {
			if (te == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			switch (te->ternary_expression_kind()) {
			case ternary_expression::kind::KIND_TERNARY: {
				shared_ptr<operand> cond = generate_binary_expression_middle_ir(mi, te->condition(), false);
				shared_ptr<label_operand> true_lab = mi->generate_next_label_operand(),
					done_lab = mi->generate_next_label_operand();
				shared_ptr<type> true_type = te->true_path()->expression_type(),
					false_type = te->false_path()->ternary_expression_type(),
					res_type = te->ternary_expression_type();
				shared_ptr<type> overall_type = te->ternary_expression_type();
				if (te->ternary_expression_value_kind() == value_kind::VALUE_LVALUE) {
					unary_insn::kind uop = unary_insn::kind::KIND_NONE;
					bool is_struct = false;
					shared_ptr<size_data> sd = nullptr;
					if (lvalue)
						true_type = add_indirection(true_type), false_type = add_indirection(false_type);
					if (is_struct_type(mi, true_type) && is_struct_type(mi, false_type)) {
						sd = spectre_sizeof(mi, true_type);
						is_struct = true;
					}
					else if (is_pointer_type(mi, true_type) && is_pointer_type(mi, false_type))
						uop = unary_insn::kind::KIND_IMOV;
					else if (is_double_type(mi, true_type) && is_double_type(mi, false_type))
						uop = unary_insn::kind::KIND_DMOV;
					else if (is_float_type(mi, true_type) && is_float_type(mi, false_type))
						uop = unary_insn::kind::KIND_FMOV;
					else if (is_long_type(mi, true_type) && is_long_type(mi, false_type))
						uop = unary_insn::kind::KIND_LMOV;
					else if (is_int_type(mi, true_type) && is_int_type(mi, false_type))
						uop = unary_insn::kind::KIND_IMOV;
					else if (is_short_type(mi, true_type) && is_short_type(mi, false_type))
						uop = unary_insn::kind::KIND_SMOV;
					else if (is_char_type(mi, true_type) && is_char_type(mi, false_type))
						uop = unary_insn::kind::KIND_CMOV;
					else if (is_bool_type(mi, true_type) && is_bool_type(mi, false_type))
						uop = unary_insn::kind::KIND_CMOV;
					else if (is_byte_type(mi, true_type) && is_byte_type(mi, false_type))
						uop = unary_insn::kind::KIND_CMOV;
					else if (is_void_type(mi, true_type) && is_void_type(mi, false_type))
						uop = unary_insn::kind::KIND_CMOV;
					else
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<register_operand> res = mi->generate_next_register(true_type);
					mi->add_insn(make_shared<conditional_insn>(cond, true_lab));
					shared_ptr<operand> fres = generate_ternary_expression_middle_ir(mi, te->false_path(), lvalue);
					if (fres->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> fres_reg = static_pointer_cast<register_operand>(fres);
						if (lvalue)
							fres_reg->set_register_type(false_type);
						fres = fres_reg;
					}
					if (uop != unary_insn::kind::KIND_NONE)
						mi->add_insn(make_shared<unary_insn>(uop, res, fres, res_type));
					else if (is_struct)
						mi->add_insn(make_shared<memcpy_insn>(res, fres, sd->size, res_type));
					else
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					mi->add_insn(make_shared<jump_insn>(done_lab));
					mi->add_insn(make_shared<label_insn>(true_lab));
					shared_ptr<operand> tres = generate_expression_middle_ir(mi, te->true_path(), lvalue);
					if (tres->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> tres_reg = static_pointer_cast<register_operand>(tres);
						if (lvalue)
							tres_reg->set_register_type(true_type);
						tres = tres_reg;
					}
					if (uop != unary_insn::kind::KIND_NONE)
						mi->add_insn(make_shared<unary_insn>(uop, res, tres, res_type));
					else if (is_struct)
						mi->add_insn(make_shared<memcpy_insn>(res, tres, sd->size, res_type));
					else
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					mi->add_insn(make_shared<label_insn>(done_lab));
					return res;
				}
				else if (te->ternary_expression_value_kind() == value_kind::VALUE_RVALUE) {
					shared_ptr<type> dom = dominant_type(mi, true_type, false_type);
					unary_insn::kind uop = unary_insn::kind::KIND_NONE;
					shared_ptr<size_data> sd = nullptr;
					bool is_struct = false;
					if (is_double_type(mi, dom))
						uop = unary_insn::kind::KIND_DMOV;
					else if (is_float_type(mi, dom))
						uop = unary_insn::kind::KIND_FMOV;
					else if (is_long_type(mi, dom))
						uop = unary_insn::kind::KIND_LMOV;
					else if (is_int_type(mi, dom) || is_pointer_type(mi, dom))
						uop = unary_insn::kind::KIND_IMOV;
					else if (is_short_type(mi, dom))
						uop = unary_insn::kind::KIND_SMOV;
					else if (is_char_type(mi, dom) || is_void_type(mi, dom) || is_byte_type(mi, dom) || is_bool_type(mi, dom))
						uop = unary_insn::kind::KIND_CMOV;
					else if (is_struct_type(mi, dom)) {
						sd = spectre_sizeof(mi, dom);
						is_struct = true;
					}
					else
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<operand> res = mi->generate_next_register(dom);
					mi->add_insn(make_shared<conditional_insn>(cond, true_lab));
					shared_ptr<operand> fres = generate_ternary_expression_middle_ir(mi, te->false_path(), lvalue);
					shared_ptr<operand> casted_fres = ir_cast(mi, fres, false_type, dom);
					if (uop != unary_insn::kind::KIND_NONE)
						mi->add_insn(make_shared<unary_insn>(uop, res, casted_fres, dom));
					else if (is_struct)
						mi->add_insn(make_shared<memcpy_insn>(res, casted_fres, sd->size, res_type));
					else
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					mi->add_insn(make_shared<jump_insn>(done_lab));
					mi->add_insn(make_shared<label_insn>(true_lab));
					shared_ptr<operand> tres = generate_expression_middle_ir(mi, te->true_path(), lvalue);
					shared_ptr<operand> casted_tres = ir_cast(mi, tres, true_type, dom);
					if (uop != unary_insn::kind::KIND_NONE)
						mi->add_insn(make_shared<unary_insn>(uop, res, casted_tres, dom));
					else if (is_struct)
						mi->add_insn(make_shared<memcpy_insn>(res, casted_tres, sd->size, res_type));
					else
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					mi->add_insn(make_shared<label_insn>(done_lab));
					return res;
				}
				else
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return nullptr;
			}
				break;
			case ternary_expression::kind::KIND_BINARY:
				return generate_binary_expression_middle_ir(mi, te->condition(), lvalue);
				break;
			}
			mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand> generate_binary_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<binary_expression> be, bool lvalue) {
			if (be == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			switch (be->binary_expression_kind()) {
			case binary_expression::kind::KIND_BINARY_EXPRESSION: {
				shared_ptr<type> res_type = be->binary_expression_type(),
					lhs_type = be->lhs()->binary_expression_type(),
					rhs_type = be->rhs()->binary_expression_type();
				if (be->binary_expression_operator_kind() != binary_expression::operator_kind::KIND_LOGICAL_AND
					&& be->binary_expression_operator_kind() != binary_expression::operator_kind::KIND_LOGICAL_OR) {
					shared_ptr<operand> lhs_op = generate_binary_expression_middle_ir(mi, be->lhs(), lvalue),
						rhs_op = generate_binary_expression_middle_ir(mi, be->rhs(), lvalue);
					shared_ptr<type> to_cast = nullptr;
					if (be->binary_expression_operator_kind() == binary_expression::operator_kind::KIND_EQUALS_EQUALS
						|| be->binary_expression_operator_kind() == binary_expression::operator_kind::KIND_NOT_EQUALS
						|| be->binary_expression_operator_kind() == binary_expression::operator_kind::KIND_GREATER_THAN
						|| be->binary_expression_operator_kind() == binary_expression::operator_kind::KIND_LESS_THAN
						|| be->binary_expression_operator_kind() == binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO
						|| be->binary_expression_operator_kind() == binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO)
						to_cast = dominant_type(mi, lhs_type, rhs_type);
					else
						to_cast = res_type;
					shared_ptr<operand> casted_lhs = ir_cast(mi, lhs_op, lhs_type, to_cast),
						casted_rhs = ir_cast(mi, rhs_op, rhs_type, to_cast);
					if (casted_lhs->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> casted_lhs_reg = static_pointer_cast<register_operand>(casted_lhs);
						casted_lhs_reg = make_shared<register_operand>(*casted_lhs_reg);
						casted_lhs_reg->set_register_type(to_cast);
						casted_lhs = casted_lhs_reg;
					}
					if (casted_rhs->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> casted_rhs_reg = static_pointer_cast<register_operand>(casted_rhs);
						casted_rhs_reg = make_shared<register_operand>(*casted_rhs_reg);
						casted_rhs_reg->set_register_type(to_cast);
						casted_rhs = casted_rhs_reg;
					}
					shared_ptr<operand> cvt = mi->generate_next_register(res_type);
					binary_insn::kind bop = binary_insn::kind::KIND_NONE;
					switch (be->binary_expression_operator_kind()) {
#define ARITH_UNSIGNED_OPGEN(OP) \
	if (is_double_type(mi, res_type)) \
		bop = binary_insn::kind::KIND_D ## OP; \
	else if (is_float_type(mi, res_type)) \
		bop = binary_insn::kind::KIND_F ## OP; \
	else if (is_long_type(mi, res_type)) \
		bop = binary_insn::kind::KIND_L ## OP; \
	else if (is_int_type(mi, res_type)) \
		bop = binary_insn::kind::KIND_I ## OP; \
	else if (is_short_type(mi, res_type)) \
		bop = binary_insn::kind::KIND_S ## OP; \
	else if (is_char_type(mi, res_type) || is_byte_type(mi, res_type)) \
		bop = binary_insn::kind::KIND_C ## OP; \
	else \
		mi->report_internal("This should be unreacahable.", __FUNCTION__, __LINE__, __FILE__);
					case binary_expression::operator_kind::KIND_ADD: {
						ARITH_UNSIGNED_OPGEN(ADD)
					}
						break;
					case binary_expression::operator_kind::KIND_SUBTRACT: {
						ARITH_UNSIGNED_OPGEN(SUB)
					}
						break;
#undef ARITH_UNSIGNED_OPGEN
#define SIGNED_OPGEN(OP) \
	if (is_double_type(mi, res_type)) \
		bop = binary_insn::kind::KIND_D ## OP; \
	else if (is_float_type(mi, res_type)) \
		bop = binary_insn::kind::KIND_F ## OP; \
	else if (is_long_type(mi, res_type)) { \
		if (is_unsigned_type(mi, res_type)) \
			bop = binary_insn::kind::KIND_L ## OP; \
		else \
			bop = binary_insn::kind::KIND_UL ## OP; \
	} \
	else if (is_int_type(mi, res_type)) { \
		if (is_unsigned_type(mi, res_type)) \
			bop = binary_insn::kind::KIND_UI ## OP; \
		else \
			bop = binary_insn::kind::KIND_I ## OP; \
	} \
	else if (is_short_type(mi, res_type)) { \
		if (is_unsigned_type(mi, res_type)) \
			bop = binary_insn::kind::KIND_US ## OP; \
		else \
			bop = binary_insn::kind::KIND_S ## OP; \
	} \
	else if (is_char_type(mi, res_type) || is_byte_type(mi, res_type)) { \
		if (is_unsigned_type(mi, res_type)) \
			bop = binary_insn::kind::KIND_UC ## OP; \
		else \
			bop = binary_insn::kind::KIND_C ## OP; \
	} \
	else \
		mi->report_internal("This should be unreacahable.", __FUNCTION__, __LINE__, __FILE__);
					case binary_expression::operator_kind::KIND_MULTIPLY: {
						SIGNED_OPGEN(MUL)
					}
						break;
					case binary_expression::operator_kind::KIND_DIVIDE: {
						SIGNED_OPGEN(DIV)
					}
						break;
#undef SIGNED_OPGEN
#define CMP_SIGNED_OPGEN(OP) \
	if (is_double_type(mi, to_cast)) \
		bop = binary_insn::kind::KIND_D ## OP; \
	else if (is_float_type(mi, to_cast)) \
		bop = binary_insn::kind::KIND_F ## OP; \
	else if (is_long_type(mi, to_cast)) { \
		if (is_unsigned_type(mi, to_cast)) \
			bop = binary_insn::kind::KIND_UL ## OP; \
		else \
			bop = binary_insn::kind::KIND_L ## OP; \
	} \
	else if (is_int_type(mi, to_cast)) { \
		if (is_unsigned_type(mi, to_cast)) \
			bop = binary_insn::kind::KIND_UI ## OP; \
		else \
			bop = binary_insn::kind::KIND_I ## OP; \
	} \
	else if (is_short_type(mi, to_cast)) { \
		if (is_unsigned_type(mi, to_cast)) \
			bop = binary_insn::kind::KIND_US ## OP; \
		else \
			bop = binary_insn::kind::KIND_S ## OP; \
	} \
	else if (is_char_type(mi, to_cast) || is_byte_type(mi, to_cast)) { \
		if (is_unsigned_type(mi, to_cast)) \
			bop = binary_insn::kind::KIND_UC ## OP; \
		else \
			bop = binary_insn::kind::KIND_C ## OP; \
	} \
	else \
		mi->report_internal("This should be unreacahable.", __FUNCTION__, __LINE__, __FILE__);
					case binary_expression::operator_kind::KIND_LESS_THAN: {
						CMP_SIGNED_OPGEN(LT)
					}
						break;
					case binary_expression::operator_kind::KIND_GREATER_THAN: {
						CMP_SIGNED_OPGEN(GT)
					}
						break;
					case binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO: {
						CMP_SIGNED_OPGEN(GE)
					}
						break;
					case binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO: {
						CMP_SIGNED_OPGEN(LE)
					}
						break;
#undef CMP_SIGNED_OPGEN
#define ARITH_INTEGRAL_SIGNED_OPGEN(OP) \
	if (is_long_type(mi, res_type)) { \
		if (is_unsigned_type(mi, res_type)) \
			bop = binary_insn::kind::KIND_UL ## OP; \
		else \
			bop = binary_insn::kind::KIND_L ## OP; \
	} \
	else if (is_int_type(mi, res_type)) { \
		if (is_unsigned_type(mi, res_type)) \
			bop = binary_insn::kind::KIND_UI ## OP; \
		else \
			bop = binary_insn::kind::KIND_I ## OP; \
	} \
	else if (is_short_type(mi, res_type)) { \
		if (is_unsigned_type(mi, res_type)) \
			bop = binary_insn::kind::KIND_US ## OP; \
		else \
			bop = binary_insn::kind::KIND_S ## OP; \
	} \
	else if (is_char_type(mi, res_type) || is_byte_type(mi, res_type)) { \
		if (is_unsigned_type(mi, res_type)) \
			bop = binary_insn::kind::KIND_UC ## OP; \
		else \
			bop = binary_insn::kind::KIND_C ## OP; \
	} \
	else \
		mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					case binary_expression::operator_kind::KIND_MODULUS: {
						ARITH_INTEGRAL_SIGNED_OPGEN(MOD)
					}
						break;
#undef ARITH_INTEGRAL_SIGNED_OPGEN
#define CMP_UNSIGNED_OPGEN(OP) \
	if (is_double_type(mi, to_cast)) \
		bop = binary_insn::kind::KIND_D ## OP; \
	else if (is_float_type(mi, to_cast)) \
		bop = binary_insn::kind::KIND_F ## OP; \
	else if (is_long_type(mi, to_cast)) \
		bop = binary_insn::kind::KIND_L ## OP; \
	else if (is_int_type(mi, to_cast) || is_pointer_type(mi, to_cast) || is_struct_type(mi, to_cast)) \
		bop = binary_insn::kind::KIND_I ## OP; \
	else if (is_short_type(mi, to_cast)) \
		bop = binary_insn::kind::KIND_S ## OP; \
	else if (is_char_type(mi, to_cast) || is_byte_type(mi, to_cast) || is_bool_type(mi, to_cast)) \
		bop = binary_insn::kind::KIND_C ## OP; \
	else \
		mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					case binary_expression::operator_kind::KIND_EQUALS_EQUALS: {
						CMP_UNSIGNED_OPGEN(EQ)
					}
						break;
					case binary_expression::operator_kind::KIND_NOT_EQUALS: {
						CMP_UNSIGNED_OPGEN(NE)
					}
						break;
#undef CMP_UNSIGNED_OPGEN
#define UNSIGNED_INTEGRAL_OPGEN(OP) \
	if (is_long_type(mi, res_type)) \
		bop = binary_insn::kind::KIND_L ## OP; \
	else if (is_int_type(mi, res_type)) \
		bop = binary_insn::kind::KIND_I ## OP; \
	else if (is_short_type(mi, res_type)) \
		bop = binary_insn::kind::KIND_S ## OP; \
	else if (is_char_type(mi, res_type) || is_byte_type(mi, res_type)) \
		bop = binary_insn::kind::KIND_C ## OP; \
	else \
		mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					case binary_expression::operator_kind::KIND_SHIFT_LEFT: {
						UNSIGNED_INTEGRAL_OPGEN(SHL)
					}
						break;
					case binary_expression::operator_kind::KIND_SHIFT_RIGHT: {
						if (is_unsigned_type(mi, res_type)) {
							UNSIGNED_INTEGRAL_OPGEN(SHR)
						}
						else {
							UNSIGNED_INTEGRAL_OPGEN(SHRA);
						}
					}
						break;
					case binary_expression::operator_kind::KIND_BITWISE_AND: {
						UNSIGNED_INTEGRAL_OPGEN(BAND)
					}
						break;
					case binary_expression::operator_kind::KIND_BITWISE_OR: {
						UNSIGNED_INTEGRAL_OPGEN(BOR)
					}
						break;
					case binary_expression::operator_kind::KIND_BITWISE_XOR: {
						UNSIGNED_INTEGRAL_OPGEN(BXOR)
					}
						break;
					default:
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						break;
					}
					if (bop == binary_insn::kind::KIND_LDIV
						|| bop == binary_insn::kind::KIND_IDIV
						|| bop == binary_insn::kind::KIND_SDIV
						|| bop == binary_insn::kind::KIND_CDIV) {
						mi->add_insn(make_shared<call_insn>(idiv_type->return_type(), cvt,
							make_shared<label_operand>(idiv_symbol_name, idiv_type), vector<shared_ptr<operand>>{
								casted_lhs, casted_rhs
							}, idiv_type));
					}
					else if (bop == binary_insn::kind::KIND_ULDIV
						|| bop == binary_insn::kind::KIND_UIDIV
						|| bop == binary_insn::kind::KIND_USDIV
						|| bop == binary_insn::kind::KIND_UCDIV) {
						mi->add_insn(make_shared<call_insn>(uidiv_type->return_type(), cvt,
							make_shared<label_operand>(uidiv_symbol_name, uidiv_type), vector<shared_ptr<operand>>{
								casted_lhs, casted_rhs
							}, uidiv_type));
					}
					else if (bop == binary_insn::kind::KIND_LMOD
						|| bop == binary_insn::kind::KIND_IMOD
						|| bop == binary_insn::kind::KIND_SMOD
						|| bop == binary_insn::kind::KIND_CMOD) {
						mi->add_insn(make_shared<call_insn>(imod_type->return_type(), cvt,
							make_shared<label_operand>(imod_symbol_name, imod_type), vector<shared_ptr<operand>>{
								casted_lhs, casted_rhs
							}, imod_type));
					}
					else if (bop == binary_insn::kind::KIND_ULMOD
						|| bop == binary_insn::kind::KIND_UIMOD
						|| bop == binary_insn::kind::KIND_USMOD
						|| bop == binary_insn::kind::KIND_UCMOD) {
						mi->add_insn(make_shared<call_insn>(uimod_type->return_type(), cvt,
							make_shared<label_operand>(uimod_symbol_name, uimod_type), vector<shared_ptr<operand>>{
								casted_lhs, casted_rhs
							}, uimod_type));
					}
					else
						mi->add_insn(make_shared<binary_insn>(bop, cvt, casted_lhs, casted_rhs, res_type));
					return cvt;
				}
				else {
					if (!is_bool_type(mi, lhs_type) || !is_bool_type(mi, rhs_type) || !is_bool_type(mi, res_type))
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					bool is_and = be->binary_expression_operator_kind() == binary_expression::operator_kind::KIND_LOGICAL_AND;
					shared_ptr<immediate_operand> false_imm = make_shared<immediate_operand>(res_type, (char)0),
						true_imm = make_shared<immediate_operand>(res_type, (char)1),
						cmp_imm = is_and ? false_imm : true_imm,
						opp_imm = is_and ? true_imm : false_imm,
						start_imm = is_and ? false_imm : true_imm;
					shared_ptr<operand> res = mi->generate_next_register(res_type),
						temp = mi->generate_next_register(res_type);
					shared_ptr<label_operand> done_lab = mi->generate_next_label_operand();
					mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_CMOV, res, start_imm, res_type));
					shared_ptr<operand> lhs_op = generate_binary_expression_middle_ir(mi, be->lhs(), lvalue);
					mi->add_insn(make_shared<binary_insn>(binary_insn::kind::KIND_CEQ, temp, lhs_op, cmp_imm, res_type));
					mi->add_insn(make_shared<conditional_insn>(temp, done_lab));
					shared_ptr<operand> rhs_op = generate_binary_expression_middle_ir(mi, be->rhs(), lvalue);
					mi->add_insn(make_shared<binary_insn>(binary_insn::kind::KIND_CEQ, temp, rhs_op, cmp_imm, res_type));
					mi->add_insn(make_shared<conditional_insn>(temp, done_lab));
					mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_CMOV, res, opp_imm, res_type));
					mi->add_insn(make_shared<label_insn>(done_lab));
					return res;
				}
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return nullptr;
			}
				break;
			case binary_expression::kind::KIND_UNARY_EXPRESSION:
				return generate_unary_expression_middle_ir(mi, be->single_lhs(), lvalue);
				break;
			}
			mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand> generate_unary_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<unary_expression> ue, bool lvalue) {
			if (ue == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			if (lvalue && !ue->unary_expression_kind_list().empty())
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			bool need_lvalue = !ue->unary_expression_kind_list().empty() && (ue->unary_expression_kind_list()[0] == unary_expression::kind::KIND_INCREMENT
				|| ue->unary_expression_kind_list()[0] == unary_expression::kind::KIND_DECREMENT);
			shared_ptr<operand> op = generate_postfix_expression_middle_ir(mi, ue->contained_postfix_expression(), lvalue || need_lvalue);
			for (unary_expression::kind uek : ue->unary_expression_kind_list()) {
				switch (uek) {
				case unary_expression::kind::KIND_INCREMENT:
				case unary_expression::kind::KIND_DECREMENT: {
					bool inc = uek == unary_expression::kind::KIND_INCREMENT;
					shared_ptr<type> t = ue->unary_expression_type();
					if (t->type_kind() != type::kind::KIND_PRIMITIVE)
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
					shared_ptr<register_operand> reg_op = to_register_operand(mi, op),
						deref_reg_op = make_shared<register_operand>(*reg_op);
					deref_reg_op->set_dereference(true);
					deref_reg_op->set_register_type(t);
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE) 
						mi->add_insn(make_shared<binary_insn>(inc ? binary_insn::kind::KIND_DADD : binary_insn::kind::KIND_DSUB, deref_reg_op, deref_reg_op,
							make_shared<immediate_operand>(pt, (double)1), pt));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
						mi->add_insn(make_shared<binary_insn>(inc ? binary_insn::kind::KIND_FADD : binary_insn::kind::KIND_FSUB, deref_reg_op, deref_reg_op,
							make_shared<immediate_operand>(pt, (float)1), pt));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_LONG)
						mi->add_insn(make_shared<binary_insn>(inc ? binary_insn::kind::KIND_LADD : binary_insn::kind::KIND_LSUB, deref_reg_op, deref_reg_op,
							make_shared<immediate_operand>(pt, (long)1), pt));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_INT)
						mi->add_insn(make_shared<binary_insn>(inc ? binary_insn::kind::KIND_IADD : binary_insn::kind::KIND_ISUB, deref_reg_op, deref_reg_op,
							make_shared<immediate_operand>(pt, (int)1), pt));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_SHORT)
						mi->add_insn(make_shared<binary_insn>(inc ? binary_insn::kind::KIND_SADD : binary_insn::kind::KIND_SSUB, deref_reg_op, deref_reg_op,
							make_shared<immediate_operand>(pt, (short)1), pt));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_BYTE || pt->primitive_type_kind() == primitive_type::kind::KIND_CHAR)
						mi->add_insn(make_shared<binary_insn>(inc ? binary_insn::kind::KIND_CADD : binary_insn::kind::KIND_CSUB, deref_reg_op, deref_reg_op,
							make_shared<immediate_operand>(pt, (char)1), pt));
					else
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					op = deref_reg_op;
				}
					break;
				case unary_expression::kind::KIND_PLUS:
				case unary_expression::kind::KIND_MINUS: {
					bool plus = uek == unary_expression::kind::KIND_PLUS;
					shared_ptr<type> t = ue->unary_expression_type();
					if (t->type_kind() != type::kind::KIND_PRIMITIVE)
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
					shared_ptr<register_operand> res = mi->generate_next_register(pt);
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
						mi->add_insn(make_shared<unary_insn>(plus ? unary_insn::kind::KIND_DPLUS : unary_insn::kind::KIND_DMINUS, res, op, pt));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
						mi->add_insn(make_shared<unary_insn>(plus ? unary_insn::kind::KIND_FPLUS : unary_insn::kind::KIND_FMINUS, res, op, pt));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_LONG)
						mi->add_insn(make_shared<unary_insn>(plus ? unary_insn::kind::KIND_LPLUS : unary_insn::kind::KIND_LMINUS, res, op, pt));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_INT)
						mi->add_insn(make_shared<unary_insn>(plus ? unary_insn::kind::KIND_IPLUS : unary_insn::kind::KIND_IMINUS, res, op, pt));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_SHORT)
						mi->add_insn(make_shared<unary_insn>(plus ? unary_insn::kind::KIND_SPLUS : unary_insn::kind::KIND_SMINUS, res, op, pt));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_BYTE || pt->primitive_type_kind() == primitive_type::kind::KIND_CHAR)
						mi->add_insn(make_shared<unary_insn>(plus ? unary_insn::kind::KIND_CPLUS : unary_insn::kind::KIND_CMINUS, res, op, pt));
					else
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					op = res;
				}
					break;
				case unary_expression::kind::KIND_LOGICAL_NOT: {
					shared_ptr<type> t = ue->unary_expression_type();
					if (t->type_kind() != type::kind::KIND_PRIMITIVE)
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
					if (pt->primitive_type_kind() != primitive_type::kind::KIND_BOOL)
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<register_operand> res = mi->generate_next_register(pt);
					mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_LNOT, res, op, pt));
					op = res;
				}
					break;
				case unary_expression::kind::KIND_BITWISE_NOT: {
					shared_ptr<type> t = ue->unary_expression_type();
					if (t->type_kind() != type::kind::KIND_PRIMITIVE)
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
					shared_ptr<register_operand> res = mi->generate_next_register(pt);
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_LONG)
						mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_LBNOT, res, op, pt));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_INT)
						mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_IBNOT, res, op, pt));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_SHORT)
						mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_SBNOT, res, op, pt));
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_CHAR || pt->primitive_type_kind() == primitive_type::kind::KIND_BYTE)
						mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_CBNOT, res, op, pt));
					else
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					op = res;
				}
					break;
				default:
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
					break;
				}
			}
			return op;
		}

		shared_ptr<operand> generate_postfix_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<postfix_expression> pe, bool lvalue) {
			if (pe == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			bool need_lvalue = !pe->postfix_type_list().empty() &&
				(pe->postfix_type_list()[0]->postfix_type_kind() == postfix_expression::kind::KIND_INCREMENT
					|| pe->postfix_type_list()[0]->postfix_type_kind() == postfix_expression::kind::KIND_DECREMENT
					|| pe->postfix_type_list()[0]->postfix_type_kind() == postfix_expression::kind::KIND_ADDRESS);
			shared_ptr<operand> op = nullptr;
			bool pure_function_call = !pe->postfix_type_list().empty()
				&& pe->postfix_type_list()[0]->postfix_type_kind() == postfix_expression::kind::KIND_FUNCTION_CALL
				&& pe->contained_primary_expression()->primary_expression_kind() == primary_expression::kind::KIND_IDENTIFIER
				&& pe->contained_primary_expression()->identifier_symbol() != nullptr
				&& pe->contained_primary_expression()->identifier_symbol()->symbol_kind() == symbol::kind::KIND_FUNCTION;
			bool first_postfix = true;
			if (pure_function_call) {
				string ident_sym = c_symbol_2_string(mi, pe->contained_primary_expression()->identifier_symbol());
				op = make_shared<label_operand>(ident_sym, pe->contained_primary_expression()->primary_expression_type());
			}
			else
				op = generate_primary_expression_middle_ir(mi, pe->contained_primary_expression(), need_lvalue || lvalue);
			shared_ptr<type> prev_type = pe->contained_primary_expression()->primary_expression_type();
			value_kind prev_vk = pe->contained_primary_expression()->primary_expression_value_kind();
			for (shared_ptr<postfix_expression::postfix_type> ptype : pe->postfix_type_list()) {
				switch (ptype->postfix_type_kind()) {
				case postfix_expression::kind::KIND_INCREMENT:
				case postfix_expression::kind::KIND_DECREMENT: {
					bool inc = ptype->postfix_type_kind() == postfix_expression::kind::KIND_INCREMENT;
					shared_ptr<register_operand> loaded = load_lvalue_to_register(mi, op);
					shared_ptr<type> t = ptype->postfix_type_type();
					if (prev_type->type_kind() != type::kind::KIND_PRIMITIVE)
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(prev_type);
					loaded = make_shared<register_operand>(*loaded);
					loaded->set_register_type(pt);
					binary_insn::kind add_insn = binary_insn::kind::KIND_NONE, sub_insn = binary_insn::kind::KIND_NONE;
					unary_insn::kind mov_insn = unary_insn::kind::KIND_NONE;
					shared_ptr<immediate_operand> _1 = nullptr;
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
						add_insn = binary_insn::kind::KIND_DADD, sub_insn = binary_insn::kind::KIND_DSUB, mov_insn = unary_insn::kind::KIND_DMOV,
						_1 = make_shared<immediate_operand>(pt, (double)1);
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
						add_insn = binary_insn::kind::KIND_FADD, sub_insn = binary_insn::kind::KIND_FSUB, mov_insn = unary_insn::kind::KIND_FMOV,
						_1 = make_shared<immediate_operand>(pt, (float)1);
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_LONG)
						add_insn = binary_insn::kind::KIND_LADD, sub_insn = binary_insn::kind::KIND_LSUB, mov_insn = unary_insn::kind::KIND_LMOV,
						_1 = make_shared<immediate_operand>(pt, (long)1);
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_INT)
						add_insn = binary_insn::kind::KIND_IADD, sub_insn = binary_insn::kind::KIND_ISUB, mov_insn = unary_insn::kind::KIND_IMOV,
						_1 = make_shared<immediate_operand>(pt, (int)1);
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_SHORT)
						add_insn = binary_insn::kind::KIND_SADD, sub_insn = binary_insn::kind::KIND_SSUB, mov_insn = unary_insn::kind::KIND_SMOV,
						_1 = make_shared<immediate_operand>(pt, (short)1);
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_CHAR || pt->primitive_type_kind() == primitive_type::kind::KIND_BYTE)
						add_insn = binary_insn::kind::KIND_CADD, sub_insn = binary_insn::kind::KIND_CSUB, mov_insn = unary_insn::kind::KIND_CMOV,
						_1 = make_shared<immediate_operand>(pt, (char)1);
					else
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<register_operand> res = mi->generate_next_register(pt);
					mi->add_insn(make_shared<unary_insn>(mov_insn, res, loaded, pt));
					mi->add_insn(make_shared<binary_insn>(inc ? add_insn : sub_insn, loaded, loaded, _1, pt));
					op = res;
					prev_vk = value_kind::VALUE_RVALUE;
				}
					break;
				case postfix_expression::kind::KIND_AS: {
					shared_ptr<type> from_type = prev_type, to_type = ptype->postfix_type_type();
					if (prev_vk == value_kind::VALUE_LVALUE)
						op = load_lvalue_to_register(mi, op);
					op = ir_cast(mi, op, from_type, to_type);
					prev_vk = value_kind::VALUE_RVALUE;
				}
					break;
				case postfix_expression::kind::KIND_SUBSCRIPT: {
					shared_ptr<type> res_type = ptype->postfix_type_type();
					if (prev_vk == value_kind::VALUE_LVALUE)
						op = load_lvalue_to_register(mi, op);
					if (op->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> reg_op = static_pointer_cast<register_operand>(op);
						reg_op = make_shared<register_operand>(*reg_op);
						reg_op->set_register_type(prev_type);
						op = reg_op;
					}
					unsigned int sz = spectre_sizeof(mi, res_type)->size;
					shared_ptr<operand> index = generate_expression_middle_ir(mi, ptype->subscript(), false);
					shared_ptr<operand> uint_index = ir_cast(mi, index, ptype->subscript()->expression_type(), uint_type);
					shared_ptr<register_operand> byte_index = mi->generate_next_register(uint_type);
					mi->add_insn(make_shared<binary_insn>(binary_insn::kind::KIND_UIMUL, byte_index, uint_index, make_shared<immediate_operand>(
						uint_type, (int)sz), uint_type));
					if (prev_type->type_array_kind() != type::array_kind::KIND_ARRAY)
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<operand> res = mi->generate_next_register(prev_type);
					mi->add_insn(make_shared<binary_insn>(binary_insn::kind::KIND_IADD, res, op, byte_index, prev_type));
					op = res;
					prev_vk = value_kind::VALUE_LVALUE;
				}
					break;
				case postfix_expression::kind::KIND_ADDRESS: {
					op = to_register_operand(mi, op);
					prev_vk = value_kind::VALUE_RVALUE;
				}
					break;
				case postfix_expression::kind::KIND_MEMBER:
				case postfix_expression::kind::KIND_ARROW:
				case postfix_expression::kind::KIND_AT: {
					shared_ptr<immediate_operand> mem_off_op = nullptr;
					shared_ptr<register_operand> base_reg_op = to_register_operand(mi, op);
					shared_ptr<register_operand> reg_op = make_shared<register_operand>(*base_reg_op);
					if (ptype->postfix_type_kind() == postfix_expression::kind::KIND_MEMBER
						|| ptype->postfix_type_kind() == postfix_expression::kind::KIND_ARROW) {
						if (prev_type->type_kind() != type::kind::KIND_STRUCT)
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						shared_ptr<struct_type> sprev_type = static_pointer_cast<struct_type>(prev_type);
						if (sprev_type->is_union())
							mem_off_op = make_shared<immediate_operand>(uint_type, (int)0);
						else {
							string member_name = ptype->member().raw_text();
							unordered_map<string, unsigned int> mem_offs = member_offsets(mi, sprev_type);
							if (mem_offs.find(member_name) == mem_offs.end())
								mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							unsigned int offset = mem_offs[member_name];
							mem_off_op = make_shared<immediate_operand>(uint_type, (int)offset);
						}
					}
					else if (ptype->postfix_type_kind() == postfix_expression::kind::KIND_AT)
						mem_off_op = make_shared<immediate_operand>(uint_type, (int)0);
					else
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if ((ptype->postfix_type_kind() == postfix_expression::kind::KIND_ARROW
						|| ptype->postfix_type_kind() == postfix_expression::kind::KIND_AT)
						&& prev_vk != value_kind::VALUE_RVALUE)
						reg_op->set_dereference(true);
					if (mem_off_op == nullptr)
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<register_operand> res = mi->generate_next_register(add_indirection(ptype->postfix_type_type()));
					if (mem_off_op->immediate_int() != 0)
						mi->add_insn(make_shared<binary_insn>(binary_insn::kind::KIND_IADD, res, reg_op, mem_off_op, ptype->postfix_type_type()));
					else
						mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_IMOV, res, reg_op, ptype->postfix_type_type()));
					op = res;
					prev_vk = value_kind::VALUE_LVALUE;
				}
					break;
				case postfix_expression::kind::KIND_FUNCTION_CALL: {
					if (first_postfix && pure_function_call && op->operand_kind() == operand::kind::KIND_LABEL);
					else if (prev_vk == value_kind::VALUE_LVALUE)
						op = load_lvalue_to_register(mi, op);
					vector<shared_ptr<operand>> args;
					if (ptype->function_type()->type_kind() != type::kind::KIND_FUNCTION)
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<function_type> ft = static_pointer_cast<function_type>(ptype->function_type());
					unsigned int index = 0;
					for (shared_ptr<assignment_expression> ae : ptype->argument_list()) {
						shared_ptr<operand> arg = generate_assignment_expression_middle_ir(mi, ae, false);
						shared_ptr<type> type_to_set = ae->assignment_expression_type();
						if (index < ft->parameter_list().size()) {
							shared_ptr<type> par_type = ft->parameter_list()[index]->variable_declaration_type(),
								arg_type = ae->assignment_expression_type();
							arg = ir_cast(mi, arg, arg_type, par_type);
							type_to_set = par_type;
						}
						if (arg->operand_kind() == operand::kind::KIND_REGISTER) {
							shared_ptr<register_operand> arg_reg = static_pointer_cast<register_operand>(arg);
							arg_reg = make_shared<register_operand>(*arg_reg);
							arg_reg->set_register_type(type_to_set);
							arg = arg_reg;
						}
						else if (arg->operand_kind() == operand::kind::KIND_LABEL) {
							shared_ptr<label_operand> arg_lab = static_pointer_cast<label_operand>(arg);
							arg_lab = make_shared<label_operand>(arg_lab->label_text(), type_to_set);
							arg = arg_lab;
						}
						else if (arg->operand_kind() == operand::kind::KIND_IMMEDIATE) {
							shared_ptr<immediate_operand> arg_imm = static_pointer_cast<immediate_operand>(arg);
							auto imm = arg_imm->immediate();
							arg = make_shared<immediate_operand>(type_to_set, imm);
						}
						else
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						args.push_back(arg);
						index++;
					}
					shared_ptr<operand> res = mi->generate_next_register(ptype->postfix_type_type());
					mi->add_insn(make_shared<call_insn>(ptype->postfix_type_type(), res, op, args, ft));
					if (is_struct_type(mi, ptype->postfix_type_type())) {
						shared_ptr<register_operand> local_copy = mi->generate_next_register(ptype->postfix_type_type());
						shared_ptr<size_data> sd = spectre_sizeof(mi, ptype->postfix_type_type());
						mi->add_insn(make_shared<align_insn>(sd->alignment));
						mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_STK, local_copy,
							make_shared<immediate_operand>(uint_type, (int) sd->size), ptype->postfix_type_type()));
						mi->add_insn(make_shared<memcpy_insn>(local_copy, res, sd->size, ptype->postfix_type_type()));
						op = local_copy;
					}
					else
						op = res;
					prev_vk = value_kind::VALUE_RVALUE;
				}
					break;
				default:
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
					break;
				}
				prev_type = ptype->postfix_type_type();
				first_postfix = false;
			}
			if (lvalue) return op;
			else {
				if (prev_vk == value_kind::VALUE_LVALUE && !is_struct_type(mi, pe->postfix_expression_type()))
					return load_lvalue_to_register(mi, op);
				else
					return op;
			}
			mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		char literal_token_to_char(shared_ptr<middle_ir> mi, token tok) {
			if (tok.token_kind() != token::kind::TOKEN_CHARACTER || tok.raw_text().size() < 2
				|| tok.raw_text()[0] != '\'' || tok.raw_text()[tok.raw_text().size() - 1] != '\'')
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			string char_text = tok.raw_text().substr(1, tok.raw_text().size() - 2);
			if (char_text[0] == '\\') {
				if (char_text.size() == 1)
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				if (char_text[1] == '\'')
					return '\'';
				else if (char_text[1] == '\"')
					return '\"';
				else if (char_text[1] == '\\')
					return '\\';
				else if (char_text[1] == '?')
					return '?';
				else if (char_text[1] == 'a')
					return '\a';
				else if (char_text[1] == 'b')
					return '\b';
				else if (char_text[1] == 'f')
					return '\f';
				else if (char_text[1] == 'n')
					return '\n';
				else if (char_text[1] == 'r')
					return '\r';
				else if (char_text[1] == 't')
					return '\t';
				else if (char_text[1] == 'v')
					return '\v';
				else if (char_text[1] == 'x') {
					string num = char_text.substr(2);
					int val = stoi(num, 0, 16);
					return val;
				}
				else if (isdigit(char_text[1])) {
					string num = char_text.substr(1);
					int val = stoi(num, 0, 8);
					return val;
				}
				else {
					mi->report(error(error::kind::KIND_ERROR, "Invalid character literal.", { tok }, 0));
					return 0;
				}
			}
			else
				return char_text[0];
			mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return 0;
		}

		shared_ptr<operand> generate_primary_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<primary_expression> pe, bool lvalue) {
			if (pe == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			switch (pe->primary_expression_kind()) {
			case primary_expression::kind::KIND_LITERAL: {
				token tok = pe->literal_token();
				switch (tok.token_kind()) {
				case token::kind::TOKEN_INTEGER: {
					string to_use = tok.raw_text();
					int base = 10;
					switch (tok.prefix_kind()) {
					case token::prefix::PREFIX_HEXADECIMAL:
						to_use = to_use.substr(2);
						base = 16;
						break;
					case token::prefix::PREFIX_BINARY:
						to_use = to_use.substr(2);
						base = 2;
						break;
					case token::prefix::PREFIX_OCTAL:
						to_use = to_use.substr(2);
						base = 8;
						break;
					}
					to_use.erase(remove(to_use.begin(), to_use.end(), '\''), to_use.end());
					unsigned long num = stoul(to_use, nullptr, base);
					if (tok.suffix_kind() == token::suffix::SUFFIX_DOUBLE)
						return make_shared<immediate_operand>(double_type, (double) num);
					else if (tok.suffix_kind() == token::suffix::SUFFIX_FLOAT)
						return make_shared<immediate_operand>(float_type, (float) num);
					else if (tok.suffix_kind() == token::suffix::SUFFIX_UNSIGNED_LONG)
						return make_shared<immediate_operand>(ulong_type, (long) num);
					else if (tok.suffix_kind() == token::suffix::SUFFIX_SIGNED_LONG)
						return make_shared<immediate_operand>(long_type, (long) num);
					else if (tok.suffix_kind() == token::suffix::SUFFIX_UNSIGNED_INT)
						return make_shared<immediate_operand>(uint_type, (int) num);
					else if (tok.suffix_kind() == token::suffix::SUFFIX_NONE)
						return make_shared<immediate_operand>(int_type, (int) num);
					else if (tok.suffix_kind() == token::suffix::SUFFIX_UNSIGNED_SHORT)
						return make_shared<immediate_operand>(ushort_type, (short) num);
					else if (tok.suffix_kind() == token::suffix::SUFFIX_SIGNED_SHORT)
						return make_shared<immediate_operand>(short_type, (short) num);
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
				}
					break;
				case token::kind::TOKEN_DECIMAL: {
					if (tok.suffix_kind() == token::suffix::SUFFIX_DOUBLE
						|| tok.suffix_kind() == token::suffix::SUFFIX_NONE)
						return make_shared<immediate_operand>(double_type, atof(tok.raw_text().c_str()));
					else if (tok.suffix_kind() == token::suffix::SUFFIX_FLOAT)
						return make_shared<immediate_operand>(float_type, (float)atof(tok.raw_text().c_str()));
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
				}
					break;
				case token::kind::TOKEN_STRING: {
					string full;
					for (token t : pe->stream()) {
						if (t.raw_text().size() < 2)
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						full += t.raw_text().substr(1, t.raw_text().size() - 2);
					}
					return make_shared<immediate_operand>(string_type, full);
				}
					break;
				case token::kind::TOKEN_CHARACTER:
					return make_shared<immediate_operand>(uchar_type, literal_token_to_char(mi, tok));
					break;
				case token::kind::TOKEN_TRUE:
				case token::kind::TOKEN_FALSE: {
					bool val = tok.token_kind() == token::kind::TOKEN_TRUE;
					return make_shared<immediate_operand>(bool_type, val);
				}
					break;
				default:
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return nullptr;
				}
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return nullptr;
			}
				break;
			case primary_expression::kind::KIND_PARENTHESIZED_EXPRESSION:
				return generate_expression_middle_ir(mi, pe->parenthesized_expression(), lvalue);
				break;
			case primary_expression::kind::KIND_STMT_EXPR: {
				shared_ptr<block_stmt> bs = pe->stmt_expression();
				unsigned int i = 0;
				for (shared_ptr<stmt> s : bs->stmt_list()) {
					if (i == bs->stmt_list().size() - 1) {
						if (s->stmt_kind() == stmt::kind::KIND_EXPRESSION)
							return generate_expression_middle_ir(mi, s->stmt_expression(), false);
						else {
							generate_stmt_middle_ir(mi, s);
							return make_shared<immediate_operand>(void_type, (char)0);
						}
					}
					else
						generate_stmt_middle_ir(mi, s);
					i++;
				}
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return nullptr;
			}
				break;
			case primary_expression::kind::KIND_SIZEOF_TYPE:
				return make_shared<immediate_operand>(uint_type, (int)spectre_sizeof(mi, pe->sizeof_type())->size);
				break;
			case primary_expression::kind::KIND_SIZEOF_EXPRESSION:
				return make_shared<immediate_operand>(uint_type, (int)spectre_sizeof(mi, pe->parenthesized_expression()->expression_type())->size);
				break;
			case primary_expression::kind::KIND_ALIGNOF_TYPE:
				return make_shared<immediate_operand>(uint_type, (int)spectre_sizeof(mi, pe->sizeof_type())->alignment);
				break;
			case primary_expression::kind::KIND_ALIGNOF_EXPRESSION:
				return make_shared<immediate_operand>(uint_type, (int)spectre_sizeof(mi, pe->parenthesized_expression()->expression_type())->alignment);
				break;
			case primary_expression::kind::KIND_IDENTIFIER: {
				string ident_sym = c_symbol_2_string(mi, pe->identifier_symbol());
				auto constexpr_lookup = mi->lookup_constexpr_mapping(ident_sym);
				if (constexpr_lookup.first)
					return constexpr_value_2_operand(mi, constexpr_lookup.second, pe->primary_expression_type());
				else {
					shared_ptr<register_operand> res = mi->generate_next_register(pe->primary_expression_type()),
						res_temp = make_shared<register_operand>(*res);
					if (pe->identifier_symbol()->symbol_kind() == symbol::kind::KIND_VARIABLE) {
						shared_ptr<register_operand> loc = make_shared<register_operand>(*mi->lookup_variable_register(ident_sym));
						if (!is_struct_type(mi, pe->primary_expression_type()))
							res_temp->set_register_type(add_indirection(res_temp->register_type()));
						mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_IMOV, res_temp, loc, pe->primary_expression_type()));
					}
					else if (pe->identifier_symbol()->symbol_kind() == symbol::kind::KIND_FUNCTION)
						mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_IMOV, res, make_shared<label_operand>(ident_sym, pe->primary_expression_type()),
							pe->primary_expression_type()));
					shared_ptr<register_operand> copy_res = make_shared<register_operand>(*res);
					bool deref = !lvalue;
					if (pe->identifier_symbol()->symbol_kind() == symbol::kind::KIND_VARIABLE) {
						shared_ptr<variable_symbol> vsym = static_pointer_cast<variable_symbol>(pe->identifier_symbol());
						if (vsym->variable_type()->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR
							|| is_struct_type(mi, vsym->variable_type()))
							deref = false;
					}
					else if (pe->identifier_symbol()->symbol_kind() == symbol::kind::KIND_FUNCTION)
						deref = false;
					copy_res->set_dereference(deref);
					return copy_res;
				}
			}
				break;
			case primary_expression::kind::KIND_ARRAY_INITIALIZER: {
				shared_ptr<register_operand> res = mi->generate_next_register(pe->primary_expression_type());
				if (pe->array_initializer().empty())
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				shared_ptr<type> elem_type = pe->array_initializer()[0]->assignment_expression_type();
				shared_ptr<size_data> elem_size_data = spectre_sizeof(mi, elem_type);
				mi->add_insn(make_shared<align_insn>(elem_size_data->alignment));
				mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_RESV, res, make_shared<immediate_operand>(uint_type,
					(int)(elem_size_data->size * pe->array_initializer().size())), pe->primary_expression_type()));
				unsigned int index = 0;
				auto[is_struct, umov] = which_mov_insn(mi, elem_type);
				for (shared_ptr<assignment_expression> ae : pe->array_initializer()) {
					shared_ptr<operand> elem = generate_assignment_expression_middle_ir(mi, ae, false);
					shared_ptr<register_operand> temp_res_base = mi->generate_next_register(pe->primary_expression_type());
					mi->add_insn(make_shared<binary_insn>(binary_insn::kind::KIND_IADD, temp_res_base, res,
						make_shared<immediate_operand>(uint_type, (int)(index * elem_size_data->size)), pe->primary_expression_type()));
					shared_ptr<register_operand> temp_res = make_shared<register_operand>(*temp_res_base);
					temp_res->set_register_type(elem_type);
					if (is_struct)
						mi->add_insn(make_shared<memcpy_insn>(temp_res, elem, elem_size_data->size, elem_type));
					else {
						temp_res->set_dereference(true);
						mi->add_insn(make_shared<unary_insn>(umov, temp_res, elem, elem_type));
					}
					index++;
				}
				return res;
			}
				break;
			case primary_expression::kind::KIND_RESV:
			case primary_expression::kind::KIND_NEW:
			case primary_expression::kind::KIND_STK: {
				unary_insn::kind uk = unary_insn::kind::KIND_NONE;
				if (pe->primary_expression_kind() == primary_expression::kind::KIND_RESV)
					uk = unary_insn::kind::KIND_RESV;
				else if (pe->primary_expression_kind() == primary_expression::kind::KIND_NEW);
				else if (pe->primary_expression_kind() == primary_expression::kind::KIND_STK)
					uk = unary_insn::kind::KIND_STK;
				else
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				shared_ptr<size_data> sd = spectre_sizeof(mi, pe->mem_type());
				shared_ptr<register_operand> res = mi->generate_next_register(pe->mem_type());
				if (pe->primary_expression_kind() == primary_expression::kind::KIND_NEW) {
					shared_ptr<register_operand> sz = mi->generate_next_register(uint_type);
					shared_ptr<operand> num = generate_expression_middle_ir(mi, pe->parenthesized_expression(), false);
					mi->add_insn(make_shared<binary_insn>(binary_insn::kind::KIND_UIMUL, sz, make_shared<immediate_operand>(uint_type, (int) sd->size), num, uint_type));
					mi->add_insn(make_shared<call_insn>(malloc_type->return_type(), res,
						make_shared<label_operand>(malloc_symbol_name, malloc_type), vector<shared_ptr<operand>>{ sz }, malloc_type));
				}
				else {
					variant<bool, int, unsigned int, float, double, string> num;
					for (shared_ptr<assignment_expression> ae : pe->parenthesized_expression()->assignment_expression_list())
						num = evaluate_constant_expression(mi, ae);
					unsigned int sz = 0;
					if (holds_alternative<bool>(num))
						sz = get<bool>(num);
					else if(holds_alternative<int>(num))
						sz = get<int>(num);
					else if(holds_alternative<unsigned int>(num))
						sz = get<unsigned int>(num);
					else if(holds_alternative<float>(num))
						sz = get<float>(num);
					else if(holds_alternative<double>(num))
						sz = get<double>(num);
					else
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (sz == 0)
						mi->report(error(error::kind::KIND_ERROR, "Cannot 'stk' or 'resv' 0 space.", pe->stream(), 0));
					sz *= sd->size;
					mi->add_insn(make_shared<align_insn>(sd->alignment));
					mi->add_insn(make_shared<unary_insn>(uk, res, make_shared<immediate_operand>(uint_type, (int) sz), pe->mem_type()));
				}
				return res;
			}
				break;
			default:
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return nullptr;
			}
			mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		void generate_function_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<function_stmt> fs) {
			if (fs == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			if (fs->function_stmt_defined_kind() != function_stmt::defined_kind::KIND_DEFINITION)
				return;
			shared_ptr<function_insn> fi = make_shared<function_insn>(fs->function_stmt_type(), fs->function_stmt_symbol(), fs->function_stmt_scope(),
				mi->generate_next_register(fs->function_stmt_type()->return_type()));
			mi->set_function_ctx(fi);
			mi->set_return_label(mi->generate_next_label_operand());
			mi->set_return_register(fi->return_register());
			string func_str = c_symbol_2_string(mi, fi->func_symbol());
			shared_ptr<label_insn> func_header_insn = make_shared<label_insn>(make_shared<label_operand>(func_str, fi->func_type()));
			func_header_insn->set_func_return_reg(fi->return_register()->virtual_register_number());
			func_header_insn->set_orig_func_insn(fi);
			mi->add_insn(func_header_insn);
			vector<int> func_param_regs;
			for (shared_ptr<variable_declaration> param : fs->function_stmt_type()->parameter_list()) {
				string param_sym = c_symbol_2_string(mi, param->variable_declaration_symbol());
				shared_ptr<register_operand> var = mi->generate_next_register(param->variable_declaration_type());
				func_param_regs.push_back(var->virtual_register_number());
				shared_ptr<size_data> sd = spectre_sizeof(mi, param->variable_declaration_type());
				mi->add_insn(make_shared<align_insn>(sd->alignment));
				mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_STK, var, make_shared<immediate_operand>(uint_type, (int)sd->size),
					param->variable_declaration_type(), param));
				mi->add_variable(param_sym, param);
				mi->map_variable_register(param_sym, var);
			}
			func_header_insn->set_func_param_regs(func_param_regs);
			shared_ptr<size_data> ret_sd = spectre_sizeof(mi, fi->func_type()->return_type());
			mi->add_insn(make_shared<align_insn>(ret_sd->alignment));
			mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_STK, fi->return_register(), make_shared<immediate_operand>(uint_type, (int)ret_sd->size),
				fi->func_type()->return_type()));
			for (shared_ptr<stmt> s : fs->function_body())
				generate_stmt_middle_ir(mi, s);
			shared_ptr<register_operand> ret_value = make_shared<register_operand>(*fi->return_register());
			if (is_struct_type(mi, fi->func_type()->return_type()))
				ret_value->set_dereference(false);
			else
				ret_value->set_dereference(true);

			main_function_kind mfk = main_function_defined(fs->function_stmt_symbol());
			if (mfk != main_function_kind::NONE) {
				shared_ptr<register_operand> cpy = make_shared<register_operand>(*ret_value);
				mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_IMOV, cpy, make_shared<immediate_operand>(int_type, 0), int_type));
			}
			mi->add_insn(make_shared<label_insn>(mi->get_return_label()));

			mi->add_insn(make_shared<return_insn>(ret_value));
			mi->set_return_label(nullptr);
			mi->set_return_register(nullptr);
			mi->unset_function_ctx();
			mi->add_insn(fi);
			mi->add_function_insn(fi);
		}

		shared_ptr<operand> constexpr_value_2_operand(shared_ptr<middle_ir> mi, variant<bool, int, unsigned int, float, double, string> c, shared_ptr<type> t) {
			shared_ptr<operand> op = nullptr;
			if (holds_alternative<bool>(c)) op = make_shared<immediate_operand>(t, (char) get<bool>(c));
			else if (holds_alternative<int>(c)) op = make_shared<immediate_operand>(t, get<int>(c));
			else if (holds_alternative<unsigned int>(c)) op = make_shared<immediate_operand>(t, (int)get<unsigned int>(c));
			else if (holds_alternative<float>(c)) op = make_shared<immediate_operand>(t, get<float>(c));
			else if (holds_alternative<double>(c)) op = make_shared<immediate_operand>(t, get<double>(c));
			else if (holds_alternative<string>(c)) op = make_shared<label_operand>(get<string>(c), t);
			else mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return op;
		}

		template<typename C, typename TL, typename TR> auto raw_arithmetic_binary_expression_evaluator(shared_ptr<middle_ir> mi, TL lhs, TR rhs, binary_expression::operator_kind ok) {
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
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return (C)0;
				break;
			}
		}

		template<typename C, typename TL, typename TR> bool raw_logical_binary_expression_evaluator(shared_ptr<middle_ir> mi, TL lhs, TR rhs, binary_expression::operator_kind ok) {
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
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return false;
				break;
			}
		}

		variant<bool, int, unsigned int, float, double, string> evaluate_constant_expression(shared_ptr<middle_ir> mi, shared_ptr<assignment_expression> ae) {
			function<variant<bool, int, unsigned int, float, double, string>(shared_ptr<expression>)> evaluate_expression;
			function<variant<bool, int, unsigned int, float, double, string>(shared_ptr<assignment_expression>)> evaluate_assignment_expression;
			function<variant<bool, int, unsigned int, float, double, string>(shared_ptr<ternary_expression>)> evaluate_ternary_expression;
			function<variant<bool, int, unsigned int, float, double, string>(shared_ptr<binary_expression>)> evaluate_binary_expression;
			mi->set_global_ctx_set(true);
			auto evaluate_primary_expression = [&](shared_ptr<primary_expression> pexpr) {
				variant<bool, int, unsigned int, float, double, string> dummy, result;
				if (pexpr->primary_expression_kind() == primary_expression::kind::KIND_SIZEOF_TYPE) {
					result = (unsigned int)spectre_sizeof(mi, pexpr->sizeof_type())->size;
					return result;
				}
				else if (pexpr->primary_expression_kind() == primary_expression::kind::KIND_SIZEOF_EXPRESSION) {
					evaluate_expression(pexpr->parenthesized_expression());
					result = (unsigned int)spectre_sizeof(mi, pexpr->parenthesized_expression()->expression_type())->size;
					return result;
				}
				else if (pexpr->primary_expression_kind() == primary_expression::kind::KIND_PARENTHESIZED_EXPRESSION) {
					result = evaluate_expression(pexpr->parenthesized_expression());
					return result;
				}
				else if (pexpr->primary_expression_kind() == primary_expression::kind::KIND_RESV) {
					variant<bool, int, unsigned int, float, double, string> par = evaluate_expression(pexpr->parenthesized_expression());
					shared_ptr<size_data> sd = spectre_sizeof(mi, pexpr->mem_type());
					int sz = sd->size;
					if (holds_alternative<bool>(par)) sz *= (int)get<bool>(par);
					else if (holds_alternative<int>(par)) sz *= get<int>(par);
					else if (holds_alternative<unsigned int>(par)) sz *= (int)get<unsigned int>(par);
					else if (holds_alternative<float>(par)) sz *= (int)get<float>(par);
					else if (holds_alternative<double>(par)) sz *= (int)get<double>(par);
					else {
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return dummy;
					}
					string lab = "__resv_const_" + to_string(mi->next_misc_counter());
					shared_ptr<label_operand> lab_op = make_shared<label_operand>(lab, pexpr->primary_expression_type());
					mi->add_insn(make_shared<align_insn>(sd->alignment));
					mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_RESV, lab_op, make_shared<immediate_operand>(uint_type, (int) sz), pexpr->primary_expression_type()));
					result = lab;
					return result;
				}
				else if (pexpr->primary_expression_kind() == primary_expression::kind::KIND_ARRAY_INITIALIZER) {
					vector<shared_ptr<operand>> vec;
					global_array_insn::directive_kind dk;
					shared_ptr<type> arr_type = pexpr->primary_expression_type();
					string lab = "__temp_const_array_" + to_string(mi->next_misc_counter());
					if (arr_type->type_array_kind() == type::array_kind::KIND_ARRAY && arr_type->array_dimensions() > 1) dk = global_array_insn::directive_kind::KIND_WORD;
					else {
						if (arr_type->type_kind() != type::kind::KIND_PRIMITIVE) {
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return dummy;
						}
						shared_ptr<primitive_type> arr_ptype = static_pointer_cast<primitive_type>(arr_type);
						if (arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_BOOL || arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_BYTE ||
							arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_CHAR)
							dk = global_array_insn::directive_kind::KIND_BYTE;
						else if (arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_SHORT)
							dk = global_array_insn::directive_kind::KIND_HALF;
						else if (arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_LONG || arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_INT)
							dk = global_array_insn::directive_kind::KIND_WORD;
						else if (arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
							dk = global_array_insn::directive_kind::KIND_FLOAT;
						else if (arr_ptype->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
							dk = global_array_insn::directive_kind::KIND_DOUBLE;
						else {
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return dummy;
						}
					}
					shared_ptr<type> aetype = nullptr;
					for (shared_ptr<assignment_expression> ae : pexpr->array_initializer()) {
						variant<bool, int, unsigned int, float, double, string> el = evaluate_assignment_expression(ae);
						aetype = ae->assignment_expression_type();
						if (dk == global_array_insn::directive_kind::KIND_DOUBLE) {
							if (holds_alternative<string>(el)) vec.push_back(make_shared<label_operand>(get<string>(el), aetype));
							else if (holds_alternative<bool>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (double)get<bool>(el)));
							else if (holds_alternative<int>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (double)get<int>(el)));
							else if (holds_alternative<unsigned int>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (double)get<unsigned int>(el)));
							else if (holds_alternative<float>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (double)get<float>(el)));
							else if (holds_alternative<double>(el)) vec.push_back(make_shared<immediate_operand>(aetype, get<double>(el)));
							else {
								mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return dummy;
							}
						}
						else if (dk == global_array_insn::directive_kind::KIND_FLOAT) {
							if (holds_alternative<string>(el)) vec.push_back(make_shared<label_operand>(get<string>(el), aetype));
							else if (holds_alternative<bool>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (float)get<bool>(el)));
							else if (holds_alternative<int>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (float)get<int>(el)));
							else if (holds_alternative<unsigned int>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (float)get<unsigned int>(el)));
							else if (holds_alternative<float>(el)) vec.push_back(make_shared<immediate_operand>(aetype, get<float>(el)));
							else if (holds_alternative<double>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (float)get<double>(el)));
							else {
								mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return dummy;
							}
						}
						else if (dk == global_array_insn::directive_kind::KIND_WORD) {
							if (holds_alternative<string>(el)) vec.push_back(make_shared<label_operand>(get<string>(el), aetype));
							else if (holds_alternative<bool>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (int)get<bool>(el)));
							else if (holds_alternative<int>(el)) vec.push_back(make_shared<immediate_operand>(aetype, get<int>(el)));
							else if (holds_alternative<unsigned int>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (int)get<unsigned int>(el)));
							else if (holds_alternative<float>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (int)get<float>(el)));
							else if (holds_alternative<double>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (int)get<double>(el)));
							else {
								mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return dummy;
							}
						}
						else if (dk == global_array_insn::directive_kind::KIND_HALF) {
							if (holds_alternative<string>(el)) vec.push_back(make_shared<label_operand>(get<string>(el), aetype));
							else if (holds_alternative<bool>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (short)get<bool>(el)));
							else if (holds_alternative<int>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (short)get<int>(el)));
							else if (holds_alternative<unsigned int>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (short)get<unsigned int>(el)));
							else if (holds_alternative<float>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (short)get<float>(el)));
							else if (holds_alternative<double>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (short)get<double>(el)));
							else {
								mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return dummy;
							}
						}
						else if (dk == global_array_insn::directive_kind::KIND_BYTE) {
							if (holds_alternative<string>(el)) vec.push_back(make_shared<label_operand>(get<string>(el), aetype));
							else if (holds_alternative<bool>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (char)get<bool>(el)));
							else if (holds_alternative<int>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (char) get<int>(el)));
							else if (holds_alternative<unsigned int>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (char)get<unsigned int>(el)));
							else if (holds_alternative<float>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (char)get<float>(el)));
							else if (holds_alternative<double>(el)) vec.push_back(make_shared<immediate_operand>(aetype, (char)get<double>(el)));
							else {
								mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return dummy;
							}
						}
					}
					if (aetype == nullptr)
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<size_data> sd = spectre_sizeof(mi, aetype);
					shared_ptr<label_operand> lab_op = make_shared<label_operand>(lab, pexpr->primary_expression_type());
					mi->add_insn(make_shared<align_insn>(sd->alignment));
					mi->add_insn(make_shared<global_array_insn>(dk, lab_op, vec, pexpr->primary_expression_type()));
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
								mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
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
						int counter = mi->next_misc_counter();
						string s_lit = "__sp_lit_" + to_string(counter);
						mi->add_insn(make_shared<align_insn>(1));
						string concated_raw_lit;
						for (token t : pexpr->stream())
							concated_raw_lit += t.raw_text().substr(1, t.raw_text().length() - 2);
						shared_ptr<immediate_operand> str_imm = make_shared<immediate_operand>(pexpr->primary_expression_type(), concated_raw_lit);
						shared_ptr<label_operand> str_lab = make_shared<label_operand>(s_lit, pexpr->primary_expression_type());
						mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_IMOV, str_lab, str_imm, pexpr->primary_expression_type()));
						result = s_lit;
					}
						break;
					}
					return result;
				}
				else if (pexpr->primary_expression_kind() == primary_expression::kind::KIND_IDENTIFIER) {
					shared_ptr<symbol> sym = pexpr->identifier_symbol();
					string sym_string = c_symbol_2_string(mi, sym);
					pair<bool, variant<bool, int, unsigned int, float, double, string>> check = mi->lookup_constexpr_mapping(sym_string);
					if (!check.first) {
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return dummy;
					}
					result = check.second;
					return result;
				}
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return dummy;
			};
			auto evaluate_postfix_expression = [&](shared_ptr<postfix_expression> pexpr) {
				variant<bool, int, unsigned int, float, double, string> dummy, result = evaluate_primary_expression(pexpr->contained_primary_expression());
				shared_ptr<type> prev_type = pexpr->contained_primary_expression()->primary_expression_type();
				for (shared_ptr<postfix_expression::postfix_type> pt : pexpr->postfix_type_list()) {
					if (pt->postfix_type_kind() != postfix_expression::kind::KIND_AS) {
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return dummy;
					}
					shared_ptr<type> to_type = pt->postfix_type_type();
					if (to_type->type_array_kind() != type::array_kind::KIND_ARRAY) {
						if (prev_type->type_array_kind() == type::array_kind::KIND_ARRAY || holds_alternative<string>(result) || to_type->type_kind() != type::kind::KIND_PRIMITIVE) {
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
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
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
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
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
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
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
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
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
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
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return dummy;
						}
					}
						break;
					default: {
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
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
					shared_ptr<middle_ir> _mi;
					result_variant_type& _res;
				public:
					collapsing_variant_assignment_visitor(shared_ptr<middle_ir> mi, result_variant_type& r) : _mi(mi), _res(r) {}

					void operator()(bool b) { _res = b; }
					void operator()(int i) { _res = i; }
					void operator()(unsigned int u) { _res = u; }
					void operator()(float f) { _res = f; }
					void operator()(double d) { _res = d; }
					void operator()(string s) { _mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__); }
				};
				visit(collapsing_variant_assignment_visitor{ mi, temp_lhs_result }, lhs_result);
				visit(collapsing_variant_assignment_visitor{ mi, temp_rhs_result }, rhs_result);

				// SIGH...
#define SPECTRE_OPERATOR_BODY(T) \
	result_variant_type result; \
	if(_logical) \
		result = raw_logical_binary_expression_evaluator<T>(_mi, lhs, rhs, _operator_kind); \
	else \
		result = raw_arithmetic_binary_expression_evaluator<T>(_mi, lhs, rhs, _operator_kind); \
	return result

				struct custom_variant_visitor {
				private:
					binary_expression::operator_kind _operator_kind;
					shared_ptr<middle_ir> _mi;
					bool _logical;
				public:
					custom_variant_visitor(shared_ptr<middle_ir> mi, binary_expression::operator_kind ok) : _mi(mi), _operator_kind(ok),
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

				result_variant_type temp_result = visit(custom_variant_visitor{ mi, bexpr->binary_expression_operator_kind() }, temp_lhs_result, temp_rhs_result);
				visit([&result](auto t) { result = t; }, temp_result);
				return result;
			};
			evaluate_ternary_expression = [&](shared_ptr<ternary_expression> texpr) {
				variant<bool, int, unsigned int, float, double, string> dummy;
				variant<bool, int, unsigned int, float, double, string> cond_result = evaluate_binary_expression(texpr->condition());
				if (texpr->ternary_expression_kind() == ternary_expression::kind::KIND_BINARY)
					return cond_result;
				if (!holds_alternative<bool>(cond_result)) {
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
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
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
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
			mi->set_global_ctx_set(false);
			return evaluate_assignment_expression(ae);
		}

		void generate_variable_declaration_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<variable_declaration> vd) {
			if (vd == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<type> vd_type = vd->variable_declaration_type();
			shared_ptr<variable_symbol> vd_sym = vd->variable_declaration_symbol();
			string vd_sym_string = c_symbol_2_string(mi, vd->variable_declaration_symbol());
			shared_ptr<size_data> variable_size = spectre_sizeof(mi, vd_type);
			if (vd_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
				if (vd->variable_declaration_initialization_kind() != variable_declaration::initialization_kind::KIND_PRESENT)
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				auto cv = evaluate_constant_expression(mi, vd->initialization());
				shared_ptr<operand> temp_res = constexpr_value_2_operand(mi, cv, vd_type);
				shared_ptr<register_operand> res = to_register_operand(mi, ir_cast(mi, temp_res, vd->initialization()->assignment_expression_type(), vd_type));
				mi->add_constexpr_mapping(vd_sym_string, cv);
				mi->add_variable(vd_sym_string, vd);
				return;
			}
			bool on_stack = !(vd_sym->parent_scope()->scope_kind() == scope::kind::KIND_NAMESPACE || vd_sym->parent_scope()->scope_kind() == scope::kind::KIND_GLOBAL
				|| vd_type->type_static_kind() == type::static_kind::KIND_STATIC),
				is_static = vd_type->type_static_kind() == type::static_kind::KIND_STATIC,
				global_var = vd_sym->parent_scope()->scope_kind() == scope::kind::KIND_NAMESPACE || vd_sym->parent_scope()->scope_kind() == scope::kind::KIND_GLOBAL,
				local_var = !global_var;
			mi->set_global_ctx_set(vd_sym->parent_scope()->scope_kind() == scope::kind::KIND_GLOBAL
			|| vd_sym->parent_scope()->scope_kind() == scope::kind::KIND_NAMESPACE);
			shared_ptr<register_operand> base_loc = mi->generate_next_register(vd_type);
			mi->add_insn(make_shared<align_insn>(variable_size->alignment));
			mi->add_insn(make_shared<unary_insn>(on_stack ? unary_insn::kind::KIND_STK : unary_insn::kind::KIND_RESV, base_loc,
				make_shared<immediate_operand>(uint_type, (int) variable_size->size), vd_type));
			mi->add_variable(vd_sym_string, vd);
			mi->map_variable_register(vd_sym_string, base_loc);
			if (vd->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_PRESENT) {
				shared_ptr<label_operand> static_lab = nullptr;
				if (is_static && local_var) {
					shared_ptr<register_operand> static_check_base = mi->generate_next_register(bool_type),
						static_check = make_shared<register_operand>(*static_check_base),
						scratch = mi->generate_next_register(bool_type);
					static_check->set_dereference(true);
					static_lab = mi->generate_next_label_operand();
					mi->add_global_insn(make_shared<align_insn>(1));
					mi->add_global_insn(make_shared<unary_insn>(unary_insn::kind::KIND_RESV, static_check_base,
						make_shared<immediate_operand>(uint_type, (int)1), bool_type));
					mi->add_insn(make_shared<binary_insn>(binary_insn::kind::KIND_CEQ, scratch, static_check,
						make_shared<immediate_operand>(bool_type, (char)1), bool_type));
					mi->add_insn(make_shared<conditional_insn>(scratch, static_lab));
					mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_CMOV, static_check,
						make_shared<immediate_operand>(bool_type, (char)1), bool_type));
				}
				shared_ptr<register_operand> loc = make_shared<register_operand>(*base_loc);
				loc->set_dereference(true);
				shared_ptr<operand> init = nullptr;
				if (!global_var)
					init = generate_assignment_expression_middle_ir(mi, vd->initialization(), false);
				else
					init = constexpr_value_2_operand(mi, evaluate_constant_expression(mi, vd->initialization()), vd_type);
				shared_ptr<operand> casted_init = ir_cast(mi, init, vd->initialization()->assignment_expression_type(), vd_type);
				auto[is_struct, umov] = which_mov_insn(mi, vd_type);
				if (is_struct)
					mi->add_insn(make_shared<memcpy_insn>(base_loc, casted_init, variable_size->size, vd_type));
				else
					mi->add_insn(make_shared<unary_insn>(umov, loc, casted_init, vd_type));
				if (is_static && local_var) {
					if (static_lab == nullptr)
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					mi->add_insn(make_shared<label_insn>(static_lab));
				}
			}
			mi->set_global_ctx_set(false);
		}

		void generate_return_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<return_stmt> rs) {
			if (rs == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<register_operand> ret_reg_base = mi->get_return_register();
			if (ret_reg_base == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<label_operand> ret_lab = mi->get_return_label();
			if (ret_lab == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<function_insn> fctx = mi->get_function_ctx();
			if (fctx == nullptr || !mi->function_ctx_set())
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			if (rs->return_stmt_value_kind() == return_stmt::value_kind::KIND_EXPRESSION) {
				shared_ptr<type> from_type = rs->return_value()->expression_type(),
					to_type = fctx->func_type()->return_type();
				shared_ptr<operand> ret_value = generate_expression_middle_ir(mi, rs->return_value(), false);
				ret_value = ir_cast(mi, ret_value, from_type, to_type);
				auto[is_struct, umov] = which_mov_insn(mi, to_type);
				shared_ptr<register_operand> ret_reg = make_shared<register_operand>(*ret_reg_base);
				ret_reg->set_dereference(true);
				if (is_struct) {
					shared_ptr<size_data> struct_size = spectre_sizeof(mi, to_type);
					mi->add_insn(make_shared<memcpy_insn>(ret_reg_base, ret_value, struct_size->size, to_type));
				}
				else
					mi->add_insn(make_shared<unary_insn>(umov, ret_reg, ret_value, to_type));
			}
			mi->add_insn(make_shared<jump_insn>(ret_lab));
		}

		void generate_if_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<if_stmt> is) {
			if (is == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<label_operand> false_path = mi->generate_next_label_operand(),
				done_path = mi->generate_next_label_operand();
			if (is->if_stmt_init_decl_kind() == if_stmt::init_decl_kind::KIND_INIT_DECL_PRESENT) {
				for (shared_ptr<variable_declaration> vd : is->init_decl_list())
					generate_variable_declaration_middle_ir(mi, vd);
			}
			shared_ptr<operand> expr_res = generate_expression_middle_ir(mi, is->condition(), false);
			shared_ptr<immediate_operand> czero = make_shared<immediate_operand>(bool_type, (char)0);
			shared_ptr<register_operand> cond_res = mi->generate_next_register(bool_type);
			mi->add_insn(make_shared<binary_insn>(binary_insn::kind::KIND_CEQ, cond_res, expr_res, czero, bool_type));
			mi->add_insn(make_shared<conditional_insn>(cond_res, false_path));
			generate_stmt_middle_ir(mi, is->true_path());
			mi->add_insn(make_shared<jump_insn>(done_path));
			mi->add_insn(make_shared<label_insn>(false_path));
			if (is->if_stmt_else_kind() == if_stmt::else_kind::KIND_ELSE_PRESENT)
				generate_stmt_middle_ir(mi, is->false_path());
			mi->add_insn(make_shared<label_insn>(done_path));
		}

		void generate_while_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<while_stmt> ws) {
			if (ws == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<label_operand> start_lab = mi->generate_next_label_operand(),
				done_lab = mi->generate_next_label_operand();
			shared_ptr<immediate_operand> czero = make_shared<immediate_operand>(bool_type, (char)0);
			shared_ptr<register_operand> cond_res = mi->generate_next_register(bool_type);
			mi->push_loop_end_stack(done_lab);
			mi->push_loop_start_stack(start_lab);
			mi->add_insn(make_shared<label_insn>(start_lab));
			if (ws->while_stmt_kind() == while_stmt::while_kind::KIND_DO_WHILE)
				generate_stmt_middle_ir(mi, ws->while_body());
			shared_ptr<operand> expr_res = generate_expression_middle_ir(mi, ws->condition(), false);
			mi->add_insn(make_shared<binary_insn>(binary_insn::kind::KIND_CEQ, cond_res, expr_res, czero, bool_type));
			mi->add_insn(make_shared<conditional_insn>(cond_res, done_lab));
			if (ws->while_stmt_kind() == while_stmt::while_kind::KIND_WHILE)
				generate_stmt_middle_ir(mi, ws->while_body());
			mi->add_insn(make_shared<jump_insn>(start_lab));
			mi->add_insn(make_shared<label_insn>(done_lab));
			mi->pop_loop_end_stack();
			mi->pop_loop_start_stack();
		}

		void generate_block_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<block_stmt> bs) {
			if (bs == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			for (shared_ptr<stmt> s : bs->stmt_list())
				generate_stmt_middle_ir(mi, s);
		}

		void generate_break_continue_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<break_continue_stmt> bcs) {
			if (bcs == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			switch (bcs->break_continue_stmt_kind()) {
			case break_continue_stmt::kind::KIND_BREAK:
				mi->add_insn(make_shared<jump_insn>(mi->loop_end()));
				break;
			case break_continue_stmt::kind::KIND_CONTINUE:
				mi->add_insn(make_shared<jump_insn>(mi->loop_start()));
				break;
			default:
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				break;
			}
		}

		void generate_switch_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<switch_stmt> ss) {
			if (ss == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<operand> expr_res = generate_expression_middle_ir(mi, ss->parent_expression(), false);
			binary_insn::kind beq = binary_insn::kind::KIND_NONE;
			if (is_double_type(mi, ss->parent_expression()->expression_type()))
				beq = binary_insn::kind::KIND_DEQ;
			else if (is_float_type(mi, ss->parent_expression()->expression_type()))
				beq = binary_insn::kind::KIND_FEQ;
			else if (is_long_type(mi, ss->parent_expression()->expression_type()))
				beq = binary_insn::kind::KIND_LEQ;
			else if (is_int_type(mi, ss->parent_expression()->expression_type())
				|| is_pointer_type(mi, ss->parent_expression()->expression_type())
				|| is_struct_type(mi, ss->parent_expression()->expression_type()))
				beq = binary_insn::kind::KIND_IEQ;
			else if (is_short_type(mi, ss->parent_expression()->expression_type()))
				beq = binary_insn::kind::KIND_SEQ;
			else if (is_char_type(mi, ss->parent_expression()->expression_type())
				|| is_bool_type(mi, ss->parent_expression()->expression_type())
				|| is_byte_type(mi, ss->parent_expression()->expression_type()))
				beq = binary_insn::kind::KIND_CEQ;
			else
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<label_operand> default_lab = nullptr,
				done_lab = mi->generate_next_label_operand();
			mi->push_loop_end_stack(done_lab);
			for (shared_ptr<case_default_stmt> cds : ss->switch_stmt_scope()->case_default_stmt_list()) {
				switch (cds->case_default_stmt_kind()) {
				case case_default_stmt::kind::KIND_CASE: {
					shared_ptr<operand> case_expr = generate_expression_middle_ir(mi, cds->case_expression(), false);
					case_expr = ir_cast(mi, case_expr, cds->case_expression()->expression_type(),
						ss->parent_expression()->expression_type());
					shared_ptr<label_operand> case_lab = make_shared<label_operand>(CASE_PREFIX + std::to_string(cds->case_default_stmt_label_number()),
						raw_pointer_type);
					shared_ptr<register_operand> comp_res = mi->generate_next_register(bool_type);
					mi->add_insn(make_shared<binary_insn>(beq, comp_res, expr_res, case_expr, bool_type));
					mi->add_insn(make_shared<conditional_insn>(comp_res, case_lab));
				}
					break;
				case case_default_stmt::kind::KIND_DEFAULT:
					default_lab = make_shared<label_operand>(DEFAULT_PREFIX + std::to_string(cds->case_default_stmt_label_number()),
						raw_pointer_type);
					break;
				default:
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return;
					break;
				}
			}
			if (ss->switch_stmt_scope()->default_stmt_hit()) {
				if (default_lab == nullptr)
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				mi->add_insn(make_shared<jump_insn>(default_lab));
			}
			mi->add_insn(make_shared<jump_insn>(done_lab));
			generate_stmt_middle_ir(mi, ss->switch_stmt_body());
			mi->add_insn(make_shared<label_insn>(done_lab));
			mi->pop_loop_end_stack();
		}

		void generate_case_default_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<case_default_stmt> cds) {
			if (cds == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			string lab_text;
			switch (cds->case_default_stmt_kind()) {
			case case_default_stmt::kind::KIND_CASE:
				lab_text = CASE_PREFIX;
				break;
			case case_default_stmt::kind::KIND_DEFAULT:
				lab_text = DEFAULT_PREFIX;
				break;
			default:
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				break;
			}
			lab_text += std::to_string(cds->case_default_stmt_label_number());
			shared_ptr<label_operand> lab_op = make_shared<label_operand>(lab_text, raw_pointer_type);
			mi->add_insn(make_shared<label_insn>(lab_op));
			generate_stmt_middle_ir(mi, cds->case_default_stmt_body());
		}

		void generate_for_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<for_stmt> fs) {
			if (fs == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<label_operand> start_lab = mi->generate_next_label_operand(),
				done_lab = mi->generate_next_label_operand(),
				continue_lab = mi->generate_next_label_operand();
			if (fs->for_stmt_initializer_present_kind() == for_stmt::initializer_present_kind::INITIALIZER_PRESENT) {
				for (shared_ptr<variable_declaration> vd : fs->initializer())
					generate_variable_declaration_middle_ir(mi, vd);
			}
			mi->push_loop_end_stack(done_lab);
			mi->push_loop_start_stack(continue_lab);
			mi->add_insn(make_shared<label_insn>(start_lab));
			if (fs->for_stmt_condition_present_kind() == for_stmt::condition_present_kind::CONDITION_PRESENT) {
				shared_ptr<operand> res = generate_expression_middle_ir(mi, fs->condition(), false),
					scratch = mi->generate_next_register(bool_type);
				shared_ptr<immediate_operand> czero = make_shared<immediate_operand>(bool_type, (char)0);
				mi->add_insn(make_shared<binary_insn>(binary_insn::kind::KIND_CEQ, scratch, res, czero, bool_type));
				mi->add_insn(make_shared<conditional_insn>(scratch, done_lab));
			}
			generate_stmt_middle_ir(mi, fs->for_stmt_body());
			mi->add_insn(make_shared<label_insn>(continue_lab));
			if (fs->for_stmt_update_present_kind() == for_stmt::update_present_kind::UPDATE_PRESENT)
				generate_expression_middle_ir(mi, fs->update(), false);
			mi->add_insn(make_shared<jump_insn>(start_lab));
			mi->add_insn(make_shared<label_insn>(done_lab));
			mi->pop_loop_end_stack();
			mi->pop_loop_start_stack();
		}

		void generate_delete_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<delete_stmt> ds) {
			if (ds == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<operand> res = generate_expression_middle_ir(mi, ds->expr(), false);
			if (res->operand_kind() == operand::kind::KIND_REGISTER) {
				shared_ptr<register_operand> reg_op = static_pointer_cast<register_operand>(res);
				reg_op = make_shared<register_operand>(*reg_op);
				reg_op->set_register_type(raw_pointer_type);
				res = reg_op;
			}
			else if (res->operand_kind() == operand::kind::KIND_LABEL) {
				shared_ptr<label_operand> lab_op = static_pointer_cast<label_operand>(res);
				lab_op = make_shared<label_operand>(lab_op->label_text(), raw_pointer_type);
				res = lab_op;
			}
			else if (res->operand_kind() == operand::kind::KIND_IMMEDIATE) {
				shared_ptr<immediate_operand> imm_op = static_pointer_cast<immediate_operand>(res);
				imm_op = make_shared<immediate_operand>(raw_pointer_type, imm_op->immediate());
				res = imm_op;
			}
			else
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			shared_ptr<register_operand> dummy = mi->generate_next_register(void_type);
			mi->add_insn(make_shared<call_insn>(void_type, dummy, make_shared<label_operand>(free_symbol_name, free_type),
				vector<shared_ptr<operand>>{ res }, free_type));
		}

		void generate_asm_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<asm_stmt> as) {
			if (as == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			for (shared_ptr<asm_stmt::asm_type> at : as->asm_type_list()) {
				switch (at->asm_type_kind()) {
				case asm_stmt::kind::KIND_LA: {
					string full;
					for (token t : at->raw_register()) {
						if (t.raw_text().size() < 2)
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						full += t.raw_text().substr(1, t.raw_text().size() - 2);
					}
					switch (at->identifier_symbol()->symbol_kind()) {
					case symbol::kind::KIND_FUNCTION: {
						shared_ptr<register_operand> floc = mi->generate_next_register(raw_pointer_type);
						shared_ptr<label_operand> fsym_lab = make_shared<label_operand>(c_symbol_2_string(mi, at->identifier_symbol()), raw_pointer_type);
						mi->add_insn(make_shared<unary_insn>(unary_insn::kind::KIND_IMOV, floc, fsym_lab, raw_pointer_type));
						mi->add_insn(make_shared<asm_insn>(make_pair(full, floc)));
					}
						break;
					case symbol::kind::KIND_VARIABLE: {
						shared_ptr<variable_symbol> vsym = static_pointer_cast<variable_symbol>(at->identifier_symbol());
						if (vsym->variable_type()->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
							mi->report(error(error::kind::KIND_ERROR, "Cannot take the address of a 'constexpr' symbol.",
								{ as->stream() }, 0));
							return;
						}
						shared_ptr<register_operand> loc = make_shared<register_operand>(
							*mi->lookup_variable_register(c_symbol_2_string(mi, at->identifier_symbol())));
						mi->add_insn(make_shared<asm_insn>(make_pair(full, loc)));
					}
						break;
					default:
						mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return;
						break;
					}
				}
					break;
				case asm_stmt::kind::KIND_RAW_STRING: {
					string full;
					for (token t : at->raw_stmt()) {
						if (t.raw_text().size() < 2)
							mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						full += t.raw_text().substr(1, t.raw_text().size() - 2);
					}
					mi->add_insn(make_shared<asm_insn>(full));
				}
					break;
				default:
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return;
					break;
				}
			}
		}

		void generate_access_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<access_stmt> as) {
			if (as == nullptr)
				mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			for (shared_ptr<symbol> s : as->declared_symbol_list()) {
				string sym = c_symbol_2_string(mi, s);
				shared_ptr<register_operand> reg = mi->generate_next_register(as->declared_type());
				mi->add_insn(make_shared<access_insn>(sym, reg));
				mi->map_variable_register(sym, reg);
				if (s->symbol_kind() != symbol::kind::KIND_VARIABLE)
					mi->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				shared_ptr<variable_symbol> vsym = static_pointer_cast<variable_symbol>(s);
				shared_ptr<variable_declaration> dummy = make_shared<variable_declaration>(vsym->variable_type(), vsym->variable_name(), true, as->stream());
				mi->add_variable(sym, dummy);
			}
		}

		main_function_kind main_function_defined(shared_ptr<function_symbol> fsym) {
			if (fsym->function_symbol_defined_kind() != function_stmt::defined_kind::KIND_DEFINITION)
				return main_function_kind::NONE;
			if (fsym->function_name().raw_text() != "main"
				|| fsym->parent_scope()->scope_kind() != scope::kind::KIND_GLOBAL)
				return main_function_kind::NONE;
			shared_ptr<function_type> ft = fsym->function_symbol_type();
			if (ft->type_static_kind() != type::static_kind::KIND_NON_STATIC
				|| ft->type_const_kind() != type::const_kind::KIND_NON_CONST
				|| ft->type_constexpr_kind() != type::constexpr_kind::KIND_NON_CONSTEXPR
				|| ft->variadic())
				return main_function_kind::NONE;
			if (ft->parameter_list().empty())
				return main_function_kind::MAIN_NO_ARGS;
			else if (ft->parameter_list().size() == 2
				|| ft->parameter_list().size() == 3) {
				shared_ptr<variable_declaration> arg1 = ft->parameter_list()[0],
					arg2 = ft->parameter_list()[1];
				shared_ptr<type> arg1_type = arg1->variable_declaration_type(),
					arg2_type = arg2->variable_declaration_type();

				// int argc
				if (arg1_type->type_kind() != type::kind::KIND_PRIMITIVE)
					return main_function_kind::NONE;
				shared_ptr<primitive_type> arg1_prim_type = static_pointer_cast<primitive_type>(arg1_type);
				if (arg1_prim_type->primitive_type_kind() != primitive_type::kind::KIND_INT
					|| arg1_prim_type->type_array_kind() != type::array_kind::KIND_NON_ARRAY
					|| arg1_prim_type->array_dimensions() != 0)
					return main_function_kind::NONE;

				// char** argv
				if (arg2_type->type_kind() != type::kind::KIND_PRIMITIVE)
					return main_function_kind::NONE;
				shared_ptr<primitive_type> arg2_prim_type = static_pointer_cast<primitive_type>(arg2_type);
				if (arg2_prim_type->primitive_type_kind() != primitive_type::kind::KIND_CHAR
					|| arg2_prim_type->type_array_kind() != type::array_kind::KIND_ARRAY
					|| arg2_prim_type->array_dimensions() != 2)
					return main_function_kind::NONE;
				
				if (ft->parameter_list().size() == 3) {
					shared_ptr<variable_declaration> arg3 = ft->parameter_list()[2];
					shared_ptr<type> arg3_type = arg3->variable_declaration_type();

					// char** envp
					if (arg3_type->type_kind() != type::kind::KIND_PRIMITIVE)
						return main_function_kind::NONE;
					shared_ptr<primitive_type> arg3_prim_type = static_pointer_cast<primitive_type>(arg3_type);
					if (arg3_prim_type->primitive_type_kind() != primitive_type::kind::KIND_CHAR
						|| arg3_prim_type->type_array_kind() != type::array_kind::KIND_ARRAY
						|| arg3_prim_type->array_dimensions() != 2)
						return main_function_kind::NONE;

					return main_function_kind::MAIN_ARGC_ARGV_ENVP;
				}
				return main_function_kind::MAIN_ARGC_ARGV;
			}
			return main_function_kind::NONE;
		}
	}
}
