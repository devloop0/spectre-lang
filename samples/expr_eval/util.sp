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
}
