#include "buffer.hpp"
#include "token.hpp"
#include "error.hpp"
#include "diagnostics.hpp"
#include "token_keywords.hpp"
#include "parser.hpp"
#include "stmt.hpp"
#include "expr.hpp"
#include "ast_printer.hpp"
#include "lir.hpp"
#include "lir_instruction_set.hpp"
#include "lir.hpp"
#include "mips.hpp"
#include "mips_instruction_set.hpp"
#include "file_io.hpp"
#include "mips2.hpp"
#include "middle_ir.hpp"
#include "basic_block.hpp"
#include "opt_passes/mem2reg.hpp"
#include "opt_passes/cvt2ssa.hpp"
#include "opt_passes/insns_in_bb.hpp"
#include "opt_passes/fold_constants.hpp"
#include "opt_passes/basic_dce.hpp"
#include <string>
#include <memory>
#include <stdio.h>

using namespace spectre::keywords;
using namespace spectre::lexer;
using namespace spectre::ast;
using namespace spectre::parser;
using namespace spectre::ast::debug;
using namespace spectre::lir::insns;
using namespace spectre::lir;
using namespace spectre::mips;
using namespace spectre::mips2;
using namespace spectre::file_io;
using std::string;
using std::make_shared;
using std::getline;
using std::cerr;
using std::exit;
using std::cin;
using std::endl;

int main(int argc, char* argv[]) {
	string src = "", line = "";
	/*while (cout << ">>> ", getline(cin, line)) {
		if (line == "$$EOF$$") break;
		else src += line + "\n";
	}
	std::cout << '\n' << '\n';*/
	if (argc < 2) {
		cerr << "Expected at least 1 command line argument with the name of the file to compile";
		exit(1);
	}
	for (string arg : vector<string>(argv + 1, argv + argc)) {
		string full_path = construct_file_name(vector<string>{ current_working_directory(), arg });
		string fn = get_file_name(full_path), src = get_file_source(full_path);
		buffer b(current_working_directory(), fn, make_shared<diagnostics>(), src);
		shared_ptr<parser> p = make_shared<parser>(b);
		bool b_ = p->parse_stmt();
		while (b_)
			b_ = p->parse_stmt();
		if (b.diagnostics_reporter()->error_count() == 0) {
			/*shared_ptr<lir_code> lc = make_shared<lir_code>(p);
			for (shared_ptr<stmt> s : p->stmt_list())
				generate_stmt_lir(lc, s);
			if (b.diagnostics_reporter()->error_count() == 0) {
				cout << '\n' << "============Generated Code============" << '\n';
				for (string s : lc->raw_instruction_list())
					cout << s << '\n';
			}*/

#if BACKEND == BACKEND_MIPS
			shared_ptr<spectre::mips2::mips_code> mc = make_shared<spectre::mips2::mips_code>(p);
			spectre::mips2::generate_mips(mc, p);
			string gen = "";
			for (string s : mc->raw_insn_list()) gen += s + "\n";
			string pd = get_parent_path(full_path), s = get_file_stem(full_path) + ".s";
			string asm_file = construct_file_name(vector<string>{ pd, s });
			write_to_file(asm_file, gen);

			// cout << '\n' << "============Generated Code============" << '\n';
			// for(string s : mc->raw_insn_list()) cout << s << '\n';

#elif BACKEND == BACKEND_IR
			shared_ptr<spectre::ir::middle_ir> mi = make_shared<spectre::ir::middle_ir>(p);
			spectre::ir::generate_middle_ir(mi, p);
			/*cout << '\n' << "============Generated Code============" << '\n';
			for (shared_ptr<spectre::ir::insn> i : mi->global_insn_list()) cout << i->to_string() << '\n';
			for (shared_ptr<spectre::ir::insn> i : mi->insn_list()) cout << i->to_string() << '\n';*/
			shared_ptr<spectre::opt::pass_manager> pm = make_shared<spectre::opt::pass_manager>(spectre::opt::pass_manager::debug_level::KIND_DEBUG);
			pm->add_pass(make_shared<spectre::opt::mem2reg_pass>(pm));
			pm->add_pass(make_shared<spectre::opt::cvt2ssa_pass>(pm));
			pm->add_pass(make_shared<spectre::opt::insns_in_bb_pass>(pm));
			shared_ptr<spectre::opt::fold_constants_pass> fcp = pm->template build_pass<spectre::opt::fold_constants_pass>();
			pm->add_pass(fcp);
			shared_ptr<spectre::opt::basic_dce_pass> bdce = pm->template build_pass<spectre::opt::basic_dce_pass>(
					unordered_set<string>{ fcp->name() });
			pm->add_pass(bdce);
			shared_ptr<spectre::ir::basic_blocks> opt_bbs = pm->run_passes(mi);
#endif
		}
	}
}
