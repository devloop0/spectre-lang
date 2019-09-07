#ifndef SPECTRE_OPT_CHECK_PASS_HPP
#define SPECTRE_OPT_CHECK_PASS_HPP

#include "pass_manager.hpp"
#include "pass.hpp"
#include "basic_block.hpp"
#include "middle_ir.hpp"

#include <functional>
#include <memory>
#include <string>
#include <unordered_set>

using namespace spectre::ir;

using std::function;
using std::string;
using std::unordered_set;
using std::shared_ptr;

namespace spectre {
	namespace opt {

		class check_pass : public pass {
		private:
			function<void(shared_ptr<basic_blocks>&)> _check_function;
		public:
			check_pass(shared_ptr<pass_manager> pm, string name, string desc,
					function<void(shared_ptr<basic_blocks>&)> c, unordered_set<string> deps);
			check_pass(shared_ptr<pass_manager> pm, string name, string desc, int counter,
					function<void(shared_ptr<basic_blocks>&)> c, unordered_set<string> deps);
			~check_pass();
			shared_ptr<basic_blocks> run_pass(shared_ptr<basic_blocks> bbs) override;
		};
	}
}

#endif
