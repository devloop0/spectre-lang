#ifndef SPECTRE_OPT_CALC_LIVENESS_HPP
#define SPECTRE_OPT_CALC_LIVENESS_HPP

#include "ir/basic_block.hpp"
#include "ir/middle_ir.hpp"

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <algorithm>

using namespace spectre::ir;

using std::unordered_map;
using std::unordered_set;
using std::shared_ptr;
using std::pair;
using std::size_t;
using std::hash;

namespace spectre {
	namespace opt {

		struct pair_hasher {
			template<class T1, class T2> size_t operator() (const pair<T1, T2>& p) const {
				return (hash<T1>{}(p.first) << 6) ^ (hash<T2>{}(p.second) >> 2);
			}
		};

		struct def_use_data {
			def_use_data(unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> d,
				unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> u,
				unordered_map<int, shared_ptr<type>> r);
			~def_use_data();

			unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> def, use;
			unordered_map<int, shared_ptr<type>> reg_num_2_type;
		};

		struct liveness_data {
			liveness_data(unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> i,
				unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> o);
			~liveness_data();

			unordered_map<pair<int, int>, unordered_set<int>, pair_hasher> in, out;
		};

		class at_insn : public insn {
		public:
			enum class kind {
				KIND_CAT, KIND_SAT, KIND_IAT, KIND_LAT,
				KIND_FAT, KIND_DAT,
				KIND_LHS_CAT, KIND_LHS_SAT, KIND_LHS_IAT, KIND_LHS_LAT,
				KIND_LHS_FAT, KIND_LHS_DAT,
				KIND_NONE
			};
		private:
			shared_ptr<register_operand> _dst;
			shared_ptr<register_operand> _src;
			shared_ptr<type> _at_type;
			kind _at_kind;
		public:
			at_insn(kind k, shared_ptr<register_operand> d, shared_ptr<register_operand> s,
				shared_ptr<type> at);
			~at_insn();
			shared_ptr<register_operand> dst();
			shared_ptr<register_operand> src();
			kind at_kind();
			shared_ptr<type> at_type();
			string to_string() override;
			void deduplicate() override;
		};

		shared_ptr<def_use_data> calc_def_use(shared_ptr<basic_blocks> bbs);
		shared_ptr<liveness_data> liveness_analysis(shared_ptr<basic_blocks> bbs, shared_ptr<def_use_data> dud);
		void print_liveness_data(shared_ptr<basic_blocks> bbs, shared_ptr<liveness_data> ld);
	}
}

#endif
