#ifndef SPECTRE_OPT_CVT_FROM_SSA_HPP
#define SPECTRE_OPT_CVT_FROM_SSA_HPP

#include "basic_block.hpp"
#include "middle_ir.hpp"
#include "pass.hpp"
#include "pass_manager.hpp"

#include <unordered_set>
#include <string>
#include <memory>

using std::unordered_set;
using std::string;
using std::shared_ptr;

namespace spectre {
	namespace opt {

		void backpropagate_phi_insns(shared_ptr<basic_blocks> bbs);

		class cvt_from_ssa_pass : public pass {
		public:
			cvt_from_ssa_pass(shared_ptr<pass_manager> pm);
			cvt_from_ssa_pass(shared_ptr<pass_manager> pm, int counter, unordered_set<string> deps);
			~cvt_from_ssa_pass();
			shared_ptr<basic_blocks> run_pass(shared_ptr<basic_blocks> bbs) override;
		};
	}
}

#endif
