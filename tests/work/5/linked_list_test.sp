import <"std/syscall">
import <"std/string">

using std::string::NULL;
using std::syscall::direct_write;

struct linked_list {
	int val;	
	type linked_list* next;
}

func void print_linked_list(const type linked_list* ll) {
	type linked_list* l = ll as type linked_list*;
	char c;
	while(l as bool) c = l->val + '0', direct_write(1, c$, 1), c = ' ',
		direct_write(1, c$, 1), l = l->next;
	c = '\n', direct_write(1, c$, 1);
}

func void map_linked_list(type linked_list* l, fn int(int) f) {
	while(l as bool) l->val = f(l->val), l = l->next;
}

func int add_one(int x) { return x + 1; }

func int main() {
	type linked_list head; head.val = 0;
	type linked_list n1; n1.val = 1;
	type linked_list n2; n2.val = 2;
	type linked_list n3; n3.val = 3;
	type linked_list n4; n4.val = 4;
	type linked_list n5; n5.val = 5;
	type linked_list n6; n6.val = 6;
	type linked_list n7; n7.val = 7;
	type linked_list n8; n8.val = 8;

	head.next = n1$, n1.next = n2$, n2.next = n3$, n3.next = n4$,
		n4.next = n5$, n5.next = n6$, n6.next = n7$, 
		n7.next = n8$, n8.next = NULL as type linked_list*;

	print_linked_list(head$);
	map_linked_list(head$, add_one);
	print_linked_list(head$);
	return 0;
}
