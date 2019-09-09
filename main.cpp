#include "lexer/buffer.hpp"
#include "lexer/token.hpp"
#include "core/error.hpp"
#include "core/diagnostics.hpp"
#include "lexer/token_keywords.hpp"
#include "parser/parser.hpp"
#include "parser/stmt.hpp"
#include "parser/expr.hpp"
#include "codegen/mips/mips_instruction_set.hpp"
#include "core/file_io.hpp"
#include "codegen/mips/mips2.hpp"
#include "ir/middle_ir.hpp"
#include "ir/basic_block.hpp"
#include "opt/mem2reg.hpp"
#include "opt/cvt2ssa.hpp"
#include "opt/insns_in_bb.hpp"
#include "opt/fold_constants.hpp"
#include "opt/basic_dce.hpp"
#include "opt/simplify_cfg.hpp"
#include "pass/cycle_pass.hpp"
#include "opt/cvt_from_ssa.hpp"
#include "pass/check_pass.hpp"
#include "opt/calc_liveness.hpp"
#include "codegen/alloc_regs.hpp"
#include "codegen/arm32/arm32.hpp"
#include <string>
#include <memory>
#include <iostream>
#include <stdio.h>

using namespace spectre::keywords;
using namespace spectre::lexer;
using namespace spectre::ast;
using namespace spectre::parser;
using namespace spectre::mips;
using namespace spectre::mips2;
using namespace spectre::file_io;
using std::string;
using std::make_shared;
using std::static_pointer_cast;
using std::getline;
using std::cerr;
using std::cout;
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

			constexpr int num_phys_regs = 3;

			shared_ptr<spectre::ir::middle_ir> mi = make_shared<spectre::ir::middle_ir>(p);
			spectre::ir::generate_middle_ir(mi, p);
			if (b.diagnostics_reporter()->error_count() != 0)
				return 0;
			/*cout << '\n' << "============Generated Code============" << '\n';
			for (shared_ptr<spectre::ir::insn> i : mi->global_insn_list()) cout << i->to_string() << '\n';
			for (shared_ptr<spectre::ir::insn> i : mi->insn_list()) cout << i->to_string() << '\n';*/
			spectre::opt::pass_manager::debug_level dl = spectre::opt::pass_manager::debug_level::KIND_NONDEBUG;
			shared_ptr<spectre::opt::pass_manager> pm = make_shared<spectre::opt::pass_manager>(dl);
			pm->add_pass(make_shared<spectre::opt::mem2reg_pass>(pm));
			pm->add_pass(make_shared<spectre::opt::cvt2ssa_pass>(pm));
			pm->add_pass(make_shared<spectre::opt::insns_in_bb_pass>(pm));
			shared_ptr<spectre::opt::fold_constants_pass> fcp = make_shared<spectre::opt::fold_constants_pass>(pm);
			shared_ptr<spectre::opt::basic_dce_pass> bdce = make_shared<spectre::opt::basic_dce_pass>(pm);
			shared_ptr<spectre::opt::simplify_cfg_pass> scp = make_shared<spectre::opt::simplify_cfg_pass>(pm);
			shared_ptr<spectre::opt::cycle_pass> basic_cycle = make_shared<spectre::opt::cycle_pass>(pm, "basic_cycle",
				"Cycles constant folding, basic dce, and cfg simplification", unordered_set<string>{ "insns_in_bb" }, 0,
				vector<shared_ptr<spectre::opt::pass>>{ fcp, bdce, scp });
			pm->add_pass(basic_cycle);


			shared_ptr<spectre::opt::check_pass> ssa_check = make_shared<spectre::opt::check_pass>(pm, "ssa_check", "Checks SSA integrity.",
					[] (shared_ptr<basic_blocks>& bbs) {
						bbs = spectre::opt::insns_in_bb_2_straight_line(bbs);
						spectre::opt::ssa_sanity_check(bbs);
						spectre::opt::place_insns_in_bb(bbs);
					}, unordered_set<string> { "insns_in_bb", "basic_cycle"} );
			pm->add_pass(ssa_check);

			shared_ptr<spectre::opt::cvt_from_ssa_pass> cfsp = pm->template build_pass<spectre::opt::cvt_from_ssa_pass>({ basic_cycle->name(), "ssa_check" });
			pm->add_pass(cfsp);

			shared_ptr<spectre::ir::basic_blocks> opt_bbs = pm->run_passes(mi);

			vector<string> asm_insn = spectre::arm32::generate_asm(opt_bbs, p, dl);
			if (dl >= spectre::opt::pass_manager::debug_level::KIND_DEBUG) {
				cout << endl << "============Generated Code============" << endl;
				for (const auto& i : asm_insn)
					cout << i << endl;
			}

			string gen;
			for (string s : asm_insn) gen += s + "\n";
			string pd = get_parent_path(full_path), s = get_file_stem(full_path) + ".s";
			string asm_file = construct_file_name(vector<string>{ pd, s });
			write_to_file(asm_file, gen);
#endif
		}
	}
}
