#ifndef SPECTRE_OPT_BASIC_DCE_HPP
#define SPECTRE_OPT_BASIC_DCE_HPP

#include "pass/pass_manager.hpp"
#include "pass/pass.hpp"
#include "ir/basic_block.hpp"
#include "ir/middle_ir.hpp"

#include <algorithm>
#include <unordered_set>
#include <memory>

using namespace spectre::ir;
using std::pair;
using std::unordered_set;
using std::shared_ptr;

namespace spectre {
	namespace opt {

		pair<unordered_set<int>, unordered_set<int>> function_and_global_headers(shared_ptr<basic_blocks> bbs);

		shared_ptr<basic_blocks> insns_in_bb_2_straight_line(shared_ptr<basic_blocks> bbs);
		void remove_unused_registers(shared_ptr<basic_blocks> bbs);
		void remove_unreachable_bbs(shared_ptr<basic_blocks> bbs);

		class basic_dce_pass : public pass {
		private:
			pass_manager::debug_level _debug_ctx;
		public:
			basic_dce_pass(shared_ptr<pass_manager> pm);
			basic_dce_pass(shared_ptr<pass_manager> pm, int counter, unordered_set<string> deps);
			~basic_dce_pass();
			shared_ptr<basic_blocks> run_pass(shared_ptr<basic_blocks> bbs) override;
		};
	}
}

#endif
