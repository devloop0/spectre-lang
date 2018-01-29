import <"std/lib">

namespace std {
	namespace lib {
	
		access type mem_block_t* | head;

		func void free(const byte* data) {
			if(!head as bool) return;
			type mem_block_t* iter = head;
			while(iter as bool) {
				if(iter->block == data) iter->free = true;
				if(iter->free) {
					if(iter->prev as bool) {
						if(iter->prev->free) {
							iter->prev->size += sizeof{type mem_block_t} + iter->size;
							iter->prev->next = iter->next;
						}
					}
					else if(iter->next as bool) {
						if(iter->next->free) {
							iter->size += sizeof{type mem_block_t} + iter->next->size;
							iter->next = iter->next->next;
						}
					}
				}
				iter = iter->next;
			}
		}	
	}
}
