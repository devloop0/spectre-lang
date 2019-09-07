#ifndef SPECTRE_OPT_MEM2REG_HPP
#define SPECTRE_OPT_MEM2REG_HPP

#include <memory>
#include <unordered_set>

#include "middle_ir.hpp"
#include "basic_block.hpp"
#include "pass.hpp"
#include "pass_manager.hpp"
#include "graph.hpp"

using namespace spectre::ir;
using std::unordered_set;

namespace spectre {
	namespace ir {
		class var_insn : public insn {
		private:
			shared_ptr<register_operand> _dst;
			shared_ptr<operand> _size;
		public:
			var_insn(shared_ptr<register_operand> d, shared_ptr<operand> s);
			~var_insn();
			shared_ptr<register_operand> dst();
			shared_ptr<operand> size();
			string to_string() override;
			void deduplicate() override;
		};
	}

	namespace opt {

		unordered_set<int> mem2reg_qualifying_registers(shared_ptr<basic_blocks> bbs);
		shared_ptr<basic_blocks> replace_qualifying_registers(shared_ptr<basic_blocks> bbs, unordered_set<int> qualifying_registers);

		class mem2reg_pass : public pass {
		public:
			mem2reg_pass(shared_ptr<pass_manager> pm);
			~mem2reg_pass();
			shared_ptr<basic_blocks> run_pass(shared_ptr<basic_blocks> bbs) override;
		};
	}
}

#endif
