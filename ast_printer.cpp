#include "ast_printer.hpp"
#include <iostream>

using std::cout;
using std::static_pointer_cast;

#define PRINT_BRACKETS_READABLE

namespace spectre {
	namespace ast {
		namespace debug {
			void print_binary_expression_operator_kind(binary_expression::operator_kind op_kind) {
				cout << ' ';
				switch (op_kind) {
				case binary_expression::operator_kind::KIND_ADD:
					cout << '+';
					break;
				case binary_expression::operator_kind::KIND_DIVIDE:
					cout << '/';
					break;
				case binary_expression::operator_kind::KIND_GREATER_THAN:
					cout << '>';
					break;
				case binary_expression::operator_kind::KIND_GREATER_THAN_OR_EQUAL_TO:
					cout << ">=";
					break;
				case binary_expression::operator_kind::KIND_LESS_THAN:
					cout << '<';
					break;
				case binary_expression::operator_kind::KIND_LESS_THAN_OR_EQUAL_TO:
					cout << "<=";
					break;
				case binary_expression::operator_kind::KIND_MODULUS:
					cout << '%';
					break;
				case binary_expression::operator_kind::KIND_MULTIPLY:
					cout << '*';
					break;
				case binary_expression::operator_kind::KIND_SHIFT_LEFT:
					cout << "<<";
					break;
				case binary_expression::operator_kind::KIND_SHIFT_RIGHT:
					cout << ">>";
					break;
				case binary_expression::operator_kind::KIND_SUBTRACT:
					cout << '-';
					break;
				case binary_expression::operator_kind::KIND_EQUALS_EQUALS:
					cout << "==";
					break;
				case binary_expression::operator_kind::KIND_NOT_EQUALS:
					cout << "!=";
					break;
				case binary_expression::operator_kind::KIND_BITWISE_AND:
					cout << '&';
					break;
				case binary_expression::operator_kind::KIND_BITWISE_OR:
					cout << '|';
					break;
				case binary_expression::operator_kind::KIND_BITWISE_XOR:
					cout << '^';
					break;
				case binary_expression::operator_kind::KIND_LOGICAL_OR:
					cout << "||";
					break;
				case binary_expression::operator_kind::KIND_LOGICAL_AND:
					cout << "&&";
					break;
				case binary_expression::operator_kind::KIND_ADD_EQUALS:
					cout << "+=";
					break;
				case binary_expression::operator_kind::KIND_SUBTRACT_EQUALS:
					cout << "-=";
					break;
				case binary_expression::operator_kind::KIND_MULTIPLY_EQUALS:
					cout << "*=";
					break;
				case binary_expression::operator_kind::KIND_DIVIDE_EQUALS:
					cout << "/=";
					break;
				case binary_expression::operator_kind::KIND_MODULUS_EQUALS:
					cout << "%=";
					break;
				case binary_expression::operator_kind::KIND_SHIFT_LEFT_EQUALS:
					cout << "<<=";
					break;
				case binary_expression::operator_kind::KIND_SHIFT_RIGHT_EQUALS:
					cout << ">>=";
					break;
				case binary_expression::operator_kind::KIND_BITWISE_AND_EQUALS:
					cout << "&=";
					break;
				case binary_expression::operator_kind::KIND_BITWISE_OR_EQUALS:
					cout << "|=";
					break;
				case binary_expression::operator_kind::KIND_BITWISE_XOR_EQUALS:
					cout << "^=";
					break;
				case binary_expression::operator_kind::KIND_LOGICAL_OR_EQUALS:
					cout << "||=";
					break;
				case binary_expression::operator_kind::KIND_LOGICAL_AND_EQUALS:
					cout << "&&=";
					break;
				}
				cout << ' ';
			}

			void print_primary_expression(shared_ptr<primary_expression> pexpr) {
				switch (pexpr->primary_expression_kind()) {
				case primary_expression::kind::KIND_LITERAL:
#ifndef PRINT_BRACKETS_READABLE
					cout << '[';
#endif
					cout << pexpr->literal_token().raw_text();
#ifndef PRINT_BRACKETS_READABLE
					cout << ']';
#endif
					break;
				case primary_expression::kind::KIND_PARENTHESIZED_EXPRESSION:
#ifndef PRINT_BRACKETS_READABLE
					cout << '[';
#endif
					cout << " ( ";
					print_expression(pexpr->parenthesized_expression());
					cout << " ) ";
#ifndef PRINT_BRACKETS_READABLE
					cout << ']';
#endif
					break;
				}
			}

			void print_postfix_expression(shared_ptr<postfix_expression> pexpr) {
				if (pexpr == nullptr) return;
#ifndef PRINT_BRACKETS_READABLE
				cout << '[';
#endif
				print_primary_expression(pexpr->contained_primary_expression());
				for (shared_ptr<postfix_expression::postfix_type> pt : pexpr->postfix_type_list()) {
					switch (pt->postfix_type_kind()) {
					case postfix_expression::kind::KIND_DECREMENT:
						cout << "--";
						break;
					case postfix_expression::kind::KIND_INCREMENT:
						cout << "++";
						break;
					}
				}
#ifndef PRINT_BRACKETS_READABLE
				cout << ']';
#endif
			}

			void print_unary_expression(shared_ptr<unary_expression> uexpr) {
				if (uexpr == nullptr) return;
#ifndef PRINT_BRACKETS_READABLE
				cout << '[';
#endif
				for (unary_expression::kind uek : uexpr->unary_expression_kind_list()) {
					switch (uek) {
					case unary_expression::kind::KIND_BITWISE_NOT:
						cout << '~';
						break;
					case unary_expression::kind::KIND_DECREMENT:
						cout << "--";
						break;
					case unary_expression::kind::KIND_INCREMENT:
						cout << "++";
						break;
					case unary_expression::kind::KIND_LOGICAL_NOT:
						cout << '!';
						break;
					case unary_expression::kind::KIND_MINUS:
						cout << '-';
						break;
					case unary_expression::kind::KIND_PLUS:
						cout << '+';
						break;
					}
				}
				print_postfix_expression(uexpr->contained_postfix_expression());
#ifndef PRINT_BRACKETS_READABLE
				cout << ']';
#endif
			}

			void print_binary_expression(shared_ptr<binary_expression> bexpr) {
				if (bexpr == nullptr) return;
				if (bexpr->binary_expression_kind() == binary_expression::kind::KIND_UNARY_EXPRESSION) {
					print_unary_expression(bexpr->single_lhs());
					return;
				}
				else if (bexpr->binary_expression_kind() == binary_expression::kind::KIND_BINARY_EXPRESSION) {
					cout << '[';
					print_binary_expression(bexpr->lhs());
					print_binary_expression_operator_kind(bexpr->binary_expression_operator_kind());
					print_binary_expression(bexpr->rhs());
					cout << ']';
				}
			}

			void print_ternary_expression(shared_ptr<ternary_expression> texpr) {
				if (texpr == nullptr) return;
#ifndef PRINT_BRACKETS_READABLE
				cout << '[';
#endif
				print_binary_expression(texpr->condition());
				if (texpr->ternary_expression_kind() == ternary_expression::kind::KIND_TERNARY) {
					cout << " ? ";
					print_expression(texpr->true_path());
					cout << " : ";
					print_ternary_expression(texpr->false_path());
				}
#ifndef PRINT_BRACKETS_READABLE
				cout << ']';
#endif
			}

			void print_assignment_expression(shared_ptr<assignment_expression> aexpr) {
				if (aexpr == nullptr) return;
#ifndef PRINT_BRACKETS_READABLE
				cout << '[';
#endif
				if (aexpr->assignment_expression_kind() == assignment_expression::kind::KIND_TERNARY)
					print_ternary_expression(aexpr->conditional_expression());
				else {
					print_unary_expression(aexpr->lhs_assignment());
					print_binary_expression_operator_kind(aexpr->assignment_operator_kind());
					print_assignment_expression(aexpr->rhs_assignment());
				}
#ifndef PRINT_BRACKETS_READABLE
				cout << ']';
#endif
			}

			void print_expression(shared_ptr<expression> expr) {
				if (expr == nullptr) return;
#ifndef PRINT_BRACKETS_READABLE
				cout << '[';
#endif
				for (int i = 0; i < expr->assignment_expression_list().size(); i++) {
					shared_ptr<assignment_expression> ae = expr->assignment_expression_list()[i];
					print_assignment_expression(ae);
					if (i != expr->assignment_expression_list().size() - 1)
						cout << " , ";
				}
#ifndef PRINT_BRACKETS_READABLE
				cout << ']';
#endif
			}

			void print_stmt(shared_ptr<stmt> s) {
				if (s == nullptr) return;
				if (s->stmt_kind() == stmt::kind::KIND_EXPRESSION) {
					print_expression(s->stmt_expression());
					cout << ';';
				}
			}

			void print_type(shared_ptr<type> t) {
				if (t == nullptr) return;
				if (t->type_const_kind() == type::const_kind::KIND_CONST) cout << "const ";
				if (t->type_static_kind() == type::static_kind::KIND_STATIC) cout << "static ";
				if (t->type_kind() == type::kind::KIND_PRIMITIVE) {
					shared_ptr<primitive_type> pt = static_pointer_cast<primitive_type>(t);
					if (pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_SIGNED)
						cout << "signed ";
					else if (pt->primitive_type_sign_kind() == primitive_type::sign_kind::KIND_UNSIGNED)
						cout << "unsigned ";
					if (pt->primitive_type_kind() == primitive_type::kind::KIND_BOOL)
						cout << "bool";
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_BYTE)
						cout << "byte";
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_CHAR)
						cout << "char";
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_DOUBLE)
						cout << "double";
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_FLOAT)
						cout << "float";
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_INT)
						cout << "int";
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_LONG)
						cout << "long";
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_SHORT)
						cout << "short";
					else if (pt->primitive_type_kind() == primitive_type::kind::KIND_VOID)
						cout << "void";
				}
				if (t->type_array_kind() == type::array_kind::KIND_ARRAY) {
					for(int i = 0; i < t->array_dimensions(); i++)
						cout << '[' << ']';
				}
			}

			void print_value_kind(value_kind vk) {
				if (vk == value_kind::VALUE_NONE)
					return;
				else if (vk == value_kind::VALUE_LVALUE)
					cout << "lvalue";
				else if (vk == value_kind::VALUE_RVALUE)
					cout << "rvalue";
			}
		}
	}
}