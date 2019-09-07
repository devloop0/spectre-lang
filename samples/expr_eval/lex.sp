import "expr_eval.hsp"

namespace expr_eval {

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
}
