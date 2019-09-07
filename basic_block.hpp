#ifndef SPECTRE_BASIC_BLOCK_HPP
#define SPECTRE_BASIC_BLOCK_HPP

#include "middle_ir.hpp"
#include "graph.hpp"
#include <unordered_set>
#include <vector>

using std::unordered_set;
using std::vector;
using std::hash;
using std::size_t;

namespace spectre {
	namespace ir {
		class basic_blocks;
		struct indices_hasher {
			size_t operator()(const pair<int, int>& p) const {
				return hash<int>()(p.first) ^ hash<int>()(p.second);
			}
		};

		class basic_block {
		public:
			enum class kind {
				KIND_NORMAL, KIND_ENTRY, KIND_EXIT
			};
		private:
			int _start, _end;
			kind _basic_block_kind;
			bool _insns_contained;
			vector<shared_ptr<insn>> _insn_list;
		public:
			basic_block(kind k, int s, int e);
			basic_block(kind k, vector<shared_ptr<insn>> il);
			~basic_block();
			int start();
			int end();
			void set_start(int s);
			void set_end(int e);
			kind basic_block_kind();
			vector<shared_ptr<insn>> get_insns(shared_ptr<basic_blocks> bbs);
			bool insns_contained();
			void set_insns_contained(bool b);
			vector<shared_ptr<insn>> insn_list();
			void set_insn(int j, shared_ptr<insn> i);
			void add_insn(shared_ptr<insn> i);
			void insert_insn(int j, shared_ptr<insn> i);
			shared_ptr<insn> get_insn(int j);
			int num_insns();
			void remove_insn(int j);
			void set_insn_list(vector<shared_ptr<insn>> il);
			void clear_insn_list();
		};

		class basic_blocks {
		private:
			shared_ptr<middle_ir> _middle_ir;
			vector<shared_ptr<insn>> _insns;
			unordered_set<pair<int, int>, indices_hasher> _function_indices;
			vector<shared_ptr<basic_block>> _basic_blocks;
			directed_graph<int> _cfg;
			int _original_num_allocated_registers;
		public:
			basic_blocks(shared_ptr<middle_ir> mi);
			~basic_blocks();
			shared_ptr<insn> get_insn(int i);
			void set_insn(int j, shared_ptr<insn> i);
			void insert_insn(int j, shared_ptr<insn> i);
			void remove_insn(int j);
			shared_ptr<basic_block> get_basic_block(int i);
			void remove_basic_block(int i);
			int add_basic_block(shared_ptr<basic_block> b);
			vector<shared_ptr<basic_block>> get_basic_blocks();
			const unordered_set<pair<int, int>, indices_hasher>& function_indices();
			shared_ptr<middle_ir> get_middle_ir();
			directed_graph<int>& cfg();
			void report_internal(string msg, string fn, int ln, string fl);
			int num_insns();
			int original_num_allocated_registers();
			void set_basic_block(int j, shared_ptr<basic_block> bb);
			void clear_insn_list();
		};

		void unravel_insns(vector<shared_ptr<insn>>& insns, unordered_set<pair<int, int>, indices_hasher>& func_inds, vector<shared_ptr<insn>> from);
		void print_bbs_cfg(shared_ptr<basic_blocks> bbs);

		shared_ptr<basic_blocks> generate_cfg(shared_ptr<middle_ir> mi);
	}
}

#endif
