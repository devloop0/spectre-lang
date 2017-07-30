#include "expr.hpp"
#include <memory>

using std::make_shared;

namespace spectre {
	namespace ast {

		primary_expression::primary_expression(kind pek, token lt, shared_ptr<type> pet, bool v, vector<token> s, value_kind vk) :
			_primary_expression_kind(pek), _literal_token(lt), _primary_expression_type(pet), _valid(v), _stream(s), _primary_expression_value_kind(vk),
			_parenthesized_expression(nullptr), _identifier_symbol(nullptr) {

		}

		primary_expression::primary_expression(kind pek, shared_ptr<expression> e, shared_ptr<type> pet, bool v, vector<token> s, value_kind vk) :
			_primary_expression_kind(pek), _parenthesized_expression(e), _primary_expression_type(pet), _valid(v), _stream(s), _primary_expression_value_kind(vk),
			_literal_token(bad_token), _identifier_symbol(nullptr) {

		}

		primary_expression::primary_expression(kind pek, token lt, shared_ptr<symbol> is, shared_ptr<type> pet, bool v, vector<token> s, value_kind vk) :
			_primary_expression_kind(pek), _parenthesized_expression(nullptr), _primary_expression_type(pet), _valid(v), _stream(s),
			_primary_expression_value_kind(vk), _literal_token(lt), _identifier_symbol(is) {
		}

		primary_expression::primary_expression() : _primary_expression_kind(primary_expression::kind::KIND_NONE), _literal_token(bad_token),
			_primary_expression_type(make_shared<type>(type::kind::KIND_NONE, type::const_kind::KIND_NONE, type::static_kind::KIND_NONE)), _valid(false), _stream(vector<token>()),
			_primary_expression_value_kind(value_kind::VALUE_NONE) {

		}

		primary_expression::primary_expression(kind pek, vector<shared_ptr<assignment_expression>> ai, shared_ptr<type> pet, bool v, vector<token> s) :
			_primary_expression_kind(pek), _array_initializer(ai), _primary_expression_type(pet), _valid(v), _stream(s), _literal_token(bad_token),
			_identifier_symbol(nullptr), _primary_expression_value_kind(value_kind::VALUE_RVALUE) {

		}
		
		primary_expression::primary_expression(kind pek, shared_ptr<type> nt, shared_ptr<expression> e, shared_ptr<type> pet, bool v, vector<token> s) :
			_primary_expression_kind(pek), _mem_type(nt), _parenthesized_expression(e), _primary_expression_type(pet), _valid(v), _stream(s), _literal_token(bad_token),
			_identifier_symbol(nullptr), _primary_expression_value_kind(value_kind::VALUE_RVALUE) {

		}

		primary_expression::primary_expression(kind pek, shared_ptr<type> t, shared_ptr<type> pet, bool v, vector<token> s) :
			_primary_expression_kind(pek), _sizeof_type(t), _primary_expression_type(pet), _valid(v), _stream(s), _primary_expression_value_kind(value_kind::VALUE_RVALUE),
			_literal_token(bad_token) {

		}

		primary_expression::primary_expression(kind pek, shared_ptr<expression> e, shared_ptr<type> pet, bool v, vector<token> s) :
			_primary_expression_kind(pek), _parenthesized_expression(e), _primary_expression_type(pet), _valid(v), _stream(s), _primary_expression_value_kind(value_kind::VALUE_RVALUE),
			_literal_token(bad_token) {

		}

		primary_expression::~primary_expression() {

		}

		primary_expression::kind primary_expression::primary_expression_kind() {
			return _primary_expression_kind;
		}

		token primary_expression::literal_token() {
			return _literal_token;
		}

		shared_ptr<type> primary_expression::primary_expression_type() {
			return _primary_expression_type;
		}

		bool primary_expression::valid() {
			return _valid;
		}

		vector<token> primary_expression::stream() {
			return _stream;
		}

		value_kind primary_expression::primary_expression_value_kind() {
			return _primary_expression_value_kind;
		}

		shared_ptr<expression> primary_expression::parenthesized_expression() {
			return _parenthesized_expression;
		}

		shared_ptr<symbol> primary_expression::identifier_symbol() {
			return _identifier_symbol;
		}

		vector<shared_ptr<assignment_expression>> primary_expression::array_initializer() {
			return _array_initializer;
		}

		shared_ptr<type> primary_expression::mem_type() {
			return _mem_type;
		}

		shared_ptr<type> primary_expression::sizeof_type() {
			return _sizeof_type;
		}

		postfix_expression::postfix_expression(vector<shared_ptr<postfix_expression::postfix_type>> ptl, shared_ptr<primary_expression> cpe, shared_ptr<type> t, bool v,
			vector<token> s, value_kind vk) :  _contained_primary_expression(cpe), _postfix_expression_type(t), _valid(v), _stream(s),
			_postfix_expression_value_kind(vk), _postfix_type_list(ptl) {
			
		}

		postfix_expression::~postfix_expression() {

		}

		shared_ptr<primary_expression> postfix_expression::contained_primary_expression() {
			return _contained_primary_expression;
		}

		shared_ptr<type> postfix_expression::postfix_expression_type() {
			return _postfix_expression_type;
		}

		vector<token> postfix_expression::stream() {
			return _stream;
		}

		bool postfix_expression::valid() {
			return _valid;
		}

		value_kind postfix_expression::postfix_expression_value_kind() {
			return _postfix_expression_value_kind;
		}

		vector<shared_ptr<postfix_expression::postfix_type>> postfix_expression::postfix_type_list() {
			return _postfix_type_list;
		}

		postfix_expression::postfix_type::postfix_type(postfix_expression::kind k, shared_ptr<type> t) : _postfix_type_kind(k), _member(bad_token),
			_postfix_type_type(t) {

		}
		
		postfix_expression::postfix_type::postfix_type(shared_ptr<type> ft, vector<shared_ptr<assignment_expression>> al, shared_ptr<type> t) :
			_postfix_type_kind(postfix_expression::kind::KIND_FUNCTION_CALL), _function_type(ft),
			_argument_list(al), _member(bad_token), _postfix_type_type(t) {

		}

		postfix_expression::postfix_type::postfix_type(shared_ptr<expression> s, shared_ptr<type> t) : _postfix_type_kind(postfix_expression::kind::KIND_SUBSCRIPT),
			_subscript(s), _member(bad_token), _postfix_type_type(t) {

		}

		postfix_expression::postfix_type::postfix_type(postfix_expression::kind k, token m, shared_ptr<type> t) : _postfix_type_kind(k), _member(m),
			_postfix_type_type(t) {

		}

		postfix_expression::postfix_type::~postfix_type() {

		}

		postfix_expression::kind postfix_expression::postfix_type::postfix_type_kind() {
			return _postfix_type_kind;
		}

		vector<shared_ptr<assignment_expression>> postfix_expression::postfix_type::argument_list() {
			return _argument_list;
		}

		shared_ptr<type> postfix_expression::postfix_type::function_type() {
			return _function_type;
		}

		shared_ptr<expression> postfix_expression::postfix_type::subscript() {
			return _subscript;
		}

		token postfix_expression::postfix_type::member() {
			return _member;
		}

		shared_ptr<type> postfix_expression::postfix_type::postfix_type_type() {
			return _postfix_type_type;
		}

		unary_expression::unary_expression(vector<unary_expression::kind> uekl, shared_ptr<postfix_expression> cpe, shared_ptr<type> t, bool v, vector<token> s,
			value_kind vk, unary_expression::kind k) : _unary_expression_kind_list(uekl), _contained_postfix_expression(cpe), _unary_expression_type(t), _valid(v),
			_stream(s), _unary_expression_value_kind(vk), _unary_expression_kind(k) {

		}

		unary_expression::~unary_expression() {

		}

		shared_ptr<postfix_expression> unary_expression::contained_postfix_expression() {
			return _contained_postfix_expression;
		}

		shared_ptr<type> unary_expression::unary_expression_type() {
			return _unary_expression_type;
		}

		vector<token> unary_expression::stream() {
			return _stream;
		}

		bool unary_expression::valid() {
			return _valid;
		}

		value_kind unary_expression::unary_expression_value_kind() {
			return _unary_expression_value_kind;
		}

		unary_expression::kind unary_expression::unary_expression_kind() {
			return _unary_expression_kind;
		}

		vector<unary_expression::kind> unary_expression::unary_expression_kind_list() {
			return _unary_expression_kind_list;
		}

		binary_expression::binary_expression(shared_ptr<type> t, shared_ptr<unary_expression> sl, vector<token> s, bool v, value_kind vk) :
			_single_lhs(sl), _binary_expression_type(t), _lhs(nullptr), _rhs(nullptr), _operator_kind(binary_expression::operator_kind::KIND_NONE),
			_stream(s), _valid(v), _binary_expression_kind(binary_expression::kind::KIND_UNARY_EXPRESSION), _binary_expression_value_kind(vk) {

		}

		binary_expression::binary_expression(shared_ptr<type> t, shared_ptr<binary_expression> l, binary_expression::operator_kind op, shared_ptr<binary_expression> r, vector<token> s,
			bool v, value_kind vk) :
			_single_lhs(nullptr), _lhs(l), _operator_kind(op), _rhs(r), _binary_expression_type(t), _stream(s),
			_valid(v), _binary_expression_kind(binary_expression::kind::KIND_BINARY_EXPRESSION), _binary_expression_value_kind(vk) {

		}

		binary_expression::~binary_expression() {

		}

		shared_ptr<unary_expression> binary_expression::single_lhs() {
			return _single_lhs;
		}

		shared_ptr<binary_expression> binary_expression::lhs() {
			return _lhs;
		}

		shared_ptr<binary_expression> binary_expression::rhs() {
			return _rhs;
		}

		binary_expression::operator_kind binary_expression::binary_expression_operator_kind() {
			return _operator_kind;
		}

		vector<token> binary_expression::stream() {
			return _stream;
		}

		shared_ptr<type> binary_expression::binary_expression_type() {
			return _binary_expression_type;
		}

		bool binary_expression::valid() {
			return _valid;
		}

		binary_expression::kind binary_expression::binary_expression_kind() {
			return _binary_expression_kind;
		}

		value_kind binary_expression::binary_expression_value_kind() {
			return _binary_expression_value_kind;
		}

		ternary_expression::ternary_expression(shared_ptr<type> t, shared_ptr<binary_expression> bexpr, vector<token> s, bool v, value_kind vk) :
			_condition(bexpr), _true_path(nullptr), _false_path(nullptr), _ternary_expression_kind(ternary_expression::kind::KIND_BINARY), _valid(v),
			_ternary_expression_value_kind(vk), _ternary_expression_type(t), _stream(s) {

		}

		ternary_expression::ternary_expression(shared_ptr<type> t, shared_ptr<binary_expression> cond, shared_ptr<expression> tp, shared_ptr<ternary_expression> fp, vector<token> s,
			bool v, value_kind vk) : _condition(cond), _true_path(tp), _false_path(fp), _ternary_expression_kind(ternary_expression::kind::KIND_TERNARY), _valid(v),
			_ternary_expression_value_kind(vk), _ternary_expression_type(t), _stream(s) {

		}

		ternary_expression::~ternary_expression() {

		}

		shared_ptr<binary_expression> ternary_expression::condition() {
			return _condition;
		}

		shared_ptr<expression> ternary_expression::true_path() {
			return _true_path;
		}

		shared_ptr<ternary_expression> ternary_expression::false_path() {
			return _false_path;
		}

		bool ternary_expression::valid() {
			return _valid;
		}

		ternary_expression::kind ternary_expression::ternary_expression_kind() {
			return _ternary_expression_kind;
		}

		value_kind ternary_expression::ternary_expression_value_kind() {
			return _ternary_expression_value_kind;
		}

		shared_ptr<type> ternary_expression::ternary_expression_type() {
			return _ternary_expression_type;
		}

		vector<token> ternary_expression::stream() {
			return _stream;
		}

		assignment_expression::assignment_expression(shared_ptr<type> t, shared_ptr<ternary_expression> te, vector<token> s, bool v, value_kind vk) :
			_conditional_expression(te), _lhs_assignment(nullptr), _assignment_operator_kind(binary_expression::operator_kind::KIND_NONE), _rhs_assignment(nullptr), _valid(v),
			_stream(s), _assignment_expression_type(t), _assignment_expression_value_kind(vk), _assignment_expression_kind(assignment_expression::kind::KIND_TERNARY) { 

		}

		assignment_expression::assignment_expression(shared_ptr<type> t, shared_ptr<unary_expression> ue, binary_expression::operator_kind op, shared_ptr<assignment_expression> ae,
			vector<token> s, bool v, value_kind vk) : _conditional_expression(nullptr), _lhs_assignment(ue), _assignment_operator_kind(op), _rhs_assignment(ae), _valid(v), _stream(s),
			_assignment_expression_type(t), _assignment_expression_value_kind(vk), _assignment_expression_kind(assignment_expression::kind::KIND_ASSIGNMENT) {

		}

		assignment_expression::~assignment_expression() {

		}

		shared_ptr<ternary_expression> assignment_expression::conditional_expression() {
			return _conditional_expression;
		}

		shared_ptr<unary_expression> assignment_expression::lhs_assignment() {
			return _lhs_assignment;
		}

		binary_expression::operator_kind assignment_expression::assignment_operator_kind() {
			return _assignment_operator_kind;
		}

		shared_ptr<assignment_expression> assignment_expression::rhs_assignment() {
			return _rhs_assignment;
		}

		bool assignment_expression::valid() {
			return _valid;
		}

		vector<token> assignment_expression::stream() {
			return _stream;
		}

		shared_ptr<type> assignment_expression::assignment_expression_type() {
			return _assignment_expression_type;
		}

		value_kind assignment_expression::assignment_expression_value_kind() {
			return _assignment_expression_value_kind;
		}

		assignment_expression::kind assignment_expression::assignment_expression_kind() {
			return _assignment_expression_kind;
		}

		expression::expression(shared_ptr<type> t, vector<shared_ptr<assignment_expression>> ael, vector<token> s, bool v, value_kind vk) : _assignment_expression_list(ael),
			_expression_type(t), _valid(v), _stream(s), _expression_value_kind(vk) {

		}

		expression::~expression() {

		}

		vector<shared_ptr<assignment_expression>> expression::assignment_expression_list() {
			return _assignment_expression_list;
		}

		bool expression::valid() {
			return _valid;
		}

		vector<token> expression::stream() {
			return _stream;
		}

		shared_ptr<type> expression::expression_type() {
			return _expression_type;
		}

		value_kind expression::expression_value_kind() {
			return _expression_value_kind;
		}
	}
}