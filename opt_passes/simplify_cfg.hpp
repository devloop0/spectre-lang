#ifndef SPECTRE_OPT_SIMPLIFY_CFG_HPP
#define SPECTRE_OPT_SIMPLIFY_CFG_HPP

#include "basic_block.hpp"
#include "middle_ir.hpp"
#include "pass.hpp"
#include "pass_manager.hpp"

#include <unordered_set>
#include <string>

using namespace spectre::ir;
using std::unordered_set;
using std::string;

namespace spectre {
	namespace opt {

		void simplify_cfg(shared_ptr<basic_blocks> bbs);

		class simplify_cfg_pass : public pass {
		public:
			simplify_cfg_pass(shared_ptr<pass_manager> pm);
			simplify_cfg_pass(shared_ptr<pass_manager> pm, int counter, unordered_set<string> deps);
			~simplify_cfg_pass();
			shared_ptr<basic_blocks> run_pass(shared_ptr<basic_blocks> bbs) override;
		};
	}
}

#endif
