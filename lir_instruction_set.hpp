#ifndef SPECTRE_LIR_INSTRUCTION_SET_HPP
#define SPECTRE_LIR_INSTRUCTION_SET_HPP

#include <string>

using std::string;

namespace spectre {
	namespace lir {
		namespace insns {

			const static string _mov = "mov";
			const static string _dmov = "dmov";
			const static string _add = "add";
			const static string _sub = "sub";
			const static string _mul = "mul";
			const static string _div = "div";
			const static string _mod = "mod";
			const static string _shl = "shl";
			const static string _shr = "shr";
			const static string _band = "band";
			const static string _bor = "bor";
			const static string _bxor = "bxor";
			const static string _lt = "lt";
			const static string _gt = "gt";
			const static string _lte = "lte";
			const static string _gte = "gte";
			const static string _eq = "eq";
			const static string _neq = "neq";

			const static string _neg = "neg";
			const static string _pos = "pos";
			const static string _inc = "inc";
			const static string _dec = "dec";
			const static string _lnot = "lnot";
			const static string _bnot = "bnot";

			const static string _jmp = "jmp";
			const static string _jlt = "jlt";
			const static string _jlte = "jlte";
			const static string _jgt = "jgt";
			const static string _jgte = "jgte";
			const static string _jeq = "jeq";
			const static string _jneq = "jneq";

			const static string _proc = "proc";
			const static string _eproc = "eproc";
			const static string _arr = "arr";
			const static string _ret = "ret";
			const static string _call = "call";
			const static string _subscr = "subscr";
			const static string _struc = "struc";
			const static string _estruc = "estruc";
			const static string _malloc = "malloc";
		}
	}
}

#endif