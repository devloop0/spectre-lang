import <"std/lib">
import <"std/syscall">
import <"std/assert">

namespace std {
	namespace lib {

		type mem_block_t* head = NULL as type mem_block_t*;

		func[static] type mem_block_t* avail_block(type mem_block_t** last,
			unsigned int size) {
			type mem_block_t* iter = head;
			while (iter as bool && !(iter->free && iter->size >= size)) {
				std::assert::assert(iter->magic == MALLOC_MAGIC, "avail_block(): found corrupted block.");
				last@ = iter;
				iter = iter->next;
			}
			return iter;
		}

		func[static] int aligned_brk() {
			using std::syscall::curr_brk;
			using std::syscall::direct_sbrk;

			byte* temp = curr_brk();
			if(temp as int == -1) return -1;
			const unsigned int temp_addr = temp as unsigned int,
				temp_word_offset = temp_addr % MALLOC_ALIGNMENT;
			if(temp_word_offset == 0) return 0;
			const unsigned int till = 
				MALLOC_ALIGNMENT - temp_word_offset;
			byte* temp2 = direct_sbrk(till);
			if(temp2 as int == -1) return -1;
			return 0;
		}

		func[static] type mem_block_t* alloc_block(type mem_block_t* last,
			unsigned int size) {
			using std::syscall::direct_sbrk;
			using std::syscall::curr_brk;

			if (aligned_brk() == -1) return NULL as type mem_block_t*;
			type mem_block_t* block = curr_brk() as type mem_block_t*;
			byte* space = direct_sbrk(size + sizeof{type mem_block_t});
			if (last as bool)
				last->next = block;

			block->size = size;
			block->next = NULL as type mem_block_t*;
			block->free = false;
			block->magic = MALLOC_MAGIC;
			return block;
		}

		func byte* malloc(const unsigned int size) {
			type mem_block_t* block = NULL as type mem_block_t*;
			if (size <= 0)
				return NULL;

			if (!(head as bool)) {
				block = alloc_block(NULL as type mem_block_t*, size);
				head = block;
			}
			else {
				type mem_block_t* last = head;
				block = avail_block(last$, size);
				if (!(block as bool))
					block = alloc_block(last, size);
				else
					block->free = false;
			}
			if (block as bool)
				return block[1]$ as byte*;
			else
				return NULL;
		}
	}
}
