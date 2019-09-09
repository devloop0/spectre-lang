#ifndef SPECTRE_MIDDLE_IR_HPP
#define SPECTRE_MIDDLE_IR_HPP

#include "parser/parser.hpp"
#include "parser/expr.hpp"
#include "parser/type.hpp"
#include "parser/stmt.hpp"
#include <variant>
#include <unordered_map>
#include <unordered_set>

#ifndef DEDUP
#define DEDUP(OP) do { \
	if ((OP) != nullptr && (OP)->operand_kind() == operand::kind::KIND_REGISTER) { \
		shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(OP); \
		(OP) = make_shared<register_operand>(*ro); \
	} \
} while (0)
#endif

using spectre::parser::parser;
using std::pair;
using std::variant;
using std::unordered_map;
using std::unordered_set;
using std::make_shared;

namespace spectre {
	namespace ir {

		static const string global_header = "@global",
			malloc_symbol_name = "_std_lib__malloc",
			free_symbol_name = "_std_lib__free",
			idiv_symbol_name = "_std_hooks__idiv",
			uidiv_symbol_name = "_std_hooks__uidiv",
			imod_symbol_name = "_std_hooks__imod",
			uimod_symbol_name = "_std_hooks__uimod";

		static constexpr unsigned int PLATFORM_CHAR_BIT = 8;

		static constexpr unsigned int SIZEOF_DOUBLE = 8;
		static constexpr unsigned int SIZEOF_FLOAT = 4;
		static constexpr unsigned int SIZEOF_LONG = 4;
		static constexpr unsigned int SIZEOF_INT = 4;
		static constexpr unsigned int SIZEOF_POINTER = 4;
		static constexpr unsigned int SIZEOF_SHORT = 2;
		static constexpr unsigned int SIZEOF_BOOL = 1;
		static constexpr unsigned int SIZEOF_CHAR = 1;
		static constexpr unsigned int SIZEOF_BYTE = SIZEOF_CHAR;
		static constexpr unsigned int SIZEOF_VOID = 1;

		static const string LABEL_PREFIX = ".L",
			CASE_PREFIX = ".C", DEFAULT_PREFIX = ".D";
		static const string GLOBAL_SCOPE_PREFIX = "_";

		static shared_ptr<primitive_type>
			raw_pointer_type = make_shared<primitive_type>(primitive_type::kind::KIND_BYTE, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_UNSIGNED, 1),
			double_type = make_shared<primitive_type>(primitive_type::kind::KIND_DOUBLE, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_NONE),
			float_type = make_shared<primitive_type>(primitive_type::kind::KIND_FLOAT, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_NONE),
			ulong_type = make_shared<primitive_type>(primitive_type::kind::KIND_LONG, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_UNSIGNED),
			long_type = make_shared<primitive_type>(primitive_type::kind::KIND_LONG, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_SIGNED),
			uint_type = make_shared<primitive_type>(primitive_type::kind::KIND_INT, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_UNSIGNED),
			int_type = make_shared<primitive_type>(primitive_type::kind::KIND_INT, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_SIGNED),
			ushort_type = make_shared<primitive_type>(primitive_type::kind::KIND_SHORT, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_UNSIGNED),
			short_type = make_shared<primitive_type>(primitive_type::kind::KIND_SHORT, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_SIGNED),
			uchar_type = make_shared<primitive_type>(primitive_type::kind::KIND_CHAR, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_UNSIGNED),
			char_type = make_shared<primitive_type>(primitive_type::kind::KIND_CHAR, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_SIGNED),
			ubyte_type = make_shared<primitive_type>(primitive_type::kind::KIND_BYTE, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_UNSIGNED),
			byte_type = make_shared<primitive_type>(primitive_type::kind::KIND_BYTE, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_SIGNED),
			bool_type = make_shared<primitive_type>(primitive_type::kind::KIND_BOOL, primitive_type::const_kind::KIND_NON_CONST,
				primitive_type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_NONE),
			string_type = make_shared<primitive_type>(primitive_type::kind::KIND_CHAR, type::const_kind::KIND_NON_CONST,
				type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_UNSIGNED, 1),
			void_type = make_shared<primitive_type>(primitive_type::kind::KIND_VOID, type::const_kind::KIND_NON_CONST,
				type::static_kind::KIND_NON_STATIC, primitive_type::sign_kind::KIND_NONE);

		static shared_ptr<function_type>
			malloc_type = make_shared<function_type>(type::const_kind::KIND_NON_CONST, type::static_kind::KIND_NON_STATIC, raw_pointer_type, 
				vector<shared_ptr<variable_declaration>>{ make_shared<variable_declaration>(uint_type,
					token("", "", 0, 0, 0, token::kind::TOKEN_IDENTIFIER, ""), true, vector<token>{}) }, -1),
			free_type = make_shared<function_type>(type::const_kind::KIND_NON_CONST, type::static_kind::KIND_NON_STATIC, void_type, 
				vector<shared_ptr<variable_declaration>>{ make_shared<variable_declaration>(raw_pointer_type,
					token("", "", 0, 0, 0, token::kind::TOKEN_IDENTIFIER, ""), true, vector<token>{}) }, -2);

		static shared_ptr<function_type>
			idiv_type = make_shared<function_type>(type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, int_type,
				vector<shared_ptr<variable_declaration>>{
					make_shared<variable_declaration>(int_type, token("", "", 0, 0, 0, token::kind::TOKEN_IDENTIFIER, ""), true, vector<token>{}),
					make_shared<variable_declaration>(int_type, token("", "", 0, 0, 0, token::kind::TOKEN_IDENTIFIER, ""), true, vector<token>{})
				}, -3),
			uidiv_type = make_shared<function_type>(type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, uint_type,
				vector<shared_ptr<variable_declaration>>{
					make_shared<variable_declaration>(uint_type, token("", "", 0, 0, 0, token::kind::TOKEN_IDENTIFIER, ""), true, vector<token>{}),
					make_shared<variable_declaration>(uint_type, token("", "", 0, 0, 0, token::kind::TOKEN_IDENTIFIER, ""), true, vector<token>{})
				}, -4),
			imod_type = make_shared<function_type>(type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, int_type,
				vector<shared_ptr<variable_declaration>>{
					make_shared<variable_declaration>(int_type, token("", "", 0, 0, 0, token::kind::TOKEN_IDENTIFIER, ""), true, vector<token>{}),
					make_shared<variable_declaration>(int_type, token("", "", 0, 0, 0, token::kind::TOKEN_IDENTIFIER, ""), true, vector<token>{})
				}, -5),
			uimod_type = make_shared<function_type>(type::const_kind::KIND_CONST, type::static_kind::KIND_NON_STATIC, uint_type,
				vector<shared_ptr<variable_declaration>>{
					make_shared<variable_declaration>(uint_type, token("", "", 0, 0, 0, token::kind::TOKEN_IDENTIFIER, ""), true, vector<token>{}),
					make_shared<variable_declaration>(uint_type, token("", "", 0, 0, 0, token::kind::TOKEN_IDENTIFIER, ""), true, vector<token>{})
				}, -6);

		class operand;
		class label_operand;
		class register_operand;

		class insn {
		public:
			enum class kind {
				KIND_BINARY, KIND_UNARY, KIND_CALL, KIND_CONDITIONAL, KIND_LABEL, KIND_RETURN, KIND_FUNCTION, KIND_ACCESS, KIND_ALIGN,
				KIND_MEMCPY, KIND_EXT, KIND_TRUNC, KIND_JUMP, KIND_ASM, KIND_VAR, KIND_PHI, KIND_GLOBAL_ARRAY, KIND_AT, KIND_NONE
			};
		private:
			kind _insn_kind;
		public:
			insn(kind k);
			~insn();
			kind insn_kind();
			virtual string to_string() = 0;
			virtual void deduplicate() = 0;
		};

		class label_operand;

		class global_array_insn : public insn {
		public:
			enum class directive_kind {
				KIND_BYTE, KIND_HALF, KIND_WORD, KIND_FLOAT, KIND_DOUBLE, KIND_NONE
			};
		private:
			shared_ptr<label_operand> _label;
			vector<shared_ptr<operand>> _data;
			shared_ptr<type> _array_type;
			directive_kind _directive;
		public:
			global_array_insn(directive_kind dk, shared_ptr<label_operand> l, vector<shared_ptr<operand>> d, shared_ptr<type> t);
			~global_array_insn();
			shared_ptr<label_operand> label();
			vector<shared_ptr<operand>> data();
			shared_ptr<type> array_type();
			directive_kind directive();
			string to_string() override;
			void deduplicate() override;
		};

		class asm_insn : public insn {
		public:
			enum class kind {
				KIND_RAW_STRING, KIND_LA, KIND_NONE
			};
		private:
			variant<string, pair<string, shared_ptr<register_operand>>> _data;
			kind _asm_insn_kind;
		public:
			asm_insn(string d);
			asm_insn(pair<string, shared_ptr<register_operand>> p);
			~asm_insn();
			string raw_string();
			pair<string, shared_ptr<register_operand>> la();
			kind asm_insn_kind();
			string to_string();
			void deduplicate() override;
		};

		class jump_insn : public insn {
		private:
			shared_ptr<label_operand> _label;
		public:
			jump_insn(shared_ptr<label_operand> l);
			~jump_insn();
			shared_ptr<label_operand> label();
			void set_label(shared_ptr<label_operand> l);
			string to_string() override;
			void deduplicate() override;
		};

		class ext_insn : public insn {
		public:
			enum kind {
				KIND_SEXT, KIND_ZEXT, KIND_NONE
			};
		private:
			shared_ptr<operand> _lhs, _rhs;
			unsigned int _from, _to;
			shared_ptr<type> _result_type;
			kind _ext_kind;
		public:
			ext_insn(kind k, shared_ptr<operand> lhs, shared_ptr<operand> rhs, unsigned int f, unsigned int t, shared_ptr<type> r);
			~ext_insn();
			shared_ptr<operand> lhs();
			shared_ptr<operand> rhs();
			unsigned int from();
			unsigned int to();
			shared_ptr<type> result_type();
			kind ext_kind();
			string to_string() override;
			void deduplicate() override;
		};

		class trunc_insn : public insn {
		private:
			shared_ptr<operand> _lhs, _rhs;
			unsigned int _from, _to;
			shared_ptr<type> _result_type;
		public:
			trunc_insn(shared_ptr<operand> lhs, shared_ptr<operand> rhs, unsigned int f, unsigned int t, shared_ptr<type> r);
			~trunc_insn();
			shared_ptr<operand> lhs();
			shared_ptr<operand> rhs();
			shared_ptr<type> result_type();
			unsigned int from();
			unsigned int to();
			string to_string() override;
			void deduplicate() override;
		};

		class memcpy_insn : public insn {
		private:
			shared_ptr<operand> _destination, _source;
			shared_ptr<type> _result_type;
			unsigned int _size;
		public:
			memcpy_insn(shared_ptr<operand> d, shared_ptr<operand> s, unsigned int sz, shared_ptr<type> r);
			~memcpy_insn();
			shared_ptr<operand> destination();
			shared_ptr<operand> source();
			unsigned int size();
			shared_ptr<type> result_type();
			string to_string() override;
			void deduplicate() override;
		};

		class align_insn : public insn {
		private:
			unsigned int _alignment;
		public:
			align_insn(unsigned int a);
			~align_insn();
			unsigned int alignment();
			string to_string() override;
			void deduplicate() override;
		};

		class access_insn : public insn {
		private:
			string _symbol;
			shared_ptr<operand> _variable;
		public:
			access_insn(string s, shared_ptr<operand> v);
			~access_insn();
			shared_ptr<operand> variable();
			string symbol();
			string to_string() override;
			void deduplicate() override;
		};

		class function_insn : public insn {
		private:
			shared_ptr<function_type> _func_type;
			shared_ptr<function_symbol> _func_symbol;
			shared_ptr<function_scope> _func_scope;
			vector<shared_ptr<insn>> _insn_list;
			shared_ptr<register_operand> _return_register;
		public:
			function_insn(shared_ptr<function_type> ft, shared_ptr<function_symbol> fs, shared_ptr<function_scope> fsc, shared_ptr<register_operand> r);
			~function_insn();
			shared_ptr<function_type> func_type();
			shared_ptr<function_symbol> func_symbol();
			shared_ptr<function_scope> func_scope();
			vector<shared_ptr<insn>> insn_list();
			shared_ptr<register_operand> return_register();
			void add_insn(shared_ptr<insn> i);
			string to_string() override;
			void deduplicate() override;
		};

		class return_insn : public insn {
		public:
			enum class kind {
				KIND_VOID, KIND_REGULAR, KIND_NONE
			};
		private:
			shared_ptr<operand> _expr;
			kind _return_kind;
		public:
			return_insn(shared_ptr<operand> e);
			return_insn();
			~return_insn();
			shared_ptr<operand> expr();
			kind return_kind();
			string to_string() override;
			void deduplicate() override;
		};

		class function_insn;

		class label_insn : public insn {
		private:
			shared_ptr<label_operand> _label;
			vector<int> _func_param_regs;
			int _func_return_reg;
			shared_ptr<function_insn> _orig_func_insn;
		public:
			label_insn(shared_ptr<label_operand> l);
			label_insn(shared_ptr<label_operand> l, vector<int> fpr, int frr, shared_ptr<function_insn> ofi);
			~label_insn();
			shared_ptr<label_operand> label();
			vector<int> func_param_regs();
			void set_func_param_regs(vector<int> regs);
			int func_return_reg();
			void set_func_return_reg(int i);
			void set_orig_func_insn(shared_ptr<function_insn> ofi);
			shared_ptr<function_insn> orig_func_insn();
			string to_string() override;
			void deduplicate() override;
		};

		class conditional_insn : public insn {
		private:
			shared_ptr<operand> _src;
			shared_ptr<label_operand> _branch;
		public:
			conditional_insn(shared_ptr<operand> s, shared_ptr<label_operand> b);
			~conditional_insn();
			shared_ptr<operand> src();
			shared_ptr<label_operand> branch();
			void set_branch(shared_ptr<label_operand> l);
			string to_string() override;
			void deduplicate() override;
		};

		class call_insn : public insn {
		private:
			vector<shared_ptr<operand>> _argument_list;
			shared_ptr<operand> _function_operand;
			shared_ptr<operand> _dest_operand;
			shared_ptr<type> _result_type;
			bool _is_variable;
			shared_ptr<variable_declaration> _variable_decl;
			shared_ptr<function_type> _called_function_type;
		public:
			call_insn(shared_ptr<type> rt, shared_ptr<operand> d, shared_ptr<operand> fo,
				vector<shared_ptr<operand>> al, shared_ptr<function_type> ft);
			call_insn(shared_ptr<type> rt, shared_ptr<operand> d, shared_ptr<operand> fo, vector<shared_ptr<operand>> al,
				shared_ptr<variable_declaration> vd, shared_ptr<function_type> ft);
			~call_insn();
			shared_ptr<operand> function_operand();
			shared_ptr<type> result_type();
			shared_ptr<operand> dest_operand();
			vector<shared_ptr<operand>> argument_list();
			bool is_variable();
			shared_ptr<variable_declaration> variable_decl();
			shared_ptr<function_type> called_function_type();
			string to_string() override;
			void deduplicate() override;
		};

		class binary_insn : public insn {
		public:
			enum class kind {
				KIND_LADD, KIND_IADD, KIND_SADD, KIND_CADD, KIND_DADD, KIND_FADD,
				KIND_LSUB, KIND_ISUB, KIND_SSUB, KIND_CSUB, KIND_DSUB, KIND_FSUB,
				KIND_LMUL, KIND_ULMUL, KIND_IMUL, KIND_UIMUL, KIND_SMUL, KIND_USMUL, KIND_CMUL, KIND_UCMUL, KIND_DMUL, KIND_FMUL,
				KIND_LDIV, KIND_ULDIV, KIND_IDIV, KIND_UIDIV, KIND_SDIV, KIND_USDIV, KIND_CDIV, KIND_UCDIV, KIND_DDIV, KIND_FDIV,
				KIND_LMOD, KIND_ULMOD, KIND_IMOD, KIND_UIMOD, KIND_SMOD, KIND_USMOD, KIND_CMOD, KIND_UCMOD,
				KIND_LSHL, KIND_ISHL, KIND_SSHL, KIND_CSHL,
				KIND_LSHR, KIND_ISHR, KIND_SSHR, KIND_CSHR,
				KIND_LSHRA, KIND_ISHRA, KIND_SSHRA, KIND_CSHRA,
				KIND_LBAND, KIND_IBAND, KIND_SBAND, KIND_CBAND,
				KIND_LBOR, KIND_IBOR, KIND_SBOR, KIND_CBOR,
				KIND_LBXOR, KIND_IBXOR, KIND_SBXOR, KIND_CBXOR,
				KIND_DGT, KIND_FGT, KIND_LGT, KIND_ULGT, KIND_IGT, KIND_UIGT, KIND_SGT, KIND_USGT, KIND_CGT, KIND_UCGT,
				KIND_DLT, KIND_FLT, KIND_LLT, KIND_ULLT, KIND_ILT, KIND_UILT, KIND_SLT, KIND_USLT, KIND_CLT, KIND_UCLT,
				KIND_DGE, KIND_FGE, KIND_LGE, KIND_ULGE, KIND_IGE, KIND_UIGE, KIND_SGE, KIND_USGE, KIND_CGE, KIND_UCGE,
				KIND_DLE, KIND_FLE, KIND_LLE, KIND_ULLE, KIND_ILE, KIND_UILE, KIND_SLE, KIND_USLE, KIND_CLE, KIND_UCLE,
				KIND_DEQ, KIND_FEQ, KIND_LEQ, KIND_IEQ, KIND_SEQ, KIND_CEQ,
				KIND_DNE, KIND_FNE, KIND_LNE, KIND_INE, KIND_SNE, KIND_CNE,
				KIND_NONE
			};
		private:
			shared_ptr<operand> _src1_operand, _src2_operand, _dst_operand;
			kind _binary_expr_kind;
			shared_ptr<type> _result_type;
			shared_ptr<variable_declaration> _variable_decl;
			bool _is_variable_decl;
		public:
			binary_insn(kind k, shared_ptr<operand> d, shared_ptr<operand> s1, shared_ptr<operand> s2, shared_ptr<type> r);
			binary_insn(kind k, shared_ptr<operand> d, shared_ptr<operand> s1, shared_ptr<operand> s2, shared_ptr<type> r, shared_ptr<variable_declaration> vdecl);
			~binary_insn();
			shared_ptr<operand> src1_operand();
			shared_ptr<operand> src2_operand();
			shared_ptr<operand> dst_operand();
			kind binary_expr_kind();
			shared_ptr<type> result_type();
			bool is_variable_decl();
			shared_ptr<variable_declaration> variable_decl();
			string to_string() override;
			static string kind_to_string(binary_insn::kind k);
			void deduplicate() override;
		};

		class unary_insn : public insn {
		public:
			enum class kind {
				KIND_LPLUS, KIND_IPLUS, KIND_SPLUS, KIND_CPLUS, KIND_DPLUS, KIND_FPLUS,
				KIND_LMINUS, KIND_IMINUS, KIND_SMINUS, KIND_CMINUS, KIND_DMINUS, KIND_FMINUS,
				KIND_LNOT,
				KIND_LBNOT, KIND_IBNOT, KIND_SBNOT, KIND_CBNOT,
				KIND_LMOV, KIND_IMOV, KIND_SMOV, KIND_CMOV, KIND_DMOV, KIND_FMOV,
				KIND_RESV, KIND_STK, 
				KIND_LTOD, KIND_LTOF, KIND_ULTOD, KIND_ULTOF,
				KIND_ITOD, KIND_ITOF, KIND_UITOD, KIND_UITOF,
				KIND_STOD, KIND_STOF, KIND_USTOD, KIND_USTOF,
				KIND_CTOD, KIND_CTOF, KIND_UCTOD, KIND_UCTOF,
				KIND_DTOL, KIND_DTOUL, KIND_DTOI, KIND_DTOUI, KIND_DTOS, KIND_DTOUS, KIND_DTOC, KIND_DTOUC, KIND_DTOF,
				KIND_FTOL, KIND_FTOUL, KIND_FTOI, KIND_FTOUI, KIND_FTOS, KIND_FTOUS, KIND_FTOC, KIND_FTOUC, KIND_FTOD,
				KIND_NONE
			};
		private:
			shared_ptr<type> _result_type;
			kind _unary_expr_kind;
			shared_ptr<operand> _src_operand, _dst_operand;
			bool _is_variable_decl;
			shared_ptr<variable_declaration> _variable_decl;
		public:
			unary_insn(kind k, shared_ptr<operand> d, shared_ptr<operand> s, shared_ptr<type> r);
			unary_insn(kind k, shared_ptr<operand> d, shared_ptr<operand> s, shared_ptr<type> r, shared_ptr<variable_declaration> vdecl);
			~unary_insn();
			shared_ptr<type> result_type();
			kind unary_expr_kind();
			shared_ptr<operand> src_operand();
			shared_ptr<operand> dst_operand();
			shared_ptr<variable_declaration> variable_decl();
			bool is_variable_decl();
			string to_string() override;
			static string kind_to_string(unary_insn::kind k);
			void deduplicate() override;
		};

		class operand {
		public:
			enum class kind {
				KIND_IMMEDIATE, KIND_REGISTER, KIND_LABEL, KIND_NONE
			};
		private:
			kind _operand_kind;
		public:
			operand(kind k);
			~operand();
			kind operand_kind();
			virtual string to_string() = 0;
		};
		
		class label_operand : public operand {
		private:
			string _label_text;
			shared_ptr<type> _result_type;
		public:
			label_operand(string lt, shared_ptr<type> t);
			~label_operand();
			string label_text();
			shared_ptr<type> result_type();
			string to_string() override;
		};

		class immediate_operand : public operand {
		public:
			enum class kind {
				KIND_CHAR, KIND_SHORT, KIND_INT, KIND_LONG, KIND_FLOAT, KIND_DOUBLE, KIND_STRING, KIND_NONE
			};
		private:
			variant<char, short, int, long, float, double, string> _immediate;
			kind _immediate_operand_kind;
			shared_ptr<type> _immediate_type;
		public:
			immediate_operand(shared_ptr<type> t, variant<char, short, int, long, float, double, string> i);
			immediate_operand(shared_ptr<type> t, char c);
			immediate_operand(shared_ptr<type> t, short s);
			immediate_operand(shared_ptr<type> t, int i);
			immediate_operand(shared_ptr<type> t, long l);
			immediate_operand(shared_ptr<type> t, float f);
			immediate_operand(shared_ptr<type> t, double d);
			immediate_operand(shared_ptr<type> t, string s);
			~immediate_operand();
			char immediate_char();
			short immediate_short();
			int immediate_int();
			long immediate_long();
			float immediate_float();
			double immediate_double();
			string immediate_string();
			kind immediate_operand_kind();
			shared_ptr<type> immediate_type();
			variant<char, short, int, long, float, double, string> immediate();
			string to_string() override;
		};

		class register_operand : public operand {
		private:
			int _virtual_register_number, _original_virtual_register_number, _base_virtual_register_number;
			bool _dereference;
			shared_ptr<type> _register_type;
		public:
			register_operand(bool d, int vrn, shared_ptr<type> rt);
			~register_operand();
			int virtual_register_number();
			shared_ptr<type> register_type();
			void set_register_type(shared_ptr<type> t);
			bool dereference();
			void set_dereference(bool b);
			void set_virtual_register_number(int vrn, bool overwrite = true);
			void set_base_virtual_register_number(int vrn);
			string to_string() override;
			int original_virtual_register_number();
			int base_virtual_register_number();
		};

		class middle_ir {
		private:
			map<string, shared_ptr<variable_declaration>> _mangled_name_2_ast_map;
			map<string, shared_ptr<register_operand>> _mangled_name_2_register_map;
			map<string, variant<bool, int, unsigned int, float, double, string>> _mangled_name_2_constexpr_value_map;
			vector<shared_ptr<insn>> _insn_list, _global_insn_list;
			shared_ptr<spectre::parser::parser> _ast_parser;
			int _virtual_register_counter, _label_counter, _misc_counter;
			shared_ptr<label_operand> _return_label;
			shared_ptr<register_operand> _return_register;
			vector<shared_ptr<function_insn>> _function_insn_list;
			stack<shared_ptr<label_operand>> _loop_start_stack, _loop_end_stack;
			shared_ptr<function_insn> _function_ctx;
			bool _function_ctx_set, _global_ctx_set;
			unordered_map<int, shared_ptr<type>> _register_2_type;
		public:
			middle_ir(shared_ptr<spectre::parser::parser> p);
			~middle_ir();
			shared_ptr<struct_symbol> find_struct_symbol(token t, int r);
			vector<shared_ptr<insn>> insn_list();
			vector<shared_ptr<insn>> global_insn_list();
			void add_function_insn(shared_ptr<function_insn> fi);
			vector<shared_ptr<function_insn>> function_insn_list();
			void set_insn_list(vector<shared_ptr<insn>> il);
			void set_global_insn_list(vector<shared_ptr<insn>> il);
			void clear_insn_list();
			void clear_global_insn_list();
			shared_ptr<variable_declaration> lookup_variable(string mangled_name);
			void add_variable(string mangled_name, shared_ptr<variable_declaration> vdecl);
			void map_variable_register(string mangled_name, shared_ptr<register_operand> op);
			shared_ptr<register_operand> lookup_variable_register(string mangled_name);
			void add_constexpr_mapping(string mangled_name, variant<bool, int, unsigned int, float, double, string> c);
			pair<bool, variant<bool, int, unsigned int, float, double, string>> lookup_constexpr_mapping(string mangled_name);
			void report_internal(string msg, string fn, int ln, string fl);
			void report(error e);
			void push_loop_start_stack(shared_ptr<label_operand> l);
			void push_loop_end_stack(shared_ptr<label_operand> l);
			shared_ptr<label_operand> loop_start();
			shared_ptr<label_operand> loop_end();
			void pop_loop_start_stack();
			void pop_loop_end_stack();
			void set_function_ctx(shared_ptr<function_insn> fi);
			shared_ptr<function_insn> get_function_ctx();
			void unset_function_ctx();
			bool function_ctx_set();
			void add_insn(shared_ptr<insn> i);
			void set_return_label(shared_ptr<label_operand> l);
			shared_ptr<label_operand> get_return_label();
			shared_ptr<register_operand> get_return_register();
			void set_return_register(shared_ptr<register_operand> r);
			shared_ptr<register_operand> generate_next_register(shared_ptr<type> t);
			shared_ptr<label_operand> generate_next_label_operand();
			void add_global_insn(shared_ptr<insn> i);
			void set_global_ctx_set(bool b);
			bool get_global_ctx_set();
			int get_num_allocated_registers();
			vector<string> variable_names();
			vector<string> constexpr_variable_names();
			int next_virtual_register_number();
			int next_misc_counter();
			int get_virtual_register_number();
			void add_register_type(int j, shared_ptr<type> t);
			shared_ptr<type> get_register_type(int j);
		};

		struct size_data {
			unsigned int size, alignment;
		};

		string c_scope_2_string_helper(shared_ptr<middle_ir> mi, shared_ptr<scope> s, string r);
		string c_scope_2_string(shared_ptr<middle_ir> mi, shared_ptr<scope> s);
		string c_symbol_2_string(shared_ptr<middle_ir> mi, shared_ptr<symbol> s);
		shared_ptr<size_data> spectre_sizeof(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		unordered_map<string, unsigned int> member_offsets(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		shared_ptr<operand> ir_cast(shared_ptr<middle_ir> mi, shared_ptr<operand> src, shared_ptr<type> from, shared_ptr<type> to);
		shared_ptr<type> dominant_type(shared_ptr<middle_ir> mi, shared_ptr<type> t1, shared_ptr<type> t2);
		shared_ptr<register_operand> load_lvalue_to_register(shared_ptr<middle_ir> mi, shared_ptr<operand> op);
		char literal_token_to_char(shared_ptr<middle_ir> mi, token tok);

		bool is_double_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_float_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_floating_point_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_struct_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_function_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_void_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_bool_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_char_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_byte_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_short_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_int_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_long_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_pointer_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_integral_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_unsigned_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		bool is_signed_type(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		shared_ptr<register_operand> to_register_operand(shared_ptr<middle_ir> mi, shared_ptr<operand> i);
		pair<bool, unary_insn::kind> which_mov_insn(shared_ptr<middle_ir> mi, shared_ptr<type> t);
		tuple<vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>, vector<shared_ptr<stmt>>> reorder_stmt_list(shared_ptr<middle_ir> mi, vector<shared_ptr<stmt>> sl);
		bool is_function_main(shared_ptr<middle_ir> mi, shared_ptr<function_symbol> fsym);
		variant<bool, int, unsigned int, float, double, string> evaluate_constant_expression(shared_ptr<middle_ir> mi, shared_ptr<assignment_expression> ae);
		template<typename C, typename TL, typename TR> auto raw_arithmetic_binary_expression_evaluator(shared_ptr<middle_ir> mi, TL lhs, TR rhs, binary_expression::operator_kind ok);
		template<typename C, typename TL, typename TR> bool raw_logical_binary_expression_evaluator(shared_ptr<middle_ir> mi, TL lhs, TR rhs, binary_expression::operator_kind ok);
		shared_ptr<operand> constexpr_value_2_operand(shared_ptr<middle_ir> mi, variant<bool, int, unsigned int, float, double, string> c, shared_ptr<type> t);

		shared_ptr<type> copy_type(shared_ptr<type> t);
		shared_ptr<type> dereference(shared_ptr<type> t);
		shared_ptr<type> add_indirection(shared_ptr<type> t);

		void generate_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<spectre::parser::parser> p);
		void generate_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<stmt> s);
		shared_ptr<operand> generate_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<expression> e, bool lvalue);
		shared_ptr<operand> generate_assignment_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<assignment_expression> ae, bool lvalue);
		shared_ptr<operand> generate_ternary_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<ternary_expression> te, bool lvalue);
		shared_ptr<operand> generate_binary_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<binary_expression> be, bool lvalue);
		shared_ptr<operand> generate_unary_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<unary_expression> ue, bool lvalue);
		shared_ptr<operand> generate_postfix_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<postfix_expression> pe, bool lvalue);
		shared_ptr<operand> generate_primary_expression_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<primary_expression> pe, bool lvalue);
		void generate_function_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<function_stmt> fs);
		void generate_variable_declaration_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<variable_declaration> vd);
		void generate_return_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<return_stmt> rs);
		void generate_if_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<if_stmt> is);
		void generate_while_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<while_stmt> ws);
		void generate_block_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<block_stmt> bs);
		void generate_break_continue_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<break_continue_stmt> bcs);
		void generate_switch_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<switch_stmt> ss);
		void generate_case_default_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<case_default_stmt> cds);
		void generate_for_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<for_stmt> fs);
		void generate_delete_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<delete_stmt> ds);
		void generate_asm_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<asm_stmt> as);
		void generate_access_stmt_middle_ir(shared_ptr<middle_ir> mi, shared_ptr<access_stmt> as);

		enum class main_function_kind {
			MAIN_NO_ARGS,
			MAIN_ARGC_ARGV,
			MAIN_ARGC_ARGV_ENVP,
			NONE
		};

		main_function_kind main_function_defined(shared_ptr<function_symbol> fsym);
	}
}

#endif
