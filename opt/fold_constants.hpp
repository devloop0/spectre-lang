#ifndef SPECTRE_OPT_FOLD_CONSTANTS_HPP
#define SPECTRE_OPT_FOLD_CONSTANTS_HPP

#include "ir/basic_block.hpp"
#include "ir/middle_ir.hpp"
#include "pass/pass_manager.hpp"
#include "pass/pass.hpp"
#include "ir/graph.hpp"

#include <memory>
#include <variant>

using namespace spectre::ir;
using std::shared_ptr;
using std::variant;

namespace spectre {
	namespace opt {
		shared_ptr<basic_blocks> insns_in_bb_2_straight_line(shared_ptr<basic_blocks> bbs);

		class constant_data {
		public:
			using immediate_data = variant<char, short, int, long, float, double, string>;
		private:
			bool _constant;
			immediate_data _immediate;
		public:
			constant_data(immediate_data v);
			constant_data(char c);
			constant_data(short s);
			constant_data(int i);
			constant_data(long l);
			constant_data(float f);
			constant_data(double d);
			constant_data(string s);
			constant_data();
			bool constant();
			immediate_data immediate();
		};

		unordered_map<int, unordered_map<int, shared_ptr<constant_data>>> fold_constants(shared_ptr<basic_blocks>& bbs);

		class fold_constants_pass : public pass {
		private:
			unordered_map<int, unordered_map<int, shared_ptr<constant_data>>> _data;
		public:
			fold_constants_pass(shared_ptr<pass_manager> pm);
			fold_constants_pass(shared_ptr<pass_manager> pm, int counter, unordered_set<string> deps);
			~fold_constants_pass();
			shared_ptr<basic_blocks> run_pass(shared_ptr<basic_blocks> bbs) override;
			unordered_map<int, unordered_map<int, shared_ptr<constant_data>>> data();
		};

		void propagate_type_changes(shared_ptr<basic_blocks> bbs, unordered_map<int, shared_ptr<type>> reg_2_type);
	}
}

#endif
