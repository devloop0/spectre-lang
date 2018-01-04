import <"std/lib">
import <"std/string">

namespace std {
	namespace lib {

		access type mem_block_t* | head;
	
		func byte* realloc(byte* data, const unsigned int size) {
			using std::string::memcpy;

			if(!data as bool && !size as bool) return NULL;
			else if(!data as bool) return malloc(size);
			else if(!size as bool) return free(data), NULL;
			
			unsigned int old_size = 0;
			type mem_block_t* iter = head;
			while(iter as bool) {
				if(iter->block == data) {
					old_size = iter->size;
					break;
				}
				iter = iter->next;
			}
			byte* n = malloc(size);
			if(!n as bool) return NULL;
			memcpy(n, data, size > old_size ? old_size : size);
			free(data);
			return n;
		}
	}
}
