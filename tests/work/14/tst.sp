import <"std/syscall">
import <"std/lib">
import <"std/ctype">

using std::lib::_Exit;
using std::syscall::direct_write;
using std::lib::NULL;
using std::ctype::isalpha;

struct tst {
	int val;
	bool is_leaf;
	char data;
	type tst* lhs, middle, rhs;
}

func type tst* new_node(char data) {
	type tst* ret = new type tst(1);
	ret->is_leaf = false;
	ret->val = -1;
	ret->data = data;
	ret->lhs = ret->middle = ret->rhs = NULL as type tst*;
	return ret;
}

func bool insert(type tst** root, const char* key, int val) {
	if(!root as bool || !key as bool || !isalpha(key@)) return false;
	if(root@ as bool) {
		if(!key[1] as bool && root@->data == key@ &&
			root@->is_leaf) return false;
	}
	if(!root@ as bool) root@ = new_node(key@);
	if(!key[1] as bool) {
		root@->is_leaf = true, root@->val = val;
		return true;
	}
	if(key@ < root@->data) return insert(root@->lhs$, key[1]$, val);
	else if(key@ > root@->data) return insert(root@->rhs$, key[1]$, val);
	return insert(root@->middle$, key[1]$, val);
}

func int search(type tst* root, const char* key) {
	if(!root as bool || !key as bool || !isalpha(key@)) return -1;
	if(!key[1] as bool) return root->is_leaf && root->data == key@ ?
		root->val : -1;
	if(key@ < root->data) return search(root->lhs, key[1]$);
	else if(key@ > root->data) return search(root->rhs, key[1]$);
	return search(root->middle, key[1]$);
}

func bool delete_tst(type tst** root, const char* key) {
	if(!root as bool || !root@ as bool || !key as bool ||!isalpha(key@)) return false;
	if(root@ as bool) {
		if(!key[1] as bool && root@->data == key@ && root@->is_leaf) {
			if(!root@->lhs as bool && !root@->rhs as bool && !root@->middle as bool) root@ = NULL as type tst*;
			else root@->val = -1, root@->is_leaf = false;
			return true;
		}
	}
	bool res = false;
	if(key@ < root@->data) res = delete_tst(root@->lhs$, key[1]$);
	else if(key@ > root@->data) res = delete_tst(root@->rhs$, key[1]$);
	else res = delete_tst(root@->middle$, key[1]$);
	if(res) if(!root@->lhs as bool && !root@->rhs as bool && !root@->middle as bool) root@ = NULL as type tst*;
	return res;
}

func void fail() {
	direct_write(1, "N\n", 2);
	_Exit(1);
}

func void success() {
	direct_write(1, "Y\n", 2);
	_Exit(0);
}

func int main() {
	type tst* t = NULL as type tst*;
	if(!insert(t$, "nikhil", 12)) fail();
	if(search(t, "nikhi") != -1) fail();
	if(search(t, "nikhils") != -1) fail();
	if(search(t, "nikhil") != 12) fail();
	if(insert(t$, "nikhil", 14)) fail();
	if(!insert(t$, "katz", 61)) fail();
	if(insert(t$, "katz", 23)) fail();
	if(search(t, "nikhil") != 12 || search(t, "katz") != 61) fail();


	t = NULL as type tst*;
	if(!insert(t$, "i", 1)) fail();
	if(!insert(t$, "in", 2)) fail();
	if(!insert(t$, "intel", 3)) fail();
	if(!insert(t$, "insert", 4)) fail();
	if(!insert(t$, "insect", 5)) fail();
	
	if(insert(t$, "intel", 6) || insert(t$, "insect", 7)) fail();

	if(search(t, "i") != 1) fail();
	if(search(t, "in") != 2) fail();
	if(search(t, "intel") != 3) fail();
	if(search(t, "insert") != 4) fail();
	if(search(t, "insect") != 5) fail();

	if(delete_tst(t$, "inser")) fail();
	if(delete_tst(t$, "inserts")) fail();
	if(!delete_tst(t$, "insert")) fail();

	if(search(t, "i") != 1) fail();
	if(search(t, "in") != 2) fail();
	if(search(t, "intel") != 3) fail();
	if(search(t, "insert") != -1) fail();
	if(search(t, "insect") != 5) fail();

	if(delete_tst(t$, "insert")) fail();
	if(!delete_tst(t$, "in")) fail();

	if(search(t, "i") != 1) fail();
	if(search(t, "in") != -1) fail();
	if(search(t, "intel") != 3) fail();
	if(search(t, "insert") != -1) fail();
	if(search(t, "insect") != 5) fail();

	if(!insert(t$, "insert", 8)) fail();

	if(search(t, "i") != 1) fail();
	if(search(t, "in") != -1) fail();
	if(search(t, "intel") != 3) fail();
	if(search(t, "insert") != 8) fail();
	if(search(t, "insect") != 5) fail();

	if(!insert(t$, "in", 9)) fail();

	if(search(t, "i") != 1) fail();
	if(search(t, "in") != 9) fail();
	if(search(t, "intel") != 3) fail();
	if(search(t, "insert") != 8) fail();
	if(search(t, "insect") != 5) fail();


	t = NULL as type tst*;
	if(!insert(t$, "in", 2)) fail();
	if(!insert(t$, "int", 3)) fail();
	if(!insert(t$, "insert", 6)) fail();
	if(!insert(t$, "i", 1)) fail();
	if(!insert(t$, "insect", 5)) fail();
	if(!insert(t$, "intel", 4)) fail();

	if(insert(t$, "i", 7)) fail();
	if(insert(t$, "insect", 8)) fail();
	if(insert(t$, "intel", 9)) fail();

	if(search(t, "i") != 1) fail();
	if(search(t, "in") != 2) fail();
	if(search(t, "int") != 3) fail();
	if(search(t, "intel") != 4) fail();
	if(search(t, "insect") != 5) fail();
	if(search(t, "insert") != 6) fail();

	if(delete_tst(t$, "ints")) fail();

	if(search(t, "i") != 1) fail();
	if(search(t, "in") != 2) fail();
	if(search(t, "int") != 3) fail();
	if(search(t, "intel") != 4) fail();
	if(search(t, "insect") != 5) fail();
	if(search(t, "insert") != 6) fail();

	if(delete_tst(t$, "ints")) fail();
	if(!delete_tst(t$, "int")) fail();

	if(search(t, "i") != 1) fail();
	if(search(t, "in") != 2) fail();
	if(search(t, "int") != -1) fail();
	if(search(t, "intel") != 4) fail();
	if(search(t, "insect") != 5) fail();
	if(search(t, "insert") != 6) fail();

	if(!insert(t$, "int", 7)) fail();

	if(search(t, "i") != 1) fail();
	if(search(t, "in") != 2) fail();
	if(search(t, "int") != 7) fail();
	if(search(t, "intel") != 4) fail();
	if(search(t, "insect") != 5) fail();
	if(search(t, "insert") != 6) fail();

	if(!delete_tst(t$, "i")) fail();

	if(search(t, "i") != -1) fail();
	if(search(t, "in") != 2) fail();
	if(search(t, "int") != 7) fail();
	if(search(t, "intel") != 4) fail();
	if(search(t, "insect") != 5) fail();
	if(search(t, "insert") != 6) fail();

	if(!delete_tst(t$, "int")) fail();

	if(search(t, "i") != -1) fail();
	if(search(t, "in") != 2) fail();
	if(search(t, "int") != -1) fail();
	if(search(t, "intel") != 4) fail();
	if(search(t, "insect") != 5) fail();
	if(search(t, "insert") != 6) fail();

	if(!delete_tst(t$, "in")) fail();

	if(search(t, "i") != -1) fail();
	if(search(t, "in") != -1) fail();
	if(search(t, "int") != -1) fail();
	if(search(t, "intel") != 4) fail();
	if(search(t, "insect") != 5) fail();
	if(search(t, "insert") != 6) fail();

	if(!delete_tst(t$, "intel")) fail();

	if(search(t, "i") != -1) fail();
	if(search(t, "in") != -1) fail();
	if(search(t, "int") != -1) fail();
	if(search(t, "intel") != -1) fail();
	if(search(t, "insect") != 5) fail();
	if(search(t, "insert") != 6) fail();

	if(!delete_tst(t$, "insert")) fail();

	if(search(t, "i") != -1) fail();
	if(search(t, "in") != -1) fail();
	if(search(t, "int") != -1) fail();
	if(search(t, "intel") != -1) fail();
	if(search(t, "insect") != 5) fail();
	if(search(t, "insert") != -1) fail();

	if(!delete_tst(t$, "insect")) fail();

	if(search(t, "i") != -1) fail();
	if(search(t, "in") != -1) fail();
	if(search(t, "int") != -1) fail();
	if(search(t, "intel") != -1) fail();
	if(search(t, "insect") != -1) fail();
	if(search(t, "insert") != -1) fail();
	success();
}
