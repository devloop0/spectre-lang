#include "cvt_from_ssa.hpp"

#include "cvt2ssa.hpp"

#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>

using std::static_pointer_cast;
using std::sort;
using std::vector;
using std::to_string;
using std::greater;
using std::unique;

namespace spectre {
	namespace opt {

		void backpropagate_phi_insns(shared_ptr<basic_blocks> bbs) {
			shared_ptr<middle_ir> mi = bbs->get_middle_ir();
			for(shared_ptr<basic_block> bb : bbs->get_basic_blocks()) {
				vector<int> to_remove;
				int counter = 0;
				for(shared_ptr<insn> i : bb->get_insns(bbs)) {
					if (i->insn_kind() == insn::kind::KIND_PHI) {
						shared_ptr<phi_insn> pi = static_pointer_cast<phi_insn>(i);
						for (shared_ptr<phi_insn::pred_data> pd : pi->pred_data_list()) {
							if (pd->reg_pred == nullptr)
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							if (pd->bb_pred < 0 || pd->bb_pred >= bbs->get_basic_blocks().size())
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							shared_ptr<basic_block> pred_bb = bbs->get_basic_block(pd->bb_pred);
							shared_ptr<register_operand> dst_copy = make_shared<register_operand>(*pi->dst()),
								src_copy = make_shared<register_operand>(*pd->reg_pred);
							shared_ptr<type> src_reg_type = pd->reg_pred->register_type(),
								dst_reg_type = dst_copy->register_type();

							unary_insn::kind uop = unary_insn::kind::KIND_NONE;
							shared_ptr<size_data> sd = nullptr;
							bool is_struct = false;
							if (src_reg_type == nullptr || dst_reg_type == nullptr)
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							else if (is_double_type(mi, dst_reg_type))
								uop = unary_insn::kind::KIND_DMOV;
							else if (is_float_type(mi, dst_reg_type))
								uop = unary_insn::kind::KIND_FMOV;
							else if (is_long_type(mi, dst_reg_type))
								uop = unary_insn::kind::KIND_LMOV;
							else if (is_int_type(mi, dst_reg_type) || is_pointer_type(mi, dst_reg_type))
								uop = unary_insn::kind::KIND_IMOV;
							else if (is_short_type(mi, dst_reg_type))
								uop = unary_insn::kind::KIND_SMOV;
							else if (is_char_type(mi, dst_reg_type) || is_void_type(mi, dst_reg_type)
								|| is_byte_type(mi, dst_reg_type) || is_bool_type(mi, dst_reg_type))
								uop = unary_insn::kind::KIND_CMOV;
							else if (is_struct_type(mi, dst_reg_type)) {
								sd = spectre_sizeof(mi, dst_reg_type);
								is_struct = true;
							}
							else
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);

							shared_ptr<operand> casted_src = ir_cast(mi, src_copy, src_reg_type, dst_reg_type);
							shared_ptr<insn> to_insert = nullptr;
							if (uop != unary_insn::kind::KIND_NONE)
								to_insert = make_shared<unary_insn>(uop, dst_copy, casted_src, dst_reg_type);
							else if (is_struct)
								to_insert = make_shared<unary_insn>(unary_insn::kind::KIND_IMOV, dst_copy, casted_src, dst_reg_type);
							else
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);

							if (pred_bb->get_insns(bbs).empty())
								pred_bb->add_insn(to_insert);
							else {
								int pos = pred_bb->get_insns(bbs).size() - 1;
								shared_ptr<insn> last_insn = pred_bb->get_insns(bbs)[pos];
								if (last_insn->insn_kind() == insn::kind::KIND_JUMP || last_insn->insn_kind() == insn::kind::KIND_CONDITIONAL
									|| last_insn->insn_kind() == insn::kind::KIND_CALL || last_insn->insn_kind() == insn::kind::KIND_RETURN)
									pred_bb->insert_insn(pos, to_insert);
								else
									pred_bb->add_insn(to_insert);
							}
						}
						to_remove.push_back(counter);
					}
					counter++;
				}

				sort(to_remove.begin(), to_remove.end(), greater{});
				unique(to_remove.begin(), to_remove.end());

				for (int index : to_remove)
					bb->remove_insn(index);
			}
		}

		cvt_from_ssa_pass::cvt_from_ssa_pass(shared_ptr<pass_manager> pm) : pass("cvt_from_ssa",
			"Converts out of SSA form.", pm->next_pass_id(), unordered_set<string>{ "cvt2ssa", "insns_in_bb" }) {

		}

		cvt_from_ssa_pass::cvt_from_ssa_pass(shared_ptr<pass_manager> pm, int counter, unordered_set<string> deps) :
			pass("cvt_from_ssa" + std::to_string(counter), "Converts out of SSA form.", pm->next_pass_id(),
				(deps.insert("cvt2ssa"), deps.insert("insns_in_bb"), deps)) {

		}

		cvt_from_ssa_pass::~cvt_from_ssa_pass() {

		}

		shared_ptr<basic_blocks> cvt_from_ssa_pass::run_pass(shared_ptr<basic_blocks> bbs) {
			backpropagate_phi_insns(bbs);
			return bbs;
		}
	}
}
