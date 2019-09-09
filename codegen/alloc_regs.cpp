#include "codegen/alloc_regs.hpp"
#include "opt/mem2reg.hpp"

#include <memory>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <stack>
#include <limits>
#include <array>

using std::make_shared;
using std::cout;
using std::endl;
using std::make_pair;
using std::sort;
using std::unique;
using std::vector;
using std::pair;
using std::unordered_set;
using std::stack;
using std::numeric_limits;
using std::array;
using std::greater;
using std::static_pointer_cast;

namespace spectre {
	namespace cgen {

		static array<const char*, 16> graphviz_color_names = {
			"antiquewhite3",
			"aquamarine3",
			"azure3",
			"blueviolet",

			"brown4",
			"chartreuse4",
			"cadetblue4",
			"blue",

			"cyan",
			"crimson",
			"darkorange",
			"deeppink",

			"black",
			"ghostwhite",
			"darksalmon",
			"yellow"
		};

		shared_ptr<undirected_graph<int>> compute_initial_interference_graph(shared_ptr<liveness_data> ld) {
			shared_ptr<undirected_graph<int>> ret = make_shared<undirected_graph<int>>();
			for (const auto& [_, vertices] : ld->in) {
				for (const auto& u : vertices) {
					for (const auto& v : vertices) {
						ret->add_vertex(u);
						ret->add_vertex(v);
						if (u != v)
							ret->add_edge(u, v);
					}
				}
			}
			return ret;
		}

		void print_interference_graph(shared_ptr<undirected_graph<int>> u, const unordered_map<int, int>& c) {
			unordered_set<pair<int, int>, pair_hasher> included_edges;
			cout << "graph InterferenceGraph {" << endl;
			for (const auto& v : u->vertices()) {
				unordered_set<int> adj = u->adjacent(v);
				if (adj.empty())
					cout << "\t" << v << ";" << endl;
				for (const auto& w : adj) {
					if (included_edges.find(make_pair(v, w)) != included_edges.end()
						|| included_edges.find(make_pair(w, v)) != included_edges.end())
						continue;
					included_edges.insert(make_pair(v, w));
					included_edges.insert(make_pair(w, v));
					cout << "\t" << v << " -- " << w << ";" << endl;
				}
			}
			if (!u->vertices().empty())
				cout << endl;
			for (const auto& v : u->vertices()) {
				cout << "\t" << v << " [shape=circle, style=filled, fillcolor=" << graphviz_color_names[c.at(v)] << "]" << endl;
			}
			cout << "}" << endl;
		}

		unordered_map<int, int> color_interference_graph(shared_ptr<basic_blocks> bbs, int num_phys_regs, int num_fp_phys_regs,
			pass_manager::debug_level dl) {
			if (num_phys_regs < 3)
				bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);

			unordered_map<int, int> coloring;

			if (dl >= pass_manager::debug_level::KIND_DEBUG)
				cout << "=========================\nRegister allocation\n";
			auto calc_rw_locs = [&bbs] (shared_ptr<def_use_data> du) {
				unordered_map<int, vector<pair<int, int>>> reg_reads, reg_writes;
				for (const auto& [insn, regs] : du->def) {
					for (const auto& reg : regs)
						reg_writes[reg].push_back(insn);
				}
				for (const auto& [insn, regs] : du->use) {
					for (const auto& reg : regs)
						reg_reads[reg].push_back(insn);
				}
				return make_pair(reg_reads, reg_writes);
			};

			shared_ptr<def_use_data> du = calc_def_use(bbs);
			shared_ptr<liveness_data> ld = liveness_analysis(bbs, du);
			shared_ptr<undirected_graph<int>> ig = compute_initial_interference_graph(ld),
				orig_ig = make_shared<undirected_graph<int>>(*ig);
			unordered_set<int> verts = ig->vertices();
			stack<int> color_order;

			if (verts.empty())
				return coloring;

			while (!verts.empty()) {
				int to_use = -1;
				int most = -1, max_num_neighbors = numeric_limits<int>::min();
				bool hit = false;
				for (const auto& v : verts) {
					if (static_cast<int>(ig->adjacent(v).size()) > max_num_neighbors) {
						max_num_neighbors = ig->adjacent(v).size();
						most = v;
					}
					if (ig->adjacent(v).size() < num_phys_regs) {
						hit = true;
						to_use = v;
						break;
					}
				}
				if (!hit)
					to_use = most;
				ig->remove_vertex(to_use);
				verts.erase(to_use);
				color_order.push(to_use);
			}

			unordered_set<int> regs_to_spill;
			while (!color_order.empty()) {
				int v = color_order.top();
				shared_ptr<type> reg_type = du->reg_num_2_type[v];
				if (reg_type == nullptr) {
					print_bbs_cfg(bbs);
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				}
				int start_reg = 0, end_reg = num_phys_regs;
				if (is_floating_point_type(bbs->get_middle_ir(), reg_type)) {
					start_reg += num_phys_regs;
					end_reg += num_fp_phys_regs;
				}
				color_order.pop();
				unordered_set<int> adj = orig_ig->adjacent(v);
				unordered_set<int> colors;
				for (const auto& w : adj) {
					if (coloring.find(w) != coloring.end())
						colors.insert(coloring[w]);
				}
				
				unordered_set<int> all_colors;
				for (int i = start_reg; i < end_reg; i++)
					all_colors.insert(i);
				bool can_color = false;
				int color = -1;
				for (const auto& c : all_colors) {
					if (colors.find(c) == colors.end()) {
						can_color = true;
						color = c;
						break;
					}
				}
				if (!can_color) {
					if (dl >= pass_manager::debug_level::KIND_DEBUG)
						cout << "Spilled: " << v << endl;
					regs_to_spill.insert(v);
				}
				else
					coloring[v] = color;
			}

			for (const auto& v : regs_to_spill) {
				shared_ptr<def_use_data> du = calc_def_use(bbs);
				auto [reg_reads, reg_writes] = calc_rw_locs(du);
				vector<pair<int, int>> reads = reg_reads[v], writes = reg_writes[v];
				unordered_map<int, vector<int>> accesses;
				vector<int> accesses_bbs;
				for (const auto& [bb, loc] : reads)
					accesses[bb].push_back(loc);
				for (const auto& [bb, loc] : writes)
					accesses[bb].push_back(loc);
				for (auto& [_, locs] : accesses) {
					sort(locs.begin(), locs.end(), greater{});
					unique(locs.begin(), locs.end());
					accesses_bbs.push_back(_);
				}

				sort(accesses_bbs.begin(), accesses_bbs.end());

				unordered_set<pair<int, int>, pair_hasher> var_locs;
				for (int i = 0; i < bbs->get_basic_blocks().size(); i++) {
					shared_ptr<basic_block> bb = bbs->get_basic_block(i);
					for (int j = 0; j < bb->get_insns(bbs).size(); j++) {
						shared_ptr<insn> in = bb->get_insn(j);
						if (in->insn_kind() == insn::kind::KIND_VAR) {
							shared_ptr<register_operand> lhs = static_pointer_cast<var_insn>(in)->dst();
							if (!lhs->dereference() && lhs->virtual_register_number() == v)
								var_locs.insert(make_pair(i, j));
						}
					}
				}

				int alloc_loc = 0;
				if (var_locs.size() > 1)
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				if (var_locs.size() == 1) {
					pair<int, int> var_loc = *var_locs.begin();
					shared_ptr<var_insn> vi = static_pointer_cast<var_insn>(bbs->get_basic_block(var_loc.first)->get_insn(var_loc.second));
					bbs->get_basic_block(var_loc.first)->set_insn(var_loc.second, make_shared<unary_insn>(unary_insn::kind::KIND_STK,
								vi->dst(), vi->size(), vi->dst()->register_type()));
				}
				else {
					if (!accesses_bbs.empty()) {
						int bb = accesses_bbs[0];
						alloc_loc = 0;
						if (!bbs->get_basic_block(bb)->get_insns(bbs).empty()
							&& bbs->get_basic_block(bb)->get_insns(bbs)[0]->insn_kind() == insn::kind::KIND_LABEL)
							alloc_loc = 1;
					}
				}

				shared_ptr<size_data> sd = nullptr;
				shared_ptr<register_operand> ref_reg = nullptr;
				at_insn::kind ak = at_insn::kind::KIND_NONE, alk = at_insn::kind::KIND_NONE;
				auto check_and_initialize_register = [&] (shared_ptr<operand> o) {
					if (sd != nullptr)
						return;
					if (o->operand_kind() == operand::kind::KIND_REGISTER) {
						shared_ptr<register_operand> reg = static_pointer_cast<register_operand>(o);
						if (reg->virtual_register_number() == v) {
							if (du->reg_num_2_type[reg->virtual_register_number()] == nullptr)
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							sd = spectre_sizeof(bbs->get_middle_ir(), du->reg_num_2_type[reg->virtual_register_number()]);
							ref_reg = make_shared<register_operand>(*reg);
							ref_reg->set_dereference(false);
							ref_reg->set_register_type(du->reg_num_2_type[ref_reg->virtual_register_number()]);
							shared_ptr<type> dst_reg_type = ref_reg->register_type();
							if (is_double_type(bbs->get_middle_ir(), dst_reg_type))
								ak = at_insn::kind::KIND_DAT, alk = at_insn::kind::KIND_LHS_DAT;
							else if (is_float_type(bbs->get_middle_ir(), dst_reg_type))
								ak = at_insn::kind::KIND_FAT, alk = at_insn::kind::KIND_LHS_FAT;
							else if (is_int_type(bbs->get_middle_ir(), dst_reg_type) || is_pointer_type(bbs->get_middle_ir(), dst_reg_type)
								|| is_long_type(bbs->get_middle_ir(), dst_reg_type))
								ak = at_insn::kind::KIND_IAT, alk = at_insn::kind::KIND_LHS_IAT;
							else if (is_short_type(bbs->get_middle_ir(), dst_reg_type))
								ak = at_insn::kind::KIND_SAT, alk = at_insn::kind::KIND_LHS_SAT;
							else if (is_char_type(bbs->get_middle_ir(), dst_reg_type) || is_void_type(bbs->get_middle_ir(), dst_reg_type)
								|| is_byte_type(bbs->get_middle_ir(), dst_reg_type) || is_bool_type(bbs->get_middle_ir(), dst_reg_type))
								ak = at_insn::kind::KIND_CAT, alk = at_insn::kind::KIND_LHS_CAT;
							else if (is_struct_type(bbs->get_middle_ir(), dst_reg_type))
								ak = at_insn::kind::KIND_IAT, alk = at_insn::kind::KIND_LHS_IAT;
							else
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						}
					}
				};

				auto insert_read_insns = [&] (shared_ptr<basic_block> bb, vector<shared_ptr<insn>> insns_to_insert, int insn_loc) {
					if (!insns_to_insert.empty()) {
						for (int j = insns_to_insert.size() - 1; j >= 0; j--)
							bb->insert_insn(insn_loc, insns_to_insert[j]);
					}
				};

				auto insert_write_insns = [&] (shared_ptr<basic_block> bb, vector<shared_ptr<insn>> insns_to_insert, int insn_loc) {
					for (int j = 0; j < insns_to_insert.size(); j++)
						bb->insert_insn(insn_loc + 1, insns_to_insert[j]);
				};

				auto handle_rhs = [&] (shared_ptr<basic_block> bb, int insn_loc, shared_ptr<operand> o) {
					if (o->operand_kind() != operand::kind::KIND_REGISTER)
						return make_pair(vector<shared_ptr<insn>>(), o);
					shared_ptr<register_operand> reg = static_pointer_cast<register_operand>(o);
					check_and_initialize_register(reg);
					if (reg->virtual_register_number() != v)
						return make_pair(vector<shared_ptr<insn>>(), o);
					if (ref_reg == nullptr)
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					vector<shared_ptr<insn>> insns;
					if (!reg->dereference()) {
						reg->set_dereference(true);
						return make_pair(insns, static_pointer_cast<operand>(reg));
					}
					shared_ptr<type> derefed_type = dereference(ref_reg->register_type());
					shared_ptr<register_operand> temp = bbs->get_middle_ir()->generate_next_register(derefed_type),
						temp_stk = make_shared<register_operand>(*temp);
					insns.push_back(make_shared<align_insn>(sd->alignment));
					insns.push_back(make_shared<unary_insn>(unary_insn::kind::KIND_STK, temp_stk,
						make_shared<immediate_operand>(uint_type, (int) sd->size), ref_reg->register_type()));
					shared_ptr<register_operand> ref_reg_copy = make_shared<register_operand>(*ref_reg);
					ref_reg_copy->set_dereference(true);
					insns.push_back(make_shared<at_insn>(ak, temp, ref_reg_copy, ref_reg->register_type()));
					temp->set_dereference(true);
					return make_pair(insns, static_pointer_cast<operand>(temp));
				};

				auto handle_lhs = [&] (shared_ptr<basic_block> bb, int insn_loc, shared_ptr<operand> o) {
					vector<shared_ptr<insn>> winsns, rinsns;
					if (o->operand_kind() != operand::kind::KIND_REGISTER)
						return make_tuple(rinsns, winsns, o);
					shared_ptr<register_operand> reg = static_pointer_cast<register_operand>(o);
					check_and_initialize_register(reg);
					if (reg->virtual_register_number() != v)
						return make_tuple(rinsns, winsns, o);
					if (!reg->dereference()) {
						reg->set_dereference(true);
						return make_tuple(rinsns, winsns, static_pointer_cast<operand>(reg));
					}
					shared_ptr<register_operand> temp = bbs->get_middle_ir()->generate_next_register(reg->register_type()),
						temp_stk = make_shared<register_operand>(*temp),
						temp2 = make_shared<register_operand>(*temp);
					rinsns.push_back(make_shared<align_insn>(sd->alignment));
					rinsns.push_back(make_shared<unary_insn>(unary_insn::kind::KIND_STK, temp_stk,
						make_shared<immediate_operand>(uint_type, (int) sd->size), reg->register_type()));
					shared_ptr<register_operand> ref_reg_copy = make_shared<register_operand>(*ref_reg);
					temp2->set_dereference(true);
					winsns.push_back(make_shared<at_insn>(alk, ref_reg_copy, temp2, reg->register_type()));
					temp->set_dereference(true);
					return make_tuple(rinsns, winsns, static_pointer_cast<operand>(temp));
				};

				for (const auto& bb_counter : accesses_bbs) {
					shared_ptr<basic_block> bb = bbs->get_basic_block(bb_counter);
					for (const auto& insn_loc : accesses[bb_counter]) {
						shared_ptr<insn> i = bb->get_insn(insn_loc);
						switch (i->insn_kind()) {
						case insn::kind::KIND_GLOBAL_ARRAY:
						case insn::kind::KIND_FUNCTION:
						case insn::kind::KIND_PHI:
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							break;
						case insn::kind::KIND_ASM: {
							shared_ptr<asm_insn> ai = static_pointer_cast<asm_insn>(i);
							if (ai->asm_insn_kind() == asm_insn::kind::KIND_LA) {
								auto [insns_to_insert, op] = handle_rhs(bb, insn_loc, ai->la().second);
								if (op->operand_kind() != operand::kind::KIND_REGISTER)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								shared_ptr<register_operand> dst = static_pointer_cast<register_operand>(op);
								bb->set_insn(insn_loc, make_shared<asm_insn>(make_pair(ai->la().first, dst)));
								insert_read_insns(bb, insns_to_insert, insn_loc);
							}
						}
							break;
						case insn::kind::KIND_ALIGN:
						case insn::kind::KIND_LABEL:
						case insn::kind::KIND_JUMP:
							break;
						case insn::kind::KIND_CONDITIONAL: {
							shared_ptr<conditional_insn> ci = static_pointer_cast<conditional_insn>(i);
							auto [insns_to_insert, src] = handle_rhs(bb, insn_loc, ci->src());
							bb->set_insn(insn_loc, make_shared<conditional_insn>(src, ci->branch()));
							insert_read_insns(bb, insns_to_insert, insn_loc);
							
						}
							break;
						case insn::kind::KIND_ACCESS: {
							shared_ptr<access_insn> ai = static_pointer_cast<access_insn>(i);
							auto [lhs_rinsns, lhs_winsns, op] = handle_lhs(bb, insn_loc, ai->variable());
							bb->set_insn(insn_loc, make_shared<access_insn>(ai->symbol(), op));
							insert_write_insns(bb, lhs_winsns, insn_loc);
							insert_read_insns(bb, lhs_rinsns, insn_loc);
						}
							break;
						case insn::kind::KIND_AT: {
							shared_ptr<at_insn> ai = static_pointer_cast<at_insn>(i);
							auto [lhs_rinsns, lhs_winsns, dst] = handle_lhs(bb, insn_loc, ai->dst());
							auto [rinsns, src] = handle_rhs(bb, insn_loc, ai->src());
							shared_ptr<register_operand> dst_reg = nullptr, src_reg = nullptr;
							if (dst->operand_kind() != operand::kind::KIND_REGISTER)
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							if (src->operand_kind() != operand::kind::KIND_REGISTER)
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							dst_reg = static_pointer_cast<register_operand>(dst);
							src_reg = static_pointer_cast<register_operand>(src);
							rinsns.insert(rinsns.end(), lhs_rinsns.begin(), lhs_rinsns.end());
							insert_write_insns(bb, lhs_winsns, insn_loc);
							insert_read_insns(bb, rinsns, insn_loc);
							bb->set_insn(insn_loc, make_shared<at_insn>(ai->at_kind(), dst_reg, src_reg, ai->at_type()));
						}
							break;
						case insn::kind::KIND_BINARY: {
							shared_ptr<binary_insn> bi = static_pointer_cast<binary_insn>(i);
							auto [lhs_rinsns, lhs_winsns, dst] = handle_lhs(bb, insn_loc, bi->dst_operand());
							auto [rinsns1, src1] = handle_rhs(bb, insn_loc, bi->src1_operand());
							auto [rinsns2, src2] = handle_rhs(bb, insn_loc, bi->src2_operand());
							if (bi->is_variable_decl()) {
								bb->set_insn(insn_loc, make_shared<binary_insn>(bi->binary_expr_kind(), dst, src1, src2, bi->result_type(),
									bi->variable_decl()));
							}
							else {
								bb->set_insn(insn_loc, make_shared<binary_insn>(bi->binary_expr_kind(), dst, src1, src2,
									bi->result_type()));
							}
							vector<shared_ptr<insn>> rinsns = rinsns1;
							rinsns.insert(rinsns.end(), rinsns2.begin(), rinsns2.end());
							rinsns.insert(rinsns.end(), lhs_rinsns.begin(), lhs_rinsns.end());
							insert_write_insns(bb, lhs_winsns, insn_loc);
							insert_read_insns(bb, rinsns, insn_loc);
						}
							break;
						case insn::kind::KIND_UNARY: {
							shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
							vector<shared_ptr<insn>> rinsns, winsns;
							shared_ptr<operand> src = nullptr, dst = nullptr;
							if (ui->unary_expr_kind() == unary_insn::kind::KIND_STK
								|| ui->unary_expr_kind() == unary_insn::kind::KIND_RESV) {
								shared_ptr<operand> lhs = ui->dst_operand();
								if (lhs->operand_kind() == operand::kind::KIND_REGISTER) {
									shared_ptr<register_operand> reg = static_pointer_cast<register_operand>(lhs);
									if (reg->dereference()) {
										auto [temp_insns, temp_dst] = handle_rhs(bb, insn_loc, reg);
										rinsns.insert(rinsns.begin(), temp_insns.begin(), temp_insns.end());
										dst = temp_dst;
									}
									else
										dst = reg;
								}
								else
									dst = lhs;
							}
							else {
								auto [lhs_rinsns, lhs_winsns, temp_dst] = handle_lhs(bb, insn_loc, ui->dst_operand());
								rinsns.insert(rinsns.end(), lhs_rinsns.begin(), lhs_rinsns.end());
								winsns.insert(winsns.end(), lhs_winsns.begin(), lhs_winsns.end());
								dst = temp_dst;
							}
							auto [temp_insns, temp_src] = handle_rhs(bb, insn_loc, ui->src_operand());
							rinsns.insert(rinsns.end(), temp_insns.begin(), temp_insns.end());
							src = temp_src;
							if (dst == nullptr || src == nullptr)
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							if (ui->is_variable_decl()) {
								bb->set_insn(insn_loc, make_shared<unary_insn>(ui->unary_expr_kind(), dst, src, ui->result_type(),
											ui->variable_decl()));
							}
							else
								bb->set_insn(insn_loc, make_shared<unary_insn>(ui->unary_expr_kind(), dst, src, ui->result_type()));
							insert_write_insns(bb, winsns, insn_loc);
							insert_read_insns(bb, rinsns, insn_loc);
						}
							break;
						case insn::kind::KIND_VAR: {
							shared_ptr<var_insn> vi = static_pointer_cast<var_insn>(i);
							auto [rinsns, src] = handle_rhs(bb, insn_loc, vi->size());
							shared_ptr<register_operand> lhs = vi->dst();
							shared_ptr<register_operand> dst = lhs;
							if (lhs->dereference()) {
								auto [temp_insns, temp_dst] = handle_rhs(bb, insn_loc, lhs);
								rinsns.insert(rinsns.end(), temp_insns.begin(), temp_insns.end());
								if (temp_dst->operand_kind() != operand::kind::KIND_REGISTER)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								dst = static_pointer_cast<register_operand>(temp_dst);
							}
							bb->insert_insn(insn_loc, make_shared<var_insn>(dst, src));
							insert_read_insns(bb, rinsns, insn_loc);
						}
							break;
						case insn::kind::KIND_CALL: {
							shared_ptr<call_insn> ci = static_pointer_cast<call_insn>(i);
							auto [lhs_rinsns, lhs_winsns, dst] = handle_lhs(bb, insn_loc, ci->dest_operand());
							auto [rinsns, func] = handle_rhs(bb, insn_loc, ci->function_operand());
							vector<shared_ptr<operand>> args;
							for (shared_ptr<operand> arg : ci->argument_list()) {
								auto [temp_rinsns, temp_arg] = handle_rhs(bb, insn_loc, arg);
								args.push_back(temp_arg);
								rinsns.insert(rinsns.end(), temp_rinsns.begin(), temp_rinsns.end());
							}
							if (ci->is_variable())
								bb->set_insn(insn_loc, make_shared<call_insn>(ci->result_type(), dst, func, args,
									ci->variable_decl(), ci->called_function_type()));
							else
								bb->set_insn(insn_loc, make_shared<call_insn>(ci->result_type(), dst, func, args,
									ci->called_function_type()));
							rinsns.insert(rinsns.end(), lhs_rinsns.begin(), lhs_rinsns.end());
							insert_write_insns(bb, lhs_winsns, insn_loc);
							insert_read_insns(bb, rinsns, insn_loc);
						}
							break;
						case insn::kind::KIND_MEMCPY: {
							shared_ptr<memcpy_insn> mi = static_pointer_cast<memcpy_insn>(i);
							auto [rinsns, dst] = handle_rhs(bb, insn_loc, mi->destination());
							auto [rinsns2, src] = handle_rhs(bb, insn_loc, mi->source());
							rinsns.insert(rinsns.end(), rinsns2.begin(), rinsns2.end());
							bb->set_insn(insn_loc, make_shared<memcpy_insn>(dst, src, mi->size(), mi->result_type()));
							insert_read_insns(bb, rinsns, insn_loc);
						}
							break;
						case insn::kind::KIND_RETURN: {
							shared_ptr<return_insn> ri = static_pointer_cast<return_insn>(i);
							vector<shared_ptr<insn>> rinsns;
							shared_ptr<operand> src = ri->expr();
							if (ri->return_kind() == return_insn::kind::KIND_REGULAR) {
								auto [temp_insns, temp_src] = handle_rhs(bb, insn_loc, ri->expr());
								src = temp_src;
								rinsns = temp_insns;
							}
							bb->set_insn(insn_loc, make_shared<return_insn>(src));
							insert_read_insns(bb, rinsns, insn_loc);
						}
							break;
						case insn::kind::KIND_EXT: {
							shared_ptr<ext_insn> ei = static_pointer_cast<ext_insn>(i);
							auto [lhs_rinsns, lhs_winsns, lhs] = handle_lhs(bb, insn_loc, ei->lhs());
							auto [rinsns, rhs] = handle_rhs(bb, insn_loc, ei->rhs());
							bb->set_insn(insn_loc, make_shared<ext_insn>(ei->ext_kind(), lhs, rhs, ei->from(), ei->to(), ei->result_type()));
							rinsns.insert(rinsns.end(), lhs_rinsns.begin(), lhs_rinsns.end());
							insert_write_insns(bb, lhs_winsns, insn_loc);
							insert_read_insns(bb, rinsns, insn_loc);
						}
							break;
						case insn::kind::KIND_TRUNC: {
							shared_ptr<trunc_insn> ti = static_pointer_cast<trunc_insn>(i);
							auto [lhs_rinsns, lhs_winsns, lhs] = handle_lhs(bb, insn_loc, ti->lhs());
							auto [rinsns, rhs] = handle_rhs(bb, insn_loc, ti->rhs());
							bb->set_insn(insn_loc, make_shared<trunc_insn>(lhs, rhs, ti->from(), ti->to(), ti->result_type()));
							rinsns.insert(rinsns.end(), lhs_rinsns.begin(), lhs_rinsns.end());
							insert_write_insns(bb, lhs_winsns, insn_loc);
							insert_read_insns(bb, rinsns, insn_loc);
						}
							break;
						default:
							break;
						}
					}
				}

				if (!accesses_bbs.empty() && var_locs.empty()) {
					int bb_num = accesses_bbs[0];
					if (sd == nullptr || ref_reg == nullptr)
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<basic_block> bb = bbs->get_basic_block(bb_num);
					shared_ptr<register_operand> ref_reg_copy = make_shared<register_operand>(*ref_reg);
					bb->insert_insn(alloc_loc, make_shared<unary_insn>(unary_insn::kind::KIND_STK, ref_reg_copy,
						make_shared<immediate_operand>(uint_type, (int) sd->size), ref_reg->register_type()));
					bb->insert_insn(alloc_loc, make_shared<align_insn>(sd->alignment));
				}
			}

			if (dl >= pass_manager::debug_level::KIND_DEBUG) {
				print_liveness_data(bbs, ld);
				cout << "=========================\n";
			}
			return coloring;
		}
	}
}
