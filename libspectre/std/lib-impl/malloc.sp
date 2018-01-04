import <"std/syscall">
import <"std/lib">
import <"std/machine">

namespace std {
	namespace lib {
		
		type mem_block_t* head = 0 as type mem_block_t*;

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

		func byte* malloc(const unsigned int sz) {
			using std::syscall::direct_sbrk;
			
			const unsigned int size =
				((sz + (MALLOC_ALIGNMENT - 1)) / MALLOC_ALIGNMENT)
				* MALLOC_ALIGNMENT;
			
			if(!head as bool) {
				if(aligned_brk() == -1) return NULL;
				byte* temp = direct_sbrk(sizeof{type mem_block_t} + size);
				head = temp as type mem_block_t*;
				head->prev = head->next = NULL as type mem_block_t*;
				head->free = false;
				head->size = size;
				head->block = temp[sizeof{type mem_block_t}]$;
				return head->block;
			}

			type mem_block_t* iter = head, prev = NULL as type mem_block_t*;
			while(iter as bool) {
				if(iter->free && iter->size >= size) {
					iter->free = false;
					if(size + sizeof{type mem_block_t} < iter->size) {
						byte* split_start = iter->block[size]$;
						type mem_block_t* split = split_start as type mem_block_t*;
						split->block = split_start[sizeof{type mem_block_t}]$;
						split->prev = iter;
						split->next = iter->next;
						iter->next = split;
						split->free = true;
						split->size = iter->size - size - sizeof{type mem_block_t};
						iter->size = size;
					}
					return iter->block;
				}
				prev = iter;
				iter = iter->next;
			}
			if(aligned_brk() == -1) return NULL;
			byte* temp = direct_sbrk(sizeof{type mem_block_t} + size);
			if(temp as int == -1) return NULL;
			type mem_block_t* tail = temp as type mem_block_t*;
			tail->prev = prev;
			tail->next = NULL as type mem_block_t*;
			tail->free = false;
			prev->next = tail;
			tail->size = size;
			tail->block = temp[sizeof{type mem_block_t}]$;
			return tail->block;
		}
	}
}
