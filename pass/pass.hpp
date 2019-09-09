#ifndef SPECTRE_OPT_PASS_HPP
#define SPECTRE_OPT_PASS_HPP

#include <unordered_set>
#include "ir/basic_block.hpp"
#include "ir/middle_ir.hpp"
#include "ir/graph.hpp"

using std::unordered_set;
using namespace spectre::ir;

namespace spectre {
	namespace opt {

		class pass_manager;

		class pass {
		private:
			string _name, _description;
			int _pass_id;
			shared_ptr<basic_blocks> _basic_blocks_ctx;
			unordered_set<string> _dependencies;
		public:
			pass(string n, string desc, int i, unordered_set<string> d);
			~pass();
			virtual shared_ptr<basic_blocks> run_pass(shared_ptr<basic_blocks> bbs) = 0;
			string description();
			int pass_id();
			shared_ptr<basic_blocks> basic_blocks_context();
			unordered_set<string> dependencies();
			string name();
		};

		class pass_builder {
		private:
			int _counter;
			shared_ptr<pass_manager> _pm;
		public:
			pass_builder(shared_ptr<pass_manager> pm);
			~pass_builder();

			template<class T> shared_ptr<T> build_pass(unordered_set<string> deps) {
				return make_shared<T>(_pm, _counter++, deps);
			}

			template<class T> shared_ptr<T> build_pass() {
				return make_shared<T>(_pm, _counter++, unordered_set<string>{});
			}
		};
	}
}

#endif
