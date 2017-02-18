import <"std/string">
import <"std/core">

namespace std {
	namespace string {

		static type _detail::strtok_container _helper;
		_helper.str_to_process = std::core::char_nullptr;
		_helper.current_index = 0;

		func[static] const unsigned int strtok_helper(char* to_use __rfuncarg__,
			const char* delims __rfuncarg__, unsigned int index __rfuncarg__) {
			while(to_use[index] != '\0') {
				unsigned int j = 0;
				bool d = false;
				while(delims[j] != '\0') {
					if(to_use[index] == delims[j]) {
						to_use[index] = '\0';
						d = true;
						break;
					}
					j++;
				}
				if(!d) break;
				index++;
			}
			return index;
		}

		func char* strtok(char* str __rfuncarg__, const char* delims __rfuncarg__) {
			using std::core::char_nullptr;
			char* to_use;
			unsigned int index, starting_index;
			str == char_nullptr ?
				(to_use = _helper.str_to_process, index = _helper.current_index) :
				(to_use = _helper.str_to_process = str, index = _helper.current_index = 0);
			if(to_use[index] == '\0') return char_nullptr;
			index = strtok_helper(to_use, delims, index);
			starting_index = index;
			bool hit = false;
			while(to_use[index] != '\0') {
				unsigned int j = 0;
				while(delims[j] != '\0') {
					if(to_use[index] == delims[j]) {
						hit = true;
						break;
					}
					j++;
				}
				if(hit) break;
				index++;
			}
			index = strtok_helper(to_use, delims, index);
			_helper.current_index = index;
			return to_use[starting_index]$;
		}
	}
}
