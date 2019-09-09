#include "opt/insns_in_bb.hpp"
#include <unordered_set>

using std::unordered_set;

namespace spectre {
	namespace opt {

		void place_insns_in_bb(shared_ptr<basic_blocks> bbs) {
			for (int j = 0; j < bbs->get_basic_blocks().size(); j++) {
				shared_ptr<basic_block> bb = bbs->get_basic_block(j);
				vector<shared_ptr<insn>> il = bb->get_insns(bbs);
				shared_ptr<basic_block> new_bb = make_shared<basic_block>(bb->basic_block_kind(), il);
				new_bb->set_start(bb->start());
				new_bb->set_end(bb->end());
				bbs->set_basic_block(j, new_bb);
			}
			bbs->clear_insn_list();
		}

		insns_in_bb_pass::insns_in_bb_pass(shared_ptr<pass_manager> pm) : pass("insns_in_bb", "Places instructions inside basic blocks.",
			pm->next_pass_id(), unordered_set<string>{ "cvt2ssa" }) {

		}

		insns_in_bb_pass::~insns_in_bb_pass() {

		}

		shared_ptr<basic_blocks> insns_in_bb_pass::run_pass(shared_ptr<basic_blocks> bbs) {
			place_insns_in_bb(bbs);
			return bbs;
		}
	}
}
