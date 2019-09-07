#include "cycle_pass.hpp"

#include <algorithm>
#include <iostream>

using std::to_string;
using std::move;
using std::cout;
using std::endl;

namespace spectre {
	namespace opt {


		cycle_pass::cycle_pass(shared_ptr<pass_manager> pm, string name, string desc,
			unordered_set<string> deps, int l, vector<shared_ptr<pass>> p) : _passes(p), pass(name, desc,
				pm->next_pass_id(), deps), _limit(l), _debug_ctx(pm->debug()) {

		}

		cycle_pass::cycle_pass(shared_ptr<pass_manager> pm, string name, string desc,
			int l, vector<shared_ptr<pass>> p, int counter, unordered_set<string> deps) :
			pass(name + to_string(counter), desc, pm->next_pass_id(), deps), _passes(p),
			_limit(l), _debug_ctx(pm->debug()) {

		}

		cycle_pass::~cycle_pass() {

		}

		shared_ptr<basic_blocks> cycle_pass::run_pass(shared_ptr<basic_blocks> bbs) {
			auto get_sizes = [&bbs] () {
				vector<int> sizes;
				for (shared_ptr<basic_block> bb : bbs->get_basic_blocks())
					sizes.push_back(bb->get_insns(bbs).size());
				return sizes;
			};
			vector<int> old_sizes, curr_sizes = get_sizes();
			int counter = 0;
			bool last_chance = false;
			while ((old_sizes != curr_sizes || last_chance) && (counter < _limit || !_limit)) {
				if (_debug_ctx >= pass_manager::debug_level::KIND_DEBUG_VERBOSE)
					cout << "____________________________________" << endl << "Cycle #" << counter << endl;
				for (shared_ptr<pass> p : _passes) {
					bbs = p->run_pass(bbs);
					if (_debug_ctx >= pass_manager::debug_level::KIND_DEBUG_VERBOSE) {
						cout << "----------------------------" << endl;
						cout << p->name() << ": " << p->description() << endl;
						print_bbs_cfg(bbs);
						cout << "----------------------------" << endl;
					}
				}
				if (_debug_ctx >= pass_manager::debug_level::KIND_DEBUG_VERBOSE)
					cout << "____________________________________" << endl;
				old_sizes = move(curr_sizes);
				curr_sizes = get_sizes();
				counter++;
				if (last_chance) {
					if (old_sizes == curr_sizes)
						break;
					else
						last_chance = false;
				}
				else if (old_sizes == curr_sizes && (counter < _limit || !_limit) && !last_chance)
					last_chance = true;
			}
			return bbs;
		}
	}
}
