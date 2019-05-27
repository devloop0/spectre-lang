#ifndef SPECTRE_GRAPH_HPP
#define SPECTRE_GRAPH_HPP

#include <unordered_set>
#include <unordered_map>
#include <queue>

using std::unordered_set;
using std::unordered_map;
using std::queue;

namespace spectre {
	namespace ir {
		template<class T> class directed_graph {
		private:
			unordered_map<T, unordered_set<T>> _adjacency_list;
		public:
			directed_graph() : _adjacency_list(unordered_map<T, unordered_set<T>>{}) {

			}

			~directed_graph() {

			}

			void add_edge(T u, T v) {
				_adjacency_list[u].insert(v);
			}

			void add_vertex(T u) {
				auto it = _adjacency_list.find(u);
				if (it == _adjacency_list.end())
					_adjacency_list[u] = unordered_set<T>{};
			}

			bool vertex_exists(T u) const {
				return _adjacency_list.find(u) != _adjacency_list.end();
			}

			bool edge_exists(T u, T v) const {
				auto it = _adjacency_list.find(u);
				if (it == _adjacency_list.end())
					return false;
				auto adj_to_set = it->second;
				return adj_to_set.find(v) != adj_to_set.end();
			}

			unordered_set<T> vertices() const {
				unordered_set<T> ret;
				for (const auto&[u, adj] : _adjacency_list)
					ret.insert(u);
				return ret;
			}

			unordered_set<T> adjacent(T u) const {
				auto it = _adjacency_list.find(u);
				if (it == _adjacency_list.end())
					return unordered_set<T>{};
				else
					return it->second;
			}

			void remove_edge(T u, T v) {
				auto uit = _adjacency_list.find(u);
				if (uit == _adjacency_list.end())
					return;
				auto vit = uit->second.find(v);
				if (vit == uit->second.end())
					return;
				uit->second.erase(vit);
			}

			void remove_vertex(T u) {
				auto uit = _adjacency_list.find(u);
				if (uit == _adjacency_list.end())
					return;
				for (auto& [_, v] : _adjacency_list)
					v.erase(u);
			}
		};

		template<class T> unordered_map<T, unordered_set<T>> get_predecessors(const directed_graph<T>& d) {
			unordered_map<T, unordered_set<T>> preds;
			for (const auto& v : d.vertices()) {
				for (const auto& a : d.adjacent(v))
					preds[a].insert(v);
			}
			for (const auto& v : d.vertices()) {
				if (preds.find(v) == preds.end())
					preds[v] = unordered_set<T>{};
			}
			return preds;
		}

		template<class T> vector<T> topological_sort(const directed_graph<T>& d) {
			vector<T> ret;
			unordered_map<T, int> in_degrees;
			for (const auto& u : d.vertices()) {
				for (const auto& v : d.adjacent(u))
					in_degrees[v]++;
			}
			queue<T> q;
			for (const auto& v : d.vertices()) {
				if (in_degrees[v] == 0)
					q.push(v);
			}
			int vertex_count = 0;
			while (!q.empty()) {
				const auto& u = q.front();
				q.pop();
				ret.push_back(u);
				for (const auto& v : d.adjacent(u)) {
					if (--in_degrees[v] == 0)
						q.push(v);
				}
				vertex_count++;
			}
			if (vertex_count != d.vertices().size())
				return vector<T>{};
			return ret;
		}

		template<class T> bool has_cycle_helper(const directed_graph<T>& d, const T& v, unordered_map<T, bool>& visited, unordered_map<T, bool>& cycle_stack) {
			if (!visited[v]) {
				visited[v] = cycle_stack[v] = true;
				for (const auto& adj : d.adjacent(v)) {
					if (!visited[adj] && has_cycle_helper(d, adj, visited, cycle_stack))
						return true;
					else if (cycle_stack[adj])
						return true;
				}
			}
			cycle_stack[v] = false;
			return false;
		}

		template<class T> bool has_cycle(const directed_graph<T>& d) {
			unordered_map<T, bool> visited, cycle_stack;
			for (const auto& v : d.vertices())
				visited[v] = cycle_stack[v] = false;
			for (const auto& v : d.vertices())
				if (has_cycle_helper(d, v, visited, cycle_stack))
					return true;
			return false;
		}
	}
}

#endif