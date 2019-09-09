#include "parser/stmt.hpp"

namespace spectre {
	namespace ast {

		variable_declaration::variable_declaration(shared_ptr<type> vdt, token vdn, shared_ptr<assignment_expression> i, bool v, vector<token> s) :
			_variable_declaration_type(vdt), _variable_declaration_name(vdn), _initialization(i), _stream(s),
			_variable_declaration_initialization_kind(variable_declaration::initialization_kind::KIND_PRESENT), _valid(v), _symbol_set(false) {

		}

		variable_declaration::variable_declaration(shared_ptr<type> vdt, token vdn, bool v, vector<token> s) :
			_variable_declaration_type(vdt), _variable_declaration_name(vdn), _initialization(nullptr), _stream(s),
			_variable_declaration_initialization_kind(variable_declaration::initialization_kind::KIND_NOT_PRESENT), _valid(v), _symbol_set(false) {

		}

		variable_declaration::~variable_declaration() {

		}

		shared_ptr<type> variable_declaration::variable_declaration_type() {
			return _variable_declaration_type;
		}

		token variable_declaration::variable_declaration_name() {
			return _variable_declaration_name;
		}

		vector<token> variable_declaration::stream() {
			return _stream;
		}

		shared_ptr<assignment_expression> variable_declaration::initialization() {
			return _initialization;
		}

		variable_declaration::initialization_kind variable_declaration::variable_declaration_initialization_kind() {
			return _variable_declaration_initialization_kind;
		}

		bool variable_declaration::valid() {
			return _valid;
		}

		bool variable_declaration::symbol_set() {
			return _symbol_set;
		}

		void variable_declaration::set_variable_declaration_symbol(shared_ptr<variable_symbol> vds) {
			_symbol_set = true;
			_variable_declaration_symbol = vds;
		}

		shared_ptr<variable_symbol> variable_declaration::variable_declaration_symbol() {
			return _variable_declaration_symbol;
		}

		if_stmt::if_stmt(shared_ptr<scope> is, vector<shared_ptr<variable_declaration>> idl, shared_ptr<expression> c, shared_ptr<stmt> tp, shared_ptr<stmt> fp, bool v,
			vector<token> s) :
			_if_scope(is), _init_decl_list(idl), _condition(c), _true_path(tp), _false_path(fp), _valid(v), _stream(s),
			_if_stmt_else_kind(if_stmt::else_kind::KIND_ELSE_PRESENT), _if_stmt_init_decl_kind(if_stmt::init_decl_kind::KIND_INIT_DECL_PRESENT) {

		}

		if_stmt::if_stmt(shared_ptr<scope> is, vector<shared_ptr<variable_declaration>> idl, shared_ptr<expression> c, shared_ptr<stmt> tp, bool v, vector<token> s) :
			_if_scope(is), _init_decl_list(idl), _condition(c), _true_path(tp), _valid(v), _stream(s), _false_path(nullptr),
			_if_stmt_else_kind(if_stmt::else_kind::KIND_ELSE_NOT_PRESENT), _if_stmt_init_decl_kind(if_stmt::init_decl_kind::KIND_INIT_DECL_PRESENT) {

		}

		if_stmt::if_stmt(shared_ptr<scope> is, shared_ptr<expression> c, shared_ptr<stmt> tp, shared_ptr<stmt> fp, bool v, vector<token> s) :
			_if_scope(is), _condition(c), _true_path(tp), _false_path(fp), _valid(v), _stream(s), _init_decl_list(vector<shared_ptr<variable_declaration>>()),
			_if_stmt_else_kind(if_stmt::else_kind::KIND_ELSE_PRESENT), _if_stmt_init_decl_kind(if_stmt::init_decl_kind::KIND_INIT_DECL_NOT_PRESENT) {

		}

		if_stmt::if_stmt(shared_ptr<scope> is, shared_ptr<expression> c, shared_ptr<stmt> tp, bool v, vector<token> s) :
			_if_scope(is), _condition(c), _true_path(tp), _valid(v), _stream(s), _init_decl_list(vector<shared_ptr<variable_declaration>>()), _false_path(nullptr),
			_if_stmt_else_kind(if_stmt::else_kind::KIND_ELSE_NOT_PRESENT), _if_stmt_init_decl_kind(if_stmt::init_decl_kind::KIND_INIT_DECL_NOT_PRESENT) {

		}

		if_stmt::~if_stmt() {

		}

		vector<shared_ptr<variable_declaration>> if_stmt::init_decl_list() {
			return _init_decl_list;
		}

		shared_ptr<expression> if_stmt::condition() {
			return _condition;
		}

		shared_ptr<stmt> if_stmt::true_path() {
			return _true_path;
		}

		shared_ptr<stmt> if_stmt::false_path() {
			return _false_path;
		}

		vector<token> if_stmt::stream() {
			return _stream;
		}

		bool if_stmt::valid() {
			return _valid;
		}

		if_stmt::else_kind if_stmt::if_stmt_else_kind() {
			return _if_stmt_else_kind;
		}

		if_stmt::init_decl_kind if_stmt::if_stmt_init_decl_kind() {
			return _if_stmt_init_decl_kind;
		}

		shared_ptr<scope> if_stmt::if_scope() {
			return _if_scope;
		}

		empty_stmt::empty_stmt(bool v, vector<token> s) : _valid(v), _stream(s) {

		}

		empty_stmt::~empty_stmt() {

		}

		bool empty_stmt::valid() {
			return _valid;
		}

		vector<token> empty_stmt::stream() {
			return _stream;
		}

		function_stmt::function_stmt(shared_ptr<function_type> ft, token fn, shared_ptr<function_symbol> fss, vector<shared_ptr<stmt>> fb, function_stmt::defined_kind dk,
			shared_ptr<function_scope> fs, bool v, vector<token> s) : _function_type(ft), _function_name(fn), _function_stmt_symbol(fss), _function_body(fb),
			_function_stmt_defined_kind(dk), _valid(v), _stream(s) {

		}

		function_stmt::~function_stmt() {

		}

		bool function_stmt::valid() {
			return _valid;
		}

		vector<token> function_stmt::stream() {
			return _stream;
		}

		shared_ptr<function_type> function_stmt::function_stmt_type() {
			return _function_type;
		}

		token function_stmt::function_name() {
			return _function_name;
		}

		vector<shared_ptr<stmt>> function_stmt::function_body() {
			return _function_body;
		}

		shared_ptr<function_symbol> function_stmt::function_stmt_symbol() {
			return _function_stmt_symbol;
		}

		function_stmt::defined_kind function_stmt::function_stmt_defined_kind() {
			return _function_stmt_defined_kind;
		}
		
		shared_ptr<function_scope> function_stmt::function_stmt_scope() {
			return _function_stmt_scope;
		}

		return_stmt::return_stmt(shared_ptr<expression> rv, bool v, vector<token> s) : _return_value(rv), _valid(v), _stream(s),
			_return_stmt_value_kind(return_stmt::value_kind::KIND_EXPRESSION) {

		}

		return_stmt::return_stmt(bool v, vector<token> s) : _return_value(nullptr), _valid(v), _stream(s), _return_stmt_value_kind(return_stmt::value_kind::KIND_EMPTY) {

		}

		return_stmt::~return_stmt() {

		}

		bool return_stmt::valid() {
			return _valid;
		}

		vector<token> return_stmt::stream() {
			return _stream;
		}

		shared_ptr<expression> return_stmt::return_value() {
			return _return_value;
		}

		return_stmt::value_kind return_stmt::return_stmt_value_kind() {
			return _return_stmt_value_kind;
		}

		while_stmt::while_stmt(while_stmt::while_kind wk, shared_ptr<expression> e, shared_ptr<stmt> b, bool v, vector<token> s) : _while_stmt_kind(wk), _condition(e),
			_while_body(b), _valid(v), _stream(s) {

		}

		while_stmt::~while_stmt() {

		}

		shared_ptr<expression> while_stmt::condition() {
			return _condition;
		}

		shared_ptr<stmt> while_stmt::while_body() {
			return _while_body;
		}

		bool while_stmt::valid() {
			return _valid;
		}

		vector<token> while_stmt::stream() {
			return _stream;
		}

		while_stmt::while_kind while_stmt::while_stmt_kind() {
			return _while_stmt_kind;
		}

		block_stmt::block_stmt(vector<shared_ptr<stmt>> sl, bool v, vector<token> s) :
			_stmt_list(sl), _valid(v), _stream(s) {

		}

		block_stmt::~block_stmt() {

		}

		vector<shared_ptr<stmt>> block_stmt::stmt_list() {
			return _stmt_list;
		}

		vector<token> block_stmt::stream() {
			return _stream;
		}

		bool block_stmt::valid() {
			return _valid;
		}

		stmt::stmt(stmt::kind sk, shared_ptr<expression> se, bool v, vector<token> s) :
			_stmt_kind(sk), _stmt_expression(se), _valid(v), _stream(s) {

		}

		stmt::stmt(stmt::kind sk, vector<shared_ptr<variable_declaration>> svdl, bool v, vector<token> s) :
			_stmt_kind(sk), _stmt_variable_declaration_list(svdl), _valid(v), _stream(s) {

		}

		stmt::stmt(stmt::kind k, shared_ptr<if_stmt> si, bool v, vector<token> s) :
			_stmt_kind(k), _stmt_if(si), _valid(v), _stream(s) {

		}

		stmt::stmt(stmt::kind k, shared_ptr<empty_stmt> se, bool v, vector<token> s) :
			_stmt_kind(k), _stmt_empty(se), _valid(v), _stream(s) {

		}
		
		stmt::stmt(stmt::kind sk, shared_ptr<function_stmt> sf, bool v, vector<token> s) :
			_stmt_kind(sk), _stmt_function(sf), _valid(v), _stream(s) {

		}

		stmt::stmt(stmt::kind sk, shared_ptr<return_stmt> sr, bool v, vector<token> s) :
			_stmt_kind(sk), _stmt_return(sr), _valid(v), _stream(s) {

		}

		stmt::stmt(stmt::kind sk, shared_ptr<while_stmt> sw, bool v, vector<token> s) :
			_stmt_kind(sk), _stmt_while(sw), _valid(v), _stream(s) {

		}

		stmt::stmt(stmt::kind sk, shared_ptr<block_stmt> sb, bool v, vector<token> s) :
			_stmt_kind(sk), _stmt_block(sb), _valid(v), _stream(s) {

		}

		stmt::stmt(stmt::kind sk, shared_ptr<break_continue_stmt> bc, bool v, vector<token> s) : _stmt_kind(sk), _stmt_break_continue(bc), _valid(v), _stream(s) {

		}

		stmt::stmt(stmt::kind sk, shared_ptr<for_stmt> f, bool v, vector<token> s) : _stmt_kind(sk), _stmt_for(f), _stream(s), _valid(v) {

		}

		stmt::stmt(stmt::kind sk, shared_ptr<case_default_stmt> cd, bool v, vector<token> s) : _stmt_kind(sk), _stmt_case_default(cd), _stream(s), _valid(v) {

		}

		stmt::stmt(stmt::kind sk, shared_ptr<switch_stmt> ss, bool v, vector<token> s) : _stmt_kind(sk), _stmt_switch(ss), _stream(s), _valid(v) {

		}
		
		stmt::stmt(stmt::kind sk, shared_ptr<struct_stmt> ss, bool v, vector<token> s) : _stmt_kind(sk), _stmt_struct(ss), _stream(s), _valid(v) {

		}

		stmt::stmt(stmt::kind sk, shared_ptr<namespace_stmt> ns, bool v, vector<token> s) : _valid(v), _stream(s), _stmt_kind(sk), _stmt_namespace(ns) {

		}

		stmt::stmt(stmt::kind sk, shared_ptr<using_stmt> us, bool v, vector<token> s) : _valid(v), _stream(s), _stmt_kind(sk), _stmt_using(us) {

		}

		stmt::stmt(stmt::kind sk, shared_ptr<asm_stmt> as, bool v, vector<token> s) : _valid(v), _stream(s), _stmt_asm(as), _stmt_kind(sk) {

		}

		stmt::stmt(stmt::kind sk, shared_ptr<include_stmt> is, bool v, vector<token> s) : _valid(v), _stream(s), _stmt_include(is), _stmt_kind(sk) {

		}

		stmt::stmt(stmt::kind sk, shared_ptr<access_stmt> as, bool v, vector<token> s) : _valid(v), _stream(s), _stmt_access(as), _stmt_kind(sk) {

		}

		stmt::stmt(stmt::kind sk, shared_ptr<delete_stmt> ds, bool v, vector<token> s) : _valid(v), _stream(s), _stmt_delete(ds), _stmt_kind(sk) {

		}

		stmt::stmt() : _stmt_kind(stmt::kind::KIND_NONE), _stmt_expression(nullptr), _valid(false), _stream(vector<token>()) {

		}

		stmt::~stmt() {

		}

		shared_ptr<expression> stmt::stmt_expression() {
			return _stmt_expression;
		}

		vector<shared_ptr<variable_declaration>> stmt::stmt_variable_declaration_list() {
			return _stmt_variable_declaration_list;
		}

		bool stmt::valid() {
			return _valid;
		}

		stmt::kind stmt::stmt_kind() {
			return _stmt_kind;
		}

		vector<token> stmt::stream() {
			return _stream;
		}

		shared_ptr<if_stmt> stmt::stmt_if() {
			return _stmt_if;
		}

		shared_ptr<empty_stmt> stmt::stmt_empty() {
			return _stmt_empty;
		}

		shared_ptr<function_stmt> stmt::stmt_function() {
			return _stmt_function;
		}

		shared_ptr<return_stmt> stmt::stmt_return() {
			return _stmt_return;
		}

		shared_ptr<while_stmt> stmt::stmt_while() {
			return _stmt_while;
		}

		shared_ptr<block_stmt> stmt::stmt_block() {
			return _stmt_block;
		}

		shared_ptr<break_continue_stmt> stmt::stmt_break_continue() {
			return _stmt_break_continue;
		}

		shared_ptr<for_stmt> stmt::stmt_for() {
			return _stmt_for;
		}

		shared_ptr<case_default_stmt> stmt::stmt_case_default() {
			return _stmt_case_default;
		}

		shared_ptr<switch_stmt> stmt::stmt_switch() {
			return _stmt_switch;
		}

		shared_ptr<struct_stmt> stmt::stmt_struct() {
			return _stmt_struct;
		}

		shared_ptr<namespace_stmt> stmt::stmt_namespace() {
			return _stmt_namespace;
		}

		shared_ptr<using_stmt> stmt::stmt_using() {
			return _stmt_using;
		}

		shared_ptr<asm_stmt> stmt::stmt_asm() {
			return _stmt_asm;
		}

		shared_ptr<include_stmt> stmt::stmt_include() {
			return _stmt_include;
		}

		shared_ptr<access_stmt> stmt::stmt_access() {
			return _stmt_access;
		}

		shared_ptr<delete_stmt> stmt::stmt_delete() {
			return _stmt_delete;
		}

		break_continue_stmt::break_continue_stmt(break_continue_stmt::kind k, vector<token> s, bool v) :
			_break_continue_stmt_kind(k), _stream(s), _valid(v) {

		}

		break_continue_stmt::~break_continue_stmt() {

		}

		break_continue_stmt::kind break_continue_stmt::break_continue_stmt_kind() {
			return _break_continue_stmt_kind;
		}

		vector<token> break_continue_stmt::stream() {
			return _stream;
		}

		bool break_continue_stmt::valid() {
			return _valid;
		}

		for_stmt::for_stmt(vector<shared_ptr<variable_declaration>> i, for_stmt::initializer_present_kind ip, shared_ptr<expression> c,
			for_stmt::condition_present_kind cp, shared_ptr<expression> u, for_stmt::update_present_kind up, shared_ptr<stmt> b, bool v, vector<token> s) :
			_initializer(i), _for_stmt_initializer_present_kind(ip), _condition(c), _for_stmt_condition_present_kind(cp), _update(u),
			_for_stmt_update_present_kind(up), _valid(v), _stream(s), _for_stmt_body(b) {

		}

		for_stmt::~for_stmt() {

		}

		vector<shared_ptr<variable_declaration>> for_stmt::initializer() {
			return _initializer;
		}

		shared_ptr<expression> for_stmt::condition() {
			return _condition;
		}

		shared_ptr<expression> for_stmt::update() {
			return _update;
		}

		vector<token> for_stmt::stream() {
			return _stream;
		}

		bool for_stmt::valid() {
			return _valid;
		}

		for_stmt::initializer_present_kind for_stmt::for_stmt_initializer_present_kind() {
			return _for_stmt_initializer_present_kind;
		}

		for_stmt::condition_present_kind for_stmt::for_stmt_condition_present_kind() {
			return _for_stmt_condition_present_kind;
		}

		for_stmt::update_present_kind for_stmt::for_stmt_update_present_kind() {
			return _for_stmt_update_present_kind;
		}

		shared_ptr<stmt> for_stmt::for_stmt_body() {
			return _for_stmt_body;
		}

		case_default_stmt::case_default_stmt(case_default_stmt::kind k, shared_ptr<expression> ce, shared_ptr<stmt> b, bool v, int l, vector<token> s) :
			_case_default_stmt_kind(k), _case_expression(ce), _case_default_stmt_body(b), _valid(v), _stream(s), _case_default_stmt_label_number(l) {

		}

		case_default_stmt::~case_default_stmt() {

		}

		case_default_stmt::kind case_default_stmt::case_default_stmt_kind() {
			return _case_default_stmt_kind;
		}

		shared_ptr<expression> case_default_stmt::case_expression() {
			return _case_expression;
		}

		shared_ptr<stmt> case_default_stmt::case_default_stmt_body() {
			return _case_default_stmt_body;
		}

		bool case_default_stmt::valid() {
			return _valid;
		}

		vector<token> case_default_stmt::stream() {
			return _stream;
		}

		int case_default_stmt::case_default_stmt_label_number() {
			return _case_default_stmt_label_number;
		}

		switch_stmt::switch_stmt(shared_ptr<expression> pe, shared_ptr<switch_scope> sss, shared_ptr<stmt> ssb, bool v, vector<token> s) :
			_parent_expression(pe), _switch_stmt_scope(sss), _switch_stmt_body(ssb), _valid(v), _stream(s) {

		}

		switch_stmt::~switch_stmt() {

		}

		shared_ptr<switch_scope> switch_stmt::switch_stmt_scope() {
			return _switch_stmt_scope;
		}

		shared_ptr<expression> switch_stmt::parent_expression() {
			return _parent_expression;
		}

		shared_ptr<stmt> switch_stmt::switch_stmt_body() {
			return _switch_stmt_body;
		}

		bool switch_stmt::valid() {
			return _valid;
		}

		vector<token> switch_stmt::stream() {
			return _stream;
		}

		struct_stmt::struct_stmt(struct_stmt::defined_kind dk, token sn, shared_ptr<scope> sss, shared_ptr<type> sst, shared_ptr<struct_symbol> ss,
			vector<shared_ptr<variable_declaration>> vdl, bool v, vector<token> s) : _struct_stmt_defined_kind(dk),
			_struct_name(sn), _struct_stmt_scope(sss), _struct_stmt_type(sst), _struct_stmt_symbol(ss), _struct_variable_declaration_list(vdl), _valid(v), _stream(s) {

		}

		struct_stmt::~struct_stmt() {

		}

		shared_ptr<scope> struct_stmt::struct_stmt_scope() {
			return _struct_stmt_scope;
		}

		token struct_stmt::struct_name() {
			return _struct_name;
		}

		vector<shared_ptr<variable_declaration>> struct_stmt::struct_variable_declaration_list() {
			return _struct_variable_declaration_list;
		}

		bool struct_stmt::valid() {
			return _valid;
		}

		vector<token> struct_stmt::stream() {
			return _stream;
		}

		shared_ptr<type> struct_stmt::struct_stmt_type() {
			return _struct_stmt_type;
		}

		shared_ptr<struct_symbol> struct_stmt::struct_stmt_symbol() {
			return _struct_stmt_symbol;
		}

		struct_stmt::defined_kind struct_stmt::struct_stmt_defined_kind() {
			return _struct_stmt_defined_kind;
		}

		namespace_stmt::namespace_stmt(namespace_stmt::kind k, namespace_stmt::defined_kind dk, token nn, shared_ptr<namespace_scope> nss, shared_ptr<namespace_symbol> ns,
			vector<shared_ptr<stmt>> nsl, bool v, vector<token> s) : _namespace_stmt_scope(nss), _namespace_name(nn), _namespace_stmt_defined_kind(dk), _namespace_stmt_list(nsl), _valid(v),
			_stream(s), _namespace_stmt_symbol(ns), _namespace_stmt_kind(k) {

		}

		namespace_stmt::~namespace_stmt() {

		}

		shared_ptr<namespace_scope> namespace_stmt::namespace_stmt_scope() {
			return _namespace_stmt_scope;
		}

		token namespace_stmt::namespace_name() {
			return _namespace_name;
		}

		vector<shared_ptr<stmt>> namespace_stmt::namespace_stmt_list() {
			return _namespace_stmt_list;
		}

		bool namespace_stmt::valid() {
			return _valid;
		}

		vector<token> namespace_stmt::stream() {
			return _stream;
		}

		shared_ptr<namespace_symbol> namespace_stmt::namespace_stmt_symbol() {
			return _namespace_stmt_symbol;
		}

		namespace_stmt::defined_kind namespace_stmt::namespace_stmt_defined_kind() {
			return _namespace_stmt_defined_kind;
		}

		namespace_stmt::kind namespace_stmt::namespace_stmt_kind() {
			return _namespace_stmt_kind;
		}

		using_stmt::using_stmt(using_stmt::kind k, shared_ptr<symbol> uss, vector<token> s, bool v) : _using_stmt_kind(k), _using_stmt_symbol(uss), _stream(s), _valid(v) {

		}

		using_stmt::~using_stmt() {

		}

		using_stmt::kind using_stmt::using_stmt_kind() {
			return _using_stmt_kind;
		}

		shared_ptr<symbol> using_stmt::using_stmt_symbol() {
			return _using_stmt_symbol;
		}

		vector<token> using_stmt::stream() {
			return _stream;
		}

		bool using_stmt::valid() {
			return _valid;
		}

		asm_stmt::asm_type::asm_type(vector<token> s) : _asm_type_kind(asm_stmt::kind::KIND_RAW_STRING), _raw_stmt(s), _raw_register(vector<token>{}), _identifier(bad_token),
			_identifier_symbol(nullptr) {

		}

		asm_stmt::asm_type::asm_type(vector<token> r, shared_ptr<symbol> is, token i) : _asm_type_kind(asm_stmt::kind::KIND_LA), _raw_stmt(vector<token>{}), _raw_register(r), _identifier(i),
			_identifier_symbol(is) {

		}

		asm_stmt::asm_type::~asm_type() {

		}

		vector<token> asm_stmt::asm_type::raw_stmt() {
			return _raw_stmt;
		}

		vector<token> asm_stmt::asm_type::raw_register() {
			return _raw_register;
		}

		token asm_stmt::asm_type::identifier() {
			return _identifier;
		}

		asm_stmt::kind asm_stmt::asm_type::asm_type_kind() {
			return _asm_type_kind;
		}

		shared_ptr<symbol> asm_stmt::asm_type::identifier_symbol() {
			return _identifier_symbol;
		}

		asm_stmt::asm_stmt(vector<shared_ptr<asm_stmt::asm_type>> atl, vector<token> s, bool v) : _asm_type_list(atl), _stream(s), _valid(v) {

		}

		asm_stmt::~asm_stmt() {

		}

		vector<shared_ptr<asm_stmt::asm_type>> asm_stmt::asm_type_list() {
			return _asm_type_list;
		}

		vector<token> asm_stmt::stream() {
			return _stream;
		}

		bool asm_stmt::valid() {
			return _valid;
		}

		include_stmt::include_type::include_type(token li) : _local_include(li), _system_include(vector<token>{}), _include_stmt_kind(include_stmt::kind::KIND_LOCAL) {

		}

		include_stmt::include_type::include_type(vector<token> si) : _local_include(bad_token), _system_include(si), _include_stmt_kind(include_stmt::kind::KIND_SYSTEM) {

		}

		token include_stmt::include_type::local_include() {
			return _local_include;
		}

		vector<token> include_stmt::include_type::system_include() {
			return _system_include;
		}

		include_stmt::kind include_stmt::include_type::include_stmt_kind() {
			return _include_stmt_kind;
		}

		include_stmt::include_stmt(include_stmt::type t, vector<shared_ptr<include_stmt::include_type>> il, vector<token> s, bool v) : _include_list(il), _stream(s), _valid(v),
			_include_stmt_type(t) {

		}

		include_stmt::~include_stmt() {

		}

		vector<shared_ptr<include_stmt::include_type>> include_stmt::include_list() {
			return _include_list;
		}

		vector<token> include_stmt::stream() {
			return _stream;
		}

		bool include_stmt::valid() {
			return _valid;
		}

		include_stmt::type include_stmt::include_stmt_type() {
			return _include_stmt_type;
		}

		access_stmt::access_stmt(shared_ptr<type> dt, vector<shared_ptr<symbol>> dsl, vector<token> s, bool v) : _declared_type(dt), _declared_symbol_list(dsl), _stream(s), _valid(v) {

		}

		access_stmt::~access_stmt() {

		}

		shared_ptr<type> access_stmt::declared_type() {
			return _declared_type;
		}

		vector<shared_ptr<symbol>> access_stmt::declared_symbol_list() {
			return _declared_symbol_list;
		}

		vector<token> access_stmt::stream() {
			return _stream;
		}

		bool access_stmt::valid() {
			return _valid;
		}

		delete_stmt::delete_stmt(shared_ptr<expression> e, vector<token> s, bool v) : _expr(e), _stream(s), _valid(v) {

		}

		delete_stmt::~delete_stmt() {

		}

		vector<token> delete_stmt::stream() {
			return _stream;
		}

		bool delete_stmt::valid() {
			return _valid;
		}

		shared_ptr<expression> delete_stmt::expr() {
			return _expr;
		}
	}
}
