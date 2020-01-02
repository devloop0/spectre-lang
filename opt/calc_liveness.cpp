#include "opt/calc_liveness.hpp"
#include "opt/mem2reg.hpp"
#include "opt/cvt2ssa.hpp"

#include <algorithm>
#include <iostream>

using std::move;
using std::make_shared;
using std::static_pointer_cast;
using std::make_pair;
using std::copy_if;
using std::inserter;
using std::copy;
using std::cout;
using std::endl;

namespace spectre {
	namespace opt {

		at_insn::at_insn(kind k, shared_ptr<register_operand> d, shared_ptr<register_operand> s,
			shared_ptr<type> at) : insn(insn::kind::KIND_AT), _at_type(at),
			_dst(d), _src(s), _at_kind(k) {

		}

		at_insn::~at_insn() {

		}

		shared_ptr<register_operand> at_insn::dst() {
			return _dst;
		}

		shared_ptr<register_operand> at_insn::src() {
			return _src;
		}

		at_insn::kind at_insn::at_kind() {
			return _at_kind;
		}

		shared_ptr<type> at_insn::at_type() {
			return _at_type;
		}

		string at_insn::to_string() {
			string a;
			switch (_at_kind) {
			case at_insn::kind::KIND_DAT:
				a = "dat";
				break;
			case at_insn::kind::KIND_LHS_DAT:
				a = "lhs_dat";
				break;
			case at_insn::kind::KIND_FAT:
				a = "fat";
				break;
			case at_insn::kind::KIND_LHS_FAT:
				a = "lhs_fat";
				break;
			case at_insn::kind::KIND_LAT:
				a = "lat";
				break;
			case at_insn::kind::KIND_LHS_LAT:
				a = "lhs_lat";
				break;
			case at_insn::kind::KIND_IAT:
				a = "iat";
				break;
			case at_insn::kind::KIND_LHS_IAT:
				a = "lhs_iat";
				break;
			case at_insn::kind::KIND_SAT:
				a = "sat";
				break;
			case at_insn::kind::KIND_LHS_SAT:
				a = "lhs_sat";
				break;
			case at_insn::kind::KIND_CAT:
				a = "cat";
				break;
			case at_insn::kind::KIND_LHS_CAT:
				a = "cat";
				break;
			}

			return a + " " + _dst->to_string() + ", " + _src->to_string();
		}

		void at_insn::deduplicate() {
			DEDUP(_dst);
			DEDUP(_src);
		}

		def_use_data::def_use_data(unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> d,
			unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> u,
			unordered_map<int, shared_ptr<type>> r) : def(d), use(u), reg_num_2_type(r) {
		}

		def_use_data::~def_use_data() {

		}

		liveness_data::liveness_data(unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> i,
			unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> o) : in(i), out(o) {

		}

		liveness_data::~liveness_data() {

		}

		shared_ptr<def_use_data> calc_def_use(shared_ptr<basic_blocks> bbs) {
			unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> def, use;
			unordered_map<int, shared_ptr<type>> reg_num_2_type;
			unordered_set<int> stk_resv_location;

			auto handle_rhs = [&use, &stk_resv_location, &reg_num_2_type] (shared_ptr<operand> o, int bb_counter, int insn_counter) {
				if (o->operand_kind() == operand::kind::KIND_REGISTER) {
					shared_ptr<register_operand> reg = static_pointer_cast<register_operand>(o);
					if (!reg->dereference())
						reg_num_2_type[reg->virtual_register_number()] = reg->register_type();
					if (stk_resv_location.find(reg->virtual_register_number()) != stk_resv_location.end())
						return;
					use[make_pair(bb_counter, insn_counter)].insert(reg->virtual_register_number());
				}
			};

			auto handle_lhs = [&def, &use, &stk_resv_location, &reg_num_2_type] (shared_ptr<operand> o, int bb_counter, int insn_counter) {
				if (o->operand_kind() == operand::kind::KIND_REGISTER) {
					shared_ptr<register_operand> reg = static_pointer_cast<register_operand>(o);
					if (!reg->dereference())
						reg_num_2_type[reg->virtual_register_number()] = reg->register_type();
					if (stk_resv_location.find(reg->virtual_register_number()) != stk_resv_location.end())
						return;
					if (reg->dereference())
						use[make_pair(bb_counter, insn_counter)].insert(reg->virtual_register_number());
					else
						def[make_pair(bb_counter, insn_counter)].insert(reg->virtual_register_number());
				}
			};

			int bb_counter = 0;
			for (shared_ptr<basic_block> bb : bbs->get_basic_blocks()) {
				int insn_counter = 0;
				for (shared_ptr<insn> i : bb->get_insns(bbs)) {
					switch (i->insn_kind()) {
					case insn::kind::KIND_GLOBAL_ARRAY: {
						shared_ptr<global_array_insn> gai = static_pointer_cast<global_array_insn>(i);
						for (shared_ptr<operand> o : gai->data()) {
							if (o->operand_kind() == operand::kind::KIND_REGISTER) {
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return nullptr;
							}
						}
					}
						break;
					case insn::kind::KIND_FUNCTION:
					case insn::kind::KIND_PHI:
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						return nullptr;
						break;
					case insn::kind::KIND_ASM: {
						shared_ptr<asm_insn> ai = static_pointer_cast<asm_insn>(i);
						if (ai->asm_insn_kind() == asm_insn::kind::KIND_LA)
							handle_rhs(ai->la().second, bb_counter, insn_counter);
					}
						break;
					case insn::kind::KIND_ALIGN:
					case insn::kind::KIND_LABEL:
					case insn::kind::KIND_JUMP:
						break;
					case insn::kind::KIND_CONDITIONAL: {
						shared_ptr<conditional_insn> ci = static_pointer_cast<conditional_insn>(i);
						handle_rhs(ci->src(), bb_counter, insn_counter);
					}
						break;
					case insn::kind::KIND_ACCESS: {
						shared_ptr<access_insn> ai = static_pointer_cast<access_insn>(i);
						shared_ptr<operand> lhs_op = ai->variable();
						if (lhs_op->operand_kind() == operand::kind::KIND_REGISTER) {
							shared_ptr<register_operand> reg_op = static_pointer_cast<register_operand>(lhs_op);
							if (reg_op->dereference())
								handle_lhs(ai->variable(), bb_counter, insn_counter);
							else {
								reg_num_2_type[reg_op->virtual_register_number()] = reg_op->register_type();
								stk_resv_location.insert(reg_op->virtual_register_number());
							}
						}
						else
							handle_lhs(ai->variable(), bb_counter, insn_counter);
					}
						break;
					case insn::kind::KIND_BINARY: {
						shared_ptr<binary_insn> bi = static_pointer_cast<binary_insn>(i);
						handle_rhs(bi->src1_operand(), bb_counter, insn_counter);
						handle_rhs(bi->src2_operand(), bb_counter, insn_counter);
						handle_lhs(bi->dst_operand(), bb_counter, insn_counter);
					}
						break;
					case insn::kind::KIND_AT: {
						shared_ptr<at_insn> ai = static_pointer_cast<at_insn>(i);
						handle_rhs(ai->src(), bb_counter, insn_counter);
						handle_lhs(ai->dst(), bb_counter, insn_counter);
					};
						break;
					case insn::kind::KIND_UNARY: {
						shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
						handle_rhs(ui->src_operand(), bb_counter, insn_counter);
						if (ui->unary_expr_kind() == unary_insn::kind::KIND_STK
							|| ui->unary_expr_kind() == unary_insn::kind::KIND_RESV) {
							shared_ptr<operand> lhs = ui->dst_operand();
							if (lhs->operand_kind() == operand::kind::KIND_REGISTER) {
								shared_ptr<register_operand> reg = static_pointer_cast<register_operand>(lhs);
								if (reg->dereference())
									handle_rhs(reg, bb_counter, insn_counter);
								else {
									reg_num_2_type[reg->virtual_register_number()] = reg->register_type();
									stk_resv_location.insert(reg->virtual_register_number());
								}
							}
						}
						else
							handle_lhs(ui->dst_operand(), bb_counter, insn_counter);
					}
						break;
					case insn::kind::KIND_VAR: {
						shared_ptr<var_insn> vi = static_pointer_cast<var_insn>(i);
						handle_rhs(vi->size(), bb_counter, insn_counter);
						shared_ptr<register_operand> lhs = vi->dst();
						if (lhs->dereference())
							handle_rhs(lhs, bb_counter, insn_counter);
						else
							reg_num_2_type[lhs->virtual_register_number()] = lhs->register_type();
					}
						break;
					case insn::kind::KIND_CALL: {
						shared_ptr<call_insn> ci = static_pointer_cast<call_insn>(i);
						handle_rhs(ci->function_operand(), bb_counter, insn_counter);
						for (shared_ptr<operand> arg : ci->argument_list())
							handle_rhs(arg, bb_counter, insn_counter);
						handle_lhs(ci->dest_operand(), bb_counter, insn_counter);
					}
						break;
					case insn::kind::KIND_MEMCPY: {
						shared_ptr<memcpy_insn> mi = static_pointer_cast<memcpy_insn>(i);
						handle_rhs(mi->source(), bb_counter, insn_counter);
						handle_rhs(mi->destination(), bb_counter, insn_counter);
					}
						break;
					case insn::kind::KIND_RETURN: {
						shared_ptr<return_insn> ri = static_pointer_cast<return_insn>(i);
						if (ri->return_kind() == return_insn::kind::KIND_REGULAR)
							handle_rhs(ri->expr(), bb_counter, insn_counter);
					}
						break;
					case insn::kind::KIND_EXT: {
						shared_ptr<ext_insn> ei = static_pointer_cast<ext_insn>(i);
						handle_rhs(ei->rhs(), bb_counter, insn_counter);
						handle_lhs(ei->lhs(), bb_counter, insn_counter);
					}
						break;
					case insn::kind::KIND_TRUNC: {
						shared_ptr<trunc_insn> ti = static_pointer_cast<trunc_insn>(i);
						handle_rhs(ti->rhs(), bb_counter, insn_counter);
						handle_lhs(ti->lhs(), bb_counter, insn_counter);
					}
						break;
					default:
						break;
					}
					insn_counter++;
				}
				bb_counter++;
			}
			return make_shared<def_use_data>(move(def), move(use), move(reg_num_2_type));
		}

		shared_ptr<liveness_data> liveness_analysis(shared_ptr<basic_blocks> bbs, shared_ptr<def_use_data> dud) {
			unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> in, out;
			unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> in_prev, out_prev;

			unordered_set<pair<int, int>, pair_hasher> nodes;
			for (int bb_counter = 0; bb_counter < bbs->get_basic_blocks().size(); bb_counter++) {
				shared_ptr<basic_block> bb = bbs->get_basic_block(bb_counter);
				for (int insn_counter = 0; insn_counter < bb->get_insns(bbs).size(); insn_counter++) {
					in[make_pair(bb_counter, insn_counter)] = unordered_set<int>{};
					out[make_pair(bb_counter, insn_counter)] = unordered_set<int>{};
					nodes.insert(make_pair(bb_counter, insn_counter));
				}
			}

			auto set_difference = [] (const unordered_set<int>& lhs, const unordered_set<int>& rhs) {
				unordered_set<int> ret;
				copy_if(lhs.begin(), lhs.end(), inserter(ret, ret.end()),
					[&rhs] (const auto& e) { return rhs.find(e) == rhs.end(); });
				return ret;
			};

			auto set_union = [] (unordered_set<int>& results, const unordered_set<int>& lhs, const unordered_set<int>& rhs) {
				for (const auto& e : lhs)
					results.insert(e);
				for (const auto& e : rhs)
					results.insert(e);
			};

			while (in_prev != in || out_prev != out) {
				unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> in_temp = move(in),
					out_temp = move(out);
				for (const auto&[bb_counter, insn_counter] : nodes) {
					const auto& n = make_pair(bb_counter, insn_counter);
					in[n].clear();
					set_union(in[n], dud->use[n], set_difference(out_temp[n], dud->def[n]));

					const vector<shared_ptr<insn>>& insns = bbs->get_basic_block(bb_counter)->get_insns(bbs);
					unordered_set<pair<int, int>, pair_hasher> succ;
					if (insns.empty() || insn_counter + 1 == insns.size()) {
						unordered_set<int> bb_succ = bbs->cfg().adjacent(bb_counter);
						for (const auto& bb : bb_succ) {
							const vector<shared_ptr<insn>>& bb_insns = bbs->get_basic_block(bb)->get_insns(bbs);
							if (!bb_insns.empty())
								succ.insert(make_pair(bb, 0));
						}
					}
					else
						succ.insert(make_pair(bb_counter, insn_counter + 1));
					out[n].clear();
					for (const auto& s : succ) {
						for (const auto& e : in_temp[s])
							out[n].insert(e);
					}
				}
				in_prev = std::move(in_temp);
				out_prev = std::move(out_temp);
			}
			return make_shared<liveness_data>(move(in), move(out));
		}

		void print_liveness_data(shared_ptr<basic_blocks> bbs, shared_ptr<liveness_data> ld) {
			auto print_set = [] (const unordered_set<int>& s) {
				if (s.empty()) {
					cout << "{}";
					return;
				}
				cout << "{ ";
				int counter = 0;
				for (const auto& e : s) {
					cout << e;
					if (counter != s.size() - 1)
						cout << ", ";
					counter++;
				}
				cout << " }";
			};

			int total_counter = 0;
			int bb_counter = 0;
			for (shared_ptr<basic_block> bb : bbs->get_basic_blocks()) {
				int insn_counter = 0;
				for (shared_ptr<insn> i : bb->get_insns(bbs)) {
					cout << total_counter << ": ";
					print_set(ld->in[make_pair(bb_counter, insn_counter)]);
					cout << " | " << i->to_string() << endl;
					insn_counter++, total_counter++;
				}
				bb_counter++;
			}
		}
	}
}
