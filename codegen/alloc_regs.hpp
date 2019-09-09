#ifndef SPECTRE_ALLOC_REGS_HPP
#define SPECTRE_ALLOG_REGS_HPP

#include "ir/graph.hpp"
#include "opt/calc_liveness.hpp"
#include "ir/basic_block.hpp"
#include "pass/pass_manager.hpp"

#include <memory>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

using namespace spectre::opt;
using namespace spectre::ir;
using std::shared_ptr;
using std::hash;
using std::unordered_set;
using std::unordered_map;
using std::size_t;

namespace spectre {
	namespace cgen {

		template<class T, class H = hash<T>> class undirected_graph : public directed_graph<T, H> {
		public:
			void add_edge(T u, T v) override {
				directed_graph<T, H>::add_edge(u, v);
				directed_graph<T, H>::add_edge(v, u);
			}

			void remove_edge(T u, T v) override {
				directed_graph<T, H>::remove_edge(u, v);
				directed_graph<T, H>::remove_edge(v, u);
			}

			bool edge_exists(T u, T v) const override {
				return directed_graph<T, H>::edge_exists(u, v)
					&& directed_graph<T, H>::edge_exists(v, u);
			}
		};

		struct unordered_set_hasher {
			template<class T, class H, class P, class A> size_t operator() (const unordered_set<T, H, P, A>& v) const {
				hash<T> hasher;
				size_t seed = 0;
				for (const auto& e : v)
					seed ^= hasher(e);
				return seed;
			}
		};

		shared_ptr<undirected_graph<int>> compute_initial_interference_graph(shared_ptr<liveness_data> ld);
		void print_interference_graph(shared_ptr<undirected_graph<int>> u, const unordered_map<int, int>& c);
		unordered_map<int, int> color_interference_graph(shared_ptr<basic_blocks> bbs, int num_phys_regs, int num_fp_phys_regs, pass_manager::debug_level dl);
	}
}

#endif
