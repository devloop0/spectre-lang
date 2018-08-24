import "expr_eval.hsp"

namespace expr_eval {

	func type xstr::string* read_stdin() {
		type xstr::string* s = xstr::new_string("");
		constexpr auto BUF_SIZE = 1024_ui;
		char* buf = stk char(BUF_SIZE);
		sstr::memset(buf as byte*, 0, BUF_SIZE);
		while(ssys::direct_read(STDIN, buf, BUF_SIZE - 1) > 0) {
			xstr::concatc(s, buf);
			sstr::memset(buf as byte*, 0, BUF_SIZE);
		}
		return s;
	}

	func type xvec::vector* lex(type xstr::string* input) {
		auto vec = xvec::new_vector(sizeof{type token});
		
		unsigned int curr_pos = 0;
		while(true) {
			while(
				curr_pos < xstr::length(input)
				&& std::ctype::isspace(xstr::at(input, curr_pos))
			)
				curr_pos++;

			if(curr_pos >= xstr::length(input)) break;

			type token cur_tok;
			char curr = xstr::at(input, curr_pos);
			switch(curr) {
			case '+':
			case '-':
			case '*':
			case '/':
			case '%':
			case '^':
			case ';':
			case '~':
			case '(':
			case ')':
			case '?':
			case ':':
			case ',': {
				cur_tok._start_position = curr_pos;
				cur_tok._end_position = curr_pos + 1;
				cur_tok._text = xstr::new_string("");
				xstr::concatch(cur_tok._text, curr);
				if(curr == '+') cur_tok._kind = token_kind::KIND_PLUS;
				else if(curr == '-') cur_tok._kind = token_kind::KIND_MINUS;
				else if(curr == '*') cur_tok._kind = token_kind::KIND_STAR;
				else if(curr == '/') cur_tok._kind = token_kind::KIND_SLASH;
				else if(curr == '%') cur_tok._kind = token_kind::KIND_PERCENT;
				else if(curr == '^') cur_tok._kind = token_kind::KIND_CARET;
				else if(curr == ';') cur_tok._kind = token_kind::KIND_SEMICOLON;
				else if(curr == '(') cur_tok._kind = token_kind::KIND_OPAR;
				else if(curr == ')') cur_tok._kind = token_kind::KIND_CPAR;
				else if(curr == '~') cur_tok._kind = token_kind::KIND_TILDE;
				else if(curr == ',') cur_tok._kind = token_kind::KIND_COMMA;
				else if(curr == '?') cur_tok._kind = token_kind::KIND_QMARK;
				else if(curr == ':') cur_tok._kind = token_kind::KIND_COLON;
				else cur_tok._kind = token_kind::KIND_ERROR;
				curr_pos++;
			}
				break;
			case '>': {
				cur_tok._start_position = curr_pos;
				bool gt = false, sr = false, gte = false;
				if(curr_pos + 1< xstr::length(input)) {
					char lookahead = xstr::at(input, curr_pos + 1);
					if(lookahead == '>') sr = true;
					else if(lookahead == '=') gte = true;
					else gt = true;
				}
				else gt = true;
				if(gt) {
					cur_tok._end_position = curr_pos + 1;
					cur_tok._text = xstr::new_string(">");
					cur_tok._kind = token_kind::KIND_GT;
					curr_pos++;
				}
				else if(sr) {
					cur_tok._end_position = curr_pos + 2;
					cur_tok._text = xstr::new_string(">>");
					cur_tok._kind = token_kind::KIND_SHIFT_RIGHT;
					curr_pos += 2;
				}
				else {
					cur_tok._end_position = curr_pos + 2;
					cur_tok._text = xstr::new_string(">=");
					cur_tok._kind = token_kind::KIND_GTE;
					curr_pos += 2;
				}
			}
				break;
			case '<': {
				cur_tok._start_position = curr_pos;
				bool lt = false, sl = false, lte = false;
				if(curr_pos + 1 < xstr::length(input)) {
					char lookahead = xstr::at(input, curr_pos + 1);
					if(lookahead == '<') sl = true;
					else if(lookahead == '=') lte = true;
					else lt = true;
				}
				else lt = true;
				if(lt) {
					cur_tok._end_position = curr_pos + 1;
					cur_tok._text = xstr::new_string("<");
					cur_tok._kind = token_kind::KIND_LT;
					curr_pos++;
				}
				else if(sl) {
					cur_tok._end_position = curr_pos + 2;
					cur_tok._text = xstr::new_string("<<");
					cur_tok._kind = token_kind::KIND_SHIFT_LEFT;
					curr_pos += 2;
				}
				else {
					cur_tok._end_position = curr_pos + 2;
					cur_tok._text = xstr::new_string("<=");
					cur_tok._kind = token_kind::KIND_LTE;
					curr_pos += 2;
				}
			}
				break;
			case '!': {
				cur_tok._start_position = curr_pos;
				bool ne = false;
				if(curr_pos + 1 < xstr::length(input)) {
					if(xstr::at(input, curr_pos + 1) == '=') ne = true;
				}

				if(ne) {
					cur_tok._end_position = curr_pos + 2;
					cur_tok._text = xstr::new_string("!=");
					cur_tok._kind = token_kind::KIND_NEQ;
					curr_pos += 2;
				}
				else {
					cur_tok._end_position = curr_pos + 1;
					cur_tok._text = xstr::new_string("!");
					cur_tok._kind = token_kind::KIND_EXCL;
					curr_pos++;
				}
			}
				break;
			case '=': {
				cur_tok._start_position = curr_pos;
				bool err = true;
				if(curr_pos + 1 < xstr::length(input)) {
					if(xstr::at(input, curr_pos + 1) == '=') err = false;
				}
				if(err) {
					cur_tok._end_position = curr_pos + 1;
					cur_tok._text = xstr::new_string("=");
					cur_tok._kind = token_kind::KIND_ERROR;
					curr_pos++;
				}
				else {
					cur_tok._end_position = curr_pos + 2;
					cur_tok._text = xstr::new_string("==");
					cur_tok._kind = token_kind::KIND_EQ;
					curr_pos += 2;
				}
			}
				break;
			case '&':
			case '|': {
				bool next_match = false;
				cur_tok._start_position = curr_pos;
				if(curr_pos + 1 < xstr::length(input)) {
					if(xstr::at(input, curr_pos + 1) == curr) next_match = true;
				}
				if(next_match) {
					cur_tok._end_position = curr_pos + 2;
					if(curr == '&') {
						cur_tok._text = xstr::new_string("&&");
						cur_tok._kind = token_kind::KIND_DOUBLE_AMP;
					}
					else {
						cur_tok._text = xstr::new_string("||");
						cur_tok._kind = token_kind::KIND_DOUBLE_BAR;
					}
					curr_pos += 2;
				}
				else {
					cur_tok._end_position = curr_pos + 1;
					if(curr == '&') {
						cur_tok._text = xstr::new_string("&");
						cur_tok._kind = token_kind::KIND_AMP;
					}
					else {
						cur_tok._text = xstr::new_string("|");
						cur_tok._kind = token_kind::KIND_BAR;
					}
					curr_pos++;
				}
			}
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				cur_tok._start_position = curr_pos;
				auto int_str = xstr::new_string("");
				xstr::concatch(int_str, curr);
				curr_pos++;
				while(
					curr_pos < xstr::length(input)
					&& std::ctype::isdigit(xstr::at(input, curr_pos))
				) {
					xstr::concatch(int_str, xstr::at(input, curr_pos));
					curr_pos++;
				}
				cur_tok._end_position = curr_pos;
				cur_tok._text = int_str;
				cur_tok._kind = token_kind::KIND_INT;
			}
				break;
			default: {
				cur_tok._start_position = curr_pos;
				cur_tok._end_position = curr_pos + 1;
				cur_tok._text = xstr::new_string("");
				xstr::concatch(cur_tok._text, curr);
				cur_tok._kind = token_kind::KIND_ERROR;
				curr_pos++;
			}
				break;
			}
			xvec::append(vec, cur_tok$ as byte*);
		}
		return vec;
	}

	func void print_token(type expr_eval::token* t) {
		char nl = '\n', sp = ' ', d1 = '0', d0 = '0';
		auto m1 = "-1";
		std::syscall::direct_write(
			expr_eval::STDOUT,
			expr_eval::xstr::data(t->_text),
			expr_eval::xstr::length(t->_text)
		);
		std::syscall::direct_write(1, sp$, 1);
		if(t->_kind < 0) std::syscall::direct_write(expr_eval::STDOUT, m1, 2);
		else if(t->_kind < 10) {
			d0 += t->_kind;
			std::syscall::direct_write(expr_eval::STDOUT, d0$, 1);
		}
		else {
			d0 += t->_kind % 10;
			d1 += t->_kind / 10;
			std::syscall::direct_write(expr_eval::STDOUT, [d1, d0], 2);
		}
		std::syscall::direct_write(1, nl$, 1);
	}

	func type token next(type buffer* b) {
		type token tok;
		if(b->_pos >= xvec::size(b->_toks)) {
			tok._kind = token_kind::KIND_ERROR;
			return tok;
		}
		tok = xvec::at(b->_toks, b->_pos++) as type token* @;
		return tok;
	}

	func type token peek(type buffer* b) {
		type token tok;
		if(b->_pos >= xvec::size(b->_toks)) {
			tok._kind = token_kind::KIND_ERROR;
			return tok;
		}
		tok = xvec::at(b->_toks, b->_pos) as type token* @;
		return tok;
	}

	func void print_int(int i) {
		if(i == 0) {
			ssys::direct_write(STDOUT, "0", 1);
			return;
		}
		bool neg = i < 0;
		char m = '-';
		i = slib::abs(i);
		char* buf = stk char(12), iter = buf;
		while(i as bool) {
			iter@ = '0' + (i % 10);
			iter = iter[1]$;
			i /= 10;
		}
		iter = iter[-1]$;
		if(neg) ssys::direct_write(1, m$, 1);
		for(; iter != buf[-1]$; iter = iter[-1]$)
			ssys::direct_write(STDOUT, iter, 1);
	}

	func void print_uint(unsigned int u) {
		if(u == 0) {
			ssys::direct_write(STDOUT, "0", 1);
			return;
		}
		char* buf = stk char(12), iter = buf;
		for(; u as bool; u /= 10, iter = iter[1]$)
			iter@ = '0' + u % 10;
		iter = iter[-1]$;
		for(; iter != buf[-1]$; iter = iter[-1]$)
			ssys::direct_write(STDOUT, iter, 1);
	}

	func void error(type buffer* b) {
		if(b->_pos >= xvec::size(b->_toks)) {
			char* eof_err = "Unexpected EOF hit.\n";
			ssys::direct_write(STDOUT, eof_err, sstr::strlen(eof_err));
			slib::exit(1);
		}
		type token* tok = xvec::at(b->_toks, b->_pos) as type token*;
		char* err = "Error here: [";
		char d = '-', nl = '\n';
		char* err2 = "]: ";
		ssys::direct_write(STDOUT, err, sstr::strlen(err));
		print_uint(tok->_start_position);
		ssys::direct_write(STDOUT, d$, 1);
		print_uint(tok->_end_position);
		ssys::direct_write(STDOUT, err2, sstr::strlen(err2));
		ssys::direct_write(STDOUT, xstr::data(tok->_text), xstr::length(tok->_text));
		ssys::direct_write(STDOUT, nl$, 1);
		slib::exit(1);
	}

	func void prog(type buffer* b) {
		char nl = '\n';
		while(true) {
			if(b->_pos >= xvec::size(b->_toks)) return;

			type result res = expression(b);
			if(res._err || peek(b)._kind != token_kind::KIND_SEMICOLON) error(b);
			next(b);
			print_int(res._res);
			ssys::direct_write(STDOUT, nl$, 1);
		}
	}

	func type result expression(type buffer* b) {
		type result res = ternary(b);
		if(res._err) return res;
		while(peek(b)._kind == token_kind::KIND_COMMA) {
			next(b);
			res = ternary(b);
			if(res._err) return res;
		}
		return res;
	}

	func type result ternary(type buffer* b) {
		type result res = logical_or(b);
		if(res._err) return res;
		if(peek(b)._kind != token_kind::KIND_QMARK) return res;
		next(b);
		type result b1 = expression(b);
		if(b1._err || peek(b)._kind != token_kind::KIND_COLON) {
			res._err = true;
			return res;
		}
		next(b);
		type result b2 = ternary(b);
		if(b2._err) {
			res._err = true;
			return res;
		}
		res._res = res._res as bool ? b1._res : b2._res;
		return res;
	}

	func type result binary_expression_helper(int* toks, fn int(int,int)* ops, unsigned int len,
		fn type result(type buffer*) base, type buffer* b) {
		type result res = base(b);
		if(res._err) return res;
		while(true) {
			bool hit = false;
			for(unsigned int i = 0; i < len; i++) {
				if(toks[i] == peek(b)._kind) {
					hit = true;
					next(b);
					type result res2 = base(b);
					if(res2._err) return res2;
					res._res = ops[i](res._res, res2._res);
					break;
				}
			}
			if(!hit) break;
		}
		return res;
	}

	func[static] int raw_logical_or(int i, int j) { return (i as bool || j as bool) as int; }
	func type result logical_or(type buffer* b) {
		return binary_expression_helper(
			[token_kind::KIND_DOUBLE_BAR],
			[raw_logical_or],
			1,
			logical_and,
			b);
	}

	func[static] int raw_logical_and(int i, int j) { return (i as bool && j as bool) as int; }
	func type result logical_and(type buffer* b) {
		return binary_expression_helper(
			[token_kind::KIND_DOUBLE_AMP],
			[raw_logical_and],
			1,
			bitwise_or,
			b);
	}

	func[static] int raw_bitwise_or(int i, int j) { return i | j; }
	func type result bitwise_or(type buffer* b) {
		return binary_expression_helper(
			[token_kind::KIND_BAR],
			[raw_bitwise_or],
			1,
			bitwise_xor,
			b);
	}

	func[static] int raw_bitwise_xor(int i, int j) { return i ^ j; }
	func type result bitwise_xor(type buffer* b) {
		return binary_expression_helper(
			[token_kind::KIND_CARET],
			[raw_bitwise_xor],
			1,
			bitwise_and,
			b);
	}

	func[static] int raw_bitwise_and(int i, int j) { return i & j; }
	func type result bitwise_and(type buffer* b) {
		return binary_expression_helper(
			[token_kind::KIND_AMP],
			[raw_bitwise_and],
			1,
			equality,
			b);
	}

	func[static] int raw_eq(int i, int j) { return (i == j) as int; }
	func[static] int raw_neq(int i, int j) { return (i != j) as int; }
	func type result equality(type buffer* b) {
		return binary_expression_helper(
			[token_kind::KIND_EQ, token_kind::KIND_NEQ],
			[raw_eq, raw_neq],
			2,
			relational,
			b);
	}

	func[static] int raw_lt(int i, int j) { return (i < j) as int; }
	func[static] int raw_gt(int i, int j) { return (i > j) as int; }
	func[static] int raw_lte(int i, int j) { return (i <= j) as int; }
	func[static] int raw_gte(int i, int j) { return (i >= j) as int; }
	func type result relational(type buffer* b) {
		return binary_expression_helper(
			[token_kind::KIND_LT, token_kind::KIND_GT,
				token_kind::KIND_LTE, token_kind::KIND_GTE],
			[raw_lt, raw_gt, raw_lte, raw_gte],
			4,
			shift,
			b);
	}

	func[static] int raw_sl(int i, int j) { return i << j; }
	func[static] int raw_sr(int i, int j) { return i >> j; }
	func type result shift(type buffer* b) {
		return binary_expression_helper(
			[token_kind::KIND_SHIFT_LEFT, token_kind::KIND_SHIFT_RIGHT],
			[raw_sl, raw_sr],
			2,
			arith,
			b);
	}

	func[static] int raw_add(int i, int j) { return i + j; }
	func[static] int raw_sub(int i, int j) { return i - j; }
	func type result arith(type buffer* b) {
		return binary_expression_helper(
			[token_kind::KIND_PLUS, token_kind::KIND_MINUS],
			[raw_add, raw_sub],
			2,
			mult,
			b);
	}

	func[static] int raw_mul(int i, int j) { return i * j; }
	func[static] int raw_div(int i, int j) { return i / j; }
	func[static] int raw_mod(int i, int j) { return i % j; }
	func type result mult(type buffer* b) {
		return binary_expression_helper(
			[token_kind::KIND_STAR, token_kind::KIND_SLASH, token_kind::KIND_PERCENT],
			[raw_mul, raw_div, raw_mod],
			3,
			unary,
			b);
	}

	func type result unary(type buffer* b) {
		auto ops = xvec::new_vector(sizeof{int});
		while(true) {
			auto n = peek(b);
			if(n._kind == token_kind::KIND_PLUS || n._kind == token_kind::KIND_MINUS
				|| n._kind == token_kind::KIND_TILDE || n._kind == token_kind::KIND_EXCL)
				xvec::append(ops, n._kind$ as byte*);
			else break;
			next(b);
		}
		type result res = primary(b);
		if(res._err) return res;
		if(xvec::size(ops) == 0) {
			xvec::delete_vector(ops);
			return res;
		}
		for(unsigned int i = xvec::size(ops) - 1; true; i--) {
			int op = xvec::at(ops, i) as int* @;
			if(op == token_kind::KIND_PLUS);
			else if(op == token_kind::KIND_MINUS) res._res = -res._res;
			else if(op == token_kind::KIND_EXCL) res._res = (!res._res as bool) as int;
			else res._res = ~res._res;
			if(i == 0) break;
		}
		xvec::delete_vector(ops);
		return res;
	}

	func type result primary(type buffer* b) {
		if(peek(b)._kind == token_kind::KIND_INT) {
			type result res;
			res._res = slib::atoi(xstr::data(next(b)._text));
			res._err = false;
			return res;
		}
		else if(peek(b)._kind == token_kind::KIND_OPAR) {
			next(b);
			type result res = expression(b);
			if(peek(b)._kind != token_kind::KIND_CPAR) {
				res._err = true;
				return res;
			}
			next(b);
			return res;
		}
		else {
			type result res;
			res._err = true;
			return res;
		}
	}
}

func int main() {
	type expr_eval::xstr::string* file_contents = expr_eval::read_stdin();
	type expr_eval::xvec::vector* token_list = expr_eval::lex(file_contents);

	type expr_eval::buffer b;
	b._toks = token_list;
	b._pos = 0;
	expr_eval::prog(b$);

	expr_eval::xstr::delete_string(file_contents);
	expr_eval::xvec::delete_vector(token_list);
}
