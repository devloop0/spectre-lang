#include <iostream>
#include <memory>
#include <algorithm>
#include "opt/cvt2ssa.hpp"
#include "opt/mem2reg.hpp"

using std::static_pointer_cast;
using std::cout;
using std::endl;
using std::sort;
using std::greater;
using std::make_pair;
using std::next;

namespace spectre {
	namespace ir {
		phi_insn::phi_insn(shared_ptr<register_operand> d, vector<shared_ptr<pred_data>> pdl) : insn(insn::kind::KIND_PHI), _pred_data_list(pdl), _dst(d) {

		}

		phi_insn::phi_insn(shared_ptr<register_operand> d) : insn(insn::kind::KIND_PHI), _pred_data_list(vector<shared_ptr<pred_data>>{}), _dst(d) {

		}

		phi_insn::~phi_insn() {

		}

		vector<shared_ptr<phi_insn::pred_data>> phi_insn::pred_data_list() {
			return _pred_data_list;
		}

		void phi_insn::add_pred(shared_ptr<pred_data> pd) {
			_pred_data_list.push_back(pd);
		}

		shared_ptr<phi_insn::pred_data> phi_insn::get_pred(int j) {
			if (j < 0 || j >= _pred_data_list.size())
				return nullptr;
			return _pred_data_list[j];
		}

		void phi_insn::set_pred(int j, shared_ptr<pred_data> pd) {
			if (j < 0 || j >= _pred_data_list.size())
				return;
			_pred_data_list[j] = pd;
		}

		int phi_insn::num_preds() {
			return _pred_data_list.size();
		}

		shared_ptr<register_operand> phi_insn::dst() {
			return _dst;
		}

		void phi_insn::remove_pred(int j) {
			if (j < 0 || j >= _pred_data_list.size())
				return;
			_pred_data_list.erase(_pred_data_list.begin() + j);
		}

		string phi_insn::to_string() {
			string ret = "phi " + _dst->to_string();
			if (_pred_data_list.empty())
				return ret;
			for (shared_ptr<pred_data> pd : _pred_data_list) {
				ret += ", [ ";
				if (pd->reg_pred != nullptr)
					ret += pd->reg_pred->to_string();
				else
					ret += "<>";
				if (pd->bb_pred != -1)
					ret += ", #" + std::to_string(pd->bb_pred);
				else
					ret += ", <>";
				ret += " ]";
			}
			return ret;
		}

		void phi_insn::deduplicate() {
			DEDUP(_dst);
			for (auto& pd : _pred_data_list)
				DEDUP(pd->reg_pred);
		}
	}

	namespace opt {
		unordered_map<int, unordered_set<int>> compute_dominators(shared_ptr<basic_blocks> bbs, const directed_graph<int>& d) {
			unordered_map<int, unordered_set<int>> preds = get_predecessors(d);
			unordered_set<int> vertices = d.vertices(), vertices_without_starts = vertices;
			unordered_map<int, unordered_set<int>> ret, temp;
			for (const auto& v : vertices) {
				if (preds[v].empty())
					ret[v] = unordered_set<int>({ v });
				else
					ret[v] = vertices;
			}
			for (const auto& v : vertices) {
				if (preds[v].empty())
					vertices_without_starts.erase(v);
			}
			bool first = true;
			while (ret != temp || first) {
				temp = ret;
				for (const auto& n : vertices_without_starts) {
					vector<unordered_set<int>> pred_doms;
					unordered_set<int> new_doms;
					for (const auto& p : preds[n])
						pred_doms.push_back(ret[p]);
					for (const auto& v : vertices) {
						bool in_all = true;
						for (const auto& p : pred_doms) {
							if (p.find(v) == p.end()) {
								in_all = false;
								break;
							}
						}
						if (in_all)
							new_doms.insert(v);
					}
					new_doms.insert(n);
					ret[n] = new_doms;
				}
				first = false;
			}
			return ret;
		}

		unordered_map<int, unordered_set<int>> compute_strict_dominators(shared_ptr<basic_blocks> bbs, const unordered_map<int, unordered_set<int>>& doms) {
			unordered_map<int, unordered_set<int>> ret = doms;
			for (auto&[v, v_doms] : ret) {
				if (v_doms.find(v) != v_doms.end())
					v_doms.erase(v);
			}
			return ret;
		}

		unordered_map<int, int> compute_immediate_dominators(shared_ptr<basic_blocks> bbs, const unordered_map<int, unordered_set<int>>& doms) {
			unordered_map<int, unordered_set<int>> strict_doms = compute_strict_dominators(bbs, doms);
			unordered_map<int, int> idoms;
			for (const auto&[v, v_doms] : strict_doms) {
				bool hit = false;
				int idom;
				for (const auto& idom_cand : v_doms) {
					bool dom_by_all = true;
					for (const auto& p : v_doms) {
						if (doms.at(idom_cand).find(p) == doms.at(idom_cand).end()) {
							dom_by_all = false;
							break;
						}
					}
					if (dom_by_all) {
						if (hit)
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						idom = idom_cand;
						hit = true;
					}
				}
				if (!hit && !v_doms.empty())
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				if (v_doms.empty())
					idoms[v] = -1;
				else
					idoms[v] = idom;
			}
			return idoms;
		}

		unordered_map<int, unordered_set<int>> compute_dominance_frontier(shared_ptr<basic_blocks> bbs, const directed_graph<int>& d,
			const unordered_map<int, unordered_set<int>>& doms) {
			unordered_set<int> verts = d.vertices();
			unordered_map<int, unordered_set<int>> preds = get_predecessors(d);
			unordered_map<int, int> idoms = compute_immediate_dominators(bbs, doms);
			unordered_map<int, unordered_set<int>> ret;
			for (const auto& b : verts)
				ret[b] = unordered_set<int>{};
			for (const auto& b : verts) {
				if (preds[b].size() >= 2) {
					for (const auto& p : preds[b]) {
						int runner = p;
						while (runner != idoms[b]) {
							ret[runner].insert(b);
							runner = idoms[runner];
						}
					}
				}
			}
			return ret;
		}

		unordered_map<int, pair<shared_ptr<register_operand>, vector<int>>> all_register_assignment_indices(shared_ptr<basic_blocks> bbs) {
			unordered_map<int, pair<shared_ptr<register_operand>, vector<int>>> ret;
			for (int reg = 0; reg < bbs->original_num_allocated_registers(); reg++)
				ret[reg] = make_pair(nullptr, vector<int>{});
			for (int _ = 0; _ < bbs->num_insns(); _++) {
				shared_ptr<insn> i = bbs->get_insn(_);
#define CHECK_ASSIGNMENT(TYPE, DST_FUNC) do { shared_ptr<TYPE> ti = static_pointer_cast<TYPE>(i); \
	if (ti->DST_FUNC()->operand_kind() == operand::kind::KIND_REGISTER) { \
		shared_ptr<register_operand> tr = static_pointer_cast<register_operand>(ti->DST_FUNC()); \
		if (!tr->dereference()) { \
			ret[tr->virtual_register_number()].first = tr; \
			ret[tr->virtual_register_number()].second.push_back(_); \
		} \
	} \
} while(0)
				switch (i->insn_kind()) {
				case insn::kind::KIND_ACCESS:
				case insn::kind::KIND_ALIGN:
					break;
				case insn::kind::KIND_ASM:
					break;
				case insn::kind::KIND_BINARY:
					CHECK_ASSIGNMENT(binary_insn, dst_operand);
					break;
				case insn::kind::KIND_CALL:
					CHECK_ASSIGNMENT(call_insn, dest_operand);
					break;
				case insn::kind::KIND_CONDITIONAL:
					break;
				case insn::kind::KIND_EXT:
					CHECK_ASSIGNMENT(ext_insn, lhs);
					break;
				case insn::kind::KIND_FUNCTION:
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					break;
				case insn::kind::KIND_JUMP:
				case insn::kind::KIND_LABEL:
					break;
				case insn::kind::KIND_MEMCPY:
					break;
				case insn::kind::KIND_PHI:
					CHECK_ASSIGNMENT(phi_insn, dst);
					break;
				case insn::kind::KIND_RETURN:
					break;
				case insn::kind::KIND_TRUNC:
					CHECK_ASSIGNMENT(trunc_insn, lhs);
					break;
				case insn::kind::KIND_UNARY:
					CHECK_ASSIGNMENT(unary_insn, dst_operand);
					break;
				case insn::kind::KIND_VAR:
					CHECK_ASSIGNMENT(var_insn, dst);
					break;
				default:
					break;
#undef CHECK_ASSIGNMENT
				}
			}
			return ret;
		}

		void insert_phi_insns(shared_ptr<basic_blocks> bbs) {
			unordered_map<int, unordered_set<int>> preds = get_predecessors(bbs->cfg());
			unordered_map<int, unordered_set<int>> doms = compute_dominators(bbs, bbs->cfg());
			unordered_map<int, unordered_set<int>> dom_frons = compute_dominance_frontier(bbs, bbs->cfg(), doms);
			
			for (int reg = 0; reg < bbs->original_num_allocated_registers(); reg++) {
				unordered_set<int> work_list, ever_on_work_list, already_has_phi_func;
				vector<int> assignment_insn_bbs;
				auto [reg_op, insns_with_assignments] = all_register_assignment_indices(bbs)[reg];
				for (int insn_index = 0, bb_index = 0; bb_index < bbs->get_basic_blocks().size() && insn_index < insns_with_assignments.size();) {
					if (insns_with_assignments[insn_index] < bbs->get_basic_block(bb_index)->start())
						insn_index++;
					else if (bbs->get_basic_block(bb_index)->start() <= insns_with_assignments[insn_index] &&
						insns_with_assignments[insn_index] < bbs->get_basic_block(bb_index)->end()) {
						assignment_insn_bbs.push_back(bb_index);
						insn_index++;
					}
					else
						bb_index++;
				}
				if (insns_with_assignments.size() != assignment_insn_bbs.size())
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				for (const auto& bb : assignment_insn_bbs)
					work_list.insert(bb);
				ever_on_work_list = work_list;
				if (reg_op == nullptr && !work_list.empty())
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				unordered_map<int, unordered_set<int>> to_insert;
				while (!work_list.empty()) {
					auto n = *work_list.begin();
					work_list.erase(work_list.begin());
					for (const auto& d : dom_frons[n]) {
						if (already_has_phi_func.find(d) == already_has_phi_func.end()) {
							to_insert[d] = preds[d];
							already_has_phi_func.insert(d);
							if (ever_on_work_list.find(d) == ever_on_work_list.end()) {
								ever_on_work_list.insert(d);
								work_list.insert(d);
							}
						}
					}
				}
				vector<int> insertion_order;
				for (const auto&[k, v] : to_insert)
					insertion_order.push_back(k);
				sort(insertion_order.begin(), insertion_order.end(), greater<int>{});
				for (const auto& k : insertion_order) {
					vector<shared_ptr<phi_insn::pred_data>> pred_list;
					for (const auto& v : to_insert[k]) {
						shared_ptr<phi_insn::pred_data> pd = make_shared<phi_insn::pred_data>();
						pd->bb_pred = v;
						pred_list.push_back(pd);
					}
					shared_ptr<register_operand> copy_reg_op = make_shared<register_operand>(*reg_op);
					shared_ptr<phi_insn> p = make_shared<phi_insn>(copy_reg_op, pred_list);
					int insert_pos = bbs->get_basic_block(k)->start();
					bbs->get_basic_block(k)->set_end(bbs->get_basic_block(k)->end() + 1);
					bbs->insert_insn(insert_pos + 1, p);
					for (int _ = k + 1; _ < bbs->get_basic_blocks().size(); _++) {
						bbs->get_basic_block(_)->set_start(bbs->get_basic_block(_)->start() + 1);
						bbs->get_basic_block(_)->set_end(bbs->get_basic_block(_)->end() + 1);
					}
				}
			}
		}

		void bb_rename_variables(shared_ptr<basic_blocks> bbs, int j, shared_ptr<rename_data> rd, bool global) {
			if (rd->visited.find(j) != rd->visited.end())
				return;
			rd->visited.insert(j);

			unordered_set<int> all_vars;
			unordered_map<int, bool> var_first_use;

			auto handle_rhs = [&bbs, &rd, &var_first_use](shared_ptr<register_operand> d) {
				if (!rd->reg_names[d->original_virtual_register_number()].empty()) {
					d->set_virtual_register_number(rd->reg_names[d->original_virtual_register_number()].top(), false);
					var_first_use[d->original_virtual_register_number()] = false;
				}
			};

			auto handle_lhs = [&bbs, &rd, &all_vars, &handle_rhs, &var_first_use](shared_ptr<register_operand> d) {
				if (all_vars.find(d->original_virtual_register_number()) != all_vars.end()
					&& rd->reg_names[d->original_virtual_register_number()].size() == 1
					&& var_first_use[d->original_virtual_register_number()]) {
					handle_rhs(d);
					var_first_use[d->original_virtual_register_number()] = false;
				}
				else {
					int next_reg_num = bbs->get_middle_ir()->next_virtual_register_number();
					bbs->get_middle_ir()->add_register_type(next_reg_num, d->register_type());
					rd->reg_names[d->original_virtual_register_number()].push(next_reg_num);
					d->set_virtual_register_number(next_reg_num, false);
				}
			};

			auto wrapped_handle_lhs = [&bbs, &rd, &handle_lhs, &handle_rhs](shared_ptr<operand> o) {
				if (o->operand_kind() == operand::kind::KIND_REGISTER) {
					shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(o);
					if (!ro->dereference())
						handle_lhs(ro);
					else
						handle_rhs(ro);
				}
			};

			auto wrapped_handle_rhs = [&bbs, &rd, &handle_rhs](shared_ptr<operand> o) {
				if (o->operand_kind() == operand::kind::KIND_REGISTER) {
					shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(o);
					handle_rhs(ro);
				}
			};

			shared_ptr<basic_block> bb = bbs->get_basic_block(j);
			for (int _ = bb->start(); _ < bb->end(); _++) {
				shared_ptr<insn> i = bbs->get_insn(_);
				if (i->insn_kind() == insn::kind::KIND_PHI) {
					shared_ptr<phi_insn> p = static_pointer_cast<phi_insn>(i);
					handle_lhs(p->dst());
				}
			}

			for (int _ = bb->start(); _ < bb->end(); _++) {
				shared_ptr<insn> i = bbs->get_insn(_);
				switch (i->insn_kind()) {
				case insn::kind::KIND_ACCESS: {
					shared_ptr<access_insn> ai = static_pointer_cast<access_insn>(i);
					wrapped_handle_lhs(ai->variable());
				}
					break;
				case insn::kind::KIND_ALIGN:
					break;
				case insn::kind::KIND_ASM: {
					shared_ptr<asm_insn> ai = static_pointer_cast<asm_insn>(i);
					if (ai->asm_insn_kind() == asm_insn::kind::KIND_LA)
						wrapped_handle_rhs(ai->la().second);
				}
					break;
				case insn::kind::KIND_BINARY: {
					shared_ptr<binary_insn> bi = static_pointer_cast<binary_insn>(i);
					wrapped_handle_rhs(bi->src1_operand());
					wrapped_handle_rhs(bi->src2_operand());
					wrapped_handle_lhs(bi->dst_operand());
				}
					break;
				case insn::kind::KIND_CALL: {
					shared_ptr<call_insn> ci = static_pointer_cast<call_insn>(i);
					wrapped_handle_rhs(ci->function_operand());
					for (auto& rhs : ci->argument_list())
						wrapped_handle_rhs(rhs);
					wrapped_handle_lhs(ci->dest_operand());
				}
					break;
				case insn::kind::KIND_CONDITIONAL: {
					shared_ptr<conditional_insn> ci = static_pointer_cast<conditional_insn>(i);
					wrapped_handle_rhs(ci->src());
				}
					break;
				case insn::kind::KIND_EXT: {
					shared_ptr<ext_insn> ei = static_pointer_cast<ext_insn>(i);
					wrapped_handle_rhs(ei->rhs());
					wrapped_handle_lhs(ei->lhs());
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
					wrapped_handle_rhs(mi->source());
					wrapped_handle_rhs(mi->destination());
				}
					break;
				case insn::kind::KIND_PHI:
					break;
				case insn::kind::KIND_RETURN: {
					shared_ptr<return_insn> ri = static_pointer_cast<return_insn>(i);
					wrapped_handle_rhs(ri->expr());
				}
					break;
				case insn::kind::KIND_TRUNC: {
					shared_ptr<trunc_insn> ti = static_pointer_cast<trunc_insn>(i);
					wrapped_handle_rhs(ti->rhs());
					wrapped_handle_lhs(ti->lhs());
				}
					break;
				case insn::kind::KIND_UNARY: {
					shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
					if (ui->unary_expr_kind() == unary_insn::kind::KIND_STK ||
						ui->unary_expr_kind() == unary_insn::kind::KIND_RESV) {
						shared_ptr<operand> d = ui->dst_operand();
						if (d->operand_kind() == operand::kind::KIND_REGISTER) {
							shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(d);
							if (!ro->dereference()) {
								all_vars.insert(ro->original_virtual_register_number());
								var_first_use[ro->original_virtual_register_number()] = true;
							}
						}
					}
					wrapped_handle_rhs(ui->src_operand());
					wrapped_handle_lhs(ui->dst_operand());
				}
					break;
				case insn::kind::KIND_VAR: {
					shared_ptr<var_insn> vi = static_pointer_cast<var_insn>(i);
					if (vi->dst()->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(vi->dst());
						if (!ro->dereference()) {
							all_vars.insert(ro->original_virtual_register_number());
							var_first_use[ro->original_virtual_register_number()] = true;
						}
					}
					wrapped_handle_rhs(vi->size());
					wrapped_handle_lhs(vi->dst());
				}
					break;
				}
			}

			unordered_set<int> adj = bbs->cfg().adjacent(j);
			for (const auto& bb_index : adj) {
				shared_ptr<basic_block> bb_adj = bbs->get_basic_block(bb_index);
				for (int index = bb_adj->start(); index < bb_adj->end(); index++) {
					shared_ptr<insn> i = bbs->get_insn(index);
					if (i->insn_kind() == insn::kind::KIND_PHI) {
						shared_ptr<phi_insn> p = static_pointer_cast<phi_insn>(i);
						for (auto& pd : p->pred_data_list()) {
							if (pd->bb_pred == -1)
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							if (bbs->get_basic_block(pd->bb_pred)->start() == bb->start() && bbs->get_basic_block(pd->bb_pred)->end() == bb->end()) {
								if (!rd->reg_names[p->dst()->original_virtual_register_number()].empty()) {
									pd->reg_pred = make_shared<register_operand>(false, p->dst()->original_virtual_register_number(),
										bbs->get_middle_ir()->get_register_type(p->dst()->original_virtual_register_number()));
									pd->reg_pred->set_virtual_register_number(rd->reg_names[p->dst()->original_virtual_register_number()].top(), false);
								}
							}
						}
					}
				}
			}

			for (const auto& a : adj)
				bb_rename_variables(bbs, a, rd, global);

			if (global)
				return;

			auto checked_reg_pop = [&bbs, &rd](shared_ptr<operand> o) {
				if (o->operand_kind() == operand::kind::KIND_REGISTER) {
					shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(o);
					if (!ro->dereference() && !rd->reg_names[ro->original_virtual_register_number()].empty())
						rd->reg_names[ro->original_virtual_register_number()].pop();
				}
			};

			for (int _ = bb->start(); _ < bb->end(); _++) {
				shared_ptr<insn> i = bbs->get_insn(_);
				switch (i->insn_kind()) {
				case insn::kind::KIND_ACCESS: {
					shared_ptr<access_insn> ai = static_pointer_cast<access_insn>(i);
					checked_reg_pop(ai->variable());
				}
					break;
				case insn::kind::KIND_ALIGN:
				case insn::kind::KIND_ASM:
					break;
				case insn::kind::KIND_BINARY: {
					shared_ptr<binary_insn> bi = static_pointer_cast<binary_insn>(i);
					checked_reg_pop(bi->dst_operand());
				}
					break;
				case insn::kind::KIND_CALL: {
					shared_ptr<call_insn> ci = static_pointer_cast<call_insn>(i);
					checked_reg_pop(ci->dest_operand());
				}
					break;
				case insn::kind::KIND_CONDITIONAL:
					break;
				case insn::kind::KIND_EXT: {
					shared_ptr<ext_insn> ei = static_pointer_cast<ext_insn>(i);
					checked_reg_pop(ei->lhs());
				}
					break;
				case insn::kind::KIND_FUNCTION:
					bbs->report_internal("This should be unreacahable.", __FUNCTION__, __LINE__, __FILE__);
					break;
				case insn::kind::KIND_JUMP:
				case insn::kind::KIND_LABEL:
					break;
				case insn::kind::KIND_MEMCPY:
					break;
				case insn::kind::KIND_PHI: {
					shared_ptr<phi_insn> pi = static_pointer_cast<phi_insn>(i);
					checked_reg_pop(pi->dst());
				}
					break;
				case insn::kind::KIND_RETURN:
					break;
				case insn::kind::KIND_TRUNC: {
					shared_ptr<trunc_insn> ti = static_pointer_cast<trunc_insn>(i);
					checked_reg_pop(ti->lhs());
				}
					break;
				case insn::kind::KIND_UNARY: {
					shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
					checked_reg_pop(ui->dst_operand());
				}
					break;
				case insn::kind::KIND_VAR: {
					shared_ptr<var_insn> vi = static_pointer_cast<var_insn>(i);
					checked_reg_pop(vi->dst());
				}
					break;
				default:
					break;
				}
			}
		}

		void rename_variables(shared_ptr<basic_blocks> bbs) {
			unordered_map<int, unordered_set<int>> preds = get_predecessors(bbs->cfg());
			unordered_set<int> start_nodes;
			for (const auto& [k, v] : preds) {
				if (v.empty())
					start_nodes.insert(k);
			}
			shared_ptr<rename_data> rd = make_shared<rename_data>();
			int global_index = -1;
			for (const auto& n : start_nodes) {
				shared_ptr<basic_block> bb = bbs->get_basic_block(n);
				if (!bb->get_insns(bbs).empty()) {
					shared_ptr<insn> first = bb->get_insns(bbs)[0];
					if (first->insn_kind() == insn::kind::KIND_LABEL) {
						shared_ptr<label_insn> li = static_pointer_cast<label_insn>(first);
						if (li->label()->label_text() == global_header) {
							global_index = n;
							break;
						}
					}
				}
			}
			if (global_index == -1)
				bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			bb_rename_variables(bbs, global_index, rd, true);
			for (const auto& n : start_nodes)
				bb_rename_variables(bbs, n, rd, false);
		}

		void clean_up_phi_insns(shared_ptr<basic_blocks> bbs) {
			int old_num_insns = bbs->num_insns();
			bool changed = true;
			unordered_map<int, int> unnecessary_phi_dst_to_orig;
			unordered_set<int> all_removed;

			while (changed || old_num_insns != bbs->num_insns()) {
				changed = false;
				old_num_insns = bbs->num_insns();

				for (int index = 0; index < bbs->num_insns(); index++) {
					shared_ptr<insn> i = static_pointer_cast<insn>(bbs->get_insn(index));
					if (i->insn_kind() == insn::kind::KIND_PHI) {
						shared_ptr<phi_insn> p = static_pointer_cast<phi_insn>(i);
						vector<int> to_remove;
						int pd_index = 0;
						for (const auto& pd : p->pred_data_list()) {
							if (pd->reg_pred == nullptr || pd->reg_pred->virtual_register_number() == p->dst()->virtual_register_number())
								to_remove.push_back(pd_index), changed = true;
							pd_index++;
						}
						reverse(to_remove.begin(), to_remove.end());
						for (const auto& pd_index : to_remove)
							p->remove_pred(pd_index);
					}
				}

				vector<int> unnecessary_phis;
				unordered_set<int> orig_unnecessary_phis;
				for (int index = 0; index < bbs->num_insns(); index++) {
					shared_ptr<insn> i = static_pointer_cast<insn>(bbs->get_insn(index));
					if (i->insn_kind() == insn::kind::KIND_PHI) {
						shared_ptr<phi_insn> p = static_pointer_cast<phi_insn>(i);
						bool remove = false;
						if (p->pred_data_list().size() <= 1)
							remove = true;
						else if (p->pred_data_list().size() > 1) {
							shared_ptr<register_operand> orig_reg_pred = p->pred_data_list()[0]->reg_pred;
							remove = true;
							for (const auto& a : p->pred_data_list()) {
								if (a->reg_pred->virtual_register_number() != orig_reg_pred->virtual_register_number()) {
									remove = false;
									break;
								}
							}
						}
						if (remove) {
							unnecessary_phis.push_back(index);
							changed = true;
							orig_unnecessary_phis.insert(p->dst()->virtual_register_number());
							all_removed.insert(p->dst()->virtual_register_number());
							if (!p->pred_data_list().empty()) {
								int curr = p->pred_data_list()[0]->reg_pred->virtual_register_number();
								if (unnecessary_phi_dst_to_orig.find(curr) != unnecessary_phi_dst_to_orig.end())
									unnecessary_phi_dst_to_orig[p->dst()->virtual_register_number()] = unnecessary_phi_dst_to_orig[curr];
								else
									unnecessary_phi_dst_to_orig[p->dst()->virtual_register_number()] = curr;
							}
						}
					}
				}

				auto wrapped_handle = [&unnecessary_phi_dst_to_orig](shared_ptr<operand> o) {
					if (o->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(o);
						if (unnecessary_phi_dst_to_orig.find(ro->virtual_register_number()) != unnecessary_phi_dst_to_orig.end())
							ro->set_virtual_register_number(unnecessary_phi_dst_to_orig[ro->virtual_register_number()], false);
					}
				};

				for (int index = 0; index < bbs->num_insns(); index++) {
					shared_ptr<insn> i = bbs->get_insn(index);
					switch (i->insn_kind()) {
					case insn::kind::KIND_ACCESS: {
						shared_ptr<access_insn> ai = static_pointer_cast<access_insn>(i);
						wrapped_handle(ai->variable());
					}
						break;
					case insn::kind::KIND_ALIGN:
						break;
					case insn::kind::KIND_ASM: {
						shared_ptr<asm_insn> ai = static_pointer_cast<asm_insn>(i);
						if (ai->asm_insn_kind() == asm_insn::kind::KIND_LA)
							wrapped_handle(ai->la().second);
					}
						break;
					case insn::kind::KIND_BINARY: {
						shared_ptr<binary_insn> bi = static_pointer_cast<binary_insn>(i);
						wrapped_handle(bi->src1_operand());
						wrapped_handle(bi->src2_operand());
						wrapped_handle(bi->dst_operand());
					}
						break;
					case insn::kind::KIND_CALL: {
						shared_ptr<call_insn> ci = static_pointer_cast<call_insn>(i);
						wrapped_handle(ci->function_operand());
						for (auto& rhs : ci->argument_list())
							wrapped_handle(rhs);
						wrapped_handle(ci->dest_operand());
					}
						break;
					case insn::kind::KIND_CONDITIONAL: {
						shared_ptr<conditional_insn> ci = static_pointer_cast<conditional_insn>(i);
						wrapped_handle(ci->src());
					}
						break;
					case insn::kind::KIND_EXT: {
						shared_ptr<ext_insn> ei = static_pointer_cast<ext_insn>(i);
						wrapped_handle(ei->rhs());
						wrapped_handle(ei->lhs());
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
						wrapped_handle(mi->source());
						wrapped_handle(mi->destination());
					}
						break;
					case insn::kind::KIND_PHI:
						break;
					case insn::kind::KIND_RETURN: {
						shared_ptr<return_insn> ri = static_pointer_cast<return_insn>(i);
						wrapped_handle(ri->expr());
					}
						break;
					case insn::kind::KIND_TRUNC: {
						shared_ptr<trunc_insn> ti = static_pointer_cast<trunc_insn>(i);
						wrapped_handle(ti->rhs());
						wrapped_handle(ti->lhs());
					}
						break;
					case insn::kind::KIND_UNARY: {
						shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
						wrapped_handle(ui->src_operand());
						wrapped_handle(ui->dst_operand());
					}
						break;
					case insn::kind::KIND_VAR: {
						shared_ptr<var_insn> vi = static_pointer_cast<var_insn>(i);
						wrapped_handle(vi->size());
						wrapped_handle(vi->dst());
					}
						break;
					}
				}

				for (int index = 0; index < bbs->num_insns(); index++) {
					shared_ptr<insn> i = static_pointer_cast<insn>(bbs->get_insn(index));
					if (i->insn_kind() == insn::kind::KIND_PHI) {
						shared_ptr<phi_insn> p = static_pointer_cast<phi_insn>(i);
						if (p->pred_data_list().size() > 1) {
							unordered_set<int> regs;
							for (auto& pd : p->pred_data_list()) {
								if (orig_unnecessary_phis.find(pd->reg_pred->virtual_register_number()) != orig_unnecessary_phis.end()) {
									if (unnecessary_phi_dst_to_orig.find(pd->reg_pred->virtual_register_number()) != unnecessary_phi_dst_to_orig.end()) {
										int to_replace = unnecessary_phi_dst_to_orig[pd->reg_pred->virtual_register_number()];
										if (all_removed.find(to_replace) != all_removed.end())
											pd->reg_pred = nullptr;
										else
											pd->reg_pred->set_virtual_register_number(to_replace, false);
									}
									else
										pd->reg_pred = nullptr;
									changed = true;
								}
							}
						}
					}
				}
				sort(unnecessary_phis.begin(), unnecessary_phis.end(), greater{});
				vector<int> bb_indices;
				for (const auto& r : unnecessary_phis) {
					int bb_index = -1;
					for (int j = 0; j < bbs->get_basic_blocks().size(); j++) {
						if (bbs->get_basic_block(j)->start() <= r && r < bbs->get_basic_block(j)->end()) {
							bb_index = j;
							break;
						}
					}
					if (bb_index == -1)
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					bb_indices.push_back(bb_index);
				}
				if (bb_indices.size() != unnecessary_phis.size())
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				for (int index = 0; index < bb_indices.size(); index++) {
					int insn_index = unnecessary_phis[index], bb_index = bb_indices[index];
					bbs->remove_insn(insn_index);
					bbs->get_basic_block(bb_index)->set_end(bbs->get_basic_block(bb_index)->end() - 1);
					for (int _ = bb_index + 1; _ < bbs->get_basic_blocks().size(); _++) {
						bbs->get_basic_block(_)->set_start(bbs->get_basic_block(_)->start() - 1);
						bbs->get_basic_block(_)->set_end(bbs->get_basic_block(_)->end() - 1);
					}
				}
			}
		}

		void ssa_sanity_check(shared_ptr<basic_blocks> bbs) {
			unordered_set<int> defined_regs;
			unordered_map<int, bool> defined_regs_first_time;

			auto check_rhs = [&defined_regs, &bbs, &defined_regs_first_time](shared_ptr<insn> i, shared_ptr<operand> o) {
				if (o->operand_kind() == operand::kind::KIND_REGISTER) {
					shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(o);
					if (defined_regs.find(ro->virtual_register_number()) == defined_regs.end())
						return false;
					defined_regs_first_time[ro->virtual_register_number()] = false;
				}
				return true;
			};

			auto check_lhs = [&defined_regs, &bbs, &check_rhs, &defined_regs_first_time](shared_ptr<insn> i, shared_ptr<operand> o) {
				if (o->operand_kind() == operand::kind::KIND_REGISTER) {
					shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(o);
					if (!ro->dereference()) {
						if (defined_regs.find(ro->virtual_register_number()) != defined_regs.end()
							&& !defined_regs_first_time[ro->virtual_register_number()])
							return false;
						defined_regs.insert(ro->virtual_register_number());
						defined_regs_first_time[ro->virtual_register_number()] = false;

					}
					else
						return check_rhs(i, o);
				}
				return true;
			};

			for (int index = 0; index < bbs->num_insns(); index++) {
				shared_ptr<insn> i = bbs->get_insn(index);
				bool res = true;
				switch (i->insn_kind()) {
				case insn::kind::KIND_ACCESS: {
					shared_ptr<access_insn> ai = static_pointer_cast<access_insn>(i);
					res = check_lhs(i, ai->variable());
				}
					break;
				case insn::kind::KIND_ALIGN:
					break;
				case insn::kind::KIND_ASM: {
					shared_ptr<asm_insn> ai = static_pointer_cast<asm_insn>(i);
					if (ai->asm_insn_kind() == asm_insn::kind::KIND_LA)
						res = check_rhs(i, ai->la().second);
				}
					break;
				case insn::kind::KIND_BINARY: {
					shared_ptr<binary_insn> bi = static_pointer_cast<binary_insn>(i);
					res = check_rhs(i, bi->src1_operand())
						&& check_rhs(i, bi->src2_operand())
						&& check_lhs(i, bi->dst_operand());
				}
					break;
				case insn::kind::KIND_CALL: {
					shared_ptr<call_insn> ci = static_pointer_cast<call_insn>(i);
					res = check_rhs(i, ci->function_operand())
						&& check_lhs(i, ci->dest_operand());
					for (auto& rhs : ci->argument_list())
						res = res && check_rhs(i, rhs);
				}
					break;
				case insn::kind::KIND_CONDITIONAL: {
					shared_ptr<conditional_insn> ci = static_pointer_cast<conditional_insn>(i);
					res = check_rhs(i, ci->src());
				}
					break;
				case insn::kind::KIND_EXT: {
					shared_ptr<ext_insn> ei = static_pointer_cast<ext_insn>(i);
					res = check_rhs(i, ei->rhs())
						&& check_lhs(i, ei->lhs());
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
					res = check_rhs(i, mi->source())
						&& check_rhs(i, mi->destination());
				}
					break;
				case insn::kind::KIND_PHI: {
					shared_ptr<phi_insn> pi = static_pointer_cast<phi_insn>(i);
					res = check_lhs(i, pi->dst());
				}
					break;
				case insn::kind::KIND_RETURN: {
					shared_ptr<return_insn> ri = static_pointer_cast<return_insn>(i);
					res = check_rhs(i, ri->expr());
				}
					break;
				case insn::kind::KIND_TRUNC: {
					shared_ptr<trunc_insn> ti = static_pointer_cast<trunc_insn>(i);
					res = check_rhs(i, ti->rhs())
						&& check_lhs(i, ti->lhs());
				}
					break;
				case insn::kind::KIND_UNARY: {
					shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
					res = check_rhs(i, ui->src_operand())
						&& check_lhs(i, ui->dst_operand());
					if (ui->unary_expr_kind() == unary_insn::kind::KIND_STK || ui->unary_expr_kind() == unary_insn::kind::KIND_RESV) {
						if (ui->dst_operand()->operand_kind() == operand::kind::KIND_REGISTER) {
							shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(ui->dst_operand());
							if (!ro->dereference())
								defined_regs_first_time[ro->virtual_register_number()] = true;
						}
					}
				}
					break;
				case insn::kind::KIND_VAR: {
					shared_ptr<var_insn> vi = static_pointer_cast<var_insn>(i);
					res = check_rhs(i, vi->size())
						&& check_lhs(i, vi->dst());
					if (vi->dst()->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(vi->dst());
						if (!ro->dereference())
							defined_regs_first_time[ro->virtual_register_number()] = true;
					}
				}
					break;
				}
				if (!res) {
					print_bbs_cfg(bbs);
					bbs->report_internal("This should be unreachable: " + i->to_string(), __FUNCTION__, __LINE__, __FILE__);
				}
			}

			for (int index = 0; index < bbs->num_insns(); index++) {
				shared_ptr<insn> i = bbs->get_insn(index);
				if (i->insn_kind() == insn::kind::KIND_PHI) {
					shared_ptr<phi_insn> p = static_pointer_cast<phi_insn>(i);
					if (p->pred_data_list().size() <= 1) {
						print_bbs_cfg(bbs);
						bbs->report_internal("This should be unreachable: " + i->to_string(), __FUNCTION__, __LINE__, __FILE__);
					}
					unordered_set<int> reg_nums;
					for (const auto& p : p->pred_data_list()) {
						if (p->reg_pred != nullptr)
							reg_nums.insert(p->reg_pred->virtual_register_number());
						if (p->bb_pred < 0 || p->bb_pred >= bbs->get_basic_blocks().size()) {
							print_bbs_cfg(bbs);
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						}
					}
					if (reg_nums.size() <= 1) {
						print_bbs_cfg(bbs);
						bbs->report_internal("This should be unreachable: " + i->to_string(), __FUNCTION__, __LINE__, __FILE__);
					}
					for (const auto& r : reg_nums) {
						if (defined_regs.find(r) == defined_regs.end()) {
							print_bbs_cfg(bbs);
							bbs->report_internal("This should be unreachable: " + i->to_string(), __FUNCTION__, __LINE__, __FILE__);
						}
					}
				}
			}
		}

		void print_bbs_dominance_data(shared_ptr<basic_blocks> bbs) {
			cout << endl << "Dominators" << endl;
			auto doms = compute_dominators(bbs, bbs->cfg());
			for (const auto&[k, v] : doms) {
				cout << k << ": ";
				for (auto it = v.begin(); it != v.end(); it++) {
					cout << *it;
					if (next(it) != v.end())
						cout << ", ";
				}
				cout << endl;
			}
			cout << endl;

			cout << endl << "Strict Dominators" << endl;
			auto sdoms = compute_strict_dominators(bbs, doms);
			for (const auto&[k, v] : sdoms) {
				cout << k << ": ";
				for (auto it = v.begin(); it != v.end(); it++) {
					cout << *it;
					if (next(it) != v.end())
						cout << ", ";
				}
				cout << endl;
			}
			cout << endl;

			cout << endl << "Webviz ImmediateDominators" << endl;
			cout << "digraph ImmediateDominators {" << endl;
			for (const auto&[k, v] : compute_immediate_dominators(bbs, doms)) {
				cout << "\t" << v << " -> " << k << endl;
			}
			cout << "}" << endl;

			cout << endl << "Dominance Frontier" << endl;
			auto dom_fron = compute_dominance_frontier(bbs, bbs->cfg(), doms);
			for (const auto&[k, v] : dom_fron) {
				cout << k << ": ";
				for (auto it = v.begin(); it != v.end(); it++) {
					cout << *it;
					if (next(it) != v.end())
						cout << ", ";
				}
				cout << endl;
			}
			cout << endl;

			cout << endl;
		}

		cvt2ssa_pass::cvt2ssa_pass(shared_ptr<pass_manager> pm) : pass("cvt2ssa", "Converts IR code to SSA form.", pm->next_pass_id(),
			unordered_set<string>{ "mem2reg" }), _debug_ctx(pm->debug()) {

		}

		cvt2ssa_pass::~cvt2ssa_pass() {

		}

		shared_ptr<basic_blocks> cvt2ssa_pass::run_pass(shared_ptr<basic_blocks> bbs) {
			auto print_debug_info = [this, &bbs]() {
				if (_debug_ctx >= pass_manager::debug_level::KIND_DEBUG_VERBOSE) {
					cout << "__________________________________" << endl;
					print_bbs_cfg(bbs);
					cout << "__________________________________" << endl;
				}
			};

			insert_phi_insns(bbs);
			print_debug_info();
			rename_variables(bbs);
			print_debug_info();
			clean_up_phi_insns(bbs);
			if (_debug_ctx >= pass_manager::debug_level::KIND_DEBUG)
				ssa_sanity_check(bbs);
			return bbs;
		}
	}
}
