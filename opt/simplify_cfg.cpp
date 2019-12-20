#include "opt/simplify_cfg.hpp"
#include "opt/basic_dce.hpp"
#include "opt/insns_in_bb.hpp"
#include "opt/cvt2ssa.hpp"

#include <algorithm>
#include <unordered_set>
#include <string>
#include <memory>
#include <vector>

using std::to_string;
using std::pair;
using std::unordered_set;
using std::string;
using std::static_pointer_cast;
using std::vector;
using std::unique;
using std::sort;
using std::greater;

namespace spectre {
	namespace opt {

		void simplify_cfg(shared_ptr<basic_blocks> bbs) {
			unordered_map<string, int> label_to_bb;
			unordered_map<int, string> bb_to_j, bb_to_label, bb_to_cond;
			unordered_map<string, int> branch_count;
			pair<unordered_set<int>, unordered_set<int>> funcs_and_glob = function_and_global_headers(bbs);
			unordered_set<int> funcs = funcs_and_glob.first, globs = funcs_and_glob.second;

			for (int i = 0; i < bbs->get_basic_blocks().size(); i++) {
				shared_ptr<basic_block> bb = bbs->get_basic_block(i);
				vector<shared_ptr<insn>> bb_insns = bb->get_insns(bbs);
				for (shared_ptr<insn> i : bb_insns) {
					if (i->insn_kind() == insn::kind::KIND_JUMP) {
						shared_ptr<jump_insn> ji = static_pointer_cast<jump_insn>(i);
						branch_count[ji->label()->label_text()]++;
					}
					else if (i->insn_kind() == insn::kind::KIND_CONDITIONAL) {
						shared_ptr<conditional_insn> ci = static_pointer_cast<conditional_insn>(i);
						branch_count[ci->branch()->label_text()]++;
					}
				}
				if (bb_insns.empty())
					continue;
				shared_ptr<insn> first_insn = *bb_insns.begin();
				if (first_insn->insn_kind() == insn::kind::KIND_LABEL) {
					shared_ptr<label_insn> lab_insn = static_pointer_cast<label_insn>(first_insn);
					label_to_bb[lab_insn->label()->label_text()] = i;
					bb_to_label[i] = lab_insn->label()->label_text();
				}
				shared_ptr<insn> last_insn = *(bb_insns.end() - 1);
				if (last_insn->insn_kind() == insn::kind::KIND_JUMP) {
					shared_ptr<jump_insn> j_insn = static_pointer_cast<jump_insn>(last_insn);
					shared_ptr<operand> j_lab = j_insn->label();
					if (j_lab->operand_kind() == operand::kind::KIND_LABEL) {
						shared_ptr<label_operand> j_lab_op = static_pointer_cast<label_operand>(j_lab);
						bb_to_j[i] = j_lab_op->label_text();
					}
				}
				else if (last_insn->insn_kind() == insn::kind::KIND_CONDITIONAL) {
					shared_ptr<conditional_insn> ci = static_pointer_cast<conditional_insn>(last_insn);
					bb_to_cond[i] = ci->branch()->label_text();
				}
			}

			bool changed = true;
			vector<int> will_be_removed;
			unordered_map<int, vector<shared_ptr<phi_insn::pred_data>>> phi_preds;
			vector<shared_ptr<phi_insn>> phi_insn_list;
			for (shared_ptr<basic_block> bb : bbs->get_basic_blocks()) {
				for (shared_ptr<insn> i : bb->get_insns(bbs)) {
					if (i->insn_kind() == insn::kind::KIND_PHI) {
						shared_ptr<phi_insn> pi = static_pointer_cast<phi_insn>(i);
						phi_insn_list.push_back(pi);
						for (shared_ptr<phi_insn::pred_data> pd : pi->pred_data_list())
							phi_preds[pd->bb_pred].push_back(pd);
					}
				}
			}
			while (changed) {
				changed = false;
				unordered_map<int, unordered_set<int>> preds = get_predecessors(bbs->cfg());
				for (int i = 0; i < bbs->get_basic_blocks().size(); i++) {
					shared_ptr<basic_block> bb = bbs->get_basic_block(i);
					if (bb->get_insns(bbs).empty())
						continue;
					unordered_set<int> adjs = bbs->cfg().adjacent(i);
					if (adjs.size() == 1) {
						int next = *adjs.begin();
						if (bb_to_j.find(i) != bb_to_j.end() || bb_to_cond.find(i) != bb_to_cond.end()) {
							string to_lab;
							if (bb_to_j.find(i) != bb_to_j.end())
						       		to_lab = bb_to_j[i];
							else
								to_lab = bb_to_cond[i];
							if (label_to_bb.find(to_lab) == label_to_bb.end())
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							if (label_to_bb[to_lab] == i + 1) {
								bb->remove_insn(bb->insn_list().size() - 1);
								if (bb_to_j.find(i) != bb_to_j.end())
									bb_to_j.erase(i);
								else
									bb_to_cond.erase(i);
								branch_count[to_lab]--;
								changed = true;
								if (bb->get_insns(bbs).empty())
									will_be_removed.push_back(i);
								else {
									shared_ptr<basic_block> bb_next = bbs->get_basic_block(i + 1);
									if (bb_next->get_insns(bbs).empty()
										|| bb_next->get_insns(bbs)[0]->insn_kind() != insn::kind::KIND_LABEL)
									will_be_removed.push_back(i + 1);
								}
							}
						}
					}
				}
				unordered_map<string, vector<shared_ptr<insn>>> lab_to_branches;
				for (shared_ptr<basic_block> bb : bbs->get_basic_blocks()) {
					for (shared_ptr<insn> i : bb->get_insns(bbs)) {
						if (i->insn_kind() == insn::kind::KIND_JUMP) {
							shared_ptr<jump_insn> ji = static_pointer_cast<jump_insn>(i);
							lab_to_branches[ji->label()->label_text()].push_back(ji);
						}
						else if (i->insn_kind() == insn::kind::KIND_CONDITIONAL) {
							shared_ptr<conditional_insn> ci = static_pointer_cast<conditional_insn>(i);
							lab_to_branches[ci->branch()->label_text()].push_back(ci);
						}
					}
				}
				for (int i = 0; i < bbs->get_basic_blocks().size(); i++) {
					shared_ptr<basic_block> bb = bbs->get_basic_block(i);
					if (bb->get_insns(bbs).empty())
						continue;
					unordered_set<int> pred = preds[i];
					if (pred.size() == 1 && bb_to_label.find(i) != bb_to_label.end()
						&& branch_count[bb_to_label[i]] <= 0
						&& phi_preds[i].empty()) {
						bb->remove_insn(0);
						label_to_bb.erase(bb_to_label[i]);
						bb_to_label.erase(i);
						changed = true;
						if (bb->get_insns(bbs).empty())
							will_be_removed.push_back(i);
						else if (i != 0) {
							shared_ptr<basic_block> bb = bbs->get_basic_block(i - 1);
							if (bb->get_insns(bbs).empty())
								will_be_removed.push_back(i);
							else {
								shared_ptr<insn> last = *(bb->get_insns(bbs).end() - 1);
								if (last->insn_kind() != insn::kind::KIND_CALL
									&& last->insn_kind() != insn::kind::KIND_CONDITIONAL
									&& last->insn_kind() != insn::kind::KIND_RETURN
									&& last->insn_kind() != insn::kind::KIND_JUMP)
									will_be_removed.push_back(i);
							}
						}
					}
				}
				for (int i = 0; i < bbs->get_basic_blocks().size(); i++) {
					shared_ptr<basic_block> bb = bbs->get_basic_block(i);
					if (bb->get_insns(bbs).size() != 1 || i == bbs->get_basic_blocks().size() - 1
						|| !phi_preds[i].empty() || funcs.find(i) != funcs.end() || globs.find(i) != globs.end())
						continue;
					shared_ptr<basic_block> bb_next = bbs->get_basic_block(i + 1);
					unordered_set<int> adjs = bbs->cfg().adjacent(i);
					if (bb_next->get_insns(bbs).empty() || adjs.size() != 1 || *adjs.begin() != i + 1)
						continue;
					shared_ptr<insn> only = bb->get_insns(bbs)[0];
					shared_ptr<insn> first_next = bb_next->get_insns(bbs)[0];
					if (only->insn_kind() != insn::kind::KIND_LABEL || first_next->insn_kind() != insn::kind::KIND_LABEL)
						continue;
					shared_ptr<label_insn> curr_li = static_pointer_cast<label_insn>(only),
						next_li = static_pointer_cast<label_insn>(first_next);
					string old_lab = curr_li->label()->label_text(),
					       next_lab = next_li->label()->label_text();
					vector<shared_ptr<insn>> curr_lab_branches = lab_to_branches[old_lab];
					for (shared_ptr<insn> i : curr_lab_branches) {
						if (i->insn_kind() == insn::kind::KIND_JUMP)
							static_pointer_cast<jump_insn>(i)->set_label(make_shared<label_operand>(next_lab, void_type));
						else if (i->insn_kind() == insn::kind::KIND_CONDITIONAL)
							static_pointer_cast<conditional_insn>(i)->set_branch(make_shared<label_operand>(next_lab, void_type));
					}
					lab_to_branches.erase(old_lab);
					lab_to_branches[next_lab].insert(lab_to_branches[next_lab].end(), curr_lab_branches.begin(), curr_lab_branches.end());
					changed = true;
					bb->remove_insn(0);
					if (bb->get_insns(bbs).empty())
						will_be_removed.push_back(i);
				}
			}
			sort(will_be_removed.begin(), will_be_removed.end(), greater{});
			unique(will_be_removed.begin(), will_be_removed.end());
			unordered_set<int> will_be_removed_set;
			copy(will_be_removed.begin(), will_be_removed.end(), inserter(will_be_removed_set,
						will_be_removed_set.end()));
			for (auto& pi : phi_insn_list) {
				vector<int> removed_bbs;
				for (int i = 0; i < pi->pred_data_list().size(); i++) {
					if (will_be_removed_set.find(pi->pred_data_list()[i]->bb_pred) != will_be_removed_set.end())
						removed_bbs.push_back(i);
				}
				reverse(removed_bbs.begin(), removed_bbs.end());
				for (const auto& r : removed_bbs)
					pi->remove_pred(r);
			}
			for (const auto& u : will_be_removed) {
				for (auto& [bb_index, pds] : phi_preds) {
					if (bb_index > u) {
						for (auto& pd : pds)
							pd->bb_pred--;
					}
				}
			}
		}

		simplify_cfg_pass::simplify_cfg_pass(shared_ptr<pass_manager> pm) :
			pass("simplify_cfg", "Removes unnecessarily complex control flow in the CFG.", pm->next_pass_id(),
					{"insns_in_bb"}) {

		}

		simplify_cfg_pass::simplify_cfg_pass(shared_ptr<pass_manager> pm, int counter, unordered_set<string> deps) :
			pass("simplify_cfg" + to_string(counter), "Removes unnecessarily complex control flow in the CFG.", pm->next_pass_id(),
				(deps.insert("insns_in_bb"), deps)) {
		}

		simplify_cfg_pass::~simplify_cfg_pass() {

		}

		shared_ptr<basic_blocks> simplify_cfg_pass::run_pass(shared_ptr<basic_blocks> bbs) {
			simplify_cfg(bbs);
			bbs = insns_in_bb_2_straight_line(bbs);
			place_insns_in_bb(bbs);
			return bbs;
		}
	}
}
