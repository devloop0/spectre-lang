#ifndef SPECTRE_TOKEN_KEYWORDS_HPP
#define SPECTRE_TOKEN_KEYWORDS_HPP

#include <string>

using std::string;

namespace spectre {
	namespace keywords {
		const static string _const = "const";
		const static string _static = "static";
		const static string _signed = "signed";
		const static string _unsigned = "unsigned";
		const static string _byte = "byte";
		const static string _short = "short";
		const static string _long = "long";
		const static string _int = "int";
		const static string _char = "char";
		const static string _float = "float";
		const static string _double = "double";
		const static string _bool = "bool";
		const static string _true = "true";
		const static string _false = "false";
		const static string _void = "void";
		const static string _if = "if";
		const static string _else = "else";
		const static string _func = "func";
		const static string _return = "return";
		const static string _while = "while";
		const static string _break = "break";
		const static string _continue = "continue";
		const static string _for = "for";
		const static string _do = "do";
		const static string _switch = "switch";
		const static string _default = "default";
		const static string _case = "case";
		const static string _struct = "struct";
		const static string _type = "type";
		const static string _new = "new";
		const static string _sizeof = "sizeof";
		const static string _namespace = "namespace";
		const static string _using = "using";
		const static string _asm_ = "__asm__";
		const static string _import = "import";
		const static string _include = "include";
		const static string _as = "as";
		const static string _stk = "stk";
		const static string _resv = "resv";
		const static string _fn = "fn";
		const static string _access = "access";
		const static string _constexpr = "constexpr";
		const static string _auto = "auto";
		const static string _delete = "delete";
	}
}

#endif