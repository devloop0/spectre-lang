#include "type.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include <memory>
#include <iostream>

using spectre::ast::primary_expression;
using spectre::ast::type;
using spectre::ast::primitive_type;
using spectre::ast::stmt;
using spectre::ast::binary_expression;
using spectre::ast::value_kind;
using spectre::ast::postfix_expression;
using spectre::ast::unary_expression;
using spectre::ast::ternary_expression;
using spectre::ast::assignment_expression;
using spectre::ast::expression;
using std::cout;
using std::shared_ptr;

namespace spectre {
	namespace ast {
		namespace debug {
			void print_expression(shared_ptr<expression> expr);
			void print_binary_expression_operator_kind(binary_expression::operator_kind op_kind);
			void print_primary_expression(shared_ptr<primary_expression> pexpr);
			void print_postfix_expression(shared_ptr<postfix_expression> pexpr);
			void print_unary_expression(shared_ptr<unary_expression> uexpr);
			void print_binary_expression(shared_ptr<binary_expression> bexpr);
			void print_ternary_expression(shared_ptr<ternary_expression> texpr);
			void print_assignment_expression(shared_ptr<assignment_expression> aexpr);
			void print_expression(shared_ptr<expression> expr);

			void print_stmt(shared_ptr<stmt> s);

			void print_type(shared_ptr<type> t);
			void print_value_kind(value_kind vk);
		}
	}
}