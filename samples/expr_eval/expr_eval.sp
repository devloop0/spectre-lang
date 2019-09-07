import "expr_eval.hsp"

namespace expr_eval {

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
