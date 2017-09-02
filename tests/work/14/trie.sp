import <"std/lib">
import <"std/syscall">
import <"std/ctype">

using std::syscall::direct_write;
using std::lib::exit;
using std::lib::_Exit;
using std::ctype::isalpha;
using std::lib::NULL;

const unsigned int ALPHABET_SIZE = 26;

struct trie {
	type trie** children;
	int val;
	bool is_leaf;
}

func type trie* new_node() {
	type trie* ret = new type trie(1);
	if(!ret as bool) return NULL as type trie*;
	ret->val = -1;
	ret->is_leaf = false;
	ret->children = new type trie*(ALPHABET_SIZE);
	for(int i = 0; i < ALPHABET_SIZE; i++) ret->children[i] = NULL as type trie*;
	return ret;
}

func bool insert(type trie* root, const char* key, int val) {
	if(!key as bool || !root as bool || !isalpha(key@)) return false;
	if(!key[1] as bool)
		if(!root->is_leaf) {
			root->is_leaf = true, root->val = val;
			return true;
		}
		else return false;
	if(!root->children[key@ - 'a'] as bool) root->children[key@ - 'a'] = new_node();
	return insert(root->children[key@ - 'a'], key[1]$, val);
}

func int search(type trie* root, const char* key) {
	if(!key as bool || !root as bool || !isalpha(key@)) return -1;
	if(!key[1] as bool) return root->is_leaf ? root->val : -1;
	return search(root->children[key@ - 'a'], key[1]$);
}

func void fail() {
	direct_write(1, "N\n", 2);
	_Exit(1);
}

func void success() {
	direct_write(1, "Y\n", 2);
	exit(0);
}

func int main() { 
	type trie* t = new_node();
	if(!insert(t, "nikhil", 12)) fail();
	if(search(t, "nikhi") != -1) fail();
	if(search(t, "nikhils") != -1) fail();
	if(search(t, "nikhil") != 12) fail();
	if(insert(t, "nikhil", 14)) fail();
	if(search(t, "nikhil") != 12) fail();
	if(!insert(t, "katz", 61)) fail();
	if(insert(t, "katz", 23)) fail();
	if(search(t, "katz") != 61) fail();
	if(search(t, "nikhil") != 12) fail();
	success();
}
