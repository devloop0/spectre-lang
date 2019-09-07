#ifndef SPECTRE_OPT_INSNS_IN_BB_HPP
#define SPECTRE_OPT_INSNS_IN_BB_HPP

#include "pass_manager.hpp"
#include "pass.hpp"
#include "basic_block.hpp"
#include "middle_ir.hpp"

using namespace spectre::ir;

namespace spectre {
	namespace opt {

		void place_insns_in_bb(shared_ptr<basic_blocks> bbs);

		class insns_in_bb_pass : public pass {
		public:
			insns_in_bb_pass(shared_ptr<pass_manager> pm);
			~insns_in_bb_pass();
			shared_ptr<basic_blocks> run_pass(shared_ptr<basic_blocks> bbs) override;
		};
	}
}

#endif
