#ifndef SPECTRE_TYPE_HPP
#define SPECTRE_TYPE_HPP

#include <vector>
#include <memory>
#include "token.hpp"

using std::vector;
using std::shared_ptr;
using spectre::lexer::token;

namespace spectre {
	namespace parser {
		class scope;
	}
}

using spectre::parser::scope;

namespace spectre {
	namespace ast {

		class variable_declaration;
		class expression;

		class type {
		public:
			enum class kind {
				KIND_PRIMITIVE, KIND_FUNCTION, KIND_STRUCT, KIND_NONE
			};
			enum class const_kind {
				KIND_CONST, KIND_NON_CONST, KIND_NONE
			};
			enum class static_kind {
				KIND_STATIC, KIND_NON_STATIC, KIND_NONE
			};
			enum class array_kind {
				KIND_ARRAY, KIND_NON_ARRAY, KIND_NONE
			};
		private:
			kind _type_kind;
			const_kind _type_const_kind;
			static_kind _type_static_kind;
			bool _valid;
			int _array_dimensions;
			array_kind _type_array_kind;
		public:
			type(kind k, const_kind ck, static_kind sk);
			type(kind k, const_kind ck, static_kind sk, int ad);
			~type();
			kind type_kind();
			const_kind type_const_kind();
			static_kind type_static_kind();
			int array_dimensions();
			bool valid();
			array_kind type_array_kind();
		};

		class primitive_type : public type {
		public:
			enum class kind {
				KIND_NONE,
				KIND_VOID,
				KIND_BOOL, // 1-bit true/false
				KIND_BYTE, // 8-bit two's complement integer
				KIND_CHAR, // 8-bit ASCII character
				KIND_SHORT, // 16-bit two's complement integer
				KIND_INT, // 32-bit two's complement integer
				KIND_FLOAT, // 32-bit floating point number
				KIND_LONG, // 64-bit two's complement integer
				KIND_DOUBLE, // 64-bit floating point number
			};
			enum class sign_kind {
				KIND_SIGNED, KIND_UNSIGNED, KIND_NONE
			};
		private:
			kind _primitive_type_kind;
			bool _valid;
			sign_kind _primitive_type_sign_kind;
		public:
			primitive_type(kind k, type::const_kind ck, type::static_kind sk, sign_kind sk2);
			primitive_type(kind k, type::const_kind ck, type::static_kind sk, sign_kind sk2, int ad);
			~primitive_type();
			kind primitive_type_kind();
			sign_kind primitive_type_sign_kind();
			bool valid();
		};

		class function_type : public type {
		private:
			shared_ptr<type> _return_type;
			vector<shared_ptr<variable_declaration>> _parameter_list;
			vector<bool> _parameter_rfuncarg_list;
			bool _valid;
			int _function_reference_number;
		public:
			function_type(type::const_kind ck, type::static_kind sk, shared_ptr<type> rt, vector<shared_ptr<variable_declaration>> pl, vector<bool> prl, int r);
			~function_type();
			shared_ptr<type> return_type();
			vector<shared_ptr<variable_declaration>> parameter_list();
			vector<bool> parameter_rfuncarg_list();
			bool valid();
			int function_reference_number();
		};

		class struct_type : public type {
		private:
			shared_ptr<scope> _parent_scope;
			token _struct_name;
			int _struct_reference_number;
			bool _valid;
		public:
			struct_type(type::const_kind ck, type::static_kind sk, token t, int r, bool v);
			struct_type(type::const_kind ck, type::static_kind sk, token t, int r, bool v, int ad);
			~struct_type();
			token struct_name();
			int struct_reference_number();
			bool valid();
		};
	}
}

#endif