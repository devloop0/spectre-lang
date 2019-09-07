#include "arm32.hpp"

#include "alloc_regs.hpp"
#include "opt_passes/basic_dce.hpp"

#include <unordered_map>
#include <algorithm>
#include <unordered_set>
#include <iostream>
#include <functional>
#include <iterator>

using namespace spectre::opt;
using namespace spectre::cgen;
using std::unordered_map;
using std::pair;
using std::unordered_set;
using std::cout;
using std::endl;
using std::static_pointer_cast;
using std::function;
using std::sort;
using std::back_inserter;
using std::copy;
using std::make_pair;
using std::find;
using std::move;
using std::stoi;
using std::tie;

namespace spectre {
	namespace arm32 {

		stk_alloc_data::stk_alloc_data(shared_ptr<register_operand> r, int s, int a) : reg(r), size(s), alignment(a) {

		}

		stk_alloc_data::~stk_alloc_data() {

		}

		int int_log2(shared_ptr<basic_blocks> bbs, int num) {
			if (num <= 0) {
				bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				return 0;
			}

			int l = 0;
			while (num >>= 1) ++l;
			return l;
		}

		bool is_pow2(int num) {
			return num > 0 && (num & (num - 1)) == 0;
		}

		unordered_set<int> compute_reachable_blocks(shared_ptr<basic_blocks> bbs, int start) {
			unordered_set<int> visited;
			
			function<void(int)> dfs = [&visited, &bbs, &dfs] (int bb) {
				if (visited.find(bb) != visited.end())
					return;
				visited.insert(bb);
				for (const auto& v : bbs->cfg().adjacent(bb))
					dfs(v);
			};

			dfs(start);
			return visited;
		}

		string arm_insn_2_string(const arm_insn& ai, int prologue_size) {
			string ret;
			for (const auto& part : ai) {
				if (holds_alternative<string>(part))
					ret += get<string>(part);
				else if (holds_alternative<arm_offset>(part)) {
					const arm_offset& ao = get<arm_offset>(part);
					switch (ao.offset_kind) {
					case arm_offset::kind::KIND_FP:
						ret += to_string(ao.immediate);
						break;
					case arm_offset::kind::KIND_TOP:
						ret += to_string(ao.immediate + prologue_size);
						break;
					}
				}
			}
			return ret;
		}


		void generate_start_hook(shared_ptr<arm_asm> aa, main_function_kind mfk) {
			if (mfk == main_function_kind::NONE)
				return;
			vector<arm_insn> function_body;
			function_body.push_back({START_SYMBOL, ":"});
			switch (mfk) {
			case main_function_kind::MAIN_NO_ARGS:
				break;
			case main_function_kind::MAIN_ARGC_ARGV: {
				function_body.push_back({TAB, "ldr r0, [sp]"});
				function_body.push_back({TAB, "add r1, sp, #4"});
			}
				break;
			case main_function_kind::MAIN_ARGC_ARGV_ENVP: {
				function_body.push_back({TAB, "ldr r0, [sp]"});
				function_body.push_back({TAB, "add r1, sp, #4"});
				function_body.push_back({TAB, "add r2, sp, #8"});
			}
				break;
			}
			function_body.push_back({TAB, "bl ", MAIN_SYMBOL});
			function_body.push_back({TAB, "bl ", EXIT_HOOK});
			function_body.push_back({});

			if (!aa->file_body.empty() && !aa->file_body[aa->file_body.size() - 1].empty())
				aa->file_body.push_back("");
			if (!aa->file_header.empty() && !aa->file_header[aa->file_header.size() - 1].empty())
				aa->file_header.push_back("");

			aa->file_header.push_back(".globl _start");
			aa->file_header.push_back(".globl " + EXIT_HOOK);
			aa->file_header.push_back("");

			for (const auto& i : function_body)
				aa->file_body.push_back(arm_insn_2_string(i, 0));
		}

		void handle_functions_asm(shared_ptr<basic_blocks> bbs, shared_ptr<arm_asm> aa,
			const unordered_set<int>& function_headers, const unordered_map<int, int>& coloring,
			unordered_map<int, string> g_var_2_sym_name_map, unordered_map<int, string> e_var_2_sym_name_map,
			pass_manager::debug_level dl) {
			int scratch_counter = 0;
			aa->file_body.push_back(".text");

			for (const auto& fh_index : function_headers) {
				shared_ptr<basic_block> fbb = bbs->get_basic_block(fh_index);
				unordered_set<int> reachable_bbs = compute_reachable_blocks(bbs, fh_index);
				vector<int> reachable_bbs_sorted;
				copy(reachable_bbs.begin(), reachable_bbs.end(), back_inserter(reachable_bbs_sorted));
				sort(reachable_bbs_sorted.begin(), reachable_bbs_sorted.end());

				vector<pair<int, int>> stack_allocations, resv_allocations;
				for (const auto& fb_index : reachable_bbs_sorted) {
					shared_ptr<basic_block> bb = bbs->get_basic_block(fb_index);
					int insn_counter = 0;
					for (shared_ptr<insn> i : bb->get_insns(bbs)) {
						if (i->insn_kind() == insn::kind::KIND_UNARY) {
							shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
							shared_ptr<operand> dst = ui->dst_operand();
							auto check_stk_resv_alloc = [&dst, &bbs, &coloring] () {
								if (dst->operand_kind() != operand::kind::KIND_REGISTER)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								shared_ptr<register_operand> rdst = static_pointer_cast<register_operand>(dst);
								if (rdst->dereference())
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								if (coloring.find(rdst->virtual_register_number()) != coloring.end())
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							};
							if (ui->unary_expr_kind() == unary_insn::kind::KIND_STK) {
								check_stk_resv_alloc();
								stack_allocations.push_back(make_pair(fb_index, insn_counter));
							}
							else if (ui->unary_expr_kind() == unary_insn::kind::KIND_RESV) {
								check_stk_resv_alloc();
								resv_allocations.push_back(make_pair(fb_index, insn_counter));
							}
						}
						insn_counter++;
					}
				}

				if (fbb->get_insns(bbs).empty())
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				shared_ptr<insn> header_insn = fbb->get_insns(bbs)[0];
				if (header_insn->insn_kind() != insn::kind::KIND_LABEL)
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
				shared_ptr<label_insn> func_label = static_pointer_cast<label_insn>(header_insn);
				if (func_label->orig_func_insn() == nullptr)
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);

				for (const auto& [bb_pos, insn_pos] : resv_allocations) {
					shared_ptr<basic_block> bb = bbs->get_basic_block(bb_pos);
					int alignment = 1;
					if (insn_pos != 0) { 
						shared_ptr<insn> prev_insn = bb->get_insn(insn_pos - 1);
						if (prev_insn->insn_kind() == insn::kind::KIND_ALIGN)
							alignment = static_pointer_cast<align_insn>(prev_insn)->alignment();
					}
					if (!is_pow2(alignment))
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					alignment = int_log2(bbs, alignment);
					if (alignment != 1)
						aa->file_bss.push_back(".align " + to_string(alignment));

					shared_ptr<insn> resv_insn = bb->get_insn(insn_pos);
					if (resv_insn->insn_kind() != insn::kind::KIND_UNARY)
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<unary_insn> ui_resv_insn = static_pointer_cast<unary_insn>(resv_insn);
					if (ui_resv_insn->unary_expr_kind() != unary_insn::kind::KIND_RESV)
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (ui_resv_insn->dst_operand()->operand_kind() != operand::kind::KIND_REGISTER)
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (ui_resv_insn->src_operand()->operand_kind() != operand::kind::KIND_IMMEDIATE)
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<register_operand> resv_reg = static_pointer_cast<register_operand>(ui_resv_insn->dst_operand());
					if (resv_reg->dereference())
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<immediate_operand> resv_size = static_pointer_cast<immediate_operand>(ui_resv_insn->src_operand());

					if (g_var_2_sym_name_map.find(resv_reg->base_virtual_register_number()) != g_var_2_sym_name_map.end())
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					string lab = FUNC_SCRATCH_PREFIX + to_string(scratch_counter++);
					g_var_2_sym_name_map[resv_reg->base_virtual_register_number()] = lab;
					aa->file_bss.push_back(lab + ": .space " + to_string(retrieve_immediate_operand<int>(bbs, resv_size)));
				}

				vector<shared_ptr<stk_alloc_data>> local_stk_allocs;
				shared_ptr<stk_alloc_data> ret_stk_alloc;
				bool ret_is_struct = is_struct_type(bbs->get_middle_ir(), func_label->orig_func_insn()->func_type()->return_type());
				unordered_map<int, int> arg_reg_num_2_arg_pos;
				unordered_map<int, shared_ptr<stk_alloc_data>> arg_reg_num_2_stk_allocs;
				unordered_map<int, shared_ptr<stk_alloc_data>> reg_num_2_stk_allocs;
				for (const auto& [bb_index, insn_index] : stack_allocations) {
					shared_ptr<basic_block> bb = bbs->get_basic_block(bb_index);
					int alignment = 1;
					if (insn_index != 0) {
						shared_ptr<insn> i = bb->get_insn(insn_index - 1);
						if (i->insn_kind() == insn::kind::KIND_ALIGN)
							alignment = static_pointer_cast<align_insn>(i)->alignment();
					}
					if (!is_pow2(alignment))
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);

					if (bb->get_insn(insn_index)->insn_kind() != insn::kind::KIND_UNARY)
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<unary_insn> stk_insn = static_pointer_cast<unary_insn>(bb->get_insn(insn_index));
					if (stk_insn->unary_expr_kind() != unary_insn::kind::KIND_STK)
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<operand> dst = stk_insn->dst_operand(), src = stk_insn->src_operand();
					if (dst->operand_kind() != operand::kind::KIND_REGISTER)
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<register_operand> dst_reg = static_pointer_cast<register_operand>(dst);
					if (dst_reg->dereference())
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (g_var_2_sym_name_map.find(dst_reg->base_virtual_register_number()) != g_var_2_sym_name_map.end())
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (src->operand_kind() != operand::kind::KIND_IMMEDIATE)
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<immediate_operand> src_imm = static_pointer_cast<immediate_operand>(src);

					shared_ptr<stk_alloc_data> sad = make_shared<stk_alloc_data>(dst_reg,
						retrieve_immediate_operand<int>(bbs, src_imm), alignment);
					bool is_arg = false;
					int arg_index = 0;
					for (; arg_index < func_label->func_param_regs().size(); arg_index++) {
						if (func_label->func_param_regs()[arg_index] == dst_reg->base_virtual_register_number()) {
							is_arg = true;
							break;
						}
					}
					if (is_arg) {
						if (arg_reg_num_2_stk_allocs.find(dst_reg->virtual_register_number()) != arg_reg_num_2_stk_allocs.end())
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						if (arg_reg_num_2_arg_pos.find(dst_reg->virtual_register_number()) != arg_reg_num_2_arg_pos.end())
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						arg_reg_num_2_stk_allocs[dst_reg->virtual_register_number()] = sad;
						arg_reg_num_2_arg_pos[dst_reg->virtual_register_number()] = arg_index;
					}
					else if (dst_reg->base_virtual_register_number() == func_label->func_return_reg()) {
						if (ret_stk_alloc != nullptr)
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						ret_stk_alloc = sad;
					}
					else
						local_stk_allocs.push_back(sad);
					if (reg_num_2_stk_allocs.find(dst_reg->virtual_register_number()) != reg_num_2_stk_allocs.end())
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					reg_num_2_stk_allocs[dst_reg->virtual_register_number()] = sad;
				}

				vector<arm_insn> function_prologue, function_body, function_epilogue;
				unordered_set<string> nonvolatile_registers_used = {
					registers::fp,
					registers::lr
				};

				function_prologue.push_back({func_label->label()->label_text(), ":"});

				auto calc_arg_locs = [&bbs] (vector<shared_ptr<type>> param_types, bool is_variadic) {
					int arg_reg_num = 0, curr_stk_offs = 0, arg_pos = 0, fp_arg_reg_num = 0;
					unordered_map<int, int> arg_pos_2_phys_reg_num;
					unordered_map<int, int> arg_pos_2_fp_phys_reg_num;
					unordered_map<int, int> arg_pos_2_stk_offs;
					for (shared_ptr<type> param_type : param_types) {
						shared_ptr<size_data> sd = spectre_sizeof(bbs->get_middle_ir(), param_type);
						if (is_floating_point_type(bbs->get_middle_ir(), param_type)) {
							bool needs_stk = fp_arg_reg_num >= registers::num_fp_argument_registers
								|| is_variadic;
							if (needs_stk) {
								if (curr_stk_offs % sd->alignment != 0) {
									int to_next_alignment = sd->alignment
										- (curr_stk_offs % sd->alignment);
									curr_stk_offs += to_next_alignment;
								}
								arg_pos_2_stk_offs[arg_pos] = curr_stk_offs;
								curr_stk_offs += sd->size;
							}
							else
								arg_pos_2_fp_phys_reg_num[arg_pos] = fp_arg_reg_num++;
						}
						else {
							bool needs_stk = arg_reg_num >= registers::num_argument_registers
								|| is_struct_type(bbs->get_middle_ir(), param_type)
								|| is_variadic;
							if (needs_stk) {
								if (curr_stk_offs % sd->alignment != 0) {
									int to_next_alignment = sd->alignment
										- (curr_stk_offs % sd->alignment);
									curr_stk_offs += to_next_alignment;
								}
								arg_pos_2_stk_offs[arg_pos] = curr_stk_offs;
								curr_stk_offs += sd->size;
							}
							else
								arg_pos_2_phys_reg_num[arg_pos] = arg_reg_num++;
						}
						arg_pos++;
					}
					return make_tuple(arg_pos_2_phys_reg_num, arg_pos_2_fp_phys_reg_num, arg_pos_2_stk_offs, curr_stk_offs);
				};
					
				// top of stack relative
				vector<shared_ptr<type>> func_arg_types;
				for (shared_ptr<variable_declaration> vdecl : func_label->orig_func_insn()->func_type()->parameter_list())
					func_arg_types.push_back(vdecl->variable_declaration_type());
				auto [arg_pos_2_phys_reg_num, arg_pos_2_fp_phys_reg_num, arg_pos_2_stk_offs, func_args_stk_size] =
					calc_arg_locs(func_arg_types, func_label->orig_func_insn()->func_type()->variadic());

				// fp relative
				int func_stk_space = 0;
				unordered_map<int, int> reg_num_2_stk_offs;
				int ret_stk_offs = 0;
				for (const auto& [arg_reg_num, sad] : arg_reg_num_2_stk_allocs) {
					int arg_pos = arg_reg_num_2_arg_pos[arg_reg_num];
					if (arg_pos_2_stk_offs.find(arg_pos) != arg_pos_2_stk_offs.end())
						continue;
					shared_ptr<size_data> sd = spectre_sizeof(bbs->get_middle_ir(), func_label->orig_func_insn()->func_type()
						->parameter_list()[arg_pos]->variable_declaration_type());
					func_stk_space += -sd->size;
					func_stk_space += -(((func_stk_space % sd->alignment) + sd->alignment) % sd->alignment);
					reg_num_2_stk_offs[sad->reg->virtual_register_number()] = func_stk_space;
				}
				if (ret_is_struct && ret_stk_alloc != nullptr) {
					shared_ptr<size_data> sd = spectre_sizeof(bbs->get_middle_ir(), func_label->orig_func_insn()->func_type()->return_type());
					func_stk_space += -sd->size;
					func_stk_space += -(((func_stk_space % sd->alignment) + sd->alignment) % sd->alignment);
					ret_stk_offs = func_stk_space;
				}
				for (shared_ptr<stk_alloc_data> sad : local_stk_allocs) {
					func_stk_space += -sad->size;
					func_stk_space += -(((func_stk_space % sad->alignment) + sad->alignment) % sad->alignment);
					reg_num_2_stk_offs[sad->reg->virtual_register_number()] = func_stk_space;
				}
				func_stk_space = -func_stk_space;

				auto get_ls_insn = [&bbs] (shared_ptr<type> t, bool l) -> string {
					string prefix = l ? "ld" : "st";
					if (is_double_type(bbs->get_middle_ir(), t))
						return "v" + prefix + "r.F64";
					else if (is_float_type(bbs->get_middle_ir(), t))
						return "v" + prefix + "r.F32";
					else if (is_long_type(bbs->get_middle_ir(), t)
						|| is_int_type(bbs->get_middle_ir(), t)
						|| is_pointer_type(bbs->get_middle_ir(), t)
						|| is_struct_type(bbs->get_middle_ir(), t)) {
						return prefix + "r";
					}
					else if (is_short_type(bbs->get_middle_ir(), t))
						return prefix + "r" + (!l || is_unsigned_type(bbs->get_middle_ir(), t) ? "h" : "sh");
					else if (is_char_type(bbs->get_middle_ir(), t)
						|| is_bool_type(bbs->get_middle_ir(), t)
						|| is_byte_type(bbs->get_middle_ir(), t)
						|| is_void_type(bbs->get_middle_ir(), t))
						return prefix + "r" + (!l || is_unsigned_type(bbs->get_middle_ir(), t) ? "b" : "sb");;
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return "";
				};

				unordered_set<int> all_args_base_regs;
				for (int r : func_label->func_param_regs())
					all_args_base_regs.insert(r);

				auto map_color_2_register = [&] (shared_ptr<register_operand> reg) {
					string mapped_temp_reg;
					int numeric_mapping = coloring.at(reg->virtual_register_number());
					if (numeric_mapping >= registers::num_general_purpose_registers) {
						numeric_mapping -= registers::num_general_purpose_registers;
						if (!is_floating_point_type(bbs->get_middle_ir(), reg->register_type()))
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						if (is_float_type(bbs->get_middle_ir(), reg->register_type()))
							mapped_temp_reg = registers::fp_general_purpose_registers[numeric_mapping];
						else {
							mapped_temp_reg = registers::fp_general_purpose_registers[numeric_mapping];
							int fp_true_num = registers::fp_register_name_2_number.at(mapped_temp_reg);
							fp_true_num >>= 1;
							mapped_temp_reg = "d" + to_string(fp_true_num);
						}
					}
					else {
						mapped_temp_reg = registers::general_purpose_registers.at(numeric_mapping);
					}
					return mapped_temp_reg;
				};

				auto add_register_if_nonvolatile = [&] (string reg_name) {
					if (auto it = registers::register_name_2_number.find(reg_name); it != registers::register_name_2_number.end()) {
						if (!registers::register_volatility_map[it->second])
							nonvolatile_registers_used.insert(it->first);
					}
					else if (auto it = registers::fp_register_name_2_number.find(reg_name); it != registers::fp_register_name_2_number.end()) {
						 if (!registers::fp_register_volatility_map[it->second]) {
							 string other = registers::fp_register_number_2_name.at(
								it->second % 2 == 0 ? it->second + 1 : it->second - 1);
							 nonvolatile_registers_used.insert(reg_name);
							 nonvolatile_registers_used.insert(other);
						 }
					}
					else if (!reg_name.empty() && reg_name[0] == 'd') {
						int d_num = stoi(reg_name.substr(1));
						if (d_num >= 16 || d_num < 0)
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						d_num <<= 1;
						if (!registers::fp_register_volatility_map[d_num]) {
							string other = registers::fp_register_number_2_name.at(d_num + 1);
							string curr = registers::fp_register_number_2_name.at(d_num);
							nonvolatile_registers_used.insert(curr);
							nonvolatile_registers_used.insert(other);
						}
					}
				};

				auto handle_rvalue_register = [&] (shared_ptr<register_operand> reg, string temp_reg, string int_temp_reg) -> string {
					string l_insn = get_ls_insn(reg->register_type(), true);
					bool is_fp_l_insn = l_insn[0] == 'v';
					add_register_if_nonvolatile(temp_reg);
					add_register_if_nonvolatile(int_temp_reg);
					if (e_var_2_sym_name_map.find(reg->base_virtual_register_number()) != e_var_2_sym_name_map.end()) {
						function_body.push_back({TAB, "ldr ", int_temp_reg, ", =",
							e_var_2_sym_name_map[reg->base_virtual_register_number()]});
						if (reg->dereference()) {
							function_body.push_back({TAB, l_insn, " ", temp_reg, ", [", int_temp_reg, "]"});
							return temp_reg;
						}
						else
							return int_temp_reg;
					}
					else if (g_var_2_sym_name_map.find(reg->base_virtual_register_number()) != g_var_2_sym_name_map.end()) {
						function_body.push_back({TAB, "ldr ", int_temp_reg, ", =",
							g_var_2_sym_name_map[reg->base_virtual_register_number()]});
						if (reg->dereference()) {
							function_body.push_back({TAB, l_insn, " ", temp_reg, ", [", int_temp_reg, "]"});
							return temp_reg;
						}
						else
							return int_temp_reg;
					}
					else if (reg_num_2_stk_offs.find(reg->virtual_register_number()) != reg_num_2_stk_offs.end()) {
						int stk_offs = reg_num_2_stk_offs[reg->virtual_register_number()];
						arm_offset offs = { arm_offset::kind::KIND_FP, stk_offs };
						if (reg->dereference()) {
							if (0 <= stk_offs && stk_offs < 256)
								function_body.push_back({TAB, l_insn, " ", temp_reg, ", [", registers::fp, ", #", offs, "]"});
							else {
								function_body.push_back({TAB, "ldr ", int_temp_reg, ", =", offs});
								if (is_fp_l_insn) {
									function_body.push_back({TAB, "add ", int_temp_reg, ", ", int_temp_reg, ", ", registers::fp});
									function_body.push_back({TAB, l_insn, " ", temp_reg, ", [", int_temp_reg, "]"});
								}
								else
									function_body.push_back({TAB, l_insn, " ", temp_reg, ", [", registers::fp, ", ", int_temp_reg, "]"});
							}
							return temp_reg;
						}
						else {
							if (0 <= stk_offs && stk_offs < 256)
								function_body.push_back({TAB, "add ", int_temp_reg, ", ", registers::fp, ", #", offs});
							else {
								function_body.push_back({TAB, "ldr ", int_temp_reg, ", =", offs});
								function_body.push_back({TAB, "add ", int_temp_reg, ", ", registers::fp, ", ", int_temp_reg});
							}
							return int_temp_reg;
						}
					}
					else {
						if (reg->base_virtual_register_number() == func_label->func_return_reg()) {
							if (!reg->dereference()) {
								if (ret_stk_alloc == nullptr || !ret_is_struct)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								arm_offset offs = { arm_offset::kind::KIND_FP, ret_stk_offs };
								function_body.push_back({TAB, "ldr ", int_temp_reg, ", =", offs});
								function_body.push_back({TAB, "add ", registers::a0, ", ", registers::fp, ", ", int_temp_reg});
								return registers::a0;
							}
							else {
								shared_ptr<type> ret_type = func_label->orig_func_insn()->func_type()->return_type();
								if (is_double_type(bbs->get_middle_ir(), ret_type))
									return "d0";
								else if (is_float_type(bbs->get_middle_ir(), ret_type))
									return registers::f0;
								else
									return registers::a0;
							}
						}
						else if (all_args_base_regs.find(reg->base_virtual_register_number()) != all_args_base_regs.end()) {
							int arg_index = 0;
							for (; arg_index < func_label->func_param_regs().size(); arg_index++) {
								if (func_label->func_param_regs()[arg_index] == reg->base_virtual_register_number())
									break;
							}
							shared_ptr<type> param_type = func_label->orig_func_insn()->func_type()
								->parameter_list()[arg_index]->variable_declaration_type();
							string base;
							if (arg_pos_2_phys_reg_num.find(arg_index) != arg_pos_2_phys_reg_num.end()) {
								base = registers::argument_registers[arg_pos_2_phys_reg_num[arg_index]];
								if (reg->dereference()) {
									function_body.push_back({TAB, l_insn, " ", temp_reg, ", [", base, "]"});
									base = temp_reg;
								}
							}
							else if (arg_pos_2_fp_phys_reg_num.find(arg_index) != arg_pos_2_fp_phys_reg_num.end()) {
								if (reg->dereference())
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								int reg_num = arg_pos_2_fp_phys_reg_num[arg_index];
								if (is_float_type(bbs->get_middle_ir(), param_type))
									base = "s" + to_string(reg_num << 1);
								else if (is_double_type(bbs->get_middle_ir(), param_type))
									base = "d" + to_string(reg_num);
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}
							else if (arg_pos_2_stk_offs.find(arg_index) != arg_pos_2_stk_offs.end()) {
								bool param_is_struct = is_struct_type(bbs->get_middle_ir(), param_type);
								arm_offset offs = { arm_offset::kind::KIND_TOP, arg_pos_2_stk_offs[arg_index] };
								bool special = false;
								if (arg_reg_num_2_stk_allocs.find(reg->virtual_register_number()) !=
									arg_reg_num_2_stk_allocs.end())
									special = true;
								function_body.push_back({TAB, "ldr ", int_temp_reg, ", =", offs});
								if (!param_is_struct) {
									if (special && !reg->dereference()) {
										function_body.push_back({TAB, "add ", int_temp_reg, ", ", int_temp_reg,
											", ", registers::fp});
										base = int_temp_reg;
									}
									else if (!special && reg->dereference()) {
										function_body.push_back({TAB, "ldr ", temp_reg, ", [", registers::fp, ", ",
											int_temp_reg, "]"});
										function_body.push_back({TAB, l_insn, " ", temp_reg, ", [", temp_reg, "]"});
										base = temp_reg;
									}
									else {
										if (is_fp_l_insn) {
											function_body.push_back({TAB, "add ", int_temp_reg, ", ", int_temp_reg, ", ",
												registers::fp});
											function_body.push_back({TAB, l_insn, " ", temp_reg, ", [", int_temp_reg,
												"]"});
										}
										else {
											function_body.push_back({TAB, l_insn, " ", temp_reg, ", [", registers::fp, ", ",
												int_temp_reg, "]"});
										}
										base = temp_reg;
									}
								}
								else {
									if (special && !reg->dereference()) {
										function_body.push_back({TAB, "add ", int_temp_reg, ", ", int_temp_reg,
											", ", registers::fp});
										base = int_temp_reg;
									}
									else if (!special && reg->dereference()) {
										if (is_fp_l_insn) {
											function_body.push_back({TAB, "add ", int_temp_reg, ", ", int_temp_reg, ", ",
												registers::fp});
											function_body.push_back({TAB, l_insn, " ", temp_reg, ", [", int_temp_reg, "]"});
										}
										else {
											function_body.push_back({TAB, l_insn, " ", temp_reg, ", [", registers::fp, ", ",
												int_temp_reg, "]"});
										}
										base = temp_reg;
									}
									else {
										function_body.push_back({TAB, "add ", int_temp_reg, ", ", registers::fp, ", ", int_temp_reg});
										base = int_temp_reg;
									}
								}
							}
							else
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							return base;
						}
						else if (coloring.find(reg->virtual_register_number()) != coloring.end()) {
							string mapped_temp_reg = map_color_2_register(reg);
							if (reg->dereference()) {
								function_body.push_back({TAB, l_insn, " ", temp_reg, ", [", mapped_temp_reg, "]"});
								add_register_if_nonvolatile(mapped_temp_reg);
								return temp_reg;
							}
							else
								return mapped_temp_reg;
						}
					}
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					return "";
				};

				auto get_fp_ext = [&] (string reg_str) {
					string ext;
					if (reg_str.empty())
						ext = "";
					else if (reg_str[0] == 's')
						ext = "F32";
					else if (reg_str[0] == 'd')
						ext = "F64";
					return ext;
				};

				auto handle_lvalue_register = [&] (shared_ptr<register_operand> reg, string int_temp_reg, string rhs) {
					shared_ptr<type> orig_reg_type = reg->register_type();
					string s_insn = get_ls_insn(orig_reg_type, false);
					bool is_fp_s_insn = s_insn[0] == 'v';
					if (!int_temp_reg.empty())
						add_register_if_nonvolatile(int_temp_reg);
					if (e_var_2_sym_name_map.find(reg->base_virtual_register_number()) != e_var_2_sym_name_map.end()) {
						if (!reg->dereference())
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FUNCTION__);
						string sym_name = e_var_2_sym_name_map[reg->base_virtual_register_number()];
						function_body.push_back({TAB, "ldr ", int_temp_reg, ", =", sym_name});
						function_body.push_back({TAB, s_insn, " ", rhs, ", [", int_temp_reg, "]"});
						return;
					}
					else if (g_var_2_sym_name_map.find(reg->base_virtual_register_number()) != g_var_2_sym_name_map.end()) {
						if (!reg->dereference())
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						string sym_name = g_var_2_sym_name_map[reg->base_virtual_register_number()];
						function_body.push_back({TAB, "ldr ", int_temp_reg, ", =", sym_name});
						function_body.push_back({TAB, s_insn, " ", rhs, ", [", int_temp_reg, "]"});
						return;
					}
					else if (reg_num_2_stk_offs.find(reg->virtual_register_number()) != reg_num_2_stk_offs.end()) {
						if (!reg->dereference())
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						int stk_offs = reg_num_2_stk_offs[reg->virtual_register_number()];
						arm_offset offs = { arm_offset::kind::KIND_FP, stk_offs };
						if (0 <= stk_offs && stk_offs < 256) {
							function_body.push_back({TAB, s_insn, " ", rhs, ", [", registers::fp, ", #",
								offs, "]"});
						}
						else {
							function_body.push_back({TAB, "ldr ", int_temp_reg, ", =", offs});
							if (is_fp_s_insn) {
								function_body.push_back({TAB, "add ", int_temp_reg, ", ", int_temp_reg, ", ", registers::fp});
								function_body.push_back({TAB, s_insn, " ", rhs, ", [", int_temp_reg, "]"});
							}
							else
								function_body.push_back({TAB, s_insn, " ", rhs, ", [", registers::fp, ", ", int_temp_reg, "]"});
						}
						return;
					}
					else {
						if (reg->base_virtual_register_number() == func_label->func_return_reg()) {
							if (!reg->dereference()) {
								if (ret_stk_alloc == nullptr || !ret_is_struct)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								arm_offset offs = { arm_offset::kind::KIND_FP, ret_stk_offs };
								function_body.push_back({TAB, "ldr ", int_temp_reg, ", =", offs});
								shared_ptr<size_data> sd = spectre_sizeof(bbs->get_middle_ir(),
									func_label->orig_func_insn()->func_type()->return_type());
								function_body.push_back({TAB, "add ", registers::a0, ", ", registers::fp, ", ", int_temp_reg});
								for (int i = 0; i < sd->size; i++) {
									function_body.push_back({TAB, "ldrb ", int_temp_reg, ", [", rhs,
										", #", to_string(i), "]"});
									function_body.push_back({TAB, "strb ", int_temp_reg, ", [", registers::a0,
										", #", to_string(i), "]"});
								}
							}
							else {
								shared_ptr<type> ret_type = func_label->orig_func_insn()->func_type()->return_type();
								string ret_reg;
								string mov_insn;
								if (is_float_type(bbs->get_middle_ir(), ret_type))
									ret_reg = registers::f0, mov_insn = "vmov.F32";
								else if (is_double_type(bbs->get_middle_ir(), ret_type))
									ret_reg = "d0", mov_insn = "vmov.F64";
								else
									ret_reg = registers::a0, mov_insn = "mov";
								if (ret_reg != rhs)
									function_body.push_back({TAB, mov_insn, " ", ret_reg, ", ", rhs});
							}
							return;
						}
						else if (all_args_base_regs.find(reg->base_virtual_register_number()) != all_args_base_regs.end()) {
							int arg_pos = 0;
							for (; arg_pos < func_label->func_param_regs().size(); arg_pos++) {
								if (func_label->func_param_regs()[arg_pos] == reg->base_virtual_register_number())
									break;
							}
							if (arg_pos_2_phys_reg_num.find(arg_pos) != arg_pos_2_phys_reg_num.end()) {
								string arg = registers::argument_registers[arg_pos_2_phys_reg_num[arg_pos]];
								if (!reg->dereference()) {
									if (arg != rhs)
										function_body.push_back({TAB, "mov ", arg, ", ", rhs});
								}
								else
									function_body.push_back({TAB, s_insn, " ", rhs, ", [", arg, "]"});
								return;
							}
							else if (arg_pos_2_fp_phys_reg_num.find(arg_pos) != arg_pos_2_fp_phys_reg_num.end()) {
								string arg = registers::fp_argument_registers[arg_pos_2_fp_phys_reg_num[arg_pos]];
								shared_ptr<type> arg_type = func_label->orig_func_insn()->func_type()
									->parameter_list()[arg_pos]->variable_declaration_type();
								string mov_insn;
								if (is_float_type(bbs->get_middle_ir(), arg_type))
									mov_insn = "vmov.F32";
								else if (is_double_type(bbs->get_middle_ir(), arg_type))
									mov_insn = "vmov.F64";
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								if (!reg->dereference()) {
									if (arg != rhs)
										function_body.push_back({TAB, mov_insn, " ", arg, ", ", rhs});
								}
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return;
							}
							else if (arg_pos_2_stk_offs.find(arg_pos) != arg_pos_2_stk_offs.end()) {
								int stk_offs = arg_pos_2_stk_offs[arg_pos];
								arm_offset offs = arm_offset{arm_offset::kind::KIND_TOP, stk_offs};
								string true_rhs;

								bool special = false;
								if (arg_reg_num_2_stk_allocs.find(reg->virtual_register_number()) !=
									arg_reg_num_2_stk_allocs.end())
									special = true;

								function_body.push_back({TAB, "ldr ", int_temp_reg, ", =", offs});
								if ((!special && !reg->dereference()) || (special && reg->dereference())) {
									if (is_fp_s_insn) {
										function_body.push_back({TAB, "add ", int_temp_reg, ", ", int_temp_reg, ", ",
											registers::fp});
										function_body.push_back({TAB, s_insn, " ", rhs, ", [", int_temp_reg, "]"});
									}
									else {
										function_body.push_back({TAB, s_insn, " ", rhs, ", [", registers::fp, ", ",
											int_temp_reg, "]"});
									}
								}
								else {
									if (special && !reg->dereference())
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);

									function_body.push_back({TAB, "ldr ", int_temp_reg, ", [", registers::fp, ", ", int_temp_reg,
										"]"});
									function_body.push_back({TAB, s_insn, " ", true_rhs, ", [", int_temp_reg, "]"});
								}
								return;
							}
						}
						else if (coloring.find(reg->virtual_register_number()) != coloring.end()) {
							string mapped_reg = map_color_2_register(reg);
							if (mapped_reg.empty())
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							bool is_fp = mapped_reg[0] == 's' || mapped_reg[0] == 'd';
							string mov_insn = mapped_reg[0] == 's' ? "vmov.F32" : (mapped_reg[0] == 'd' ? "vmov.F64" : "mov");
							add_register_if_nonvolatile(mapped_reg);
							if (!reg->dereference()) {
								if (is_fp) {
									if (mapped_reg != rhs)
										function_body.push_back({TAB, mov_insn, " ", mapped_reg, ", ", rhs});
									else
										function_body.push_back({TAB, s_insn, " ", rhs, ", [", mapped_reg, "]"});
								}
							}
							else {
								if (is_fp)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								else
									function_body.push_back({TAB, s_insn, " ", rhs, ", [", mapped_reg, "]"});
							}
							return;
						}
					}
					if (dl >= pass_manager::debug_level::KIND_DEBUG)
						cout << "Unwriteable (no reads afterwards or error) register hit: " << reg->to_string() << endl;
				};

				auto get_pure_register = [&] (shared_ptr<register_operand> reg) -> pair<bool, string> {
					if (e_var_2_sym_name_map.find(reg->base_virtual_register_number()) != e_var_2_sym_name_map.end()
						|| g_var_2_sym_name_map.find(reg->base_virtual_register_number()) != g_var_2_sym_name_map.end()
						|| reg_num_2_stk_offs.find(reg->virtual_register_number()) != reg_num_2_stk_offs.end())
						return make_pair(false, "");

					if (reg->base_virtual_register_number() == func_label->func_return_reg()) {
						if (!reg->dereference())
							return make_pair(false, "");

						shared_ptr<type> ret_type = func_label->orig_func_insn()->func_type()->return_type();
						string ret_reg;
						if (is_float_type(bbs->get_middle_ir(), ret_type))
							ret_reg = registers::f0;
						else if (is_double_type(bbs->get_middle_ir(), ret_type))
							ret_reg = "d0";
						else
							ret_reg = registers::a0;
						return make_pair(true, ret_reg);
					}
					if (reg->dereference())
						return make_pair(false, "");
					if (all_args_base_regs.find(reg->base_virtual_register_number()) != all_args_base_regs.end()) {
						int arg_pos = 0;
						for (; arg_pos < func_label->func_param_regs().size(); arg_pos++) {
							if (func_label->func_param_regs()[arg_pos] == reg->base_virtual_register_number())
								break;
						}
						if (arg_pos_2_phys_reg_num.find(arg_pos) != arg_pos_2_phys_reg_num.end())
							return make_pair(true, registers::argument_registers[arg_pos_2_phys_reg_num[arg_pos]]);
						else if (arg_pos_2_fp_phys_reg_num.find(arg_pos) != arg_pos_2_fp_phys_reg_num.end())
							return make_pair(true, registers::fp_argument_registers[arg_pos_2_fp_phys_reg_num[arg_pos]]);
						else if (arg_pos_2_stk_offs.find(arg_pos) != arg_pos_2_stk_offs.end())
							return make_pair(false, "");
					}
					if (coloring.find(reg->virtual_register_number()) != coloring.end()) {
						string mapped_reg = map_color_2_register(reg);
						add_register_if_nonvolatile(mapped_reg);
						return make_pair(true, mapped_reg);
					}
					return make_pair(false, "");
				};

				auto construct_push_list = [] (unordered_set<string> to_check) {
					vector<int> push_regs, fp_push_regs;
					for (const auto& reg_name : to_check) {
						if (reg_name[0] == 'r')
							push_regs.push_back(registers::register_name_2_number.at(reg_name));
						else
							fp_push_regs.push_back(registers::fp_register_name_2_number.at(reg_name));
					}
					sort(push_regs.begin(), push_regs.end());
					sort(fp_push_regs.begin(), fp_push_regs.end());

					string push_reglist, fp_push_reglist;
					for (int index = 0; index < push_regs.size(); index++) {
						if (!push_reglist.empty())
							push_reglist += ", ";
						push_reglist += registers::register_number_2_name.at(push_regs[index]);
					}
					if (!fp_push_regs.empty()) {
						int start = fp_push_regs[0], end = fp_push_regs[fp_push_regs.size() - 1];
						for (int i = start; i <= end; i++) {
							if (!fp_push_reglist.empty())
								fp_push_reglist += ", ";
							fp_push_reglist += registers::fp_register_number_2_name.at(i);
						}
					}

					return make_pair(push_reglist, fp_push_reglist);
				};

				auto preserve_stack_alignment = [&construct_push_list] (unordered_set<string> to_check) {
					int registers_left_for_alignment = to_check.size() % (STACK_ALIGNMENT >> 2);
					if (registers_left_for_alignment != 0) {
						for (const auto& [_, name] : registers::register_number_2_name) {
							if (to_check.find(name) == to_check.end() && name != registers::pc
								&& name != registers::sp) {
								to_check.insert(name);
								if (--registers_left_for_alignment == 0)
									break;
							}
						}
					}
					auto [push_reglist, fp_push_reglist] = construct_push_list(to_check);
					return make_tuple(to_check.size(), push_reglist, fp_push_reglist);
				};

				auto create_string_literal = [&] (shared_ptr<immediate_operand> rhs_imm) {
					string lab = FUNC_SCRATCH_PREFIX + to_string(scratch_counter++);
					aa->file_data.push_back(lab + ": .asciz " + rhs_imm->to_string());
					aa->file_data.push_back("");
					return lab;
				};

				auto create_fp_literal = [&] (shared_ptr<immediate_operand> rhs_imm, bool is_float) {
					string lab = FUNC_SCRATCH_PREFIX + to_string(scratch_counter++);
					string asm_lab = ASM_BRANCH_PREFIX + to_string(scratch_counter++);
					function_body.push_back({TAB, "b ", asm_lab});
					function_body.push_back({TAB, ".align ", is_float ? "2" : "3"});
					function_body.push_back({TAB, lab, is_float ? ": .float " : ": .double ",
						fp_max_prec_to_string(retrieve_immediate_operand<double>(bbs, rhs_imm))});
					function_body.push_back({asm_lab, ":"});
					return lab;
				};

				bool first_block = true;
				int total_function_insn_counter = 0, last_lit_pool_insertion = -LITERAL_POOL_RANGE;
				for (const auto& fb_index : reachable_bbs_sorted) {
					shared_ptr<basic_block> bb = bbs->get_basic_block(fb_index);
					for (int insn_counter = (first_block ? 1 : 0); insn_counter < bb->get_insns(bbs).size(); insn_counter++) {
						shared_ptr<insn> i = bb->get_insn(insn_counter);
						if (dl >= pass_manager::debug_level::KIND_DEBUG) {
							if (i->insn_kind() != insn::kind::KIND_LABEL)
								function_body.push_back({TAB, "; ", i->to_string()});
						}

						switch (i->insn_kind()) {
						case insn::kind::KIND_LABEL:
							function_body.push_back({static_pointer_cast<label_insn>(i)->label()->label_text(), ":"});
							break;
						case insn::kind::KIND_JUMP:
							function_body.push_back({TAB, "b ", static_pointer_cast<jump_insn>(i)->label()->label_text()});
							break;
						case insn::kind::KIND_ALIGN:
						case insn::kind::KIND_VAR:
							break;
						case insn::kind::KIND_ACCESS:
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							break;
						case insn::kind::KIND_PHI:
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							break;
						case insn::kind::KIND_GLOBAL_ARRAY:
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							break;
						case insn::kind::KIND_FUNCTION:
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							break;
						case insn::kind::KIND_CONDITIONAL: {
							shared_ptr<conditional_insn> ci = static_pointer_cast<conditional_insn>(i);
							shared_ptr<operand> src = ci->src();
							switch (src->operand_kind()) {
							case operand::kind::KIND_LABEL: {
								function_body.push_back({TAB, "ldr ", registers::s1, ", =",
									static_pointer_cast<label_operand>(src)->label_text()});
								function_body.push_back({TAB, "cmp ", registers::s1, ", #1"});
								function_body.push_back({TAB, "beq ", ci->branch()->label_text()});
							}
								break;
							case operand::kind::KIND_REGISTER: {
								shared_ptr<register_operand> reg_src = static_pointer_cast<register_operand>(src);
								string temp_reg = registers::s1;
								if (is_floating_point_type(bbs->get_middle_ir(), reg_src->register_type()))
									temp_reg = registers::f30;
								string reg_str = handle_rvalue_register(reg_src, temp_reg, temp_reg);
								function_body.push_back({TAB, "cmp ", reg_str, ", #1"});
								function_body.push_back({TAB, "beq ", ci->branch()->label_text()});
							}
								break;
							case operand::kind::KIND_IMMEDIATE: {
								shared_ptr<immediate_operand> io = static_pointer_cast<immediate_operand>(src);
								if (io->immediate_operand_kind() == immediate_operand::kind::KIND_STRING) {
									string lab = create_string_literal(io);
									function_body.push_back({TAB, "ldr ", registers::s1, ", =", lab});
									function_body.push_back({TAB, "cmp ", registers::s1, ", #1"});
									function_body.push_back({TAB, "beq ", ci->branch()->label_text()});
								}
								else {
									int val = retrieve_immediate_operand<int>(bbs, io);
									if (val != 0)
										function_body.push_back({TAB, "b ", ci->branch()->label_text()});
								}
							}
								break;
							default:
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}
						}
							break;
						case insn::kind::KIND_TRUNC: {
							shared_ptr<trunc_insn> ti = static_pointer_cast<trunc_insn>(i);
							int to_num_bits = 32 - (ti->to() << 3);
							string rhs;
							if (ti->lhs()->operand_kind() != operand::kind::KIND_REGISTER)
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							shared_ptr<register_operand> lhs_reg = static_pointer_cast<register_operand>(ti->lhs());
							if (is_floating_point_type(bbs->get_middle_ir(), lhs_reg->register_type()))
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							string trunc_insn = "asr";
							if (is_unsigned_type(bbs->get_middle_ir(), lhs_reg->register_type()))
								trunc_insn = "lsr";
							if (ti->to() == ti->from())
								to_num_bits = 0;
							pair<bool, string> purity = get_pure_register(lhs_reg);
							if (ti->rhs()->operand_kind() == operand::kind::KIND_REGISTER) {
								string mask = registers::s0;
								add_register_if_nonvolatile(mask);
								rhs = handle_rvalue_register(static_pointer_cast<register_operand>(ti->rhs()), registers::s1,
									registers::s1);
								function_body.push_back({TAB, "lsl ", mask, ", ", rhs, ", #", to_string(to_num_bits)});
								if (purity.first) {
									function_body.push_back({TAB, trunc_insn, " ", purity.second, ", ", mask, ", #",
										to_string(to_num_bits)});
								}
								else {
									function_body.push_back({TAB, trunc_insn, " ", mask, ", ", mask, ", #",
										to_string(to_num_bits)});
									handle_lvalue_register(lhs_reg, registers::s0, mask);
								}
							}
							else if (ti->rhs()->operand_kind() == operand::kind::KIND_LABEL) {
								rhs = registers::s1;
								string mask = registers::s0;
								add_register_if_nonvolatile(mask);
								function_body.push_back({TAB, "ldr ", registers::s1, ", =",
									static_pointer_cast<label_operand>(ti->rhs())->label_text()});
								function_body.push_back({TAB, "lsl ", mask, ", ", rhs, ", #", to_string(to_num_bits)});
								if (purity.first)
									function_body.push_back({TAB, trunc_insn, " ", purity.second, ", ", mask, ", #",
										to_string(to_num_bits)});
								else {
									function_body.push_back({TAB, trunc_insn, " ", mask, ", ", mask, ", #",
										to_string(to_num_bits)});
									handle_lvalue_register(lhs_reg, registers::s0, mask);
								}
							}
							else if (ti->rhs()->operand_kind() == operand::kind::KIND_IMMEDIATE) {
								shared_ptr<immediate_operand> io = static_pointer_cast<immediate_operand>(ti->rhs());
								if (io->immediate_operand_kind() == immediate_operand::kind::KIND_STRING) {
									rhs = registers::s1;
									string lab = create_string_literal(io);
									string mask = registers::s0;
									add_register_if_nonvolatile(mask);
									function_body.push_back({TAB, "ldr ", rhs, ", =", lab});
									function_body.push_back({TAB, "lsl ", mask, ", ", rhs, ", #", to_string(to_num_bits)});
									if (purity.first)
										function_body.push_back({TAB, trunc_insn, " ", purity.second, ", ", rhs, ", #", mask});
									else {
										function_body.push_back({TAB, trunc_insn, " ", mask, ", ", mask, ", #",
											to_string(to_num_bits)});
										handle_lvalue_register(lhs_reg, registers::s1, mask);
									}
								}
								else {
									int truncd_imm = retrieve_immediate_operand<int>(bbs, io) << to_num_bits;
									if (trunc_insn == "asr")
										truncd_imm = ((unsigned int) truncd_imm) >> 16;
									else
										truncd_imm = truncd_imm >> 16;
									if (purity.first) {
										function_body.push_back({TAB, "ldr ", purity.second, ", =", to_string(truncd_imm)});
									}
									else {
										function_body.push_back({TAB, "ldr ", registers::s1, ", =", to_string(truncd_imm)});
										add_register_if_nonvolatile(registers::s0);
										handle_lvalue_register(lhs_reg, registers::s0, registers::s1);
									}
								}
							}
							else
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						}
							break;
						case insn::kind::KIND_EXT: {
							shared_ptr<ext_insn> ei = static_pointer_cast<ext_insn>(i);
							string e_type;
							if (ei->ext_kind() == ext_insn::kind::KIND_SEXT)
								e_type += "sxt";
							else if (ei->ext_kind() == ext_insn::kind::KIND_ZEXT)
								e_type += "uxt";
							else
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							if (ei->to() == ei->from())
								break;
							if (ei->from() >= ei->to())
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							switch (ei->from()) {
							case 1:
								e_type += "b";
								break;
							case 2:
								e_type += "h";
								break;
							case 4:
							default:
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								break;
							}
							if (ei->lhs()->operand_kind() != operand::kind::KIND_REGISTER)
								bbs->report_internal("This should be unreachable", __FUNCTION__, __LINE__, __FILE__);
							shared_ptr<register_operand> lhs_reg = static_pointer_cast<register_operand>(ei->lhs());
							if (is_floating_point_type(bbs->get_middle_ir(), lhs_reg->register_type()))
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							pair<bool, string> purity = get_pure_register(lhs_reg);
							if (ei->rhs()->operand_kind() == operand::kind::KIND_REGISTER) {
								shared_ptr<register_operand> rhs_reg = static_pointer_cast<register_operand>(ei->rhs());
								string rhs_reg_str = handle_rvalue_register(rhs_reg, registers::s1, registers::s1);
								if (purity.first)
									function_body.push_back({TAB, e_type, " ", purity.second, ", ", rhs_reg_str});
								else {
									function_body.push_back({TAB, e_type, " ", registers::s0, ", ", rhs_reg_str});
									add_register_if_nonvolatile(registers::s0);
									handle_lvalue_register(lhs_reg, registers::s1, rhs_reg_str);
								}
							}
							else if (ei->rhs()->operand_kind() == operand::kind::KIND_LABEL) {
								shared_ptr<label_operand> lop = static_pointer_cast<label_operand>(ei->rhs());
								function_body.push_back({TAB, "ldr ", registers::s1, ", =", lop->label_text()});
								if (purity.first)
									function_body.push_back({TAB, e_type, " ", purity.second, ", ", registers::s1});
								else {
									function_body.push_back({TAB, e_type, " ", registers::s0, ", ", registers::s1});
									add_register_if_nonvolatile(registers::s0);
									handle_lvalue_register(lhs_reg, registers::s1, registers::s0);
								}
							}
							else if (ei->rhs()->operand_kind() == operand::kind::KIND_IMMEDIATE) {
								shared_ptr<immediate_operand> rhs_imm = static_pointer_cast<immediate_operand>(ei->rhs());
								if (rhs_imm->immediate_operand_kind() == immediate_operand::kind::KIND_STRING) {
									string lab = create_string_literal(rhs_imm);
									function_body.push_back({TAB, "ldr ", registers::s1, ", =", lab});
								}
								else {
									int imm = retrieve_immediate_operand<int>(bbs, rhs_imm);
									function_body.push_back({TAB, "ldr ", registers::s1, ", =", to_string(imm)});
								}
								if (purity.first)
									function_body.push_back({TAB, e_type, " ", purity.second, ", ", registers::s1});
								else {
									function_body.push_back({TAB, e_type, " ", registers::s0, ", ", registers::s1});
									add_register_if_nonvolatile(registers::s0);
									handle_lvalue_register(lhs_reg, registers::s1, registers::s0);
								}
							}
							else
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						}
							break;
						case insn::kind::KIND_RETURN:
							break;
						case insn::kind::KIND_UNARY: {
							shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
							shared_ptr<operand> dst = ui->dst_operand(), src = ui->src_operand();
							if (ui->unary_expr_kind() == unary_insn::kind::KIND_STK
								|| ui->unary_expr_kind() == unary_insn::kind::KIND_RESV)
								break;
							if (dst->operand_kind() != operand::kind::KIND_REGISTER)
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							shared_ptr<register_operand> dst_reg = static_pointer_cast<register_operand>(dst);
							pair<bool, string> purity = get_pure_register(dst_reg);
							switch (ui->unary_expr_kind()) {
							case unary_insn::kind::KIND_FMOV:
							case unary_insn::kind::KIND_FPLUS:
							case unary_insn::kind::KIND_DMOV:
							case unary_insn::kind::KIND_DPLUS:
							case unary_insn::kind::KIND_LMOV:
							case unary_insn::kind::KIND_LPLUS:
							case unary_insn::kind::KIND_IMOV:
							case unary_insn::kind::KIND_IPLUS:
							case unary_insn::kind::KIND_SMOV:
							case unary_insn::kind::KIND_SPLUS:
							case unary_insn::kind::KIND_CMOV:
							case unary_insn::kind::KIND_CPLUS: {
								string mov_insn, temp_reg, temp_reg2;
								bool is_fp = true;
								if (is_float_type(bbs->get_middle_ir(), dst_reg->register_type()))
									temp_reg = registers::f28, mov_insn = "vmov.F32", temp_reg2 = registers::f30;
								else if (is_double_type(bbs->get_middle_ir(), dst_reg->register_type()))
									temp_reg = "d14", mov_insn = "vmov.F64", temp_reg2 = "d15";
								else
									temp_reg = registers::s1, mov_insn = "mov", temp_reg2 = registers::s0, is_fp = false;
								add_register_if_nonvolatile(temp_reg);
								add_register_if_nonvolatile(temp_reg2);
								add_register_if_nonvolatile(registers::s0);
								if (src->operand_kind() == operand::kind::KIND_REGISTER) {
									shared_ptr<register_operand> src_reg = static_pointer_cast<register_operand>(src);
									string rhs_reg_str = handle_rvalue_register(src_reg, temp_reg, registers::s1);
									if (purity.first) {
										if (purity.second != rhs_reg_str)
											function_body.push_back({TAB, mov_insn, " ", purity.second, ", ", rhs_reg_str});
									}
									else
										handle_lvalue_register(dst_reg, registers::s0, rhs_reg_str);
								}
								else if (src->operand_kind() == operand::kind::KIND_LABEL) {
									shared_ptr<label_operand> lop = static_pointer_cast<label_operand>(src);
									if (mov_insn != "mov")
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									if (purity.first)
										function_body.push_back({TAB, "ldr ", purity.second, ", =", lop->label_text()});
									else {
										function_body.push_back({TAB, "ldr ", temp_reg, ", =", lop->label_text()});
										handle_lvalue_register(dst_reg, registers::s0, temp_reg);
									}
								}
								else if (src->operand_kind() == operand::kind::KIND_IMMEDIATE) {
									shared_ptr<immediate_operand> io = static_pointer_cast<immediate_operand>(src);
									if (io->immediate_operand_kind() == immediate_operand::kind::KIND_STRING) {
										string lab = create_string_literal(io);
										if (purity.first)
											function_body.push_back({TAB, "ldr ", purity.second, ", =", lab});
										else {
											function_body.push_back({TAB, "ldr ", temp_reg, ", =", lab});
											add_register_if_nonvolatile(registers::s0);
											handle_lvalue_register(dst_reg, registers::s0, temp_reg);
										}
									}
									else {
										if (is_fp) {
											string fp_ext = get_fp_ext(temp_reg);
											string temp_lab = create_fp_literal(io, fp_ext == "F32");
											if (fp_ext.empty()) {
												bbs->report_internal("This should be unreachable.",
													__FUNCTION__, __LINE__, __FILE__);
											}
											if (purity.first) {
												function_body.push_back({TAB, "vldr.", fp_ext, " ", purity.second,
													", ", temp_lab});
											}
											else {
												function_body.push_back({TAB, "vldr.", fp_ext, " ", temp_reg,
													", ", temp_lab});
												add_register_if_nonvolatile(registers::s0);
												handle_lvalue_register(dst_reg, registers::s0, temp_reg);
											}
										}
										else {
											if (purity.first) {
												function_body.push_back({TAB, "ldr ", purity.second, ", =",
													to_string(retrieve_immediate_operand<int>(bbs, io))});
											}
											else {
												function_body.push_back({TAB, "ldr ", temp_reg, ", =", to_string(
													retrieve_immediate_operand<int>(bbs, io))});
												add_register_if_nonvolatile(registers::s0);
												handle_lvalue_register(dst_reg, registers::s0, temp_reg);
											}
										}
									}
								}
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}
								break;
							case unary_insn::kind::KIND_LMINUS:
							case unary_insn::kind::KIND_IMINUS:
							case unary_insn::kind::KIND_SMINUS:
							case unary_insn::kind::KIND_CMINUS:
							case unary_insn::kind::KIND_FMINUS:
							case unary_insn::kind::KIND_DMINUS: {
								string neg_insn, temp_reg, temp_reg2, ldr_insn, mov_insn;
								bool is_fp = true;
								if (is_float_type(bbs->get_middle_ir(), dst_reg->register_type())) {
									neg_insn = "vneg.F32", temp_reg = registers::f28, temp_reg2 = registers::f30,
										ldr_insn = "vldr.F32", mov_insn = "vmov.F32";
								}
								else if (is_double_type(bbs->get_middle_ir(), dst_reg->register_type())) {
									neg_insn = "vneg.F64", temp_reg = "d14", temp_reg2 = "d15",
										ldr_insn = "vldr.F64", mov_insn = "vmov.F64";
								}
								else {
									is_fp = false, temp_reg = registers::s1, temp_reg2 = registers::s0, ldr_insn = "ldr",
										mov_insn = "mov";
								}
								add_register_if_nonvolatile(temp_reg);
								add_register_if_nonvolatile(temp_reg2);
								add_register_if_nonvolatile(registers::s0);
								if (src->operand_kind() == operand::kind::KIND_REGISTER) {
									shared_ptr<register_operand> src_reg = static_pointer_cast<register_operand>(src);
									string rhs_reg_str = handle_rvalue_register(src_reg, temp_reg, registers::s0);
									if (purity.first) {
										if (is_fp)
											function_body.push_back({TAB, neg_insn, " ", purity.second, ", ", rhs_reg_str});
										else {
											function_body.push_back({TAB, "rsbs ", purity.second, ", ", rhs_reg_str,
											 ", #0"});
										}
									}
									else {
										if (is_fp)
											function_body.push_back({TAB, neg_insn, " ", temp_reg2, ", ", rhs_reg_str});
										else
											function_body.push_back({TAB, "rsbs ", temp_reg2, ", ", rhs_reg_str, ", #0"});
										handle_lvalue_register(dst_reg, temp_reg, temp_reg2);
									}
								}
								else if (src->operand_kind() == operand::kind::KIND_IMMEDIATE) {
									shared_ptr<immediate_operand> src_imm = static_pointer_cast<immediate_operand>(src);
									if (src_imm->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									if (is_fp) {
										string temp_lab = create_fp_literal(
											make_shared<immediate_operand>(src_imm->immediate_type(),
												-retrieve_immediate_operand<double>(bbs, src_imm)),
											ldr_insn == "vldr.F32");
										function_body.push_back({TAB, ldr_insn, " ", temp_reg, ", ", temp_lab});
									}
									else {
										int imm = -retrieve_immediate_operand<int>(bbs, src_imm);
										function_body.push_back({TAB, ldr_insn, " ", temp_reg, ", =", to_string(imm)});
									}
									if (purity.first)
										function_body.push_back({TAB, mov_insn, " ", purity.second, ", ", temp_reg});
									else
										handle_lvalue_register(dst_reg, registers::s0, temp_reg);
								}
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}
								break;
							case unary_insn::kind::KIND_LBNOT:
							case unary_insn::kind::KIND_IBNOT:
							case unary_insn::kind::KIND_SBNOT:
							case unary_insn::kind::KIND_CBNOT: {
								string temp_reg = registers::s1, temp_reg2 = registers::s0;
								add_register_if_nonvolatile(temp_reg);
								add_register_if_nonvolatile(temp_reg2);
								if (src->operand_kind() == operand::kind::KIND_REGISTER) {
									shared_ptr<register_operand> src_reg = static_pointer_cast<register_operand>(src);
									string rhs_reg_str = handle_rvalue_register(src_reg, temp_reg, temp_reg);
									if (purity.first)
										function_body.push_back({TAB, "mvn ", purity.second, ", ", rhs_reg_str});
									else {
										function_body.push_back({TAB, "mvn ", temp_reg, ", ", rhs_reg_str});
										handle_lvalue_register(dst_reg, temp_reg2, temp_reg);
									}
								}
								else if (src->operand_kind() == operand::kind::KIND_IMMEDIATE) {
									shared_ptr<immediate_operand> src_imm = static_pointer_cast<immediate_operand>(src);
									if (src_imm->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									int imm = retrieve_immediate_operand<int>(bbs, src_imm);
									if (purity.first)
										function_body.push_back({TAB, "ldr ", purity.second, ", =", to_string(~imm)});
									else {
										function_body.push_back({TAB, "ldr ", temp_reg, ", =", to_string(~imm)});
										handle_lvalue_register(dst_reg, temp_reg2, temp_reg);
									}
								}
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}
								break;
							case unary_insn::kind::KIND_STK:
							case unary_insn::kind::KIND_RESV:
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								break;
							case unary_insn::kind::KIND_FTOL:
							case unary_insn::kind::KIND_FTOI:
							case unary_insn::kind::KIND_FTOS:
							case unary_insn::kind::KIND_FTOC:
							case unary_insn::kind::KIND_FTOUL:
							case unary_insn::kind::KIND_FTOUI:
							case unary_insn::kind::KIND_FTOUS:
							case unary_insn::kind::KIND_FTOUC:
							case unary_insn::kind::KIND_DTOL:
							case unary_insn::kind::KIND_DTOI:
							case unary_insn::kind::KIND_DTOS:
							case unary_insn::kind::KIND_DTOC:
							case unary_insn::kind::KIND_DTOUL:
							case unary_insn::kind::KIND_DTOUI:
							case unary_insn::kind::KIND_DTOUS:
							case unary_insn::kind::KIND_DTOUC: {
								string rty, fext, temp_reg = registers::f28, rhs_temp_reg;
								if (ui->unary_expr_kind() == unary_insn::kind::KIND_FTOL
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_FTOI
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_FTOS
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_FTOC)
									rty = "S32", fext = "F32", rhs_temp_reg = registers::f30;
								else if (ui->unary_expr_kind() == unary_insn::kind::KIND_FTOUL
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_FTOUI
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_FTOUS
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_FTOUC)
									rty = "U32", fext = "F32", rhs_temp_reg = registers::f30;
								else if (ui->unary_expr_kind() == unary_insn::kind::KIND_DTOL
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_DTOI
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_DTOS
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_DTOC)
									rty = "S32", fext = "F64", rhs_temp_reg = "d15";
								else if (ui->unary_expr_kind() == unary_insn::kind::KIND_DTOUL
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_DTOUI
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_DTOUS
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_DTOUC)
									rty = "U32", fext = "F64", rhs_temp_reg = "d15";
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								string cvt_insn = "vcvt." + rty + "." + fext;
								add_register_if_nonvolatile(temp_reg);
								if (src->operand_kind() == operand::kind::KIND_REGISTER) {
									shared_ptr<register_operand> src_reg = static_pointer_cast<register_operand>(src);
									string rhs_reg_str = handle_rvalue_register(src_reg, rhs_temp_reg, registers::s1);
									function_body.push_back({TAB, cvt_insn, " ", temp_reg, ", ", rhs_reg_str});
									add_register_if_nonvolatile(rhs_temp_reg);
									if (purity.first)
										function_body.push_back({TAB, "vmov ", purity.second, ", ", temp_reg});
									else {
										add_register_if_nonvolatile(registers::s1);
										function_body.push_back({TAB, "vmov ", registers::s1, ", ", temp_reg});
										handle_lvalue_register(dst_reg, registers::s0, registers::s1);
									}
								}
								else if (src->operand_kind() == operand::kind::KIND_IMMEDIATE) {
									shared_ptr<immediate_operand> imm_src = static_pointer_cast<immediate_operand>(src);
									if (imm_src->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									int imm = retrieve_immediate_operand<int>(bbs, imm_src);
									if (purity.first)
										function_body.push_back({TAB, "ldr ", purity.second, ", =", to_string(imm)});
									else {
										add_register_if_nonvolatile(registers::s0);
										function_body.push_back({TAB, "ldr ", registers::s1, ", =", to_string(imm)});
										handle_lvalue_register(dst_reg, registers::s0, registers::s1);
									}
								}
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}
								break;
							case unary_insn::kind::KIND_CTOF:
							case unary_insn::kind::KIND_STOF:
							case unary_insn::kind::KIND_ITOF:
							case unary_insn::kind::KIND_LTOF:
							case unary_insn::kind::KIND_UCTOF:
							case unary_insn::kind::KIND_USTOF:
							case unary_insn::kind::KIND_UITOF:
							case unary_insn::kind::KIND_ULTOF:
							case unary_insn::kind::KIND_CTOD:
							case unary_insn::kind::KIND_STOD:
							case unary_insn::kind::KIND_ITOD:
							case unary_insn::kind::KIND_LTOD:
							case unary_insn::kind::KIND_UCTOD:
							case unary_insn::kind::KIND_USTOD:
							case unary_insn::kind::KIND_UITOD:
							case unary_insn::kind::KIND_ULTOD: {
								string rty, fext, temp_reg = registers::f28, ldr_insn, lhs_temp_reg;
								if (ui->unary_expr_kind() == unary_insn::kind::KIND_CTOF
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_STOF
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_ITOF
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_LTOF)
									rty = "S32", fext = "F32", ldr_insn = "vldr.F32", lhs_temp_reg = registers::f30;
								else if (ui->unary_expr_kind() == unary_insn::kind::KIND_UCTOF
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_USTOF
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_UITOF
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_ULTOF)
									rty = "U32", fext = "F32", ldr_insn = "vldr.F32", lhs_temp_reg = registers::f30;
								else if (ui->unary_expr_kind() == unary_insn::kind::KIND_CTOD
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_STOD
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_ITOD
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_LTOD)
									rty = "S32", fext = "F64", ldr_insn = "vldr.F64", lhs_temp_reg = "d15";
								else if (ui->unary_expr_kind() == unary_insn::kind::KIND_UCTOD
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_USTOD
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_UITOD
									|| ui->unary_expr_kind() == unary_insn::kind::KIND_ULTOD)
									rty = "U32", fext = "F64", ldr_insn = "vldr.F64", lhs_temp_reg = "d15";
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								add_register_if_nonvolatile(temp_reg);
								string cvt_insn = "vcvt." + fext + "." + rty;
								if (src->operand_kind() == operand::kind::KIND_REGISTER) {
									shared_ptr<register_operand> src_reg = static_pointer_cast<register_operand>(src);
									string rhs_reg_str = handle_rvalue_register(src_reg, registers::s1, registers::s1);
									function_body.push_back({TAB, "vmov ", temp_reg, ", ", rhs_reg_str});
									if (purity.first)
										function_body.push_back({TAB, cvt_insn, " ", purity.second, ", ", temp_reg});
									else {
										add_register_if_nonvolatile(lhs_temp_reg);
										function_body.push_back({TAB, cvt_insn, " ", lhs_temp_reg, ", ", temp_reg});
										handle_lvalue_register(dst_reg, registers::s1, lhs_temp_reg);
									}
								}
								else if (src->operand_kind() == operand::kind::KIND_IMMEDIATE) {
									shared_ptr<immediate_operand> src_imm = static_pointer_cast<immediate_operand>(src);
									if (src_imm->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									string temp_lab = create_fp_literal(src_imm, fext == "F32");
									if (purity.first)
										function_body.push_back({TAB, ldr_insn, " ", purity.second, ", ", temp_lab});
									else {
										function_body.push_back({TAB, ldr_insn, " ", temp_reg, ", ", temp_lab});
										handle_lvalue_register(dst_reg, registers::s1, temp_reg);
									}
								}
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}
								break;
							case unary_insn::kind::KIND_DTOF:
							case unary_insn::kind::KIND_FTOD: {
								string cvt_insn = "vcvt.F32.F64", ldr_insn = "vldr.F32",
									rvalue_temp_reg = "d15", lvalue_temp_reg = registers::f28;
								if (ui->unary_expr_kind() == unary_insn::kind::KIND_FTOD) {
									cvt_insn = "vcvt.F64.F32", rvalue_temp_reg = registers::f30,
										 ldr_insn = "vldr.F64", lvalue_temp_reg = "d14";
								}
								if (src->operand_kind() == operand::kind::KIND_REGISTER) {
									shared_ptr<register_operand> src_reg = static_pointer_cast<register_operand>(src);
									add_register_if_nonvolatile(rvalue_temp_reg);
									string rhs_reg_str = handle_rvalue_register(src_reg, rvalue_temp_reg, registers::s1);
									if (purity.first) {
										function_body.push_back({TAB, cvt_insn, " ", purity.second, ", ", rhs_reg_str});
									}
									else {
										add_register_if_nonvolatile(lvalue_temp_reg);
										function_body.push_back({TAB, cvt_insn, " ", lvalue_temp_reg, ", ", rhs_reg_str});
										handle_lvalue_register(dst_reg, registers::s1, lvalue_temp_reg);
									}
								}
								else if (src->operand_kind() == operand::kind::KIND_IMMEDIATE) {
									shared_ptr<immediate_operand> imm_src = static_pointer_cast<immediate_operand>(src);
									if (imm_src->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									string temp_lab = create_fp_literal(imm_src, ldr_insn == "vldr.F32");
									if (purity.first)
										function_body.push_back({TAB, ldr_insn, " ", purity.second, ", ", temp_lab});
									else {
										add_register_if_nonvolatile(lvalue_temp_reg);
										function_body.push_back({TAB, ldr_insn, " ", lvalue_temp_reg, ", ", temp_lab});
										handle_lvalue_register(dst_reg, registers::s1, lvalue_temp_reg);
									}
								}
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}
								break;
							case unary_insn::kind::KIND_LNOT: {
								string temp_reg = registers::s1, temp_reg2 = registers::s0;
								add_register_if_nonvolatile(temp_reg);
								add_register_if_nonvolatile(temp_reg2);
								if (src->operand_kind() == operand::kind::KIND_REGISTER) {
									shared_ptr<register_operand> src_reg = static_pointer_cast<register_operand>(src);
									string rhs_reg_str = handle_rvalue_register(src_reg, temp_reg, temp_reg);
									if (purity.first)
										function_body.push_back({TAB, "eor ", purity.second, ", ", rhs_reg_str, ", #1"});
									else {
										function_body.push_back({TAB, "eor ", temp_reg, ", ", rhs_reg_str, ", #1"});
										handle_lvalue_register(dst_reg, temp_reg2, temp_reg);
									}
								}
								else if (src->operand_kind() == operand::kind::KIND_IMMEDIATE) {
									shared_ptr<immediate_operand> src_imm = static_pointer_cast<immediate_operand>(src);
									if (src_imm->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									bool imm = !retrieve_immediate_operand<int>(bbs, src_imm);
									if (purity.first)
										function_body.push_back({TAB, "ldr ", purity.second, ", =", to_string(imm)});
									else {
										function_body.push_back({TAB, "ldr ", temp_reg, ", =", to_string(imm)});
										handle_lvalue_register(dst_reg, temp_reg2, temp_reg);
									}
								}
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}
								break;
							default:
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}
						}
							break;
						case insn::kind::KIND_MEMCPY: {
							shared_ptr<memcpy_insn> mi = static_pointer_cast<memcpy_insn>(i);
							shared_ptr<operand> src = mi->source(), dst = mi->destination();
							int size = mi->size();

							auto memcpy_operand_to_reg = [&] (shared_ptr<operand> op, string reg_str) {
								if (op->operand_kind() == operand::kind::KIND_REGISTER) {
									shared_ptr<register_operand> reg_op = static_pointer_cast<register_operand>(op);
									reg_str = handle_rvalue_register(reg_op, reg_str, reg_str);
								}
								else if (op->operand_kind() == operand::kind::KIND_LABEL) {
									shared_ptr<label_operand> lab_op = static_pointer_cast<label_operand>(op);
									function_body.push_back({TAB, "ldr ", reg_str, ", =", lab_op->label_text()});
								}
								else if (op->operand_kind() == operand::kind::KIND_IMMEDIATE) {
									shared_ptr<immediate_operand> imm_op = static_pointer_cast<immediate_operand>(op);
									if (imm_op->immediate_operand_kind() != immediate_operand::kind::KIND_STRING)
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									string lab = create_string_literal(imm_op);
									function_body.push_back({TAB, "ldr ", reg_str, ", =", lab});
								}
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return reg_str;
							};

							bool r0_temp = true;
							string lhs_reg_str = memcpy_operand_to_reg(dst, registers::s0);
							string rhs_reg_str = memcpy_operand_to_reg(src, registers::s1);
							string temp_reg;
							add_register_if_nonvolatile(registers::s0);
							add_register_if_nonvolatile(registers::s1);

							if (rhs_reg_str != registers::s1)
								r0_temp = false, temp_reg = registers::s1;
							else if (lhs_reg_str != registers::s0)
								r0_temp = false, temp_reg = registers::s0;
							else
								temp_reg = registers::a0;

							if (r0_temp)
								function_body.push_back({TAB, "push {", registers::a0, "}"});

							for (int i = 0; i < size; i++) {
								function_body.push_back({TAB, "ldrb ", temp_reg, ", [", rhs_reg_str, "], #1"});
								function_body.push_back({TAB, "strb ", temp_reg, ", [", lhs_reg_str, "], #1"});
							}

							if (0 <= size && size <= 4095) {
								if (lhs_reg_str != registers::s0)
									function_body.push_back({TAB, "sub ", lhs_reg_str, ", ", lhs_reg_str, ", #", to_string(size)});
								if (rhs_reg_str != registers::s1)
									function_body.push_back({TAB, "sub ", rhs_reg_str, ", ", rhs_reg_str, ", #", to_string(size)});
							}
							else {
								function_body.push_back({TAB, "ldr ", temp_reg, ", =", to_string(size)});
								if (lhs_reg_str != registers::s0)
									function_body.push_back({TAB, "sub ", lhs_reg_str, ", ", lhs_reg_str, ", ", temp_reg});
								if (rhs_reg_str != registers::s1)
									function_body.push_back({TAB, "sub ", rhs_reg_str, ", ", rhs_reg_str, ", #", temp_reg});
							}

							if (r0_temp)
								function_body.push_back({TAB, "pop {", registers::a0, "}"});
						}
							break;
						case insn::kind::KIND_CALL: {
							shared_ptr<call_insn> ci = static_pointer_cast<call_insn>(i);
							shared_ptr<function_type> ft = ci->called_function_type();
							shared_ptr<operand> func_op = ci->function_operand(), dst_op = ci->dest_operand();
							vector<shared_ptr<operand>> args_ops = ci->argument_list();

							string ret_reg, temp_ret_reg, ret_mov_insn;
							if (is_float_type(bbs->get_middle_ir(), ft->return_type()))
								temp_ret_reg = registers::f28, ret_reg = registers::f0, ret_mov_insn = "vmov.F32";
							else if (is_double_type(bbs->get_middle_ir(), ft->return_type()))
								temp_ret_reg = "d14", ret_reg = "d0", ret_mov_insn = "vmov.F64";
							else
								temp_ret_reg = registers::s1, ret_reg = registers::a0, ret_mov_insn = "mov";
							unordered_set<string> need_to_save;
							unordered_set<string> arg_regs_used, fp_arg_regs_used;

							for (const auto& [_, arg_reg_index] : arg_pos_2_phys_reg_num)
								arg_regs_used.insert(registers::argument_registers.at(arg_reg_index));
							for (const auto& [_, fp_arg_reg_index] : arg_pos_2_fp_phys_reg_num) {
								int true_index = fp_arg_reg_index << 1;
								string fcurr = registers::fp_register_number_2_name.at(true_index),
									fnext = registers::fp_register_number_2_name.at(true_index + 1);
								fp_arg_regs_used.insert(fcurr);
								fp_arg_regs_used.insert(fnext);
							}
							need_to_save.insert(arg_regs_used.begin(), arg_regs_used.end());
							need_to_save.insert(fp_arg_regs_used.begin(), fp_arg_regs_used.end());

							for (int index = 0; index < registers::register_volatility_map.size(); index++) {
								if (!registers::register_volatility_map[index])
									continue;

								string reg_name = registers::register_number_2_name.at(index);
								if (arg_regs_used.find(reg_name) == arg_regs_used.end()
									|| find(registers::scratch_registers.begin(), registers::scratch_registers.end(), reg_name)
										!= registers::scratch_registers.end()
									|| reg_name == registers::pc)
									continue;
								need_to_save.insert(reg_name);
							}
							for (int index = 0; index < registers::fp_register_volatility_map.size(); index++) {
								if (!registers::fp_register_volatility_map[index])
									continue;

								string reg_name = registers::fp_register_number_2_name.at(index);
								if (fp_arg_regs_used.find(reg_name) == fp_arg_regs_used.end()
									|| find(registers::fp_scratch_registers.begin(), registers::fp_scratch_registers.end(),
										reg_name) != registers::fp_scratch_registers.end())
									continue;
								need_to_save.insert(reg_name);
								if (index != registers::fp_register_volatility_map.size() - 1)
									need_to_save.insert(registers::fp_register_number_2_name.at(index + 1));
							}

							auto [num_regs_pushed, temp_push_reglist, fp_temp_push_reglist] = preserve_stack_alignment(need_to_save);

							if (!temp_push_reglist.empty())
								function_body.push_back({TAB, "push {", temp_push_reglist, "}"});
							if (!fp_temp_push_reglist.empty())
								function_body.push_back({TAB, "vpush {", fp_temp_push_reglist, "}"});

							vector<shared_ptr<type>> arg_types;
							for (shared_ptr<operand> arg : args_ops) {
								if (arg->operand_kind() == operand::kind::KIND_REGISTER)
									arg_types.push_back(static_pointer_cast<register_operand>(arg)->register_type());
								else if (arg->operand_kind() == operand::kind::KIND_LABEL)
									arg_types.push_back(static_pointer_cast<label_operand>(arg)->result_type());
								else if (arg->operand_kind() == operand::kind::KIND_IMMEDIATE)
									arg_types.push_back(static_pointer_cast<immediate_operand>(arg)->immediate_type());
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}

							auto [called_func_arg_pos_2_reg, called_func_arg_pos_2_fp_reg, called_func_arg_pos_2_stk_offs,
							     called_func_arg_stk_space] = calc_arg_locs(arg_types, ft->variadic());
							called_func_arg_stk_space = (called_func_arg_stk_space + STACK_ALIGNMENT - 1) & ~(STACK_ALIGNMENT - 1);

							bool no_stk = true;
							int func_loc = -1;

							if (func_op->operand_kind() == operand::kind::KIND_REGISTER) {
								shared_ptr<register_operand> reg_op = static_pointer_cast<register_operand>(func_op);
								for (const auto& reg_num : func_label->func_param_regs()) {
									if (reg_num == reg_op->base_virtual_register_number()) {
										no_stk = false;
										break;
									}
								}
							}

							string func_op_temp_reg;
							if (!no_stk) {
								func_loc = called_func_arg_stk_space;
								called_func_arg_stk_space += 8;
								if (func_op->operand_kind() == operand::kind::KIND_REGISTER) {
									shared_ptr<register_operand> reg_op = static_pointer_cast<register_operand>(func_op);
									func_op_temp_reg = handle_rvalue_register(reg_op, registers::s1, registers::s1);
								}
								else if (func_op->operand_kind() == operand::kind::KIND_LABEL) {
									shared_ptr<label_operand> lab_op = static_pointer_cast<label_operand>(func_op);
									add_register_if_nonvolatile(registers::s1);
									function_body.push_back({TAB, "ldr ", registers::s1, ", =", lab_op->label_text()});
									func_op_temp_reg = registers::s1;
								}
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}

							if (0 < called_func_arg_stk_space && called_func_arg_stk_space <= 255) {
								function_body.push_back({TAB, "sub ", registers::sp, ", ", registers::sp, ", #",
									to_string(called_func_arg_stk_space)});
							}
							else if (called_func_arg_stk_space != 0) {
								function_body.push_back({TAB, "ldr ", registers::s0, ", =", to_string(called_func_arg_stk_space)});
								function_body.push_back({TAB, "sub ", registers::sp, ", ", registers::sp, ", ", registers::s0});
								add_register_if_nonvolatile(registers::s0);
							}

							if (!no_stk && func_loc != -1) {
								if (0 <= func_loc && func_loc <= 255)
									function_body.push_back({TAB, "str ", func_op_temp_reg, ", [", registers::sp, ", #", to_string(func_loc), "]"});
								else {
									function_body.push_back({TAB, "ldr ", registers::s0, ", =", to_string(func_loc)});
									function_body.push_back({TAB, "str ", func_op_temp_reg, ", [", registers::sp, 
										", ", registers::s0, "]"});
									add_register_if_nonvolatile(registers::s0);
								}
							}

							int arg_save_offs = 0;
							unordered_map<int, int> arg_pos_2_saved_offs;
							for (int index = 0; index < arg_types.size(); index++) {
								shared_ptr<type> t = arg_types[index];
								shared_ptr<operand> arg = args_ops[index];
								if (called_func_arg_pos_2_reg.find(index) != called_func_arg_pos_2_reg.end()) {
									if (arg_save_offs % SIZEOF_INT != 0)
										arg_save_offs += SIZEOF_INT - (arg_save_offs % SIZEOF_INT);
									arg_pos_2_saved_offs[index] = arg_save_offs;
									arg_save_offs += SIZEOF_INT;
								}
								else if (called_func_arg_pos_2_fp_reg.find(index) != called_func_arg_pos_2_fp_reg.end()) {
									int mod_by = SIZEOF_FLOAT;
									if (is_double_type(bbs->get_middle_ir(), t))
										mod_by = SIZEOF_DOUBLE;
									if (arg_save_offs % mod_by != 0)
										arg_save_offs += mod_by - (arg_save_offs % mod_by);
									arg_pos_2_saved_offs[index] = arg_save_offs;
									arg_save_offs += mod_by;
								}
							}
							if (arg_save_offs % STACK_ALIGNMENT != 0)
								arg_save_offs += STACK_ALIGNMENT - (arg_save_offs % STACK_ALIGNMENT);

							if (arg_save_offs != 0) {
								if (0 < arg_save_offs && arg_save_offs <= 4095) {
									function_body.push_back({TAB, "sub ", registers::sp, ", ", registers::sp, ", #",
										to_string(arg_save_offs)});
								}
								else {
									function_body.push_back({TAB, "ldr ", registers::s1, ", =", to_string(arg_save_offs)});
									function_body.push_back({TAB, "sub ", registers::sp, ", ", registers::sp, ", ",
										registers::s1});
								}
							}

							for (int index = arg_types.size() - 1; index >= 0; index--) {
								shared_ptr<type> t = arg_types[index];
								shared_ptr<operand> arg = args_ops[index];
								string l_insn = get_ls_insn(t, true), s_insn = get_ls_insn(t, false); 
								bool is_fp_l_insn = l_insn[0] == 'v', is_fp_s_insn = s_insn[0] == 'v';
								if (called_func_arg_pos_2_reg.find(index) != called_func_arg_pos_2_reg.end()) {
									string arg_reg = registers::argument_registers[called_func_arg_pos_2_reg[index]];
									string to_save;
									if (arg->operand_kind() == operand::kind::KIND_REGISTER) {
										shared_ptr<register_operand> reg_op = static_pointer_cast<register_operand>(arg);
										string temp_reg = registers::s1;
										string rhs_reg_str = handle_rvalue_register(reg_op, temp_reg, temp_reg);
										add_register_if_nonvolatile(temp_reg);
										to_save = rhs_reg_str;
									}
									else if (arg->operand_kind() == operand::kind::KIND_LABEL) {
										shared_ptr<label_operand> lab_op = static_pointer_cast<label_operand>(arg);
										function_body.push_back({TAB, "ldr ", registers::s1, ", =",
											lab_op->label_text()});
										to_save = registers::s1;
									}
									else if (arg->operand_kind() == operand::kind::KIND_IMMEDIATE) {
										shared_ptr<immediate_operand> imm_op = static_pointer_cast<immediate_operand>(arg);
										if (imm_op->immediate_operand_kind() == immediate_operand::kind::KIND_STRING) {
											string lab_text = create_string_literal(imm_op);
											function_body.push_back({TAB, "ldr ", registers::s1, ", =",
												lab_text});
											to_save = registers::s1;
										}
										else {
											int imm = retrieve_immediate_operand<int>(bbs, imm_op);
											function_body.push_back({TAB, "ldr ", registers::s1, ", =",
												to_string(imm)});
											to_save = registers::s1;
										}
									}
									else
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);

									if (arg_pos_2_saved_offs.find(index) == arg_pos_2_saved_offs.end())
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									int offs = arg_pos_2_saved_offs[index];
									if (0 <= offs && offs <= 255) {
										function_body.push_back({TAB, s_insn, " ", to_save, ", [", registers::sp, ", #",
											to_string(offs), "]"});
									}
									else {
										add_register_if_nonvolatile(registers::s0);
										function_body.push_back({TAB, "ldr ", registers::s0, ", =", to_string(offs)});
										function_body.push_back({TAB, s_insn, " ", to_save, ", [", registers::sp,
											", ", registers::s0, "]"});
									}
								}
								else if (called_func_arg_pos_2_fp_reg.find(index) != called_func_arg_pos_2_fp_reg.end()) {
									int arg_num = called_func_arg_pos_2_fp_reg[index];
									string arg_reg, mov_insn, temp_reg;
									if (is_double_type(bbs->get_middle_ir(), t))
										arg_reg = "d" + to_string(arg_num), mov_insn = "vmov.F64", temp_reg = "d14";
									else if (is_float_type(bbs->get_middle_ir(), t))
										arg_reg = "s" + to_string(arg_num << 1), mov_insn = "vmov.F32", temp_reg = registers::f28;
									else
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);

									string to_save;
									if (arg->operand_kind() == operand::kind::KIND_REGISTER) {
										shared_ptr<register_operand> reg_op = static_pointer_cast<register_operand>(arg);
										string rhs_reg_str = handle_rvalue_register(reg_op, temp_reg, registers::s1);
										add_register_if_nonvolatile(temp_reg);
										add_register_if_nonvolatile(registers::s1);
										to_save = rhs_reg_str;
									}
									else if (arg->operand_kind() == operand::kind::KIND_IMMEDIATE) {
										shared_ptr<immediate_operand> imm_op = static_pointer_cast<immediate_operand>(arg);
										if (imm_op->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
											bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
										string temp_lab = create_fp_literal(imm_op, l_insn == "vldr.F32");
										add_register_if_nonvolatile(temp_reg);
										function_body.push_back({TAB, l_insn, " ", temp_reg, ", ", temp_lab});
										to_save = temp_reg;
									}
									else
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									if (arg_pos_2_saved_offs.find(index) == arg_pos_2_saved_offs.end())
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									int offs = arg_pos_2_saved_offs[index];
									if (0 <= offs && offs <= 255) {
										function_body.push_back({TAB, s_insn, " ", to_save, ", [", registers::sp, ", #",
											to_string(offs), "]"});
									}
									else {
										add_register_if_nonvolatile(registers::s0);
										function_body.push_back({TAB, "ldr ", registers::s0, ", =", to_string(offs)});
										function_body.push_back({TAB, "add ", registers::s0, ", ", registers::s0, ", ",
											registers::sp});
										function_body.push_back({TAB, s_insn, " ", to_save, ", [", registers::s0, "]"});
									}
								}
								else if (called_func_arg_pos_2_stk_offs.find(index) != called_func_arg_pos_2_stk_offs.end()) {
									int stk_offs = called_func_arg_pos_2_stk_offs[index] + arg_save_offs;
									if (is_struct_type(bbs->get_middle_ir(), t)) {
										shared_ptr<size_data> sd = spectre_sizeof(bbs->get_middle_ir(), t);
										string temp_reg = registers::s0;
										add_register_if_nonvolatile(temp_reg);
										if (arg->operand_kind() == operand::kind::KIND_REGISTER) {
											shared_ptr<register_operand> reg_op = static_pointer_cast<register_operand>(arg);
											temp_reg = handle_rvalue_register(reg_op, temp_reg, temp_reg);
										}
										else if (arg->operand_kind() == operand::kind::KIND_LABEL) {
											shared_ptr<label_operand> lab_op = static_pointer_cast<label_operand>(arg);
											function_body.push_back({TAB, "ldr ", temp_reg, ", =", lab_op->label_text()});
										}
										else
											bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
										if (stk_offs != 0) {
											if (0 < stk_offs && stk_offs <= 255)
												function_body.push_back({TAB, "add ", registers::sp, ", ", registers::sp, ", #", to_string(stk_offs)});
											else {
												function_body.push_back({TAB, "ldr ", registers::s1, ", =", to_string(stk_offs)});
												function_body.push_back({TAB, "add ", registers::sp, ", ", registers::sp, ", ", registers::s1});
											}
										}

										for (int offs = 0; offs < sd->size; offs++) {
											function_body.push_back({TAB, "ldrb ", registers::s1, ", [", temp_reg, "], #1"});
											function_body.push_back({TAB, "strb ", registers::s1, ", [", registers::sp, "], #1"});
										}

										if (sd->size != 0) {
											if (0 < sd->size && sd->size <= 255) {
												function_body.push_back({TAB, "sub ", temp_reg, ", ", temp_reg, ", #", to_string(sd->size)});
												function_body.push_back({TAB, "sub ", registers::sp, ", ", registers::sp, ", #", to_string(sd->size)});
											}
											else {
												function_body.push_back({TAB, "ldr ", registers::s1, ", =", to_string(sd->size)});
												function_body.push_back({TAB, "sub ", temp_reg, ", ", temp_reg, ", ", registers::s1});
												function_body.push_back({TAB, "sub ", registers::sp, ", ", registers::sp, ", ", registers::s1});
											}
										}
										if (stk_offs != 0) {
											if (0 < stk_offs && stk_offs <= 255)
												function_body.push_back({TAB, "sub ", registers::sp, ", ", registers::sp, ", #", to_string(stk_offs)});
											else {
												function_body.push_back({TAB, "ldr ", registers::s1, ", =", to_string(stk_offs)});
												function_body.push_back({TAB, "sub ", registers::sp, ", ", registers::sp, ", ", registers::s1});
											}
										}
									}
									else {
										string temp_reg;
										if (arg->operand_kind() == operand::kind::KIND_REGISTER) {
											shared_ptr<register_operand> reg_op = static_pointer_cast<register_operand>(arg);
											if (is_double_type(bbs->get_middle_ir(), t))
												temp_reg = "d14";
											else if (is_float_type(bbs->get_middle_ir(), t))
												temp_reg = registers::f28;
											else
												temp_reg = registers::s0;
											temp_reg = handle_rvalue_register(reg_op, temp_reg, registers::s0);
										}
										else if (arg->operand_kind() == operand::kind::KIND_IMMEDIATE) {
											shared_ptr<immediate_operand> imm_op = static_pointer_cast<immediate_operand>(arg);
											if (imm_op->immediate_operand_kind() == immediate_operand::kind::KIND_STRING) {
												string lab_text = create_string_literal(imm_op);
												temp_reg = registers::s0;
												function_body.push_back({TAB, "ldr ", temp_reg, ", =", lab_text});
											}
											else {
												if (is_double_type(bbs->get_middle_ir(), t)) {
													string temp_lab = create_fp_literal(imm_op,
														is_float_type(bbs->get_middle_ir(), t));
													temp_reg = "d14";
													function_body.push_back({TAB, "vldr.F64 ", temp_reg, ", ",
														temp_lab});
												}
												else if (is_float_type(bbs->get_middle_ir(), t)) {
													string temp_lab = create_fp_literal(imm_op,
														is_float_type(bbs->get_middle_ir(), t));
													temp_reg = registers::f28;
													function_body.push_back({TAB, "vldr.F32 ", temp_reg, ", ",
														temp_lab});
												}
												else {
													int imm = retrieve_immediate_operand<int>(bbs, imm_op);
													temp_reg = registers::s0;
													function_body.push_back({TAB, "ldr ", temp_reg, ", =", to_string(imm)});
												}
											}
										}
										else if (arg->operand_kind() == operand::kind::KIND_LABEL) {
											shared_ptr<label_operand> lab_op = static_pointer_cast<label_operand>(arg);
											temp_reg = registers::s0;
											function_body.push_back({TAB, "ldr ", temp_reg, ", =", lab_op->label_text()});
										}
										else
											bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);

										add_register_if_nonvolatile(temp_reg);
										if (0 <= stk_offs && stk_offs < 256)
											function_body.push_back({TAB, s_insn, " ", temp_reg, ", [", registers::sp, ", #", to_string(stk_offs), "]"});
										else {
											nonvolatile_registers_used.insert(registers::s1);
											function_body.push_back({TAB, "ldr ", registers::s1, ", =", to_string(stk_offs)});
											if (is_fp_s_insn) {
												function_body.push_back({TAB, "add ", registers::s1, ", ", registers::s1,
													", ", registers::sp});
												function_body.push_back({TAB, s_insn, " ", temp_reg, ", [", registers::s1, "]"});
											}
											else
												function_body.push_back({TAB, s_insn, " ", temp_reg, ", [", registers::sp, ", ", registers::s1, "]"});
										}
									}
								}
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}

							for (int index = arg_types.size() - 1; index >= 0; index--) {
								shared_ptr<type> t = arg_types[index];
								shared_ptr<operand> arg = args_ops[index];
								string l_insn = get_ls_insn(t, true), s_insn = get_ls_insn(t, false); 
								if (called_func_arg_pos_2_reg.find(index) != called_func_arg_pos_2_reg.end()) {
									string arg_reg = registers::argument_registers[called_func_arg_pos_2_reg[index]];
									if (arg_pos_2_saved_offs.find(index) == arg_pos_2_saved_offs.end())
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									int offs = arg_pos_2_saved_offs[index];
									if (0 <= offs && offs <= 255) {
										function_body.push_back({TAB, l_insn, " ", arg_reg, ", [", registers::sp, ", #",
											to_string(offs), "]"});
									}
									else {
										function_body.push_back({TAB, "ldr ", registers::s1, ", =", to_string(offs)});
										function_body.push_back({TAB, l_insn, " ", arg_reg, ", [", registers::sp,
											", ", registers::s1, "]"});
									}
								}
								else if (called_func_arg_pos_2_fp_reg.find(index) != called_func_arg_pos_2_fp_reg.end()) {
									int arg_num = called_func_arg_pos_2_fp_reg[index];
									string arg_reg;
									if (is_double_type(bbs->get_middle_ir(), t))
										arg_reg = "d" + to_string(arg_num);
									else if (is_float_type(bbs->get_middle_ir(), t))
										arg_reg = "s" + to_string(arg_num << 1);
									else
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									if (arg_pos_2_saved_offs.find(index) == arg_pos_2_saved_offs.end())
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									int offs = arg_pos_2_saved_offs[index];
									if (0 <= offs && offs <= 255) {
										function_body.push_back({TAB, l_insn, " ", arg_reg, ", [", registers::sp, ", #",
											to_string(offs), "]"});
									}
									else {
										function_body.push_back({TAB, "ldr ", registers::s1, ", =", to_string(offs)});
										function_body.push_back({TAB, "add ", registers::s1, ", ", registers::s0, ", ",
											registers::sp});
										function_body.push_back({TAB, l_insn, " ", arg_reg, ", [", registers::s0, "]"});
									}
								}
							}

							if (arg_save_offs != 0) {
								if (0 < arg_save_offs && arg_save_offs <= 4095) {
									function_body.push_back({TAB, "add ", registers::sp, ", ", registers::sp, ", #",
										to_string(arg_save_offs)});
								}
								else {
									function_body.push_back({TAB, "ldr ", registers::s1, ", =", to_string(arg_save_offs)});
									function_body.push_back({TAB, "add ", registers::sp, ", ", registers::sp, ", ",
										registers::s1});
								}
							}

							if (!no_stk && func_loc != -1) {
								if (0 <= func_loc && func_loc < 255) {
									function_body.push_back({TAB, "ldr ", registers::s1, ", [", registers::sp, ", #",
										to_string(func_loc), "]"});
								}
								else {
									function_body.push_back({TAB, "ldr ", registers::s1, ", =", to_string(func_loc)});
									function_body.push_back({TAB, "ldr ", registers::s1, ", [", registers::sp, ", ",
										registers::s1, "]"});
								}
								function_body.push_back({TAB, "blx ", registers::s1});
							}
							else {
								if (func_op->operand_kind() == operand::kind::KIND_REGISTER) {
									shared_ptr<register_operand> reg_op = static_pointer_cast<register_operand>(func_op);
									string func_reg = handle_rvalue_register(reg_op, registers::s1, registers::s1);
									function_body.push_back({TAB, "blx ", func_reg});
								}
								else if (func_op->operand_kind() == operand::kind::KIND_LABEL) {
									shared_ptr<label_operand> lab_op = static_pointer_cast<label_operand>(func_op);
									function_body.push_back({TAB, "bl ", lab_op->label_text()});
								}
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							}

							if (dst_op->operand_kind() != operand::kind::KIND_REGISTER)
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							shared_ptr<register_operand> dst_reg_op = static_pointer_cast<register_operand>(dst_op);
							if (temp_ret_reg != ret_reg)
								function_body.push_back({TAB, ret_mov_insn, " ", temp_ret_reg, ", ", ret_reg});

							if (0 < called_func_arg_stk_space && called_func_arg_stk_space <= 255) {
								function_body.push_back({TAB, "add ", registers::sp, ", ", registers::sp, ", #",
									to_string(called_func_arg_stk_space)});
							}
							else if (called_func_arg_stk_space != 0) {
								add_register_if_nonvolatile(registers::s0);
								function_body.push_back({TAB, "ldr ", registers::s0, ", =", to_string(called_func_arg_stk_space)});
								function_body.push_back({TAB, "add ", registers::sp, ", ", registers::sp, ", ", registers::s0});
							}
							if (!fp_temp_push_reglist.empty())
								function_body.push_back({TAB, "vpop {", fp_temp_push_reglist, "}"});
							if (!temp_push_reglist.empty())
								function_body.push_back({TAB, "pop {", temp_push_reglist, "}"});

							pair<bool, string> purity = get_pure_register(dst_reg_op);
							if (purity.first) {
								if (purity.second != temp_ret_reg)
									function_body.push_back({TAB, ret_mov_insn, " ", purity.second, ", ", temp_ret_reg});
							}
							else {
								add_register_if_nonvolatile(registers::s0);
								handle_lvalue_register(dst_reg_op, registers::s0, temp_ret_reg);
							}
						}
							break;
						case insn::kind::KIND_ASM: {
							shared_ptr<asm_insn> ai = static_pointer_cast<asm_insn>(i);
							if (ai->asm_insn_kind() == asm_insn::kind::KIND_RAW_STRING)
								function_body.push_back({TAB, ai->raw_string()});
							else {
								string to_place_in = ai->la().first;
								shared_ptr<register_operand> reg_op = static_pointer_cast<register_operand>(ai->la().second);
								if (reg_op->dereference())
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								string reg_str = handle_rvalue_register(reg_op, registers::s1, registers::s1);
								if (reg_str != to_place_in)
									function_body.push_back({TAB, "mov ", to_place_in, ", ", reg_str});
							}
						}
							break;
						case insn::kind::KIND_BINARY: {
							shared_ptr<binary_insn> bi = static_pointer_cast<binary_insn>(i);
							shared_ptr<operand> dst = bi->dst_operand(), src1 = bi->src1_operand(),
								src2 = bi->src2_operand();
							if (dst->operand_kind() != operand::kind::KIND_REGISTER)
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							shared_ptr<register_operand> dst_reg = static_pointer_cast<register_operand>(dst);

							string temp_reg1, temp_reg2;
							if (is_double_type(bbs->get_middle_ir(), dst_reg->register_type()))
								temp_reg1 = "d14", temp_reg2 = "d15";
							else if (is_float_type(bbs->get_middle_ir(), dst_reg->register_type()))
								temp_reg1 = registers::f28, temp_reg2 = registers::f30;
							else
								temp_reg1 = registers::s1, temp_reg2 = registers::s0;
							add_register_if_nonvolatile(temp_reg1);
							add_register_if_nonvolatile(temp_reg2);

							auto op_to_maybe_imm = [&] (shared_ptr<operand> o, bool first, int lo, int hi) {
								string reg_str;
								string temp_reg = first ? temp_reg1 : temp_reg2;
								if (o->operand_kind() == operand::kind::KIND_REGISTER) {
									shared_ptr<register_operand> rego = static_pointer_cast<register_operand>(o);
									string int_temp_reg = first ? registers::s1 : registers::s0;
									reg_str = handle_rvalue_register(rego, temp_reg, int_temp_reg);
								}
								else if (o->operand_kind() == operand::kind::KIND_LABEL) {
									shared_ptr<label_operand> labo = static_pointer_cast<label_operand>(o);
									function_body.push_back({TAB, "ldr ", temp_reg, ", =", labo->label_text()});
									reg_str = temp_reg;
								}
								else if (o->operand_kind() == operand::kind::KIND_IMMEDIATE) {
									shared_ptr<immediate_operand> immo = static_pointer_cast<immediate_operand>(o);
									if (immo->immediate_operand_kind() == immediate_operand::kind::KIND_STRING) {
										string lab = create_string_literal(immo);
										function_body.push_back({TAB, "ldr ", temp_reg, ", =", lab});
										reg_str = temp_reg;
									}
									else {
										int imm = retrieve_immediate_operand<int>(bbs, immo);
										if (lo <= imm && imm < hi && !first)
											reg_str = "#" + to_string(imm);
										else {
											if (temp_reg[0] == 'r') {
												function_body.push_back({TAB, "ldr ", temp_reg, ", =", to_string(imm)});
												reg_str = temp_reg; 
											}
											else {
												string temp_lab = create_fp_literal(immo, temp_reg[0] == 's');
												function_body.push_back({TAB, "vldr.",
													(temp_reg[0] == 's' ? "F32" : "F64"), " ", temp_reg,
													", ", temp_lab});
												reg_str = temp_reg;
											}
										}
									}
								}
								else
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return reg_str;
							};

							auto generate_ops = [&bbs, &src1, &src2, &op_to_maybe_imm] (bool supports_immediate, bool commutative,
								int lo, int hi) {
								shared_ptr<operand> rhs_op = nullptr, lhs_op = nullptr;
								bool src1_is_imm = src1->operand_kind() == operand::kind::KIND_IMMEDIATE,
								     src2_is_imm = src2->operand_kind() == operand::kind::KIND_IMMEDIATE;
								if (supports_immediate) {
									if (src1_is_imm && !src2_is_imm) {
										if (commutative)
											lhs_op = src2, rhs_op = src1;
										else
											lhs_op = src1, rhs_op = src2;
									}
									else if (src2_is_imm && !src1_is_imm)
										lhs_op = src1, rhs_op = src2;
									else if (!src1_is_imm && !src2_is_imm)
										lhs_op = src1, rhs_op = src2;
									else {
										if (!commutative)
											lhs_op = src1, rhs_op = src2;
										else {
											shared_ptr<immediate_operand> src1_imm
												= static_pointer_cast<immediate_operand>(src1),
												src2_imm = static_pointer_cast<immediate_operand>(src2);
											if (src1_imm->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
												lhs_op = src1, rhs_op = src2;
											else if (src2_imm->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
												lhs_op = src2, rhs_op = src1;
											else {
												int src1_int_imm = retrieve_immediate_operand<int>(bbs, src1_imm),
												    src2_int_imm = retrieve_immediate_operand<int>(bbs, src2_imm);
												if (lo <= src1_int_imm && src1_int_imm < hi)
													lhs_op = src2, rhs_op = src1;
												else
													lhs_op = src1, rhs_op = src2;
											}
										}
									}
								}
								else
									lhs_op = src1, rhs_op = src2;
								if (lhs_op == nullptr || rhs_op == nullptr)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								return make_pair(op_to_maybe_imm(lhs_op, true, lo, hi), op_to_maybe_imm(rhs_op, false, lo, hi));
							};

							pair<bool, string> lhs_pure = get_pure_register(dst_reg);
							auto generate_normal_binary_insn = [&] (string instruction, string left_op, string right_op) {
								if (lhs_pure.first)
									function_body.push_back({TAB, instruction, " ", lhs_pure.second, ", ", left_op, ", ", right_op});
								else {
									function_body.push_back({TAB, instruction, " ", temp_reg1, ", ", left_op, ", ", right_op});
									string int_temp_reg = temp_reg1 == registers::s1 ? registers::s0 : registers::s1;
									handle_lvalue_register(dst_reg, int_temp_reg, temp_reg1);
								}
							};

							auto generate_mod_insn = [&] (string left_op, string right_op, bool is_unsigned) {
								string temp_reg3;
								if (left_op != temp_reg1)
									temp_reg3 = registers::s1;
								else if (left_op != temp_reg2)
									temp_reg3 = registers::s0;
								string div_op = is_unsigned ? "udiv" : "sdiv";
								if (lhs_pure.first && !temp_reg3.empty()) {
									function_body.push_back({TAB, div_op, " ", temp_reg3, ", ", left_op, ", ", right_op});
									function_body.push_back({TAB, "mls ", lhs_pure.second, ", ", temp_reg3, ", ",
										right_op, ", ", left_op});
								}
								else {
									function_body.push_back({TAB, "push {", registers::a0, "}"});
									function_body.push_back({TAB, div_op, " ", registers::a0, ", ", left_op, ", ", right_op});
									function_body.push_back({TAB, "mls ", registers::a0, ", ", registers::a0, ", ",
										left_op, ", ", right_op});
									handle_lvalue_register(dst_reg, registers::s1, registers::a0);
								}
							};

							auto generate_cmp_insn = [&] (string instruction, string left_op, string right_op,
								string true_cmp_code, string false_cmp_code, bool is_fp) {
								function_body.push_back({TAB, instruction, " ", left_op, ", ", right_op});
								if (is_fp)
									function_body.push_back({TAB, "vmrs APSR_nzcv, FPSCR"});
								if (lhs_pure.first) {
									function_body.push_back({TAB, true_cmp_code, " ", lhs_pure.second, ", #1"});
									function_body.push_back({TAB, false_cmp_code, " ", lhs_pure.second, ", #0"});
								}
								else {
									function_body.push_back({TAB, true_cmp_code, " ", registers::s1, ", #1"});
									function_body.push_back({TAB, false_cmp_code, " ", registers::s1, ", #0"});
									add_register_if_nonvolatile(registers::s0);
									handle_lvalue_register(dst_reg, registers::s0, registers::s1);
								}
							};

							switch (bi->binary_expr_kind()) {
							case binary_insn::kind::KIND_LADD:
							case binary_insn::kind::KIND_IADD:
							case binary_insn::kind::KIND_SADD:
							case binary_insn::kind::KIND_CADD:
							case binary_insn::kind::KIND_FADD:
							case binary_insn::kind::KIND_DADD: {
								string instruction, left_op, right_op;
								if (is_double_type(bbs->get_middle_ir(), dst_reg->register_type()))
									instruction = "vadd.F64", tie(left_op, right_op) = generate_ops(false, true, 0, 0);
								else if (is_float_type(bbs->get_middle_ir(), dst_reg->register_type()))
									instruction = "vadd.F32", tie(left_op, right_op) = generate_ops(false, true, 0, 0);
								else
									instruction = "add", tie(left_op, right_op) = generate_ops(true, true, 0, 4095);
								generate_normal_binary_insn(instruction, left_op, right_op);
							}
								break;
							case binary_insn::kind::KIND_LSUB:
							case binary_insn::kind::KIND_ISUB:
							case binary_insn::kind::KIND_SSUB:
							case binary_insn::kind::KIND_CSUB:
							case binary_insn::kind::KIND_FSUB:
							case binary_insn::kind::KIND_DSUB: {
								string instruction, left_op, right_op;
								if (is_double_type(bbs->get_middle_ir(), dst_reg->register_type()))
									instruction = "vsub.F64", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								else if (is_float_type(bbs->get_middle_ir(), dst_reg->register_type()))
									instruction = "vsub.F32", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								else
									instruction = "sub", tie(left_op, right_op) = generate_ops(true, false, 0, 4095);
								generate_normal_binary_insn(instruction, left_op, right_op);
							}
								break;
							case binary_insn::kind::KIND_LMUL:
							case binary_insn::kind::KIND_ULMUL:
							case binary_insn::kind::KIND_IMUL:
							case binary_insn::kind::KIND_UIMUL:
							case binary_insn::kind::KIND_SMUL:
							case binary_insn::kind::KIND_USMUL:
							case binary_insn::kind::KIND_CMUL:
							case binary_insn::kind::KIND_UCMUL:
							case binary_insn::kind::KIND_FMUL:
							case binary_insn::kind::KIND_DMUL: {
								string instruction, left_op, right_op;
								if (is_double_type(bbs->get_middle_ir(), dst_reg->register_type()))
									instruction = "vmul.F64", tie(left_op, right_op) = generate_ops(false, true, 0, 0);
								else if (is_float_type(bbs->get_middle_ir(), dst_reg->register_type()))
									instruction = "vmul.F32", tie(left_op, right_op) = generate_ops(false, true, 0, 0);
								else
									instruction = "mul", tie(left_op, right_op) = generate_ops(false, true, 0, 0);
								generate_normal_binary_insn(instruction, left_op, right_op);
							}
								break;
							case binary_insn::kind::KIND_LDIV:
							case binary_insn::kind::KIND_ULDIV:
							case binary_insn::kind::KIND_IDIV:
							case binary_insn::kind::KIND_UIDIV:
							case binary_insn::kind::KIND_SDIV:
							case binary_insn::kind::KIND_USDIV:
							case binary_insn::kind::KIND_CDIV:
							case binary_insn::kind::KIND_UCDIV:
							case binary_insn::kind::KIND_FDIV:
							case binary_insn::kind::KIND_DDIV: {
								if (bi->binary_expr_kind() == binary_insn::kind::KIND_ULDIV
									|| bi->binary_expr_kind() == binary_insn::kind::KIND_LDIV
									|| bi->binary_expr_kind() == binary_insn::kind::KIND_UIDIV
									|| bi->binary_expr_kind() == binary_insn::kind::KIND_IDIV
									|| bi->binary_expr_kind() == binary_insn::kind::KIND_USDIV
									|| bi->binary_expr_kind() == binary_insn::kind::KIND_SDIV
									|| bi->binary_expr_kind() == binary_insn::kind::KIND_UCDIV
									|| bi->binary_expr_kind() == binary_insn::kind::KIND_CDIV)
									bbs->report_internal("This should be unreachable (no support for hard integer division).",
										__FUNCTION__, __LINE__, __FILE__);

								string instruction, left_op, right_op;
								if (is_double_type(bbs->get_middle_ir(), dst_reg->register_type()))
									instruction = "vdiv.F64", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								else if (is_float_type(bbs->get_middle_ir(), dst_reg->register_type()))
									instruction = "vdiv.F32", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								else {
									if (is_signed_type(bbs->get_middle_ir(), dst_reg->register_type()))
										instruction = "sdiv", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
									else
										instruction = "udiv", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								}
								generate_normal_binary_insn(instruction, left_op, right_op);
							}
								break;
							case binary_insn::kind::KIND_LMOD:
							case binary_insn::kind::KIND_ULMOD:
							case binary_insn::kind::KIND_IMOD:
							case binary_insn::kind::KIND_UIMOD:
							case binary_insn::kind::KIND_SMOD:
							case binary_insn::kind::KIND_USMOD:
							case binary_insn::kind::KIND_CMOD:
							case binary_insn::kind::KIND_UCMOD: {
								bbs->report_internal("This should be unreachable (no support for hard integer modulo).",
									__FUNCTION__, __LINE__, __FILE__);

								string left_op, right_op;
								bool is_unsigned = true;
								if (is_signed_type(bbs->get_middle_ir(), dst_reg->register_type()))
									tie(left_op, right_op) = generate_ops(false, false, 0, 0), is_unsigned = false;
								else
									tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								generate_mod_insn(left_op, right_op, is_unsigned);
							}
								break;
							case binary_insn::kind::KIND_LSHL:
							case binary_insn::kind::KIND_ISHL:
							case binary_insn::kind::KIND_SSHL:
							case binary_insn::kind::KIND_CSHL: {
								string instruction, left_op, right_op;
								instruction = "lsl", tie(left_op, right_op) = generate_ops(true, false, 0, 32);
								generate_normal_binary_insn(instruction, left_op, right_op);
							}
								break;
							case binary_insn::kind::KIND_LSHR:
							case binary_insn::kind::KIND_ISHR:
							case binary_insn::kind::KIND_SSHR:
							case binary_insn::kind::KIND_CSHR: {
								string instruction, left_op, right_op;
								instruction = "lsr", tie(left_op, right_op) = generate_ops(true, false, 1, 33);
								generate_normal_binary_insn(instruction, left_op, right_op);
							}
								break;
							case binary_insn::kind::KIND_LSHRA:
							case binary_insn::kind::KIND_ISHRA:
							case binary_insn::kind::KIND_SSHRA:
							case binary_insn::kind::KIND_CSHRA: {
								string instruction, left_op, right_op;
								instruction = "asr", tie(left_op, right_op) = generate_ops(true, false, 1, 33);
								generate_normal_binary_insn(instruction, left_op, right_op);
							}
								break;
							case binary_insn::kind::KIND_LBAND:
							case binary_insn::kind::KIND_IBAND:
							case binary_insn::kind::KIND_SBAND:
							case binary_insn::kind::KIND_CBAND: {
								string instruction, left_op, right_op;
								instruction = "and", tie(left_op, right_op) = generate_ops(true, true, 0, 256);
								generate_normal_binary_insn(instruction, left_op, right_op);
							}
								break;
							case binary_insn::kind::KIND_LBOR:
							case binary_insn::kind::KIND_IBOR:
							case binary_insn::kind::KIND_SBOR:
							case binary_insn::kind::KIND_CBOR: {
								string instruction, left_op, right_op;
								instruction = "orr", tie(left_op, right_op) = generate_ops(true, true, 0, 256);
								generate_normal_binary_insn(instruction, left_op, right_op);
							}
								break;
							case binary_insn::kind::KIND_LBXOR:
							case binary_insn::kind::KIND_IBXOR:
							case binary_insn::kind::KIND_SBXOR:
							case binary_insn::kind::KIND_CBXOR: {
								string instruction, left_op, right_op;
								instruction = "eor", tie(left_op, right_op) = generate_ops(true, true, 0, 256);
								generate_normal_binary_insn(instruction, left_op, right_op);
							}
								break;
							case binary_insn::kind::KIND_LGT:
							case binary_insn::kind::KIND_ULGT:
							case binary_insn::kind::KIND_IGT:
							case binary_insn::kind::KIND_UIGT:
							case binary_insn::kind::KIND_SGT:
							case binary_insn::kind::KIND_USGT:
							case binary_insn::kind::KIND_CGT:
							case binary_insn::kind::KIND_UCGT:
							case binary_insn::kind::KIND_DGT:
							case binary_insn::kind::KIND_FGT: {
								string instruction, left_op, right_op;
								string true_cmp_code = "movgt", false_cmp_code = "movle";
								bool is_fp = true;
								if (bi->binary_expr_kind() == binary_insn::kind::KIND_DGT) {
									temp_reg1 = "d14", temp_reg2 = "d15";
									instruction = "vcmp.F64", tie(left_op, right_op) =  generate_ops(false, false, 0, 0);
								}
								else if (is_float_type(bbs->get_middle_ir(), dst_reg->register_type())) {
									temp_reg1 = registers::f28, temp_reg2 = registers::f30;
									instruction = "vcmp.F32", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								}
								else {
									temp_reg1 = registers::s1, temp_reg2 = registers::s0;
									instruction = "cmp", tie(left_op, right_op) = generate_ops(false, false, 0, 0), is_fp = false;
									if (bi->binary_expr_kind() == binary_insn::kind::KIND_ULGT
										|| bi->binary_expr_kind() == binary_insn::kind::KIND_UIGT
										|| bi->binary_expr_kind() == binary_insn::kind::KIND_USGT
										|| bi->binary_expr_kind() == binary_insn::kind::KIND_UCGT)
										true_cmp_code = "movhi", false_cmp_code = "movls";
								}
								generate_cmp_insn(instruction, left_op, right_op, true_cmp_code, false_cmp_code, is_fp);
							}
								break;
							case binary_insn::kind::KIND_LLT:
							case binary_insn::kind::KIND_ULLT:
							case binary_insn::kind::KIND_ILT:
							case binary_insn::kind::KIND_UILT:
							case binary_insn::kind::KIND_SLT:
							case binary_insn::kind::KIND_USLT:
							case binary_insn::kind::KIND_CLT:
							case binary_insn::kind::KIND_UCLT:
							case binary_insn::kind::KIND_DLT:
							case binary_insn::kind::KIND_FLT: {
								string instruction, left_op, right_op;
								bool is_fp = true;
								string true_cmp_code = "movlt", false_cmp_code = "movge";
								if (bi->binary_expr_kind() == binary_insn::kind::KIND_DLT) {
									temp_reg1 = "d14", temp_reg2 = "d15";
									instruction = "vcmp.F64", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								}
								else if (bi->binary_expr_kind() == binary_insn::kind::KIND_FLT) {
									temp_reg1 = registers::f28, temp_reg2 = registers::f30;
									instruction = "vcmp.F32", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								}
								else {
									temp_reg1 = registers::s1, temp_reg2 = registers::s0;
									instruction = "cmp", tie(left_op, right_op) = generate_ops(false, false, 0, 0), is_fp = false;
									if (bi->binary_expr_kind() == binary_insn::kind::KIND_ULLT
										|| bi->binary_expr_kind() == binary_insn::kind::KIND_UILT
										|| bi->binary_expr_kind() == binary_insn::kind::KIND_USLT
										|| bi->binary_expr_kind() == binary_insn::kind::KIND_UCLT)
										true_cmp_code = "movlo", false_cmp_code = "movhs";
								}
								generate_cmp_insn(instruction, left_op, right_op, true_cmp_code, false_cmp_code, is_fp);
							}
								break;
							case binary_insn::kind::KIND_LGE:
							case binary_insn::kind::KIND_ULGE:
							case binary_insn::kind::KIND_IGE:
							case binary_insn::kind::KIND_UIGE:
							case binary_insn::kind::KIND_SGE:
							case binary_insn::kind::KIND_USGE:
							case binary_insn::kind::KIND_CGE:
							case binary_insn::kind::KIND_UCGE:
							case binary_insn::kind::KIND_DGE:
							case binary_insn::kind::KIND_FGE: {
								string instruction, left_op, right_op;
								bool is_fp = true;
								string true_cmp_code = "movge", false_cmp_code = "movlt";
								if (bi->binary_expr_kind() == binary_insn::kind::KIND_DGE) {
									temp_reg1 = "d14", temp_reg2 = "d15";
									instruction = "vcmp.F64", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								}
								else if (bi->binary_expr_kind() == binary_insn::kind::KIND_FGE) {
									temp_reg1 = registers::f28, temp_reg2 = registers::f30;
									instruction = "vcmp.F32", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								}
								else {
									temp_reg1 = registers::s1, temp_reg2 = registers::s0;
									instruction = "cmp", tie(left_op, right_op) = generate_ops(false, false, 0, 0), is_fp = false;
									if (bi->binary_expr_kind() == binary_insn::kind::KIND_ULGE
										|| bi->binary_expr_kind() == binary_insn::kind::KIND_UIGE
										|| bi->binary_expr_kind() == binary_insn::kind::KIND_USGE
										|| bi->binary_expr_kind() == binary_insn::kind::KIND_UCGE)
										true_cmp_code = "movhs", false_cmp_code = "movlo";
								}
								generate_cmp_insn(instruction, left_op, right_op, true_cmp_code, false_cmp_code, is_fp);
							}
								break;
							case binary_insn::kind::KIND_LLE:
							case binary_insn::kind::KIND_ULLE:
							case binary_insn::kind::KIND_ILE:
							case binary_insn::kind::KIND_UILE:
							case binary_insn::kind::KIND_SLE:
							case binary_insn::kind::KIND_USLE:
							case binary_insn::kind::KIND_CLE:
							case binary_insn::kind::KIND_UCLE:
							case binary_insn::kind::KIND_DLE:
							case binary_insn::kind::KIND_FLE: {
								string instruction, left_op, right_op;
								bool is_fp = true;
								string true_cmp_code = "movle", false_cmp_code = "movgt";
								if (bi->binary_expr_kind() == binary_insn::kind::KIND_DLE) {
									temp_reg1 = "d14", temp_reg2 = "d15";
									instruction = "vcmp.F64", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								}
								else if (bi->binary_expr_kind() == binary_insn::kind::KIND_FLE) {
									temp_reg1 = registers::f28, temp_reg2 = registers::f30;
									instruction = "vcmp.F32", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								}
								else {
									temp_reg1 = registers::s1, temp_reg2 = registers::s0;
									instruction = "cmp", tie(left_op, right_op) = generate_ops(false, false, 0, 0), is_fp = false;
									if (bi->binary_expr_kind() == binary_insn::kind::KIND_ULLE
										|| bi->binary_expr_kind() == binary_insn::kind::KIND_UILE
										|| bi->binary_expr_kind() == binary_insn::kind::KIND_USLE
										|| bi->binary_expr_kind() == binary_insn::kind::KIND_UCLE)
										true_cmp_code = "movls", false_cmp_code = "movhi";
								}
								generate_cmp_insn(instruction, left_op, right_op, true_cmp_code, false_cmp_code, is_fp);
							}
								break;
							case binary_insn::kind::KIND_LEQ:
							case binary_insn::kind::KIND_IEQ:
							case binary_insn::kind::KIND_SEQ:
							case binary_insn::kind::KIND_CEQ:
							case binary_insn::kind::KIND_DEQ:
							case binary_insn::kind::KIND_FEQ: {
								string instruction, left_op, right_op;
								bool is_fp = true;
								string true_cmp_code = "moveq", false_cmp_code = "movne";
								if (bi->binary_expr_kind() == binary_insn::kind::KIND_DEQ) {
									temp_reg1 = "d14", temp_reg2 = "d15";
									instruction = "vcmp.F64", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								}
								else if (bi->binary_expr_kind() == binary_insn::kind::KIND_FEQ) {
									temp_reg1 = registers::f28, temp_reg2 = registers::f30;
									instruction = "vcmp.F32", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								}
								else {
									temp_reg1 = registers::s1, temp_reg2 = registers::s0;
									instruction = "cmp", tie(left_op, right_op) = generate_ops(false, false, 0, 0), is_fp = false;
								}
								generate_cmp_insn(instruction, left_op, right_op, true_cmp_code, false_cmp_code, is_fp);
							}
								break;
							case binary_insn::kind::KIND_LNE:
							case binary_insn::kind::KIND_INE:
							case binary_insn::kind::KIND_SNE:
							case binary_insn::kind::KIND_CNE:
							case binary_insn::kind::KIND_FNE:
							case binary_insn::kind::KIND_DNE: {
								string instruction, left_op, right_op;
								bool is_fp = true;
								string true_cmp_code = "movne", false_cmp_code = "moveq";
								if (bi->binary_expr_kind() == binary_insn::kind::KIND_DNE) {
									temp_reg1 = "d14", temp_reg2 = "d15";
									instruction = "vcmp.F64", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								}
								else if (bi->binary_expr_kind() == binary_insn::kind::KIND_FNE) {
									temp_reg1 = registers::f28, temp_reg2 = registers::f30;
									instruction = "vcmp.F32", tie(left_op, right_op) = generate_ops(false, false, 0, 0);
								}
								else {
									temp_reg1 = registers::s1, temp_reg2 = registers::s0;
									instruction = "cmp", tie(left_op, right_op) = generate_ops(false, false, 0, 0), is_fp = false;
								}
								generate_cmp_insn(instruction, left_op, right_op, true_cmp_code, false_cmp_code, is_fp);
							}
								break;
							}
						}
							break;
						case insn::kind::KIND_AT: {
							shared_ptr<at_insn> ai = static_pointer_cast<at_insn>(i);
							shared_ptr<register_operand> dst = ai->dst(), src = ai->src();
							if (ai->at_kind() == at_insn::kind::KIND_LHS_DAT
								|| ai->at_kind() == at_insn::kind::KIND_LHS_FAT
								|| ai->at_kind() == at_insn::kind::KIND_LHS_LAT
								|| ai->at_kind() == at_insn::kind::KIND_LHS_IAT
								|| ai->at_kind() == at_insn::kind::KIND_LHS_SAT
								|| ai->at_kind() == at_insn::kind::KIND_LHS_CAT) {
								string lhs_reg = handle_rvalue_register(dst, registers::s1, registers::s1);
								function_body.push_back({TAB, "ldr ", lhs_reg, ", [", lhs_reg, "]"});

								string temp_reg, s_insn = get_ls_insn(src->register_type(), false);
								if (is_double_type(bbs->get_middle_ir(), src->register_type()))
									temp_reg = "d14";
								else if (is_float_type(bbs->get_middle_ir(), src->register_type()))
									temp_reg = registers::f28;
								else
									temp_reg = registers::s0;
								add_register_if_nonvolatile(registers::s0);
								string rhs_reg = handle_rvalue_register(src, temp_reg, registers::s0);
								function_body.push_back({TAB, s_insn, " ", rhs_reg, ", [", lhs_reg, "]"});
							}
							else {
								string rhs_reg_str = handle_rvalue_register(src, registers::s1, registers::s1);
								string temp_reg, mov_insn;
								if (is_double_type(bbs->get_middle_ir(), dst->register_type()))
									temp_reg = "d14";
								else if (is_float_type(bbs->get_middle_ir(), dst->register_type()))
									temp_reg = registers::f28;
								else
									temp_reg = registers::s0;
								string l_insn = get_ls_insn(dst->register_type(), true);
								pair<bool, string> purity = get_pure_register(dst);
								if (purity.first)
									function_body.push_back({TAB, l_insn, " ", purity.second, ", [", registers::s1, "]"});
								else {
									add_register_if_nonvolatile(temp_reg);
									function_body.push_back({TAB, l_insn, " ", temp_reg, ", [", registers::s1, "]"});
									handle_lvalue_register(dst, registers::s1, temp_reg);
								}
							}
						}
							break;
						default:
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							break;
						}
						total_function_insn_counter = function_prologue.size() + function_body.size() + function_epilogue.size();
						if ((total_function_insn_counter % LITERAL_POOL_RANGE) + LITERAL_POOL_INSERTION_BEFORE >= LITERAL_POOL_RANGE
							&& last_lit_pool_insertion + LITERAL_POOL_RANGE < total_function_insn_counter) {
							string asm_lab = ASM_BRANCH_PREFIX + to_string(scratch_counter++);
							function_body.push_back({TAB, "b ", asm_lab});
							function_body.push_back({".ltorg"});
							function_body.push_back({asm_lab, ":"});
							last_lit_pool_insertion = total_function_insn_counter;
						}
					}
					first_block = false;
				}

				auto [num_regs_pushed, initial_push_reglist, fp_initial_push_reglist] = preserve_stack_alignment(nonvolatile_registers_used);
				int prologue_size = num_regs_pushed << 2;

				function_prologue.push_back({TAB, "push {", initial_push_reglist, "}"});
				if (!fp_initial_push_reglist.empty())
					function_prologue.push_back({TAB, "vpush {", fp_initial_push_reglist, "}"});
				function_prologue.push_back({TAB, "mov ", registers::fp, ", ", registers::sp});
				if (func_stk_space != 0) {
					int to_use = (func_stk_space + (STACK_ALIGNMENT - 1)) & ~(STACK_ALIGNMENT - 1);
					if (0 <= to_use && to_use < 256)
						function_prologue.push_back({TAB, "sub ", registers::sp, ", ", registers::sp, ", #", to_string(to_use)});
					else {
						function_prologue.push_back({TAB, "ldr ", registers::s1, ", =", to_string(to_use)});
						function_prologue.push_back({TAB, "sub ", registers::sp, ", ", registers::sp, ", ", registers::s1});
					}
				}

				for (const auto& [reg_num, sad] : arg_reg_num_2_stk_allocs) {
					int arg_pos = arg_reg_num_2_arg_pos[reg_num];
					if (arg_pos_2_phys_reg_num.find(arg_pos) != arg_pos_2_phys_reg_num.end()) {
						int arg_reg_num = arg_pos_2_phys_reg_num[arg_pos];
						string arg_reg = registers::argument_registers[arg_reg_num];
						shared_ptr<type> param_type = func_label->orig_func_insn()->func_type()->parameter_list()[arg_pos]
							->variable_declaration_type();
						function_prologue.push_back({TAB, "ldr ", registers::s1, ", =",
							arm_offset{arm_offset::kind::KIND_FP, reg_num_2_stk_offs[sad->reg->virtual_register_number()]}});
						if (is_int_type(bbs->get_middle_ir(), param_type)
							|| is_pointer_type(bbs->get_middle_ir(), param_type)
							|| is_long_type(bbs->get_middle_ir(), param_type))
							function_prologue.push_back({TAB, "str ", arg_reg, ", [", registers::fp, ", ", registers::s1, "]"});
						else if (is_short_type(bbs->get_middle_ir(), param_type)) {
							function_prologue.push_back({TAB, "strh ", arg_reg, ", [", registers::fp, ", ", registers::s1, "]"});
						}
						else if (is_char_type(bbs->get_middle_ir(), param_type)
							|| is_bool_type(bbs->get_middle_ir(), param_type)
							|| is_char_type(bbs->get_middle_ir(), param_type)) {
							function_prologue.push_back({TAB, "strb ", arg_reg, ", [", registers::fp, ", ", registers::s1, "]"});
						}
						else
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					}
					else if (arg_pos_2_fp_phys_reg_num.find(arg_pos) != arg_pos_2_fp_phys_reg_num.end()) {
						shared_ptr<type> param_type = func_label->orig_func_insn()->func_type()->parameter_list()[arg_pos]
							->variable_declaration_type();
						int arg_num = arg_pos_2_fp_phys_reg_num[arg_pos];
						function_prologue.push_back({TAB, "ldr ", registers::s1, ", =",
							arm_offset{arm_offset::kind::KIND_FP, reg_num_2_stk_offs[sad->reg->virtual_register_number()]}});
						string reg_str, str_insn;
						if (is_float_type(bbs->get_middle_ir(), param_type))
							reg_str = "s" + to_string(arg_num << 1), str_insn = "vstr.F32";
						else if (is_double_type(bbs->get_middle_ir(), param_type))
							reg_str = "d" + to_string(arg_num), str_insn = "vstr.F64";
						else
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						function_prologue.push_back({TAB, "add ", registers::s1, ", ", registers::s1, ", ", registers::fp});
						function_prologue.push_back({TAB, str_insn, " ", reg_str, ", [", registers::s1, "]"});
					}
				}

				function_epilogue.push_back({TAB, "mov ", registers::sp, ", ", registers::fp});
				if (!fp_initial_push_reglist.empty())
					function_epilogue.push_back({TAB, "vpop {", fp_initial_push_reglist, "}"});
				function_epilogue.push_back({TAB, "pop {", initial_push_reglist, "}"});
				function_epilogue.push_back({TAB, "bx lr"});
				function_epilogue.push_back({TAB, ".ltorg"});
				function_epilogue.push_back({""});

				for (const auto& ai : function_prologue)
					aa->file_body.push_back(arm_insn_2_string(ai, prologue_size));
				for (const auto& ai : function_body)
					aa->file_body.push_back(arm_insn_2_string(ai, prologue_size));
				for (const auto& ai : function_epilogue)
					aa->file_body.push_back(arm_insn_2_string(ai, prologue_size));
			}
		}

		vector<string> generate_asm(shared_ptr<basic_blocks> bbs, shared_ptr<spectre::parser::parser> p, pass_manager::debug_level dl) {
			shared_ptr<arm_asm> aa = make_shared<arm_asm>();
			aa->file_bss.push_back(".bss");
			aa->file_data.push_back(".data");
			int scratch_counter = 0;

			unordered_map<int, int> coloring = spectre::cgen::color_interference_graph(bbs, registers::num_general_purpose_registers,
				registers::num_fp_general_purpose_registers, dl);
			auto [function_headers, global_headers] = function_and_global_headers(bbs);
			if (global_headers.size() != 1)
				bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
			unordered_map<string, shared_ptr<function_symbol>> mangled_name_2_func_map;
			for (const auto& fi : bbs->get_middle_ir()->function_insn_list())
				mangled_name_2_func_map[c_symbol_2_string(bbs->get_middle_ir(), fi->func_symbol())] = fi->func_symbol();

			bool inserted_globl_directive = false;
			int gh_index = *global_headers.begin();
			inserted_globl_directive = false;
			shared_ptr<basic_block> gbb = bbs->get_basic_block(gh_index);
			unordered_map<int, shared_ptr<variable_declaration>> g_var_2_decl_map;
			unordered_map<int, string> g_var_2_sym_name_map;

			aa->file_header.push_back(".globl " + malloc_symbol_name);
			aa->file_header.push_back(".globl " + free_symbol_name);

			vector<shared_ptr<symbol>> all_global_syms;
			unordered_set<string> all_global_sym_names;
			function<void(vector<shared_ptr<symbol>>)> gather_all_global_syms = [&] (vector<shared_ptr<symbol>> symbols) {
				for (shared_ptr<symbol> s : symbols) {
					if (s->symbol_kind() != symbol::kind::KIND_NAMESPACE) {
						string sym_name = c_symbol_2_string(bbs->get_middle_ir(), s);
						if (all_global_sym_names.find(sym_name)== all_global_sym_names.end()) {
							all_global_syms.push_back(s);
							all_global_sym_names.insert(sym_name);
						}
					}
					else {
						shared_ptr<namespace_symbol> ns = static_pointer_cast<namespace_symbol>(s);
						gather_all_global_syms(ns->namespace_symbol_scope()->symbol_list());
					}
				}
			};

			gather_all_global_syms(p->global_scope()->symbol_list());
			vector<shared_ptr<function_symbol>> func_syms;
			for (shared_ptr<symbol> sym : all_global_syms) {
				if (sym->symbol_kind() == symbol::kind::KIND_VARIABLE) {
					shared_ptr<variable_symbol> vsym = static_pointer_cast<variable_symbol>(sym);
					shared_ptr<type> vtype = vsym->variable_type();
					if (vtype->type_constexpr_kind() == type::constexpr_kind::KIND_CONSTEXPR)
						continue;
					string vsym_string = c_symbol_2_string(bbs->get_middle_ir(), vsym);
					if (vtype->type_static_kind() != type::static_kind::KIND_STATIC) {
						aa->file_header.push_back(".globl " + vsym_string);
						inserted_globl_directive = true;
					}
					shared_ptr<register_operand> reg = bbs->get_middle_ir()->lookup_variable_register(vsym_string);
					shared_ptr<variable_declaration> vdecl = bbs->get_middle_ir()->lookup_variable(vsym_string);
					if (vdecl == nullptr)
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (reg == nullptr)
						bbs->report_internal("This should be unreachable." + vsym_string, __FUNCTION__, __LINE__, __FILE__);
					if (reg->dereference())
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					if (g_var_2_decl_map.find(reg->base_virtual_register_number()) != g_var_2_decl_map.end())
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					g_var_2_decl_map[reg->base_virtual_register_number()] = vdecl;
					g_var_2_sym_name_map[reg->base_virtual_register_number()] = vsym_string;
				}
				else if (sym->symbol_kind() == symbol::kind::KIND_FUNCTION) {
					shared_ptr<function_symbol> fsym = static_pointer_cast<function_symbol>(sym);
					func_syms.push_back(fsym);
					shared_ptr<type> ftype = fsym->function_symbol_type();
					string fsym_string = c_symbol_2_string(bbs->get_middle_ir(), fsym);
					if (ftype->type_static_kind() != type::static_kind::KIND_STATIC) {
						aa->file_header.push_back(".globl " + fsym_string);
						inserted_globl_directive = true;
					}
				}
			}
			if (inserted_globl_directive)
				aa->file_header.push_back("");

			unordered_map<string, bool> g_sym_name_2_bss;
			unordered_map<string, int> g_sym_name_2_alignment;
			unordered_map<int, string> e_var_2_sym_name_map;
			unordered_set<string> g_initialized;
			for (int g_insn_counter = 1; g_insn_counter < gbb->get_insns(bbs).size(); g_insn_counter++) {
				shared_ptr<insn> i = gbb->get_insn(g_insn_counter);
				bool g_align_hit = false;
				int g_alignment = -1;
				while (i->insn_kind() == insn::kind::KIND_ALIGN && g_insn_counter < gbb->get_insns(bbs).size()) {
					g_align_hit = true, g_insn_counter++, g_alignment = static_pointer_cast<align_insn>(i)->alignment();
					i = gbb->get_insn(g_insn_counter);
				}
				if (g_insn_counter >= gbb->get_insns(bbs).size())
					break;
				switch (i->insn_kind()) {
				case insn::kind::KIND_ALIGN:
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					break;
				case insn::kind::KIND_ACCESS: {
					shared_ptr<access_insn> ai = static_pointer_cast<access_insn>(i);
					if (ai->variable()->operand_kind() != operand::kind::KIND_REGISTER)
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					shared_ptr<register_operand> reg = static_pointer_cast<register_operand>(ai->variable());
					if (reg->dereference())
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					e_var_2_sym_name_map[reg->base_virtual_register_number()] = ai->symbol();
				}
					break;
				case insn::kind::KIND_UNARY: {
					shared_ptr<unary_insn> ui = static_pointer_cast<unary_insn>(i);
					switch (ui->unary_expr_kind()) {
					case unary_insn::kind::KIND_RESV: {
						shared_ptr<operand> loc = ui->dst_operand();
						bool bss = false;
						string sym_name;
						if (loc->operand_kind() == operand::kind::KIND_REGISTER) {
							shared_ptr<register_operand> rloc = static_pointer_cast<register_operand>(loc);
							if (rloc->dereference())
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							if (g_var_2_decl_map.find(rloc->base_virtual_register_number()) == g_var_2_decl_map.end()) {
								sym_name = g_var_2_sym_name_map[rloc->base_virtual_register_number()]
									= GLOBAL_SCRATCH_PREFIX + to_string(scratch_counter++);
								bss = true;
							}
							else {
								shared_ptr<variable_declaration> vdecl = g_var_2_decl_map[rloc->base_virtual_register_number()];
								shared_ptr<variable_symbol> vsym = vdecl->variable_declaration_symbol();
								sym_name = c_symbol_2_string(bbs->get_middle_ir(), vsym);
								if (vdecl->variable_declaration_initialization_kind() != variable_declaration::initialization_kind::KIND_PRESENT)
									bss = true;
							}
							if (bss) {
								shared_ptr<operand> src_op = ui->src_operand();
								if (src_op->operand_kind() != operand::kind::KIND_IMMEDIATE)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								shared_ptr<immediate_operand> isrc = static_pointer_cast<immediate_operand>(src_op);

								if (g_align_hit && g_alignment != 1) {
									if (!is_pow2(g_alignment))
										bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
									aa->file_bss.push_back(".align " + to_string(int_log2(bbs, g_alignment)));
								}
								aa->file_bss.push_back(sym_name + ": .space " + to_string(retrieve_immediate_operand<int>(bbs, isrc)));
							}
						}
						else if (loc->operand_kind() == operand::kind::KIND_LABEL) {
							shared_ptr<label_operand> lloc = static_pointer_cast<label_operand>(loc);
							sym_name = lloc->label_text();
							shared_ptr<operand> src_op = ui->src_operand();
							if (src_op->operand_kind() != operand::kind::KIND_IMMEDIATE)
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							shared_ptr<immediate_operand> isrc = static_pointer_cast<immediate_operand>(src_op);
							bss = true;
							if (g_align_hit && g_alignment != 1) {
								if (!is_pow2(g_alignment))
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								aa->file_bss.push_back(".align " + to_string(int_log2(bbs, g_alignment)));
							}

							aa->file_bss.push_back(sym_name + ": .space " + to_string(retrieve_immediate_operand<int>(bbs, isrc)));
						}
						else
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);

						if (g_align_hit) {
							if (g_sym_name_2_alignment.find(sym_name) != g_sym_name_2_alignment.end())
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							g_sym_name_2_alignment[sym_name] = g_alignment;
						}

						if (g_sym_name_2_bss.find(sym_name) != g_sym_name_2_bss.end())
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						g_sym_name_2_bss[sym_name] = bss;
					}
						break;
					case unary_insn::kind::KIND_LMOV:
					case unary_insn::kind::KIND_IMOV:
					case unary_insn::kind::KIND_SMOV:
					case unary_insn::kind::KIND_CMOV:
					case unary_insn::kind::KIND_FMOV:
					case unary_insn::kind::KIND_DMOV: {
						shared_ptr<operand> dst = ui->dst_operand();
						string sym_name;
						if (dst->operand_kind() == operand::kind::KIND_REGISTER) {
							shared_ptr<register_operand> rdst = static_pointer_cast<register_operand>(dst);
							if (!rdst->dereference())
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
							if (g_var_2_sym_name_map.find(rdst->base_virtual_register_number()) == g_var_2_sym_name_map.end())
								bbs->report_internal("This should be unreachable." + ui->to_string(), __FUNCTION__, __LINE__, __FILE__);
							sym_name = g_var_2_sym_name_map[rdst->base_virtual_register_number()];
							if (g_sym_name_2_bss.find(sym_name) == g_sym_name_2_bss.end())
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						}
						else if (dst->operand_kind() == operand::kind::KIND_LABEL) {
							shared_ptr<label_operand> ldst = static_pointer_cast<label_operand>(dst);
							sym_name = ldst->label_text();
						}
						else
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						if (g_initialized.find(sym_name) != g_initialized.end())
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						g_initialized.insert(sym_name);
						bool bss = g_sym_name_2_bss[sym_name];
						int alignment = 1;
						if (g_sym_name_2_alignment.find(sym_name) != g_sym_name_2_alignment.end())
							alignment = g_sym_name_2_alignment[sym_name];
						if (!is_pow2(alignment))
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						alignment = int_log2(bbs, alignment);

						shared_ptr<operand> src = ui->src_operand();
						string directive;
						string data;
						if (src->operand_kind() == operand::kind::KIND_IMMEDIATE) {
							shared_ptr<immediate_operand> isrc = static_pointer_cast<immediate_operand>(src);
							switch (ui->unary_expr_kind()) {
							case unary_insn::kind::KIND_LMOV: {
								if (isrc->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								directive = ".word";
								data = to_string(retrieve_immediate_operand<long>(bbs, isrc));
							}
								break;
							case unary_insn::kind::KIND_IMOV: {
								if (isrc->immediate_operand_kind() == immediate_operand::kind::KIND_STRING) {
									directive = ".asciz";
									data = isrc->to_string();
								}
								else {
									directive = ".word";
									data = to_string(retrieve_immediate_operand<int>(bbs, isrc));
								}
							}
								break;
							case unary_insn::kind::KIND_SMOV: {
								if (isrc->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								directive = ".short";
								data = to_string(retrieve_immediate_operand<short>(bbs, isrc));
							}
								break;
							case unary_insn::kind::KIND_CMOV: {
								if (isrc->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								directive = ".byte";
								data = to_string(retrieve_immediate_operand<int>(bbs, isrc));
							}
								break;
							case unary_insn::kind::KIND_FMOV: {
								if (isrc->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								directive = ".float";
								data = fp_max_prec_to_string(retrieve_immediate_operand<float>(bbs, isrc));
							}
								break;
							case unary_insn::kind::KIND_DMOV: {
								if (isrc->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								directive = ".double";
								data = fp_max_prec_to_string(retrieve_immediate_operand<double>(bbs, isrc));
							}
								break;
							default:
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								break;
							}
						}
						else if (src->operand_kind() == operand::kind::KIND_LABEL) {
							shared_ptr<label_operand> lsrc = static_pointer_cast<label_operand>(src);
							directive = ".word";
							data = lsrc->label_text();
						}
						else
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);

						if (bss) {
							if (alignment != 0)
								aa->file_bss.push_back(".align " + to_string(alignment));
							aa->file_bss.push_back(sym_name + ": " + directive + " " + data);
							aa->file_bss.push_back("");
						}
						else {
							if (alignment != 0)
								aa->file_data.push_back(".align " + to_string(alignment));
							aa->file_data.push_back(sym_name + ": " + directive + " " + data);
							aa->file_data.push_back("");
						}
					}
						break;
					default:
						bbs->report_internal("This should be unreachable." + i->to_string(), __FUNCTION__, __LINE__, __FILE__);
						break;
					}
				}
					break;
				case insn::kind::KIND_GLOBAL_ARRAY: {
					shared_ptr<global_array_insn> gai = static_pointer_cast<global_array_insn>(i);
					string sym_name = gai->label()->label_text();
					if (g_initialized.find(sym_name) != g_initialized.end())
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					g_initialized.insert(sym_name);
					vector<string> data;
					string directive;
					switch (gai->directive()) {
					case global_array_insn::directive_kind::KIND_WORD: {
						directive = ".word";
						for (const auto& p : gai->data()) {
							if (p->operand_kind() == operand::kind::KIND_IMMEDIATE) {
								shared_ptr<immediate_operand> io = static_pointer_cast<immediate_operand>(p);
								if (io->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								data.push_back(cast_immediate_2_string(bbs, retrieve_immediate_operand<int>(bbs, io),
									immediate_operand_cast_kind::KIND_INTEGRAL));
							}
							else if (p->operand_kind() == operand::kind::KIND_LABEL)
								data.push_back(static_pointer_cast<label_operand>(p)->label_text());
							else
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						}
					}
						break;
					case global_array_insn::directive_kind::KIND_DOUBLE: {
						directive = ".double";
						for (const auto& p : gai->data()) {
							if (p->operand_kind() == operand::kind::KIND_IMMEDIATE) {
								shared_ptr<immediate_operand> io = static_pointer_cast<immediate_operand>(p);
								if (io->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								data.push_back(cast_immediate_2_string(bbs, retrieve_immediate_operand<double>(bbs, io),
									immediate_operand_cast_kind::KIND_DOUBLE));
							}
							else
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						}
					}
						break;
					case global_array_insn::directive_kind::KIND_FLOAT: {
						directive = ".float";
						for (const auto& p : gai->data()) {
							if (p->operand_kind() == operand::kind::KIND_IMMEDIATE) {
								shared_ptr<immediate_operand> io = static_pointer_cast<immediate_operand>(p);
								if (io->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								data.push_back(cast_immediate_2_string(bbs, retrieve_immediate_operand<float>(bbs, io),
									immediate_operand_cast_kind::KIND_FLOAT));
							}
							else
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						}
					}
						break;
					case global_array_insn::directive_kind::KIND_HALF: {
						directive = ".short";
						for (const auto& p : gai->data()) {
							if (p->operand_kind() == operand::kind::KIND_IMMEDIATE) {
								shared_ptr<immediate_operand> io = static_pointer_cast<immediate_operand>(p);
								if (io->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								data.push_back(cast_immediate_2_string(bbs, retrieve_immediate_operand<short>(bbs, io),
									immediate_operand_cast_kind::KIND_INTEGRAL));
							}
							else
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						}
					}
						break;
					case global_array_insn::directive_kind::KIND_BYTE: {
						directive = ".byte";
						for (const auto& p : gai->data()) {
							if (p->operand_kind() == operand::kind::KIND_IMMEDIATE) {
								shared_ptr<immediate_operand> io = static_pointer_cast<immediate_operand>(p);
								if (io->immediate_operand_kind() == immediate_operand::kind::KIND_STRING)
									bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
								data.push_back(cast_immediate_2_string(bbs, retrieve_immediate_operand<int>(bbs, io),
									immediate_operand_cast_kind::KIND_INTEGRAL));
							}
							else
								bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						}
					}
						break;
					default:
						bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						break;
					}
					if (g_align_hit && g_alignment != 1) {
						if (!is_pow2(g_alignment))
							bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
						int alignment = int_log2(bbs, g_alignment);
						aa->file_data.push_back(".align " + to_string(alignment));
					}
					string instruction;
					instruction += sym_name + ": " + directive + " ";
					for (int i = 0; i < data.size(); i++) {
						instruction += data[i];
						if (i != data.size() - 1)
							instruction += ", ";
					}
					aa->file_data.push_back(instruction);
					aa->file_data.push_back("");
				}
					break;
				default:
					bbs->report_internal("This should be unreachable.", __FUNCTION__, __LINE__, __FILE__);
					break;
				}
			}

			handle_functions_asm(bbs, aa, function_headers, coloring, g_var_2_sym_name_map, e_var_2_sym_name_map, dl);

			for (shared_ptr<function_symbol> fsym : func_syms) {
				main_function_kind mfk = main_function_defined(fsym);
				if (mfk != main_function_kind::NONE) {
					generate_start_hook(aa, mfk);
					break;
				}
			}

			if (!aa->file_header.empty() && !aa->file_header[aa->file_header.size() - 1].empty())
				aa->file_header.push_back("");
			if (!aa->file_bss.empty() && !aa->file_bss[aa->file_bss.size() - 1].empty())
				aa->file_bss.push_back("");
			if (!aa->file_data.empty() && !aa->file_data[aa->file_data.size() - 1].empty())
				aa->file_data.push_back("");
			vector<string> full_file;
			full_file.insert(full_file.end(), aa->file_header.begin(), aa->file_header.end());
			full_file.insert(full_file.end(), aa->file_data.begin(), aa->file_data.end());
			full_file.insert(full_file.end(), aa->file_bss.begin(), aa->file_bss.end());
			full_file.insert(full_file.end(), aa->file_body.begin(), aa->file_body.end());
			return full_file;
		}
	}
}
