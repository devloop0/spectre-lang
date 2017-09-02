import <"std/syscall">
import <"std/string">

using std::syscall::direct_write;
using std::syscall::exit;

struct sql {
	long sqlcode;
}
type sql sqlca;

struct data_record {
	int dummy;
	int* a;
}
type data_record* data_ptr;
type data_record data_tmp;

func int num_records() {
	return 1;
}

func void fetch() {
	static int fetch_count;
	std::string::memset((((((((((data_tmp)))$))) as byte*))), 0x55, sizeof(data_tmp));
	sqlca.sqlcode = (++(((fetch_count))) > 1 ? 100 : 0);
}

func void load_data() {
	type data_record* p;
	int num = num_records();
	data_ptr = new type data_record(num);
	data_ptr->a = new int(100);
	std::string::memset(data_ptr as byte*, 0xaa, num * sizeof{type data_record});
	fetch();
	p = data_ptr;
	while(sqlca.sqlcode == 0) {
		p@ = data_tmp;
		p = p[1]$;
		fetch();
	}
}

func int main() {
	load_data();
	if(sizeof{int} == 2) {
		direct_write(1, "N\n", 2);
		return 1;
	}
	else if(sizeof{int} == 2 && data_ptr[0].dummy != 0x55'55) {
		direct_write(1, "N\n", 2);
		return 1;
	}
	else if(sizeof{int} > 2 && data_ptr[0].dummy != 0x55'55'55'55) {
		direct_write(1, "N\n", 2);
		return 1;
	}
	direct_write(1, "Y\n", 2);
	return 0;
}
