#pragma once

#include <stdlib.h>

#ifdef __cplusplus 
extern "C" {
#endif 

typedef struct simple_buf_s {
	char				*_base;
	size_t				_len;
	struct simple_buf_s	*_next;
} simple_buf_t;

extern simple_buf_t * simple_buf_alloc(simple_buf_t **pp_buf_freelist, size_t suggested_size);
extern void simple_buf_free(simple_buf_t **pp_buf_freelist, char *buf);
extern void simple_buf_freelist_clear(simple_buf_t **pp_buf_freelist);

#ifdef __cplusplus 
}
#endif 

/*EOF*/
