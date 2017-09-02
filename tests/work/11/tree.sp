import <"std/syscall">
import <"std/string">

using namespace std::string;

struct binary_tree {
	char* value;
	type binary_tree* lhs;
	type binary_tree* rhs;
}

struct linked_list {
	char* value;
	type linked_list* next;
}

func type binary_tree* new_node(char* value) {
	type binary_tree* t = new type binary_tree(1);
	t->value = new char(strlen(value) + 1);
	strcpy(t->value, value);
	t->lhs = t->rhs = NULL as type binary_tree*;
	return t;
}

func type binary_tree* insert(type binary_tree* t, char* value) {
	if(!t as bool) return new_node(value);
	if(strcmp(value, t->value) < 0) t->lhs = insert(t->lhs, value);
	else if(strcmp(value, t->value) > 0) t->rhs = insert(t->rhs, value);
	return t;
}

func unsigned int binary_tree_size(type binary_tree* t) {
	if(!t as bool) return 0;
	return binary_tree_size(t->lhs) + 1 + binary_tree_size(t->rhs);
}

func type linked_list* sort(type binary_tree* t) {
	if(!t as bool) return NULL as type linked_list*;
	type linked_list* head, curr;
	head = sort(t->lhs);
	curr = new type linked_list(1);
	curr->value = new char(strlen(t->value) + 1);
	strcpy(curr->value, t->value);
	if(!head as bool) head = curr;
	type linked_list* iter = head;
	while(iter as bool) {
		if(iter->next as bool) iter = iter->next;
		else break;
	}
	iter->next = curr;
	curr->next = sort(t->rhs);
	return head;
}

func int main() {
	type binary_tree* root = NULL as type binary_tree*;
        root = insert(root, "1");
        insert(root, "2");
        insert(root, "4");
        insert(root, "6");
        insert(root, "7");
        insert(root, "3");
        insert(root, "3");
        insert(root, "3");
        insert(root, "9");
        insert(root, "8");
        insert(root, "5");
	type linked_list* ll = sort(root);
	while(ll as bool)
		std::syscall::direct_write(1, ll->value, strlen(ll->value)), ll = ll->next;
	char* c = new char(2);
	c[1] = 0, c[0] = '0' + binary_tree_size(root);
	std::syscall::direct_write(1, c, 1);
	c[0] = '\n';
	std::syscall::direct_write(1, c, 1);
	root = NULL as type binary_tree*;
	root = insert(root, "2");
	insert(root, "3");
	ll = sort(root);
	while(ll as bool)
		std::syscall::direct_write(1, ll->value, strlen(ll->value)), ll = ll->next;
	c[0] = '\n';
	std::syscall::direct_write(1, c, 1);
	root = NULL as type binary_tree*;
	root = insert(root, "3");
	insert(root, "2");
	ll = sort(root);
	while(ll as bool)
		std::syscall::direct_write(1, ll->value, strlen(ll->value)), ll = ll->next;
}
