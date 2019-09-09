#ifndef SPECTRE_OPT_CYCLE_PASS_HPP
#define SPECTRE_OPT_CYCLE_PASS_HPP

#include <memory>
#include <unordered_set>
#include <string>
#include <vector>

#include "pass/pass_manager.hpp"
#include "pass/pass.hpp"
#include "ir/middle_ir.hpp"
#include "ir/basic_block.hpp"

using std::shared_ptr;
using std::vector;
using std::unordered_set;
using std::string;
using namespace spectre::ir;

namespace spectre {
	namespace opt {

		class cycle_pass : public pass {
		private:
			vector<shared_ptr<pass>> _passes;
			pass_manager::debug_level _debug_ctx;
			int _limit;
		public:
			cycle_pass(shared_ptr<pass_manager> pm, string name, string desc,
					unordered_set<string> deps, int l, vector<shared_ptr<pass>> p);
			cycle_pass(shared_ptr<pass_manager> pm, string name, string desc, int l,
					vector<shared_ptr<pass>> p, int counter,
					unordered_set<string> deps);
			~cycle_pass();
			shared_ptr<basic_blocks> run_pass(shared_ptr<basic_blocks> bbs) override;
		};
	}
}

#endif
