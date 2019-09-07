#include "basic_dce.hpp"
#include "cvt2ssa.hpp"
#include "mem2reg.hpp"
#include "insns_in_bb.hpp"

#include <algorithm>
#include <memory>
#include <iterator>
#include <functional>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

using std::to_string;
using std::static_pointer_cast;
using std::copy_if;
using std::copy;
using std::inserter;
using std::back_inserter;
using std::sort;
using std::greater;
using std::make_pair;
using std::function;
using std::copy;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::unique;

namespace spectre {
	namespace opt {

		pair<unordered_set<int>, unordered_set<int>> function_and_global_headers(shared_ptr<basic_blocks> bbs) {
			vector<shared_ptr<function_insn>> fis = bbs->get_middle_ir()->function_insn_list();
			unordered_map<int, unordered_set<int>> preds = get_predecessors(bbs->cfg());
			unordered_set<int> no_preds;
			for (const auto& [k, v] : preds) {
				if (v.empty())
					no_preds.insert(k);
			}
			unordered_set<string> us;
			for (shared_ptr<function_insn> fi : fis)
				us.insert(c_symbol_2_string(bbs->get_middle_ir(), fi->func_symbol()));
			unordered_set<int> functions, globals;
			for (const auto& j : no_preds) {
				shared_ptr<basic_block> bb = bbs->get_basic_block(j);
				vector<shared_ptr<insn>> insns = bb->get_insns(bbs);
				if (insns.empty())
					continue;
				shared_ptr<insn> i = insns[0];
				if (i->insn_kind() != insn::kind::KIND_LABEL)
					continue;
				shared_ptr<label_insn> li = static_pointer_cast<label_insn>(i);
				string lab_text = li->label()->label_text();
				if (us.find(lab_text) != us.end())
					functions.insert(j);
				else if (lab_text == global_header)
					globals.insert(j);
			}
			return make_pair(functions, globals);
		}

		void remove_unused_registers(shared_ptr<basic_blocks> bbs) {
			unordered_map<int, pair<int, int>> dst_reg_2_pos, var_alloc_2_pos;
			unordered_set<int> all_regs, regs_read_from;
			auto wrapped_handle_rhs = [&all_regs, &regs_read_from] (shared_ptr<operand> o, int bb_index, int insn_index) {
				if (o->operand_kind() != operand::kind::KIND_REGISTER)
					return;
				shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(o);
				all_regs.insert(ro->virtual_register_number());
				regs_read_from.insert(ro->virtual_register_number());
			};

			auto wrapped_handle_lhs = [&dst_reg_2_pos, &all_regs, &wrapped_handle_rhs] (shared_ptr<operand> o, int bb_index, int insn_index) {
				if (o->operand_kind() != operand::kind::KIND_REGISTER)
					return;
				shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(o);
				all_regs.insert(ro->virtual_register_number());
				if (ro->dereference()) {
					wrapped_handle_rhs(ro, bb_index, insn_index);
					return;
				}
				dst_reg_2_pos[ro->virtual_register_number()] = make_pair(bb_index, insn_index);
			};

			auto visit_insn = [&bbs, &wrapped_handle_lhs, &wrapped_handle_rhs, &all_regs, &var_alloc_2_pos] (shared_ptr<insn> i, int bb_index, int insn_index) {
				switch (i->insn_kind()) {
				case insn::kind::KIND_ACCESS: {
					shared_ptr<access_insn> ai = static_pointer_cast<access_insn>(i);
					wrapped_handle_lhs(ai->variable(), bb_index, insn_index);
				}
					break;
				case insn::kind::KIND_ALIGN:
					break;
				case insn::kind::KIND_ASM: {
					shared_ptr<asm_insn> ai = static_pointer_cast<asm_insn>(i);
					if (ai->asm_insn_kind() == asm_insn::kind::KIND_LA)
						wrapped_handle_rhs(ai->la().second, bb_index, insn_index);
				}
					break;
				case insn::kind::KIND_BINARY: {
					shared_ptr<binary_insn> bi = static_pointer_cast<binary_insn>(i);
					wrapped_handle_rhs(bi->src1_operand(), bb_index, insn_index);
					wrapped_handle_rhs(bi->src2_operand(), bb_index, insn_index);
					wrapped_handle_lhs(bi->dst_operand(), bb_index, insn_index);
				}
					break;
				case insn::kind::KIND_CALL: {
					shared_ptr<call_insn> ci = static_pointer_cast<call_insn>(i);
					wrapped_handle_rhs(ci->function_operand(), bb_index, insn_index);
					for (auto& rhs : ci->argument_list())
						wrapped_handle_rhs(rhs, bb_index, insn_index);
					wrapped_handle_rhs(ci->dest_operand(), bb_index, insn_index);
				}
					break;
				case insn::kind::KIND_CONDITIONAL: {
					shared_ptr<conditional_insn> ci = static_pointer_cast<conditional_insn>(i);
					wrapped_handle_rhs(ci->src(), bb_index, insn_index);
				}
					break;
				case insn::kind::KIND_EXT: {
					shared_ptr<ext_insn> ei = static_pointer_cast<ext_insn>(i);
					wrapped_handle_rhs(ei->rhs(), bb_index, insn_index);
					wrapped_handle_lhs(ei->lhs(), bb_index, insn_index);
				}
					break;
				case insn::kind::KIND_FUNCTION:
					bbs->report_internal("This should be unreacahable.", __FUNCTION__, __LINE__, __FILE__);
					break;
				case insn::kind::KIND_JUMP:
				case insn::kind::KIND_LABEL:
					break;
				case insn::kind::KIND_MEMCPY: {
					shared_ptr<memcpy_insn> mi = static_pointer_cast<memcpy_insn>(i);
					wrapped_handle_rhs(mi->source(), bb_index, insn_index);
					wrapped_handle_rhs(mi->destination(), bb_index, insn_index);
				}
					break;
				case insn::kind::KIND_PHI: {
					shared_ptr<phi_insn> pi = static_pointer_cast<phi_insn>(i);
					for (shared_ptr<phi_insn::pred_data> pd : pi->pred_data_list()) {
						if (pd->reg_pred == nullptr)
							continue;
						wrapped_handle_rhs(pd->reg_pred, bb_index, insn_index);
					}
					wrapped_handle_lhs(pi->dst(), bb_index, insn_index);
				}
					break;
				case insn::kind::KIND_RETURN: {
					shared_ptr<return_insn> ri = static_pointer_cast<return_insn>(i);
					wrapped_handle_rhs(ri->expr(), bb_index, insn_index);
				}
					break;
				case insn::kind::KIND_TRUNC: {
					shared_ptr<trunc_insn> ti = static_pointer_cast<trunc_insn>(i);
					wrapped_handle_rhs(ti->rhs(), bb_index, insn_index);
					wrapped_handle_lhs(ti->lhs(), bb_index, insn_index);
				}
					break;
				case insn::kind::KIND_UNARY: {
					shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
					wrapped_handle_rhs(ui->src_operand(), bb_index, insn_index);
					if (ui->unary_expr_kind() == unary_insn::kind::KIND_STK ||
						ui->unary_expr_kind() == unary_insn::kind::KIND_RESV) {
						shared_ptr<operand> d = ui->dst_operand();
						if (d->operand_kind() == operand::kind::KIND_REGISTER) {
							shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(d);
							all_regs.insert(ro->virtual_register_number());
							if (!ro->dereference() && ui->unary_expr_kind() == unary_insn::kind::KIND_STK)
								var_alloc_2_pos[ro->virtual_register_number()] = make_pair(bb_index, insn_index);
							else
								wrapped_handle_rhs(ro, bb_index, insn_index);
						}
					}
					else
						wrapped_handle_lhs(ui->dst_operand(), bb_index, insn_index);
				}
					break;
				case insn::kind::KIND_VAR: {
					shared_ptr<var_insn> vi = static_pointer_cast<var_insn>(i);
					wrapped_handle_rhs(vi->size(), bb_index, insn_index);
					if (vi->dst()->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(vi->dst());
						all_regs.insert(ro->virtual_register_number());
						if (!ro->dereference())
							var_alloc_2_pos[ro->virtual_register_number()] = make_pair(bb_index, insn_index);
						else
							wrapped_handle_rhs(ro, bb_index, insn_index);
					}
				}
					break;
				}
			};

			bool changed = true;
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
			vector<int> will_be_removed;
			while (changed) {
				changed = false;

				int bb_counter = 0;
				for (shared_ptr<basic_block> bb : bbs->get_basic_blocks()) {
					int insn_counter = 0;
					for (shared_ptr<insn> i : bb->get_insns(bbs)) {
						visit_insn(i, bb_counter, insn_counter);
						insn_counter++;
					}
					bb_counter++;
				}

				unordered_set<int> unread_registers;
				copy_if(all_regs.begin(), all_regs.end(), inserter(unread_registers, unread_registers.end()),
					[&regs_read_from] (const auto& n) { return regs_read_from.find(n) == regs_read_from.end(); });
				unordered_map<int, vector<int>> to_remove;
				for (const auto& reg : unread_registers) {
					if (dst_reg_2_pos.find(reg) != dst_reg_2_pos.end()) {
						pair<int, int> write_index = dst_reg_2_pos[reg];
						to_remove[write_index.first].push_back(write_index.second);
						changed = true;
					}
					if (var_alloc_2_pos.find(reg) != var_alloc_2_pos.end()) {
						pair<int, int> alloc_index = var_alloc_2_pos[reg];
						to_remove[alloc_index.first].push_back(alloc_index.second);
						shared_ptr<basic_block> bb = bbs->get_basic_block(alloc_index.first);
						if (alloc_index.second != 0
							&& bb->get_insns(bbs)[alloc_index.second - 1]->insn_kind() == insn::kind::KIND_ALIGN)
							to_remove[alloc_index.first].push_back(alloc_index.second - 1);
						changed = true;
					}
				}
				for (auto& [k, v] : to_remove)
					sort(v.begin(), v.end(), greater{});

				bb_counter = 0;
				for (shared_ptr<basic_block> bb : bbs->get_basic_blocks()) {
					for (const auto& index : to_remove[bb_counter]) {
						bb->remove_insn(index);
						if (bb->get_insns(bbs).empty())
							will_be_removed.push_back(bb_counter);
					}
					bb_counter++;
				}

				all_regs.clear(), regs_read_from.clear();
				dst_reg_2_pos.clear(), var_alloc_2_pos.clear();
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

		void remove_unreachable_bbs(shared_ptr<basic_blocks> bbs) {
			pair<unordered_set<int>, unordered_set<int>> funcs_and_glob = function_and_global_headers(bbs);
			unordered_set<int> funcs = funcs_and_glob.first, glob = funcs_and_glob.second;

			unordered_set<int> visited, all = bbs->cfg().vertices(), unreachable_set;
			vector<int> unreachable;
			function<void(int)> reachable = [&visited, &bbs, &reachable] (int j) {
				if (visited.find(j) != visited.end())
					return;

				visited.insert(j);
				unordered_set<int> adj = bbs->cfg().adjacent(j);
				for (const auto& v : adj)
					reachable(v);
			};

			for (const auto& v : funcs)
				reachable(v);
			for (const auto& v : glob)
				reachable(v);

			copy_if(all.begin(), all.end(), back_inserter(unreachable),
				[&visited] (const auto& a) { return visited.find(a) == visited.end(); });
			if (unreachable.empty())
				return;
			sort(unreachable.begin(), unreachable.end(), greater{});
			copy(unreachable.begin(), unreachable.end(), inserter(unreachable_set,
				unreachable_set.end()));

			unordered_map<int, vector<shared_ptr<phi_insn::pred_data>>> phi_preds;
			for (const auto& v : bbs->cfg().vertices()) {
				shared_ptr<basic_block> bb = bbs->get_basic_block(v);
				if (bb == nullptr)
					continue;
				for (shared_ptr<insn> i : bb->get_insns(bbs)) {
					if (i->insn_kind() == insn::kind::KIND_PHI) {
						shared_ptr<phi_insn> pi = static_pointer_cast<phi_insn>(i);
						vector<int> to_remove;
						for (int index = 0; index < pi->pred_data_list().size(); index++) {
							shared_ptr<phi_insn::pred_data> pd = pi->pred_data_list()[index];
							if (unreachable_set.find(pd->bb_pred) != unreachable_set.end())
								to_remove.push_back(index);
							else
								phi_preds[pd->bb_pred].push_back(pd);
						}
						sort(to_remove.begin(), to_remove.end(), greater{});
						for (const auto& r : to_remove)
							pi->remove_pred(r);
					}
				}
			}
			for (const auto& u : unreachable) {
				bbs->cfg().remove_vertex(u);
				bbs->remove_basic_block(u);
				for (auto& [bb_index, pds] : phi_preds) {
					if (bb_index > u) {
						for (auto& pd : pds)
							pd->bb_pred--;
					}
				}
			}
		}

		shared_ptr<basic_blocks> insns_in_bb_2_straight_line(shared_ptr<basic_blocks> bbs) {
			vector<shared_ptr<insn>> all_insns;
			for (shared_ptr<basic_block> bb : bbs->get_basic_blocks()) {
				vector<shared_ptr<insn>> temp = bb->get_insns(bbs);
				all_insns.insert(all_insns.end(), temp.begin(), temp.end());
				bb->clear_insn_list();
				bb->set_insns_contained(false);
			}
			bbs->get_middle_ir()->clear_global_insn_list();
			bbs->get_middle_ir()->set_insn_list(all_insns);
			return generate_cfg(bbs->get_middle_ir());
		}

		basic_dce_pass::basic_dce_pass(shared_ptr<pass_manager> pm) : pass("basic_dce", "Removes trivially unused registers",
			pm->next_pass_id(), unordered_set<string>{}) {

		}

		basic_dce_pass::basic_dce_pass(shared_ptr<pass_manager> pm, int counter, unordered_set<string> deps) :
			pass("basic_dce" + to_string(counter), "Removes trivially unused registers", pm->next_pass_id(),
					(deps.insert("insns_in_bb"), deps)), _debug_ctx(pm->debug()) {

		}

		basic_dce_pass::~basic_dce_pass() {

		}

		shared_ptr<basic_blocks> basic_dce_pass::run_pass(shared_ptr<basic_blocks> bbs) {
			remove_unused_registers(bbs);
			bbs = insns_in_bb_2_straight_line(bbs);
			clean_up_phi_insns(bbs);
			place_insns_in_bb(bbs);

			remove_unreachable_bbs(bbs);
			bbs = insns_in_bb_2_straight_line(bbs);
			clean_up_phi_insns(bbs);
			if (_debug_ctx >= pass_manager::debug_level::KIND_DEBUG)
				ssa_sanity_check(bbs);
			place_insns_in_bb(bbs);
			return bbs;
		}
	}
}
