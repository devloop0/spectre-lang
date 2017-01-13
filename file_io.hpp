#ifndef SPECTRE_FILE_IO_HPP
#define SPECTRE_FILE_IO_HPP

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace spectre {
	namespace file_io {

#ifdef _WIN32
		const static string system_include_path = "C:\\spectre-include";
#else
		const static string system_include_path = "/usr/include/spectre-include";
#endif

		string current_working_directory();
		string construct_file_name(vector<string> vec);
		bool file_exists(string fn);
		string get_file_source(string p);
		string get_parent_path(string p);
		string get_file_name(string p);
		string get_file_stem(string p);
		void write_to_file(string p, string src);
	}
}

#endif