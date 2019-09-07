#include "basic_block.hpp"
#include <algorithm>
#include <memory>
#include <iostream>
#include <iterator>
#include <functional>

using std::static_pointer_cast;
using std::make_pair;
using std::sort;
using std::cout;
using std::next;
using std::inserter;
using std::endl;
using std::set_intersection;
using std::make_pair;
using std::to_string;
using std::reverse;
using std::function;

namespace spectre {
	namespace ir {
		basic_block::basic_block(kind k, int s, int e) : _start(s), _end(e), _basic_block_kind(k), _insns_contained(false),
			_insn_list(vector<shared_ptr<insn>>{}) {

		}

		basic_block::basic_block(kind k, vector<shared_ptr<insn>> il) : _basic_block_kind(k), _start(-1), _end(-1), _insns_contained(true),
			_insn_list(il) {

		}

		basic_block::~basic_block() {

		}

		int basic_block::start() {
			return _start;
		}

		int basic_block::end() {
			return _end;
		}

		void basic_block::set_start(int s) {
			_start = s;
		}

		void basic_block::set_end(int e) {
			_end = e;
		}

		basic_block::kind basic_block::basic_block_kind() {
			return _basic_block_kind;
		}

		bool basic_block::insns_contained() {
			return _insns_contained;
		}

		void basic_block::set_insns_contained(bool b) {
			_insns_contained = b;
		}

		vector<shared_ptr<insn>> basic_block::insn_list() {
			return _insn_list;
		}

		void basic_block::set_insn(int j, shared_ptr<insn> i) {
			if (j < 0 || j >= _insn_list.size())
				return;
			_insn_list[j] = i;
		}

		void basic_block::insert_insn(int j, shared_ptr<insn> i) {
			if (j < 0 || j > _insn_list.size())
				return;
			_insn_list.insert(_insn_list.begin() + j, i);
		}

		void basic_block::add_insn(shared_ptr<insn> i) {
			_insn_list.push_back(i);
		}

		shared_ptr<insn> basic_block::get_insn(int j) {
			if (j < 0 || j >= _insn_list.size())
				return nullptr;
			return _insn_list[j];
		}

		int basic_block::num_insns() {
			return _insn_list.size();
		}

		void basic_block::remove_insn(int j) {
			if (j < 0 || j >= _insn_list.size())
				return;
			_insn_list.erase(_insn_list.begin() + j);
		}

		void basic_block::set_insn_list(vector<shared_ptr<insn>> il) {
			_insn_list = il;
		}

		void basic_block::clear_insn_list() {
			if (!_insns_contained)
				return;
			_insn_list.clear();
		}

		vector<shared_ptr<insn>> basic_block::get_insns(shared_ptr<basic_blocks> bbs) {
			if (_insns_contained)
				return _insn_list;
			vector<shared_ptr<insn>> ret;
			for (int i = _start; i < _end; i++)
				ret.push_back(bbs->get_insn(i));
			return ret;
		}

		void unravel_insns(vector<shared_ptr<insn>>& insns, unordered_set<pair<int, int>, indices_hasher>& func_inds, vector<shared_ptr<insn>> from) {
			for (shared_ptr<insn> i : from) {
				if (i->insn_kind() == insn::kind::KIND_FUNCTION) {
					int start = insns.size();
					unravel_insns(insns, func_inds, static_pointer_cast<function_insn>(i)->insn_list());
					shared_ptr<register_operand> ret_reg = make_shared<register_operand>(*static_pointer_cast<function_insn>(i)->return_register());
					ret_reg->set_dereference(true);
					insns.push_back(make_shared<return_insn>(ret_reg));
					int end = insns.size();
					func_inds.insert(make_pair(start, end));
				}
				else
					insns.push_back(i);
			}
		}

		basic_blocks::basic_blocks(shared_ptr<middle_ir> mi) : _middle_ir(mi), _basic_blocks(vector<shared_ptr<basic_block>>{}), _cfg(directed_graph<int>{}) {
			_original_num_allocated_registers = mi->get_num_allocated_registers();
			unravel_insns(_insns, _function_indices, _middle_ir->global_insn_list());
			unravel_insns(_insns, _function_indices, _middle_ir->insn_list());
			for (auto& i : _insns)
				i->deduplicate();
		}

		basic_blocks::~basic_blocks() {

		}

		void basic_blocks::clear_insn_list() {
			_insns.clear();
		}

		shared_ptr<insn> basic_blocks::get_insn(int i) {
			if (i < 0 || i >= _insns.size())
				return nullptr;
			return _insns[i];
		}

		void basic_blocks::set_insn(int j, shared_ptr<insn> i) {
			if (j < 0 || j >= _insns.size())
				return;
			_insns[j] = i;
		}

		void basic_blocks::insert_insn(int j, shared_ptr<insn> i) {
			if (j < 0 || j > _insns.size())
				return;
			_insns.insert(_insns.begin() + j, i);
		}

		void basic_blocks::remove_insn(int j) {
			if (j < 0 || j > _insns.size())
				return;
			_insns.erase(_insns.begin() + j);
		}

		shared_ptr<basic_block> basic_blocks::get_basic_block(int i) {
			if (0 <= i && i < _basic_blocks.size())
				return _basic_blocks[i];
			return nullptr;
		}

		void basic_blocks::remove_basic_block(int i) {
			if (i < 0 || i >= _basic_blocks.size())
				return;
			_basic_blocks.erase(_basic_blocks.begin() + i);
		}

		vector<shared_ptr<basic_block>> basic_blocks::get_basic_blocks() {
			return _basic_blocks;
		}

		shared_ptr<middle_ir> basic_blocks::get_middle_ir() {
			return _middle_ir;
		}

		directed_graph<int>& basic_blocks::cfg() {
			return _cfg;
		}

		int basic_blocks::add_basic_block(shared_ptr<basic_block> b) {
			_basic_blocks.push_back(b);
			return _basic_blocks.size() - 1;
		}

		int basic_blocks::num_insns() {
			return _insns.size();
		}

		void basic_blocks::report_internal(string msg, string fn, int ln, string fl) {
			_middle_ir->report_internal(msg, fn, ln, fl);
		}

		const unordered_set<pair<int, int>, indices_hasher>& basic_blocks::function_indices() {
			return _function_indices;
		}

		int basic_blocks::original_num_allocated_registers() {
			return _original_num_allocated_registers;
		}

		void basic_blocks::set_basic_block(int j, shared_ptr<basic_block> bb) {
			if (j < 0 || j >= _basic_blocks.size())
				return;
			_basic_blocks[j] = bb;
		}

		shared_ptr<basic_blocks> generate_cfg(shared_ptr<middle_ir> mi) {
			if (mi == nullptr)
				return nullptr;
			shared_ptr<basic_blocks> bbs = make_shared<basic_blocks>(mi);
			unordered_map<int, int> function_start_to_end_map;
			vector<int> function_start_indices;
			for (const auto& k : bbs->function_indices()) {
				function_start_to_end_map[k.first] = k.second;
				function_start_indices.push_back(k.first);
			}
			sort(function_start_indices.begin(), function_start_indices.end());
			int index = 0, function_start_counter = 0;
			while (index < bbs->num_insns()) {
				bool is_function_start = function_start_to_end_map.find(index) != function_start_to_end_map.end();
				int limit = 0;
				if (is_function_start)
					limit = function_start_to_end_map[index];
				else {
					if (function_start_counter < function_start_indices.size())
						limit = function_start_indices[function_start_counter++];
					else
						limit = bbs->num_insns();
				}
				vector<pair<int, shared_ptr<basic_block>>> temp_bbs;
				if (is_function_start) {
					shared_ptr<basic_block> bb = make_shared<basic_block>(basic_block::kind::KIND_ENTRY, index, index);
					bbs->add_basic_block(bb);
					temp_bbs.push_back(make_pair(bbs->get_basic_blocks().size() - 1, bb));
				}
				while (true) {
					int start_index = index;
					for (; index < limit; index++) {
						shared_ptr<insn> i = bbs->get_insn(index);
						bool is_label_insn = i->insn_kind() == insn::kind::KIND_LABEL,
							is_other_bb_delim = i->insn_kind() == insn::kind::KIND_CALL
							|| i->insn_kind() == insn::kind::KIND_CONDITIONAL
							|| i->insn_kind() == insn::kind::KIND_JUMP
							|| i->insn_kind() == insn::kind::KIND_RETURN;
						if (is_label_insn) {
							if (index == start_index)
								continue;
							else
								break;
						}
						if (is_other_bb_delim) {
							index++;
							break;
						}
					}
					shared_ptr<basic_block> bb = make_shared<basic_block>(basic_block::kind::KIND_NORMAL, start_index, index);
					bbs->add_basic_block(bb);
					temp_bbs.push_back(make_pair(bbs->get_basic_blocks().size() - 1, bb));
					if (index >= limit)
						break;
				}
				if (is_function_start) {
					shared_ptr<basic_block> bb = make_shared<basic_block>(basic_block::kind::KIND_EXIT, index, index);
					bbs->add_basic_block(bb);
					temp_bbs.push_back(make_pair(bbs->get_basic_blocks().size() - 1, bb));
				}
				for (const auto& k : temp_bbs)
					bbs->cfg().add_vertex(k.first);
				pair<int, shared_ptr<basic_block>> prev;
				unordered_map<string, int> label_to_bb;
				if (is_function_start && (temp_bbs.empty() || temp_bbs[0].second->basic_block_kind() != basic_block::kind::KIND_ENTRY))
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				for (auto it = temp_bbs.begin(); it != temp_bbs.end(); it++) {
					const auto& k = *it;
					const auto& insns = k.second->get_insns(bbs);
					if (insns.empty())
						continue;
					shared_ptr<insn> first_insn = insns[0];
					if (first_insn->insn_kind() == insn::kind::KIND_LABEL)
						label_to_bb[static_pointer_cast<label_insn>(first_insn)->label()->label_text()] = k.first;
				}
				auto handle_bb_tail = [&](const auto& k) {
					vector<shared_ptr<insn>> bb_insns = k.second->get_insns(bbs);
					if (!bb_insns.empty()) {
						shared_ptr<insn> last_insn = bb_insns[bb_insns.size() - 1];
						string lab_text;
						if (last_insn->insn_kind() == insn::kind::KIND_JUMP)
							lab_text = static_pointer_cast<jump_insn>(last_insn)->label()->label_text();
						else if (last_insn->insn_kind() == insn::kind::KIND_CONDITIONAL)
							lab_text = static_pointer_cast<conditional_insn>(last_insn)->branch()->label_text();
						else
							return;
						if (label_to_bb.find(lab_text) == label_to_bb.end())
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						int to_bb = label_to_bb[lab_text];
						bbs->cfg().add_edge(k.first, to_bb);
					}
				};
				auto bb_ends_with_jump = [&](const auto& k) {
					vector<shared_ptr<insn>> bb_insns = k.second->get_insns(bbs);
					if (bb_insns.empty())
						return false;
					shared_ptr<insn> last_insn = bb_insns[bb_insns.size() - 1];
					return last_insn->insn_kind() == insn::kind::KIND_JUMP || last_insn->insn_kind() == insn::kind::KIND_RETURN;
				};

				unordered_set<string> func_names;
				for (const auto& fi : mi->function_insn_list())
					func_names.insert(c_symbol_2_string(bbs->get_middle_ir(), fi->func_symbol()));

				auto bb_starts_function = [&func_names, &bbs] (const auto& k) {
					vector<shared_ptr<insn>> bb_insns = k.second->get_insns(bbs);
					if (bb_insns.empty())
						return false;
					shared_ptr<insn> first_insn = bb_insns[0];
					if (first_insn->insn_kind() != insn::kind::KIND_LABEL)
						return false;
					shared_ptr<label_insn> li = static_pointer_cast<label_insn>(first_insn);
					string lab_text = li->label()->label_text();
					return func_names.find(lab_text) != func_names.end();
				};

				if (!temp_bbs.empty()) {
					prev = temp_bbs[0];
					handle_bb_tail(prev);
				}
				for (int i = 1; i < temp_bbs.size(); i++) {
					pair<int, shared_ptr<basic_block>> curr = temp_bbs[i];
					if (!bbs->cfg().vertex_exists(prev.first) || !bbs->cfg().vertex_exists(curr.first))
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (!bb_ends_with_jump(prev) && !bb_starts_function(curr))
						bbs->cfg().add_edge(prev.first, curr.first);
					handle_bb_tail(curr);
					prev = curr;
				}
			}
			return bbs;
		}

		void print_bbs_cfg(shared_ptr<basic_blocks> bbs) {
			cout << "Webviz CFG" << endl;
			cout << "digraph CFG {" << endl;
			for (const auto& v : bbs->cfg().vertices())
				for (const auto& w : bbs->cfg().adjacent(v))
					cout << "\t" << v << " -> " << w << endl;
			cout << "}" << endl << endl;

			int index = 0;
			bool all_contained = true;
			for (const auto& bb : bbs->get_basic_blocks()) {
				cout << "Basic block #: " << index++ << endl;
				int start = bb->insns_contained() ? 0 : bb->start();
				all_contained = all_contained && bb->insns_contained();
				for (const auto& i : bb->get_insns(bbs))
					cout << start++ << ": " << i->to_string() << endl;
				cout << endl;
			}

			cout << endl;

			if (all_contained) {
				int counter = 0;
				for (shared_ptr<basic_block> bb : bbs->get_basic_blocks()) {
					for (shared_ptr<insn> i : bb->insn_list())
						cout << counter++ << ": " << i->to_string() << endl;
				}
			}
			else {
				for (int i = 0; i < bbs->num_insns(); i++)
					cout << i << ": " << bbs->get_insn(i)->to_string() << endl;
			}
		}
	}
}
