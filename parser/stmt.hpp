#ifndef SPECTRE_STMT_HPP
#define SPECTRE_STMT_HPP

#include "parser/expr.hpp"
#include <memory>

using spectre::ast::expression;
using std::shared_ptr;

namespace spectre {
	namespace parser {
		class variable_symbol;
		class function_symbol;
		class scope;
		class function_scope;
		class switch_scope;
		class struct_symbol;
		class namespace_scope;
		class namespace_symbol;
	}
}

using spectre::parser::variable_symbol;
using spectre::parser::scope;
using spectre::parser::function_symbol;
using spectre::parser::function_scope;
using spectre::parser::switch_scope;
using spectre::parser::struct_symbol;
using spectre::parser::namespace_scope;
using spectre::parser::namespace_symbol;

namespace spectre {
	namespace ast {

		class stmt;

		class variable_declaration {
		public:
			enum class initialization_kind {
				KIND_PRESENT, KIND_NOT_PRESENT, KIND_NONE
			};
		private:
			shared_ptr<type> _variable_declaration_type;
			shared_ptr<variable_symbol> _variable_declaration_symbol;
			token _variable_declaration_name;
			vector<token> _stream;
			shared_ptr<assignment_expression> _initialization;
			initialization_kind _variable_declaration_initialization_kind;
			bool _valid, _symbol_set;
		public:
			variable_declaration(shared_ptr<type> vdt, token vdn, shared_ptr<assignment_expression> i, bool v, vector<token> s);
			variable_declaration(shared_ptr<type> vdt, token vdn, bool v, vector<token> s);
			~variable_declaration();
			shared_ptr<type> variable_declaration_type();
			token variable_declaration_name();
			vector<token> stream();
			shared_ptr<assignment_expression> initialization();
			initialization_kind variable_declaration_initialization_kind();
			bool valid();
			bool symbol_set();
			void set_variable_declaration_symbol(shared_ptr<variable_symbol> vds);
			shared_ptr<variable_symbol> variable_declaration_symbol();
		};

		class if_stmt {
		public:
			enum class else_kind {
				KIND_ELSE_PRESENT, KIND_ELSE_NOT_PRESENT, KIND_NONE
			};
			enum class init_decl_kind {
				KIND_INIT_DECL_PRESENT, KIND_INIT_DECL_NOT_PRESENT, KIND_NONE
			};
		private:
			shared_ptr<scope> _if_scope;
			vector<shared_ptr<variable_declaration>> _init_decl_list;
			shared_ptr<expression> _condition;
			shared_ptr<stmt> _true_path, _false_path;
			vector<token> _stream;
			bool _valid;
			else_kind _if_stmt_else_kind;
			init_decl_kind _if_stmt_init_decl_kind;
		public:
			if_stmt(shared_ptr<scope> is, vector<shared_ptr<variable_declaration>> id, shared_ptr<expression> c, shared_ptr<stmt> tp, shared_ptr<stmt> fp, bool v, vector<token> s);
			if_stmt(shared_ptr<scope> is, vector<shared_ptr<variable_declaration>> id, shared_ptr<expression> c, shared_ptr<stmt> tp, bool v, vector<token> s);
			if_stmt(shared_ptr<scope> is, shared_ptr<expression> c, shared_ptr<stmt> tp, shared_ptr<stmt> fp, bool v, vector<token> s);
			if_stmt(shared_ptr<scope> is, shared_ptr<expression> c, shared_ptr<stmt> tp, bool v, vector<token> s);
			~if_stmt();
			vector<shared_ptr<variable_declaration>> init_decl_list();
			shared_ptr<expression> condition();
			shared_ptr<stmt> true_path(), false_path();
			vector<token> stream();
			bool valid();
			else_kind if_stmt_else_kind();
			init_decl_kind if_stmt_init_decl_kind();
			shared_ptr<scope> if_scope();
		};

		class empty_stmt {
		private:
			bool _valid;
			vector<token> _stream;
		public:
			empty_stmt(bool v, vector<token> s);
			~empty_stmt();
			bool valid();
			vector<token> stream();
		};

		class function_stmt {
		public:
			enum class defined_kind {
				KIND_DECLARATION, KIND_DEFINITION, KIND_NONE
			};
		private:
			bool _valid;
			vector<token> _stream;
			shared_ptr<function_type> _function_type;
			token _function_name;
			vector<shared_ptr<stmt>> _function_body;
			shared_ptr<function_symbol> _function_stmt_symbol;
			shared_ptr<function_scope> _function_stmt_scope;
			defined_kind _function_stmt_defined_kind;
		public:
			function_stmt(shared_ptr<function_type> ft, token fn, shared_ptr<function_symbol> fss, vector<shared_ptr<stmt>> fb, defined_kind dk,
				shared_ptr<function_scope> fs, bool v, vector<token> s);
			~function_stmt();
			bool valid();
			vector<token> stream();
			shared_ptr<function_type> function_stmt_type();
			token function_name();
			vector<shared_ptr<stmt>> function_body();
			shared_ptr<function_symbol> function_stmt_symbol();
			defined_kind function_stmt_defined_kind();
			shared_ptr<function_scope> function_stmt_scope();
		};

		class return_stmt {
		public:
			enum class value_kind {
				KIND_EXPRESSION, KIND_EMPTY, KIND_NONE
			};
		private:
			bool _valid;
			vector<token> _stream;
			shared_ptr<expression> _return_value;
			value_kind _return_stmt_value_kind;
		public:
			return_stmt(shared_ptr<expression> rv, bool v, vector<token> s);
			return_stmt(bool v, vector<token> s);
			~return_stmt();
			bool valid();
			vector<token> stream();
			shared_ptr<expression> return_value();
			value_kind return_stmt_value_kind();
		};

		class while_stmt {
		public:
			enum class while_kind {
				KIND_WHILE, KIND_DO_WHILE, KIND_NONE
			};
		private:
			shared_ptr<expression> _condition;
			shared_ptr<stmt> _while_body;
			bool _valid;
			vector<token> _stream;
			while_kind _while_stmt_kind;
		public:
			while_stmt(while_kind wk, shared_ptr<expression> e, shared_ptr<stmt> b, bool v, vector<token> s);
			~while_stmt();
			shared_ptr<expression> condition();
			shared_ptr<stmt> while_body();
			bool valid();
			vector<token> stream();
			while_kind while_stmt_kind();
		};

		class block_stmt {
		private:
			vector<shared_ptr<stmt>> _stmt_list;
			bool _valid;
			vector<token> _stream;
		public:
			block_stmt(vector<shared_ptr<stmt>> sl, bool v, vector<token> s);
			~block_stmt();
			vector<token> stream();
			bool valid();
			vector<shared_ptr<stmt>> stmt_list();
		};

		class break_continue_stmt {
		public:
			enum class kind {
				KIND_CONTINUE, KIND_BREAK, KIND_NONE
			};
		private:
			kind _break_continue_stmt_kind;
			vector<token> _stream;
			bool _valid;
		public:
			break_continue_stmt(kind k, vector<token> s, bool v);
			~break_continue_stmt();
			kind break_continue_stmt_kind();
			vector<token> stream();
			bool valid();
		};

		class for_stmt {
		public:
			enum class initializer_present_kind {
				INITIALIZER_PRESENT, INITIALIZER_NOT_PRESENT, INITIALIZER_NONE
			};
			enum class condition_present_kind {
				CONDITION_PRESENT, CONDITION_NOT_PRESENT, CONDITION_NONE
			};
			enum class update_present_kind {
				UPDATE_PRESENT, UPDATE_NOT_PRESENT, UPDATE_NONE
			};
		private:
			vector<shared_ptr<variable_declaration>> _initializer;
			shared_ptr<expression> _condition, _update;
			vector<token> _stream;
			bool _valid;
			initializer_present_kind _for_stmt_initializer_present_kind;
			condition_present_kind _for_stmt_condition_present_kind;
			update_present_kind _for_stmt_update_present_kind;
			shared_ptr<stmt> _for_stmt_body;
		public:
			for_stmt(vector<shared_ptr<variable_declaration>> i, initializer_present_kind ip, shared_ptr<expression> c, condition_present_kind cp,
				shared_ptr<expression> u, update_present_kind up, shared_ptr<stmt> b, bool v, vector<token> s);
			~for_stmt();
			vector<shared_ptr<variable_declaration>> initializer();
			shared_ptr<expression> condition();
			shared_ptr<expression> update();
			vector<token> stream();
			bool valid();
			initializer_present_kind for_stmt_initializer_present_kind();
			condition_present_kind for_stmt_condition_present_kind();
			update_present_kind for_stmt_update_present_kind();
			shared_ptr<stmt> for_stmt_body();
		};

		class case_default_stmt {
		public:
			enum class kind {
				KIND_CASE, KIND_DEFAULT, KIND_NONE
			};
		private:
			kind _case_default_stmt_kind;
			shared_ptr<expression> _case_expression;
			shared_ptr<stmt> _case_default_stmt_body;
			bool _valid;
			vector<token> _stream;
			int _case_default_stmt_label_number;
		public:
			case_default_stmt(kind k, shared_ptr<expression> ce, shared_ptr<stmt> b, bool v, int l, vector<token> s);
			~case_default_stmt();
			kind case_default_stmt_kind();
			shared_ptr<expression> case_expression();
			shared_ptr<stmt> case_default_stmt_body();
			bool valid();
			vector<token> stream();
			int case_default_stmt_label_number();
		};

		class switch_stmt {
		private:
			shared_ptr<switch_scope> _switch_stmt_scope;
			shared_ptr<expression> _parent_expression;
			shared_ptr<stmt> _switch_stmt_body;
			bool _valid;
			vector<token> _stream;
		public:
			switch_stmt(shared_ptr<expression> pe, shared_ptr<switch_scope> sss, shared_ptr<stmt> ssb, bool v, vector<token> s);
			~switch_stmt();
			shared_ptr<switch_scope> switch_stmt_scope();
			shared_ptr<expression> parent_expression();
			shared_ptr<stmt> switch_stmt_body();
			bool valid();
			vector<token> stream();
		};

		class struct_stmt {
		public:
			enum class defined_kind {
				KIND_DEFINED, KIND_DECLARED, KIND_NONE
			};
		private:
			shared_ptr<scope> _struct_stmt_scope;
			token _struct_name;
			vector<shared_ptr<variable_declaration>> _struct_variable_declaration_list;
			bool _valid;
			vector<token> _stream;
			shared_ptr<type> _struct_stmt_type;
			shared_ptr<struct_symbol> _struct_stmt_symbol;
			defined_kind _struct_stmt_defined_kind;
		public:
			struct_stmt(defined_kind dk, token sn, shared_ptr<scope> sss, shared_ptr<type> sst, shared_ptr<struct_symbol> ss, vector<shared_ptr<variable_declaration>> vdl, bool v,
				vector<token> s);
			~struct_stmt();
			shared_ptr<scope> struct_stmt_scope();
			token struct_name();
			vector<shared_ptr<variable_declaration>> struct_variable_declaration_list();
			bool valid();
			vector<token> stream();
			shared_ptr<type> struct_stmt_type();
			shared_ptr<struct_symbol> struct_stmt_symbol();
			defined_kind struct_stmt_defined_kind();
		};

		class namespace_stmt {
		public:
			enum class defined_kind {
				KIND_DEFINED, KIND_DECLARED, KIND_NONE
			};
			enum class kind {
				KIND_ALIAS, KIND_REGULAR, KIND_NONE
			};
		private:
			shared_ptr<namespace_scope> _namespace_stmt_scope;
			token _namespace_name;
			vector<shared_ptr<stmt>> _namespace_stmt_list;
			bool _valid;
			vector<token> _stream;
			defined_kind _namespace_stmt_defined_kind;
			shared_ptr<namespace_symbol> _namespace_stmt_symbol;
			kind _namespace_stmt_kind;
		public:
			namespace_stmt(kind k, defined_kind dk, token nn, shared_ptr<namespace_scope> nss, shared_ptr<namespace_symbol> ns, vector<shared_ptr<stmt>> nsl, bool v, vector<token> s);
			~namespace_stmt();
			shared_ptr<namespace_scope> namespace_stmt_scope();
			token namespace_name();
			vector<shared_ptr<stmt>> namespace_stmt_list();
			bool valid();
			vector<token> stream();
			shared_ptr<namespace_symbol> namespace_stmt_symbol();
			defined_kind namespace_stmt_defined_kind();
			kind namespace_stmt_kind();
		};

		class using_stmt {
		public:
			enum class kind {
				KIND_NAMESPACE, KIND_REGULAR_SYMBOL, KIND_NONE
			};
		private:
			kind _using_stmt_kind;
			shared_ptr<symbol> _using_stmt_symbol;
			vector<token> _stream;
			bool _valid;
		public:
			using_stmt(kind k, shared_ptr<symbol> uss, vector<token> s, bool v);
			~using_stmt();
			kind using_stmt_kind();
			shared_ptr<symbol> using_stmt_symbol();
			vector<token> stream();
			bool valid();
		};

		class asm_stmt {
		public:
			enum class kind {
				KIND_RAW_STRING, KIND_LA, KIND_NONE
			};
			class asm_type {
				vector<token> _raw_stmt, _raw_register;
				token _identifier;
				shared_ptr<symbol> _identifier_symbol;
				kind _asm_type_kind;
			public:
				asm_type(vector<token> s);
				asm_type(vector<token> r, shared_ptr<symbol> is, token i);
				~asm_type();
				vector<token> raw_stmt();
				vector<token> raw_register();
				token identifier();
				kind asm_type_kind();
				shared_ptr<symbol> identifier_symbol();
			};
		private:
			vector<shared_ptr<asm_type>> _asm_type_list;
			vector<token> _stream;
			bool _valid;
		public:
			asm_stmt(vector<shared_ptr<asm_type>> atl, vector<token> s, bool v);
			~asm_stmt();
			vector<shared_ptr<asm_type>> asm_type_list();
			vector<token> stream();
			bool valid();
		};

		class include_stmt {
		public:
			enum class type {
				KIND_IMPORT, KIND_INCLUDE, KIND_NONE
			};
			enum class kind {
				KIND_LOCAL, KIND_SYSTEM, KIND_NONE
			};
			class include_type {
			private:
				kind _include_stmt_kind;
				token _local_include;
				vector<token> _system_include;
			public:
				include_type(token li);
				include_type(vector<token> si);
				token local_include();
				vector<token> system_include();
				kind include_stmt_kind();
			};
		private:
			vector<shared_ptr<include_type>> _include_list;
			vector<token> _stream;
			bool _valid;
			type _include_stmt_type;
		public:
			include_stmt(type t, vector<shared_ptr<include_type>> il, vector<token> s, bool v);
			~include_stmt();
			vector<shared_ptr<include_type>> include_list();
			vector<token> stream();
			bool valid();
			type include_stmt_type();
		};

		class access_stmt {
		private:
			vector<token> _stream;
			shared_ptr<type> _declared_type;
			vector<shared_ptr<symbol>> _declared_symbol_list;
			bool _valid;
		public:
			access_stmt(shared_ptr<type> dt, vector<shared_ptr<symbol>> dsl, vector<token> s, bool v);
			~access_stmt();
			vector<token> stream();
			shared_ptr<type> declared_type();
			vector<shared_ptr<symbol>> declared_symbol_list();
			bool valid();
		};

		class delete_stmt {
		private:
			vector<token> _stream;
			shared_ptr<expression> _expr;
			bool _valid;
		public:
			delete_stmt(shared_ptr<expression> e, vector<token> s, bool v);
			~delete_stmt();
			vector<token> stream();
			bool valid();
			shared_ptr<expression> expr();
		};

		class stmt {
		public:
			enum class kind {
				KIND_EXPRESSION, KIND_VARIABLE_DECLARATION, KIND_IF, KIND_EMPTY, KIND_FUNCTION, KIND_RETURN, KIND_WHILE,
				KIND_BLOCK, KIND_BREAK_CONTINUE, KIND_FOR, KIND_DO_WHILE, KIND_CASE_DEFAULT, KIND_SWITCH, KIND_STRUCT,
				KIND_NAMESPACE, KIND_USING, KIND_ASM, KIND_INCLUDE, KIND_ACCESS, KIND_DELETE, KIND_NONE
			};
		private:
			shared_ptr<expression> _stmt_expression;
			vector<shared_ptr<variable_declaration>> _stmt_variable_declaration_list;
			shared_ptr<if_stmt> _stmt_if;
			shared_ptr<empty_stmt> _stmt_empty;
			shared_ptr<function_stmt> _stmt_function;
			shared_ptr<return_stmt> _stmt_return;
			shared_ptr<while_stmt> _stmt_while;
			shared_ptr<block_stmt> _stmt_block;
			shared_ptr<break_continue_stmt> _stmt_break_continue;
			shared_ptr<for_stmt> _stmt_for;
			shared_ptr<case_default_stmt> _stmt_case_default;
			shared_ptr<switch_stmt> _stmt_switch;
			shared_ptr<struct_stmt> _stmt_struct;
			shared_ptr<namespace_stmt> _stmt_namespace;
			shared_ptr<using_stmt> _stmt_using;
			shared_ptr<asm_stmt> _stmt_asm;
			shared_ptr<include_stmt> _stmt_include;
			shared_ptr<access_stmt> _stmt_access;
			shared_ptr<delete_stmt> _stmt_delete;
			kind _stmt_kind;
			bool _valid;
			vector<token> _stream;
		public:
			stmt(kind sk, shared_ptr<expression> se, bool v, vector<token> s);
			stmt(kind sk, vector<shared_ptr<variable_declaration>> svdl, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<if_stmt> si, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<empty_stmt> se, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<function_stmt> sf, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<return_stmt> sr, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<while_stmt> sw, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<block_stmt> sb, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<break_continue_stmt> bc, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<for_stmt> f, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<case_default_stmt> cd, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<switch_stmt> ss, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<struct_stmt> ss, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<namespace_stmt> ns, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<using_stmt> us, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<asm_stmt> as, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<include_stmt> is, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<access_stmt> as, bool v, vector<token> s);
			stmt(kind sk, shared_ptr<delete_stmt> ds, bool v, vector<token> s);
			stmt();
			~stmt();
			shared_ptr<expression> stmt_expression();
			vector<shared_ptr<variable_declaration>> stmt_variable_declaration_list();
			bool valid();
			kind stmt_kind();
			vector<token> stream();
			shared_ptr<if_stmt> stmt_if();
			shared_ptr<empty_stmt> stmt_empty();
			shared_ptr<function_stmt> stmt_function();
			shared_ptr<return_stmt> stmt_return();
			shared_ptr<while_stmt> stmt_while();
			shared_ptr<block_stmt> stmt_block();
			shared_ptr<break_continue_stmt> stmt_break_continue();
			shared_ptr<for_stmt> stmt_for();
			shared_ptr<case_default_stmt> stmt_case_default();
			shared_ptr<switch_stmt> stmt_switch();
			shared_ptr<struct_stmt> stmt_struct();
			shared_ptr<namespace_stmt> stmt_namespace();
			shared_ptr<using_stmt> stmt_using();
			shared_ptr<asm_stmt> stmt_asm();
			shared_ptr<include_stmt> stmt_include();
			shared_ptr<access_stmt> stmt_access();
			shared_ptr<delete_stmt> stmt_delete();
		};
	}
}

#endif
