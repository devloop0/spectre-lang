import <"std/syscall">
import <"std/string">

func int main() {
	using std::string::strlen;
	using std::string::strcpy;
	using std::syscall::direct_write;
	char* str1 = "Nikhil Athreya\n",
		str2 = "Nidhi Athreya \n";
	direct_write(1, str1, strlen(str1));
	direct_write(1, str2, strlen(str2));
	 strcpy(str2, str1);
	# str2 = str1;
	str2[2] = 'd';
	direct_write(1, str1, strlen(str1));
	direct_write(1, str2, strlen(str2));
}
