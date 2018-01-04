import <"std/string">

namespace std {
	namespace string {

		static type strtok_data_t strtok_container;
		
		func[static] const unsigned int strtok_helper(char* to_use, const char* delims, unsigned int index) {
			while(to_use[index] != 0) {
				unsigned int j = 0;
				bool d = false;
				while(delims[j] != 0) {
					if(to_use[index] == delims[j]) {
						to_use[index] = 0;
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

		func char* strtok(char* str, const char* delim) {
			char* to_use;
			unsigned int index, starting_index;
			str == NULL ?
				(to_use = strtok_container.str, index = strtok_container.pos) :
				(to_use = strtok_container.str = str, index = strtok_container.pos = 0);
			if(to_use[index] == 0) return NULL;
			index = strtok_helper(to_use, delim, index);
			starting_index = index;
			bool hit = false;
			while(to_use[index] != 0) {
				unsigned int j = 0;
				while(delim[j] != 0) {
					if(to_use[index] == delim[j]) {
						hit = true;
						break;
					}
					j++;
				}
				if(hit) break;
				index++;
			}
			index = strtok_helper(to_use, delim, index);
			strtok_container.pos = index;
			return to_use[starting_index]$;
		}
	}
}
