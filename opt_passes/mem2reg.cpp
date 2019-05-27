#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include "mem2reg.hpp"

using std::static_pointer_cast;
using std::unordered_map;
using std::unordered_set;
using std::make_shared;
using std::cout;
using std::endl;

namespace spectre {
	namespace ir {
		var_insn::var_insn(shared_ptr<register_operand> d, shared_ptr<operand> s) : insn(insn::kind::KIND_VAR), _dst(d), _size(s) {

		}

		var_insn::~var_insn() {

		}

		shared_ptr<register_operand> var_insn::dst() {
			return _dst;
		}

		shared_ptr<operand> var_insn::size() {
			return _size;
		}

		string var_insn::to_string() {
			return "var " + _dst->to_string() + ", " + _size->to_string();
		}
		
		void var_insn::deduplicate() {
			DEDUP(_dst);
		}
	}

	namespace opt {
		unordered_set<int> mem2reg_qualifying_registers(shared_ptr<basic_blocks> bbs) {
			vector<string> all_variable_names = bbs->get_middle_ir()->variable_names();
			unordered_set<int> variable_registers;
			for (const auto& variable_name : all_variable_names) {
				shared_ptr<variable_declaration> vd = bbs->get_middle_ir()->lookup_variable(variable_name);
				shared_ptr<type> vd_type = vd->variable_declaration_type();
				if (vd_type->type_kind() == type::kind::KIND_STRUCT && vd_type->array_dimensions() == 0)
					continue;
				variable_registers.insert(bbs->get_middle_ir()->lookup_variable_register(variable_name)->virtual_register_number());
			}

			unordered_set<int> pruned_variable_registers, allocating_insns;
			directed_graph<int> dependency_graph;
			for (int _ = 0; _ < bbs->num_insns(); _++) {
				shared_ptr<insn> i = bbs->get_insn(_);
				if (i->insn_kind() == insn::kind::KIND_UNARY) {
					shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
					if (ui->unary_expr_kind() == unary_insn::kind::KIND_STK && ui->dst_operand()->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> dst_reg = static_pointer_cast<register_operand>(ui->dst_operand());
						if (!dst_reg->dereference() && variable_registers.find(dst_reg->virtual_register_number()) != variable_registers.end()) {
							pruned_variable_registers.insert(dst_reg->virtual_register_number());
							allocating_insns.insert(_);
						}
					}
				}
			}
			for (const auto& r : pruned_variable_registers)
				dependency_graph.add_vertex(r);
			unordered_set<int> tainted;

			auto basic_taint_check = [&tainted, &dependency_graph](shared_ptr<operand> o) {
				if (o->operand_kind() == operand::kind::KIND_REGISTER) {
					shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(o);
					if (!ro->dereference() && dependency_graph.vertex_exists(ro->virtual_register_number()))
						tainted.insert(ro->virtual_register_number());
				}
			};

			for (int _ = 0; _ < bbs->num_insns(); _++) {
				if (allocating_insns.find(_) != allocating_insns.end())
					continue;
				shared_ptr<insn> i = bbs->get_insn(_);
				switch (i->insn_kind()) {
				case insn::kind::KIND_ACCESS: {
					shared_ptr<access_insn> ai = static_pointer_cast<access_insn>(i);
					basic_taint_check(ai->variable());
				};
					break;
				case insn::kind::KIND_ALIGN:
					break;
				case insn::kind::KIND_ASM: {
					for (const auto& n : pruned_variable_registers)
						tainted.insert(n);
				};
					break;
				case insn::kind::KIND_CALL: {
					shared_ptr<call_insn> ci = static_pointer_cast<call_insn>(i);
					basic_taint_check(ci->dest_operand());
					basic_taint_check(ci->function_operand());
					for (const auto& o : ci->argument_list())
						basic_taint_check(o);
				};
					break;
				case insn::kind::KIND_BINARY: {
					shared_ptr<binary_insn> bi = static_pointer_cast<binary_insn>(i);
					shared_ptr<operand> s1 = bi->src1_operand(), s2 = bi->src2_operand(), dst = bi->dst_operand();
					basic_taint_check(dst);
					basic_taint_check(s1);
					basic_taint_check(s2);
				};
					break;
				case insn::kind::KIND_CONDITIONAL:
					break;
				case insn::kind::KIND_EXT: {
					shared_ptr<ext_insn> ei = static_pointer_cast<ext_insn>(i);
					basic_taint_check(ei->lhs());
					basic_taint_check(ei->rhs());
				};
					break;
				case insn::kind::KIND_FUNCTION:
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					break;
				case insn::kind::KIND_JUMP:
				case insn::kind::KIND_LABEL:
					break;
				case insn::kind::KIND_MEMCPY: {
					shared_ptr<memcpy_insn> mi = static_pointer_cast<memcpy_insn>(i);
					basic_taint_check(mi->destination());
					basic_taint_check(mi->source());
				};
					break;
				case insn::kind::KIND_RETURN: {
					shared_ptr<return_insn> ri = static_pointer_cast<return_insn>(i);
					if (ri->return_kind() == return_insn::kind::KIND_REGULAR)
						basic_taint_check(ri->expr());
				}
					break;
				case insn::kind::KIND_TRUNC: {
					shared_ptr<trunc_insn> ti = static_pointer_cast<trunc_insn>(i);
					basic_taint_check(ti->lhs());
					basic_taint_check(ti->rhs());
				};
					break;
				case insn::kind::KIND_UNARY: {
					shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
					shared_ptr<operand> s = ui->src_operand(), d = ui->dst_operand();
					unary_insn::kind uik = ui->unary_expr_kind();
					basic_taint_check(d);
					if (s->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> rs = static_pointer_cast<register_operand>(s);
						bool rs_taint = !rs->dereference() && dependency_graph.vertex_exists(rs->virtual_register_number());
						if (uik != unary_insn::kind::KIND_CMOV && uik != unary_insn::kind::KIND_DMOV
							&& uik != unary_insn::kind::KIND_FMOV && uik != unary_insn::kind::KIND_IMOV
							&& uik != unary_insn::kind::KIND_LMOV && uik != unary_insn::kind::KIND_SMOV) {
							if (rs_taint)
								tainted.insert(rs->virtual_register_number());
						}
						else {
							if (d->operand_kind() == operand::kind::KIND_REGISTER) {
								shared_ptr<register_operand> dr = static_pointer_cast<register_operand>(d);
								if (dependency_graph.vertex_exists(dr->virtual_register_number()) && !dr->dereference()
									&& pruned_variable_registers.find(dr->virtual_register_number()) == pruned_variable_registers.end())
									tainted.insert(dr->virtual_register_number());
								if (dr->dereference() && rs_taint)
									tainted.insert(rs->virtual_register_number());
								else if (!dr->dereference())
									dependency_graph.add_edge(dr->virtual_register_number(), rs->virtual_register_number()),
										dependency_graph.add_edge(rs->virtual_register_number(), dr->virtual_register_number());
							}
							else {
								if (rs_taint)
									tainted.insert(rs->virtual_register_number());
							}
						}
					}
				};
					break;
				default:
					break;
				}
			}

			unordered_set<int> all_tainted;
			for (const auto& t : tainted) {
				unordered_set<int> visited;
				stack<int> next_to_visit;
				next_to_visit.push(t);
				while (!next_to_visit.empty()) {
					auto v = next_to_visit.top();
					next_to_visit.pop();
					if (visited.find(v) != visited.end())
						continue;
					visited.insert(v);
					for (const auto& n : dependency_graph.adjacent(v))
						next_to_visit.push(n);
				}
				all_tainted.insert(visited.begin(), visited.end());
			}

			unordered_set<int> unqualified_registers;
			for (const auto& n : pruned_variable_registers)
				if (all_tainted.find(n) != all_tainted.end())
					unqualified_registers.insert(n);

			unordered_set<int> qualifying_registers;
			for (const auto& n : pruned_variable_registers)
				if (unqualified_registers.find(n) == unqualified_registers.end())
					qualifying_registers.insert(n);
			return qualifying_registers;
		}

		shared_ptr<basic_blocks> replace_qualifying_registers(shared_ptr<basic_blocks> bbs, unordered_set<int> qualifying_registers) {
			shared_ptr<middle_ir> mi = bbs->get_middle_ir();
			vector<shared_ptr<insn>> insn_list;
			for (int i = 0; i < bbs->num_insns(); i++)
				insn_list.push_back(bbs->get_insn(i));
			vector<pair<int, shared_ptr<var_insn>>> allocating_insn_replacements;
			int _ = 0;
			for (shared_ptr<insn> i : insn_list) {
				if (i->insn_kind() == insn::kind::KIND_UNARY) {
					shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
					if (ui->unary_expr_kind() == unary_insn::kind::KIND_STK && ui->dst_operand()->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> dst_reg = static_pointer_cast<register_operand>(ui->dst_operand());
						if (!dst_reg->dereference() && qualifying_registers.find(dst_reg->virtual_register_number()) != qualifying_registers.end())
							allocating_insn_replacements.push_back(make_pair(_, make_shared<var_insn>(dst_reg, ui->src_operand())));
					}
				}
				_++;
			}

			for (const auto&[index, vi] : allocating_insn_replacements)
				insn_list[index] = vi;

			vector<int> to_remove;
			auto perform_indices_removal = [&to_remove, &insn_list]() {
				for (int removal_index = 0, i = 0; i < insn_list.size(); i++) {
					if (removal_index < to_remove.size() && to_remove[removal_index] - removal_index == i) {
						insn_list.erase(insn_list.begin() + i);
						removal_index++;
						i--;
					}
				}
			};

			unordered_map<int, int> base_register_mapping;
			for (const auto& n : qualifying_registers)
				base_register_mapping[n] = n;

			auto replacement_check = [&bbs, &base_register_mapping](shared_ptr<operand> o) {
				if (o->operand_kind() == operand::kind::KIND_REGISTER) {
					shared_ptr<register_operand> ro = static_pointer_cast<register_operand>(o);
					if (base_register_mapping.find(ro->virtual_register_number()) != base_register_mapping.end()) {
						int true_reg = base_register_mapping[ro->virtual_register_number()];
						ro->set_dereference(false);
						ro->set_virtual_register_number(true_reg);
					}
				}
			};

			to_remove.clear();
			for (int _ = 0; _ < insn_list.size(); _++) {
				shared_ptr<insn> i = insn_list[_];
				switch (i->insn_kind()) {
				case insn::kind::KIND_ACCESS: {
					shared_ptr<access_insn> ai = static_pointer_cast<access_insn>(i);
					replacement_check(ai->variable());
				};
					break;
				case insn::kind::KIND_ALIGN:
				case insn::kind::KIND_ASM:
					break;
				case insn::kind::KIND_BINARY: {
					shared_ptr<binary_insn> bi = static_pointer_cast<binary_insn>(i);
					shared_ptr<operand> d = bi->dst_operand(), s1 = bi->src1_operand(), s2 = bi->src2_operand();
					replacement_check(d);
					replacement_check(s1);
					replacement_check(s2);
				}
					break;
				case insn::kind::KIND_CALL: {
					shared_ptr<call_insn> ci = static_pointer_cast<call_insn>(i);
					replacement_check(ci->dest_operand());
					replacement_check(ci->function_operand());
					for (const auto& arg : ci->argument_list())
						replacement_check(arg);
				}
					break;

				case insn::kind::KIND_CONDITIONAL: {
					shared_ptr<conditional_insn> ci = static_pointer_cast<conditional_insn>(i);
					replacement_check(ci->src());
				}
					break;
				case insn::kind::KIND_EXT: {
					shared_ptr<ext_insn> ei = static_pointer_cast<ext_insn>(i);
					replacement_check(ei->lhs());
					replacement_check(ei->rhs());
				}
					break;
				case insn::kind::KIND_FUNCTION:
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					break;
				case insn::kind::KIND_JUMP:
				case insn::kind::KIND_LABEL:
					break;
				case insn::kind::KIND_MEMCPY: {
					shared_ptr<memcpy_insn> mi = static_pointer_cast<memcpy_insn>(i);
					replacement_check(mi->destination());
					replacement_check(mi->source());
				}
					break;
				case insn::kind::KIND_RETURN: {
					shared_ptr<return_insn> ri = static_pointer_cast<return_insn>(i);
					if (ri->return_kind() == return_insn::kind::KIND_REGULAR)
						replacement_check(ri->expr());
				}
					break;
				case insn::kind::KIND_TRUNC: {
					shared_ptr<trunc_insn> ti = static_pointer_cast<trunc_insn>(i);
					replacement_check(ti->lhs());
					replacement_check(ti->rhs());
				}
					break;
				case insn::kind::KIND_UNARY: {
					shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
					shared_ptr<operand> s = ui->src_operand(), d = ui->dst_operand();
					unary_insn::kind uik = ui->unary_expr_kind();
					if (s->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> rs = static_pointer_cast<register_operand>(s);
						if (uik == unary_insn::kind::KIND_CMOV || uik == unary_insn::kind::KIND_DMOV
							|| uik == unary_insn::kind::KIND_FMOV || uik == unary_insn::kind::KIND_IMOV
							|| uik == unary_insn::kind::KIND_LMOV || uik == unary_insn::kind::KIND_SMOV) {
							if (base_register_mapping.find(rs->virtual_register_number()) != base_register_mapping.end()) {
								if (d->operand_kind() == operand::kind::KIND_REGISTER) {
									shared_ptr<register_operand> rd = static_pointer_cast<register_operand>(d);
									if ((!rd->dereference() && base_register_mapping.find(rd->virtual_register_number()) != base_register_mapping.end())
										|| (rd->dereference() && !rs->dereference()))
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									if (!rs->dereference()) {
										to_remove.push_back(_);
										base_register_mapping[rd->virtual_register_number()] = base_register_mapping[rs->virtual_register_number()];
									}
									else {
										replacement_check(rs);
										replacement_check(rd);
									}
								}
								else
									replacement_check(s);
							}
							else {
								replacement_check(d);
								replacement_check(s);
							}
						}
						else {
							replacement_check(d);
							replacement_check(s);
						}
					}
					else {
						replacement_check(d);
						replacement_check(s);
					}
				};
					break;
				default:
					break;
				}
			}

			perform_indices_removal();
			mi->clear_global_insn_list();
			mi->set_insn_list(insn_list);

			return generate_cfg(mi);
		}

		mem2reg_pass::mem2reg_pass(shared_ptr<pass_manager> pm)
			: pass("mem2reg", "Calculates memory locations that can be replaced by a registers and replace them.", pm->next_pass_id(), unordered_set<string>{}) {

		}

		mem2reg_pass::~mem2reg_pass() {

		}

		shared_ptr<basic_blocks> mem2reg_pass::run_pass(shared_ptr<basic_blocks> bbs) {
			unordered_set<int> u = mem2reg_qualifying_registers(bbs);
			shared_ptr<basic_blocks> _bbs = replace_qualifying_registers(bbs, u);
			return _bbs;
		}
	}
}