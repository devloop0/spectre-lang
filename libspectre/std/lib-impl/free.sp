import <"std/lib">
import <"std/assert">

namespace std {
	namespace lib {

		access type mem_block_t* | head;

		func void free(const byte* data) {
			if (!(data as bool) || !(head as bool)) return;

			type mem_block_t* block = (data as type mem_block_t*)[-1]$;
			
			std::assert::assert(!block->free, "free(): called free() on a non-free block.");
			std::assert::assert(block->magic == MALLOC_MAGIC, "free(): called free() on a corrupted block.");

			block->free = true;
		}
	}
}
