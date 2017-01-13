#include "lir.hpp"
#include <iostream>

using std::make_shared;
using std::static_pointer_cast;
using std::to_string;

namespace spectre {
	namespace lir {

		insn::insn(insn::kind k) : _insn_kind(k) {

		}

		insn::~insn() {

		}

		insn::kind insn::insn_kind() {
			return _insn_kind;
		}

		operand::operand(kind ok) : _operand_kind(ok) {

		}

		operand::~operand() {

		}

		operand::kind operand::operand_kind() {
			return _operand_kind;
		}

		register_operand::register_operand(int n) : operand(operand::kind::KIND_REGISTER), _register_number(n) {

		}

		register_operand::~register_operand() {

		}

		int register_operand::register_number() {
			return _register_number;
		}

		string register_operand::raw_text() {
			return "r" + to_string(_register_number);
		}

		variable_operand::variable_operand(string vn) : operand(operand::kind::KIND_VARIABLE), _variable_name(vn) {

		}

		variable_operand::~variable_operand() {

		}

		string variable_operand::variable_name() {
			return _variable_name;
		}

		string variable_operand::raw_text() {
			return _variable_name;
		}

		immediate_operand::immediate_operand(bool b) : operand(operand::kind::KIND_IMMEDIATE), _immediate_operand_kind(immediate_operand::kind::KIND_BOOL),
			_immediate_bool(b) {

		}

		immediate_operand::immediate_operand(signed char b_c, bool is_char) : operand(operand::kind::KIND_IMMEDIATE),
			_immediate_operand_kind(is_char ? immediate_operand::kind::KIND_SIGNED_CHAR : immediate_operand::kind::KIND_SIGNED_BYTE) {
			if (is_char)
				_immediate_signed_char = b_c;
			else
				_immediate_signed_byte = b_c;
		}

		immediate_operand::immediate_operand(unsigned char b_c, bool is_char) : operand(operand::kind::KIND_IMMEDIATE),
			_immediate_operand_kind(is_char ? immediate_operand::kind::KIND_UNSIGNED_CHAR : immediate_operand::kind::KIND_UNSIGNED_BYTE) {
			if (is_char)
				_immediate_unsigned_char = b_c;
			else
				_immediate_unsigned_byte = b_c;
		}

		immediate_operand::immediate_operand(signed short s) : operand(operand::kind::KIND_IMMEDIATE), _immediate_operand_kind(immediate_operand::kind::KIND_SIGNED_SHORT),
			_immediate_signed_short(s) {

		}

		immediate_operand::immediate_operand(unsigned short s) : operand(operand::kind::KIND_IMMEDIATE), _immediate_operand_kind(immediate_operand::kind::KIND_UNSIGNED_SHORT),
			_immediate_unsigned_short(s) {

		}

		immediate_operand::immediate_operand(signed int i) : operand(operand::kind::KIND_IMMEDIATE), _immediate_operand_kind(immediate_operand::kind::KIND_SIGNED_INT),
			_immediate_signed_int(i) {

		}

		immediate_operand::immediate_operand(unsigned int i) : operand(operand::kind::KIND_IMMEDIATE), _immediate_operand_kind(immediate_operand::kind::KIND_UNSIGNED_INT),
			_immediate_unsigned_int(i) {

		}

		immediate_operand::immediate_operand(signed long l) : operand(operand::kind::KIND_IMMEDIATE), _immediate_operand_kind(immediate_operand::kind::KIND_SIGNED_LONG),
			_immediate_signed_long(l) {

		}

		immediate_operand::immediate_operand(unsigned long l) : operand(operand::kind::KIND_IMMEDIATE), _immediate_operand_kind(immediate_operand::kind::KIND_UNSIGNED_LONG),
			_immediate_unsigned_long(l) {

		}

		immediate_operand::immediate_operand(float f) : operand(operand::kind::KIND_IMMEDIATE), _immediate_operand_kind(immediate_operand::kind::KIND_FLOAT),
			_immediate_float(f) {

		}

		immediate_operand::immediate_operand(double d) : operand(operand::kind::KIND_IMMEDIATE), _immediate_operand_kind(immediate_operand::kind::KIND_DOUBLE),
			_immediate_double(d) {

		}

		immediate_operand::immediate_operand(string s) : operand(operand::kind::KIND_IMMEDIATE), _immediate_operand_kind(immediate_operand::kind::KIND_STRING),
			_immediate_string(s) {

		}

		immediate_operand::~immediate_operand() {

		}

		immediate_operand::kind immediate_operand::immediate_operand_kind() {
			return _immediate_operand_kind;
		}

		bool immediate_operand::immediate_bool() {
			return _immediate_bool;
		}

		signed char immediate_operand::immediate_signed_byte() {
			return _immediate_signed_byte;
		}

		unsigned char immediate_operand::immediate_unsigned_byte() {
			return _immediate_unsigned_byte;
		}

		signed char immediate_operand::immediate_signed_char() {
			return _immediate_signed_char;
		}

		unsigned char immediate_operand::immediate_unsigned_char() {
			return _immediate_unsigned_char;
		}

		signed short immediate_operand::immediate_signed_short() {
			return _immediate_signed_short;
		}

		unsigned short immediate_operand::immediate_unsigned_short() {
			return _immediate_unsigned_short;
		}

		signed int immediate_operand::immediate_signed_int() {
			return _immediate_signed_int;
		}

		unsigned int immediate_operand::immediate_unsigned_int() {
			return _immediate_unsigned_int;
		}

		signed long immediate_operand::immediate_signed_long() {
			return _immediate_signed_long;
		}

		unsigned long immediate_operand::immediate_unsigned_long() {
			return _immediate_unsigned_long;
		}

		float immediate_operand::immediate_float() {
			return _immediate_float;
		}

		double immediate_operand::immediate_double() {
			return _immediate_double;
		}

		string immediate_operand::immediate_string() {
			return _immediate_string;
		}

		string immediate_operand::raw_text() {
			switch (_immediate_operand_kind) {
			case immediate_operand::kind::KIND_BOOL:
				return string("[bool] ") + (_immediate_bool ? "true" : "false");
			case immediate_operand::kind::KIND_DOUBLE:
				return "[double] " + to_string(_immediate_double);
			case immediate_operand::kind::KIND_FLOAT:
				return "[float] " + to_string(_immediate_float);
			case immediate_operand::kind::KIND_SIGNED_BYTE:
				return "[signed byte] " + to_string(_immediate_signed_byte);
			case immediate_operand::kind::KIND_SIGNED_CHAR:
				return "[signed char] " + to_string(_immediate_signed_char);
			case immediate_operand::kind::KIND_SIGNED_INT:
				return "[signed int] " + to_string(_immediate_signed_int);
			case immediate_operand::kind::KIND_SIGNED_LONG:
				return "[signed long] " + to_string(_immediate_signed_long);
			case immediate_operand::kind::KIND_SIGNED_SHORT:
				return "[signed short] " + to_string(_immediate_signed_short);
			case immediate_operand::kind::KIND_UNSIGNED_BYTE:
				return "[unsigned byte] " + to_string(_immediate_unsigned_byte);
			case immediate_operand::kind::KIND_UNSIGNED_CHAR:
				return "[unsigned char] " + to_string(_immediate_unsigned_char);
			case immediate_operand::kind::KIND_UNSIGNED_INT:
				return "[unsigned int] " + to_string(_immediate_unsigned_int);
			case immediate_operand::kind::KIND_UNSIGNED_LONG:
				return "[unsigned long] " + to_string(_immediate_unsigned_long);
			case immediate_operand::kind::KIND_UNSIGNED_SHORT:
				return "[unsigned short] " + to_string(_immediate_unsigned_short);
			case immediate_operand::kind::KIND_STRING:
				return "[signed char []] " + _immediate_string;
			}
			return "";
		}

		array_insn::array_insn(shared_ptr<operand> s, vector<shared_ptr<operand>> ail) : insn(insn::kind::KIND_ARRAY), _store(s), _array_initializer_list(ail) {

		}

		array_insn::~array_insn() {

		}

		shared_ptr<operand> array_insn::store() {
			return _store;
		}

		vector<shared_ptr<operand>> array_insn::array_initializer_list() {
			return _array_initializer_list;
		}

		string array_insn::raw_text() {
			if (_array_initializer_list.size() == 0)
				return "";
			string ret = insns::_arr + " " + get_operand_text(_store) + ", ";
			for (int i = 0; i < _array_initializer_list.size(); i++)
				ret += get_operand_text(_array_initializer_list[i]) + (i == _array_initializer_list.size() - 1 ? "" : ", ");
			return ret;
		}

		regular_insn::regular_insn(regular_insn::operation_kind ok) : insn(insn::kind::KIND_REGULAR), _regular_insn_operation_kind(ok),
			_regular_insn_operation_number_kind(regular_insn::operation_number_kind::KIND_0), _first_operand(nullptr), _second_operand(nullptr), _third_operand(nullptr) {

		}

		regular_insn::regular_insn(regular_insn::operation_kind ok, shared_ptr<operand> f) : insn(insn::kind::KIND_REGULAR), _regular_insn_operation_kind(ok),
			_regular_insn_operation_number_kind(regular_insn::operation_number_kind::KIND_1), _first_operand(f), _second_operand(nullptr), _third_operand(nullptr) {
		}

		regular_insn::regular_insn(regular_insn::operation_kind ok, shared_ptr<operand> f, shared_ptr<operand> s) : insn(insn::kind::KIND_REGULAR), _regular_insn_operation_kind(ok),
			_regular_insn_operation_number_kind(regular_insn::operation_number_kind::KIND_2), _first_operand(f), _second_operand(s), _third_operand(nullptr) {

		}

		regular_insn::regular_insn(regular_insn::operation_kind ok, shared_ptr<operand> f, shared_ptr<operand> s, shared_ptr<operand> t) : insn(insn::kind::KIND_REGULAR),
			_regular_insn_operation_kind(ok), _regular_insn_operation_number_kind(regular_insn::operation_number_kind::KIND_3), _first_operand(f), _second_operand(s), _third_operand(t) {

		}
		shared_ptr<operand> regular_insn::first_operand() {
			return _first_operand;
		}

		shared_ptr<operand> regular_insn::second_operand() {
			return _second_operand;
		}

		shared_ptr<operand> regular_insn::third_operand() {
			return _third_operand;
		}

		regular_insn::operation_kind regular_insn::regular_insn_operation_kind() {
			return _regular_insn_operation_kind;
		}

		regular_insn::operation_number_kind regular_insn::regular_insn_operation_number_kind() {
			return _regular_insn_operation_number_kind;
		}

		string regular_insn::raw_text() {
			string op = operation_2_raw_instruction_map.at(_regular_insn_operation_kind);
			switch (_regular_insn_operation_number_kind) {
			case operation_number_kind::KIND_0:
				return op;
			case operation_number_kind::KIND_1:
				return op + " " + get_operand_text(_first_operand);
			case operation_number_kind::KIND_2:
				return op + " " + get_operand_text(_first_operand) + ", " + get_operand_text(_second_operand);
			case operation_number_kind::KIND_3:
				return op + " " + get_operand_text(_first_operand) + ", " + get_operand_text(_second_operand) + ", " + get_operand_text(_third_operand);
			}
			return "";
		}

		label_insn::label_insn(string ln) : insn(insn::kind::KIND_LABEL), _label_name(ln) {

		}

		label_insn::~label_insn() {

		}

		string label_insn::label_name() {
			return _label_name;
		}

		string label_insn::raw_text() {
			return _label_name + ":";
		}

		proc_header_insn::proc_header_insn(string rt, shared_ptr<variable_operand> fn, vector<shared_ptr<variable_operand>> fpl, vector<string> fptl) :
			insn(insn::kind::KIND_PROC_HEADER), _function_name(fn), _function_parameter_list(fpl), _function_parameter_type_list(fptl), _return_type(rt) {

		}

		proc_header_insn::~proc_header_insn() {

		}

		shared_ptr<variable_operand>  proc_header_insn::function_name() {
			return _function_name;
		}

		vector<shared_ptr<variable_operand>> proc_header_insn::function_parameter_list() {
			return _function_parameter_list;
		}

		vector<string> proc_header_insn::function_parameter_type_list() {
			return _function_parameter_type_list;
		}

		string proc_header_insn::return_type() {
			return _return_type;
		}

		string proc_header_insn::raw_text() {
			if (_function_parameter_list.size() != _function_parameter_type_list.size())
				return "";
			string ret = insns::_proc + " [" + _return_type + "] " + _function_name->raw_text() + ":";
			if (_function_parameter_list.size() == 0)
				return ret;
			else {
				ret += " ";
				for (int i = 0; i < _function_parameter_list.size(); i++) {
					ret += "[" + _function_parameter_type_list[i] + "] ";
					ret += _function_parameter_list[i]->variable_name();
					if (i != _function_parameter_list.size() - 1)
						ret += ",";
				}
				return ret;
			}
			return "";
		}
		
		proc_footer_insn::proc_footer_insn() : insn(insn::kind::KIND_PROC_FOOTER) {

		}

		proc_footer_insn::~proc_footer_insn() {

		}

		string proc_footer_insn::raw_text() {
			return insns::_eproc;
		}

		ret_insn::ret_insn(shared_ptr<operand> rv) : insn(insn::kind::KIND_RET), _return_value(rv) {

		}

		ret_insn::ret_insn() : insn(insn::kind::KIND_RET), _return_value(nullptr) {

		}

		ret_insn::~ret_insn() {

		}

		shared_ptr<operand> ret_insn::return_value() {
			return _return_value;
		}

		string ret_insn::raw_text() {
			return insns::_ret + (_return_value == nullptr ? "" : " " + get_operand_text(_return_value));
		}

		decl_insn::decl_insn(string dt, shared_ptr<variable_operand> dvo, shared_ptr<operand> rhs) : insn(insn::kind::KIND_DECL), _decl_type(dt), _decl_variable_operand(dvo), _rhs(rhs) {
			
		}

		decl_insn::decl_insn(string dt, shared_ptr<variable_operand> dvo) : insn(insn::kind::KIND_DECL), _decl_type(dt), _decl_variable_operand(dvo), _rhs(nullptr) {

		}

		decl_insn::~decl_insn() {

		}

		shared_ptr<variable_operand> decl_insn::decl_variable_operand() {
			return _decl_variable_operand;
		}

		shared_ptr<operand> decl_insn::rhs() {
			return _rhs;
		}

		string decl_insn::decl_type() {
			return _decl_type;
		}

		string decl_insn::raw_text() {
			return lir::insns::_mov + " [" + _decl_type + "] " + _decl_variable_operand->raw_text() + (_rhs == nullptr ? "" : ", " + get_operand_text(_rhs));
		}

		struct_header_insn::struct_header_insn(string sn) : insn(insn::kind::KIND_STRUCT_HEADER), _struct_name(sn) {

		}

		struct_header_insn::~struct_header_insn() {

		}

		string struct_header_insn::struct_name() {
			return _struct_name;
		}

		string struct_header_insn::raw_text() {
			return insns::_struc + " " + _struct_name;
		}

		struct_footer_insn::struct_footer_insn() : insn(insn::kind::KIND_STRUCT_FOOTER) {

		}

		struct_footer_insn::~struct_footer_insn() {

		}

		string struct_footer_insn::raw_text() {
			return insns::_estruc;
		}

		call_insn::call_insn(shared_ptr<operand> ro, shared_ptr<variable_operand> fo, vector<shared_ptr<operand>> aol) : insn(insn::kind::KIND_CALL),
			_result_operand(ro), _function_operand(fo), _argument_operand_list(aol) {

		}

		call_insn::call_insn(shared_ptr<variable_operand> fo, vector<shared_ptr<operand>> aol) : insn(insn::kind::KIND_CALL),
			_result_operand(nullptr), _function_operand(fo), _argument_operand_list(aol) {

		}

		call_insn::~call_insn() {

		}

		shared_ptr<variable_operand> call_insn::function_operand() {
			return _function_operand;
		}

		shared_ptr<operand> call_insn::result_operand() {
			return _result_operand;
		}

		vector<shared_ptr<operand>> call_insn::argument_operand_list() {
			return _argument_operand_list;
		}

		string call_insn::raw_text() {
			string ret;
			if (_result_operand != nullptr)
				ret = insns::_call + " " + get_operand_text(_result_operand) + ", " + get_operand_text(_function_operand);
			else
				ret = insns::_call + " " + get_operand_text(_function_operand);
			for (int i = 0; i < _argument_operand_list.size(); i++)
				ret += ", " + get_operand_text(_argument_operand_list[i]);
			return ret;
		}

		malloc_insn::malloc_insn(string mt, shared_ptr<operand> d, shared_ptr<operand> n) : insn(insn::kind::KIND_MALLOC), _malloc_type(mt), _destination(d), _number(n) {

		}

		malloc_insn::~malloc_insn() {

		}

		string malloc_insn::malloc_type() {
			return _malloc_type;
		}

		shared_ptr<operand> malloc_insn::destination() {
			return _destination;
		}

		shared_ptr<operand> malloc_insn::number() {
			return _number;
		}

		string malloc_insn::raw_text() {
			return insns::_malloc + " [" + _malloc_type + "] " + get_operand_text(_destination) + (_number != nullptr ? ", " + get_operand_text(_number) : "");
		}

		lir_code::lir_code(shared_ptr<spectre::parser::parser> p) : _ast_parser(p), _instruction_list(vector<shared_ptr<insn>>()),
			_post_increment_operand_list(vector<shared_ptr<operand>>()), _post_decrement_operand_list(vector<shared_ptr<operand>>()), _current_register_counter(0),
			 _current_internal_label_counter(0), _loop_start_stack(stack<shared_ptr<label_insn>>{}), _pre_decrement_operand_list(vector<shared_ptr<operand>>{}),
			_pre_increment_operand_list(vector<shared_ptr<operand>>{}), _loop_end_stack(stack<shared_ptr<label_insn>>{}),
			_pre_increment_decrement_flag_list(vector<int>{}) {

		}

		lir_code::~lir_code() {

		}

		void lir_code::add_instruction(shared_ptr<insn> i) {
			_instruction_list.push_back(i);
		}

		void lir_code::flag_pre_increment_decrement_position() {
			_pre_increment_decrement_flag_list.push_back(_instruction_list.size());
		}

		void lir_code::add_post_increment_operand(shared_ptr<operand> o) {
			_post_increment_operand_list.push_back(o);
		}

		void lir_code::add_post_decrement_operand(shared_ptr<operand> o) {
			_post_decrement_operand_list.push_back(o);
		}

		void lir_code::add_pre_increment_operand(shared_ptr<operand> o) {
			_pre_increment_operand_list.push_back(o);
		}

		void lir_code::add_pre_decrement_operand(shared_ptr<operand> o) {
			_pre_decrement_operand_list.push_back(o);
		}

		void lir_code::insert_pre_post_increment_decrement_operands() {
			vector<shared_ptr<insn>> post_inc_insns, post_dec_insns, pre_inc_insns, pre_dec_insns;
			for (shared_ptr<operand> o : _post_increment_operand_list)
				post_inc_insns.push_back(make_shared<regular_insn>(regular_insn::operation_kind::KIND_INC, o));
			for (shared_ptr<operand> o : _post_decrement_operand_list)
				post_dec_insns.push_back(make_shared<regular_insn>(regular_insn::operation_kind::KIND_DEC, o));
			for (shared_ptr<operand> o : _pre_increment_operand_list)
				pre_inc_insns.push_back(make_shared<regular_insn>(regular_insn::operation_kind::KIND_INC, o));
			for (shared_ptr<operand> o : _pre_decrement_operand_list)
				pre_dec_insns.push_back(make_shared<regular_insn>(regular_insn::operation_kind::KIND_DEC, o));
			_post_increment_operand_list.clear();
			_post_decrement_operand_list.clear();
			_pre_increment_operand_list.clear();
			_pre_decrement_operand_list.clear();
			vector<shared_ptr<insn>> post_total, pre_total;
			post_total.insert(post_total.end(), post_dec_insns.begin(), post_dec_insns.end());
			post_total.insert(post_total.end(), post_inc_insns.begin(), post_inc_insns.end());
			pre_total.insert(pre_total.end(), pre_inc_insns.begin(), pre_inc_insns.end());
			pre_total.insert(pre_total.end(), pre_dec_insns.begin(), pre_dec_insns.end());
			int pre_pos = _pre_increment_decrement_flag_list[_pre_increment_decrement_flag_list.size() - 1];
			_pre_increment_decrement_flag_list.pop_back();
			_instruction_list.insert(_instruction_list.end(), post_total.begin(), post_total.end());
			_instruction_list.insert(_instruction_list.begin() + pre_pos, pre_total.begin(), pre_total.end());
		}

		shared_ptr<register_operand> lir_code::generate_next_register_operand() {
			return make_shared<register_operand>(_current_register_counter++);
		}

		vector<shared_ptr<insn>> lir_code::instruction_list() {
			return _instruction_list;
		}

		vector<string> lir_code::raw_instruction_list() {
			vector<string> ret;
			for (shared_ptr<insn> i : _instruction_list)
				ret.push_back(get_insn_text(i));
			return ret;
		}

		void lir_code::report(error e) {
			_ast_parser->report(e);
		}

		void lir_code::report_internal(string msg, string fn, int ln, string fl) {
			_ast_parser->report_internal(msg, fn, ln, fl);
		}

		shared_ptr<label_insn> lir_code::generate_next_internal_label_insn() {
			return make_shared<label_insn>(".L" + to_string(_current_internal_label_counter++));
		}

		void lir_code::push_to_loop_start_stack(shared_ptr<label_insn> l) {
			_loop_start_stack.push(l);
		}

		void lir_code::push_to_loop_end_stack(shared_ptr<label_insn> l) {
			_loop_end_stack.push(l);
		}

		shared_ptr<label_insn> lir_code::loop_start_stack_top() {
			return _loop_start_stack.top();
		}

		shared_ptr<label_insn> lir_code::loop_end_stack_top() {
			return _loop_end_stack.top();
		}

		void lir_code::pop_loop_start_stack() {
			_loop_start_stack.pop();
		}

		void lir_code::pop_loop_end_stack() {
			_loop_end_stack.pop();
		}

		string get_operand_text(shared_ptr<operand> op) {
			if (op == nullptr) return "";
			switch (op->operand_kind()) {
			case operand::kind::KIND_IMMEDIATE:
				return static_pointer_cast<immediate_operand>(op)->raw_text();
			case operand::kind::KIND_REGISTER:
				return static_pointer_cast<register_operand>(op)->raw_text();
			case operand::kind::KIND_VARIABLE:
				return static_pointer_cast<variable_operand>(op)->raw_text();
			}
			return "";
		}

		string get_insn_text(shared_ptr<insn> i) {
			if (i == nullptr) return "";
			switch (i->insn_kind()) {
			case insn::kind::KIND_LABEL:
				return static_pointer_cast<label_insn>(i)->raw_text();
			case insn::kind::KIND_REGULAR:
				return static_pointer_cast<regular_insn>(i)->raw_text();
			case insn::kind::KIND_DECL:
				return static_pointer_cast<decl_insn>(i)->raw_text();
			case insn::kind::KIND_PROC_HEADER:
				return static_pointer_cast<proc_header_insn>(i)->raw_text();
			case insn::kind::KIND_PROC_FOOTER:
				return static_pointer_cast<proc_footer_insn>(i)->raw_text();
			case insn::kind::KIND_ARRAY:
				return static_pointer_cast<array_insn>(i)->raw_text();
			case insn::kind::KIND_RET:
				return static_pointer_cast<ret_insn>(i)->raw_text();
			case insn::kind::KIND_CALL:
				return static_pointer_cast<call_insn>(i)->raw_text();
			case insn::kind::KIND_STRUCT_HEADER:
				return static_pointer_cast<struct_header_insn>(i)->raw_text();
			case insn::kind::KIND_STRUCT_FOOTER:
				return static_pointer_cast<struct_footer_insn>(i)->raw_text();
			case insn::kind::KIND_MALLOC:
				return static_pointer_cast<malloc_insn>(i)->raw_text();
			}
			return "";
		}

		shared_ptr<operand> literal_token_2_operand(shared_ptr<lir_code> lc, token lit, shared_ptr<type> t) {
			if (lit.token_kind() == bad_token.token_kind())
				return nullptr;
			switch (lit.token_kind()) {
			case token::kind::TOKEN_TRUE:
			case token::kind::TOKEN_FALSE: {
				return make_shared<immediate_operand>(lit.token_kind() == token::kind::TOKEN_TRUE);
			}
				break;
			case token::kind::TOKEN_INTEGER:
			case token::kind::TOKEN_DECIMAL: {
				string str = lit.raw_text();
				int base = 10;
				switch (lit.prefix_kind()) {
				case token::prefix::PREFIX_BINARY:
					str = str.substr(2);
					base = 2;
					break;
				case token::prefix::PREFIX_HEXADECIMAL:
					str = str.substr(2);
					base = 16;
					break;
				case token::prefix::PREFIX_OCTAL:
					str = str.substr(2);
					base = 8;
					break;
				}
				switch (lit.suffix_kind()) {
				case token::suffix::SUFFIX_DOUBLE:
					str = str.substr(0, str.length() - 2);
					if (lit.token_kind() == token::kind::TOKEN_INTEGER)
						return make_shared<immediate_operand>((double)stoi(str, nullptr, base));
					else
						return make_shared<immediate_operand>(stod(str));
					break;
				case token::suffix::SUFFIX_FLOAT:
					str = str.substr(0, str.length() - 2);
					if (lit.token_kind() == token::kind::TOKEN_INTEGER)
						return make_shared<immediate_operand>((float)stoi(str, nullptr, base));
					else
						return make_shared<immediate_operand>(stof(str));
					break;
				case token::suffix::SUFFIX_NONE:
					if (lit.token_kind() == token::kind::TOKEN_INTEGER)
						return make_shared<immediate_operand>((signed int)stoi(str, nullptr, base));
					else
						return make_shared<immediate_operand>(stod(str));
					break;
				case token::suffix::SUFFIX_SIGNED_LONG:
					if (lit.token_kind() == token::kind::TOKEN_INTEGER)
						return make_shared<immediate_operand>((signed long)stol(str, nullptr, base));
					else
						return make_shared<immediate_operand>(stod(str));
					break;
				case token::suffix::SUFFIX_SIGNED_SHORT:
					if (lit.token_kind() == token::kind::TOKEN_INTEGER)
						return make_shared<immediate_operand>((signed short)stoi(str, nullptr, base));
					else
						return make_shared<immediate_operand>(stod(str));
					break;
				case token::suffix::SUFFIX_UNSIGNED_INT:
					if (lit.token_kind() == token::kind::TOKEN_INTEGER)
						return make_shared<immediate_operand>((unsigned int)stoul(str, nullptr, base));
					else
						return make_shared<immediate_operand>(stod(str));
					break;
				case token::suffix::SUFFIX_UNSIGNED_LONG:
					if (lit.token_kind() == token::kind::TOKEN_INTEGER)
						return make_shared<immediate_operand>(stoul(str, nullptr, base));
					else
						return make_shared<immediate_operand>(stod(str));
					break;
				case token::suffix::SUFFIX_UNSIGNED_SHORT:
					if (lit.token_kind() == token::kind::TOKEN_INTEGER)
						return make_shared<immediate_operand>((unsigned short)stoul(str, nullptr, base));
					else
						return make_shared<immediate_operand>(stod(str));
					break;
				}
			}
				break;
			case token::kind::TOKEN_STRING: {
				return make_shared<immediate_operand>(lit.raw_text());
			}
				break;
			}
			lc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		string type_2_string(shared_ptr<lir_code> lc, shared_ptr<type> t, bool spaces) {
			if (t == nullptr || !t->valid())
				return "";
			string sep = spaces ? " " : "_";
			string arr = "";
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
				return "struct" + sep + s->struct_name().raw_text() + "_" + to_string(s->struct_reference_number()) + arr;
			}
			lc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return "";
		}

		string scope_2_string_helper(shared_ptr<lir_code> lc, shared_ptr<scope> s, string r) {
			if (s == nullptr)
				return r;
			else if (s->parent_scope() == nullptr || s->scope_kind() == scope::kind::KIND_GLOBAL)
				return r + "global_";
			switch (s->scope_kind()) {
			case scope::kind::KIND_BLOCK:
				return scope_2_string_helper(lc, s->parent_scope(), r + "block" + to_string(static_pointer_cast<block_scope>(s)->block_number()) + "_");
				break;
			case scope::kind::KIND_LOOP:
				return scope_2_string_helper(lc, s->parent_scope(), r + "loop_");
				break;
			case scope::kind::KIND_SWITCH:
				return scope_2_string_helper(lc, s->parent_scope(), r + "switch_");
				break;
			case scope::kind::KIND_FUNCTION: {
				shared_ptr<function_scope> fs = static_pointer_cast<function_scope>(s);
				string c, st;
				if (fs->function_const_kind() == type::const_kind::KIND_CONST) c = "C";
				if (fs->function_static_kind() == type::static_kind::KIND_STATIC) st = "S";
				string rt = type_2_string(lc, fs->return_type(), false);
				return scope_2_string_helper(lc, s->parent_scope(), r + "function" + c + st + "_" + rt + "_" + fs->function_name().raw_text() + "_");
			}
				break;
			};
			lc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return "";
		}

		string scope_2_string(shared_ptr<lir_code> lc, shared_ptr<scope> s) {
			if (s == nullptr)
				return "";
			return "__" + scope_2_string_helper(lc, s, "");
		}

		string symbol_2_string_helper(shared_ptr<lir_code> lc, shared_ptr<symbol> s, string r) {
			string scope_string = scope_2_string(lc, s->parent_scope());
			if (s->symbol_kind() == symbol::kind::KIND_VARIABLE) {
				shared_ptr<type> t = static_pointer_cast<variable_symbol>(s)->variable_type();
				return scope_string + type_2_string(lc, t, false) + "_" + static_pointer_cast<variable_symbol>(s)->variable_name().raw_text();
			}
			if (s->symbol_kind() == symbol::kind::KIND_FUNCTION) {
				shared_ptr<function_symbol> fsym = static_pointer_cast<function_symbol>(s);
				shared_ptr<function_type> ft = fsym->function_symbol_type();
				string c, st;
				if (ft->type_const_kind() == type::const_kind::KIND_CONST) c = "C";
				if (ft->type_static_kind() == type::static_kind::KIND_STATIC) st = "S";
				return scope_string + "function" + c + st + "_" + type_2_string(lc, ft->return_type(), false) + "_" + fsym->function_name().raw_text();
			}
			if (s->symbol_kind() == symbol::kind::KIND_STRUCT) {
				shared_ptr<struct_symbol> ss = static_pointer_cast<struct_symbol>(s);
				return scope_string + "struct_" + ss->struct_name().raw_text() + "_" + to_string(ss->struct_symbol_type()->struct_reference_number());
			}
			lc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return "";
		}

		string symbol_2_string(shared_ptr<lir_code> lc, shared_ptr<symbol> s) {
			if (s == nullptr)
				return "";
			return symbol_2_string_helper(lc, s, "");
		}

		void generate_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<stmt> s) {
			if (s == nullptr || !s->valid())
				return;
			switch (s->stmt_kind()) {
			case stmt::kind::KIND_EXPRESSION: {
				lc->flag_pre_increment_decrement_position();
				generate_expression_lir(lc, s->stmt_expression(), false);
				lc->insert_pre_post_increment_decrement_operands();
				return;
			}
				break;
			case stmt::kind::KIND_VARIABLE_DECLARATION: {
				lc->flag_pre_increment_decrement_position();
				for (shared_ptr<variable_declaration> vd : s->stmt_variable_declaration_list())
					generate_variable_declaration_lir(lc, vd, true);
				lc->insert_pre_post_increment_decrement_operands();
				return;
			}
				break;
			case stmt::kind::KIND_IF: {
				generate_if_stmt_lir(lc, s->stmt_if());
				return;
			}
				break;
			case stmt::kind::KIND_FUNCTION: {
				generate_function_stmt_lir(lc, s->stmt_function());
				return;
			}
				break;
			case stmt::kind::KIND_RETURN: {
				lc->flag_pre_increment_decrement_position();
				generate_return_stmt_lir(lc, s->stmt_return());
				lc->insert_pre_post_increment_decrement_operands();
				return;
			}
				break;
			case stmt::kind::KIND_WHILE: {
				generate_while_stmt_lir(lc, s->stmt_while());
				return;
			}
				break;
			case stmt::kind::KIND_BLOCK: {
				generate_block_stmt_lir(lc, s->stmt_block());
				return;
			};
				break;
			case stmt::kind::KIND_BREAK_CONTINUE: {
				generate_break_continue_stmt_lir(lc, s->stmt_break_continue());
				return;
			};
				break;
			case stmt::kind::KIND_FOR: {
				generate_for_stmt_lir(lc, s->stmt_for());
				return;
			}
				break;
			case stmt::kind::KIND_DO_WHILE: {
				generate_do_while_stmt_lir(lc, s->stmt_while());
				return;
			}
				break;
			case stmt::kind::KIND_CASE_DEFAULT: {
				generate_case_default_stmt_lir(lc, s->stmt_case_default());
				return;
			}
				break;
			case stmt::kind::KIND_SWITCH: {
				generate_switch_stmt_lir(lc, s->stmt_switch());
				return;
			}
				break;
			case stmt::kind::KIND_STRUCT: {
				generate_struct_stmt_lir(lc, s->stmt_struct());
				return;
			}
				break;
			case stmt::kind::KIND_EMPTY:
				return;
				break;
			}
			lc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
		}

		shared_ptr<operand> generate_expression_lir(shared_ptr<lir_code> lc, shared_ptr<expression> e, bool reference) {
			if (e == nullptr || !e->valid())
				return nullptr;
			int index = 0;
			shared_ptr<operand> ret = nullptr;
			for (shared_ptr<assignment_expression> ae : e->assignment_expression_list()) {
				if (index == e->assignment_expression_list().size() - 1)
					ret = generate_assignment_expression_lir(lc, ae, reference);
				else
					ret = generate_assignment_expression_lir(lc, ae, false);
				index++;
			}
			return ret;
		}

		shared_ptr<operand> generate_assignment_expression_lir(shared_ptr<lir_code> lc, shared_ptr<assignment_expression> ae, bool reference) {
			if (ae == nullptr || !ae->valid())
				return nullptr;
			switch (ae->assignment_expression_kind()) {
			case assignment_expression::kind::KIND_ASSIGNMENT: {
				shared_ptr<operand> lhs_op = generate_unary_expression_lir(lc, ae->lhs_assignment(), true);
				if (ae->assignment_operator_kind() == binary_expression::operator_kind::KIND_LOGICAL_AND_EQUALS) {
					shared_ptr<immediate_operand> true_op = make_shared<immediate_operand>(true),
						false_op = make_shared<immediate_operand>(false);
					shared_ptr<label_insn> false_path = lc->generate_next_internal_label_insn(),
						done = lc->generate_next_internal_label_insn();
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JEQ, lhs_op, false_op, make_shared<variable_operand>(false_path->label_name())));
					shared_ptr<operand> rhs_op = generate_assignment_expression_lir(lc, ae->rhs_assignment(), reference);
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JEQ, rhs_op, false_op, make_shared<variable_operand>(false_path->label_name())));
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, lhs_op, true_op));
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JMP, make_shared<variable_operand>(done->label_name())));
					lc->add_instruction(false_path);
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, lhs_op, false_op));
					lc->add_instruction(done);
				}
				else if (ae->assignment_operator_kind() == binary_expression::operator_kind::KIND_LOGICAL_OR_EQUALS) {
					shared_ptr<immediate_operand> true_op = make_shared<immediate_operand>(true),
						false_op = make_shared<immediate_operand>(false);
					shared_ptr<label_insn> true_path = lc->generate_next_internal_label_insn(),
						done = lc->generate_next_internal_label_insn();
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JEQ, lhs_op, true_op, make_shared<variable_operand>(true_path->label_name())));
					shared_ptr<operand> rhs_op = generate_assignment_expression_lir(lc, ae->rhs_assignment(), reference);
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JEQ, rhs_op, true_op, make_shared<variable_operand>(true_path->label_name())));
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, lhs_op, false_op));
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JMP, make_shared<variable_operand>(done->label_name())));
					lc->add_instruction(true_path);
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, lhs_op, true_op));
					lc->add_instruction(done);
				}
				else {
					shared_ptr<operand> rhs_op = generate_assignment_expression_lir(lc, ae->rhs_assignment(), reference);
					lc->add_instruction(make_shared<regular_insn>(binary_operator_2_operation_map.at(ae->assignment_operator_kind()), lhs_op, rhs_op));
				}
				if (!reference) {
					shared_ptr<operand> op = lc->generate_next_register_operand();
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, op, lhs_op));
					return op;
				}
				else
					return lhs_op;
			}
				break;
			case assignment_expression::kind::KIND_TERNARY: {
				shared_ptr<operand> res = generate_ternary_expression_lir(lc, ae->conditional_expression(), reference);
				return res;
			}
				break;
			}
			lc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand> generate_ternary_expression_lir(shared_ptr<lir_code> lc, shared_ptr<ternary_expression> te, bool reference) {
			if (te == nullptr || !te->valid())
				return nullptr;
			switch (te->ternary_expression_kind()) {
			case ternary_expression::kind::KIND_TERNARY: {
				shared_ptr<binary_expression> cond = te->condition();
				shared_ptr<expression> tp = te->true_path();

				shared_ptr<ternary_expression> fp = te->false_path();
				shared_ptr<label_insn> fp_label = lc->generate_next_internal_label_insn(),
					done_label = lc->generate_next_internal_label_insn();
				shared_ptr<operand> res_reg = lc->generate_next_register_operand();
				regular_insn::operation_kind correct_mov = reference ? regular_insn::operation_kind::KIND_DMOV : regular_insn::operation_kind::KIND_MOV;

				shared_ptr<operand> cond_res = generate_binary_expression_lir(lc, cond, false);
				shared_ptr<immediate_operand> false_lit = make_shared<immediate_operand>(false);
				lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JEQ, cond_res, false_lit, make_shared<variable_operand>(fp_label->label_name())));

				shared_ptr<operand> tp_res = generate_expression_lir(lc, tp, reference);
				lc->add_instruction(make_shared<regular_insn>(correct_mov, res_reg, tp_res));
				lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JMP, make_shared<variable_operand>(done_label->label_name())));

				lc->add_instruction(fp_label);

				shared_ptr<operand> fp_res = generate_ternary_expression_lir(lc, fp, reference);
				lc->add_instruction(make_shared<regular_insn>(correct_mov, res_reg, fp_res));

				lc->add_instruction(done_label);
				return res_reg;
			}
				break;
			case ternary_expression::kind::KIND_BINARY: {
				return generate_binary_expression_lir(lc, te->condition(), reference);
			}
				break;
			}
			lc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand> generate_binary_expression_lir(shared_ptr<lir_code> lc, shared_ptr<binary_expression> be, bool reference) {
			if (be == nullptr || !be->valid())
				return nullptr;
			switch (be->binary_expression_kind()) {
			case binary_expression::kind::KIND_BINARY_EXPRESSION: {
				if (be->binary_expression_operator_kind() == binary_expression::operator_kind::KIND_LOGICAL_OR) {
					shared_ptr<operand> lhs_base = generate_binary_expression_lir(lc, be->lhs(), reference);
					shared_ptr<operand> lhs_op = lhs_base;
					if (lhs_op->operand_kind() == operand::kind::KIND_VARIABLE) {
						lhs_op = lc->generate_next_register_operand();
						lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, lhs_op, lhs_base));
					}
					shared_ptr<immediate_operand> true_op = make_shared<immediate_operand>(true),
						false_op = make_shared<immediate_operand>(false);
					shared_ptr<label_insn> true_path = lc->generate_next_internal_label_insn(), done = lc->generate_next_internal_label_insn();
					shared_ptr<register_operand> res = lc->generate_next_register_operand();
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JEQ, lhs_op, true_op, make_shared<variable_operand>(true_path->label_name())));
					shared_ptr<operand> rhs_base = generate_binary_expression_lir(lc, be->rhs(), reference);
					shared_ptr<operand> rhs_op = rhs_base;
					if (rhs_op->operand_kind() == operand::kind::KIND_VARIABLE) {
						rhs_op = lc->generate_next_register_operand();
						lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, rhs_op, rhs_base));
					}
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JEQ, rhs_op, true_op, make_shared<variable_operand>(true_path->label_name())));
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, res, false_op));
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JMP, make_shared<variable_operand>(done->label_name())));
					lc->add_instruction(true_path);
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, res, true_op));
					lc->add_instruction(done);
					return res;
				}
				else if (be->binary_expression_operator_kind() == binary_expression::operator_kind::KIND_LOGICAL_AND) {
					shared_ptr<operand> lhs_base = generate_binary_expression_lir(lc, be->lhs(), reference);
					shared_ptr<operand> lhs_op = lhs_base;
					if (lhs_op->operand_kind() == operand::kind::KIND_VARIABLE) {
						lhs_op = lc->generate_next_register_operand();
						lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, lhs_op, lhs_base));
					}
					shared_ptr<immediate_operand> true_op = make_shared<immediate_operand>(true),
						false_op = make_shared<immediate_operand>(false);
					shared_ptr<label_insn> false_path = lc->generate_next_internal_label_insn(), done = lc->generate_next_internal_label_insn();
					shared_ptr<register_operand> res = lc->generate_next_register_operand();
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JNEQ, lhs_op, true_op, make_shared<variable_operand>(false_path->label_name())));
					shared_ptr<operand> rhs_base = generate_binary_expression_lir(lc, be->rhs(), reference);
					shared_ptr<operand> rhs_op = rhs_base;
					if (rhs_op->operand_kind() == operand::kind::KIND_VARIABLE) {
						rhs_op = lc->generate_next_register_operand();
						lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, rhs_op, rhs_base));
					}
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JNEQ, rhs_op, true_op, make_shared<variable_operand>(false_path->label_name())));
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, res, true_op));
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JMP, make_shared<variable_operand>(done->label_name())));
					lc->add_instruction(false_path);
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, res, false_op));
					lc->add_instruction(done);
					return res;
				}
				else {
					shared_ptr<operand> lhs_base = generate_binary_expression_lir(lc, be->lhs(), reference);
					shared_ptr<operand> lhs_op = lhs_base;
					if (lhs_op->operand_kind() == operand::kind::KIND_VARIABLE) {
						lhs_op = lc->generate_next_register_operand();
						lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, lhs_op, lhs_base));
					}
					shared_ptr<operand> rhs_base = generate_binary_expression_lir(lc, be->rhs(), reference);
					shared_ptr<operand> rhs_op = rhs_base;
					if (rhs_op->operand_kind() == operand::kind::KIND_VARIABLE) {
						rhs_op = lc->generate_next_register_operand();
						lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, rhs_op, rhs_base));
					}
					lc->add_instruction(make_shared<regular_insn>(binary_operator_2_operation_map.at(be->binary_expression_operator_kind()), lhs_op, rhs_op));
					return lhs_op;
				}
			}
				break;
			case binary_expression::kind::KIND_UNARY_EXPRESSION: {
				return generate_unary_expression_lir(lc, be->single_lhs(), reference);
			}
				break;
			}
			lc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand> generate_unary_expression_lir(shared_ptr<lir_code> lc, shared_ptr<unary_expression> ue, bool reference) {
			if (ue == nullptr || !ue->valid())
				return nullptr;
			shared_ptr<operand> correct_pe = generate_postfix_expression_lir(lc, ue->contained_postfix_expression(), true);
			for (unary_expression::kind uek : ue->unary_expression_kind_list()) {
				switch (uek) {
				case unary_expression::kind::KIND_BITWISE_NOT:
				case unary_expression::kind::KIND_LOGICAL_NOT:
				case unary_expression::kind::KIND_MINUS:
				case unary_expression::kind::KIND_PLUS: {
					shared_ptr<register_operand> res = lc->generate_next_register_operand();
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, res, correct_pe));
					lc->add_instruction(make_shared<regular_insn>(unary_operator_2_operation_map.at(uek), res));
					correct_pe = res;
				}
					break;
				case unary_expression::kind::KIND_DECREMENT:
				case unary_expression::kind::KIND_INCREMENT: {
					if (uek == unary_expression::kind::KIND_INCREMENT)
						lc->add_pre_increment_operand(correct_pe);
					else
						lc->add_pre_decrement_operand(correct_pe);
				}
					break;
				}
			}
			if (!reference) {
				if (ue->unary_expression_kind_list().size() == 0 && ue->contained_postfix_expression()->postfix_type_list().size() == 0)
					return correct_pe;
				shared_ptr<operand> ret = lc->generate_next_register_operand();
				lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, ret, correct_pe));
				return ret;
			}
			return correct_pe;
		}

		shared_ptr<operand> generate_postfix_expression_lir(shared_ptr<lir_code> lc, shared_ptr<postfix_expression> pe, bool reference) {
			if (pe == nullptr || !pe->valid())
				return nullptr;
			shared_ptr<operand> ref_op = generate_primary_expression_lir(lc, pe->contained_primary_expression(), true);
			for (shared_ptr<postfix_expression::postfix_type> pt : pe->postfix_type_list()) {
				switch (pt->postfix_type_kind()) {
				case postfix_expression::kind::KIND_DECREMENT: {
					lc->add_post_decrement_operand(ref_op);
				}
					break;
				case postfix_expression::kind::KIND_INCREMENT: {
					lc->add_post_increment_operand(ref_op);
				}
					break;
				case postfix_expression::kind::KIND_FUNCTION_CALL: {
					shared_ptr<function_symbol> fsym = static_pointer_cast<function_symbol>(pt->function());
					shared_ptr<variable_operand> fo = make_shared<variable_operand>(symbol_2_string(lc, fsym));
					vector<shared_ptr<operand>> arg_list;
					for (shared_ptr<assignment_expression> ae : pt->argument_list())
						arg_list.push_back(generate_assignment_expression_lir(lc, ae, false));
					shared_ptr<register_operand> res = lc->generate_next_register_operand();
					if (fsym->function_symbol_type()->return_type()->type_kind() == type::kind::KIND_PRIMITIVE &&
						static_pointer_cast<primitive_type>(fsym->function_symbol_type()->return_type())->primitive_type_kind() == primitive_type::kind::KIND_VOID) {
						lc->add_instruction(make_shared<call_insn>(fo, arg_list));
						lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, res));
					}
					else
						lc->add_instruction(make_shared<call_insn>(res, fo, arg_list));
					ref_op = res;
				}
					break;
				case postfix_expression::kind::KIND_SUBSCRIPT: {
					shared_ptr<operand> res = lc->generate_next_register_operand(),
						s = generate_expression_lir(lc, pt->subscript(), false);
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_SUBSCR, res, ref_op, s));
					ref_op = res;
				}
					break;
				case postfix_expression::kind::KIND_MEMBER: {
					string ref_text = get_operand_text(ref_op);
					ref_op = make_shared<variable_operand>(ref_text + "@" + pt->member().raw_text());
				}
					break;
				}
			}
			if (!reference) {
				if (pe->postfix_type_list().size() == 0 && pe->contained_primary_expression()->primary_expression_kind() != primary_expression::kind::KIND_IDENTIFIER)
					return ref_op;
				shared_ptr<operand> ret = lc->generate_next_register_operand();
				lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, ret, ref_op));
				return ret;
			}
			return ref_op;
		}

		shared_ptr<operand> generate_primary_expression_lir(shared_ptr<lir_code> lc, shared_ptr<primary_expression> pe, bool reference) {
			if (pe == nullptr || !pe->valid())
				return nullptr;
			switch (pe->primary_expression_kind()) {
			case primary_expression::kind::KIND_PARENTHESIZED_EXPRESSION: {
				return generate_expression_lir(lc, pe->parenthesized_expression(), reference);
			}
				break;
			case primary_expression::kind::KIND_LITERAL: {
				shared_ptr<operand> reg = lc->generate_next_register_operand();
				shared_ptr<operand> lit = literal_token_2_operand(lc, pe->literal_token(), pe->primary_expression_type());
				lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, reg, lit));
				return reg;
			}
				break;
			case primary_expression::kind::KIND_IDENTIFIER: {
				shared_ptr<variable_operand> vo = make_shared<variable_operand>(symbol_2_string(lc, pe->identifier_symbol()));
				if (!reference) {
					shared_ptr<register_operand> reg = lc->generate_next_register_operand();
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_MOV, reg, vo));
					return reg;
				}
				else
					return vo;
			}
				break;
			case primary_expression::kind::KIND_ARRAY_INITIALIZER: {
				vector<shared_ptr<assignment_expression>> ai = pe->array_initializer();
				shared_ptr<operand> op = lc->generate_next_register_operand();
				vector<shared_ptr<operand>> ail;
				for (shared_ptr<assignment_expression> e : ai)
					ail.push_back(generate_assignment_expression_lir(lc, e, false));
				lc->add_instruction(make_shared<array_insn>(op, ail));
				return op;
			}
				break;
			case primary_expression::kind::KIND_NEW: {
				shared_ptr<type> nt = pe->new_type();
				string nts = type_2_string(lc, nt, true);
				shared_ptr<operand> op = lc->generate_next_register_operand();
				shared_ptr<operand> n = generate_expression_lir(lc, pe->parenthesized_expression(), false);
				lc->add_instruction(make_shared<malloc_insn>(nts, op, n));
				return op;
			}
				break;
			}
			lc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<operand> generate_variable_declaration_lir(shared_ptr<lir_code> lc, shared_ptr<variable_declaration> vd, bool qualify) {
			if (vd == nullptr || !vd->valid() || !vd->symbol_set())
				return nullptr;
			shared_ptr<variable_symbol> vs = vd->variable_declaration_symbol();
			string name = qualify ? symbol_2_string(lc, vs) : vs->variable_name().raw_text();
			string type = type_2_string(lc, vs->variable_type(), true);
			shared_ptr<variable_operand> lhs = make_shared<variable_operand>(name);
			if (vd->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_NOT_PRESENT) {
				lc->add_instruction(make_shared<decl_insn>(type, lhs));
				return lhs;
			}
			else if (vd->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_PRESENT) {
				shared_ptr<operand> rhs = generate_assignment_expression_lir(lc, vd->initialization(), false);
				lc->add_instruction(make_shared<decl_insn>(type, lhs, rhs));
				return lhs;
			}
			lc->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		void generate_if_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<if_stmt> is) {
			if (is == nullptr || !is->valid())
				return;
			bool else_present = is->if_stmt_else_kind() == if_stmt::else_kind::KIND_ELSE_PRESENT;
			if (is->if_stmt_init_decl_kind() == if_stmt::init_decl_kind::KIND_INIT_DECL_PRESENT) {
				lc->flag_pre_increment_decrement_position();
				for (shared_ptr<variable_declaration> vd : is->init_decl_list())
					generate_variable_declaration_lir(lc, vd, true);
				lc->insert_pre_post_increment_decrement_operands();
			}
			shared_ptr<expression> cond = is->condition();
			lc->flag_pre_increment_decrement_position();
			shared_ptr<operand> res = generate_expression_lir(lc, cond, false);
			lc->insert_pre_post_increment_decrement_operands();
			shared_ptr<immediate_operand> false_op = make_shared<immediate_operand>(false);
			shared_ptr<label_insn> fp_label = lc->generate_next_internal_label_insn(), done_label = nullptr;
			if (else_present)
				done_label = lc->generate_next_internal_label_insn();
			lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JEQ, res, false_op, make_shared<variable_operand>(fp_label->label_name())));
			shared_ptr<stmt> tp = is->true_path();
			generate_stmt_lir(lc, tp);
			if (else_present)
				lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JMP, make_shared<variable_operand>(done_label->label_name())));
			lc->add_instruction(fp_label);
			if (!else_present)
				return;
			shared_ptr<stmt> fp = is->false_path();
			generate_stmt_lir(lc, fp);
			lc->add_instruction(done_label);
		}

		void generate_function_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<function_stmt> fs) {
			if (fs == nullptr || !fs->valid())
				return;
			if(fs->function_stmt_symbol()->function_symbol_defined_kind() == function_stmt::defined_kind::KIND_DECLARATION) {
				lc->report(error(error::kind::KIND_ERROR, "Expected a function to be defined at some point during the program.", fs->stream(), 0));
				return;
			}
			if (fs->function_stmt_defined_kind() == function_stmt::defined_kind::KIND_DECLARATION)
				return;
			vector<shared_ptr<variable_operand>> fpl;
			vector<string> fptl;
			shared_ptr<function_type> ft = fs->function_stmt_type();
			vector<shared_ptr<variable_declaration>> pl = ft->parameter_list();
			for (int i = 0; i < pl.size(); i++) {
				string psym = symbol_2_string(lc, pl[i]->variable_declaration_symbol());
				string pt = type_2_string(lc, pl[i]->variable_declaration_type(), true);
				fpl.push_back(make_shared<variable_operand>(psym));
				fptl.push_back(pt);
			}
			string fsym = symbol_2_string(lc, fs->function_stmt_symbol());
			lc->add_instruction(make_shared<proc_header_insn>(type_2_string(lc, ft->return_type(), true), make_shared<variable_operand>(fsym), fpl, fptl));
			for (shared_ptr<stmt> s : fs->function_body())
				generate_stmt_lir(lc, s);
			lc->add_instruction(make_shared<proc_footer_insn>());
			return;
		}

		void generate_return_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<return_stmt> rstmt) {
			if (rstmt == nullptr || !rstmt->valid())
				return;
			if (rstmt->return_stmt_value_kind() == return_stmt::value_kind::KIND_EMPTY)
				lc->add_instruction(make_shared<ret_insn>());
			else {
				lc->flag_pre_increment_decrement_position();
				shared_ptr<operand> ret = generate_expression_lir(lc, rstmt->return_value(), false);
				lc->insert_pre_post_increment_decrement_operands();
				if (rstmt->return_value()->expression_type()->type_kind() == type::kind::KIND_PRIMITIVE &&
					static_pointer_cast<primitive_type>(rstmt->return_value()->expression_type())->primitive_type_kind() == primitive_type::kind::KIND_VOID)
					lc->add_instruction(make_shared<ret_insn>());
				else
					lc->add_instruction(make_shared<ret_insn>(ret));
			}
		}

		void generate_while_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<while_stmt> wstmt) {
			if (wstmt == nullptr || !wstmt->valid() || wstmt->while_stmt_kind() != while_stmt::while_kind::KIND_WHILE)
				return;
			shared_ptr<label_insn> start_label = lc->generate_next_internal_label_insn(),
				end_label = lc->generate_next_internal_label_insn();
			shared_ptr<immediate_operand> true_op = make_shared<immediate_operand>(true);
			lc->push_to_loop_start_stack(start_label);
			lc->push_to_loop_end_stack(end_label);
			lc->add_instruction(start_label);
			lc->flag_pre_increment_decrement_position();
			shared_ptr<operand> cond = generate_expression_lir(lc, wstmt->condition(), false);
			lc->insert_pre_post_increment_decrement_operands();
			lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JNEQ, cond, true_op,
				make_shared<variable_operand>(end_label->label_name())));
			generate_stmt_lir(lc, wstmt->while_body());
			lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JMP, make_shared<variable_operand>(start_label->label_name())));
			lc->add_instruction(end_label);
			lc->pop_loop_start_stack();
			lc->pop_loop_end_stack();
		}

		void generate_block_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<block_stmt> bstmt) {
			if (bstmt == nullptr || !bstmt->valid())
				return;
			for (shared_ptr<stmt> s : bstmt->stmt_list())
				generate_stmt_lir(lc, s);
		}

		void generate_break_continue_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<break_continue_stmt> bcstmt) {
			if (bcstmt == nullptr || !bcstmt->valid() || bcstmt->break_continue_stmt_kind() == break_continue_stmt::kind::KIND_NONE)
				return;
			if (bcstmt->break_continue_stmt_kind() == break_continue_stmt::kind::KIND_BREAK)
				lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JMP, make_shared<variable_operand>(
					lc->loop_end_stack_top()->label_name())));
			else
				lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JMP, make_shared<variable_operand>(
					lc->loop_start_stack_top()->label_name())));
		}

		void generate_for_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<for_stmt> fstmt) {
			if (fstmt == nullptr || !fstmt->valid())
				return;
			if (fstmt->for_stmt_initializer_present_kind() == for_stmt::initializer_present_kind::INITIALIZER_PRESENT) {
				lc->flag_pre_increment_decrement_position();
				for (shared_ptr<variable_declaration> vd : fstmt->initializer())
					generate_variable_declaration_lir(lc, vd, true);
				lc->insert_pre_post_increment_decrement_operands();
			}
			shared_ptr<label_insn> start_loop = lc->generate_next_internal_label_insn(),
				end_loop = lc->generate_next_internal_label_insn();
			lc->add_instruction(start_loop);
			lc->push_to_loop_start_stack(start_loop);
			lc->push_to_loop_end_stack(end_loop);
			if (fstmt->for_stmt_condition_present_kind() == for_stmt::condition_present_kind::CONDITION_PRESENT) {
				lc->flag_pre_increment_decrement_position();
				shared_ptr<operand> cond_res = generate_expression_lir(lc, fstmt->condition(), false);
				lc->insert_pre_post_increment_decrement_operands();
				shared_ptr<immediate_operand> true_op = make_shared<immediate_operand>(true);
				lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JNEQ, cond_res, true_op,
					make_shared<variable_operand>(end_loop->label_name())));
			}
			generate_stmt_lir(lc, fstmt->for_stmt_body());
			if (fstmt->for_stmt_update_present_kind() == for_stmt::update_present_kind::UPDATE_PRESENT) {
				lc->flag_pre_increment_decrement_position();
				generate_expression_lir(lc, fstmt->update(), false);
				lc->insert_pre_post_increment_decrement_operands();
			}
			lc->pop_loop_start_stack();
			lc->pop_loop_end_stack();
			lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JMP, make_shared<variable_operand>(start_loop->label_name())));
			lc->add_instruction(end_loop);
		}

		void generate_do_while_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<while_stmt> wstmt) {
			if (wstmt == nullptr || !wstmt->valid() || wstmt->while_stmt_kind() != while_stmt::while_kind::KIND_DO_WHILE)
				return;
			shared_ptr<label_insn> start_loop = lc->generate_next_internal_label_insn(),
				end_loop = lc->generate_next_internal_label_insn();
			lc->push_to_loop_start_stack(start_loop);
			lc->push_to_loop_end_stack(end_loop);
			lc->add_instruction(start_loop);
			generate_stmt_lir(lc, wstmt->while_body());
			lc->flag_pre_increment_decrement_position();
			shared_ptr<operand> op = generate_expression_lir(lc, wstmt->condition(), false);
			lc->insert_pre_post_increment_decrement_operands();
			shared_ptr<immediate_operand> true_op = make_shared<immediate_operand>(true);
			lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JEQ, op, true_op, make_shared<variable_operand>(start_loop->label_name())));
			lc->add_instruction(end_loop);
			lc->pop_loop_start_stack();
			lc->pop_loop_end_stack();
		}

		void generate_case_default_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<case_default_stmt> cdstmt) {
			if (cdstmt == nullptr || !cdstmt->valid())
				return;
			int lnumber = cdstmt->case_default_stmt_label_number();
			lc->add_instruction(make_shared<label_insn>(".C" + to_string(lnumber)));
			generate_stmt_lir(lc, cdstmt->case_default_stmt_body());
		}

		void generate_switch_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<switch_stmt> sstmt) {
			if (sstmt == nullptr || !sstmt->valid())
				return;
			shared_ptr<label_insn> end_switch = lc->generate_next_internal_label_insn();
			shared_ptr<expression> parent = sstmt->parent_expression();
			lc->flag_pre_increment_decrement_position();
			shared_ptr<operand> parent_res = generate_expression_lir(lc, parent, false);
			lc->insert_pre_post_increment_decrement_operands();
			shared_ptr<switch_scope> s_scope = sstmt->switch_stmt_scope();
			shared_ptr<stmt> s_body = sstmt->switch_stmt_body();
			for (shared_ptr<case_default_stmt> cd : s_scope->case_default_stmt_list()) {
				shared_ptr<label_insn> label = make_shared<label_insn>(".C" + to_string(cd->case_default_stmt_label_number()));
				if (cd->case_default_stmt_kind() == case_default_stmt::kind::KIND_DEFAULT)
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JMP, make_shared<variable_operand>(label->label_name())));
				else {
					lc->flag_pre_increment_decrement_position();
					shared_ptr<operand> op = generate_expression_lir(lc, cd->case_expression(), false);
					lc->insert_pre_post_increment_decrement_operands();
					lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JEQ, parent_res, op, make_shared<variable_operand>(label->label_name())));
				}
			}
			lc->add_instruction(make_shared<regular_insn>(regular_insn::operation_kind::KIND_JMP, make_shared<variable_operand>(end_switch->label_name())));
			lc->push_to_loop_end_stack(end_switch);
			generate_stmt_lir(lc, s_body);
			lc->pop_loop_end_stack();
			lc->add_instruction(end_switch);
		}

		void generate_struct_stmt_lir(shared_ptr<lir_code> lc, shared_ptr<struct_stmt> sstmt) {
			if (sstmt == nullptr || !sstmt->valid())
				return;
			shared_ptr<struct_symbol> ssymbol = static_pointer_cast<struct_symbol>(sstmt->struct_stmt_symbol());
			if (ssymbol->struct_symbol_defined_kind() != struct_stmt::defined_kind::KIND_DEFINED) {
				lc->report(error(error::kind::KIND_ERROR, "This 'struct' is declared but never defined.", ssymbol->stream(), 0));
				return;
			}
			if (sstmt->struct_stmt_defined_kind() != struct_stmt::defined_kind::KIND_DEFINED)
				return;
			token struct_name = sstmt->struct_name();
			string type_string = type_2_string(lc, sstmt->struct_stmt_type(), false);
			lc->add_instruction(make_shared<struct_header_insn>(type_string));
			vector<shared_ptr<variable_declaration>> vdl = sstmt->struct_variable_declaration_list();
			for (shared_ptr<variable_declaration> vd : vdl)
				generate_variable_declaration_lir(lc, vd, false);
			lc->add_instruction(make_shared<struct_footer_insn>());
		}
	}
}