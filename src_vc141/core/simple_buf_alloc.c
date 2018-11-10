#include "simple_buf_alloc.h"

simple_buf_t *
simple_buf_alloc(simple_buf_t **pp_buf_freelist, size_t suggested_size) {
	simple_buf_t *list_item = (*pp_buf_freelist);
	if (list_item) {
		(*pp_buf_freelist) = list_item->_next;
	}
	else {
		list_item = malloc(suggested_size);
		list_item->_base = (char *)(list_item + 1);
		list_item->_len = suggested_size - sizeof(simple_buf_t);
		list_item->_next = 0;
	}
	return list_item;
}

void 
simple_buf_free(simple_buf_t **pp_buf_freelist, char *buf) {
	if (buf) {
		simple_buf_t *list_item = (simple_buf_t *)(buf - sizeof(simple_buf_t));
		if (list_item) {
			list_item->_next = (*pp_buf_freelist);
			(*pp_buf_freelist) = list_item;
		}
	}
}

void 
simple_buf_freelist_clear(simple_buf_t **pp_buf_freelist) {
	simple_buf_t *list_item = 0;
	while(list_item = (*pp_buf_freelist)) {
		(*pp_buf_freelist) = list_item->_next;
		free(list_item);
	}
}

/** -- EOF -- **/