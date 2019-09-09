#include <iostream>
#include "pass/pass_manager.hpp"

using std::cout;
using std::endl;
using std::cerr;

namespace spectre {
	namespace opt {

		pass_manager::pass_manager(debug_level d) : _pass_dependency_graph(directed_graph<int>{}), _pass_id_counter(0), _debug(d),
			_pass_id_2_pass(unordered_map<int, shared_ptr<pass>>{}), _pass_name_2_pass(unordered_map<string, shared_ptr<pass>>{}),
			_pass_builder(nullptr) {
		}

		pass_manager::~pass_manager() {

		}

		directed_graph<int>& pass_manager::pass_dependency_graph() {
			return _pass_dependency_graph;
		}

		int pass_manager::current_pass_id() {
			return _pass_id_counter;
		}

		int pass_manager::next_pass_id() {
			return _pass_id_counter++;
		}

		void pass_manager::add_pass(shared_ptr<pass> p) {
			if (_pass_id_2_pass.find(p->pass_id()) != _pass_id_2_pass.end()
				|| _pass_name_2_pass.find(p->name()) != _pass_name_2_pass.end()
				|| _pass_dependency_graph.vertex_exists(p->pass_id())) {
				report_internal("This should be unreachable. (Pass, ID): (\"" + p->name() + "\", " + to_string(p->pass_id()) + ") with description:\n"
					+ "\t" + p->description() + "\n"
					+ "already exists.", __FUNCTION__, __LINE__, __FILE__);
				return;
			}
			for (const auto& v : p->dependencies()) {
				bool non_existent = _pass_name_2_pass.find(v) == _pass_name_2_pass.end();
				if (non_existent) {
					report_internal("This should be unreachable. (Pass, ID): (\"" + p->name() + "\", " + to_string(p->pass_id()) + ") with description:\n"
						+ "\t" + p->description() + "\n"
						+ "has an unsatisfied dependency of pass: " + v + ".", __FUNCTION__, __LINE__, __FILE__);
					return;
				}
			}
			_pass_dependency_graph.add_vertex(p->pass_id());
			for (const auto& v : p->dependencies())
				_pass_dependency_graph.add_edge(_pass_name_2_pass[v]->pass_id(), p->pass_id());
			_pass_id_2_pass[p->pass_id()] = p;
			_pass_name_2_pass[p->name()] = p;
		}

		vector<shared_ptr<pass>> pass_manager::passes() {
			vector<shared_ptr<pass>> ret;
			for (auto[k, v] : _pass_id_2_pass)
				ret.push_back(v);
			return ret;
		}

		shared_ptr<pass> pass_manager::lookup_pass_by_id(int j) {
			if (_pass_id_2_pass.find(j) == _pass_id_2_pass.end())
				return nullptr;
			return _pass_id_2_pass[j];
		}

		shared_ptr<pass> pass_manager::lookup_pass_by_name(string n) {
			if (_pass_name_2_pass.find(n) == _pass_name_2_pass.end())
				return nullptr;
			return _pass_name_2_pass[n];
		}

		shared_ptr<basic_blocks> pass_manager::run_passes(shared_ptr<middle_ir> mi) {
			if (has_cycle(_pass_dependency_graph)) {
				report_internal("This should be unreachable. The pass dependency graph has a cycle.", __FUNCTION__, __LINE__, __FILE__);
				return nullptr;
			}

			shared_ptr<basic_blocks> bbs = generate_cfg(mi);
			place_insns_in_bb(bbs);
			simplify_cfg(bbs);
			remove_unreachable_bbs(bbs);
			bbs = insns_in_bb_2_straight_line(bbs);

			auto print_bbs = [this](shared_ptr<basic_blocks> bbs) {
				if (_debug >= debug_level::KIND_DEBUG) {
					print_bbs_cfg(bbs);
					cout << endl << "=========================" << endl;
				}
			};

			if (_debug >= debug_level::KIND_DEBUG)
				cout << "Initial CFG" << endl;
			print_bbs(bbs);
			vector<int> pass_order = topological_sort(_pass_dependency_graph);
			for (const auto& p : pass_order) {
				if (_debug >= debug_level::KIND_DEBUG)
					cout << _pass_id_2_pass[p]->name() << ": " << _pass_id_2_pass[p]->description() << endl;
				bbs = _pass_id_2_pass[p]->run_pass(bbs);
				print_bbs(bbs);
			}
			return bbs;
		}

		void pass_manager::set_debug(debug_level d) {
			_debug = d;
		}

		pass_manager::debug_level pass_manager::debug() {
			return _debug;
		}

		void pass_manager::report_internal(string msg, string fn, int ln, string fl) {
			cerr << "Internal Compiler Error: [" << fl << ":" << fn << ":" << ln << "] " << msg << "\n";
			exit(1);
		}
	}
}
