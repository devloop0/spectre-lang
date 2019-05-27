#include "pass.hpp"

namespace spectre {
	namespace opt {

		pass::pass(string n, string desc, int i, unordered_set<string> d) : _description(desc), _pass_id(i), _dependencies(d), _name(n) {

		}

		pass::~pass() {

		}

		string pass::description() {
			return _description;
		}

		int pass::pass_id() {
			return _pass_id;
		}

		shared_ptr<basic_blocks> pass::basic_blocks_context() {
			return _basic_blocks_ctx;
		}

		unordered_set<string> pass::dependencies() {
			return _dependencies;
		}

		string pass::name() {
			return _name;
		}

		pass_builder::pass_builder(shared_ptr<pass_manager> pm) : _counter(0), _pm(pm) {

		}

		pass_builder::~pass_builder() {

		}
	}
}
