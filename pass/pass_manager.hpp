#ifndef SPECTRE_OPT_PASS_MANAGER_HPP
#define SPECTRE_OPT_PASS_MANAGER_HPP

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "ir/basic_block.hpp"
#include "pass/pass.hpp"
#include "ir/graph.hpp"

using std::unordered_map;
using std::unordered_set;
using std::enable_shared_from_this;
using namespace spectre::ir;

namespace spectre {
	namespace opt {

		void simplify_cfg(shared_ptr<basic_blocks> bbs);
		shared_ptr<basic_blocks> insns_in_bb_2_straight_line(shared_ptr<basic_blocks> bbs);
		void place_insns_in_bb(shared_ptr<basic_blocks> bbs);
		void remove_unreachable_bbs(shared_ptr<basic_blocks> bbs);

		class pass_manager : public enable_shared_from_this<pass_manager> {
		public:
			enum class debug_level : int {
				KIND_NONDEBUG, KIND_DEBUG, KIND_DEBUG_VERBOSE
			};
		private:
			directed_graph<int> _pass_dependency_graph;
			int _pass_id_counter;
			unordered_map<int, shared_ptr<pass>> _pass_id_2_pass;
			unordered_map<string, shared_ptr<pass>> _pass_name_2_pass;
			shared_ptr<pass_builder> _pass_builder;
			debug_level _debug;
		public:
			pass_manager(debug_level d = debug_level::KIND_NONDEBUG);
			~pass_manager();
			directed_graph<int>& pass_dependency_graph();
			int current_pass_id();
			int next_pass_id();
			void add_pass(shared_ptr<pass> p);
			vector<shared_ptr<pass>> passes();
			shared_ptr<pass> lookup_pass_by_id(int j);
			shared_ptr<pass> lookup_pass_by_name(string n);
			shared_ptr<basic_blocks> run_passes(shared_ptr<middle_ir> mi);
			void set_debug(debug_level d);
			debug_level debug();
			void report_internal(string msg, string fn, int ln, string fl);

			template<class T> shared_ptr<T> build_pass() {
				if (_pass_builder == nullptr)
					_pass_builder = make_shared<pass_builder>(shared_from_this());
				return _pass_builder->template build_pass<T>();
			}

			template<class T> shared_ptr<T> build_pass(unordered_set<string> deps) {
				if (_pass_builder == nullptr)
					_pass_builder = make_shared<pass_builder>(shared_from_this());
				return _pass_builder->template build_pass<T>(deps);
			}
		};
	}
}

#endif
