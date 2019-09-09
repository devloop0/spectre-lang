#ifndef SPECTRE_OPT_CVT2SSA_HPP
#define SPECTRE_OPT_CVT2SSA_HPP

#include <unordered_map>
#include <unordered_set>
#include <stack>

#include "pass/pass.hpp"
#include "pass/pass_manager.hpp"
#include "ir/basic_block.hpp"
#include "ir/middle_ir.hpp"

using std::unordered_map;
using std::unordered_set;
using std::stack;

using namespace spectre::ir;

namespace spectre {
	namespace ir {
		class phi_insn : public insn {
		public:
			struct pred_data {
				shared_ptr<register_operand> reg_pred;
				int bb_pred = -1;
			};
		private:
			shared_ptr<register_operand> _dst;
			vector<shared_ptr<pred_data>> _pred_data_list;
		public:
			phi_insn(shared_ptr<register_operand> d, vector<shared_ptr<pred_data>> pdl);
			phi_insn(shared_ptr<register_operand> d);
			~phi_insn();
			vector<shared_ptr<pred_data>> pred_data_list();
			void add_pred(shared_ptr<pred_data> pd);
			shared_ptr<pred_data> get_pred(int j);
			void set_pred(int j, shared_ptr<pred_data> pd);
			void remove_pred(int j);
			int num_preds();
			shared_ptr<register_operand> dst();
			string to_string() override;
			void deduplicate() override;
		};
	}

	namespace opt {

		unordered_map<int, unordered_set<int>> compute_dominators(shared_ptr<basic_blocks> bbs, const directed_graph<int>& d);
		unordered_map<int, unordered_set<int>> compute_strict_dominators(shared_ptr<basic_blocks> bbs, const unordered_map<int, unordered_set<int>>& doms);
		unordered_map<int, int> compute_immediate_dominators(shared_ptr<basic_blocks> bbs, const unordered_map<int, unordered_set<int>>& doms);
		unordered_map<int, unordered_set<int>> compute_dominance_frontier(shared_ptr<basic_blocks> bbs, const directed_graph<int>& d,
			const unordered_map<int, unordered_set<int>>& doms);

		unordered_map<int, pair<shared_ptr<register_operand>, vector<int>>> all_register_assignment_indices(shared_ptr<basic_blocks> bbs);
		void insert_phi_insns(shared_ptr<basic_blocks> bbs);

		struct rename_data {
			unordered_map<int, stack<int>> reg_names;
			unordered_set<int> visited;
		};

		pair<unordered_set<int>, unordered_set<int>> function_and_global_headers(shared_ptr<basic_blocks> bbs);

		void bb_rename_variables(shared_ptr<basic_blocks> bbs, int j, shared_ptr<rename_data> rd, bool global);
		void rename_variables(shared_ptr<basic_blocks> bbs);
		void clean_up_phi_insns(shared_ptr<basic_blocks> bbs);
		void ssa_sanity_check(shared_ptr<basic_blocks> bbs);

		void print_bbs_dominance_data(shared_ptr<basic_blocks> bbs);

		class cvt2ssa_pass : public pass {
		private:
			pass_manager::debug_level _debug_ctx;
		public:
			cvt2ssa_pass(shared_ptr<pass_manager> pm);
			~cvt2ssa_pass();
			shared_ptr<basic_blocks> run_pass(shared_ptr<basic_blocks> bbs) override;
		};
	}
}

#endif
