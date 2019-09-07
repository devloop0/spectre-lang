#include "check_pass.hpp"

#include <algorithm>

using std::to_string;

namespace spectre {
	namespace opt {

		check_pass::check_pass(shared_ptr<pass_manager> pm, string name, string desc,
				function<void(shared_ptr<basic_blocks>&)> c, unordered_set<string> deps) :
			pass(name, desc, pm->next_pass_id(), deps), _check_function(c) {

		}

		check_pass::check_pass(shared_ptr<pass_manager> pm, string name, string desc, int counter,
				function<void(shared_ptr<basic_blocks>&)> c, unordered_set<string> deps) :
			pass(name + to_string(counter), desc, pm->next_pass_id(), deps), _check_function(c) {

		}
		check_pass::~check_pass() {

		}

		shared_ptr<basic_blocks> check_pass::run_pass(shared_ptr<basic_blocks> bbs) {
			_check_function(bbs);
			return bbs;
		}
	}
}
