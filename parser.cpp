#include "config.hpp"
#include "parser.hpp"
#include "file_io.hpp"
#include <memory>
#include <iostream>
#include <experimental/filesystem>

using std::make_shared;
using std::static_pointer_cast;
using spectre::ast::empty_stmt;
using std::make_tuple;
using std::get;
using namespace spectre::file_io;

namespace spectre {
	namespace parser {

		symbol::symbol(symbol::kind k, shared_ptr<scope> ps) : _symbol_kind(k), _parent_scope(ps) {

		}

		symbol::~symbol() {

		}

		symbol::kind symbol::symbol_kind() {
			return _symbol_kind;
		}

		shared_ptr<scope> symbol::parent_scope() {
			return _parent_scope;
		}

		variable_symbol::variable_symbol(shared_ptr<scope> ps, token vn, shared_ptr<type> vt, vector<token> s) : symbol(symbol::kind::KIND_VARIABLE, ps), _variable_name(vn),
			_variable_type(vt), _stream(s) {

		}

		variable_symbol::~variable_symbol() {

		}

		shared_ptr<type> variable_symbol::variable_type() {
			return _variable_type;
		}

		token variable_symbol::variable_name() {
			return _variable_name;
		}

		vector<token> variable_symbol::stream() {
			return _stream;
		}

		function_symbol::function_symbol(shared_ptr<scope> ps, shared_ptr<function_type> fst, token fn, vector<token> s, function_stmt::defined_kind dk) :
			symbol(symbol::kind::KIND_FUNCTION, ps), _function_symbol_type(fst), _function_name(fn), _stream(s), _function_symbol_defined_kind(dk) {

		}

		function_symbol::~function_symbol() {

		}

		token function_symbol::function_name() {
			return _function_name;
		}

		shared_ptr<function_type> function_symbol::function_symbol_type() {
			return _function_symbol_type;
		}

		vector<token> function_symbol::stream() {
			return _stream;
		}

		function_stmt::defined_kind function_symbol::function_symbol_defined_kind() {
			return _function_symbol_defined_kind;
		}

		void function_symbol::set_function_symbol_defined_kind(function_stmt::defined_kind dk) {
			_function_symbol_defined_kind = dk;
		}

		struct_symbol::struct_symbol(shared_ptr<scope> ps, shared_ptr<struct_type> sst, token sn, shared_ptr<scope> ss, vector<token> s, struct_stmt::defined_kind dk) :
			symbol(symbol::kind::KIND_STRUCT, ps), _struct_symbol_type(sst), _struct_name(sn), _stream(s), _struct_symbol_scope(ss), _struct_symbol_defined_kind(dk) {

		}

		struct_symbol::~struct_symbol() {

		}

		vector<token> struct_symbol::stream() {
			return _stream;
		}

		struct_stmt::defined_kind struct_symbol::struct_symbol_defined_kind() {
			return _struct_symbol_defined_kind;
		}

		shared_ptr<struct_type> struct_symbol::struct_symbol_type() {
			return _struct_symbol_type;
		}

		token struct_symbol::struct_name() {
			return _struct_name;
		}

		void struct_symbol::set_struct_symbol_defined_kind(struct_stmt::defined_kind dk) {
			_struct_symbol_defined_kind = dk;
		}

		shared_ptr<scope> struct_symbol::struct_symbol_scope() {
			return _struct_symbol_scope;
		}

		namespace_symbol::namespace_symbol(shared_ptr<scope> ps, token nsn, shared_ptr<namespace_scope> ns, vector<token> s, namespace_stmt::defined_kind dk) :
			symbol(symbol::kind::KIND_NAMESPACE, ps), _namespace_symbol_name(nsn), _namespace_symbol_scope(ns), _stream(s), _namespace_symbol_defined_kind(dk) {

		}

		namespace_symbol::~namespace_symbol() {

		}

		vector<token> namespace_symbol::stream() {
			return _stream;
		}

		namespace_stmt::defined_kind namespace_symbol::namespace_symbol_defined_kind() {
			return _namespace_symbol_defined_kind;
		}

		token namespace_symbol::namespace_symbol_name() {
			return _namespace_symbol_name;
		}

		void namespace_symbol::set_namespace_symbol_defined_kind(namespace_stmt::defined_kind dk) {
			_namespace_symbol_defined_kind = dk;
		}

		shared_ptr<namespace_scope> namespace_symbol::namespace_symbol_scope() {
			return _namespace_symbol_scope;
		}

		scope::scope(scope::kind sk, shared_ptr<scope> ps) : _scope_kind(sk), _parent_scope(ps), _symbol_list(vector<shared_ptr<symbol>>()) {
	
		}

		scope::~scope() {

		}

		void scope::add_symbol(shared_ptr<symbol> s) {
			_symbol_list.push_back(s);
		}

		shared_ptr<symbol> scope::find_symbol(token t, bool strict) {
			if(t.token_kind() != token::kind::TOKEN_IDENTIFIER)
				return nullptr;
			for (shared_ptr<symbol> s : _symbol_list) {
				if (s->symbol_kind() == symbol::kind::KIND_VARIABLE) {
					if (static_pointer_cast<variable_symbol>(s)->variable_name().raw_text() == t.raw_text())
						return s;
				}
				else if (s->symbol_kind() == symbol::kind::KIND_FUNCTION) {
					if (static_pointer_cast<function_symbol>(s)->function_name().raw_text() == t.raw_text())
						return s;
				}
				else if (s->symbol_kind() == symbol::kind::KIND_STRUCT) {
					if (static_pointer_cast<struct_symbol>(s)->struct_name().raw_text() == t.raw_text())
						return s;
				}
				else if (s->symbol_kind() == symbol::kind::KIND_NAMESPACE)
					if (static_pointer_cast<namespace_symbol>(s)->namespace_symbol_name().raw_text() == t.raw_text())
						return s;
			}
			if (_scope_kind == scope::kind::KIND_GLOBAL || _parent_scope == nullptr)
				return nullptr;
			return strict ? nullptr : _parent_scope->find_symbol(t, strict);
		}

		scope::kind scope::scope_kind() {
			return _scope_kind;
		}

		shared_ptr<scope> scope::parent_scope() {
			return _parent_scope;
		}

		namespace_scope::namespace_scope(token nn, shared_ptr<scope> ps) : scope(scope::kind::KIND_NAMESPACE, ps), _namespace_name(nn) {

		}

		namespace_scope::~namespace_scope() {

		}

		token namespace_scope::namespace_name() {
			return _namespace_name;
		}

		vector<shared_ptr<symbol>> scope::symbol_list() {
			return _symbol_list;
		}

		switch_scope::switch_scope(shared_ptr<expression> pe, shared_ptr<scope> ps) :
			scope(scope::kind::KIND_SWITCH, ps), _parent_expression(pe), _case_default_stmt_list(vector<shared_ptr<case_default_stmt>>{}), _default_stmt_hit(false) {

		}

		switch_scope::~switch_scope() {

		}

		shared_ptr<expression> switch_scope::parent_expression() {
			return _parent_expression;
		}

		bool switch_scope::default_stmt_hit() {
			return _default_stmt_hit;
		}

		void switch_scope::default_stmt_hit(bool b) {
			_default_stmt_hit = b;
		}

		vector<shared_ptr<case_default_stmt>> switch_scope::case_default_stmt_list() {
			return _case_default_stmt_list;
		}

		void switch_scope::add_case_default_stmt(shared_ptr<case_default_stmt> c) {
			_case_default_stmt_list.push_back(c);
		}

		block_scope::block_scope(int bn, shared_ptr<scope> ps) : scope(scope::kind::KIND_BLOCK, ps), _block_number(bn) {

		}

		block_scope::~block_scope() {

		}

		int block_scope::block_number() {
			return _block_number;
		}

		function_scope::function_scope(shared_ptr<scope> ps, type::const_kind ck, type::static_kind sk, shared_ptr<type> rt, token fn) :
			scope(scope::kind::KIND_FUNCTION, ps), _return_type(rt), _function_name(fn), _function_const_kind(ck), _function_static_kind(sk) {

		}

		function_scope::~function_scope() {

		}

		shared_ptr<type> function_scope::return_type() {
			return _return_type;
		}

		type::const_kind function_scope::function_const_kind() {
			return _function_const_kind;
		}

		type::static_kind function_scope::function_static_kind() {
			return _function_static_kind;
		}

		token function_scope::function_name() {
			return _function_name;
		}

		parser::parser(buffer sb) : _source_buffer(sb), _pos(0), _stmt_list(vector<shared_ptr<stmt>>()), _function_reference_number_counter(0),
			_diagnostics_reporter(sb.diagnostics_reporter()), _global_scope(make_shared<scope>(scope::kind::KIND_GLOBAL, nullptr)), _current_scope(_global_scope),
			_block_scope_number(0), _case_default_stmt_label_number_counter(0), _struct_reference_number_counter(0), _struct_symbol_table(vector<shared_ptr<struct_symbol>>{}),
			_function_symbol_table(vector<shared_ptr<function_symbol>>{}), _consumed_token_list(vector<token>{}) {

		}

		parser::~parser() {

		}

		vector<shared_ptr<stmt>> parser::stmt_list() {
			return _stmt_list;
		}

		void parser::report(error err) {
			_diagnostics_reporter->report(err);
		}

		void parser::report_internal(string msg, string fn, int ln, string fl) {
			diagnostics::report_internal(msg, fn, ln, fl);
		}

		void parser::add_symbol_to_current_scope(shared_ptr<symbol> s) {
			_current_scope->add_symbol(s);
		}

		int parser::next_case_default_stmt_label_number() {
			return _case_default_stmt_label_number_counter++;
		}

		shared_ptr<symbol> parser::find_symbol_in_current_scope(token t, bool strict) {
			return _current_scope->find_symbol(t, strict);
		}

		shared_ptr<scope> parser::current_scope() {
			return _current_scope;
		}

		shared_ptr<scope> parser::global_scope() {
			return _global_scope;
		}

		shared_ptr<scope> parser::within_scope(scope::kind sk) {
			if (sk == scope::kind::KIND_GLOBAL)
				return _global_scope;
			return within_scope_helper(sk, _current_scope);
		}

		int parser::next_struct_reference_number() {
			return _struct_reference_number_counter++;
		}

		shared_ptr<scope> parser::within_scope_helper(scope::kind sk, shared_ptr<scope> s) {
			if (s == nullptr)
				return nullptr;
			if (s->scope_kind() == sk)
				return s;
			else if (s->scope_kind() == scope::kind::KIND_GLOBAL || s->parent_scope() == nullptr)
				return nullptr;
			else
				return within_scope_helper(sk, s->parent_scope());
		}

		void parser::open_new_scope(shared_ptr<scope> s) {
			_current_scope = nullptr;
			_current_scope = s;
		}

		void parser::close_current_scope() {
			if (_current_scope->scope_kind() != scope::kind::KIND_GLOBAL)
				_current_scope = _current_scope->parent_scope();
		}

		void parser::backtrack() {
			if (_pos == 0) return;
			else
				_pos--;
		}

		void parser::set_buffer_position(int pos) {
			_pos = pos;
		}

		token parser::pop() {
			if (_pos >= _consumed_token_list.size()) {
				_pos++;
				token tok = _source_buffer.pop();
				_consumed_token_list.push_back(tok);
				return tok;
			}
			token ret = _consumed_token_list[_pos];
			_pos++;
			return ret;
		}

		int parser::get_buffer_position() {
			return _pos;
		}

		bool parser::parse_stmt() {
			token tok = pop();
			if (tok.token_kind() == token::kind::TOKEN_EOF) {
				_pos--;
				return false;
			}
			_pos--;
			stmt_parser sp = stmt_parser(shared_from_this());
			if (sp.valid() && sp.contained_stmt()->stmt_kind() != stmt::kind::KIND_FUNCTION && sp.contained_stmt()->stmt_kind() != stmt::kind::KIND_STRUCT &&
				sp.contained_stmt()->stmt_kind() != stmt::kind::KIND_VARIABLE_DECLARATION && sp.contained_stmt()->stmt_kind() != stmt::kind::KIND_NAMESPACE &&
				sp.contained_stmt()->stmt_kind() != stmt::kind::KIND_EMPTY && sp.contained_stmt()->stmt_kind() != stmt::kind::KIND_USING &&
				sp.contained_stmt()->stmt_kind() != stmt::kind::KIND_INCLUDE && sp.contained_stmt()->stmt_kind() != stmt::kind::KIND_ACCESS) {
				report(error(error::kind::KIND_ERROR,
					"Only includes/imports, namespaces, using statements, functions, struct's, accesses, and variable declarations are allowed at the global scope.",
					sp.contained_stmt()->stream(), 0));
			}
			_stmt_list.push_back(sp.contained_stmt());
			return sp.valid();
		}

		token parser::peek() {
			token ret = pop();
			_pos--;
			return ret;
		}

		token parser::get_token(int pos) {
			if (pos >= _source_buffer.consumed_token_list().size())
				return bad_token;
			return _source_buffer.consumed_token_list()[pos];
		}

		int parser::next_block_scope_number() {
			return _block_scope_number++;
		}

		void parser::add_to_struct_symbol_table(shared_ptr<struct_symbol> s) {
			_struct_symbol_table.push_back(s);
		}

		shared_ptr<struct_symbol> parser::find_struct_symbol(token n, int r) {
			for (shared_ptr<struct_symbol> s : _struct_symbol_table)
				if (s->struct_name().raw_text() == n.raw_text() && s->struct_symbol_type()->struct_reference_number() == r)
					return s;
			return nullptr;
		}

		int parser::next_function_reference_number() {
			return _function_reference_number_counter++;
		}

		void parser::add_to_function_symbol_table(shared_ptr<function_symbol> s) {
			_function_symbol_table.push_back(s);
		}

		shared_ptr<function_symbol> parser::find_function_symbol(token n, int r) {
			for (shared_ptr<function_symbol> s : _function_symbol_table)
				if (s->function_name().raw_text() == n.raw_text() && s->function_symbol_type()->function_reference_number() == r)
					return s;
			return nullptr;
		}

		void parser::insert_token_list(int i, vector<token> vec) {
			for (int j = 0; j < vec.size(); j++)
				_consumed_token_list.insert(_consumed_token_list.begin() + i + j, vec[j]);
		}

		void parser::add_to_import_list(shared_ptr<fs::path> f) {
			_import_list.push_back(f);
		}

		bool parser::in_import_list(shared_ptr<fs::path> f) {
			for (shared_ptr<fs::path> fp : _import_list)
				if (fs::equivalent(*f, *fp)) return true;
			return false;
		}

		primary_expression_parser::primary_expression_parser(shared_ptr<parser> p) : 
			_valid(false), _contained_primary_expression(nullptr) {
			token tok = p->pop();
			if (tok.token_kind() == token::kind::TOKEN_INTEGER) {
				shared_ptr<type> t = nullptr;
				if (tok.suffix_kind() == token::suffix::SUFFIX_DOUBLE)
					t = make_shared<primitive_type>(primitive_type::kind::KIND_DOUBLE, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
						type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_NONE);
				else if (tok.suffix_kind() == token::suffix::SUFFIX_FLOAT)
					t = make_shared<primitive_type>(primitive_type::kind::KIND_FLOAT, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
						type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_NONE);
				else if (tok.suffix_kind() == token::suffix::SUFFIX_SIGNED_LONG)
					t = make_shared<primitive_type>(primitive_type::kind::KIND_LONG, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
						type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_SIGNED);
				else if (tok.suffix_kind() == token::suffix::SUFFIX_SIGNED_SHORT)
					t = make_shared<primitive_type>(primitive_type::kind::KIND_SHORT, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
						type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_SIGNED);
				else if (tok.suffix_kind() == token::suffix::SUFFIX_UNSIGNED_LONG)
					t = make_shared<primitive_type>(primitive_type::kind::KIND_LONG, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
						type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_UNSIGNED);
				else if (tok.suffix_kind() == token::suffix::SUFFIX_UNSIGNED_SHORT)
					t = make_shared<primitive_type>(primitive_type::kind::KIND_SHORT, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
						type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_UNSIGNED);
				else if (tok.suffix_kind() == token::suffix::SUFFIX_UNSIGNED_INT)
					t = make_shared<primitive_type>(primitive_type::kind::KIND_INT, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
						type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_UNSIGNED);
				else {
					if (tok.prefix_kind() == token::prefix::PREFIX_BINARY)
						t = make_shared<primitive_type>(primitive_type::kind::KIND_BYTE, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
							type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_UNSIGNED);
					else
						t = make_shared<primitive_type>(primitive_type::kind::KIND_INT, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
							type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_SIGNED);
				}
				_valid = true;
				_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_LITERAL, tok, t, _valid, vector<token>{ tok }, value_kind::VALUE_RVALUE);
			}
			else if (tok.token_kind() == token::kind::TOKEN_DECIMAL) {
				shared_ptr<type> t = nullptr;
				if (tok.suffix_kind() == token::suffix::SUFFIX_DOUBLE)
					t = make_shared<primitive_type>(primitive_type::kind::KIND_DOUBLE, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
						type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_NONE);
				else if (tok.suffix_kind() == token::suffix::SUFFIX_FLOAT)
					t = make_shared<primitive_type>(primitive_type::kind::KIND_FLOAT, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
						type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_NONE);
				else if(tok.suffix_kind() == token::suffix::SUFFIX_NONE)
					t = make_shared<primitive_type>(primitive_type::kind::KIND_DOUBLE, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
						type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_NONE);
				else {
					p->report(error(error::kind::KIND_ERROR, "Cannot have a non-float or double suffix for a decimal literal.", vector<token>{ tok }, 0));
					_valid = false;
					_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_LITERAL, tok, t, _valid, vector<token>{ tok }, value_kind::VALUE_RVALUE);
				}
				_valid = true;
				_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_LITERAL, tok, t, _valid, vector<token>{ tok }, value_kind::VALUE_RVALUE);
			}
			else if (tok.token_kind() == token::kind::TOKEN_STRING) {
				vector<token> stream;
				stream.push_back(tok);
				while (p->peek().token_kind() == token::kind::TOKEN_STRING)
					stream.push_back(p->pop());
				shared_ptr<type> t = make_shared<primitive_type>(primitive_type::kind::KIND_CHAR, type::const_kind::KIND_NON_CONST, type::static_kind::KIND_NONE,
					primitive_type::sign_kind::KIND_UNSIGNED, 1);
				_valid = true;
				_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_LITERAL, stream[stream.size() - 1], t, _valid, stream, value_kind::VALUE_RVALUE);
			}
			else if (tok.token_kind() == token::kind::TOKEN_CHARACTER) {
				shared_ptr<type> t = make_shared<primitive_type>(primitive_type::kind::KIND_CHAR, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
					type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_UNSIGNED);
				_valid = true;
				_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_LITERAL, tok, t, _valid, vector<token>{ tok }, value_kind::VALUE_RVALUE);
			}
			else if (tok.token_kind() == token::kind::TOKEN_TRUE || tok.token_kind() == token::kind::TOKEN_FALSE) {
				shared_ptr<type> t = make_shared<primitive_type>(primitive_type::kind::KIND_BOOL, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
					type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_NONE);
				_valid = true;
				_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_LITERAL, tok, t, _valid, vector<token>{ tok }, value_kind::VALUE_RVALUE);
			}
			else if (tok.token_kind() == token::kind::TOKEN_SIZEOF) {
				vector<token> stream;
				stream.push_back(tok);
				token look_ahead = p->peek();
				shared_ptr<type> t = make_shared<primitive_type>(primitive_type::kind::KIND_INT, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
					type::constexpr_kind::KIND_CONSTEXPR, primitive_type::sign_kind::KIND_UNSIGNED);
				if (look_ahead.token_kind() == token::kind::TOKEN_OPEN_BRACE) {
					stream.push_back(p->pop());
					type_parser tp(p);
					if (!tp.valid()) {
						p->report(error(error::kind::KIND_ERROR, "Cannot take the 'sizeof' an invalid type.", stream, 0));
						_valid = false;
						_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_SIZEOF_TYPE, tp.contained_type(), t, _valid, stream);
						return;
					}
					if (tp.contained_type()->type_kind() == type::kind::KIND_AUTO) {
						p->report(error(error::kind::KIND_ERROR, "Cannot take the 'sizeof' an auto-type.", stream, 0));
						_valid = false;
						_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_SIZEOF_TYPE, tp.contained_type(), t, _valid, stream);
						return;
					}
					vector<token> tstream = tp.stream();
					stream.insert(stream.end(), tstream.begin(), tstream.end());
					token cbrace = p->peek();
					if (cbrace.token_kind() != token::kind::TOKEN_CLOSE_BRACE) {
						p->report(error(error::kind::KIND_ERROR, "Expected a close brace '}' after taking the 'sizeof' a type.", stream, 0));
						_valid = false;
						_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_SIZEOF_TYPE, tp.contained_type(), t, _valid, stream);
						return;
					}
					stream.push_back(p->pop());
					_valid = true;
					_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_SIZEOF_TYPE, tp.contained_type(), t, _valid, stream);
					return;
				}
				else if(look_ahead.token_kind() == token::kind::TOKEN_OPEN_PARENTHESIS) {
					stream.push_back(p->pop());
					expression_parser ep(p);
					if (!ep.valid()) {
						p->report(error(error::kind::KIND_ERROR, "Cannot take the 'sizeof' an invalid expression.", stream, 0));
						_valid = false;
						_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_SIZEOF_EXPRESSION, ep.contained_expression(), t, _valid, stream);
						return;
					}
					vector<token> estream = ep.contained_expression()->stream();
					stream.insert(stream.end(), estream.begin(), estream.end());
					token cparen = p->peek();
					if (cparen.token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
						p->report(error(error::kind::KIND_ERROR, "Expected a close parenthesis ')' after taking the 'sizeof' an expression.", stream, 0));
						_valid = false;
						_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_SIZEOF_EXPRESSION, ep.contained_expression(), t, _valid, stream);
						return;
					}
					stream.push_back(p->pop());
					_valid = true;
					_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_SIZEOF_EXPRESSION, ep.contained_expression(), t, _valid, stream);
					return;
				}
				else {
					p->report(error(error::kind::KIND_ERROR, "Expected a '{' or '(' to take the 'sizeof' a type or expression respectively.", stream, 0));
					_valid = false;
					_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_SIZEOF_EXPRESSION, (shared_ptr<expression>) nullptr, t, _valid, stream);
					return;
				}
			}
			else if (tok.token_kind() == token::kind::TOKEN_OPEN_PARENTHESIS) {
				vector<token> stream;
				stream.push_back(tok);
				expression_parser ep(p);
				shared_ptr<expression> e = ep.contained_expression();
				for (token t : e->stream()) stream.push_back(t);
				token close_parenthesis = p->pop();
				stream.push_back(close_parenthesis);
				if (close_parenthesis.token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
					p->report(error(error::kind::KIND_ERROR, "Expected a close parenthesis here.", stream, stream.size() - 1));
					_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_PARENTHESIZED_EXPRESSION, e, e->expression_type(), false,
						stream, e->expression_value_kind());
					_valid = false;
					return;
				}
				_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_PARENTHESIZED_EXPRESSION, e, e->expression_type(), e->valid(),
					stream, e->expression_value_kind());
				_valid = e->valid();
			}
			else if (tok.token_kind() == token::kind::TOKEN_IDENTIFIER || tok.token_kind() == token::kind::TOKEN_COLON_COLON) {
				p->backtrack();
				tuple<bool, vector<token>, shared_ptr<symbol>> tup = handle_qualified_identifier_name(p, true);
				shared_ptr<symbol> s = get<2>(tup);
				vector<token> stream = get<1>(tup);
				if (!get<0>(tup)) {
					p->report(error(error::kind::KIND_ERROR, "Invalid identifier here.", stream, 0));
					_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_IDENTIFIER, tok, s, make_shared<type>(type::kind::KIND_NONE,
						type::const_kind::KIND_NONE, type::static_kind::KIND_NONE), false, stream, value_kind::VALUE_NONE);
					_valid = false;
					return;
				}
				value_kind vk;
				shared_ptr<type> t = nullptr;
				if (s->symbol_kind() == symbol::kind::KIND_VARIABLE)
					t = static_pointer_cast<variable_symbol>(s)->variable_type(), vk = value_kind::VALUE_LVALUE;
				else if (s->symbol_kind() == symbol::kind::KIND_FUNCTION)
					t = static_pointer_cast<function_symbol>(s)->function_symbol_type(), vk = value_kind::VALUE_RVALUE;
				else if (s->symbol_kind() == symbol::kind::KIND_STRUCT) {
					p->report(error(error::kind::KIND_ERROR, "A raw struct type cannot be part of an expression.", stream, 0));
					_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_IDENTIFIER, tok, s, make_shared<type>(type::kind::KIND_NONE,
						type::const_kind::KIND_NONE, type::static_kind::KIND_NONE), false, stream, value_kind::VALUE_NONE);
					_valid = false;
					return;
				}
				if (t->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR)
					vk = value_kind::VALUE_RVALUE;
				_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_IDENTIFIER, tok, s, t, true, stream, vk);
				_valid = true;
			}
			else if (tok.token_kind() == token::kind::TOKEN_OPEN_BRACKET) {
				vector<token> stream;
				stream.push_back(tok);
				vector<shared_ptr<assignment_expression>> array_initializer;
				while (p->peek().token_kind() != token::kind::TOKEN_CLOSE_BRACKET) {
					shared_ptr<assignment_expression> e = expression_parser_helper::parse_assignment_expression(p);
					if (!e->valid()) {
						p->report(error(error::kind::KIND_ERROR, "Expected a valid expression to initialize an array.", stream, 0));
						_valid = false;
						_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_ARRAY_INITIALIZER, array_initializer, nullptr,
							false, stream);
						return;
					}
					vector<token> e_stream = e->stream();
					array_initializer.push_back(e);
					stream.insert(stream.end(), e_stream.begin(), e_stream.end());
					if (p->peek().token_kind() == token::kind::TOKEN_COMMA)
						stream.push_back(p->pop());
				}
				if (p->peek().token_kind() != token::kind::TOKEN_CLOSE_BRACKET) {
					p->report(error(error::kind::KIND_ERROR, "Expected a close bracket (']') to finish an array initializer.", stream, 0));
					_valid = false;
					_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_ARRAY_INITIALIZER, array_initializer, nullptr, false,
						stream);
					return;
				}
				stream.push_back(p->pop());
				shared_ptr<type> t = deduce_array_initializer_type(p, array_initializer, stream);
				_valid = t->valid();
				_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_ARRAY_INITIALIZER, array_initializer, t, t->valid(),
					stream);
			}
			else if (tok.token_kind() == token::kind::TOKEN_NEW || tok.token_kind() == token::kind::TOKEN_STK || tok.token_kind() == token::kind::TOKEN_RESV) {
				primary_expression::kind which = tok.token_kind() == token::kind::TOKEN_NEW ? primary_expression::kind::KIND_NEW : 
					(tok.token_kind() == token::kind::TOKEN_RESV ? primary_expression::kind::KIND_RESV : primary_expression::kind::KIND_STK);
				vector<token> stream;
				stream.push_back(tok);
				type_parser tp(p);
				shared_ptr<type> nt = tp.contained_type();
				if (!tp.valid() || !nt->valid() || nt->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
					p->report(error(error::kind::KIND_ERROR, "Expected a valid type to allocate.", stream, stream.size() - 1));
					_contained_primary_expression = make_shared<primary_expression>(which, nt, nullptr, make_shared<type>(type::kind::KIND_NONE,
						type::const_kind::KIND_NONE, type::static_kind::KIND_NONE), false, stream);
					_valid = false;
					return;
				}
				vector<token> nt_stream = tp.stream();
				stream.insert(stream.end(), nt_stream.begin(), nt_stream.end());
				if (nt->type_kind() == type::kind::KIND_PRIMITIVE && static_pointer_cast<primitive_type>(nt)->primitive_type_kind() == primitive_type::kind::KIND_VOID) {
					p->report(error(error::kind::KIND_ERROR, "Cannot allocate a void type.", nt_stream, 0));
					_contained_primary_expression = make_shared<primary_expression>(which, nt, nullptr, make_shared<type>(type::kind::KIND_NONE,
						type::const_kind::KIND_NONE, type::static_kind::KIND_NONE), false, stream);
					_valid = false;
					return;
				}
				stream.push_back(p->pop());
				shared_ptr<expression> e = expression_parser(p).contained_expression();
				if (!e->valid()) {
					p->report(error(error::kind::KIND_ERROR, "Expected a valid expression for the amount of memory to allocate.", stream, 0));
					_contained_primary_expression = make_shared<primary_expression>(which, nt, e, make_shared<type>(type::kind::KIND_NONE,
						type::const_kind::KIND_NONE, type::static_kind::KIND_NONE), false, stream);
					_valid = false;
					return;
				}
				if (!(e->expression_type()->type_kind() == type::kind::KIND_PRIMITIVE && e->expression_type()->type_array_kind() == type::array_kind::KIND_NON_ARRAY)) {
					p->report(error(error::kind::KIND_ERROR, "Expected an integral expression for the amount of memory to allocate.", stream, 0));
					_contained_primary_expression = make_shared<primary_expression>(which, nt, e, make_shared<type>(type::kind::KIND_NONE,
						type::const_kind::KIND_NONE, type::static_kind::KIND_NONE), false, stream);
					_valid = false;
					return;
				}
				shared_ptr<primitive_type> prim_e_type = static_pointer_cast<primitive_type>(e->expression_type());
				if (prim_e_type->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE || prim_e_type->primitive_type_kind() == primitive_type::kind::KIND_FLOAT ||
					prim_e_type->primitive_type_kind() == primitive_type::kind::KIND_VOID || prim_e_type->primitive_type_kind() == primitive_type::kind::KIND_BOOL) {
					p->report(error(error::kind::KIND_ERROR, "Expected an integral expression for the amount of memory to allocate.", stream, 0));
					_contained_primary_expression = make_shared<primary_expression>(which, nt, e, make_shared<type>(type::kind::KIND_NONE,
						type::const_kind::KIND_NONE, type::static_kind::KIND_NONE), false, stream);
					_valid = false;
					return;
				}
				if (which == primary_expression::kind::KIND_STK || which == primary_expression::kind::KIND_RESV) {
					bool c = true;
					for (shared_ptr<assignment_expression> ae : e->assignment_expression_list())
						c = c && is_constant_expression(p, ae);
					if (!c) {
						p->report(error(error::kind::KIND_ERROR, "Expected a constant expression for the amount of stack or static space to allocate.", stream, 0));
						_contained_primary_expression = make_shared<primary_expression>(which, nt, e, make_shared<type>(type::kind::KIND_NONE,
							type::const_kind::KIND_NONE, type::static_kind::KIND_NONE), false, stream);
						_valid = false;
						return;
					}
				}
				vector<token> e_stream = e->stream();
				stream.insert(stream.end(), e_stream.begin(), e_stream.end());
				if (p->peek().token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
					p->report(error(error::kind::KIND_ERROR, "Expected a close parenthesis (')') to end the declaration of how much memory to allocate.", stream, 0));
					_contained_primary_expression = make_shared<primary_expression>(which, nt, e, make_shared<type>(type::kind::KIND_NONE,
						type::const_kind::KIND_NONE, type::static_kind::KIND_NONE), false, stream);
					_valid = false;
					return;
				}
				stream.push_back(p->pop());
				shared_ptr<type> pet = nullptr;
				if (nt->type_kind() == type::kind::KIND_PRIMITIVE) {
					shared_ptr<primitive_type> pnt = static_pointer_cast<primitive_type>(nt);
					pet = make_shared<primitive_type>(pnt->primitive_type_kind(), pnt->type_const_kind(), pnt->type_static_kind(), pnt->primitive_type_sign_kind(),
						pnt->array_dimensions() + 1);
				}
				else if (nt->type_kind() == type::kind::KIND_STRUCT) {
					shared_ptr<struct_type> snt = static_pointer_cast<struct_type>(nt);
					pet = make_shared<struct_type>(snt->type_const_kind(), snt->type_static_kind(), snt->struct_name(), snt->struct_reference_number(), snt->valid(),
						snt->array_dimensions() + 1);
				}
				else if (nt->type_kind() == type::kind::KIND_FUNCTION) {
					shared_ptr<function_type> fnt = static_pointer_cast<function_type>(nt);
					pet = make_shared<function_type>(fnt->type_const_kind(), fnt->type_static_kind(), fnt->return_type(), fnt->parameter_list(), fnt->function_reference_number(),
						fnt->array_dimensions() + 1);
				}
				else
					p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				_contained_primary_expression = make_shared<primary_expression>(which, nt, e, pet, true, stream);
				_valid = true;
			}
			else {
				_contained_primary_expression = make_shared<primary_expression>(primary_expression::kind::KIND_NONE, tok,
					make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE), false, vector<token>{ tok }, value_kind::VALUE_RVALUE);
				_valid = false;
				p->backtrack();
			}
		}

		primary_expression_parser::~primary_expression_parser() {

		}

		shared_ptr<primary_expression> primary_expression_parser::contained_primary_expression() {
			return _contained_primary_expression;
		}

		bool primary_expression_parser::valid() {
			return _valid;
		}

		postfix_expression_parser::postfix_expression_parser(shared_ptr<parser> p) : _contained_postfix_expression(nullptr), _valid(false) {
			primary_expression_parser pep(p);
			shared_ptr<primary_expression> pe = pep.contained_primary_expression();
			if (!pep.valid()) {
				_contained_postfix_expression = make_shared<postfix_expression>(vector<shared_ptr<postfix_expression::postfix_type>>{}, pe,
					pe->primary_expression_type(), false, pe->stream(), pe->primary_expression_value_kind());
				_valid = false;
				return;
			}
			token op = p->peek();
			vector<shared_ptr<postfix_expression::postfix_type>> ptl;
			shared_ptr<type> prev_type = pe->primary_expression_type();
			value_kind prev_vk = pe->primary_expression_value_kind();
			vector<token> stream = pe->stream();
			shared_ptr<symbol> last_symbol = nullptr;
			if (pe->primary_expression_kind() == primary_expression::kind::KIND_IDENTIFIER)
				last_symbol = pe->identifier_symbol();
			_valid = true;
			while (true) {
				if (op.token_kind() == token::kind::TOKEN_INCREMENT || op.token_kind() == token::kind::TOKEN_DECREMENT) {
					stream.push_back(p->pop());
					prev_type = deduce_postfix_expression_type(p, prev_type, prev_vk, op, stream);
					shared_ptr<postfix_expression::postfix_type> pt = make_shared<postfix_expression::postfix_type>(
						op.token_kind() == token::kind::TOKEN_INCREMENT ? postfix_expression::kind::KIND_INCREMENT :
						postfix_expression::kind::KIND_DECREMENT, prev_type);
					if (prev_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
						p->report(error(error::kind::KIND_ERROR, "Cannot increment or decrement a 'constexpr' expression,", stream, 0));
						ptl.push_back(pt);
						_valid = false;
						break;
					}
					prev_vk = deduce_postfix_expression_value_kind(p, pt);
					_valid = _valid && prev_type->valid() && prev_vk != value_kind::VALUE_NONE;
					ptl.push_back(pt);
				}
				else if (op.token_kind() == token::kind::TOKEN_DOLLAR_SIGN) {
					stream.push_back(p->pop());
					if (prev_vk != value_kind::VALUE_LVALUE) {
						p->report(error(error::kind::KIND_ERROR, "Expected an l-value to take an address of.", stream, 0));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_ADDRESS, nullptr));
						break;
					}
					if (prev_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
						p->report(error(error::kind::KIND_ERROR, "Cannot take the address of a 'constexpr' expression.", stream, 0));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_ADDRESS, nullptr));
						break;
					}
					if (prev_type->type_kind() == type::kind::KIND_PRIMITIVE) {
						shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(prev_type);
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_VOID) {
							p->report(error(error::kind::KIND_ERROR, "Cannot take the address of a void type.", stream, 0));
							_valid = false;
							ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_ADDRESS, nullptr));
							break;
						}
						prev_type = make_shared<primitive_type>(pt->primitive_type_kind(), pt->type_const_kind(), pt->type_static_kind(), pt->primitive_type_sign_kind(),
							pt->array_dimensions() + 1);
					}
					else if (prev_type->type_kind() == type::kind::KIND_STRUCT) {
						shared_ptr<struct_type> ss = static_pointer_cast<struct_type>(prev_type);
						prev_type = make_shared<struct_type>(ss->type_const_kind(), ss->type_static_kind(), ss->struct_name(), ss->struct_reference_number(), ss->valid(),
							ss->array_dimensions() + 1);
					}
					else if (prev_type->type_kind() == type::kind::KIND_FUNCTION) {
						shared_ptr<function_type> ff = static_pointer_cast<function_type>(prev_type);
						prev_type = make_shared<function_type>(ff->type_const_kind(), ff->type_static_kind(), ff->return_type(), ff->parameter_list(), ff->function_reference_number(),
							ff->array_dimensions() + 1);
					}
					else {
						p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return;
					}
					prev_vk = value_kind::VALUE_RVALUE;
					ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_ADDRESS, prev_type));
				}
				else if (op.token_kind() == token::kind::TOKEN_AT) {
					stream.push_back(p->pop());
					if (prev_type->type_array_kind() != type::array_kind::KIND_ARRAY || prev_type->array_dimensions() == 0) {
						p->report(error(error::kind::KIND_ERROR, "You need an array type to dereference.", stream, 0));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_AT, nullptr));
						break;
					}
					if (prev_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
						p->report(error(error::kind::KIND_ERROR, "Cannot dereference a 'constexpr' expression.", stream, 0));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_AT, nullptr));
						break;
					}
					if (prev_type->type_kind() == type::kind::KIND_PRIMITIVE) {
						shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(prev_type);
						if (pt->primitive_type_kind() == primitive_type::kind::KIND_VOID) {
							p->report(error(error::kind::KIND_ERROR, "Cannot dereference a void type.", stream, 0));
							_valid = false;
							ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_AT, nullptr));
							break;
						}
						prev_type = make_shared<primitive_type>(pt->primitive_type_kind(), pt->type_const_kind(), pt->type_static_kind(), pt->primitive_type_sign_kind(),
							pt->array_dimensions() - 1);
					}
					else if (prev_type->type_kind() == type::kind::KIND_STRUCT) {
						shared_ptr<struct_type> ss = static_pointer_cast<struct_type>(prev_type);
						prev_type = make_shared<struct_type>(ss->type_const_kind(), ss->type_static_kind(), ss->struct_name(), ss->struct_reference_number(), ss->valid(),
							ss->array_dimensions() - 1);
					}
					else if (prev_type->type_kind() == type::kind::KIND_FUNCTION) {
						shared_ptr<function_type> ff = static_pointer_cast<function_type>(prev_type);
						prev_type = make_shared<function_type>(ff->type_const_kind(), ff->type_static_kind(), ff->return_type(), ff->parameter_list(), ff->function_reference_number(),
							ff->array_dimensions() - 1);
					}
					else {
						p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return;
					}
					prev_vk = value_kind::VALUE_LVALUE;
					ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_AT, prev_type));
				}
				else if (op.token_kind() == token::kind::TOKEN_AS) {
					stream.push_back(p->pop());
					type_parser tp(p);
					if (!tp.valid() || !tp.contained_type()->valid()) {
						p->report(error(error::kind::KIND_ERROR, "Cannot cast to this type.", tp.stream(), 0));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_AS, nullptr));
						break;
					}
					shared_ptr<type> to_type = tp.contained_type();
					if(to_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR && prev_type->type_constexpr_kind() == type::constexpr_kind::KIND_NON_CONSTEXPR) {
						p->report(error(error::kind::KIND_ERROR, "Cannot cast from a 'non-constexpr' type to a 'non-constexpr' type.", tp.stream(), 0));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_AS, nullptr));
						break;
					}
					if (to_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
						if (to_type->type_array_kind() != type::array_kind::KIND_ARRAY) {
							if (to_type->type_kind() != type::kind::KIND_PRIMITIVE) {
								p->report(error(error::kind::KIND_ERROR, "Cannot cast to a non-array, constexpr function or struct type from a constexpr type.",
									tp.stream(), 0));
								_valid = false;
								ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_AS, nullptr));
								break;
							}
						}
					}
					vector<token> type_stream = tp.stream();
					stream.insert(stream.end(), type_stream.begin(), type_stream.end());
					shared_ptr<type> from_type = prev_type;
					ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_AS, to_type));
					prev_type = to_type;
					prev_vk = value_kind::VALUE_RVALUE;
				}
				else if (op.token_kind() == token::kind::TOKEN_ARROW) {
					stream.push_back(p->pop());
					if (p->peek().token_kind() != token::kind::TOKEN_IDENTIFIER) {
						p->report(error(error::kind::KIND_ERROR, "Expected an identifier to point to a struct a member.", stream, 0));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_ARROW, bad_token, nullptr));
						break;
					}
					token ident = p->pop();
					stream.push_back(ident);
					if (prev_type->array_dimensions() != 1 || prev_type->type_array_kind() != type::array_kind::KIND_ARRAY) {
						p->report(error(error::kind::KIND_ERROR, "'->' should be used to select members from array types.", stream, 0));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_ARROW, ident, nullptr));
						break;
					}
					if (prev_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
						p->report(error(error::kind::KIND_ERROR, "'->' cannot be used on a constexpr expression.", stream, 0));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_ARROW, ident, nullptr));
						break;
					}
					if (prev_type->type_kind() != type::kind::KIND_STRUCT) {
						p->report(error(error::kind::KIND_ERROR, "Expected a struct type for the '->' operator.", stream, stream.size() - 1));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_ARROW, ident, nullptr));
						break;
					}
					shared_ptr<struct_type> st = static_pointer_cast<struct_type>(prev_type);
					shared_ptr<struct_symbol> s_sym = p->find_struct_symbol(st->struct_name(), st->struct_reference_number());
					if (s_sym == nullptr) {
						p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_ARROW, ident, nullptr));
						break;
					}
					if (s_sym->struct_symbol_defined_kind() != struct_stmt::defined_kind::KIND_DEFINED) {
						p->report(error(error::kind::KIND_ERROR, "Cannot select members of incomplete types.", stream, stream.size() - 1));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_ARROW, ident, nullptr));
						break;
					}
					last_symbol = s_sym->struct_symbol_scope()->find_symbol(ident, true);
					if (last_symbol == nullptr) {
						p->report(error(error::kind::KIND_ERROR, "This is not a member of the given structure type.", stream, stream.size() - 1));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_ARROW, ident, nullptr));
						break;
					}
					if (last_symbol->symbol_kind() == symbol::kind::KIND_STRUCT)
						prev_type = static_pointer_cast<struct_symbol>(last_symbol)->struct_symbol_type();
					else if (last_symbol->symbol_kind() == symbol::kind::KIND_VARIABLE)
						prev_type = static_pointer_cast<variable_symbol>(last_symbol)->variable_type();
					else {
						p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return;
					}
					prev_vk = value_kind::VALUE_LVALUE;
					ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_ARROW, ident, prev_type));
				}
				else if (op.token_kind() == token::kind::TOKEN_DOT) {
					stream.push_back(p->pop());
					if (p->peek().token_kind() != token::kind::TOKEN_IDENTIFIER) {
						p->report(error(error::kind::KIND_ERROR, "Expected an identifier to select a struct member.", stream, 0));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_MEMBER, bad_token, nullptr));
						break;
					}
					token ident = p->pop();
					stream.push_back(ident);
					if (prev_type->array_dimensions() != 0 || prev_type->type_array_kind() != type::array_kind::KIND_NON_ARRAY) {
						p->report(error(error::kind::KIND_ERROR, "Can only select members from non-array types.", stream, 0));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_MEMBER, ident, nullptr));
						break;
					}
					if (prev_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
						p->report(error(error::kind::KIND_ERROR, "'.' cannot be used to select from a 'constexpr' expression.", stream, 0));
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_MEMBER, ident, nullptr));
						_valid = false;
						break;
					}
					if (prev_type->type_kind() != type::kind::KIND_STRUCT) {
						p->report(error(error::kind::KIND_ERROR, "Expected a struct type to select membership from.", stream, stream.size() - 1));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_MEMBER, ident, nullptr));
						break;
					}
					shared_ptr<struct_type> st = static_pointer_cast<struct_type>(prev_type);
					shared_ptr<struct_symbol> s_sym = p->find_struct_symbol(st->struct_name(), st->struct_reference_number());
					if (s_sym == nullptr) {
						p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_MEMBER, ident, nullptr));
						break;
					}
					if (s_sym->struct_symbol_defined_kind() != struct_stmt::defined_kind::KIND_DEFINED) {
						p->report(error(error::kind::KIND_ERROR, "Cannot select members of incomplete types.", stream, stream.size() - 1));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_MEMBER, ident, nullptr));
						break;
					}
					last_symbol = s_sym->struct_symbol_scope()->find_symbol(ident, true);
					if (last_symbol == nullptr) {
						p->report(error(error::kind::KIND_ERROR, "This is not a member of the given structure type.", stream, stream.size() - 1));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_MEMBER, ident, nullptr));
						break;
					}
					if (last_symbol->symbol_kind() == symbol::kind::KIND_STRUCT)
						prev_type = static_pointer_cast<struct_symbol>(last_symbol)->struct_symbol_type();
					else if (last_symbol->symbol_kind() == symbol::kind::KIND_VARIABLE)
						prev_type = static_pointer_cast<variable_symbol>(last_symbol)->variable_type();
					else {
						p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return;
					}
					prev_vk = value_kind::VALUE_LVALUE;
					ptl.push_back(make_shared<postfix_expression::postfix_type>(postfix_expression::kind::KIND_MEMBER, ident, prev_type));
				}
				else if (op.token_kind() == token::kind::TOKEN_OPEN_PARENTHESIS) {
					stream.push_back(p->pop());
					vector<shared_ptr<assignment_expression>> al;
					if (p->peek().token_kind() == token::kind::TOKEN_CLOSE_PARENTHESIS)
						stream.push_back(p->pop());
					else {
						shared_ptr<assignment_expression> ae = expression_parser_helper::parse_assignment_expression(p);
						if (!ae->valid()) {
							p->report(error(error::kind::KIND_ERROR, "Expected a valid expression to for a function argument.", stream, stream.size() - 1));
							_valid = false;
							ptl.push_back(make_shared<postfix_expression::postfix_type>(prev_type, al, nullptr));
							break;
						}
						vector<token> ae_stream = ae->stream();
						stream.insert(stream.end(), ae_stream.begin(), ae_stream.end());
						al.push_back(ae);
						while (p->peek().token_kind() == token::kind::TOKEN_COMMA) {
							stream.push_back(p->pop());
							shared_ptr<assignment_expression> arg = expression_parser_helper::parse_assignment_expression(p);
							if (!arg->valid()) {
								p->report(error(error::kind::KIND_ERROR, "Expected a valid expression for a function argument.", stream, stream.size() - 1));
								_valid = false;
								ptl.push_back(make_shared<postfix_expression::postfix_type>(prev_type, al, nullptr));
								break;
							}
							vector<token> arg_stream = arg->stream();
							stream.insert(stream.end(), arg_stream.begin(), arg_stream.end());
							al.push_back(arg);
						}
						if (!_valid)
							break;
						if (p->peek().token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
							p->report(error(error::kind::KIND_ERROR, "Expected a close parenthesis (')') at the end of a function argument list.", stream, stream.size() - 1));
							_valid = false;
							ptl.push_back(make_shared<postfix_expression::postfix_type>(prev_type, al, nullptr));
							break;
						}
						stream.push_back(p->pop());
					}
					if (prev_type->type_kind() != type::kind::KIND_FUNCTION || prev_type->array_dimensions() != 0 ||
						prev_type->type_array_kind() == type::array_kind::KIND_ARRAY) {
						p->report(error(error::kind::KIND_ERROR, "Expected a raw function type for a function call.", stream, stream.size() - 1));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(prev_type, al, nullptr));
						break;
					}
					else if (prev_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
						p->report(error(error::kind::KIND_ERROR, "Cannot use a 'constexpr' expression as a function.", stream, stream.size() - 1));
						_valid = false;
						ptl.push_back(make_shared<postfix_expression::postfix_type>(prev_type, al, nullptr));
						break;
					}
					else {
						shared_ptr<function_type> t = static_pointer_cast<function_type>(prev_type);
						if (t->parameter_list().size() != al.size()) {
							p->report(error(error::kind::KIND_ERROR, "The argument number at the call site does not match the declared number of parameters.",
								stream, 0));
							_valid = false;
							ptl.push_back(make_shared<postfix_expression::postfix_type>(prev_type, al, nullptr));
							break;
						}
						for (int i = 0; i < al.size(); i++) {
							shared_ptr<type> par_type = t->parameter_list()[i]->variable_declaration_type(),
								arg_type = al[i]->assignment_expression_type();
							if (par_type->type_kind() != arg_type->type_kind() || par_type->type_array_kind() != arg_type->type_array_kind() ||
								par_type->array_dimensions() != arg_type->array_dimensions()) {
								p->report(error(error::kind::KIND_ERROR, "Incompatible types between the function parameter and the function call.", stream, 0));
								_valid = false;
								break;
							}
							if (par_type->type_array_kind() == type::array_kind::KIND_ARRAY) {
								if (arg_type->type_const_kind() == type::const_kind::KIND_CONST && par_type->type_const_kind() == type::const_kind::KIND_NON_CONST) {
									p->report(error(error::kind::KIND_ERROR, "Incompatible types between the function parameter and the function call.", stream, 0));
									_valid = false;
									break;
								}
							}
							if (par_type->type_kind() == type::kind::KIND_PRIMITIVE && !primitive_types_compatible(p, par_type, arg_type)) {
								p->report(error(error::kind::KIND_ERROR, "Expected compatible types between a function parameter and the given function argument.",
									stream, 0));
								_valid = false;
								break;
							}
							else if (par_type->type_kind() == type::kind::KIND_STRUCT) {
								shared_ptr<struct_type> ps = static_pointer_cast<struct_type>(par_type), as = static_pointer_cast<struct_type>(arg_type);
								if (ps->struct_reference_number() != as->struct_reference_number() || ps->struct_name().raw_text() != as->struct_name().raw_text()) {
									p->report(error(error::kind::KIND_ERROR, "Expected matching struct types between a function parameter and the given function argument.",
										stream, 0));
									_valid = false;
									break;
								}
							}
							else if (par_type->type_kind() == type::kind::KIND_FUNCTION) {
								if (!matching_function_types(p, par_type, arg_type)) {
									p->report(error(error::kind::KIND_ERROR, "Expected matching function types between a function parameters and a given function argument.",
										stream, 0));
									_valid = false;
									break;
								}
							}
						}
						ptl.push_back(make_shared<postfix_expression::postfix_type>(prev_type, al, t->return_type()));
						prev_type = t->return_type();
						prev_vk = value_kind::VALUE_RVALUE;
					}
				}
				else if (op.token_kind() == token::kind::TOKEN_OPEN_BRACKET) {
					stream.push_back(p->pop());
					shared_ptr<expression> s = expression_parser(p).contained_expression();
					if (!s->valid()) {
						p->report(error(error::kind::KIND_ERROR, "Expected a valid expression to subscript for.", stream, 0));
						_valid = false;
						break;
					}
					vector<token> s_stream = s->stream();
					stream.insert(stream.end(), s_stream.begin(), s_stream.end());
					if (p->peek().token_kind() != token::kind::TOKEN_CLOSE_BRACKET) {
						p->report(error(error::kind::KIND_ERROR, "Expected a close bracket (']') after a subscript expression.", stream, stream.size() - 1));
						_valid = false;
						break;
					}
					stream.push_back(p->pop());
					shared_ptr<type> subscript_type = s->expression_type();
					if (subscript_type->array_dimensions() > 0 || subscript_type->type_array_kind() == type::array_kind::KIND_ARRAY ||
						subscript_type->type_kind() != type::kind::KIND_PRIMITIVE) {
						p->report(error(error::kind::KIND_ERROR, "Expected an integer for a subscript expression.", s_stream, 0));
						_valid = false;
						break;
					}
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(subscript_type);
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_BOOL || pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE ||
						pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT || pt->primitive_type_kind() == primitive_type::kind::KIND_LONG ||
						pt->primitive_type_kind() == primitive_type::kind::KIND_VOID) {
						p->report(error(error::kind::KIND_ERROR, "Expected an integral expression to subscript for.", s_stream, 0));
						_valid = false;
						break;
					}
					if (prev_type->type_array_kind() != type::array_kind::KIND_ARRAY || prev_type->array_dimensions() == 0) {
						p->report(error(error::kind::KIND_ERROR, "Expected to subscript an array.", stream, 0));
						_valid = false;
						break;
					}
					if (prev_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
						p->report(error(error::kind::KIND_ERROR, "Cannot index into a 'constexpr' expression.", stream, 0));
						_valid = false;
						break;
					}
					if (prev_type->type_kind() == type::kind::KIND_PRIMITIVE) {
						shared_ptr<primitive_type> prev_pt = static_pointer_cast<primitive_type>(prev_type);
						prev_type = make_shared<primitive_type>(prev_pt->primitive_type_kind(), prev_pt->type_const_kind(), prev_pt->type_static_kind(),
							prev_pt->primitive_type_sign_kind(), prev_pt->array_dimensions() - 1);
					}
					else if (prev_type->type_kind() == type::kind::KIND_STRUCT) {
						shared_ptr<struct_type> prev_st = static_pointer_cast<struct_type>(prev_type);
						prev_type = make_shared<struct_type>(prev_st->type_const_kind(), prev_st->type_static_kind(), prev_st->struct_name(), prev_st->struct_reference_number(),
							prev_st->valid(), prev_st->array_dimensions() - 1);
					}
					else if (prev_type->type_kind() == type::kind::KIND_FUNCTION) {
						shared_ptr<function_type> ff = static_pointer_cast<function_type>(prev_type);
						prev_type = make_shared<function_type>(ff->type_const_kind(), ff->type_static_kind(), ff->return_type(), ff->parameter_list(), ff->function_reference_number(),
							ff->array_dimensions() - 1);
					}
					else {
						p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						_valid = false;
						break;
					}
					prev_vk = value_kind::VALUE_LVALUE;
					ptl.push_back(make_shared<postfix_expression::postfix_type>(s, prev_type));
				}
				else
					break;
				op = p->peek();
			}
			_contained_postfix_expression = make_shared<postfix_expression>(ptl, pe, prev_type, _valid, stream, prev_vk);
		}

		postfix_expression_parser::~postfix_expression_parser() {

		}

		shared_ptr<postfix_expression> postfix_expression_parser::contained_postfix_expression() {
			return _contained_postfix_expression;
		}

		bool postfix_expression_parser::valid() {
			return _valid;
		}

		unary_expression_parser::unary_expression_parser(shared_ptr<parser> p) : _contained_unary_expression(nullptr), _valid(false) {
			token op = p->peek();
			vector<unary_expression::kind> uekl;
			unary_expression::kind uek = unary_expression::kind::KIND_NONE;
			vector<token> stream;
			while (true) {
				unary_expression::kind inner = unary_expression::kind::KIND_NONE;
				if (op.token_kind() == token::kind::TOKEN_INCREMENT) inner = unary_expression::kind::KIND_INCREMENT;
				else if (op.token_kind() == token::kind::TOKEN_DECREMENT) inner = unary_expression::kind::KIND_DECREMENT;
				else if (op.token_kind() == token::kind::TOKEN_PLUS) inner = unary_expression::kind::KIND_PLUS;
				else if (op.token_kind() == token::kind::TOKEN_MINUS) inner = unary_expression::kind::KIND_MINUS;
				else if (op.token_kind() == token::kind::TOKEN_EXCALAMATION) inner = unary_expression::kind::KIND_LOGICAL_NOT;
				else if (op.token_kind() == token::kind::TOKEN_TILDE) inner = unary_expression::kind::KIND_BITWISE_NOT;
				if (inner == unary_expression::kind::KIND_NONE)
					break;
				else {
					uek = inner;
					uekl.push_back(inner);
					stream.push_back(p->pop());
					op = p->peek();
				}
			}
			reverse(uekl.begin(), uekl.end());
			postfix_expression_parser pep(p);
			shared_ptr<postfix_expression> pe = pep.contained_postfix_expression();
			vector<token> pe_stream = pe->stream();
			stream.insert(stream.end(), pe_stream.begin(), pe_stream.end());
			shared_ptr<type> prev_type = pe->postfix_expression_type();
			value_kind prev_vk = pe->postfix_expression_value_kind();
			_valid = pe->valid();
			for (unary_expression::kind uek : uekl) {
				prev_type = deduce_unary_expression_type(p, prev_type, prev_vk, uek, stream);
				prev_vk = deduce_unary_expression_value_kind(p, uek);
				_valid = _valid && prev_type->valid();
			}
			_contained_unary_expression = make_shared<unary_expression>(uekl, pe, prev_type, _valid, stream, prev_vk, uek);
		}

		unary_expression_parser::~unary_expression_parser() {

		}

		shared_ptr<unary_expression> unary_expression_parser::contained_unary_expression() {
			return _contained_unary_expression;
		}

		bool unary_expression_parser::valid() {
			return _valid;
		}

		ternary_expression_parser::ternary_expression_parser(shared_ptr<parser> p) : _contained_ternary_expression(nullptr), _valid(false) {
			vector<token> stream;
			shared_ptr<binary_expression> bexpr = expression_parser_helper::parse_logical_or_expression(p);
			for (token t : bexpr->stream()) stream.push_back(t);
			token op = p->peek();
			if (op.token_kind() != token::kind::TOKEN_QUESTION_MARK || !bexpr->valid()) {
				_contained_ternary_expression = make_shared<ternary_expression>(bexpr->binary_expression_type(), bexpr, bexpr->stream(), bexpr->valid(),
					bexpr->binary_expression_value_kind());
				_valid = _contained_ternary_expression->valid();
				return;
			}
			p->pop();
			stream.push_back(op);
			expression_parser ep(p);
			shared_ptr<expression> e = ep.contained_expression();
			for (token t : e->stream()) stream.push_back(t);
			if (!e->valid()) {
				_contained_ternary_expression = make_shared<ternary_expression>(make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE), bexpr,
					e, nullptr, stream, false, value_kind::VALUE_NONE);
				_valid = false;
				return;
			}
			token op2 = p->peek();
			if (op2.token_kind() != token::kind::TOKEN_COLON) {
				_contained_ternary_expression = make_shared<ternary_expression>(make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE), bexpr,
					e, nullptr, stream, false, value_kind::VALUE_NONE);
				_valid = false;
				return;
			}
			p->pop();
			stream.push_back(op2);
			ternary_expression_parser tep(p);
			shared_ptr<ternary_expression> te = tep.contained_ternary_expression();
			for (token t : te->stream()) stream.push_back(t);
			if (!te->valid()) {
				_contained_ternary_expression = make_shared<ternary_expression>(make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE), bexpr,
					e, te, stream, false, value_kind::VALUE_NONE);
				_valid = false;
				return;
			}
			shared_ptr<type> t = deduce_ternary_expression_type(p, bexpr, op, e, op2, te);
			value_kind vk = deduce_ternary_expression_value_kind(p, bexpr, e, te);
			_contained_ternary_expression = make_shared<ternary_expression>(t, bexpr, e, te, stream, t->valid(), vk);
			_valid = _contained_ternary_expression->valid();
		}

		ternary_expression_parser::~ternary_expression_parser() {

		}

		shared_ptr<ternary_expression> ternary_expression_parser::contained_ternary_expression() {
			return _contained_ternary_expression;
		}

		bool ternary_expression_parser::valid() {
			return _valid;
		}

		expression_parser::expression_parser(shared_ptr<parser> p) : _contained_expression(nullptr), _valid(false) {
			vector<shared_ptr<assignment_expression>> assignment_expression_list;
			shared_ptr<assignment_expression> first = expression_parser_helper::parse_assignment_expression(p);
			vector<token> stream;
			for (token t : first->stream()) stream.push_back(t);
			if (!first->valid()) {
				_contained_expression = make_shared<expression>(make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE),
					assignment_expression_list, first->stream(), false, first->assignment_expression_value_kind());
				_valid = false;
				return;
			}
			assignment_expression_list.push_back(first);
			token op = p->peek();
			bool all_constexpr = true;
			while (op.token_kind() == token::kind::TOKEN_COMMA) {
				stream.push_back(op);
				p->pop();
				shared_ptr<assignment_expression> ae = expression_parser_helper::parse_assignment_expression(p);
				for (token t : ae->stream()) stream.push_back(t);
				if (!ae->valid()) {
					_contained_expression = make_shared<expression>(make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE),
						assignment_expression_list, assignment_expression_list[assignment_expression_list.size() - 1]->stream(), false,
						assignment_expression_list[assignment_expression_list.size() - 1]->assignment_expression_value_kind());
					_valid = false;
					return;
				}
				all_constexpr = all_constexpr && ae->assignment_expression_type()->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR;
				assignment_expression_list.push_back(ae);
				op = p->peek();
			}
			_contained_expression = make_shared<expression>(assignment_expression_list[assignment_expression_list.size() - 1]->assignment_expression_type(), assignment_expression_list,
				stream, true, assignment_expression_list[assignment_expression_list.size() - 1]->assignment_expression_value_kind());
			if (!all_constexpr)
				_contained_expression->expression_type()->set_constexpr_kind(type::constexpr_kind::KIND_NON_CONSTEXPR);
			_valid = true;
		}

		expression_parser::~expression_parser() {

		}

		shared_ptr<expression> expression_parser::contained_expression() {
			return _contained_expression;
		}

		bool expression_parser::valid() {
			return _valid;
		}

		stmt_parser::stmt_parser(shared_ptr<parser> p) :
			_contained_stmt(nullptr), _valid(false) {
			int start = p->get_buffer_position();
			vector<token> stream;
			if (p->peek().token_kind() == token::kind::TOKEN_IF) {
				shared_ptr<if_stmt> i = if_stmt_parser(p).contained_if_stmt();
				if (i != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_IF, i, i->valid(), i->stream());
					stream = i->stream();
					_valid = i->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_DELETE) {
				shared_ptr<delete_stmt> d = delete_stmt_parser(p).contained_delete_stmt();
				if (d != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_DELETE, d, d->valid(), d->stream());
					stream = d->stream();
					_valid = d->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_ACCESS) {
				shared_ptr<access_stmt> a = access_stmt_parser(p).contained_access_stmt();
				if (a != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_ACCESS, a, a->valid(), a->stream());
					stream = a->stream();
					_valid = a->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_INCLUDE || p->peek().token_kind() == token::kind::TOKEN_IMPORT) {
				shared_ptr<include_stmt> i = include_stmt_parser(p).contained_include_stmt();
				if (i != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_INCLUDE, i, i->valid(), i->stream());
					stream = i->stream();
					_valid = i->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_NAMESPACE) {
				shared_ptr<namespace_stmt> n = namespace_stmt_parser(p).contained_namespace_stmt();
				if (n != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_NAMESPACE, n, n->valid(), n->stream());
					stream = n->stream();
					_valid = n->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_ASM) {
				shared_ptr<asm_stmt> a = asm_stmt_parser(p).contained_asm_stmt();
				if(a != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_ASM, a, a->valid(), a->stream());
					stream = a->stream();
					_valid = a->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_USING) {
				shared_ptr<using_stmt> u = using_stmt_parser(p).contained_using_stmt();
				if (u != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_USING, u, u->valid(), u->stream());
					stream = u->stream();
					_valid = u->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_FOR) {
				shared_ptr<for_stmt> f = for_stmt_parser(p).contained_for_stmt();
				if (f != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_FOR, f, f->valid(), f->stream());
					stream = f->stream();
					_valid = f->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_STRUCT) {
				shared_ptr<struct_stmt> s = struct_stmt_parser(p).contained_struct_stmt();
				if (s != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_STRUCT, s, s->valid(), s->stream());
					stream = s->stream();
					_valid = s->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_SWITCH) {
				shared_ptr<switch_stmt> s = switch_stmt_parser(p).contained_switch_stmt();
				if (s != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_SWITCH, s, s->valid(), s->stream());
					stream = s->stream();
					_valid = s->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_DO) {
				shared_ptr<while_stmt> d = do_while_stmt_parser(p).contained_do_while_stmt();
				if (d != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_DO_WHILE, d, d->valid(), d->stream());
					stream = d->stream();
					_valid = d->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_CASE || p->peek().token_kind() == token::kind::TOKEN_DEFAULT) {
				shared_ptr<case_default_stmt> c = case_default_stmt_parser(p).contained_case_default_stmt();
				if (c != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_CASE_DEFAULT, c, c->valid(), c->stream());
					stream = c->stream();
					_valid = c->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_BREAK || p->peek().token_kind() == token::kind::TOKEN_CONTINUE) {
				shared_ptr<break_continue_stmt> b = break_continue_stmt_parser(p).contained_break_continue_stmt();
				if (b != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_BREAK_CONTINUE, b, b->valid(), b->stream());
					stream = b->stream();
					_valid = b->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_OPEN_BRACE) {
				shared_ptr<block_stmt> b = block_stmt_parser(p).contained_block_stmt();
				if (b != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_BLOCK, b, b->valid(), b->stream());
					stream = b->stream();
					_valid = b->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_WHILE) {
				shared_ptr<while_stmt> w = while_stmt_parser(p).contained_while_stmt();
				if (w != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_WHILE, w, w->valid(), w->stream());
					stream = w->stream();
					_valid = w->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_RETURN) {
				shared_ptr<return_stmt> r = return_stmt_parser(p).contained_return_stmt();
				if (r != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_RETURN, r, r->valid(), r->stream());
					stream = r->stream();
					_valid = r->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_FUNC) {
				shared_ptr<function_stmt> f = function_stmt_parser(p).contained_function_stmt();
				if (f != nullptr) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_FUNCTION, f, f->valid(), f->stream());
					stream = f->stream();
					_valid = f->valid();
				}
				else {
					_contained_stmt = make_shared<stmt>();
					_valid = false;
				}
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_SEMICOLON) {
				shared_ptr<empty_stmt> e = make_shared<empty_stmt>(true, vector<token>{ p->peek() });
				p->report(error(error::kind::KIND_WARNING, "Empty statement, ignoring.", vector<token>{ p->peek() }, 0));
				_contained_stmt = make_shared<stmt>(stmt::kind::KIND_EMPTY, e, true, vector<token>{ p->peek() });
				_valid = true;
				p->pop();
			}
			else {
				shared_ptr<expression> expr = expression_parser(p).contained_expression();
				if (expr != nullptr && expr->stream().size() != 0)
					stream = expr->stream();
				if (expr->valid()) {
					_contained_stmt = make_shared<stmt>(stmt::kind::KIND_EXPRESSION, expr, expr->valid(), expr->stream());
				}
				else {
					p->set_buffer_position(start);
					variable_declaration_parser vp(p);
					vector<shared_ptr<variable_declaration>> decl_list = vp.contained_variable_declaration_list();
					if (vp.stream().size() != 0)
						stream = vp.stream();
					if (vp.valid())
						_contained_stmt = make_shared<stmt>(stmt::kind::KIND_VARIABLE_DECLARATION, decl_list, vp.valid(), vp.stream());
					else
						_contained_stmt = make_shared<stmt>();
				}
				token tok = p->pop();
				_valid = tok.token_kind() == token::kind::TOKEN_SEMICOLON && _contained_stmt->valid();
				if (tok.token_kind() != token::kind::TOKEN_SEMICOLON)
					p->report(error(error::kind::KIND_ERROR, "Expected a semicolon ';'.", vector<token>{ tok }, 0));
				else
					stream.push_back(tok);
				if(_contained_stmt != nullptr && _contained_stmt->valid())
					switch (_contained_stmt->stmt_kind()) {
					case stmt::kind::KIND_EXPRESSION:
						_contained_stmt = make_shared<stmt>(stmt::kind::KIND_EXPRESSION, _contained_stmt->stmt_expression(), true, stream);
						break;
					case stmt::kind::KIND_VARIABLE_DECLARATION:
						_contained_stmt = make_shared<stmt>(stmt::kind::KIND_VARIABLE_DECLARATION, _contained_stmt->stmt_variable_declaration_list(), true, stream);
						break;
					}
			}
			if (_contained_stmt->stmt_kind() == stmt::kind::KIND_NONE || !_valid) {
				p->report(error(error::kind::KIND_ERROR, "Invalid statement starting here.", stream, 0));
				return;
			}
		}

		stmt_parser::~stmt_parser() {

		}

		bool stmt_parser::valid() {
			return _valid;
		}

		shared_ptr<stmt> stmt_parser::contained_stmt() {
			return _contained_stmt;
		}

		namespace expression_parser_helper {

			shared_ptr<binary_expression> parse_multiplicative_expression(shared_ptr<parser> p) {
				unary_expression_parser uep_first(p);
				shared_ptr<binary_expression> lhs = make_shared<binary_expression>(uep_first.contained_unary_expression()->unary_expression_type(),
					uep_first.contained_unary_expression(), uep_first.contained_unary_expression()->stream(),
					uep_first.valid(), uep_first.contained_unary_expression()->unary_expression_value_kind());
				if(!lhs->valid())
					return lhs;
				token op = p->peek();
				while (op.token_kind() == token::kind::TOKEN_SLASH || op.token_kind() == token::kind::TOKEN_STAR || op.token_kind() == token::kind::TOKEN_PERCENT) {
					p->pop();
					binary_expression::operator_kind op_kind = op.token_kind() == token::kind::TOKEN_SLASH ? binary_expression::operator_kind::KIND_DIVIDE :
						(op.token_kind() == token::kind::TOKEN_STAR ? binary_expression::operator_kind::KIND_MULTIPLY : binary_expression::operator_kind::KIND_MODULUS);
					unary_expression_parser uep_second(p);
					shared_ptr<binary_expression> rhs = make_shared<binary_expression>(uep_second.contained_unary_expression()->unary_expression_type(),
						uep_second.contained_unary_expression(), uep_second.contained_unary_expression()->stream(),
						uep_second.valid(), uep_second.contained_unary_expression()->unary_expression_value_kind());
					if(!rhs->valid())
						return lhs;
					vector<token> stream;
					for (token tok : lhs->stream()) stream.push_back(tok);
					stream.push_back(op);
					for (token tok : rhs->stream()) stream.push_back(tok);
					shared_ptr<type> t = deduce_binary_expression_type(p, lhs, op, rhs);
					value_kind vk = deduce_binary_expression_value_kind(p, lhs, op_kind, rhs);
					lhs = make_shared<binary_expression>(t, lhs, op_kind, rhs, stream, rhs->valid() && lhs->valid() && t->valid(), vk);
					if (!lhs->valid())
						return lhs;
					op = p->peek();
				}
				return lhs;
			}

			shared_ptr<binary_expression> parse_left_associative_binary_expression(shared_ptr<parser> p, function<shared_ptr<binary_expression>(shared_ptr<parser>)> base_func, 
				map<token::kind, binary_expression::operator_kind> mapping) {
				shared_ptr<binary_expression> lhs = base_func(p);
				if (!lhs->valid())
					return lhs;
				token op = p->peek();
				while (mapping.find(op.token_kind()) != mapping.end()) {
					p->pop();
					binary_expression::operator_kind op_kind = mapping[op.token_kind()];
					shared_ptr<binary_expression> rhs = base_func(p);
					vector<token> stream;
					for (token tok : lhs->stream()) stream.push_back(tok);
					stream.push_back(op);
					for (token tok : rhs->stream()) stream.push_back(tok);
					shared_ptr<type> t = deduce_binary_expression_type(p, lhs, op, rhs);
					value_kind vk = deduce_binary_expression_value_kind(p, lhs, op_kind, rhs);
					lhs = make_shared<binary_expression>(t, lhs, op_kind, rhs, stream, rhs->valid() && lhs->valid() && t->valid(), vk);
					if (!lhs->valid())
						return lhs;
					op = p->peek();
				}
				return lhs;
			}

			shared_ptr<binary_expression> parse_additive_expression(shared_ptr<parser> p) {
				return parse_left_associative_binary_expression(p, parse_multiplicative_expression, map<token::kind, binary_expression::operator_kind>{
					{ token::kind::TOKEN_PLUS, binary_expression::operator_kind::KIND_ADD },
					{ token::kind::TOKEN_MINUS, binary_expression::operator_kind::KIND_SUBTRACT }
				});
			}

			shared_ptr<binary_expression> parse_bitwise_shift_expression(shared_ptr<parser> p) {
				return parse_left_associative_binary_expression(p, parse_additive_expression, map<token::kind, binary_expression::operator_kind>{
					{ token::kind::TOKEN_SHIFT_LEFT, binary_expression::operator_kind::KIND_SHIFT_LEFT },
					{ token::kind::TOKEN_SHIFT_RIGHT, binary_expression::operator_kind::KIND_SHIFT_RIGHT }
				});
			}

			shared_ptr<binary_expression> parse_relational_comparative_expression(shared_ptr<parser> p) {
				return parse_left_associative_binary_expression(p, parse_bitwise_shift_expression, map<token::kind, binary_expression::operator_kind>{
					{ token::kind::TOKEN_RIGHT_ANGLE_BRACKET, binary_expression::operator_kind::KIND_GREATER_THAN },
					{ token::kind::TOKEN_LEFT_ANGLE_BRACKET, binary_expression::operator_kind::KIND_LESS_THAN },
					{ token::kind::TOKEN_LESS_THAN_OR_EQUAL_TO, binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO },
					{ token::kind::TOKEN_GREATER_THAN_OR_EQUAL_TO, binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO }
				});
			}

			shared_ptr<binary_expression> parse_relational_equality_expression(shared_ptr<parser> p) {
				return parse_left_associative_binary_expression(p, parse_relational_comparative_expression, map<token::kind, binary_expression::operator_kind>{
					{ token::kind::TOKEN_EQUALS_EQUALS, binary_expression::operator_kind::KIND_EQUALS_EQUALS },
					{ token::kind::TOKEN_NOT_EQUALS, binary_expression::operator_kind::KIND_NOT_EQUALS }
				});
			}

			shared_ptr<binary_expression> parse_bitwise_and_expression(shared_ptr<parser> p) {
				return parse_left_associative_binary_expression(p, parse_relational_equality_expression, map<token::kind, binary_expression::operator_kind>{
					{ token::kind::TOKEN_AMPERSAND, binary_expression::operator_kind::KIND_BITWISE_AND }
				});
			}

			shared_ptr<binary_expression> parse_bitwise_xor_expression(shared_ptr<parser> p) {
				return parse_left_associative_binary_expression(p, parse_bitwise_and_expression, map<token::kind, binary_expression::operator_kind>{
					{ token::kind::TOKEN_CARET, binary_expression::operator_kind::KIND_BITWISE_XOR }
				});
			}

			shared_ptr<binary_expression> parse_bitwise_or_expression(shared_ptr<parser> p) {
				return parse_left_associative_binary_expression(p, parse_bitwise_xor_expression, map<token::kind, binary_expression::operator_kind>{
					{ token::kind::TOKEN_BAR, binary_expression::operator_kind::KIND_BITWISE_OR }
				});
			}

			shared_ptr<binary_expression> parse_logical_and_expression(shared_ptr<parser> p) {
				return parse_left_associative_binary_expression(p, parse_bitwise_or_expression, map<token::kind, binary_expression::operator_kind>{
					{ token::kind::TOKEN_LOGICAL_AND, binary_expression::operator_kind::KIND_LOGICAL_AND }
				});
			}

			shared_ptr<binary_expression> parse_logical_or_expression(shared_ptr<parser> p) {
				return parse_left_associative_binary_expression(p, parse_logical_and_expression, map<token::kind, binary_expression::operator_kind>{
					{ token::kind::TOKEN_LOGICAL_OR, binary_expression::operator_kind::KIND_LOGICAL_OR }
				});
			}

			shared_ptr<assignment_expression> parse_assignment_expression(shared_ptr<parser> p) {
				int start = p->get_buffer_position();
				vector<token> stream;
				unary_expression_parser uep(p);
				shared_ptr<unary_expression> ue = uep.contained_unary_expression();
				if (!ue->valid()) {
					p->set_buffer_position(start);
					ternary_expression_parser tep(p);
					shared_ptr<ternary_expression> te = tep.contained_ternary_expression();
					return make_shared<assignment_expression>(te->ternary_expression_type(), te, te->stream(), te->valid(), te->ternary_expression_value_kind());
				}
				for (token t : ue->stream()) stream.push_back(t);
				token op = p->peek();
				binary_expression::operator_kind op_kind = binary_expression::operator_kind::KIND_NONE;
				if (op.token_kind() == token::kind::TOKEN_EQUALS)
					op_kind = binary_expression::operator_kind::KIND_EQUALS;
				else if (op.token_kind() == token::kind::TOKEN_PLUS_EQUALS)
					op_kind = binary_expression::operator_kind::KIND_ADD_EQUALS;
				else if (op.token_kind() == token::kind::TOKEN_MINUS_EQUALS)
					op_kind = binary_expression::operator_kind::KIND_SUBTRACT_EQUALS;
				else if (op.token_kind() == token::kind::TOKEN_MULTIPLY_EQUALS)
					op_kind = binary_expression::operator_kind::KIND_MULTIPLY_EQUALS;
				else if (op.token_kind() == token::kind::TOKEN_DIVIDE_EQUALS)
					op_kind = binary_expression::operator_kind::KIND_DIVIDE_EQUALS;
				else if (op.token_kind() == token::kind::TOKEN_MODULO_EQUALS)
					op_kind = binary_expression::operator_kind::KIND_MODULUS_EQUALS;
				else if (op.token_kind() == token::kind::TOKEN_SHIFT_LEFT_EQUALS)
					op_kind = binary_expression::operator_kind::KIND_SHIFT_LEFT_EQUALS;
				else if (op.token_kind() == token::kind::TOKEN_SHIFT_RIGHT_EQUALS)
					op_kind = binary_expression::operator_kind::KIND_SHIFT_RIGHT_EQUALS;
				else if (op.token_kind() == token::kind::TOKEN_BITWISE_AND_EQUALS)
					op_kind = binary_expression::operator_kind::KIND_BITWISE_AND_EQUALS;
				else if (op.token_kind() == token::kind::TOKEN_BITWISE_OR_EQUALS)
					op_kind = binary_expression::operator_kind::KIND_BITWISE_OR_EQUALS;
				else if (op.token_kind() == token::kind::TOKEN_XOR_EQUALS)
					op_kind = binary_expression::operator_kind::KIND_BITWISE_XOR_EQUALS;
				else if (op.token_kind() == token::kind::TOKEN_LOGICAL_AND_EQUALS)
					op_kind = binary_expression::operator_kind::KIND_LOGICAL_AND_EQUALS;
				else if (op.token_kind() == token::kind::TOKEN_LOGICAL_OR_EQUALS)
					op_kind = binary_expression::operator_kind::KIND_LOGICAL_OR_EQUALS;
				if (op_kind == binary_expression::operator_kind::KIND_NONE) {
					p->set_buffer_position(start);
					ternary_expression_parser tep(p);
					shared_ptr<ternary_expression> te = tep.contained_ternary_expression();
					return make_shared<assignment_expression>(te->ternary_expression_type(), te, te->stream(), te->valid(), te->ternary_expression_value_kind());
				}
				stream.push_back(op);
				p->pop();
				shared_ptr<assignment_expression> ae = parse_assignment_expression(p);
				for (token t : ae->stream()) stream.push_back(t);
				value_kind vk = value_kind::VALUE_LVALUE;
				shared_ptr<type> t = deduce_assignment_expression_type(p, ue, op, ae);
				return make_shared<assignment_expression>(t, ue, op_kind, ae, stream, t->valid() && ae->valid() && ue->valid(), vk);
			}
		}

		shared_ptr<type> deduce_postfix_expression_type(shared_ptr<parser> p, shared_ptr<type> t, value_kind vk, token op, vector<token> stream) {
			if(!t->valid())
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			if (t->type_kind() == type::kind::KIND_PRIMITIVE) {
				switch (op.token_kind()) {
					case token::kind::TOKEN_INCREMENT:
					case token::kind::TOKEN_DECREMENT: {
						shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
						primitive_type::kind pt_kind = pt->primitive_type_kind();
						if (pt->type_array_kind() == type::array_kind::KIND_ARRAY) {
							p->report(error(error::kind::KIND_ERROR, "Cannot increment an array.", stream, 0));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						if (pt_kind == primitive_type::kind::KIND_BOOL || pt_kind == primitive_type::kind::KIND_VOID) {
							p->report(error(error::kind::KIND_ERROR, "Expected numeric types for this operator.", stream, stream.size() - 1));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						if (vk != value_kind::VALUE_LVALUE) {
							p->report(error(error::kind::KIND_ERROR, "Expected an lvalue for this operator.", stream, 0));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						if (t->type_const_kind() == type::const_kind::KIND_CONST) {
							p->report(error(error::kind::KIND_ERROR, "Cannot increment or decrement a constant variable.", stream, 0));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						return t;
					}
					break;
				}
			}
			else if (t->type_kind() == type::kind::KIND_FUNCTION) {
				switch (op.token_kind()) {
				case token::kind::TOKEN_INCREMENT:
				case token::kind::TOKEN_DECREMENT: {
					p->report(error(error::kind::KIND_ERROR, "Cannot increment or decrement a function.", stream, stream.size() - 1));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
					break;
				}
			}
			else if (t->type_kind() == type::kind::KIND_STRUCT) {
				switch (op.token_kind()) {
				case token::kind::TOKEN_INCREMENT:
				case token::kind::TOKEN_DECREMENT: {
					p->report(error(error::kind::KIND_ERROR, "Cannot increment or decrement a 'struct' type.", stream, stream.size() - 1));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
					break;
				}
			}
			p->report_internal("This should unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<type> deduce_unary_expression_type(shared_ptr<parser> p, shared_ptr<type> t, value_kind vk, unary_expression::kind uek, vector<token> stream) {
			if(!t->valid())
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			if (t->type_kind() == type::kind::KIND_PRIMITIVE) {
				shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
				primitive_type::kind pt_kind = pt->primitive_type_kind();
				if ((uek == unary_expression::kind::KIND_INCREMENT || uek == unary_expression::kind::KIND_DECREMENT)
					&& pt->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
					p->report(error(error::kind::KIND_ERROR, "Cannot increment or decrement a 'constexpr' expression.", stream, 0));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE, type::constexpr_kind::KIND_NONE);
				}
				switch (uek) {
				case unary_expression::kind::KIND_PLUS:
				case unary_expression::kind::KIND_MINUS:
				case unary_expression::kind::KIND_INCREMENT:
				case unary_expression::kind::KIND_DECREMENT: {
						if (pt->type_array_kind() == type::array_kind::KIND_ARRAY) {
							p->report(error(error::kind::KIND_ERROR, "Cannot apply a numeric operator to an array type.", stream, 0));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						if (pt_kind == primitive_type::kind::KIND_BOOL || pt_kind == primitive_type::kind::KIND_VOID) {
							p->report(error(error::kind::KIND_ERROR, "Expected a numeric type for this operator.", stream, stream.size() - 1));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						if (vk != value_kind::VALUE_LVALUE && (uek == unary_expression::kind::KIND_INCREMENT || uek == unary_expression::kind::KIND_DECREMENT)) {
							p->report(error(error::kind::KIND_ERROR, "Expected an lvalue for this operator.", stream, 0));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						if (t->type_const_kind() == type::const_kind::KIND_CONST && (uek == unary_expression::kind::KIND_INCREMENT || uek == unary_expression::kind::KIND_DECREMENT)) {
							p->report(error(error::kind::KIND_ERROR, "Cannot increment or decrement a constant variable.", stream, 0));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						return t;
					}
					break;
				case unary_expression::kind::KIND_BITWISE_NOT: {
					if (pt->type_array_kind() == type::array_kind::KIND_ARRAY) {
						p->report(error(error::kind::KIND_ERROR, "Cannot apply a numeric operator to an array type.", stream, 0));
						return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
					}
					if (pt_kind == primitive_type::kind::KIND_BOOL || pt_kind == primitive_type::kind::KIND_VOID ||
						pt_kind == primitive_type::kind::KIND_FLOAT || pt_kind == primitive_type::kind::KIND_DOUBLE) {
						p->report(error(error::kind::KIND_ERROR, "Expected an integral type for this operator.", stream, 0));
						return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
					}
					return t;
				}
					break;
				case unary_expression::kind::KIND_LOGICAL_NOT: {
					if (pt->type_array_kind() == type::array_kind::KIND_ARRAY) {
						p->report(error(error::kind::KIND_ERROR, "Cannot apply a boolean operator to an array type.", stream, 0));
						return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
					}
					if (pt_kind != primitive_type::kind::KIND_BOOL) {
						p->report(error(error::kind::KIND_ERROR, "Expected a boolean operand for this operator.", stream, 0));
						return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
					}
					return t;
				}
					break;
				}
			}
			else if (t->type_kind() == type::kind::KIND_FUNCTION) {
				p->report(error(error::kind::KIND_ERROR, "Cannot apply any unary operators on a function type.", stream, 0));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			}
			else if (t->type_kind() == type::kind::KIND_STRUCT) {
				p->report(error(error::kind::KIND_ERROR, "Cannot apply any unary operators on a struct type.", stream, 0));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			}
			p->report_internal("This should unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<type> deduce_binary_expression_type(shared_ptr<parser> p, shared_ptr<binary_expression> lhs, token op, shared_ptr<binary_expression> rhs) {
			vector<token> stream;
			for (token tok : lhs->stream())
				stream.push_back(tok);
			stream.push_back(op);
			for (token tok : rhs->stream())
				stream.push_back(tok);
			if (!lhs->valid() || !rhs->valid())
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			shared_ptr<type> lhs_type = lhs->binary_expression_type(), rhs_type = rhs->binary_expression_type();
			type::constexpr_kind deduced_constexpr_kind = lhs_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR
				&& rhs_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR ?
				type::constexpr_kind::KIND_CONSTEXPR : type::constexpr_kind::KIND_NON_CONSTEXPR;
			if (lhs_type->type_kind() == type::kind::KIND_FUNCTION || rhs_type->type_kind() == type::kind::KIND_FUNCTION) {
				if (lhs_type->type_kind() != type::kind::KIND_FUNCTION || rhs_type->type_kind() != type::kind::KIND_FUNCTION) {
					p->report(error(error::kind::KIND_ERROR, "Cannot apply any binary operators on a function type and any other type.", stream, lhs->stream().size()));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				if (op.token_kind() != token::kind::TOKEN_EQUALS_EQUALS && op.token_kind() != token::kind::TOKEN_NOT_EQUALS) {
					p->report(error(error::kind::KIND_ERROR, "The only binary operators you can apply on function types are '==' and '!='.", stream, lhs->stream().size()));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				if (!matching_function_types(p, lhs_type, rhs_type)) {
					p->report(error(error::kind::KIND_ERROR, "Expected matching function types to compare for equality or inequality.", stream, lhs->stream().size()));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				return make_shared<primitive_type>(primitive_type::kind::KIND_BOOL, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
					deduced_constexpr_kind, primitive_type::sign_kind::KIND_NONE);
			}
			else if (lhs_type->type_kind() == type::kind::KIND_STRUCT || rhs_type->type_kind() == type::kind::KIND_STRUCT) {
				if (lhs_type->type_kind() != type::kind::KIND_STRUCT || rhs_type->type_kind() != type::kind::KIND_STRUCT) {
					p->report(error(error::kind::KIND_ERROR, "Cannot apply any binary operators on a struct type and any other type.", stream, lhs->stream().size()));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				if (op.token_kind() != token::kind::TOKEN_EQUALS_EQUALS && op.token_kind() != token::kind::TOKEN_NOT_EQUALS) {
					p->report(error(error::kind::KIND_ERROR, "The only binary operators you can apply on struct types are '==' and '!='.", stream, lhs->stream().size()));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				shared_ptr<struct_type> s1 = static_pointer_cast<struct_type>(lhs_type), s2 = static_pointer_cast<struct_type>(rhs_type);
				if(s1->struct_reference_number() != s2->struct_reference_number() || s1->struct_name().raw_text() != s2->struct_name().raw_text()) {
					p->report(error(error::kind::KIND_ERROR, "Expected matching struct types to compare for equality or inequality.", stream, lhs->stream().size()));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				return make_shared<primitive_type>(primitive_type::kind::KIND_BOOL, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
					deduced_constexpr_kind, primitive_type::sign_kind::KIND_NONE);
			}
			if (lhs_type->type_kind() == type::kind::KIND_PRIMITIVE && rhs_type->type_kind() == type::kind::KIND_PRIMITIVE) {
				shared_ptr<primitive_type> lhs_pt = static_pointer_cast<primitive_type>(lhs_type), rhs_pt = static_pointer_cast<primitive_type>(rhs_type);
				primitive_type::sign_kind sk = lhs_pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_SIGNED && rhs_pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_SIGNED ?
					primitive_type::sign_kind::KIND_SIGNED : (lhs_pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_NONE || rhs_pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_NONE ?
						primitive_type::sign_kind::KIND_NONE : primitive_type::sign_kind::KIND_UNSIGNED);
				primitive_type::kind lhs_primitive_kind = lhs_pt->primitive_type_kind(), rhs_primitive_kind = rhs_pt->primitive_type_kind();
				switch (op.token_kind()) {
					case token::kind::TOKEN_PLUS:
					case token::kind::TOKEN_MINUS:
					case token::kind::TOKEN_SLASH:
					case token::kind::TOKEN_STAR: {
						if (lhs_pt->type_array_kind() == type::array_kind::KIND_ARRAY || rhs_pt->type_array_kind() == type::array_kind::KIND_ARRAY) {
							p->report(error(error::kind::KIND_ERROR, "Addition cannot involve array types.", stream, lhs->stream().size()));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						if (lhs_primitive_kind == primitive_type::kind::KIND_BOOL || lhs_primitive_kind == primitive_type::kind::KIND_VOID ||
							rhs_primitive_kind == primitive_type::kind::KIND_BOOL || rhs_primitive_kind == primitive_type::kind::KIND_VOID) {
							p->report(error(error::kind::KIND_ERROR, "Expected numeric types for this operator.", stream, lhs->stream().size()));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						if (lhs_primitive_kind == primitive_type::kind::KIND_DOUBLE || rhs_primitive_kind == primitive_type::kind::KIND_DOUBLE)
							return make_shared<primitive_type>(primitive_type::kind::KIND_DOUBLE, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, deduced_constexpr_kind, sk);
						else if (lhs_primitive_kind == primitive_type::kind::KIND_LONG || rhs_primitive_kind == primitive_type::kind::KIND_LONG)
							return make_shared<primitive_type>(primitive_type::kind::KIND_LONG, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, deduced_constexpr_kind, sk);
						else if (lhs_primitive_kind == primitive_type::kind::KIND_FLOAT || rhs_primitive_kind == primitive_type::kind::KIND_FLOAT)
							return make_shared<primitive_type>(primitive_type::kind::KIND_FLOAT, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, deduced_constexpr_kind, sk);
						else if(lhs_primitive_kind == primitive_type::kind::KIND_INT || rhs_primitive_kind == primitive_type::kind::KIND_INT)
							return make_shared<primitive_type>(primitive_type::kind::KIND_INT, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, deduced_constexpr_kind, sk);
						else if(lhs_primitive_kind == primitive_type::kind::KIND_SHORT || rhs_primitive_kind == primitive_type::kind::KIND_SHORT)
							return make_shared<primitive_type>(primitive_type::kind::KIND_SHORT, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, deduced_constexpr_kind, sk);
						else if(lhs_primitive_kind == primitive_type::kind::KIND_CHAR || rhs_primitive_kind == primitive_type::kind::KIND_CHAR)
							return make_shared<primitive_type>(primitive_type::kind::KIND_CHAR, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, deduced_constexpr_kind, sk);
						else if(lhs_primitive_kind == primitive_type::kind::KIND_BYTE || rhs_primitive_kind == primitive_type::kind::KIND_BYTE)
							return make_shared<primitive_type>(primitive_type::kind::KIND_BYTE, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, deduced_constexpr_kind, sk);
					}
					break;
					case token::kind::TOKEN_RIGHT_ANGLE_BRACKET:
					case token::kind::TOKEN_LEFT_ANGLE_BRACKET:
					case token::kind::TOKEN_LESS_THAN_OR_EQUAL_TO:
					case token::kind::TOKEN_GREATER_THAN_OR_EQUAL_TO: {
						if (lhs_pt->type_array_kind() == type::array_kind::KIND_ARRAY || rhs_pt->type_array_kind() == type::array_kind::KIND_ARRAY) {
							p->report(error(error::kind::KIND_ERROR, "Cannot use a comparative operator with an array type.", stream, lhs->stream().size()));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						if (lhs_primitive_kind == primitive_type::kind::KIND_BOOL || lhs_primitive_kind == primitive_type::kind::KIND_VOID ||
							rhs_primitive_kind == primitive_type::kind::KIND_BOOL || rhs_primitive_kind == primitive_type::kind::KIND_VOID) {
							p->report(error(error::kind::KIND_ERROR, "Expected numeric types to compare.", stream, lhs->stream().size()));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						return make_shared<primitive_type>(primitive_type::kind::KIND_BOOL, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
							deduced_constexpr_kind, primitive_type::sign_kind::KIND_NONE);
					}
					break;
					case token::kind::TOKEN_CARET:
					case token::kind::TOKEN_BAR:
					case token::kind::TOKEN_AMPERSAND:
					case token::kind::TOKEN_SHIFT_RIGHT:
					case token::kind::TOKEN_SHIFT_LEFT:
					case token::kind::TOKEN_PERCENT: {
						if (lhs_pt->type_array_kind() == type::array_kind::KIND_ARRAY || rhs_pt->type_array_kind() == type::array_kind::KIND_ARRAY) {
							p->report(error(error::kind::KIND_ERROR, "Cannot use a numeric operator with an array type.", stream, lhs->stream().size()));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						if (lhs_primitive_kind == primitive_type::kind::KIND_BOOL || lhs_primitive_kind == primitive_type::kind::KIND_VOID ||
							lhs_primitive_kind == primitive_type::kind::KIND_FLOAT || lhs_primitive_kind == primitive_type::kind::KIND_DOUBLE ||
							rhs_primitive_kind == primitive_type::kind::KIND_BOOL || rhs_primitive_kind == primitive_type::kind::KIND_VOID ||
							rhs_primitive_kind == primitive_type::kind::KIND_FLOAT || rhs_primitive_kind == primitive_type::kind::KIND_DOUBLE) {
							p->report(error(error::kind::KIND_ERROR, "Expected integral types for this operator.", stream, lhs->stream().size()));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						if (lhs_primitive_kind == primitive_type::kind::KIND_LONG || rhs_primitive_kind == primitive_type::kind::KIND_LONG)
							return make_shared<primitive_type>(primitive_type::kind::KIND_LONG, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, deduced_constexpr_kind, sk);
						else if(lhs_primitive_kind == primitive_type::kind::KIND_INT || rhs_primitive_kind == primitive_type::kind::KIND_INT)
							return make_shared<primitive_type>(primitive_type::kind::KIND_INT, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, deduced_constexpr_kind, sk);
						else if(lhs_primitive_kind == primitive_type::kind::KIND_SHORT || rhs_primitive_kind == primitive_type::kind::KIND_SHORT)
							return make_shared<primitive_type>(primitive_type::kind::KIND_SHORT, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, deduced_constexpr_kind, sk);
						else if(lhs_primitive_kind == primitive_type::kind::KIND_CHAR || rhs_primitive_kind == primitive_type::kind::KIND_CHAR)
							return make_shared<primitive_type>(primitive_type::kind::KIND_CHAR, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, deduced_constexpr_kind, sk);
						else if(lhs_primitive_kind == primitive_type::kind::KIND_BYTE || rhs_primitive_kind == primitive_type::kind::KIND_BYTE)
							return make_shared<primitive_type>(primitive_type::kind::KIND_BYTE, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, deduced_constexpr_kind, sk);
					}
					break;
					case token::kind::TOKEN_EQUALS_EQUALS:
					case token::kind::TOKEN_NOT_EQUALS: {
						auto handle_error = [&](string error_message) {
							p->report(error(error::kind::KIND_ERROR, error_message, stream, lhs->stream().size()));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						};
						if (lhs_pt->type_array_kind() != rhs_pt->type_array_kind())
							return handle_error("Cannot compare array and non-array types.");
						if (lhs_pt->array_dimensions() != rhs_pt->array_dimensions())
							return handle_error("Cannot compare arrays of an unequal number of dimensions.");
						if (lhs_primitive_kind == primitive_type::kind::KIND_VOID || rhs_primitive_kind == primitive_type::kind::KIND_VOID)
							return handle_error("Expected a non-void type for this operator.");
						shared_ptr<type> good = make_shared<primitive_type>(primitive_type::kind::KIND_BOOL, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
							deduced_constexpr_kind, primitive_type::sign_kind::KIND_NONE),
							bad = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						if (lhs_primitive_kind == primitive_type::kind::KIND_BOOL || rhs_primitive_kind == primitive_type::kind::KIND_BOOL) {
							if (lhs_primitive_kind == primitive_type::kind::KIND_BOOL && rhs_primitive_kind == primitive_type::kind::KIND_BOOL)
								return good;
							return handle_error("Expected matching boolean types for this operator.");
						}
						return good;
					}
					break;
					case token::kind::TOKEN_LOGICAL_AND:
					case token::kind::TOKEN_LOGICAL_OR: {
						if (lhs_pt->type_array_kind() == type::array_kind::KIND_ARRAY || rhs_pt->type_array_kind() == type::array_kind::KIND_ARRAY) {
							p->report(error(error::kind::KIND_ERROR, "Cannot use boolean operators on array types.", stream, lhs->stream().size()));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						if (lhs_primitive_kind != primitive_type::kind::KIND_BOOL || rhs_primitive_kind != primitive_type::kind::KIND_BOOL) {
							p->report(error(error::kind::KIND_ERROR, "Expected boolean operands for this operator.", stream, lhs->stream().size()));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
						return make_shared<primitive_type>(primitive_type::kind::KIND_BOOL, type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC,
							deduced_constexpr_kind, primitive_type::sign_kind::KIND_NONE);
					}
					break;
				}
			}
			p->report_internal("This should unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<type> deduce_ternary_expression_type(shared_ptr<parser> p, shared_ptr<binary_expression> cond, token q, shared_ptr<expression> true_path, token c,
			shared_ptr<ternary_expression> false_path) {
			if (!cond->valid() || !true_path->valid() || !false_path->valid())
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			vector<token> stream;
			for (token t : cond->stream()) stream.push_back(t);
			stream.push_back(q);
			for (token t : true_path->stream()) stream.push_back(t);
			stream.push_back(c);
			for (token t : false_path->stream()) stream.push_back(t);
			if (cond->binary_expression_type()->type_kind() == type::kind::KIND_PRIMITIVE) {
				shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(cond->binary_expression_type());
				if (pt->primitive_type_kind() != primitive_type::kind::KIND_BOOL || pt->type_array_kind() == type::array_kind::KIND_ARRAY) {
					p->report(error(error::kind::KIND_ERROR, "Expected a boolean condition to test for.", stream, 0));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
			}
			else {
				p->report(error(error::kind::KIND_ERROR, "Expected a boolean condition to test for.", stream, 0));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			}
			shared_ptr<type> t1 = true_path->expression_type(), t2 = false_path->ternary_expression_type();
			if (t1->type_array_kind() != t2->type_array_kind()) {
				p->report(error(error::kind::KIND_ERROR, "Cannot have array and non-array types.", stream, 0));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			}
			if (t1->array_dimensions() != t2->array_dimensions()) {
				p->report(error(error::kind::KIND_ERROR, "Cannot have array types of an unequal number of dimensions.", stream, 0));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			}
			type::const_kind ck = t1->type_const_kind() == type::const_kind::KIND_CONST || t2->type_const_kind() == type::const_kind::KIND_CONST ?
				type::const_kind::KIND_CONST : type::const_kind::KIND_NON_CONST;
			type::static_kind sk = t1->type_static_kind() == type::static_kind::KIND_STATIC && t2->type_static_kind() == type::static_kind::KIND_STATIC ?
				type::static_kind::KIND_STATIC : type::static_kind::KIND_NON_STATIC;
			type::constexpr_kind cek = t1->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR && t2->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR ?
				type::constexpr_kind::KIND_CONSTEXPR : type::constexpr_kind::KIND_NON_CONSTEXPR;
			if (t1->type_kind() == type::kind::KIND_PRIMITIVE && t2->type_kind() == type::kind::KIND_PRIMITIVE) {
				shared_ptr<primitive_type> pt1 = static_pointer_cast<primitive_type>(t1), pt2 = static_pointer_cast<primitive_type>(t2);
				primitive_type::sign_kind sk2 = pt1->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_SIGNED && pt2->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_SIGNED ?
					primitive_type::sign_kind::KIND_SIGNED : (pt1->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_NONE || pt2->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_NONE ?
						primitive_type::sign_kind::KIND_NONE : primitive_type::sign_kind::KIND_UNSIGNED);
				if (t1->type_array_kind() == type::array_kind::KIND_ARRAY && (pt1->primitive_type_kind() != pt2->primitive_type_kind())) {
					p->report(error(error::kind::KIND_ERROR, "Expected 2 compatible types between this colon.", stream, cond->stream().size() + true_path->stream().size() + 1));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				if (pt1->primitive_type_kind() == primitive_type::kind::KIND_VOID || pt2->primitive_type_kind() == primitive_type::kind::KIND_VOID) {
					if (pt1->primitive_type_kind() == primitive_type::kind::KIND_VOID && pt2->primitive_type_kind() == primitive_type::kind::KIND_VOID)
						return make_shared<primitive_type>(primitive_type::kind::KIND_VOID, ck, sk, primitive_type::sign_kind::KIND_NONE);
					p->report(error(error::kind::KIND_ERROR, "Expected 2 matching void types between this colon.", stream, cond->stream().size() + true_path->stream().size() + 1));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				else if (pt1->primitive_type_kind() == primitive_type::kind::KIND_BOOL || pt2->primitive_type_kind() == primitive_type::kind::KIND_BOOL) {
					if (pt1->primitive_type_kind() == primitive_type::kind::KIND_BOOL && pt2->primitive_type_kind() == primitive_type::kind::KIND_BOOL)
						return make_shared<primitive_type>(primitive_type::kind::KIND_BOOL, ck, sk, primitive_type::sign_kind::KIND_NONE);
					p->report(error(error::kind::KIND_ERROR, "Expected 2 matching boolean types between this colon.", stream, cond->stream().size() + true_path->stream().size() + 1));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				if (pt1->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE || pt2->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
					return make_shared<primitive_type>(primitive_type::kind::KIND_DOUBLE, ck, sk, cek, primitive_type::sign_kind::KIND_NONE, t1->array_dimensions());
				else if (pt1->primitive_type_kind() == primitive_type::kind::KIND_LONG || pt2->primitive_type_kind() == primitive_type::kind::KIND_LONG)
					return make_shared<primitive_type>(primitive_type::kind::KIND_LONG, ck, sk, cek, sk2, t1->array_dimensions());
				else if (pt1->primitive_type_kind() == primitive_type::kind::KIND_FLOAT || pt2->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
					return make_shared<primitive_type>(primitive_type::kind::KIND_FLOAT, ck, sk, cek, primitive_type::sign_kind::KIND_NONE, t1->array_dimensions());
				else if(pt1->primitive_type_kind() == primitive_type::kind::KIND_INT || pt2->primitive_type_kind() == primitive_type::kind::KIND_INT)
					return make_shared<primitive_type>(primitive_type::kind::KIND_INT, ck, sk, cek, sk2, t1->array_dimensions());
				else if(pt1->primitive_type_kind() == primitive_type::kind::KIND_SHORT || pt2->primitive_type_kind() == primitive_type::kind::KIND_SHORT)
					return make_shared<primitive_type>(primitive_type::kind::KIND_SHORT, ck, sk, cek, sk2, t1->array_dimensions());
				else if(pt1->primitive_type_kind() == primitive_type::kind::KIND_CHAR || pt2->primitive_type_kind() == primitive_type::kind::KIND_CHAR)
					return make_shared<primitive_type>(primitive_type::kind::KIND_CHAR, ck, sk, cek, sk2, t1->array_dimensions());
				else if(pt1->primitive_type_kind() == primitive_type::kind::KIND_BYTE || pt2->primitive_type_kind() == primitive_type::kind::KIND_BYTE)
					return make_shared<primitive_type>(primitive_type::kind::KIND_BYTE, ck, sk, cek, sk2, t1->array_dimensions());
			}
			else if (t1->type_kind() == type::kind::KIND_STRUCT && t2->type_kind() == type::kind::KIND_STRUCT) {
				shared_ptr<struct_type> s1 = static_pointer_cast<struct_type>(t1), s2 = static_pointer_cast<struct_type>(t2);
				if (s1->struct_reference_number() != s2->struct_reference_number() || s1->struct_name().raw_text() != s2->struct_name().raw_text()) {
					p->report(error(error::kind::KIND_ERROR, "Expected matching struct types between this colon.", stream, cond->stream().size() + true_path->stream().size() + 1));
				}
				return make_shared<struct_type>(ck, sk, s1->struct_name(), s1->struct_reference_number(), true, s1->array_dimensions());
			}
			else if (t1->type_kind() == type::kind::KIND_FUNCTION && t2->type_kind() == type::kind::KIND_FUNCTION) {
				if (!matching_function_types(p, t1, t2))
					p->report(error(error::kind::KIND_ERROR, "Expected matching function types between this colon.", stream, cond->stream().size() + true_path->stream().size() + 1));
				shared_ptr<function_type> ft1 = static_pointer_cast<function_type>(t1), ft2 = static_pointer_cast<function_type>(t2);
				if (ft2->type_const_kind() == type::const_kind::KIND_CONST)
					return ft2;
				else
					return ft1;
			}
			else {
				p->report(error(error::kind::KIND_ERROR, "Expected 2 matching types between this colon.", stream, cond->stream().size() + true_path->stream().size() + 1));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			}
			p->report_internal("This should unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		shared_ptr<type> deduce_assignment_expression_type(shared_ptr<parser> p, shared_ptr<unary_expression> lhs, token op, shared_ptr<assignment_expression> ae) {
			if (!lhs->valid() || !ae->valid())
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			vector<token> stream;
			for (token t : lhs->stream()) stream.push_back(t);
			stream.push_back(op);
			for (token t : ae->stream()) stream.push_back(t);
			if (lhs->unary_expression_value_kind() != value_kind::VALUE_LVALUE) {
				p->report(error(error::kind::KIND_ERROR, "Cannot assign to a rvalue expression.", stream, lhs->stream().size()));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			}
			if (lhs->unary_expression_type()->type_const_kind() == type::const_kind::KIND_CONST
				|| lhs->unary_expression_type()->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
				p->report(error(error::kind::KIND_ERROR, "Cannot re-assign a constant lvalue.", stream, lhs->stream().size()));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			}
			if (lhs->unary_expression_type()->type_kind() != ae->assignment_expression_type()->type_kind()) {
				p->report(error(error::kind::KIND_ERROR, "Assignment cannot occur between two incompatible types.", stream, lhs->stream().size()));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			}
			if (lhs->unary_expression_type()->type_array_kind() != ae->assignment_expression_type()->type_array_kind()) {
				p->report(error(error::kind::KIND_ERROR, "Cannot an assign array and non-array types.", stream, lhs->stream().size()));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			}
			if (ae->assignment_expression_type()->array_dimensions() != lhs->unary_expression_type()->array_dimensions()) {
				p->report(error(error::kind::KIND_ERROR, "Cannot assign arrays with an unequal number of dimensions.", stream, lhs->stream().size()));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			}
			if(lhs->unary_expression_type()->array_dimensions() > 0 && lhs->unary_expression_type()->type_const_kind() == type::const_kind::KIND_NON_CONST &&
				ae->assignment_expression_type()->type_const_kind() == type::const_kind::KIND_CONST) {
				p->report(error(error::kind::KIND_ERROR, "Cannot assign a const array to a non-const array (this could result in unintended side-effects).",
					stream, lhs->stream().size()));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			}
			if (lhs->unary_expression_type()->type_array_kind() == type::array_kind::KIND_ARRAY && op.token_kind() != token::kind::TOKEN_EQUALS) {
				p->report(error(error::kind::KIND_ERROR, "Cannot use a non-direct assignment operator on array types.", stream, lhs->stream().size()));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
			}
			if (lhs->unary_expression_type()->type_kind() == type::kind::KIND_PRIMITIVE && ae->assignment_expression_type()->type_kind() == type::kind::KIND_PRIMITIVE) {
				shared_ptr<primitive_type> lhs_primitive = static_pointer_cast<primitive_type>(lhs->unary_expression_type()),
					ae_primitive = static_pointer_cast<primitive_type>(ae->assignment_expression_type());
				if (lhs_primitive->type_array_kind() == type::array_kind::KIND_ARRAY && lhs_primitive->primitive_type_kind() != ae_primitive->primitive_type_kind()) {
					p->report(error(error::kind::KIND_ERROR, "Assignment cannot occur between two incompatible array types.", stream, lhs->stream().size()));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				if (lhs_primitive->primitive_type_kind() == primitive_type::kind::KIND_VOID || ae_primitive->primitive_type_kind() == primitive_type::kind::KIND_VOID) {
					p->report(error(error::kind::KIND_ERROR, "Assignment cannot occur between two void types.", stream, lhs->stream().size()));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				else if (lhs_primitive->primitive_type_kind() == primitive_type::kind::KIND_BOOL || ae_primitive->primitive_type_kind() == primitive_type::kind::KIND_BOOL) {
					if (lhs_primitive->primitive_type_kind() == primitive_type::kind::KIND_BOOL && ae_primitive->primitive_type_kind() == primitive_type::kind::KIND_BOOL) {
						if(op.token_kind() == token::kind::TOKEN_EQUALS || op.token_kind() == token::kind::TOKEN_LOGICAL_AND_EQUALS ||
							op.token_kind() == token::kind::TOKEN_LOGICAL_OR_EQUALS)
							return lhs->unary_expression_type();
						else {
							p->report(error(error::kind::KIND_ERROR, "This assignment operator cannot be applied to boolean types.", stream, lhs->stream().size()));
							return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						}
					}
					p->report(error(error::kind::KIND_ERROR, "Expected two matching boolean types for assignment.", stream, lhs->stream().size()));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				else if (lhs_primitive->primitive_type_kind() == primitive_type::kind::KIND_BYTE || lhs_primitive->primitive_type_kind() == primitive_type::kind::KIND_CHAR ||
					lhs_primitive->primitive_type_kind() == primitive_type::kind::KIND_INT || lhs_primitive->primitive_type_kind() == primitive_type::kind::KIND_LONG ||
					lhs_primitive->primitive_type_kind() == primitive_type::kind::KIND_SHORT) {
					return lhs->unary_expression_type();
				}
				else if (lhs_primitive->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE || lhs_primitive->primitive_type_kind() == primitive_type::kind::KIND_FLOAT) {
					if (op.token_kind() == token::kind::TOKEN_PLUS_EQUALS || op.token_kind() == token::kind::TOKEN_MINUS_EQUALS || op.token_kind() == token::kind::TOKEN_MULTIPLY_EQUALS ||
						op.token_kind() == token::kind::TOKEN_DIVIDE_EQUALS || op.token_kind() == token::kind::TOKEN_MODULO_EQUALS || op.token_kind() == token::kind::TOKEN_EQUALS)
						return lhs->unary_expression_type();
					p->report(error(error::kind::KIND_ERROR, "This assignment operator could not be applied to a floating point number.", stream, lhs->stream().size()));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
			}
			else if (lhs->unary_expression_type()->type_kind() == type::kind::KIND_STRUCT && ae->assignment_expression_type()->type_kind() == type::kind::KIND_STRUCT) {
				if (op.token_kind() != token::kind::TOKEN_EQUALS) {
					p->report(error(error::kind::KIND_ERROR, "Assignment between two struct types must be pure ('=').", stream, lhs->stream().size()));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				shared_ptr<struct_type> lhs_stype = static_pointer_cast<struct_type>(lhs->unary_expression_type()),
					ae_stype = static_pointer_cast<struct_type>(ae->assignment_expression_type());
				if (lhs_stype->struct_reference_number() != ae_stype->struct_reference_number() || lhs_stype->struct_name().raw_text() != ae_stype->struct_name().raw_text()) {
					p->report(error(error::kind::KIND_ERROR, "Expected two compatible 'struct' types to assign between.", stream, lhs->stream().size() - 1));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				return lhs->unary_expression_type();
			}
			else if (lhs->unary_expression_type()->type_kind() == type::kind::KIND_FUNCTION && ae->assignment_expression_type()->type_kind() == type::kind::KIND_FUNCTION) {
				if (!matching_function_types(p, lhs->unary_expression_type(), ae->assignment_expression_type())) {
					p->report(error(error::kind::KIND_ERROR, "Expected two compatible function types to assign between.", stream, lhs->stream().size() - 1));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				}
				return lhs->unary_expression_type();
			}
			p->report_internal("This should unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return nullptr;
		}

		value_kind deduce_postfix_expression_value_kind(shared_ptr<parser> p, shared_ptr<postfix_expression::postfix_type> pt) {
			if(pt == nullptr)
				return value_kind::VALUE_NONE;
			postfix_expression::kind pek = pt->postfix_type_kind();
			switch (pek) {
			case postfix_expression::kind::KIND_INCREMENT:
			case postfix_expression::kind::KIND_DECREMENT: {
				return value_kind::VALUE_RVALUE;
			}
				break;
			}
			p->report_internal("This should unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return value_kind::VALUE_NONE;
		}

		value_kind deduce_unary_expression_value_kind(shared_ptr<parser> p, unary_expression::kind uek) {
			switch (uek) {
				case unary_expression::kind::KIND_BITWISE_NOT:
				case unary_expression::kind::KIND_DECREMENT:
				case unary_expression::kind::KIND_INCREMENT:
				case unary_expression::kind::KIND_LOGICAL_NOT:
				case unary_expression::kind::KIND_MINUS:
				case unary_expression::kind::KIND_PLUS: {
					return value_kind::VALUE_RVALUE;
				}
				break;
			}
			p->report_internal("This should unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return value_kind::VALUE_NONE;
		}

		value_kind deduce_binary_expression_value_kind(shared_ptr<parser> p, shared_ptr<binary_expression> lhs, binary_expression::operator_kind op, shared_ptr<binary_expression> rhs) {
			if (!lhs->valid() || !rhs->valid())
				return value_kind::VALUE_NONE;
			if (op == binary_expression::operator_kind::KIND_ADD_EQUALS || op == binary_expression::operator_kind::KIND_BITWISE_AND_EQUALS || op == binary_expression::operator_kind::KIND_BITWISE_OR_EQUALS ||
				op == binary_expression::operator_kind::KIND_BITWISE_XOR_EQUALS || op == binary_expression::operator_kind::KIND_DIVIDE_EQUALS ||
				op == binary_expression::operator_kind::KIND_LOGICAL_AND_EQUALS || op == binary_expression::operator_kind::KIND_LOGICAL_OR_EQUALS ||
				op == binary_expression::operator_kind::KIND_MODULUS_EQUALS || op == binary_expression::operator_kind::KIND_MULTIPLY_EQUALS ||
				op == binary_expression::operator_kind::KIND_SHIFT_LEFT_EQUALS || op == binary_expression::operator_kind::KIND_SHIFT_RIGHT_EQUALS ||
				op == binary_expression::operator_kind::KIND_SUBTRACT_EQUALS) {
				return value_kind::VALUE_LVALUE;
			}
			return value_kind::VALUE_RVALUE;
		}

		value_kind deduce_ternary_expression_value_kind(shared_ptr<parser> p, shared_ptr<binary_expression> cond, shared_ptr<expression> true_path,
			shared_ptr<ternary_expression> false_path) {
			if (!cond->valid() || !true_path->valid() || !false_path->valid())
				return value_kind::VALUE_NONE;
			if (true_path->expression_value_kind() == value_kind::VALUE_LVALUE && false_path->ternary_expression_value_kind() == value_kind::VALUE_LVALUE) {
				shared_ptr<type> t1 = true_path->expression_type(), t2 = false_path->ternary_expression_type();
				if (t1->array_dimensions() != t2->array_dimensions() || t1->type_array_kind() != t2->type_array_kind() || t1->type_kind() != t2->type_kind())
					return value_kind::VALUE_RVALUE;
				if (t1->type_kind() == type::kind::KIND_PRIMITIVE) {
					shared_ptr<primitive_type> pt1 = static_pointer_cast<primitive_type>(t1), pt2 = static_pointer_cast<primitive_type>(t2);
					if (pt1->primitive_type_kind() == pt2->primitive_type_kind() && pt1->primitive_type_sign_kind() == pt2->primitive_type_sign_kind())
						return value_kind::VALUE_LVALUE;
					else
						return value_kind::VALUE_RVALUE;
				}
				else if (t1->type_kind() == type::kind::KIND_STRUCT) {
					shared_ptr<struct_type> st1 = static_pointer_cast<struct_type>(t1), st2 = static_pointer_cast<struct_type>(t2);
					if (st1->struct_name().raw_text() == st2->struct_name().raw_text() && st1->struct_reference_number() == st2->struct_reference_number())
						return value_kind::VALUE_LVALUE;
					else
						return value_kind::VALUE_RVALUE;
				}
				else if (t1->type_kind() == type::kind::KIND_FUNCTION) {
					if (matching_function_types(p, t1, t2))
						return value_kind::VALUE_LVALUE;
					else
						return value_kind::VALUE_RVALUE;
				}
			}
			else
				return value_kind::VALUE_RVALUE;
			p->report_internal("This should unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return value_kind::VALUE_NONE;
		}

		type_parser::type_parser(shared_ptr<parser> p) : _valid(false), _contained_type(nullptr), _stream(vector<token>()) {
			int start = p->get_buffer_position();
			token tok = p->pop();
			bool signed_unsigned_hit = false, const_hit = false, static_hit = false, primitive_hit = false, struct_hit = false, function_hit = false,
				constexpr_hit = false, auto_hit = false;
			shared_ptr<symbol> s_symbol = nullptr;
			shared_ptr<type> r_type = nullptr;
			vector<shared_ptr<variable_declaration>> param_list;
			token important = bad_token, important_sign = bad_token;
			while (true) {
				_stream.push_back(tok);
				if (tok.token_kind() == token::kind::TOKEN_SIGNED || tok.token_kind() == token::kind::TOKEN_UNSIGNED) {
					signed_unsigned_hit = true;
					important_sign = tok;
					if (struct_hit || function_hit) {
						_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						_valid = false;
						p->report(error(error::kind::KIND_ERROR, "Cannot have a signed or unsigned 'struct' or function types.", _stream, _stream.size() - 1));
						return;
					}
					else if (auto_hit) {
						_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						_valid = false;
						p->report(error(error::kind::KIND_ERROR, "Cannot use 'signed' or 'unsigned' in conjunction with auto.", _stream, _stream.size() - 1));
						return;
					}
				}
				else if (tok.token_kind() == token::kind::TOKEN_CONST)
					const_hit = true;
				else if (tok.token_kind() == token::kind::TOKEN_STATIC)
					static_hit = true;
				else if (tok.token_kind() == token::kind::TOKEN_CONSTEXPR)
					constexpr_hit = true;
				else if (tok.token_kind() == token::kind::TOKEN_BYTE || tok.token_kind() == token::kind::TOKEN_CHAR || tok.token_kind() == token::kind::TOKEN_SHORT ||
					tok.token_kind() == token::kind::TOKEN_INT || tok.token_kind() == token::kind::TOKEN_LONG || tok.token_kind() == token::kind::TOKEN_DOUBLE ||
					tok.token_kind() == token::kind::TOKEN_FLOAT || tok.token_kind() == token::kind::TOKEN_BOOL || tok.token_kind() == token::kind::TOKEN_VOID) {
					if (primitive_hit || struct_hit || function_hit || auto_hit) {
						_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						_valid = false;
						p->report(error(error::kind::KIND_ERROR, "Repeated type specifiers.", _stream, _stream.size() - 1));
						return;
					}
					else {
						primitive_hit = true;
						important = tok;
					}
				}
				else if (tok.token_kind() == token::kind::TOKEN_AUTO) {
					if (primitive_hit || struct_hit || function_hit || auto_hit) {
						_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						_valid = false;
						p->report(error(error::kind::KIND_ERROR, "Repeated type specifiers.", _stream, _stream.size() - 1));
						return;
					}
					else
						auto_hit = true;
				}
				else if (tok.token_kind() == token::kind::TOKEN_TYPE) {
					if (primitive_hit || struct_hit || function_hit || auto_hit) {
						_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						_valid = false;
						p->report(error(error::kind::KIND_ERROR, "Repeated type specifiers.", _stream, _stream.size() - 1));
						return;
					}
					else if (signed_unsigned_hit) {
						_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						_valid = false;
						p->report(error(error::kind::KIND_ERROR, "Cannot have a signed or unsigned 'struct' type.", _stream, _stream.size() - 1));
						return;
					}
					else {
						struct_hit = true;
						tuple<bool, vector<token>, shared_ptr<symbol>> tup = handle_qualified_identifier_name(p, true);
						if (!get<0>(tup)) {
							p->report(error(error::kind::KIND_ERROR, "Expected a qualified name referring to a struct type after the 'type' keyword.", _stream, _stream.size() - 1));
							_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
							_valid = false;
							return;
						}
						shared_ptr<symbol> s = get<2>(tup);
						vector<token> istream = get<1>(tup);
						if (s == nullptr || s->symbol_kind() != symbol::kind::KIND_STRUCT) {
							p->report(error(error::kind::KIND_ERROR, "Type not found.", istream, 0));
							_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
							_valid = false;
							return;
						}
						_stream.insert(_stream.end(), istream.begin(), istream.end());
						important = istream[istream.size() - 1];
						s_symbol = s;
					}
				}
				else if (tok.token_kind() == token::kind::TOKEN_FN) {
					if (primitive_hit || struct_hit || function_hit || auto_hit) {
						_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						_valid = false;
						p->report(error(error::kind::KIND_ERROR, "Repeated type specifiers.", _stream, _stream.size() - 1));
						return;
					}
					function_hit = true;
					type_parser rt_parser(p);
					if (!rt_parser.valid() || rt_parser.contained_type()->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR
						|| rt_parser.contained_type()->type_kind() == type::kind::KIND_AUTO) {
						_valid = false;
						p->report(error(error::kind::KIND_ERROR, "Invalid return type for a function type.", _stream, _stream.size() - 1));
						return;
					}
					r_type = rt_parser.contained_type();
					if (p->peek().token_kind() != token::kind::TOKEN_OPEN_PARENTHESIS) {
						_valid = false;
						p->report(error(error::kind::KIND_ERROR, "Expected a function parameter type list following a return type for a function variable.", _stream, _stream.size() - 1));
						return;
					}
					_stream.push_back(p->pop());
					while (p->peek().token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
						type_parser param_type_parser(p);
						if (!param_type_parser.valid() || param_type_parser.contained_type()->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR
							|| param_type_parser.contained_type()->type_kind() == type::kind::KIND_AUTO) {
							_valid = false;
							p->report(error(error::kind::KIND_ERROR, "Expected a valid function parameter type in a function type's parameter list.", _stream, _stream.size() - 1));
							return;
						}
						vector<token> param_stream = param_type_parser.stream();
						_stream.insert(_stream.end(), param_stream.begin(), param_stream.end());
						shared_ptr<type> param_type = param_type_parser.contained_type();
						shared_ptr<variable_declaration> param = make_shared<variable_declaration>(param_type, bad_token, true, param_stream);
						if (!check_declaration_type_is_correct(p, make_shared<type_parser>(param_type_parser))) {
							_valid = false;
							p->report(error(error::kind::KIND_ERROR, "Expected a valid type that a function could take.", param_stream, 0));
							return;
						}
						param_list.push_back(param);
						if (p->peek().token_kind() == token::kind::TOKEN_COMMA) {
							token comma = p->pop();
							if (p->peek().token_kind() == token::kind::TOKEN_CLOSE_PARENTHESIS) {
								_valid = false;
								p->report(error(error::kind::KIND_ERROR, "After a comma, there were no more function arguments.", vector<token>{ comma, p->peek() }, 0));
								return;
							}
							_stream.push_back(comma);
						}
					}
					if (p->peek().token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
						_valid = false;
						p->report(error(error::kind::KIND_ERROR, "Expected a close parenthesis ')' to finish a function parameter list.", _stream, _stream.size() - 1));
						break;
					}
					_stream.push_back(p->pop());
				}
				else
					break;
				tok = p->pop();
			}
			p->backtrack();
			_stream.pop_back();
			token arr_open = p->pop();
			int ad = 0;
			if (constexpr_hit) static_hit = true;
			while (arr_open.token_kind() == token::kind::TOKEN_OPEN_BRACKET || arr_open.token_kind() == token::kind::TOKEN_STAR) {
				ad++;
				_stream.push_back(arr_open);
				if (arr_open.token_kind() == token::kind::TOKEN_OPEN_BRACKET) {
					token next = p->pop();
					if (next.token_kind() == token::kind::TOKEN_CLOSE_BRACKET)
						_stream.push_back(next);
					else {
						p->report(error(error::kind::KIND_ERROR, "Expected an array type.", _stream, _stream.size() - 1));
						_valid = false;
						_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE, ad);
						return;
					}
				}
				arr_open = p->pop();
			}
			p->backtrack();
			if(!primitive_hit && !struct_hit && !function_hit && !auto_hit) {
				_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
				_valid = false;
				p->set_buffer_position(start);
				return;
			}
			if (constexpr_hit && (struct_hit || function_hit) && ad == 0) {
				p->report(error(error::kind::KIND_ERROR, "Cannot have a constexpr struct or function.", _stream, _stream.size() - 1));
				_valid = false;
				_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE, ad);
				return;
			}
			if (primitive_hit) {
				primitive_type::kind k = primitive_type::kind::KIND_NONE;
				primitive_type::sign_kind s = primitive_type::sign_kind::KIND_NONE;
				if (important.token_kind() == token::kind::TOKEN_BOOL || important.token_kind() == token::kind::TOKEN_DOUBLE ||
					important.token_kind() == token::kind::TOKEN_FLOAT || important.token_kind() == token::kind::TOKEN_VOID) {
					if (signed_unsigned_hit) {
						_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE);
						_valid = false;
						important = tok;
						p->report(error(error::kind::KIND_ERROR, "The given type specifier is neither signed nor unsigned.", _stream, _stream.size() - 1));
						return;
					}
				}
				else {
					if (important_sign.token_kind() == bad_token.token_kind() || important_sign.token_kind() == token::kind::TOKEN_SIGNED)
						s = primitive_type::sign_kind::KIND_SIGNED;
					else
						s = primitive_type::sign_kind::KIND_UNSIGNED;
				}
				_valid = true;
				if (important.token_kind() == token::kind::TOKEN_BYTE) {
					if (important_sign.token_kind() == bad_token.token_kind())
						s = primitive_type::sign_kind::KIND_UNSIGNED;
					k = primitive_type::kind::KIND_BYTE;
				}
				else if (important.token_kind() == token::kind::TOKEN_BOOL)
					k = primitive_type::kind::KIND_BOOL;
				else if (important.token_kind() == token::kind::TOKEN_CHAR) {
					if (important_sign.token_kind() == bad_token.token_kind())
						s = primitive_type::sign_kind::KIND_UNSIGNED;
					k = primitive_type::kind::KIND_CHAR;
				}
				else if (important.token_kind() == token::kind::TOKEN_SHORT)
					k = primitive_type::kind::KIND_SHORT;
				else if (important.token_kind() == token::kind::TOKEN_INT)
					k = primitive_type::kind::KIND_INT;
				else if (important.token_kind() == token::kind::TOKEN_LONG)
					k = primitive_type::kind::KIND_LONG;
				else if (important.token_kind() == token::kind::TOKEN_FLOAT)
					k = primitive_type::kind::KIND_FLOAT;
				else if (important.token_kind() == token::kind::TOKEN_DOUBLE)
					k = primitive_type::kind::KIND_DOUBLE;
				else if (important.token_kind() == token::kind::TOKEN_VOID)
					k = primitive_type::kind::KIND_VOID;
				else if (important.token_kind() == bad_token.token_kind())
					_valid = false;
				if (k == primitive_type::kind::KIND_VOID && ad > 0) {
					p->report(error(error::kind::KIND_ERROR, "Cannot have an array of void.", _stream, 0));
					_contained_type = make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE, 0);
					_valid = false;
					return;
				}
				_contained_type = make_shared<primitive_type>(k, const_hit ? type::const_kind::KIND_CONST : type::const_kind::KIND_NON_CONST,
					static_hit ? type::static_kind::KIND_STATIC : type::static_kind::KIND_NON_STATIC,
					constexpr_hit ? type::constexpr_kind::KIND_CONSTEXPR : type::constexpr_kind::KIND_NON_CONSTEXPR, s, ad);
				return;
			}
			else if (struct_hit) {
				shared_ptr<struct_symbol> ss = static_pointer_cast<struct_symbol>(s_symbol);
				_contained_type = make_shared<struct_type>(const_hit ? type::const_kind::KIND_CONST : type::const_kind::KIND_NON_CONST, static_hit ? type::static_kind::KIND_STATIC :
					type::static_kind::KIND_NON_STATIC, constexpr_hit ? type::constexpr_kind::KIND_CONSTEXPR : type::constexpr_kind::KIND_NON_CONSTEXPR,
					important, ss->struct_symbol_type()->struct_reference_number(), true, ad);
				_valid = true;
				return;
			}
			else if (function_hit) {
				_contained_type = make_shared<function_type>(const_hit ? type::const_kind::KIND_CONST : type::const_kind::KIND_NON_CONST,
					static_hit ? type::static_kind::KIND_STATIC : type::static_kind::KIND_NON_STATIC,
					constexpr_hit ? type::constexpr_kind::KIND_CONSTEXPR : type::constexpr_kind::KIND_NON_CONSTEXPR,
					r_type, param_list, -1, ad);
				_valid = true;
				return;
			}
			else if (auto_hit) {
				_contained_type = make_shared<auto_type>(const_hit ? type::const_kind::KIND_CONST : type::const_kind::KIND_NON_CONST,
					static_hit ? type::static_kind::KIND_STATIC : type::static_kind::KIND_NON_STATIC,
					constexpr_hit ? type::constexpr_kind::KIND_CONSTEXPR : type::constexpr_kind::KIND_NON_CONSTEXPR,
					ad);
				_valid = true;
				return;
			}
			p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
		}

		type_parser::~type_parser() {

		}

		shared_ptr<type> type_parser::contained_type() {
			return _contained_type;
		}

		bool type_parser::valid() {
			return _valid;
		}

		vector<token> type_parser::stream() {
			return _stream;
		}

		variable_declaration_parser::variable_declaration_parser(shared_ptr<parser> p) : _contained_variable_declaration_list(vector<shared_ptr<variable_declaration>>()), _valid(false),
			_stream(vector<token>()) {
			shared_ptr<type_parser> tp = make_shared<type_parser>(p);
			vector<token> tstream = tp->stream();
			_stream.insert(_stream.end(), tstream.begin(), tstream.end());
			if (!tp->valid()) {
				_contained_variable_declaration_list = { make_shared<variable_declaration>(tp->contained_type(), bad_token, false, _stream) };
				_valid = false;
				return;
			}
			bool good_type = check_declaration_type_is_correct(p, tp);
			if (!good_type) {
				_contained_variable_declaration_list = { make_shared<variable_declaration>(tp->contained_type(), bad_token, false, _stream) };
				_valid = false;
				return;
			}
			shared_ptr<type> decl_type = tp->contained_type(), orig_type = decl_type;
			vector<token> current_stream = _stream;
			token current_name = p->pop();
			_valid = true;
			while (current_name.token_kind() == token::kind::TOKEN_IDENTIFIER) {
				current_stream.push_back(current_name);
				_stream.push_back(current_name);
				shared_ptr<variable_declaration> vd = nullptr;
				if (p->peek().token_kind() == token::kind::TOKEN_EQUALS) {
					token equals = p->pop();
					_stream.push_back(equals);
					current_stream.push_back(equals);
					shared_ptr<assignment_expression> ae = expression_parser_helper::parse_assignment_expression(p);
					if (!ae->valid()) {
						p->report(error(error::kind::KIND_ERROR, "Expected a valid initializer here.", current_stream, current_stream.size() - 1));
						_contained_variable_declaration_list.push_back(make_shared<variable_declaration>(decl_type, current_name, ae, false, current_stream));
						_valid = false;
						return;
					}
					if (decl_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR && !is_constant_expression(p, ae, false)) {
						p->report(error(error::kind::KIND_ERROR, "Cannot initialize a 'constexpr' variable with a 'non-constexpr' initializer.", current_stream, current_stream.size() - 1));
						_contained_variable_declaration_list.push_back(make_shared<variable_declaration>(decl_type, current_name, ae, false, current_stream));
						_valid = false;
						return;
					}
					vector<token> estream = ae->stream();
					if ((p->current_scope()->scope_kind() == scope::kind::KIND_GLOBAL || p->current_scope()->scope_kind() == scope::kind::KIND_NAMESPACE) && !is_constant_expression(p, ae)) {
						p->report(error(error::kind::KIND_ERROR, "Expected a constant initializer for a global or namespace variable.", estream, 0));
						_contained_variable_declaration_list.push_back(make_shared<variable_declaration>(decl_type, current_name, ae, false, current_stream));
						_valid = false;
						return;
					}
					_stream.insert(_stream.end(), estream.begin(), estream.end());
					current_stream.insert(current_stream.end(), estream.begin(), estream.end());
					vd = check_declaration_initializer_is_correct(p, make_shared<variable_declaration>(decl_type, current_name, ae, true, current_stream));
					if (decl_type->type_kind() == type::kind::KIND_AUTO) decl_type = vd->variable_declaration_type();
					_valid = _valid && vd->valid();
				}
				else {
					if (decl_type->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
						p->report(error(error::kind::KIND_ERROR, "A 'constexpr' variable must be initialized.", current_stream, current_stream.size() - 1));
						_contained_variable_declaration_list.push_back(make_shared<variable_declaration>(decl_type, current_name, true, current_stream));
						_valid = false;
						return;
					}
					if (orig_type->type_kind() == type::kind::KIND_AUTO) {
						p->report(error(error::kind::KIND_ERROR, "An 'auto' variable must be initialized.", current_stream, current_stream.size() - 1));
						_contained_variable_declaration_list.push_back(make_shared<variable_declaration>(decl_type, current_name, true, current_stream));
						_valid = false;
						return;
					}
					vd = check_declaration_initializer_is_correct(p, make_shared<variable_declaration>(decl_type, current_name, true, current_stream));
					_valid = _valid && vd->valid();
				}
				shared_ptr<variable_symbol> vs = make_shared<variable_symbol>(p->current_scope(), vd->variable_declaration_name(), vd->variable_declaration_type(),
					vd->stream());
				vd->set_variable_declaration_symbol(vs);
				shared_ptr<symbol> s = p->find_symbol_in_current_scope(vd->variable_declaration_name(), true);
				if (s != nullptr) {
					p->report(error(error::kind::KIND_ERROR, "Cannot redeclare a symbol with the same name in the same scope.", vd->stream(), 0));
					if (s->symbol_kind() == symbol::kind::KIND_VARIABLE) {
						shared_ptr<variable_symbol> ovs = static_pointer_cast<variable_symbol>(s);
						p->report(error(error::kind::KIND_NOTE, "Originally declared here.", ovs->stream(), 0));
					}
					else if (s->symbol_kind() == symbol::kind::KIND_FUNCTION) {
						shared_ptr<function_symbol> ovs = static_pointer_cast<function_symbol>(s);
						vector<token> ovs_stream = ovs->stream();
						p->report(error(error::kind::KIND_NOTE, "Originally declared here.", ovs_stream.size() > 10 ?
							vector<token>(ovs_stream.begin(), ovs_stream.begin() + 10) : ovs_stream, 0));
					}
					_valid = false;
				}
				else
					p->add_symbol_to_current_scope(vs);
				_contained_variable_declaration_list.push_back(vd);
				current_stream.clear();
				if (p->peek().token_kind() == token::kind::TOKEN_COMMA)
					_stream.push_back(p->pop());
				else {
					p->pop();
					break;
				}
				current_name = p->pop();
			}
			if (_contained_variable_declaration_list.size() == 0) {
				p->report(error(error::kind::KIND_ERROR, "Expected at least one variable name after a type.", _stream, _stream.size() - 1));
				_valid = false;
				return;
			}
			p->backtrack();
		}

		variable_declaration_parser::~variable_declaration_parser() {
			
		}
		
		vector<shared_ptr<variable_declaration>> variable_declaration_parser::contained_variable_declaration_list() {
			return _contained_variable_declaration_list;
		}

		bool variable_declaration_parser::valid() {
			return _valid;
		}

		vector<token> variable_declaration_parser::stream() {
			return _stream;
		}

		bool check_declaration_type_is_correct(shared_ptr<parser> p, shared_ptr<type_parser> tp) {
			if (tp == nullptr || !tp->valid())
				return false;
			shared_ptr<type> t = tp->contained_type();
			if (t->type_kind() == type::kind::KIND_PRIMITIVE) {
				shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
				if (pt->primitive_type_kind() == primitive_type::kind::KIND_VOID) {
					p->report(error(error::kind::KIND_ERROR, "Cannot have a void declaration type.", tp->stream(), 0));
					return false;
				}
			}
			return true;
		}

		shared_ptr<variable_declaration> check_declaration_initializer_is_correct(shared_ptr<parser> p, shared_ptr<variable_declaration> vd) {
			if (vd == nullptr || !vd->valid())
				return vd;
			shared_ptr<variable_declaration> bad_variable = make_shared<variable_declaration>(vd->variable_declaration_type(), vd->variable_declaration_name(),
				vd->initialization(), false, vd->stream());
			if (vd->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_PRESENT) {
				shared_ptr<type> decl_type = vd->variable_declaration_type(), i_type = vd->initialization()->assignment_expression_type();
				if (decl_type->type_kind() != type::kind::KIND_AUTO) {
					if (decl_type->type_kind() != i_type->type_kind()) {
						p->report(error(error::kind::KIND_ERROR, "Initialization of declaration cannot occur between two incompatible types.", vd->initialization()->stream(), 0));
						return bad_variable;
					}
					if (decl_type->type_array_kind() != i_type->type_array_kind()) {
						p->report(error(error::kind::KIND_ERROR, "Cannot assign array and non-array types.", vd->initialization()->stream(), 0));
						return bad_variable;
					}
					if (decl_type->array_dimensions() != i_type->array_dimensions()) {
						p->report(error(error::kind::KIND_ERROR, "Cannot assign array types of unequal dimensions.", vd->initialization()->stream(), 0));
						return bad_variable;
					}
				}
				else {
					if (decl_type->array_dimensions() > i_type->array_dimensions()) {
						p->report(error(error::kind::KIND_ERROR, "The declared auto type and the initializer type cannot be compatible.", vd->initialization()->stream(), 0));
						return bad_variable;
					}
				}
				if (decl_type->array_dimensions() > 0 && decl_type->type_const_kind() == type::const_kind::KIND_NON_CONST &&
					i_type->type_const_kind() == type::const_kind::KIND_CONST) {
					p->report(error(error::kind::KIND_ERROR, "Cannot assign a const array to a non-const array (this could result in unintended side-effects).",
						vd->initialization()->stream(), 0));
					return bad_variable;
				}
				if (decl_type->type_kind() == type::kind::KIND_AUTO) {
					type::static_kind sk = decl_type->type_static_kind();
					type::const_kind ck = decl_type->type_const_kind();
					type::constexpr_kind cek = decl_type->type_constexpr_kind();
					int ad = i_type->array_dimensions();
					if (i_type->type_kind() == type::kind::KIND_PRIMITIVE) {
						shared_ptr<primitive_type> i_ptype = static_pointer_cast<primitive_type>(i_type);
						primitive_type::kind pk = i_ptype->primitive_type_kind();
						primitive_type::sign_kind psk = i_ptype->primitive_type_sign_kind();
						decl_type = make_shared<primitive_type>(pk, ck, sk, cek, psk, ad);
					}
					else if (i_type->type_kind() == type::kind::KIND_FUNCTION) {
						shared_ptr<function_type> i_ftype = static_pointer_cast<function_type>(i_type);
						int fr = i_ftype->function_reference_number();
						shared_ptr<type> rt = i_ftype->return_type();
						vector<shared_ptr<variable_declaration>> pl = i_ftype->parameter_list();
						decl_type = make_shared<function_type>(ck, sk, cek, rt, pl, fr, ad);
					}
					else if (i_type->type_kind() == type::kind::KIND_STRUCT) {
						shared_ptr<struct_type> i_stype = static_pointer_cast<struct_type>(i_type);
						token sn = i_stype->struct_name();
						int sr = i_stype->struct_reference_number();
						bool v = i_stype->valid();
						decl_type = make_shared<struct_type>(ck, sk, cek, sn, sr, v, ad);
					}
					else {
						p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
					}
					vd = make_shared<variable_declaration>(decl_type, vd->variable_declaration_name(), vd->initialization(), vd->valid(), vd->stream());
				}
				if (decl_type->type_kind() == type::kind::KIND_PRIMITIVE && i_type->type_kind() == type::kind::KIND_PRIMITIVE) {
					shared_ptr<primitive_type> decl_ptype = static_pointer_cast<primitive_type>(decl_type),
						i_ptype = static_pointer_cast<primitive_type>(i_type);
					if (decl_ptype->type_array_kind() == type::array_kind::KIND_ARRAY && decl_ptype->primitive_type_kind() != i_ptype->primitive_type_kind()) {
						p->report(error(error::kind::KIND_ERROR, "Cannot assign incompatible array types.", vd->initialization()->stream(), 0));
						return bad_variable;
					}
					if (decl_ptype->primitive_type_kind() == primitive_type::kind::KIND_VOID)
						return bad_variable;
					if (i_ptype->primitive_type_kind() == primitive_type::kind::KIND_VOID) {
						p->report(error(error::kind::KIND_ERROR, "A void type cannot be assigned to anything.", vd->initialization()->stream(), 0));
						return bad_variable;
					}
					if (decl_ptype->primitive_type_kind() == primitive_type::kind::KIND_BOOL) {
						if (i_ptype->primitive_type_kind() == primitive_type::kind::KIND_BOOL)
							return vd;
						else {
							p->report(error(error::kind::KIND_ERROR, "Expected matching bool types for an initializer and a declaration type.", vd->stream(), 0));
							return bad_variable;
						}
					}
					else {
						if (i_ptype->primitive_type_kind() == primitive_type::kind::KIND_BOOL) {
							p->report(error(error::kind::KIND_ERROR, "Expected a numeric initializer to initialize this variable.", vd->initialization()->stream(), 0));
							return bad_variable;
						}
						return vd;
					}
				}
				if (decl_type->type_kind() == type::kind::KIND_STRUCT && i_type->type_kind() == type::kind::KIND_STRUCT) {
					shared_ptr<struct_type> d_stype = static_pointer_cast<struct_type>(decl_type), i_stype = static_pointer_cast<struct_type>(i_type);
					if (d_stype->struct_reference_number() != i_stype->struct_reference_number() || d_stype->struct_name().raw_text() != i_stype->struct_name().raw_text()) {
						p->report(error(error::kind::KIND_ERROR, "Expected compatible struct types between an initializer and a declaration type.", vd->stream(), 0));
						return bad_variable;
					}
					return vd;
				}
				if (decl_type->type_kind() == type::kind::KIND_FUNCTION && i_type->type_kind() == type::kind::KIND_FUNCTION) {
					if (!matching_function_types(p, decl_type, i_type)) {
						p->report(error(error::kind::KIND_ERROR, "Expected compatible function types between an initializer and a declaration type.", vd->stream(), 0));
						return bad_variable;
					}
					return vd;
				}
			}
			else if (vd->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_NOT_PRESENT)
				return vd;
			p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return bad_variable;
		}

		if_stmt_parser::if_stmt_parser(shared_ptr<parser> p) : _contained_if_stmt(nullptr), _valid(false) {
			shared_ptr<scope> if_scope = make_shared<block_scope>(p->next_block_scope_number(), p->current_scope());
			p->open_new_scope(if_scope);
			if (p->peek().token_kind() != token::kind::TOKEN_IF) {
				_contained_if_stmt = make_shared<if_stmt>(if_scope, nullptr, nullptr, false, vector<token>{});
				_valid = false;
				p->close_current_scope();
				return;
			}
			vector<token> stream;
			stream.push_back(p->pop());
			token opar = p->pop();
			if (opar.token_kind() != token::kind::TOKEN_OPEN_PARENTHESIS) {
				p->report(error(error::kind::KIND_ERROR, "Expected an open parenthesis to start an if statement.", stream, stream.size() - 1));
				_contained_if_stmt = make_shared<if_stmt>(if_scope, nullptr, nullptr, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			stream.push_back(opar);
			variable_declaration_parser vp(p);
			int start = p->get_buffer_position();
			vector<shared_ptr<variable_declaration>> vdlist = vp.contained_variable_declaration_list();
			bool vdlist_present = false;
			if (vp.valid()) {
				vector<token> vd_stream = vp.stream();
				stream.insert(stream.end(), vd_stream.begin(), vd_stream.end());
				vdlist_present = true;
				token semicolon = p->pop();
				if (semicolon.token_kind() != token::kind::TOKEN_SEMICOLON) {
					p->report(error(error::kind::KIND_ERROR, "Expected an if statement variable initializer to end with a semicolon.", stream, 0));
					_contained_if_stmt = make_shared<if_stmt>(if_scope, vdlist, nullptr, nullptr, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				stream.push_back(semicolon);
			}
			else
				p->set_buffer_position(start);
			shared_ptr<expression> cond = expression_parser(p).contained_expression();
			int cond_start_pos = stream.size();
			if (!cond->valid()) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid condition for an if statement.", stream, 0));
				if (vdlist_present)
					_contained_if_stmt = make_shared<if_stmt>(if_scope, vdlist, cond, nullptr, false, stream);
				else
					_contained_if_stmt = make_shared<if_stmt>(if_scope, cond, nullptr, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			vector<token> cond_stream = cond->stream();
			stream.insert(stream.end(), cond_stream.begin(), cond_stream.end());
			shared_ptr<type> cond_type = cond->expression_type();
			if (cond_type->type_kind() != type::kind::KIND_PRIMITIVE || cond_type->type_array_kind() == type::array_kind::KIND_ARRAY ||
				static_pointer_cast<primitive_type>(cond_type)->primitive_type_kind() != primitive_type::kind::KIND_BOOL) {
				p->report(error(error::kind::KIND_ERROR, "Expected a boolean type to use as a condition in an if statement.", stream, cond_start_pos));
				if (vdlist_present)
					_contained_if_stmt = make_shared<if_stmt>(if_scope, vdlist, cond, nullptr, false, stream);
				else
					_contained_if_stmt = make_shared<if_stmt>(if_scope, cond, nullptr, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			token close_par = p->pop();
			if(close_par.token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
				p->report(error(error::kind::KIND_ERROR, "Expected a close parenthesis to terminate an if condition.", stream, stream.size() - 1));
				if (vdlist_present)
					_contained_if_stmt = make_shared<if_stmt>(if_scope, vdlist, cond, nullptr, false, stream);
				else
					_contained_if_stmt = make_shared<if_stmt>(if_scope, cond, nullptr, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			stream.push_back(close_par);
			stmt_parser stp(p);
			vector<token> tp_stream = stp.contained_stmt()->stream();
			shared_ptr<stmt> tp = stp.contained_stmt();
			stream.insert(stream.end(), tp_stream.begin(), tp_stream.end());
			if (!stp.valid()) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid statement following an if statement condition.", stream, stream.size() - 1));
				if (vdlist_present)
					_contained_if_stmt = make_shared<if_stmt>(if_scope, vdlist, cond, tp, false, stream);
				else
					_contained_if_stmt = make_shared<if_stmt>(if_scope, cond, tp, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			if (!valid_statement_inside_block_scope(tp)) {
				p->report(error(error::kind::KIND_ERROR, "This statement cannot be nested within an if statement path.", stream, stream.size() - 1));
				if (vdlist_present)
					_contained_if_stmt = make_shared<if_stmt>(if_scope, vdlist, cond, tp, false, stream);
				else
					_contained_if_stmt = make_shared<if_stmt>(if_scope, cond, tp, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			if (p->peek().token_kind() != token::kind::TOKEN_ELSE) {
				if (vdlist_present)
					_contained_if_stmt = make_shared<if_stmt>(if_scope, vdlist, cond, tp, true, stream);
				else
					_contained_if_stmt = make_shared<if_stmt>(if_scope, cond, tp, true, stream);
				_valid = true;
				p->close_current_scope();
				return;
			}
			stream.push_back(p->pop());
			stmt_parser sfp(p);
			vector<token> fp_stream = sfp.contained_stmt()->stream();
			shared_ptr<stmt> fp = sfp.contained_stmt();
			stream.insert(stream.end(), fp_stream.begin(), fp_stream.end());
			if (!sfp.valid()) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid statement to follow an else conditional path.", stream, stream.size() - 1));
				if (vdlist_present)
					_contained_if_stmt = make_shared<if_stmt>(if_scope, vdlist, cond, tp, fp, false, stream);
				else
					_contained_if_stmt = make_shared<if_stmt>(if_scope, cond, tp, fp, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			if (!valid_statement_inside_block_scope(fp)) {
				p->report(error(error::kind::KIND_ERROR, "This statement cannot be nested within an else statement path.", stream, stream.size() - 1));
				if (vdlist_present)
					_contained_if_stmt = make_shared<if_stmt>(if_scope, vdlist, cond, tp, fp, false, stream);
				else
					_contained_if_stmt = make_shared<if_stmt>(if_scope, cond, tp, fp, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			if (vdlist_present)
				_contained_if_stmt = make_shared<if_stmt>(if_scope, vdlist, cond, tp, fp, true, stream);
			else
				_contained_if_stmt = make_shared<if_stmt>(if_scope, cond, tp, fp, true, stream);
			_valid = true;
			p->close_current_scope();
		}

		if_stmt_parser::~if_stmt_parser() {

		}

		shared_ptr<if_stmt> if_stmt_parser::contained_if_stmt() {
			return _contained_if_stmt;
		}

		bool if_stmt_parser::valid() {
			return _valid;
		}

		function_stmt_parser::function_stmt_parser(shared_ptr<parser> p) : _contained_function_stmt(nullptr), _valid(false) {
			if (p->peek().token_kind() != token::kind::TOKEN_FUNC) {
				_contained_function_stmt = make_shared<function_stmt>(nullptr, bad_token, nullptr, vector<shared_ptr<stmt>>{}, function_stmt::defined_kind::KIND_NONE,
					nullptr, false, vector<token>{});
				_valid = false;
				return;
			}
			vector<token> stream;
			stream.push_back(p->pop());
			type::const_kind ck = type::const_kind::KIND_NON_CONST;
			type::static_kind sk = type::static_kind::KIND_NON_STATIC;
			if (p->peek().token_kind() == token::kind::TOKEN_OPEN_BRACKET) {
				stream.push_back(p->pop());
				while (p->peek().token_kind() == token::kind::TOKEN_CONST || p->peek().token_kind() == token::kind::TOKEN_STATIC) {
					token tok = p->pop();
					stream.push_back(tok);
					if (tok.token_kind() == token::kind::TOKEN_CONST)
						ck = type::const_kind::KIND_CONST;
					else
						sk = type::static_kind::KIND_STATIC;
					if (p->peek().token_kind() == token::kind::TOKEN_COMMA)
						stream.push_back(p->pop());
				}
				token close = p->pop();
				if (close.token_kind() != token::kind::TOKEN_CLOSE_BRACKET) {
					p->report(error(error::kind::KIND_ERROR, "Expected a close bracket to close a function type list.", stream, 0));
					_contained_function_stmt = make_shared<function_stmt>(nullptr, bad_token, nullptr, vector<shared_ptr<stmt>>{}, function_stmt::defined_kind::KIND_NONE,
						nullptr, false, stream);
					_valid = false;
					return;
				}
				stream.push_back(close);
			}
			type_parser rtp(p);
			shared_ptr<type> rt = rtp.contained_type();
			vector<token> rt_stream = rtp.stream();
			if(!rtp.valid() || rtp.contained_type()->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR
				|| rtp.contained_type()->type_kind() == type::kind::KIND_AUTO) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid return type for a function.", stream, 0));
				_contained_function_stmt = make_shared<function_stmt>(nullptr, bad_token, nullptr, vector<shared_ptr<stmt>>{}, function_stmt::defined_kind::KIND_NONE,
					nullptr, false, stream);
				_valid = false;
				return;
			}
			stream.insert(stream.end(), rt_stream.begin(), rt_stream.end());
			shared_ptr<scope> ps = p->current_scope();
			token fn = p->pop();
			if (fn.token_kind() != token::kind::TOKEN_IDENTIFIER) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid function name.", stream, 0));
				_contained_function_stmt = make_shared<function_stmt>(nullptr, bad_token, nullptr, vector<shared_ptr<stmt>>{}, function_stmt::defined_kind::KIND_NONE,
					nullptr, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			stream.push_back(fn);
			shared_ptr<function_scope> fs = make_shared<function_scope>(ps, ck, sk, rt, fn);
			p->open_new_scope(fs);
			token opar = p->pop();
			if (opar.token_kind() != token::kind::TOKEN_OPEN_PARENTHESIS) {
				p->report(error(error::kind::KIND_ERROR, "Expected an open parenthesis ('(') to begin a parameter declaration list.", stream, 0));
				_contained_function_stmt = make_shared<function_stmt>(nullptr, fn, nullptr, vector<shared_ptr<stmt>>{}, function_stmt::defined_kind::KIND_NONE,
					fs, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			stream.push_back(opar);
			vector<shared_ptr<variable_declaration>> pl;
			while (p->peek().token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
				variable_declaration_parser vp(p);
				if (!vp.valid()) {
					p->report(error(error::kind::KIND_ERROR, "Expected a valid parameter list for a function.", stream, 0));
					_contained_function_stmt = make_shared<function_stmt>(nullptr, fn, nullptr, vector<shared_ptr<stmt>>{},
						function_stmt::defined_kind::KIND_NONE, fs, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				vector<token> vd_stream = vp.stream();
				vector<shared_ptr<variable_declaration>> vd_list = vp.contained_variable_declaration_list();
				if (vp.contained_variable_declaration_list().size() != 1) {
					p->report(error(error::kind::KIND_ERROR, "Expected only a single name per parameter.", vd_stream, 0));
					_contained_function_stmt = make_shared<function_stmt>(nullptr, fn, nullptr, vector<shared_ptr<stmt>>{},
						function_stmt::defined_kind::KIND_NONE, fs, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				if (vd_list[0]->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_PRESENT) {
					p->report(error(error::kind::KIND_ERROR, "Cannot initialize a function parameter.", vd_stream, 0));
					_contained_function_stmt = make_shared<function_stmt>(nullptr, fn, nullptr, vector<shared_ptr<stmt>>{},
						function_stmt::defined_kind::KIND_NONE, fs, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				if (vd_list[0]->variable_declaration_type()->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
					p->report(error(error::kind::KIND_ERROR, "A function parameter cannot be declared 'constexpr.'", vd_stream, 0));
					_contained_function_stmt = make_shared<function_stmt>(nullptr, fn, nullptr, vector<shared_ptr<stmt>>{},
						function_stmt::defined_kind::KIND_NONE, fs, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				if (vd_list[0]->variable_declaration_type()->type_kind() == type::kind::KIND_AUTO) {
					p->report(error(error::kind::KIND_ERROR, "A function parameter cannot be declared as 'auto.'", vd_stream, 0));
					_contained_function_stmt = make_shared<function_stmt>(nullptr, fn, nullptr, vector<shared_ptr<stmt>>{},
						function_stmt::defined_kind::KIND_NONE, fs, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				stream.insert(stream.end(), vd_stream.begin(), vd_stream.end());
				pl.insert(pl.end(), vd_list.begin(), vd_list.end());
				if (p->peek().token_kind() == token::kind::TOKEN_COMMA)
					stream.push_back(p->pop());
			}
			p->close_current_scope();
			token cpar = p->pop();
			shared_ptr<function_type> ft = nullptr;
			shared_ptr<symbol> res = p->find_symbol_in_current_scope(fn, true);
			if(res != nullptr) {
				bool bad = false;
				if (res->symbol_kind() != symbol::kind::KIND_FUNCTION) {
					p->report(error(error::kind::KIND_ERROR, "Function name conflicts with symbol already declared.", stream, 0));
					if(res->symbol_kind() == symbol::kind::KIND_NAMESPACE)
						p->report(error(error::kind::KIND_NOTE, "Originally declared here.", static_pointer_cast<namespace_symbol>(res)->stream(), 0));
					else if(res->symbol_kind() == symbol::kind::KIND_VARIABLE)
						p->report(error(error::kind::KIND_NOTE, "Originally declared here.", static_pointer_cast<variable_symbol>(res)->stream(), 0));
					else if(res->symbol_kind() == symbol::kind::KIND_STRUCT)
						p->report(error(error::kind::KIND_NOTE, "Originally declared here.", static_pointer_cast<struct_symbol>(res)->stream(), 0));
					bad = true;
				}
				else { 
					shared_ptr<function_symbol> temp = static_pointer_cast<function_symbol>(res);
					ft = temp->function_symbol_type();
					vector<token> tstream = temp->stream();
					vector<token> temp_stream = temp->stream().size() > 10 ? vector<token>(tstream.begin(), tstream.begin() + 10) : tstream;
					if (temp->function_symbol_defined_kind() == function_stmt::defined_kind::KIND_DEFINITION) {
						p->report(error(error::kind::KIND_ERROR, "A function with the same name was already defined.", stream, 0));
						p->report(error(error::kind::KIND_ERROR, "Originally declared here.", temp_stream, 0));
						bad = true;
					}
					if(!bad)
						bad = !matching_function_symbol_and_partial_function_stmt(p, temp, make_shared<function_type>(ck, sk, rt, pl, p->next_function_reference_number()), fn, stream);
				}
				if (bad) {
					_contained_function_stmt = make_shared<function_stmt>(ft, fn, nullptr, vector<shared_ptr<stmt>>{}, function_stmt::defined_kind::KIND_NONE, fs, false,
						stream);
					_valid = false;
					return;
				}
				ft = make_shared<function_type>(ck, sk, rt, pl, ft->function_reference_number());
			}
			else
				ft = make_shared<function_type>(ck, sk, rt, pl, p->next_function_reference_number());
			if (cpar.token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
				p->report(error(error::kind::KIND_ERROR, "Expected a close parenthesis (')') to finish a parameter list.", stream, 0));
				_contained_function_stmt = make_shared<function_stmt>(ft, fn, nullptr, vector<shared_ptr<stmt>>{}, function_stmt::defined_kind::KIND_NONE, fs, false,
					stream);
				_valid = false;
				return;
			}
			stream.push_back(cpar);
			shared_ptr<function_symbol> fsym = nullptr;
			if (res != nullptr) {
				if (res->symbol_kind() != symbol::kind::KIND_FUNCTION) {
					p->report(error(error::kind::KIND_ERROR, "Function name conflicts conflicts with an existing symbol in the current scope.", stream, 0));
					_contained_function_stmt = make_shared<function_stmt>(ft, fn, nullptr, vector<shared_ptr<stmt>>{}, function_stmt::defined_kind::KIND_NONE, fs, false,
						stream);
					_valid = false;
					return;
				}
				fsym = static_pointer_cast<function_symbol>(res);
			}
			else {
				fsym = make_shared<function_symbol>(p->current_scope(), ft, fn, stream, function_stmt::defined_kind::KIND_DECLARATION);
				p->add_to_function_symbol_table(fsym);
				p->add_symbol_to_current_scope(fsym);
			}
			p->open_new_scope(fs);
			if (p->peek().token_kind() == token::kind::TOKEN_SEMICOLON) {
				stream.push_back(p->pop());
				_contained_function_stmt = make_shared<function_stmt>(ft, fn, fsym, vector<shared_ptr<stmt>>{}, function_stmt::defined_kind::KIND_DECLARATION, fs,
					true, stream);
				_valid = true;
				p->close_current_scope();
				return;
			}
			if (p->peek().token_kind() != token::kind::TOKEN_OPEN_BRACE) {
				p->report(error(error::kind::KIND_ERROR, "Expected an open brace to begin a function body.", stream, 0));
				_contained_function_stmt = make_shared<function_stmt>(ft, fn, fsym, vector<shared_ptr<stmt>>{}, function_stmt::defined_kind::KIND_NONE, fs, false,
					stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			stream.push_back(p->pop());
			fsym->set_function_symbol_defined_kind(function_stmt::defined_kind::KIND_DEFINITION);
			vector<shared_ptr<stmt>> fb;
			while (p->peek().token_kind() != token::kind::TOKEN_CLOSE_BRACE) {
				stmt_parser sp(p);
				if (!sp.valid()) {
					p->report(error(error::kind::KIND_ERROR, "Expected a valid statement inside a function body.",
						stream.size() > 10 ? vector<token>(stream.end() - 10, stream.end()) : stream , 0));
					_contained_function_stmt = make_shared<function_stmt>(ft, fn, fsym, fb, function_stmt::defined_kind::KIND_DEFINITION, fs, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				if (sp.contained_stmt()->stmt_kind() == stmt::kind::KIND_FUNCTION || sp.contained_stmt()->stmt_kind() == stmt::kind::KIND_STRUCT ||
					(sp.contained_stmt()->stmt_kind() == stmt::kind::KIND_NAMESPACE
						&& sp.contained_stmt()->stmt_namespace()->namespace_stmt_kind() != namespace_stmt::kind::KIND_ALIAS)) {
					p->report(error(error::kind::KIND_ERROR, "This statement cannot be nested within a function.",
						stream.size() > 10 ? vector<token>(stream.end() - 10, stream.end()) : stream , 0));
					_contained_function_stmt = make_shared<function_stmt>(ft, fn, fsym, fb, function_stmt::defined_kind::KIND_DEFINITION, fs, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				fb.push_back(sp.contained_stmt());
				vector<token> s_stream = sp.contained_stmt()->stream();
				stream.insert(stream.end(), s_stream.begin(), s_stream.end());
			}
			token cbrace = p->pop();
			if (cbrace.token_kind() != token::kind::TOKEN_CLOSE_BRACE) {
				p->report(error(error::kind::KIND_ERROR, "Expected a close brace ('}') to conclude a function declaration.", vector<token>{ cbrace }, 0));
				_contained_function_stmt = make_shared<function_stmt>(ft, fn, fsym, fb, function_stmt::defined_kind::KIND_DEFINITION, fs, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			stream.push_back(cbrace);
			_contained_function_stmt = make_shared<function_stmt>(ft, fn, fsym, fb, function_stmt::defined_kind::KIND_DEFINITION, fs, true, stream);
			_valid = true;
			p->close_current_scope();
		}

		function_stmt_parser::~function_stmt_parser() {

		}

		shared_ptr<function_stmt> function_stmt_parser::contained_function_stmt() {
			return _contained_function_stmt;
		}

		bool function_stmt_parser::valid() {
			return _valid;
		}

		bool primitive_types_equal(shared_ptr<parser> p, shared_ptr<type> t1, shared_ptr<type> t2) {
			if (t1 == nullptr || t2 == nullptr)
				return false;
			if (t1->type_kind() != type::kind::KIND_PRIMITIVE || t2->type_kind() != type::kind::KIND_PRIMITIVE) {
				p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return false;
			}
			if (t1->type_array_kind() != t2->type_array_kind() || t1->type_const_kind() != t2->type_const_kind() ||
				t1->type_static_kind() != t2->type_static_kind() || t1->type_kind() != t2->type_kind() ||
				t1->array_dimensions() != t2->array_dimensions())
				return false;
			shared_ptr<primitive_type> pt1 = static_pointer_cast<primitive_type>(t1), pt2 = static_pointer_cast<primitive_type>(t2);
			return pt1->primitive_type_kind() == pt2->primitive_type_kind() && pt1->primitive_type_sign_kind() == pt2->primitive_type_sign_kind();
		}

		bool primitive_types_compatible(shared_ptr<parser> p, shared_ptr<type> t1, shared_ptr<type> t2) {
			if (t1 == nullptr || t2 == nullptr)
				return false;
			if (t1->type_kind() != type::kind::KIND_PRIMITIVE || t2->type_kind() != type::kind::KIND_PRIMITIVE) {
				p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return false;
			}
			if (t1->type_array_kind() != t2->type_array_kind() || t1->array_dimensions() != t2->array_dimensions())
				return false;
			shared_ptr<primitive_type> pt1 = static_pointer_cast<primitive_type>(t1),
				pt2 = static_pointer_cast<primitive_type>(t2);
			if (t1->type_array_kind() == type::array_kind::KIND_ARRAY && pt1->primitive_type_kind() != pt2->primitive_type_kind()) return false;
			switch (pt1->primitive_type_kind()) {
			case primitive_type::kind::KIND_BOOL:
				return pt2->primitive_type_kind() == primitive_type::kind::KIND_BOOL;
				break;
			case primitive_type::kind::KIND_BYTE:
			case primitive_type::kind::KIND_CHAR:
			case primitive_type::kind::KIND_DOUBLE:
			case primitive_type::kind::KIND_FLOAT:
			case primitive_type::kind::KIND_INT:
			case primitive_type::kind::KIND_LONG:
			case primitive_type::kind::KIND_SHORT:
				return pt2->primitive_type_kind() != primitive_type::kind::KIND_VOID && pt2->primitive_type_kind() != primitive_type::kind::KIND_BOOL;
				break;
			case primitive_type::kind::KIND_VOID:
				return pt2->primitive_type_kind() == primitive_type::kind::KIND_VOID;
				break;
			}
			p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			return false;
		}

		bool matching_function_symbol_and_partial_function_stmt(shared_ptr<parser> p, shared_ptr<function_symbol> fsym, shared_ptr<function_type> ft, token fn,
			vector<token> s) {
			if (fsym == nullptr || fn.raw_text() != fsym->function_name().raw_text()) {
				p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return false;
			}
			vector<token> sym_stream = fsym->stream();
			shared_ptr<type> sym_rt = fsym->function_symbol_type()->return_type(), rt = ft->return_type();
			vector<shared_ptr<variable_declaration>> sym_pl = fsym->function_symbol_type()->parameter_list(),
				pl = ft->parameter_list();
			type::const_kind sym_ck = fsym->function_symbol_type()->type_const_kind(),
				ck = ft->type_const_kind();
			type::static_kind sym_sk = fsym->function_symbol_type()->type_static_kind(),
				sk = ft->type_static_kind();
			if (ck != sym_ck) {
				p->report(error(error::kind::KIND_ERROR, "The function declaration and the given function statement's const-ness do not match.", s, 0));
				p->report(error(error::kind::KIND_NOTE, "Originally declared here.", sym_stream, 0));
				return false;
			}
			if (sk != sym_sk) {
				p->report(error(error::kind::KIND_ERROR, "The function declaration and the given function statement's static-ness do not match.", s, 0));
				p->report(error(error::kind::KIND_NOTE, "Originally declared here.", sym_stream, 0));
				return false;
			}
			if(sym_rt->type_kind() != rt->type_kind()) {
				p->report(error(error::kind::KIND_ERROR, "The function declaration and the given function statement's return types do not match.", s, 0));
				p->report(error(error::kind::KIND_NOTE, "Originally declared here.", sym_stream, 0));
				return false;
			}
			if (sym_rt->type_kind() == type::kind::KIND_PRIMITIVE) {
				if (!primitive_types_equal(p, sym_rt, rt)) {
					p->report(error(error::kind::KIND_ERROR, "The function declaration and the given function statement's return types do not match.", s, 0));
					p->report(error(error::kind::KIND_NOTE, "Originally declared here.", sym_stream, 0));
					return false;
				}
			}
			else if (sym_rt->type_kind() == type::kind::KIND_STRUCT) {
				shared_ptr<struct_type> sym_rt_stype = static_pointer_cast<struct_type>(sym_rt), rt_stype = static_pointer_cast<struct_type>(rt);
				if (sym_rt_stype->struct_reference_number() != rt_stype->struct_reference_number() || sym_rt_stype->struct_name().raw_text() != rt_stype->struct_name().raw_text()) {
					p->report(error(error::kind::KIND_ERROR, "The function declaration and the given function statement's return types do not match.", s, 0));
					p->report(error(error::kind::KIND_NOTE, "Originally declared here.", sym_stream, 0));
					return false;
				}
			}
			else {
				p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return false;
			}
			if (sym_pl.size() != pl.size()) {
				p->report(error(error::kind::KIND_ERROR, "The function declaration and the given function statment's parameter lists differ.", s, 0));
				p->report(error(error::kind::KIND_ERROR, "Originally declared here.", sym_stream, 0));
				return false;
			}
			for (int i = 0; i < pl.size(); i++) {
				shared_ptr<type> spar_t = sym_pl[i]->variable_declaration_type(),
					par_t = pl[i]->variable_declaration_type();
				if (spar_t->type_kind() != par_t->type_kind()) {
					p->report(error(error::kind::KIND_ERROR, "The function declaration and the given function statment's parameter lists differ.", s, 0));
					p->report(error(error::kind::KIND_ERROR, "Originally declared here.", sym_stream, 0));
					return false;
				}
				if (spar_t->type_kind() == type::kind::KIND_PRIMITIVE) {
					if (!primitive_types_equal(p, spar_t, par_t)) {
						p->report(error(error::kind::KIND_ERROR, "The function declaration and the given function statment's parameter lists differ.", s, 0));
						p->report(error(error::kind::KIND_ERROR, "Originally declared here.", sym_stream, 0));
						return false;
					}
				}
				else if (spar_t->type_kind() == type::kind::KIND_STRUCT) {
					shared_ptr<struct_type> spar_t_stype = static_pointer_cast<struct_type>(spar_t),
						par_t_stype = static_pointer_cast<struct_type>(par_t);
					if (spar_t_stype->struct_reference_number() != par_t_stype->struct_reference_number() || spar_t_stype->struct_name().raw_text() != par_t_stype->struct_name().raw_text() ||
						spar_t_stype->type_array_kind() != par_t_stype->type_array_kind() || spar_t_stype->array_dimensions() != par_t_stype->array_dimensions()) {
						p->report(error(error::kind::KIND_ERROR, "The function declaration and the given function statment's parameter lists differ.", s, 0));
						p->report(error(error::kind::KIND_ERROR, "Originally declared here.", sym_stream, 0));
						return false;
					}
				}
				else if (spar_t->type_kind() == type::kind::KIND_FUNCTION) {
					if (!matching_function_types(p, spar_t, par_t, true)) {
						p->report(error(error::kind::KIND_ERROR, "The function declaration and the given function statment's parameter lists differ.", s, 0));
						p->report(error(error::kind::KIND_ERROR, "Originally declared here.", sym_stream, 0));
						return false;
					}
				}
				else {
					p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return false;
				}
			}
			return true;
		}

		shared_ptr<type> deduce_array_initializer_type(shared_ptr<parser> p, vector<shared_ptr<assignment_expression>> ai, vector<token> stream) {
			if (ai.size() == 0) {
				p->report(error(error::kind::KIND_ERROR, "Expected at least a single expression to deduce the type of an array initializer.", stream, 0));
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE, 0);
			}
			shared_ptr<type> deduced = ai[0]->assignment_expression_type();
			for (int i = 1; i < ai.size(); i++) {
				shared_ptr<assignment_expression> e = ai[i];
				shared_ptr<type> t = ai[i]->assignment_expression_type();
				if (deduced->type_kind() != t->type_kind() || deduced->type_array_kind() != t->type_array_kind() ||
					deduced->array_dimensions() != t->array_dimensions()) {
					p->report(error(error::kind::KIND_ERROR, "Expected comparable types in an array initializer list.", e->stream(), 0));
					p->report(error(error::kind::KIND_NOTE, "From this initializer list.", stream, 0));
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE, 0);
				}
				if (deduced->type_kind() == type::kind::KIND_PRIMITIVE) {
					shared_ptr<primitive_type> dpt = static_pointer_cast<primitive_type>(deduced),
						pt = static_pointer_cast<primitive_type>(t);
					if(dpt->primitive_type_kind() == primitive_type::kind::KIND_VOID || pt->primitive_type_kind() == primitive_type::kind::KIND_VOID) {
						p->report(error(error::kind::KIND_ERROR, "Expected non-void types in an array initializer list.", e->stream(), 0));
						p->report(error(error::kind::KIND_NOTE, "From this initializer list.", stream, 0));
						return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE, 0);
					}
					if (dpt->primitive_type_kind() != pt->primitive_type_kind() || dpt->primitive_type_sign_kind() != pt->primitive_type_sign_kind()) {
						p->report(error(error::kind::KIND_ERROR, "Expected comparable types in an array initializer list.", e->stream(), 0));
						p->report(error(error::kind::KIND_NOTE, "From this initializer list.", stream, 0));
						return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE, 0);
					}
					deduced = make_shared<primitive_type>(dpt->primitive_type_kind(), type::const_kind::KIND_NON_CONST, type::static_kind::KIND_NON_STATIC,
						dpt->primitive_type_sign_kind(), dpt->array_dimensions());
				}
				else if (deduced->type_kind() == type::kind::KIND_STRUCT) {
					shared_ptr<struct_type> dst = static_pointer_cast<struct_type>(deduced),
						st = static_pointer_cast<struct_type>(t);
					if (dst->struct_reference_number() != st->struct_reference_number() || dst->struct_name().raw_text() != st->struct_name().raw_text()) {
						p->report(error(error::kind::KIND_ERROR, "Expected comparable types in an array initializer list.", e->stream(), 0));
						p->report(error(error::kind::KIND_NOTE, "From this initializer list.", stream, 0));
						return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE, 0);
					}
					deduced = make_shared<struct_type>(type::const_kind::KIND_NON_CONST, type::static_kind::KIND_NON_STATIC, dst->struct_name(), dst->struct_reference_number(), true,
						dst->array_dimensions());
				}
				else if (deduced->type_kind() == type::kind::KIND_FUNCTION) {
					shared_ptr<function_type> dft = static_pointer_cast<function_type>(t);
					if (!matching_function_types(p, deduced, t, true)) {
						p->report(error(error::kind::KIND_ERROR, "Expected comparable types in an array initializer list.", e->stream(), 0));
						p->report(error(error::kind::KIND_NOTE, "From this initializer list.", stream, 0));
						return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE, 0);
					}
					dft = make_shared<function_type>(type::const_kind::KIND_NON_CONST, type::static_kind::KIND_NON_STATIC, dft->return_type(), dft->parameter_list(),
						dft->function_reference_number(), dft->array_dimensions());
				}
				else {
					p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE, 0);
				}
			}
			if (deduced->type_kind() == type::kind::KIND_PRIMITIVE) {
				shared_ptr<primitive_type> d = static_pointer_cast<primitive_type>(deduced);
				return make_shared<primitive_type>(d->primitive_type_kind(), type::const_kind::KIND_NON_CONST, type::static_kind::KIND_NON_STATIC,
					d->primitive_type_sign_kind(), d->array_dimensions() + 1);
			}
			else if (deduced->type_kind() == type::kind::KIND_STRUCT) {
				shared_ptr<struct_type> d = static_pointer_cast<struct_type>(deduced);
				return make_shared<struct_type>(type::const_kind::KIND_NON_CONST, type::static_kind::KIND_NON_STATIC, d->struct_name(), d->struct_reference_number(), true,
					d->array_dimensions() + 1);
			}
			else if (deduced->type_kind() == type::kind::KIND_FUNCTION) {
				shared_ptr<function_type> d = static_pointer_cast<function_type>(deduced);
				return make_shared<function_type>(type::const_kind::KIND_NON_CONST, type::static_kind::KIND_NON_STATIC, d->return_type(), d->parameter_list(),
					d->function_reference_number(), d->array_dimensions() + 1);
			}
			else {
				p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE, 0);
			}
		}

		return_stmt_parser::return_stmt_parser(shared_ptr<parser> p) : _contained_return_stmt(nullptr), _valid(false) {
			if (p->peek().token_kind() != token::kind::TOKEN_RETURN) {
				_contained_return_stmt = make_shared<return_stmt>(false, vector<token>{});
				_valid = false;
				return;
			}
			vector<token> stream;
			stream.push_back(p->pop());
			shared_ptr<scope> fs = p->within_scope(scope::kind::KIND_FUNCTION);
			if (fs == nullptr) {
				p->report(error(error::kind::KIND_ERROR, "You can only have a return statement within a function.", stream, 0));
				_contained_return_stmt = make_shared<return_stmt>(false, stream);
				_valid = false;
				return;
			}
			shared_ptr<function_scope> f_scope = static_pointer_cast<function_scope>(fs);
			if (p->peek().token_kind() == token::kind::TOKEN_SEMICOLON) {
				stream.push_back(p->pop());
				shared_ptr<type> rt = f_scope->return_type();
				if (rt->type_kind() != type::kind::KIND_PRIMITIVE || static_pointer_cast<primitive_type>(rt)->primitive_type_kind() != primitive_type::kind::KIND_VOID) {
					p->report(error(error::kind::KIND_ERROR, "You can only return nothing from a void function.", stream, 0));
					_contained_return_stmt = make_shared<return_stmt>(false, stream);
				}
				else
					_contained_return_stmt = make_shared<return_stmt>(true, stream);
				_valid = _contained_return_stmt->valid();
				return;
			}
			shared_ptr<expression> rv = expression_parser(p).contained_expression();
			if (!rv->valid()) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid expression for a return statement.", stream, 0));
				_contained_return_stmt = make_shared<return_stmt>(rv, false, stream);
				_valid = false;
				return;
			}
			vector<token> rv_stream = rv->stream();
			stream.insert(stream.end(), rv_stream.begin(), rv_stream.end());
			shared_ptr<type> rt = rv->expression_type(), frt = f_scope->return_type();
			if (rt->type_array_kind() != frt->type_array_kind() || rt->array_dimensions() != frt->array_dimensions() ||
				rt->type_kind() != frt->type_kind()) {
				p->report(error(error::kind::KIND_ERROR, "Expected matching types to return from a function.", stream, 0));
				_contained_return_stmt = make_shared<return_stmt>(rv, false, stream);
				_valid = false;
				return;
			}
			if(rt->type_array_kind() == type::array_kind::KIND_ARRAY && rt->type_const_kind() == type::const_kind::KIND_CONST &&
				frt->type_const_kind() == type::const_kind::KIND_NON_CONST) {
				p->report(error(error::kind::KIND_ERROR, "Cannot return a const array from a function returning a non-const array.", stream, 0));
				_contained_return_stmt = make_shared<return_stmt>(rv, false, stream);
				_valid = false;
				return;
			}
			if (rt->type_kind() == type::kind::KIND_PRIMITIVE) {
				shared_ptr<primitive_type> prt = static_pointer_cast<primitive_type>(rt), pfrt = static_pointer_cast<primitive_type>(frt);
				if (pfrt->primitive_type_kind() == primitive_type::kind::KIND_VOID && prt->primitive_type_kind() != pfrt->primitive_type_kind()) {
					p->report(error(error::kind::KIND_ERROR, "Cannot return a non-void type from a void function.", stream, 0));
					_contained_return_stmt = make_shared<return_stmt>(rv, false, stream);
					_valid = false;
					return;
				}
				if (prt->primitive_type_kind() == primitive_type::kind::KIND_VOID && pfrt->primitive_type_kind() != primitive_type::kind::KIND_VOID) {
					p->report(error(error::kind::KIND_ERROR, "Cannot return void from a non-void function.", stream, 0));
					_contained_return_stmt = make_shared<return_stmt>(rv, false, stream);
					_valid = false;
					return;
				}
				if (pfrt->primitive_type_kind() == primitive_type::kind::KIND_BOOL && prt->primitive_type_kind() != pfrt->primitive_type_kind()) {
					p->report(error(error::kind::KIND_ERROR, "Cannot return a non-bool type from a bool function.", stream, 0));
					_contained_return_stmt = make_shared<return_stmt>(rv, false, stream);
					_valid = false;
					return;
				}
				if (prt->primitive_type_kind() == primitive_type::kind::KIND_BOOL && pfrt->primitive_type_kind() != primitive_type::kind::KIND_BOOL) {
					p->report(error(error::kind::KIND_ERROR, "Cannot return bool from a non-bool function.", stream, 0));
					_contained_return_stmt = make_shared<return_stmt>(rv, false, stream);
					_valid = false;
					return;
				}
				if (rt->type_array_kind() == type::array_kind::KIND_ARRAY && prt->primitive_type_kind() != pfrt->primitive_type_kind()) {
					p->report(error(error::kind::KIND_ERROR, "Cannot return incompatible array types from a function.", stream, 0));
					_contained_return_stmt = make_shared<return_stmt>(rv, false, stream);
					_valid = false;
					return;
				}
			}
			else if (rt->type_kind() == type::kind::KIND_STRUCT) {
				shared_ptr<struct_type> srt = static_pointer_cast<struct_type>(rt), sfrt = static_pointer_cast<struct_type>(frt);
				if (srt->struct_reference_number() != sfrt->struct_reference_number() || srt->struct_name().raw_text() != sfrt->struct_name().raw_text()) {
					p->report(error(error::kind::KIND_ERROR, "A return statement inside of a function cannot contradict a function declaration's return type.", stream, 0));
					_contained_return_stmt = make_shared<return_stmt>(rv, false, stream);
					_valid = false;
					return;
				}
			}
			else if (rt->type_kind() == type::kind::KIND_FUNCTION) {
				if (!matching_function_types(p, rt, frt)) {
					p->report(error(error::kind::KIND_ERROR, "A return statement inside of a function cannot contradict a function declaration's return type.", stream, 0));
					_contained_return_stmt = make_shared<return_stmt>(rv, false, stream);
					_valid = false;
					return;
				}
			}
			else {
				p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				_contained_return_stmt = make_shared<return_stmt>(rv, false, stream);
				_valid = false;
				return;
			}
			if (p->peek().token_kind() != token::kind::TOKEN_SEMICOLON) {
				p->report(error(error::kind::KIND_ERROR, "Expected a semicolon to end a return statement.", stream, 0));
				_contained_return_stmt = make_shared<return_stmt>(rv, false, stream);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			_contained_return_stmt = make_shared<return_stmt>(rv, true, stream);
			_valid = true;
		}

		return_stmt_parser::~return_stmt_parser() {
			
		}
		
		shared_ptr<return_stmt> return_stmt_parser::contained_return_stmt() {
			return _contained_return_stmt;
		}

		bool return_stmt_parser::valid() {
			return _valid;
		}

		while_stmt_parser::while_stmt_parser(shared_ptr<parser> p) : _contained_while_stmt(nullptr), _valid(false) {
			if (p->peek().token_kind() != token::kind::TOKEN_WHILE) {
				_contained_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_NONE, nullptr, nullptr, false, vector<token>{});
				_valid = false;
				return;
			}
			vector<token> stream;
			stream.push_back(p->pop());
			if (p->peek().token_kind() != token::kind::TOKEN_OPEN_PARENTHESIS) {
				p->report(error(error::kind::KIND_ERROR, "Expected an open parenthesis ('(') after the 'while' keyword.", stream, 0));
				_contained_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_WHILE, nullptr, nullptr, false, stream);	
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			shared_ptr<expression> e = expression_parser(p).contained_expression();
			if (!e->valid()) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid expression to test for in a while-loop condition.", stream, 0));
				_contained_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_WHILE, e, nullptr, false, stream);
				_valid = false;
				return;
			}
			shared_ptr<type> cond_type = e->expression_type();
			if (cond_type->array_dimensions() != 0 || cond_type->type_array_kind() != type::array_kind::KIND_NON_ARRAY ||
				cond_type->type_kind() != type::kind::KIND_PRIMITIVE ||
				static_pointer_cast<primitive_type>(cond_type)->primitive_type_kind() != primitive_type::kind::KIND_BOOL ||
				static_pointer_cast<primitive_type>(cond_type)->primitive_type_sign_kind() != primitive_type::sign_kind::KIND_NONE) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid boolean expression to test for in a valid while-loop condition.",
					stream, 0));
				_contained_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_WHILE, e, nullptr, false, stream);
				_valid = false;
				return;
			}
			vector<token> cond_stream = e->stream();
			stream.insert(stream.end(), cond_stream.begin(), cond_stream.end());
			if (p->peek().token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
				p->report(error(error::kind::KIND_ERROR, "Expected a close parenthesis after a while loop condition.", stream, stream.size() - 1));
				_contained_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_WHILE, e, nullptr, false, stream);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			shared_ptr<scope> loop_scope = make_shared<scope>(scope::kind::KIND_LOOP, p->current_scope());
			p->open_new_scope(loop_scope);
			p->open_new_scope(make_shared<block_scope>(p->next_block_scope_number(), p->current_scope()));
			stmt_parser sp(p);
			shared_ptr<stmt> s = sp.contained_stmt();
			if (!s->valid()) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid statement inside a while loop.", stream, stream.size() - 1));
				_contained_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_WHILE, e, s, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			vector<token> s_stream = s->stream();
			stream.insert(stream.end(), s_stream.begin(), s_stream.end());
			if (!valid_statement_inside_block_scope(s)) {
				p->report(error(error::kind::KIND_ERROR, "This statement cannot be placed in a while statement.", stream, stream.size() - 1));
				_contained_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_WHILE, e, s, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			p->close_current_scope();
			p->close_current_scope();
			_contained_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_WHILE, e, s, true, stream);
			_valid = true;
		}

		while_stmt_parser::~while_stmt_parser() {

		}

		shared_ptr<while_stmt> while_stmt_parser::contained_while_stmt() {
			return _contained_while_stmt;
		}

		bool while_stmt_parser::valid() {
			return _valid;
		}

		bool valid_statement_inside_block_scope(shared_ptr<stmt> s) {
			if (s == nullptr || !s->valid() || s->stmt_kind() == stmt::kind::KIND_NONE)
				return false;
			if (s->stmt_kind() == stmt::kind::KIND_NAMESPACE)
				return s->stmt_namespace()->namespace_stmt_kind() == namespace_stmt::kind::KIND_ALIAS;
			return s->stmt_kind() != stmt::kind::KIND_FUNCTION && s->stmt_kind() != stmt::kind::KIND_STRUCT;
		}

		block_stmt_parser::block_stmt_parser(shared_ptr<parser> p) : _contained_block_stmt(nullptr), _valid(false) {
			if (p->peek().token_kind() != token::kind::TOKEN_OPEN_BRACE) {
				_contained_block_stmt = make_shared<block_stmt>(vector<shared_ptr<stmt>>{}, false, vector<token>{});
				_valid = false;
				return;
			}
			vector<token> stream;
			stream.push_back(p->pop());
			vector<shared_ptr<stmt>> stmt_list;
			shared_ptr<block_scope> bl = make_shared<block_scope>(p->next_block_scope_number(), p->current_scope());
			p->open_new_scope(bl);
			while (p->peek().token_kind() != token::kind::TOKEN_CLOSE_BRACE) {
				shared_ptr<stmt> s = stmt_parser(p).contained_stmt();
				if (!s->valid()) {
					p->report(error(error::kind::KIND_ERROR, "Expected a valid statement inside a block scope.", stream, 0));
					_contained_block_stmt = make_shared<block_stmt>(stmt_list, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				if (!valid_statement_inside_block_scope(s)) {
					p->report(error(error::kind::KIND_ERROR, "Did not expect this kind of statement inside a block scope.", stream, stream.size() - 1));
					_contained_block_stmt = make_shared<block_stmt>(stmt_list, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				vector<token> s_stream = s->stream();
				stream.insert(stream.end(), s_stream.begin(), s_stream.end());
				stmt_list.push_back(s);
			}
			if (p->peek().token_kind() != token::kind::TOKEN_CLOSE_BRACE) {
				p->report(error(error::kind::KIND_ERROR, "Expected a close brace ('}') here to finish a block scope.", stream, 0));
				_contained_block_stmt = make_shared<block_stmt>(stmt_list, false, stream);
				_valid = false;
				p->close_current_scope();
				return;
			}
			stream.push_back(p->pop());
			p->close_current_scope();
			_contained_block_stmt = make_shared<block_stmt>(stmt_list, true, stream);
			_valid = true;
		}

		block_stmt_parser::~block_stmt_parser() {

		}

		shared_ptr<block_stmt> block_stmt_parser::contained_block_stmt() {
			return _contained_block_stmt;
		}

		bool block_stmt_parser::valid() {
			return _valid;
		}

		break_continue_stmt_parser::break_continue_stmt_parser(shared_ptr<parser> p) : _contained_break_continue_stmt(nullptr), _valid(false) {
			vector<token> stream;
			break_continue_stmt::kind bk = break_continue_stmt::kind::KIND_NONE;
			if (p->peek().token_kind() == token::kind::TOKEN_BREAK)
				bk = break_continue_stmt::kind::KIND_BREAK;
			else if (p->peek().token_kind() == token::kind::TOKEN_CONTINUE)
				bk = break_continue_stmt::kind::KIND_CONTINUE;
			else {
				_valid = false;
				_contained_break_continue_stmt = make_shared<break_continue_stmt>(break_continue_stmt::kind::KIND_NONE, stream, _valid);
				return;
			}
			stream.push_back(p->pop());
			if (p->peek().token_kind() != token::kind::TOKEN_SEMICOLON) {
				p->report(error(error::kind::KIND_ERROR, "Expected a semicolon to terminate a 'break' or 'continue' statement.", stream, 0));
				_valid = false;
				_contained_break_continue_stmt = make_shared<break_continue_stmt>(bk, stream, _valid);
				return;
			}
			stream.push_back(p->pop());
			if (p->within_scope(scope::kind::KIND_LOOP) || (bk == break_continue_stmt::kind::KIND_BREAK && p->within_scope(scope::kind::KIND_SWITCH)));
			else {
				p->report(error(error::kind::KIND_ERROR, "Cannot 'break' from a non-loop or non-switch scope or 'continue' from a non-loop scope.", stream, 0));
				_valid = false;
				_contained_break_continue_stmt = make_shared<break_continue_stmt>(bk, stream, _valid);
				return;
			}
			_valid = true;
			_contained_break_continue_stmt = make_shared<break_continue_stmt>(bk, stream, _valid);
		}

		break_continue_stmt_parser::~break_continue_stmt_parser() {

		}

		shared_ptr<break_continue_stmt> break_continue_stmt_parser::contained_break_continue_stmt() {
			return _contained_break_continue_stmt;
		}

		bool break_continue_stmt_parser::valid() {
			return _valid;
		}

		for_stmt_parser::for_stmt_parser(shared_ptr<parser> p) : _contained_for_stmt(nullptr), _valid(false) {
			for_stmt::initializer_present_kind ip = for_stmt::initializer_present_kind::INITIALIZER_NONE;
			for_stmt::condition_present_kind cp = for_stmt::condition_present_kind::CONDITION_NONE;
			for_stmt::update_present_kind up = for_stmt::update_present_kind::UPDATE_NONE;
			vector<shared_ptr<variable_declaration>> i;
			shared_ptr<expression> c = nullptr, u = nullptr;
			shared_ptr<stmt> s = nullptr;
			vector<token> stream;
			if (p->peek().token_kind() != token::kind::TOKEN_FOR) {
				_contained_for_stmt = make_shared<for_stmt>(i, ip, c, cp, u, up, s, false, stream);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			if (p->peek().token_kind() != token::kind::TOKEN_OPEN_PARENTHESIS) {
				p->report(error(error::kind::KIND_ERROR, "Expected an open parenthesis ('(') following the 'for' keyword.", stream, stream.size() - 1));
				_contained_for_stmt = make_shared<for_stmt>(i, ip, c, cp, u, up, s, false, stream);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			shared_ptr<scope> loop_scope = make_shared<scope>(scope::kind::KIND_LOOP, p->current_scope());
			p->open_new_scope(loop_scope);
			shared_ptr<block_scope> for_scope = make_shared<block_scope>(p->next_block_scope_number(), p->current_scope());
			p->open_new_scope(for_scope);
			if (p->peek().token_kind() == token::kind::TOKEN_SEMICOLON)
				ip = for_stmt::initializer_present_kind::INITIALIZER_NOT_PRESENT;
			else {
				ip = for_stmt::initializer_present_kind::INITIALIZER_PRESENT;
				variable_declaration_parser vp(p);
				i = vp.contained_variable_declaration_list();
				vector<token> vd_stream = vp.stream();
				if (!vp.valid()) {
					p->report(error(error::kind::KIND_ERROR, "Expected a valid variable declaration list inside a for loop initializer.", vd_stream, 0));
					_contained_for_stmt = make_shared<for_stmt>(i, ip, c, cp, u, up, s, false, stream);
					_valid = false;
					p->close_current_scope();
					p->close_current_scope();
					return;
				}
				stream.insert(stream.end(), vd_stream.begin(), vd_stream.end());
				if (p->peek().token_kind() != token::kind::TOKEN_SEMICOLON) {
					p->report(error(error::kind::KIND_ERROR, "Expected a semicolon (';') to terminate a for loop initializer.", stream, stream.size() - 1));
					_contained_for_stmt = make_shared<for_stmt>(i, ip, c, cp, u, up, s, false, stream);
					_valid = false;
					p->close_current_scope();
					p->close_current_scope();
					return;
				}
			}
			stream.push_back(p->pop());
			if (p->peek().token_kind() == token::kind::TOKEN_SEMICOLON)
				cp = for_stmt::condition_present_kind::CONDITION_NOT_PRESENT;
			else {
				cp = for_stmt::condition_present_kind::CONDITION_PRESENT;
				expression_parser ep(p);
				c = ep.contained_expression();
				vector<token> c_stream = c->stream();
				if (!c->valid()) {
					p->report(error(error::kind::KIND_ERROR, "Expected a valid expression for a condition of a for-loop.", c_stream, 0));
					_contained_for_stmt = make_shared<for_stmt>(i, ip, c, cp, u, up, s, false, stream);
					_valid = false;
					p->close_current_scope();
					p->close_current_scope();
					return;
				}
				shared_ptr<type> cond_type = c->expression_type();
				bool invalid_type = false;
				if (cond_type->type_kind() == type::kind::KIND_PRIMITIVE && cond_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY &&
					cond_type->array_dimensions() == 0 && cond_type->valid()) {
					if (static_pointer_cast<primitive_type>(cond_type)->primitive_type_kind() != primitive_type::kind::KIND_BOOL ||
						static_pointer_cast<primitive_type>(cond_type)->primitive_type_sign_kind() != primitive_type::sign_kind::KIND_NONE)
						invalid_type = true;
				}
				else invalid_type = true;
				if (invalid_type) {
					p->report(error(error::kind::KIND_ERROR, "Expected a boolean expression to compare for in a for loop condition.", c_stream, 0));
					_contained_for_stmt = make_shared<for_stmt>(i, ip, c, cp, u, up, s, false, stream);
					_valid = false;
					p->close_current_scope();
					p->close_current_scope();
					return;
				}
				stream.insert(stream.end(), c_stream.begin(), c_stream.end());
				if (p->peek().token_kind() != token::kind::TOKEN_SEMICOLON) {
					p->report(error(error::kind::KIND_ERROR, "Expected a semicolon (';') to terminate a for loop condition.", stream, stream.size() - 1));
					_contained_for_stmt = make_shared<for_stmt>(i, ip, c, cp, u, up, s, false, stream);
					_valid = false;
					p->close_current_scope();
					p->close_current_scope();
					return;
				}
			}
			stream.push_back(p->pop());
			if (p->peek().token_kind() == token::kind::TOKEN_CLOSE_PARENTHESIS)
				up = for_stmt::update_present_kind::UPDATE_NOT_PRESENT;
			else {
				up = for_stmt::update_present_kind::UPDATE_PRESENT;
				expression_parser ep(p);
				u = ep.contained_expression();
				vector<token> u_stream = u->stream();
				if (!u->valid()) {
					p->report(error(error::kind::KIND_ERROR, "Expected a valid expression for an update part of the for-loop.", u_stream, 0));
					_contained_for_stmt = make_shared<for_stmt>(i, ip, c, cp, u, up, s, false, stream);
					_valid = false;
					p->close_current_scope();
					p->close_current_scope();
					return;
				}
				stream.insert(stream.end(), u_stream.begin(), u_stream.end());
				if (p->peek().token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
					p->report(error(error::kind::KIND_ERROR, "Expected a close parenthesis (')') to terminate a for loop header.", stream, stream.size() - 1));
					_contained_for_stmt = make_shared<for_stmt>(i, ip, c, cp, u, up, s, false, stream);
					_valid = false;
					p->close_current_scope();
					p->close_current_scope();
					return;
				}
			}
			stream.push_back(p->pop());
			s = stmt_parser(p).contained_stmt();
			vector<token> s_stream = s->stream();
			if (!s->valid()) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid statement after a for-loop header.", stream, stream.size() - 1));
				_contained_for_stmt = make_shared<for_stmt>(i, ip, c, cp, u, up, s, false, stream);
				_valid = false;
				p->close_current_scope();
				p->close_current_scope();
				return;
			}
			if (!valid_statement_inside_block_scope(s)) {
				p->report(error(error::kind::KIND_ERROR, "Cannot nest this kind of statement inside a for-loop.", s_stream, 0));
				_valid = false;
				_contained_for_stmt = make_shared<for_stmt>(i, ip, c, cp, u, up, s, false, stream);
				p->close_current_scope();
				p->close_current_scope();
				return;
			}
			p->close_current_scope();
			p->close_current_scope();
			_contained_for_stmt = make_shared<for_stmt>(i, ip, c, cp, u, up, s, true, stream);
			_valid = true;
		}

		for_stmt_parser::~for_stmt_parser() {

		}

		shared_ptr<for_stmt> for_stmt_parser::contained_for_stmt() {
			return _contained_for_stmt;
		}

		bool for_stmt_parser::valid() {
			return _valid;
		}

		do_while_stmt_parser::do_while_stmt_parser(shared_ptr<parser> p) : _contained_do_while_stmt(nullptr), _valid(false) {
			if (p->peek().token_kind() != token::kind::TOKEN_DO) {
				_contained_do_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_NONE, nullptr, nullptr, false, vector<token>{});
				_valid = false;
				return;
			}
			vector<token> stream;
			stream.push_back(p->pop());
			shared_ptr<scope> loop_scope = make_shared<scope>(scope::kind::KIND_LOOP, p->current_scope());
			p->open_new_scope(loop_scope);
			shared_ptr<scope> b_scope = make_shared<block_scope>(p->next_block_scope_number(), p->current_scope());
			p->open_new_scope(b_scope);
			shared_ptr<stmt> s = stmt_parser(p).contained_stmt();
			vector<token> s_stream = s->stream();
			if (!s->valid()) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid statement inside a do-while loop.", s_stream, s_stream.size() - 1));
				_contained_do_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_DO_WHILE, nullptr, s, false, stream);
				_valid = false;
				p->close_current_scope();
				p->close_current_scope();
				return;
			}
			if (!valid_statement_inside_block_scope(s)) {
				p->report(error(error::kind::KIND_ERROR, "Cannot nest this kind of statement inside a do-while loop.", s_stream, s_stream.size() - 1));
				_contained_do_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_DO_WHILE, nullptr, s, false, stream);
				_valid = false;
				p->close_current_scope();
				p->close_current_scope();
				return;
			}
			stream.insert(stream.end(), s_stream.begin(), s_stream.end());
			p->close_current_scope();
			p->close_current_scope();
			if (p->peek().token_kind() != token::kind::TOKEN_WHILE) {
				p->report(error(error::kind::KIND_ERROR, "Expected a 'while' in a do-while loop.", stream, stream.size() - 1));
				_contained_do_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_DO_WHILE, nullptr, s, false, stream);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			if (p->peek().token_kind() != token::kind::TOKEN_OPEN_PARENTHESIS) {
				p->report(error(error::kind::KIND_ERROR, "Expected an open parenthesis ('(') preceeding a 'while' keyword in do-while loop.", stream, stream.size() - 1));
				_contained_do_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_DO_WHILE, nullptr, s, false, stream);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			shared_ptr<expression> e = expression_parser(p).contained_expression();
			if (!e->valid()) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid expression for a condition of a do-while loop.", stream, stream.size() - 1));
				_contained_do_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_DO_WHILE, e, s, false, stream);
				_valid = false;
				return;
			}
			vector<token> e_stream = e->stream();
			stream.insert(stream.end(), e_stream.begin(), e_stream.end());
			shared_ptr<type> e_type = e->expression_type();
			bool invalid_type = false;
			if (e_type->type_kind() != type::kind::KIND_PRIMITIVE || e_type->type_array_kind() == type::array_kind::KIND_ARRAY ||
				e_type->array_dimensions() > 0) invalid_type = true;
			else {
				shared_ptr<primitive_type> ept = static_pointer_cast<primitive_type>(e_type);
				if (ept->primitive_type_kind() != primitive_type::kind::KIND_BOOL || ept->primitive_type_sign_kind() != primitive_type::sign_kind::KIND_NONE)
					invalid_type = true;
			}
			if (invalid_type) {
				p->report(error(error::kind::KIND_ERROR, "Expected a boolean type for the condition of a do-while loop.", e_stream, 0));
				_contained_do_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_DO_WHILE, e, s, false, stream);
				_valid = false;
				return;
			}
			if (p->peek().token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
				p->report(error(error::kind::KIND_ERROR, "Expected a close parenthesis (')') after the condition of a do-while loop.", stream, stream.size() - 1));
				_contained_do_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_DO_WHILE, e, s, false, stream);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			if (p->peek().token_kind() != token::kind::TOKEN_SEMICOLON) {
				p->report(error(error::kind::KIND_ERROR, "Expected a semicolon (';') to terminate a do-while loop.", stream, stream.size() - 1));
				_contained_do_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_DO_WHILE, e, s, false, stream);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			_contained_do_while_stmt = make_shared<while_stmt>(while_stmt::while_kind::KIND_DO_WHILE, e, s, true, stream);
			_valid = true;
			return;
		}

		do_while_stmt_parser::~do_while_stmt_parser() {

		}

		shared_ptr<while_stmt> do_while_stmt_parser::contained_do_while_stmt() {
			return _contained_do_while_stmt;
		}

		bool do_while_stmt_parser::valid() {
			return _valid;
		}

		case_default_stmt_parser::case_default_stmt_parser(shared_ptr<parser> p) : _contained_case_default_stmt(nullptr), _valid(false) {
			if (p->peek().token_kind() != token::kind::TOKEN_CASE && p->peek().token_kind() != token::kind::TOKEN_DEFAULT) {
				_contained_case_default_stmt = make_shared<case_default_stmt>(case_default_stmt::kind::KIND_NONE, nullptr, nullptr, false, -1, vector<token>{});
				_valid = false;
				return;
			}
			int lnumber = p->next_case_default_stmt_label_number();
			token which = p->pop();
			vector<token> stream;
			stream.push_back(which);
			case_default_stmt::kind k = which.token_kind() == token::kind::TOKEN_CASE ? case_default_stmt::kind::KIND_CASE : case_default_stmt::kind::KIND_DEFAULT;
			shared_ptr<scope> test_scope = p->within_scope(scope::kind::KIND_SWITCH);
			if (!test_scope) {
				p->report(error(error::kind::KIND_ERROR, "Can only have a 'case' or 'default' statement within a switch block.", stream, 0));
				_contained_case_default_stmt = make_shared<case_default_stmt>(k, nullptr, nullptr, false, lnumber, stream);
				_valid = false;
				return;
			}
			shared_ptr<switch_scope> p_scope = static_pointer_cast<switch_scope>(test_scope);
			shared_ptr<expression> ce = nullptr;
			if (k == case_default_stmt::kind::KIND_CASE) {
				ce = expression_parser(p).contained_expression();
				vector<token> ce_stream = ce->stream();
				if (!ce->valid()) {
					p->report(error(error::kind::KIND_ERROR, "Expected a valid expression for a case statement.", stream, 0));
					_contained_case_default_stmt = make_shared<case_default_stmt>(k, ce, nullptr, false, lnumber, stream);
					_valid = false;
					return;
				}
				stream.insert(stream.end(), ce_stream.begin(), ce_stream.end());
				shared_ptr<type> ce_type = ce->expression_type();
				shared_ptr<type> p_type = p_scope->parent_expression()->expression_type();
				if (ce_type->type_kind() != p_type->type_kind()) {
					p->report(error(error::kind::KIND_ERROR, "Expected compatible types for a switch scope's case.", stream, 0));
					p->report(error(error::kind::KIND_NOTE, "Original expression being compared against is here.", p_scope->parent_expression()->stream(), 0));
					_contained_case_default_stmt = make_shared<case_default_stmt>(k, ce, nullptr, false, lnumber, stream);
					_valid = false;
					return;
				}
				if (ce_type->type_kind() == type::kind::KIND_PRIMITIVE) {
					shared_ptr<primitive_type> ce_ptype = static_pointer_cast<primitive_type>(ce_type),
						p_ptype = static_pointer_cast<primitive_type>(p_type);
					bool valid_case = true;
					if (p_ptype->primitive_type_kind() == primitive_type::kind::KIND_VOID)
						p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (ce_type->array_dimensions() != p_ptype->array_dimensions() || ce_ptype->type_array_kind() != p_ptype->type_array_kind()) valid_case = false;
					if ((p_ptype->primitive_type_kind() == primitive_type::kind::KIND_BOOL && ce_ptype->primitive_type_kind() != primitive_type::kind::KIND_BOOL) ||
						ce_ptype->primitive_type_kind() == primitive_type::kind::KIND_VOID ||
						(p_ptype->primitive_type_kind() != primitive_type::kind::KIND_BOOL && ce_ptype->primitive_type_kind() == primitive_type::kind::KIND_BOOL))
						valid_case = false;
					if(!valid_case) {
						p->report(error(error::kind::KIND_ERROR, "Expected compatible types for a switch scope's case.", stream, 0));
						p->report(error(error::kind::KIND_ERROR, "Original expression being compared against is here.", p_scope->parent_expression()->stream(), 0));
						_contained_case_default_stmt = make_shared<case_default_stmt>(k, ce, nullptr, false, lnumber, stream);
						_valid = false;
						return;
					}
				}
				else if (ce_type->type_kind() == type::kind::KIND_STRUCT) {
					shared_ptr<struct_type> ce_stype = static_pointer_cast<struct_type>(ce_type), s_stype = static_pointer_cast<struct_type>(p_type);
					if (ce_stype->struct_reference_number() != s_stype->struct_reference_number() || ce_stype->struct_name().raw_text() != s_stype->struct_name().raw_text()) {
						p->report(error(error::kind::KIND_ERROR, "Expected compatible types for a switch scope's case.", stream, 0));
						p->report(error(error::kind::KIND_ERROR, "Original expression being compared against is here.", p_scope->parent_expression()->stream(), 0));
						_valid = false;
						return;
					}
				}
				else if (ce_type->type_kind() == type::kind::KIND_FUNCTION) {
					if (!matching_function_types(p, ce_type, p_type)) {
						p->report(error(error::kind::KIND_ERROR, "Expected compatible types for a switch scope's case.", stream, 0));
						p->report(error(error::kind::KIND_ERROR, "Original expression being compared against is here.", p_scope->parent_expression()->stream(), 0));
						_valid = false;
						return;
					}
				}
				else
					p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			}
			else {
				if (p_scope->default_stmt_hit()) {
					p->report(error(error::kind::KIND_ERROR, "Cannot have multiple default statements for a single switch statement.", stream, 0));
					_contained_case_default_stmt = make_shared<case_default_stmt>(k, ce, nullptr, false, lnumber, stream);
					_valid = false;
					return;
				}
				p_scope->default_stmt_hit(true);
			}
			if (p->peek().token_kind() != token::kind::TOKEN_COLON) {
				p->report(error(error::kind::KIND_ERROR, "Expected a colon ':' after a 'case' or 'default' statement header.", stream, 0));
				_contained_case_default_stmt = make_shared<case_default_stmt>(k, ce, nullptr, false, lnumber, stream);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			shared_ptr<stmt> s = stmt_parser(p).contained_stmt();
			if (!s->valid()) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid statement for a 'case' or 'default' statement.", stream, 0));
				_contained_case_default_stmt = make_shared<case_default_stmt>(k, ce, s, false, lnumber, stream);
				_valid = false;
				return;
			}
			vector<token> s_stream = s->stream();
			if (!valid_statement_inside_block_scope(s)) {
				p->report(error(error::kind::KIND_ERROR, "Cannot nest this kind of statement inside a 'case' or 'default' statement.", s_stream, 0));
				_contained_case_default_stmt = make_shared<case_default_stmt>(k, ce, s, false, lnumber, stream);
				_valid = false;
				return;
			}
			stream.insert(stream.end(), s_stream.begin(), s_stream.end());
			_contained_case_default_stmt = make_shared<case_default_stmt>(k, ce, s, true, lnumber, stream);
			p_scope->add_case_default_stmt(_contained_case_default_stmt);
			_valid = true;
		}

		case_default_stmt_parser::~case_default_stmt_parser() {

		}

		shared_ptr<case_default_stmt> case_default_stmt_parser::contained_case_default_stmt() {
			return _contained_case_default_stmt;
		}

		bool case_default_stmt_parser::valid() {
			return _valid;
		}

		switch_stmt_parser::switch_stmt_parser(shared_ptr<parser> p) : _contained_switch_stmt(nullptr), _valid(false) {
			if (p->peek().token_kind() != token::kind::TOKEN_SWITCH) {
				_contained_switch_stmt = make_shared<switch_stmt>(nullptr, nullptr, nullptr, false, vector<token>{});
				_valid = false;
				return;
			}
			vector<token> stream;
			stream.push_back(p->pop());
			if (p->peek().token_kind() != token::kind::TOKEN_OPEN_PARENTHESIS) {
				p->report(error(error::kind::KIND_ERROR, "Expected an open parenthesis ('(') to begin a switch statement header.", stream, 0));
				_contained_switch_stmt = make_shared<switch_stmt>(nullptr, nullptr, nullptr, false, stream);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			shared_ptr<expression> pe = expression_parser(p).contained_expression();
			if (!pe->valid()) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid expression to switch.", stream, stream.size() - 1));
				_contained_switch_stmt = make_shared<switch_stmt>(pe, nullptr, nullptr, false, stream);
				_valid = false;
				return;
			}
			vector<token> pe_stream = pe->stream();
			stream.insert(stream.end(), pe_stream.begin(), pe_stream.end());
			if (pe->expression_type()->type_kind() == type::kind::KIND_PRIMITIVE &&
				static_pointer_cast<primitive_type>(pe->expression_type())->primitive_type_kind() == primitive_type::kind::KIND_VOID) {
				p->report(error(error::kind::KIND_ERROR, "Cannot switch an expression with a void type.", pe_stream, 0));
				_contained_switch_stmt = make_shared<switch_stmt>(pe, nullptr, nullptr, false, stream);
				_valid = false;
				return;
			}
			if (p->peek().token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
				p->report(error(error::kind::KIND_ERROR, "Expected a close parenthesis (')') to end a switch statement header.", stream, stream.size() - 1));
				_contained_switch_stmt = make_shared<switch_stmt>(pe, nullptr, nullptr, false, stream);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			shared_ptr<switch_scope> s_scope = make_shared<switch_scope>(pe, p->current_scope());
			p->open_new_scope(s_scope);
			shared_ptr<block_scope> b_scope = make_shared<block_scope>(p->next_block_scope_number(), p->current_scope());
			p->open_new_scope(b_scope);
			shared_ptr<stmt> s = stmt_parser(p).contained_stmt();
			if (!s->valid()) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid statement inside a 'switch' statement body.", stream, stream.size() - 1));;
				_contained_switch_stmt = make_shared<switch_stmt>(pe, s_scope, s, false, stream);
				_valid = false;
				p->close_current_scope();
				p->close_current_scope();
				return;
			}
			vector<token> s_stream = s->stream();
			stream.insert(stream.end(), s_stream.begin(), s_stream.end());
			if (!valid_statement_inside_block_scope(s)) {
				p->report(error(error::kind::KIND_ERROR, "Cannot nest this kind of statement inside a switch statement.", s_stream, s_stream.size() - 1));
				_valid = false;
				p->close_current_scope();
				p->close_current_scope();
				return;
			}
			p->close_current_scope();
			p->close_current_scope();
			_contained_switch_stmt = make_shared<switch_stmt>(pe, s_scope, s, true, stream);
			_valid = true;
		}

		switch_stmt_parser::~switch_stmt_parser() {

		}

		bool switch_stmt_parser::valid() {
			return _valid;
		}

		shared_ptr<switch_stmt> switch_stmt_parser::contained_switch_stmt() {
			return _contained_switch_stmt;
		}

		struct_stmt_parser::struct_stmt_parser(shared_ptr<parser> p) : _contained_struct_stmt(nullptr), _valid(false) {
			if (p->peek().token_kind() != token::kind::TOKEN_STRUCT) {
				_contained_struct_stmt = make_shared<struct_stmt>(struct_stmt::defined_kind::KIND_NONE, bad_token, nullptr, nullptr, nullptr, vector<shared_ptr<variable_declaration>>{},
					false, vector<token>{});
				_valid = false;
				return;
			}
			vector<token> stream;
			stream.push_back(p->pop());
			struct_stmt::defined_kind dk = struct_stmt::defined_kind::KIND_NONE;
			if (p->peek().token_kind() != token::kind::TOKEN_IDENTIFIER) {
				p->report(error(error::kind::KIND_ERROR, "Expected an identifier in order to declare a 'struct'.", stream, 0));
				_contained_struct_stmt = make_shared<struct_stmt>(dk, bad_token, nullptr, nullptr, nullptr, vector<shared_ptr<variable_declaration>>{}, false, stream);
				_valid = false;
				return;
			}
			token struct_name = p->pop();
			stream.push_back(struct_name);
			shared_ptr<symbol> test_symbol = p->find_symbol_in_current_scope(struct_name, true);
			shared_ptr<struct_symbol> s_symbol = nullptr;
			shared_ptr<struct_type> s_type = nullptr;
			shared_ptr<scope> s_scope = nullptr;
			dk = struct_stmt::defined_kind::KIND_DECLARED;
			if (test_symbol != nullptr) {
				if (test_symbol->symbol_kind() != symbol::kind::KIND_STRUCT) {
					p->report(error(error::kind::KIND_ERROR, "Conflicting names detected for this 'struct' declaration.", stream, 0));
					_contained_struct_stmt = make_shared<struct_stmt>(dk, struct_name, nullptr, nullptr, nullptr, vector<shared_ptr<variable_declaration>>{}, false, stream);
					_valid = false;
					return;
				}
				s_symbol = static_pointer_cast<struct_symbol>(test_symbol);
				s_type = s_symbol->struct_symbol_type();
				s_scope = s_symbol->struct_symbol_scope();
				if (s_symbol->struct_symbol_defined_kind() == struct_stmt::defined_kind::KIND_DEFINED) {
					p->report(error(error::kind::KIND_ERROR, "Redefinition/redeclaration of an already defined 'struct.'", stream, 0));
					p->report(error(error::kind::KIND_ERROR, "Originally declared here.", s_symbol->stream(), 0));
					_contained_struct_stmt = make_shared<struct_stmt>(dk, struct_name, s_scope, s_type, s_symbol, vector<shared_ptr<variable_declaration>>{}, false, stream);
					_valid = false;
					return;
				}
			}
			else {
				s_type = make_shared<struct_type>(type::const_kind::KIND_NON_CONST, type::static_kind::KIND_NON_STATIC, struct_name, p->next_struct_reference_number(), true);
				s_scope = make_shared<scope>(scope::kind::KIND_STRUCT, p->current_scope());
				s_symbol = make_shared<struct_symbol>(p->current_scope(), s_type, struct_name, s_scope, stream, dk);
				p->add_symbol_to_current_scope(s_symbol);
				p->add_to_struct_symbol_table(s_symbol);
			}
			if (p->peek().token_kind() == token::kind::TOKEN_SEMICOLON) {
				stream.push_back(p->pop());
				_contained_struct_stmt = make_shared<struct_stmt>(dk, struct_name, s_scope, s_type, s_symbol, vector<shared_ptr<variable_declaration>>{}, true, stream);
				_valid = true;
				return;
			}
			if (p->peek().token_kind() != token::kind::TOKEN_OPEN_BRACE) {
				p->report(error(error::kind::KIND_ERROR, "Expected a semicolon (';') for a 'struct' declaration or an open brace ('{') for a 'struct' declaration.", stream,
					stream.size() - 1));
				_contained_struct_stmt = make_shared<struct_stmt>(dk, struct_name, s_scope, s_type, s_symbol, vector<shared_ptr<variable_declaration>>{}, false, stream);
				_valid = false;
				return;
			}
			dk = struct_stmt::defined_kind::KIND_DEFINED;
			s_symbol->set_struct_symbol_defined_kind(dk);
			stream.push_back(p->pop());
			p->open_new_scope(s_scope);
			vector<shared_ptr<variable_declaration>> struct_members;
			while (p->peek().token_kind() != token::kind::TOKEN_CLOSE_BRACE) {
				variable_declaration_parser vdp(p);
				vector<shared_ptr<variable_declaration>> vdl = vdp.contained_variable_declaration_list();
				if (!vdp.valid() || vdp.contained_variable_declaration_list().empty()) {
					p->report(error(error::kind::KIND_ERROR, "Expected a valid variable declaration for a 'struct' member list.", stream, stream.size() - 1));
					_contained_struct_stmt = make_shared<struct_stmt>(dk, struct_name, s_scope, s_type, s_symbol, struct_members, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				if (vdp.contained_variable_declaration_list()[0]->variable_declaration_type()->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR) {
					p->report(error(error::kind::KIND_ERROR, "A member of a struct cannot be declared 'constexpr.'", stream, stream.size() - 1));
					_contained_struct_stmt = make_shared<struct_stmt>(dk, struct_name, s_scope, s_type, s_symbol, struct_members, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				if (vdp.contained_variable_declaration_list()[0]->variable_declaration_type()->type_kind() == type::kind::KIND_AUTO) {
					p->report(error(error::kind::KIND_ERROR, "A member of a struct cannot be declared as 'auto.'", stream, stream.size() - 1));
					_contained_struct_stmt = make_shared<struct_stmt>(dk, struct_name, s_scope, s_type, s_symbol, struct_members, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				vector<token> vdp_stream = vdp.stream();
				struct_members.insert(struct_members.end(), vdl.begin(), vdl.end());
				stream.insert(stream.end(), vdp_stream.begin(), vdp_stream.end());
				for (shared_ptr<variable_declaration> vd : vdl) {
					vector<token> vec = vd->stream();
					if (vd->variable_declaration_initialization_kind() == variable_declaration::initialization_kind::KIND_PRESENT) {
						p->report(error(error::kind::KIND_ERROR, "Cannot initialize a 'struct' member inside a struct definition.", vec, 0));
						_contained_struct_stmt = make_shared<struct_stmt>(dk, struct_name, s_scope, s_type, s_symbol, struct_members, false, stream);
						_valid = false;
						p->close_current_scope();
						return;
					}
					if (vd->variable_declaration_type()->type_kind() == type::kind::KIND_STRUCT) {
						shared_ptr<struct_type> st = static_pointer_cast<struct_type>(vd->variable_declaration_type());
						if (st->struct_reference_number() == s_type->struct_reference_number() && vd->variable_declaration_type()->type_array_kind() != type::array_kind::KIND_ARRAY) {
							p->report(error(error::kind::KIND_ERROR, "Cannot have an incomplete type inside of a struct definition.", vec, 0));
							_contained_struct_stmt = make_shared<struct_stmt>(dk, struct_name, s_scope, s_type, s_symbol, struct_members, false, stream);
							_valid = false;
							p->close_current_scope();
							return;
						}
					}
				}
				if (p->peek().token_kind() != token::kind::TOKEN_SEMICOLON) {
					p->report(error(error::kind::KIND_ERROR, "Expected a semicolon (';') to end a variable declaration.", vdp_stream, vdp_stream.size() - 1));
					_contained_struct_stmt = make_shared<struct_stmt>(dk, struct_name, s_scope, s_type, s_symbol, struct_members, false, stream);
					_valid = false;
					p->close_current_scope();
					return;
				}
				stream.push_back(p->pop());
			}
			stream.push_back(p->pop());
			p->close_current_scope();
			_valid = true;
			_contained_struct_stmt = make_shared<struct_stmt>(dk, struct_name, s_scope, s_type, s_symbol, struct_members, true, stream);
		}

		struct_stmt_parser::~struct_stmt_parser() {

		}

		shared_ptr<struct_stmt> struct_stmt_parser::contained_struct_stmt() {
			return _contained_struct_stmt;
		}

		bool struct_stmt_parser::valid() {
			return _valid;
		}

		namespace_stmt_parser::namespace_stmt_parser(shared_ptr<parser> p) : _contained_namespace_stmt(nullptr), _valid(false) {
			if (p->peek().token_kind() != token::kind::TOKEN_NAMESPACE) {
				_contained_namespace_stmt = make_shared<namespace_stmt>(namespace_stmt::kind::KIND_NONE, namespace_stmt::defined_kind::KIND_NONE, bad_token, nullptr, nullptr,
					vector<shared_ptr<stmt>>{}, false, vector<token>{});
				_valid = false;
				return;
			}
			vector<token> stream;
			stream.push_back(p->pop());
			if (p->peek().token_kind() != token::kind::TOKEN_IDENTIFIER) {
				p->report(error(error::kind::KIND_ERROR, "Expected an identifier to declare or alias the name of the namespace.", stream, 0));
				_contained_namespace_stmt = make_shared<namespace_stmt>(namespace_stmt::kind::KIND_NONE, namespace_stmt::defined_kind::KIND_NONE, bad_token, nullptr, nullptr, 
					vector<shared_ptr<stmt>>{}, false, stream);
				_valid = false;
				return;
			}
			namespace_stmt::defined_kind dk = namespace_stmt::defined_kind::KIND_DECLARED;
			namespace_stmt::kind k = namespace_stmt::kind::KIND_REGULAR;
			token nn = p->pop();
			stream.push_back(nn);
			shared_ptr<symbol> test_symbol = p->find_symbol_in_current_scope(nn, true);
			shared_ptr<namespace_scope> n_scope = nullptr;
			shared_ptr<namespace_symbol> n_symbol = nullptr;
			if (test_symbol != nullptr) {
				if (test_symbol->symbol_kind() != symbol::kind::KIND_NAMESPACE) {
					p->report(error(error::kind::KIND_ERROR, "This symbol was already declared/defined as something other than a namespace.", stream, 0));
					_contained_namespace_stmt = make_shared<namespace_stmt>(k, dk, nn, nullptr, nullptr, vector<shared_ptr<stmt>>{}, false, stream);
					_valid = false;
					return;
				}
				n_symbol = static_pointer_cast<namespace_symbol>(test_symbol);
				n_scope = n_symbol->namespace_symbol_scope();
			}
			else if(p->peek().token_kind() != token::kind::TOKEN_EQUALS) {
				n_scope = make_shared<namespace_scope>(nn, p->current_scope());
				n_symbol = make_shared<namespace_symbol>(p->current_scope(), nn, n_scope, stream, dk);
				p->add_symbol_to_current_scope(n_symbol);
			}
			if (p->peek().token_kind() == token::kind::TOKEN_SEMICOLON) {
				stream.push_back(p->pop());
				_contained_namespace_stmt = make_shared<namespace_stmt>(k, dk, nn, n_scope, n_symbol, vector<shared_ptr<stmt>>{}, true, stream);
				_valid = true;
				return;
			}
			else if (p->peek().token_kind() == token::kind::TOKEN_EQUALS) {
				dk = namespace_stmt::defined_kind::KIND_DEFINED;
				k = namespace_stmt::kind::KIND_ALIAS;
				stream.push_back(p->pop());
				tuple<bool, vector<token>, shared_ptr<symbol>> tup = handle_qualified_identifier_name(p, false);
				if (!get<0>(tup)) {
					p->report(error(error::kind::KIND_ERROR, "Expected a valid identifier to alias the name of a namespace of.", stream, 0));
					_contained_namespace_stmt = make_shared<namespace_stmt>(k, dk, nn, n_scope, n_symbol, vector<shared_ptr<stmt>>{}, false, stream);
					_valid = false;
					return;
				}
				if (test_symbol != nullptr) {
					p->report(error(error::kind::KIND_ERROR, "This symbol was already declared and cannot be used as a name for a namespace alias.", stream, 0));
					_contained_namespace_stmt = make_shared<namespace_stmt>(k, dk, nn, n_scope, n_symbol, vector<shared_ptr<stmt>>{}, false, stream);
					_valid = false;
					return;
				}
				vector<token> istream = get<1>(tup);
				shared_ptr<symbol> test = get<2>(tup);
				stream.insert(stream.end(), istream.begin(), istream.end());
				if (test->symbol_kind() != symbol::kind::KIND_NAMESPACE) {
					p->report(error(error::kind::KIND_ERROR, "Expected a namespace name to alias.", stream, 0));
					_contained_namespace_stmt = make_shared<namespace_stmt>(k, dk, nn, n_scope, n_symbol, vector<shared_ptr<stmt>>{}, false, stream);
					_valid = false;
					return;
				}
				if (p->peek().token_kind() != token::kind::TOKEN_SEMICOLON) {
					p->report(error(error::kind::KIND_ERROR, "Expected a semicolon to terminate a namespace alias statement.", stream, 0));
					_contained_namespace_stmt = make_shared<namespace_stmt>(k, dk, nn, n_scope, n_symbol, vector<shared_ptr<stmt>>{}, false, stream);
					_valid = false;
					return;
				}
				stream.push_back(p->pop());
				shared_ptr<namespace_symbol> n_sym = static_pointer_cast<namespace_symbol>(test);
				shared_ptr<namespace_symbol> aliased = make_shared<namespace_symbol>(p->current_scope(), nn, n_sym->namespace_symbol_scope(), stream, namespace_stmt::defined_kind::KIND_DEFINED);
				_contained_namespace_stmt = make_shared<namespace_stmt>(k, dk, nn, n_sym->namespace_symbol_scope(), aliased, vector<shared_ptr<stmt>>{}, true, stream);
				p->add_symbol_to_current_scope(aliased);
				_valid = true;
				return;
			}
			if (p->peek().token_kind() != token::kind::TOKEN_OPEN_BRACE) {
				p->report(error(error::kind::KIND_ERROR, "Expected a ';' or '{' to either declare or defined (respectively) a namespace.", stream, 0));
				_contained_namespace_stmt = make_shared<namespace_stmt>(k, dk, nn, n_scope, n_symbol, vector<shared_ptr<stmt>>{}, false, stream);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			dk = namespace_stmt::defined_kind::KIND_DEFINED;
			n_symbol->set_namespace_symbol_defined_kind(dk);
			p->open_new_scope(n_scope);
			vector<shared_ptr<stmt>> nsl;
			while (p->peek().token_kind() != token::kind::TOKEN_CLOSE_BRACE) {
				stmt_parser sp(p);
				shared_ptr<stmt> s = sp.contained_stmt();
				if (!sp.valid() || !s->valid()) {
					p->report(error(error::kind::KIND_ERROR, "Expected a valid statement as part of a namespace definition.", stream, 0));
					_contained_namespace_stmt = make_shared<namespace_stmt>(k, dk, nn, n_scope, n_symbol, nsl, false, stream);
					_valid = false;
					return;
				}
				if (s->stmt_kind() != stmt::kind::KIND_FUNCTION && s->stmt_kind() != stmt::kind::KIND_VARIABLE_DECLARATION &&
					s->stmt_kind() != stmt::kind::KIND_NAMESPACE && s->stmt_kind() != stmt::kind::KIND_EMPTY && s->stmt_kind() != stmt::kind::KIND_STRUCT &&
					s->stmt_kind() != stmt::kind::KIND_USING && s->stmt_kind() != stmt::kind::KIND_INCLUDE && s->stmt_kind() != stmt::kind::KIND_ACCESS) {
					p->report(error(error::kind::KIND_ERROR, "Only namespaces, using statements, functions, struct's, import/include statements, accesses, and variable declarations are"
						"allowed at the namespace scope.", stream, 0));
					_contained_namespace_stmt = make_shared<namespace_stmt>(k, dk, nn, n_scope, n_symbol, nsl, false, stream);
					_valid = false;
					return;
				}
				nsl.push_back(s);
				vector<token> sstream = s->stream();
				stream.insert(stream.end(), sstream.begin(), sstream.end());
			}
			if (p->peek().token_kind() != token::kind::TOKEN_CLOSE_BRACE) {
				p->report(error(error::kind::KIND_ERROR, "Expected a close brace ('}') to finish a namespace definition.", stream, 0));
				_contained_namespace_stmt = make_shared<namespace_stmt>(k, dk, nn, n_scope, n_symbol, nsl, false, stream);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			p->close_current_scope();
			_valid = true;
			_contained_namespace_stmt = make_shared<namespace_stmt>(k, dk, nn, n_scope, n_symbol, nsl, true, stream);
		}

		namespace_stmt_parser::~namespace_stmt_parser() {

		}

		shared_ptr<namespace_stmt> namespace_stmt_parser::contained_namespace_stmt() {
			return _contained_namespace_stmt;
		}

		bool namespace_stmt_parser::valid() {
			return _valid;
		}

		tuple<bool, vector<token>, shared_ptr<symbol>> handle_qualified_identifier_name(shared_ptr<parser> p, bool disallow_namespace_symbol) {
			if(p->peek().token_kind() != token::kind::TOKEN_COLON_COLON && p->peek().token_kind() != token::kind::TOKEN_IDENTIFIER)
				return make_tuple(false, vector<token>{}, nullptr);
			shared_ptr<scope> current_scope = nullptr;
			shared_ptr<symbol> last_symbol = nullptr;
			if (p->peek().token_kind() == token::kind::TOKEN_COLON_COLON)
				current_scope = p->global_scope();
			else
				current_scope = p->current_scope();
			vector<token> stream;
			if (p->peek().token_kind() == token::kind::TOKEN_COLON_COLON)
				stream.push_back(p->pop());
			if (p->peek().token_kind() != token::kind::TOKEN_IDENTIFIER) {
				p->report(error(error::kind::KIND_ERROR, "Expected an identifier after '::'.", stream, 0));
				return make_tuple(false, stream, last_symbol);
			}
			bool first_iter = true;
			while (p->peek().token_kind() == token::kind::TOKEN_IDENTIFIER) {
				token ident = p->pop();
				stream.push_back(ident);
				last_symbol = current_scope->find_symbol(ident, !first_iter);
				if (last_symbol == nullptr) {
					p->report(error(error::kind::KIND_ERROR, "This symbol was not found.", stream, 0));
					return make_tuple(false, stream, last_symbol);
				}
				if (p->peek().token_kind() == token::kind::TOKEN_COLON_COLON) {
					stream.push_back(p->pop());
					if (last_symbol->symbol_kind() != symbol::kind::KIND_NAMESPACE) {
						p->report(error(error::kind::KIND_ERROR, "Cannot use the '::' operator on a non-namespace symbol.", stream, 0));
						return make_tuple(false, stream, last_symbol);
					}
					shared_ptr<namespace_symbol> nsym = static_pointer_cast<namespace_symbol>(last_symbol);
					current_scope = nsym->namespace_symbol_scope();
				}
				else
					break;
				first_iter = false;
			}
			if (disallow_namespace_symbol) {
				if (last_symbol != nullptr && last_symbol->symbol_kind() == symbol::kind::KIND_NAMESPACE) {
					p->report(error(error::kind::KIND_ERROR, "A raw namespace symbol is disallowed in this context.", stream, 0));
					return make_tuple(false, stream, last_symbol);
				}
			}
			return make_tuple(true, stream, last_symbol);
		}

		using_stmt_parser::using_stmt_parser(shared_ptr<parser> p) : _contained_using_stmt(nullptr), _valid(false) {
			if (p->peek().token_kind() != token::kind::TOKEN_USING) {
				_contained_using_stmt = make_shared<using_stmt>(using_stmt::kind::KIND_NONE, nullptr, vector<token>{}, false);
				_valid = false;
				return;
			}
			vector<token> stream;
			stream.push_back(p->pop());
			using_stmt::kind k = using_stmt::kind::KIND_REGULAR_SYMBOL;
			if (p->peek().token_kind() == token::kind::TOKEN_NAMESPACE) {
				stream.push_back(p->pop());
				k = using_stmt::kind::KIND_NAMESPACE;
			}
			tuple<bool, vector<token>, shared_ptr<symbol>> tup = handle_qualified_identifier_name(p, false);
			if (!get<0>(tup)) {
				p->report(error(error::kind::KIND_ERROR, "Invalid symbol found here.", get<1>(tup), 0));
				_contained_using_stmt = make_shared<using_stmt>(k, nullptr, stream, false);
				_valid = false;
				return;
			}
			shared_ptr<symbol> s = get<2>(tup);
			vector<token> istream = get<1>(tup);
			stream.insert(stream.end(), istream.begin(), istream.end());
			if (p->peek().token_kind() != token::kind::TOKEN_SEMICOLON) {
				p->report(error(error::kind::KIND_ERROR, "Expected a semicolon (';') after a using statement.", stream, 0));
				_contained_using_stmt = make_shared<using_stmt>(k, s, stream, false);
				_valid = false;
			}
			stream.push_back(p->pop());
			if (s->symbol_kind() == symbol::kind::KIND_NAMESPACE && k == using_stmt::kind::KIND_REGULAR_SYMBOL) {
				p->report(error(error::kind::KIND_ERROR, "The referenced symbol is a namespace but this using statement only supports other symbols, use 'using namespace <namespace-name>;' to "
					"use a namespace.", stream, 0));
				_contained_using_stmt = make_shared<using_stmt>(k, s, stream, false);
				_valid = false;
				return;
			}
			else if (k == using_stmt::kind::KIND_NAMESPACE && s->symbol_kind() != symbol::kind::KIND_NAMESPACE) {
				p->report(error(error::kind::KIND_ERROR, "Expected a namespace symbol for a 'using namespace <namespace-name>;' statement", stream, 0));
				_contained_using_stmt = make_shared<using_stmt>(k, s, stream, false);
				_valid = false;
				return;
			}
			if (k == using_stmt::kind::KIND_REGULAR_SYMBOL) {
				token lit = bad_token;
				if (s->symbol_kind() == symbol::kind::KIND_FUNCTION)
					lit = static_pointer_cast<function_symbol>(s)->function_name();
				else if (s->symbol_kind() == symbol::kind::KIND_STRUCT)
					lit = static_pointer_cast<struct_symbol>(s)->struct_name();
				else if (s->symbol_kind() == symbol::kind::KIND_VARIABLE)
					lit = static_pointer_cast<variable_symbol>(s)->variable_name();
				else {
					p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					_contained_using_stmt = make_shared<using_stmt>(k, s, stream, false);
					_valid = false;
					return;
				}
				shared_ptr<symbol> test = p->find_symbol_in_current_scope(lit, true);
				if (test != nullptr) {
					p->report(error(error::kind::KIND_ERROR, "This using statement clobbers an existing symbol with the same name in the current scope.", stream, 0));
					if (test->symbol_kind() == symbol::kind::KIND_FUNCTION)
						p->report(error(error::kind::KIND_NOTE, "Originally declared here.", static_pointer_cast<function_symbol>(test)->stream(), 0));
					else if (test->symbol_kind() == symbol::kind::KIND_NAMESPACE)
						p->report(error(error::kind::KIND_NOTE, "Originally declared here.", static_pointer_cast<namespace_symbol>(test)->stream(), 0));
					else if (test->symbol_kind() == symbol::kind::KIND_STRUCT)
						p->report(error(error::kind::KIND_NOTE, "Originally declared here.", static_pointer_cast<struct_symbol>(test)->stream(), 0));
					else if (test->symbol_kind() == symbol::kind::KIND_VARIABLE)
						p->report(error(error::kind::KIND_NOTE, "Originally declared here.", static_pointer_cast<variable_symbol>(test)->stream(), 0));
					_contained_using_stmt = make_shared<using_stmt>(k, s, stream, false);
					_valid = false;
					return;
				}
				p->add_symbol_to_current_scope(s);
			}
			else if(k == using_stmt::kind::KIND_NAMESPACE) {
				token lit = static_pointer_cast<namespace_symbol>(s)->namespace_symbol_name();
				for (shared_ptr<symbol> curr : static_pointer_cast<namespace_symbol>(s)->namespace_symbol_scope()->symbol_list()) {
					vector<token> sym_stream;
					shared_ptr<symbol> test = nullptr;
					if (curr->symbol_kind() == symbol::kind::KIND_FUNCTION)
						test = p->find_symbol_in_current_scope(static_pointer_cast<function_symbol>(curr)->function_name(), true);
					else if (curr->symbol_kind() == symbol::kind::KIND_NAMESPACE)
						test = p->find_symbol_in_current_scope(static_pointer_cast<namespace_symbol>(curr)->namespace_symbol_name(), true);
					else if (curr->symbol_kind() == symbol::kind::KIND_STRUCT)
						test = p->find_symbol_in_current_scope(static_pointer_cast<struct_symbol>(curr)->struct_name(), true);
					else if (curr->symbol_kind() == symbol::kind::KIND_VARIABLE)
						test = p->find_symbol_in_current_scope(static_pointer_cast<variable_symbol>(curr)->variable_name(), true);
					else {
						p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						_contained_using_stmt = make_shared<using_stmt>(k, s, stream, false);
						_valid = false;
						return;
					}
					if (test != nullptr) {
						if (test->symbol_kind() == symbol::kind::KIND_FUNCTION)
							sym_stream = static_pointer_cast<function_symbol>(test)->stream();
						else if (test->symbol_kind() == symbol::kind::KIND_NAMESPACE)
							sym_stream = static_pointer_cast<namespace_symbol>(test)->stream();
						else if (test->symbol_kind() == symbol::kind::KIND_STRUCT)
							sym_stream = static_pointer_cast<struct_symbol>(test)->stream();
						else if (test->symbol_kind() == symbol::kind::KIND_VARIABLE)
							sym_stream = static_pointer_cast<variable_symbol>(test)->stream();
						else
							p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						p->report(error(error::kind::KIND_ERROR, "Bringing in this namespace to the current scope also clobbers an existing symbol.", stream, 0));
						p->report(error(error::kind::KIND_NOTE, "Originally declared here.", sym_stream, 0));
						_contained_using_stmt = make_shared<using_stmt>(k, s, stream, false);
						_valid = false;
						return;
					}
					p->add_symbol_to_current_scope(curr);
				}
			}
			_contained_using_stmt = make_shared<using_stmt>(k, s, stream, true);
			_valid = true;
		}

		using_stmt_parser::~using_stmt_parser() {

		}

		shared_ptr<using_stmt> using_stmt_parser::contained_using_stmt() {
			return _contained_using_stmt;
		}

		bool using_stmt_parser::valid() {
			return _valid;
		}

		asm_stmt_parser::asm_stmt_parser(shared_ptr<parser> p) : _contained_asm_stmt(nullptr), _valid(false) {
			if (p->peek().token_kind() != token::kind::TOKEN_ASM) {
				_contained_asm_stmt = make_shared<asm_stmt>(vector<shared_ptr<asm_stmt::asm_type>>{}, vector<token>{}, false);
				_valid = false;
				return;
			}
			vector<token> stream;
			stream.push_back(p->pop());
			vector<shared_ptr<asm_stmt::asm_type>> atl;
			if (p->peek().token_kind() != token::kind::TOKEN_OPEN_PARENTHESIS) {
				p->report(error(error::kind::KIND_ERROR, "Expected an open parenthesis ('(') to being an __asm__ block.", stream, 0));
				_contained_asm_stmt = make_shared<asm_stmt>(atl, stream, false);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			token last = bad_token;
			while (p->peek().token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
				if (p->peek().token_kind() == token::kind::TOKEN_STRING) {
					vector<token> rs;
					rs.push_back(p->pop());
					while (p->peek().token_kind() == token::kind::TOKEN_STRING) rs.push_back(p->pop());
					stream.insert(stream.end(), rs.begin(), rs.end());
					if (p->peek().token_kind() != token::kind::TOKEN_COLON && p->peek().token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
						p->report(error(error::kind::KIND_ERROR, "Expected a colon to terminate a raw assembly statement.", rs, 0));
						_contained_asm_stmt = make_shared<asm_stmt>(atl, stream, false);
						_valid = false;
						return;
					}
					last = p->pop();
					stream.push_back(last);
					atl.push_back(make_shared<asm_stmt::asm_type>(rs));
				}
				else if (p->peek().token_kind() == token::kind::TOKEN_IDENTIFIER) {
					token ident = p->pop();
					stream.push_back(ident);
					shared_ptr<symbol> test = p->find_symbol_in_current_scope(ident, false);
					if (test == nullptr) {
						p->report(error(error::kind::KIND_ERROR, "This symbol was not found in the current scope.", vector<token>{ ident }, 0));
						_contained_asm_stmt = make_shared<asm_stmt>(atl, stream, false);
						_valid = false;
						return;
					}
					if (test->symbol_kind() == symbol::kind::KIND_NAMESPACE || test->symbol_kind() == symbol::kind::KIND_STRUCT) {
						p->report(error(error::kind::KIND_ERROR, "Only functions and variables have physical addresses that can be loaded.", vector<token> { ident }, 0));
						_contained_asm_stmt = make_shared<asm_stmt>(atl, stream, false);
						_valid = false;
						return;
					}
					vector<token> rr;
					while (p->peek().token_kind() == token::kind::TOKEN_STRING) rr.push_back(p->pop());
					if (rr.size() == 0) {
						p->report(error(error::kind::KIND_ERROR, "Expected a register to load the address of the given symbol into.", vector<token> { ident }, 0));
						_contained_asm_stmt = make_shared<asm_stmt>(atl, stream, false);
						_valid = false;
						return;
					}
					stream.insert(stream.end(), rr.begin(), rr.end());
					if (p->peek().token_kind() != token::kind::TOKEN_COLON && p->peek().token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) {
						p->report(error(error::kind::KIND_ERROR, "Expected a colon to terminate a raw assembly statement.", rr, 0));
						_contained_asm_stmt = make_shared<asm_stmt>(atl, stream, false);
						_valid = false;
						return;
					}
					last = p->pop();
					stream.push_back(last);
					atl.push_back(make_shared<asm_stmt::asm_type>(rr, test, ident));
				}
				else {
					p->report(error(error::kind::KIND_ERROR, "Expected either a string or an identifier for a raw assembly statement.", stream, 0));
					_contained_asm_stmt = make_shared<asm_stmt>(atl, stream, false);
					_valid = false;
					return;
				}
				if (last.token_kind() == token::kind::TOKEN_CLOSE_PARENTHESIS) break;
			}
			if (last.token_kind() == token::kind::TOKEN_COLON) {
				p->report(error(error::kind::KIND_ERROR, "There was a trailing colon after the assembly instructions.", stream, 0));
				_contained_asm_stmt = make_shared<asm_stmt>(atl, stream, false);
				_valid = false;
				return;
			}
			if(last.token_kind() != token::kind::TOKEN_CLOSE_PARENTHESIS) stream.push_back(p->pop());
			if (p->peek().token_kind() != token::kind::TOKEN_SEMICOLON) {
				p->report(error(error::kind::KIND_ERROR, "Expected a semicolon (';') to terminate an assembly block.", stream, 0));
				_contained_asm_stmt = make_shared<asm_stmt>(atl, stream, false);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			_contained_asm_stmt = make_shared<asm_stmt>(atl, stream, true);
			_valid = true;
			return;
		}

		asm_stmt_parser::~asm_stmt_parser() {

		}

		shared_ptr<asm_stmt> asm_stmt_parser::contained_asm_stmt() {
			return _contained_asm_stmt;
		}

		bool asm_stmt_parser::valid() {
			return _valid;
		}

		include_stmt_parser::include_stmt_parser(shared_ptr<parser> p) : _contained_include_stmt(nullptr), _valid(false) {
			if (p->peek().token_kind() != token::kind::TOKEN_INCLUDE && p->peek().token_kind() != token::kind::TOKEN_IMPORT) {
				_contained_include_stmt = make_shared<include_stmt>(include_stmt::type::KIND_NONE, vector<shared_ptr<include_stmt::include_type>>{}, vector<token>{}, false);
				_valid = false;
				return;
			}
			vector<token> stream;
			token inc = p->pop();
			include_stmt::type t = inc.token_kind() == token::kind::TOKEN_INCLUDE ? include_stmt::type::KIND_INCLUDE : include_stmt::type::KIND_IMPORT;
			stream.push_back(inc);
			vector<shared_ptr<include_stmt::include_type>> il;
			vector<token> tstream;
			while (p->peek().line_number() == inc.line_number() && p->peek().parent_directory() == inc.parent_directory() && p->peek().file_name() == inc.file_name()) {
				if (p->peek().token_kind() == token::kind::TOKEN_STRING) {
					token li = p->pop();
					stream.push_back(li);
					string raw = li.raw_text().substr(1, li.raw_text().size() - 2);
					string full_path = construct_file_name(vector<string>{ li.parent_directory(), raw });
					if (!file_exists(full_path)) {
						p->report(error(error::kind::KIND_ERROR, "The file: \"" + full_path + "\" does not exist.", stream, 0));
						_contained_include_stmt = make_shared<include_stmt>(t, il, stream, false);
						_valid = false;
						return;
					}
					if ((!p->in_import_list(make_shared<fs::path>(full_path)) && inc.token_kind() == token::kind::TOKEN_IMPORT) || inc.token_kind() == token::kind::TOKEN_INCLUDE) {
						string pd = get_parent_path(full_path), fn = get_file_name(full_path), src = get_file_source(full_path);
						buffer buff(pd, fn, make_shared<diagnostics>(), src);
						token tok = bad_token;
						while (true) {
							tok = buff.pop();
							if (tok.token_kind() != token::kind::TOKEN_EOF) tstream.push_back(tok);
							else break;
						}
						if (buff.diagnostics_reporter()->error_count() > 0) {
							p->report(error(error::kind::KIND_ERROR, "The included file is lexically invalid.", stream, 0));
							_contained_include_stmt = make_shared<include_stmt>(t, il, stream, false);
							_valid = false;
							return;
						}
						if(!p->in_import_list(make_shared<fs::path>(full_path)) && inc.token_kind() == token::kind::TOKEN_IMPORT)
							p->add_to_import_list(make_shared<fs::path>(full_path));
					}
					il.push_back(make_shared<include_stmt::include_type>(li));
				}
				else if (p->peek().token_kind() == token::kind::TOKEN_LEFT_ANGLE_BRACKET) {
					token lb = p->pop();
					stream.push_back(lb);
					if (p->peek().token_kind() != token::kind::TOKEN_STRING) {
						p->report(error(error::kind::KIND_ERROR, "Expected a string to a valid system include file.", stream, 0));
						_contained_include_stmt = make_shared<include_stmt>(t, il, stream, false);
						_valid = false;
						return;
					}
					token si = p->pop();
					stream.push_back(si);
					if (p->peek().token_kind() != token::kind::TOKEN_RIGHT_ANGLE_BRACKET) {
						p->report(error(error::kind::KIND_ERROR, "Expected a close bracket ('>') to end a system include directive.", stream, 0));
						_contained_include_stmt = make_shared<include_stmt>(t, il, stream, false);
						_valid = false;
						return;
					}
					token rb = p->pop();
					stream.push_back(rb);
					string raw = si.raw_text().substr(1, si.raw_text().size() - 2);
					string full_path = construct_file_name(vector<string>{ system_include_path, raw });
					if ((!p->in_import_list(make_shared<fs::path>(full_path)) && inc.token_kind() == token::kind::TOKEN_IMPORT) || inc.token_kind() == token::kind::TOKEN_INCLUDE) {
						if (!file_exists(full_path)) {
							p->report(error(error::kind::KIND_ERROR, "The file: \"" + full_path + "\" does not exist.", stream, 0));
							_contained_include_stmt = make_shared<include_stmt>(t, il, stream, false);
							_valid = false;
							return;
						}
						string pd = get_parent_path(full_path), fn = get_file_name(full_path), src = get_file_source(full_path);
						buffer buff(pd, fn, make_shared<diagnostics>(), src);
						token tok = bad_token;
						while (true) {
							tok = buff.pop();
							if (tok.token_kind() != token::kind::TOKEN_EOF) tstream.push_back(tok);
							else break;
						}
						if (buff.diagnostics_reporter()->error_count() > 0) {
							p->report(error(error::kind::KIND_ERROR, "The included file is lexically invalid.", stream, 0));
							_contained_include_stmt = make_shared<include_stmt>(t, il, stream, false);
							_valid = false;
							return;
						}
						if(!p->in_import_list(make_shared<fs::path>(full_path)) && inc.token_kind() == token::kind::TOKEN_IMPORT)
							p->add_to_import_list(make_shared<fs::path>(full_path));
					}
					il.push_back(make_shared<include_stmt::include_type>(vector<token> { lb, si, rb }));
				}
				else {
					p->report(error(error::kind::KIND_ERROR, "Expected a local include or a system include.", stream, 0));
					_contained_include_stmt = make_shared<include_stmt>(t, il, stream, false);
					_valid = false;
					return;
				}
				if (p->peek().token_kind() == token::kind::TOKEN_COMMA) {
					stream.push_back(p->pop());
					if (inc.line_number() != p->peek().line_number()) {
						p->report(error(error::kind::KIND_ERROR, "Expected a local include or a system include after a comma (',').", stream, 0));
						_contained_include_stmt = make_shared<include_stmt>(t, il, stream, false);
						_valid = false;
						return;
					}
				}
			}
			p->insert_token_list(p->get_buffer_position(), tstream);
			_contained_include_stmt = make_shared<include_stmt>(t, il, stream, true);
			_valid = true;
		}

		include_stmt_parser::~include_stmt_parser() {

		}

		shared_ptr<include_stmt> include_stmt_parser::contained_include_stmt() {
			return _contained_include_stmt;
		}

		bool include_stmt_parser::valid() {
			return _valid;
		}

		access_stmt_parser::access_stmt_parser(shared_ptr<parser> p) : _valid(false), _contained_access_stmt(nullptr) {
			vector<token> stream;
			if (p->peek().token_kind() != token::kind::TOKEN_ACCESS ||
				(p->current_scope()->scope_kind() != scope::kind::KIND_GLOBAL && p->current_scope()->scope_kind() != scope::kind::KIND_NAMESPACE)) {
				_contained_access_stmt = make_shared<access_stmt>(nullptr, vector<shared_ptr<symbol>>{}, vector<token>{}, false);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			type_parser tp(p);
			if (!tp.valid() || !check_declaration_type_is_correct(p, make_shared<type_parser>(tp))
				|| tp.contained_type()->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR
				|| tp.contained_type()->type_kind() == type::kind::KIND_AUTO) {
				p->report(error(error::kind::KIND_ERROR, "Expected a valid type for an access statement.", stream, 0));
				_contained_access_stmt = make_shared<access_stmt>(nullptr, vector<shared_ptr<symbol>>{}, stream, false);
				_valid = false;
				return;
			}
			if (p->peek().token_kind() != token::kind::TOKEN_BAR) {
				p->report(error(error::kind::KIND_ERROR, "Expected a '|' to divide an access type from identifiers.", stream, 0));
				_contained_access_stmt = make_shared<access_stmt>(nullptr, vector<shared_ptr<symbol>>{}, stream, false);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			shared_ptr<type> dt = tp.contained_type();
			auto consume_qualified_name = [&dt](shared_ptr<parser> p) -> tuple<vector<token>, shared_ptr<symbol>, bool> {
				if (p->peek().token_kind() != token::kind::TOKEN_COLON_COLON && p->peek().token_kind() != token::kind::TOKEN_IDENTIFIER)
					return make_tuple(vector<token>{}, nullptr, false);
				vector<token> stream;
				bool global = false;
				if (p->peek().token_kind() == token::kind::TOKEN_COLON_COLON)
					stream.push_back(p->pop()), global = true;
				if (p->peek().token_kind() != token::kind::TOKEN_IDENTIFIER) {
					p->report(error(error::kind::KIND_ERROR, "Expected an identifier after '::'.", stream, 0));
					return make_tuple(stream, nullptr, false);
				}
				vector<token> istream;
				while (p->peek().token_kind() == token::kind::TOKEN_IDENTIFIER) {
					token ident = p->pop();
					stream.push_back(ident);
					istream.push_back(ident);
					if (p->peek().token_kind() == token::kind::TOKEN_COLON_COLON)
						stream.push_back(p->pop());
					else
						break;
				}
				shared_ptr<scope> iter_scope;
				if (global) iter_scope = p->global_scope();
				else iter_scope = p->current_scope();
				shared_ptr<symbol> last_symbol;
				bool first_iter = true;
				for (int i = 0; i < istream.size() - 1; i++) {
					last_symbol = iter_scope->find_symbol(istream[i], !first_iter);
					if (last_symbol == nullptr) {
						p->report(error(error::kind::KIND_ERROR, "This symbol was not found.", stream, 0));
						return make_tuple(stream, nullptr, false);
					}
					if (last_symbol->symbol_kind() != symbol::kind::KIND_NAMESPACE) {
						p->report(error(error::kind::KIND_ERROR, "Cannot use the '::' operator on a non-namespace symbol.", stream, 0));
						return make_tuple(stream, nullptr, false);
					}
					shared_ptr<namespace_symbol> nsym = static_pointer_cast<namespace_symbol>(last_symbol);
					iter_scope = nsym->namespace_symbol_scope();
					first_iter = false;
				}
				shared_ptr<symbol> res = iter_scope->find_symbol(istream[istream.size() - 1], !first_iter);
				if (res != nullptr) {
					if (res->symbol_kind() != symbol::kind::KIND_VARIABLE) {
						p->report(error(error::kind::KIND_ERROR, "Expected to 'access' a variable.", stream, 0));
						return make_tuple(stream, nullptr, false);
					}
					shared_ptr<type> to_comp = static_pointer_cast<variable_symbol>(res)->variable_type();
					if (!exact_type_match(p, to_comp, dt)) {
						p->report(error(error::kind::KIND_ERROR, "Conflicting types for an 'access' statement with an already defined symbol.", stream, 0));
						return make_tuple(stream, nullptr, false);
					}
					return make_tuple(stream, res, true);
				}
				else {
					shared_ptr<variable_symbol> constructed = make_shared<variable_symbol>(iter_scope, istream[istream.size() - 1], dt, stream);
					iter_scope->add_symbol(constructed);
					return make_tuple(stream, constructed, true);
				}
				return make_tuple(stream, nullptr, false);
			};
			vector<shared_ptr<symbol>> dsl;
			while (true) {
				tuple<vector<token>, shared_ptr<symbol>, bool> tup = consume_qualified_name(p);
				if (!get<2>(tup)) {
					p->report(error(error::kind::KIND_ERROR, "Invalid identifier name.", stream, 0));
					_contained_access_stmt = make_shared<access_stmt>(dt, dsl, stream, false);
					_valid = false;
					return;
				}
				vector<token> istream = get<0>(tup);
				shared_ptr<symbol> curr = get<1>(tup);
				stream.insert(stream.end(), istream.begin(), istream.end());
				dsl.push_back(curr);
				if (p->peek().token_kind() == token::kind::TOKEN_COMMA)
					stream.push_back(p->pop());
				else
					break;
			}
			if (p->peek().token_kind() != token::kind::TOKEN_SEMICOLON) {
				p->report(error(error::kind::KIND_ERROR, "Expected a ';' to terminate an 'access' statement.", stream, 0));
				_contained_access_stmt = make_shared<access_stmt>(dt, dsl, stream, false);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			_valid = true;
			_contained_access_stmt = make_shared<access_stmt>(dt, dsl, stream, true);
		}

		access_stmt_parser::~access_stmt_parser() {

		}

		bool access_stmt_parser::valid() {
			return _valid;
		}

		shared_ptr<access_stmt> access_stmt_parser::contained_access_stmt() {
			return _contained_access_stmt;
		}

		delete_stmt_parser::delete_stmt_parser(shared_ptr<parser> p) : _contained_delete_stmt(nullptr), _valid(false) {
			vector<token> stream;
			if (p->peek().token_kind() != token::kind::TOKEN_DELETE) {
				_contained_delete_stmt = make_shared<delete_stmt>(nullptr, stream, false);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			shared_ptr<expression> e = expression_parser(p).contained_expression();
			if (e == nullptr || !e->valid()) {
				p->report(error(error::kind::KIND_ERROR, "Invalid expression for a \"delete\" expression.", stream, 0));
				_contained_delete_stmt = make_shared<delete_stmt>(e, stream, false);
				_valid = false;
				return;
			}
			vector<token> estream = e->stream();
			stream.insert(stream.end(), estream.begin(), estream.end());
			shared_ptr<type> et = e->expression_type();
			// std::lib::free(const byte* data) requires at least an array type.
			if (et->type_array_kind() != type::array_kind::KIND_ARRAY) {
				p->report(error(error::kind::KIND_ERROR, "Expected an array type for a \"delete\" expression.", stream, 0));
				_contained_delete_stmt = make_shared<delete_stmt>(e, stream, false);
				_valid = false;
				return;
			}
			if (p->peek().token_kind() != token::kind::TOKEN_SEMICOLON) {
				p->report(error(error::kind::KIND_ERROR, "Expected a semicolon (';') to end a \"delete\" statement.", stream, 0));
				_contained_delete_stmt = make_shared<delete_stmt>(e, stream, false);
				_valid = false;
				return;
			}
			if (SYSTEM != 0) {
				p->report(error(error::kind::KIND_ERROR, "Cannot use a delete statement (';') on a non-MIPS machine.", stream, 0));
				_contained_delete_stmt = make_shared<delete_stmt>(e, stream, false);
				_valid = false;
				return;
			}
			stream.push_back(p->pop());
			_contained_delete_stmt = make_shared<delete_stmt>(e, stream, true);
			_valid = true;
		}

		delete_stmt_parser::~delete_stmt_parser() {

		}

		shared_ptr<delete_stmt> delete_stmt_parser::contained_delete_stmt() {
			return _contained_delete_stmt;
		}

		bool delete_stmt_parser::valid() {
			return _valid;
		}

		bool is_constant_expression(shared_ptr<parser> p, shared_ptr<assignment_expression> aexpr, bool mem) {
			function<bool(shared_ptr<expression>)> descend_expression;
			function<bool(shared_ptr<assignment_expression>)> descend_assignment_expression;
			function<bool(shared_ptr<ternary_expression>)> descend_ternary_expression;
			function<bool(shared_ptr<binary_expression>)> descend_binary_expression;
			auto descend_primary_expression = [&](shared_ptr<primary_expression> pe) {
				if (pe->primary_expression_kind() == primary_expression::kind::KIND_NEW || pe->primary_expression_kind() == primary_expression::kind::KIND_STK) return false;
				else if (pe->primary_expression_kind() == primary_expression::kind::KIND_IDENTIFIER)
					return pe->primary_expression_type()->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR;
				else if (pe->primary_expression_kind() == primary_expression::kind::KIND_SIZEOF_TYPE) return true;
				else if (pe->primary_expression_kind() == primary_expression::kind::KIND_LITERAL) {
					if (pe->literal_token().token_kind() == token::kind::TOKEN_STRING && !mem) return false;
					return true;
				}
				else if (pe->primary_expression_kind() == primary_expression::kind::KIND_PARENTHESIZED_EXPRESSION ||
					pe->primary_expression_kind() == primary_expression::kind::KIND_SIZEOF_EXPRESSION)
					return descend_expression(pe->parenthesized_expression());
				else if (pe->primary_expression_kind() == primary_expression::kind::KIND_RESV) {
					if (!mem) return false;
					return descend_expression(pe->parenthesized_expression());
				}
				else if (pe->primary_expression_kind() == primary_expression::kind::KIND_ARRAY_INITIALIZER) {
					if (!mem) return false;
					for (shared_ptr<assignment_expression> ae : pe->array_initializer())
						if (!descend_assignment_expression(ae)) return false;
					return true;
				}
				return false;
			};
			auto descend_postfix_expression = [&](shared_ptr<postfix_expression> pe) {
				bool primary_constant = descend_primary_expression(pe->contained_primary_expression());
				if (!primary_constant) return false;
				shared_ptr<type> prev_type = pe->contained_primary_expression()->primary_expression_type(), orig_type = prev_type;
				for (shared_ptr<postfix_expression::postfix_type> pt : pe->postfix_type_list()) {
					if (pt->postfix_type_kind() != postfix_expression::kind::KIND_AS)
						return false;
					else {
						shared_ptr<type> to_type = pt->postfix_type_type();
						if (to_type->type_kind() == type::kind::KIND_STRUCT && to_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) return false;
						else if (prev_type->type_kind() == type::kind::KIND_FUNCTION && prev_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY) return false;
						else if (prev_type->array_dimensions() == to_type->array_dimensions() && prev_type->type_array_kind() == to_type->type_array_kind());
						else if (orig_type->type_kind() == type::kind::KIND_PRIMITIVE && orig_type->type_array_kind() == type::array_kind::KIND_NON_ARRAY);
						else return false;
					}
					prev_type = pt->postfix_type_type();
				}
				return true;
			};
			auto descend_unary_expression = [&](shared_ptr<unary_expression> ue) {
				bool post_constant = descend_postfix_expression(ue->contained_postfix_expression());
				if (!post_constant) return post_constant;
				for (unary_expression::kind uek : ue->unary_expression_kind_list())
					if (uek == unary_expression::kind::KIND_INCREMENT || uek == unary_expression::kind::KIND_DECREMENT) return false;
				return true;
			};
			descend_binary_expression = [&](shared_ptr<binary_expression> be) {
				if (be->binary_expression_kind() == binary_expression::kind::KIND_UNARY_EXPRESSION) return descend_unary_expression(be->single_lhs());
				bool lhs = descend_binary_expression(be->lhs()), rhs = descend_binary_expression(be->rhs());
				return lhs && rhs;
			};
			descend_ternary_expression = [&](shared_ptr<ternary_expression> te) {
				bool cond_constant = descend_binary_expression(te->condition());
				if (te->ternary_expression_kind() == ternary_expression::kind::KIND_BINARY) return cond_constant;
				bool true_constant = descend_expression(te->true_path()), false_constant = descend_ternary_expression(te->false_path());
				return cond_constant && true_constant && false_constant;
			};
			descend_assignment_expression = [&](shared_ptr<assignment_expression> ae) {
				if (ae->assignment_expression_kind() == assignment_expression::kind::KIND_ASSIGNMENT) return false;
				return descend_ternary_expression(ae->conditional_expression());
			};
			descend_expression = [&](shared_ptr<expression> e) {
				bool constant = true;
				for (shared_ptr<assignment_expression> ae : e->assignment_expression_list())
					constant = constant && descend_assignment_expression(ae);
				return constant;
			};
			return descend_assignment_expression(aexpr);
		}

		bool exact_type_match(shared_ptr<parser> p, shared_ptr<type> t1, shared_ptr<type> t2) {
			if (t1->type_kind() != t2->type_kind() || t1->type_array_kind() != t2->type_array_kind() || t1->array_dimensions() != t2->array_dimensions() ||
				t1->type_const_kind() != t2->type_const_kind() || t1->type_static_kind() != t2->type_static_kind())
				return false;
			if (t1->type_kind() == type::kind::KIND_PRIMITIVE)
				return primitive_types_equal(p, t1, t2);
			else if (t1->type_kind() == type::kind::KIND_FUNCTION)
				return matching_function_types(p, t1, t2, true);
			else if (t1->type_kind() == type::kind::KIND_STRUCT) {
				shared_ptr<struct_type> s1 = static_pointer_cast<struct_type>(t1), s2 = static_pointer_cast<struct_type>(t2);
				return s1->struct_reference_number() == s2->struct_reference_number() && s1->struct_name().raw_text() == s2->struct_name().raw_text();
			}
			return false;
		}

		bool matching_function_types(shared_ptr<parser> p, shared_ptr<type> t1, shared_ptr<type> t2, bool e) {
			if (t1->type_kind() != type::kind::KIND_FUNCTION || t2->type_kind() != type::kind::KIND_FUNCTION) {
				p->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return false;
			}
			if (t1->type_array_kind() != t2->type_array_kind() || t1->array_dimensions() != t2->array_dimensions())
				return false;
			if((t1->type_const_kind() != t2->type_const_kind() || t1->type_static_kind() != t2->type_static_kind()) && e)
				return false;
			shared_ptr<function_type> ft1 = static_pointer_cast<function_type>(t1), ft2 = static_pointer_cast<function_type>(t2);
			if (!exact_type_match(p, ft1->return_type(), ft2->return_type()))
				return false;
			if (ft1->parameter_list().size() != ft2->parameter_list().size())
				return false;
			for (int i = 0; i < ft1->parameter_list().size(); i++)
				if (!exact_type_match(p, ft1->parameter_list()[i]->variable_declaration_type(), ft2->parameter_list()[i]->variable_declaration_type())) return false;
			return true;
		}
	}
}