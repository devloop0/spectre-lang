#include "file_io.hpp"
#include <stdio.h>
#include <experimental/filesystem>
#include <fstream>
#include <cstdlib>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

namespace filesystem = std::experimental::filesystem;
namespace fs = filesystem;
using std::ifstream;
using std::ofstream;
using std::getline;
using std::endl;

namespace spectre {
	namespace file_io {

		string current_working_directory() {
			char buff[FILENAME_MAX];
#ifdef _WIN32
			_getcwd(buff, FILENAME_MAX);
#else
			getcwd(buff, FILENAME_MAX);
#endif
			return string(buff);
		}

		string construct_file_name(vector<string> vec) {
			if (vec.size() == 0) return "";
			fs::path p(vec[0]);
			for (int i = 1; i < vec.size(); i++) p /= vec[i];
			return p.string();
		}

		bool file_exists(string fn) {
			return fs::exists(fn) && fs::is_regular_file(fn);
		}

		string get_file_source(string p) {
			if (!file_exists(p)) return "";
			ifstream f(p);
			string src, str;
			while (getline(f, str)) src += str + "\n";
			return src;
		}

		string get_parent_path(string p) {
			fs::path pt(p);
			return pt.parent_path().string();
		}

		string get_file_name(string p) {
			fs::path pt(p);
			return pt.filename().string();
		}

		string get_file_stem(string p) {
			fs::path pt(p);
			return pt.filename().stem().string();
		}

		void write_to_file(string p, string src) {
			ofstream ofile(p);
			if (!ofile.is_open()) return;
			ofile << src << endl;
			ofile.close();
		}
	}
}