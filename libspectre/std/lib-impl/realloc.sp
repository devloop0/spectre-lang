import <"std/lib">
import <"std/string">

namespace std {
	namespace lib {

		func byte* realloc(byte* data, const unsigned int size) {
			if(!data as bool && !size as bool) return NULL;
			else if(!data as bool) return malloc(size);
			else if(!size as bool) return free(data), NULL;

			type mem_block_t* block = (data as type mem_block_t*)[-1]$;
			if (block->size >= size)
				return data;

			byte* new_space = malloc(size);
			if (!(new_space as bool)) return NULL;

			std::string::memcpy(new_space, data, block->size);
			free(data);
			return new_space;
		}
	}
}
