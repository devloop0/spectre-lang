#include "parser/type.hpp"

namespace spectre {
	namespace ast {

		type::type(type::kind k, type::const_kind ck, type::static_kind sk) :
			_type_kind(k), _type_const_kind(ck), _type_static_kind(sk), _valid(k != type::kind::KIND_NONE),
			_array_dimensions(0), _type_array_kind(type::array_kind::KIND_NON_ARRAY),
			_type_constexpr_kind(constexpr_kind::KIND_NON_CONSTEXPR) {

		}

		type::type(type::kind k, type::const_kind ck, type::static_kind sk, int ad) :
			_type_kind(k), _type_const_kind(ck), _type_static_kind(sk), _array_dimensions(ad), _valid(k != type::kind::KIND_NONE),
			_type_array_kind(ad == 0 ? type::array_kind::KIND_NON_ARRAY : type::array_kind::KIND_ARRAY),
			_type_constexpr_kind(type::constexpr_kind::KIND_NON_CONSTEXPR) {

		}

		type::type(type::kind k, type::const_kind ck, type::static_kind sk, type::constexpr_kind cek) :
			_type_kind(k), _type_const_kind(ck), _type_static_kind(sk), _valid(k != type::kind::KIND_NONE),
			_array_dimensions(0), _type_array_kind(type::array_kind::KIND_NON_ARRAY),
			_type_constexpr_kind(cek) {
		}

		type::type(type::kind k, type::const_kind ck, type::static_kind sk, type::constexpr_kind cek, int ad) :
			_type_kind(k), _type_const_kind(ck), _type_static_kind(sk), _array_dimensions(ad), _valid(k != type::kind::KIND_NONE),
			_type_array_kind(ad == 0 ? type::array_kind::KIND_NON_ARRAY : type::array_kind::KIND_ARRAY),
			_type_constexpr_kind(cek) {

		}

		type::~type() {

		}

		void type::set_array_dimensions(int ad) {
			_array_dimensions = ad;
		}

		void type::set_type_array_kind(array_kind ak) {
			_type_array_kind = ak;
		}

		type::kind type::type_kind() {
			return _type_kind;
		}

		type::const_kind type::type_const_kind() {
			return _type_const_kind;
		}

		type::static_kind type::type_static_kind() {
			return _type_static_kind;
		}

		bool type::valid() {
			return _valid;
		}

		int type::array_dimensions() {
			return _array_dimensions;
		}

		type::array_kind type::type_array_kind() {
			return _type_array_kind;
		}

		type::constexpr_kind type::type_constexpr_kind() {
			return _type_constexpr_kind;
		}

		void type::set_constexpr_kind(type::constexpr_kind cek) {
			_type_constexpr_kind = cek;
		}

		primitive_type::primitive_type(primitive_type::kind k, type::const_kind ck, type::static_kind sk, primitive_type::sign_kind sk2) :
			type(type::kind::KIND_PRIMITIVE, ck, sk), _primitive_type_kind(k), _valid(k != primitive_type::kind::KIND_NONE), _primitive_type_sign_kind(sk2) {

		}

		primitive_type::primitive_type(primitive_type::kind k, type::const_kind ck, type::static_kind sk, primitive_type::sign_kind sk2,
			int ad) : type(type::kind::KIND_PRIMITIVE, ck, sk, ad), _primitive_type_kind(k), _valid(k != primitive_type::kind::KIND_NONE),
			_primitive_type_sign_kind(sk2) {

		}

		primitive_type::primitive_type(primitive_type::kind k, type::const_kind ck, type::static_kind sk, type::constexpr_kind cek, primitive_type::sign_kind sk2) :
			type(type::kind::KIND_PRIMITIVE, ck, sk, cek), _primitive_type_kind(k), _valid(k != primitive_type::kind::KIND_NONE), _primitive_type_sign_kind(sk2) {

		}

		primitive_type::primitive_type(primitive_type::kind k, type::const_kind ck, type::static_kind sk, type::constexpr_kind cek, primitive_type::sign_kind sk2,
			int ad) : type(type::kind::KIND_PRIMITIVE, ck, sk, cek, ad), _primitive_type_kind(k), _valid(k != primitive_type::kind::KIND_NONE),
			_primitive_type_sign_kind(sk2) {

		}

		primitive_type::~primitive_type() {

		}

		primitive_type::kind primitive_type::primitive_type_kind() {
			return _primitive_type_kind;
		}

		bool primitive_type::valid() {
			return _valid;
		}

		primitive_type::sign_kind primitive_type::primitive_type_sign_kind() {
			return _primitive_type_sign_kind;
		}

		function_type::function_type(type::const_kind ck, type::static_kind sk, shared_ptr<type> rt, vector<shared_ptr<variable_declaration>> pl, int r, bool v) :
			type(type::kind::KIND_FUNCTION, ck, sk), _return_type(rt), _parameter_list(pl), _valid(rt != nullptr && rt->type_kind() != type::kind::KIND_NONE),
			_function_reference_number(r), _variadic(v) {

		}

		function_type::function_type(type::const_kind ck, type::static_kind sk, shared_ptr<type> rt, vector<shared_ptr<variable_declaration>> pl, int r, int ad, bool v) :
			type(type::kind::KIND_FUNCTION, ck, sk, ad), _return_type(rt), _parameter_list(pl), _valid(rt != nullptr && rt->type_kind() != type::kind::KIND_NONE),
			_function_reference_number(r), _variadic(v) {

		}

		function_type::function_type(type::const_kind ck, type::static_kind sk, type::constexpr_kind cek, shared_ptr<type> rt,
			vector<shared_ptr<variable_declaration>> pl, int r, bool v) :
			type(type::kind::KIND_FUNCTION, ck, sk, cek), _return_type(rt), _parameter_list(pl), _valid(rt != nullptr && rt->type_kind() != type::kind::KIND_NONE),
			_function_reference_number(r), _variadic(v) {

		}

		function_type::function_type(type::const_kind ck, type::static_kind sk, type::constexpr_kind cek, shared_ptr<type> rt,
			vector<shared_ptr<variable_declaration>> pl, int r, int ad, bool v) :
			type(type::kind::KIND_FUNCTION, ck, sk, cek, ad), _return_type(rt), _parameter_list(pl), _valid(rt != nullptr && rt->type_kind() != type::kind::KIND_NONE),
			_function_reference_number(r), _variadic(v) {

		}

		function_type::~function_type() {

		}

		shared_ptr<type> function_type::return_type() {
			return _return_type;
		}

		vector<shared_ptr<variable_declaration>> function_type::parameter_list() {
			return _parameter_list;
		}

		bool function_type::valid() {
			return _valid;
		}

		bool function_type::variadic() {
			return _variadic;
		}

		int function_type::function_reference_number() {
			return _function_reference_number;
		}

		struct_type::struct_type(type::const_kind ck, type::static_kind sk, token t, int r, bool v, bool u) :
			type(type::kind::KIND_STRUCT, ck, sk), _struct_name(t), _struct_reference_number(r), _valid(v), _union(u) {

		}

		struct_type::struct_type(type::const_kind ck, type::static_kind sk, token t, int r, bool v, int ad, bool u) :
			type(type::kind::KIND_STRUCT, ck, sk, ad), _struct_name(t), _struct_reference_number(r), _valid(v), _union(u) {

		}

		struct_type::struct_type(type::const_kind ck, type::static_kind sk, type::constexpr_kind cek, token t, int r, bool v, bool u) :
			type(type::kind::KIND_STRUCT, ck, sk, cek), _struct_name(t), _struct_reference_number(r), _valid(v), _union(u) {

		}

		struct_type::struct_type(type::const_kind ck, type::static_kind sk, type::constexpr_kind cek, token t, int r, bool v, int ad, bool u) :
			type(type::kind::KIND_STRUCT, ck, sk, cek, ad), _struct_name(t), _struct_reference_number(r), _valid(v), _union(u) {

		}

		struct_type::~struct_type() {

		}

		token struct_type::struct_name() {
			return _struct_name;
		}

		int struct_type::struct_reference_number() {
			return _struct_reference_number;
		}

		bool struct_type::valid() {
			return _valid;
		}

		bool struct_type::is_union() {
			return _union;
		}

		auto_type::auto_type(type::const_kind ck, type::static_kind sk) : type(type::kind::KIND_AUTO, ck, sk) {

		}

		auto_type::auto_type(type::const_kind ck, type::static_kind sk, int ad) : type(type::kind::KIND_AUTO, ck, sk, ad) {

		}

		auto_type::auto_type(type::const_kind ck, type::static_kind sk, type::constexpr_kind cek) : type(type::kind::KIND_AUTO, ck, sk, cek) {

		}

		auto_type::auto_type(type::const_kind ck, type::static_kind sk, type::constexpr_kind cek, int ad) : type(type::kind::KIND_AUTO, ck, sk, cek, ad) {

		}

		auto_type::~auto_type() {

		}
	}
}
