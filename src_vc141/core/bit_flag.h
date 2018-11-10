#pragma once

#ifdef _MSC_VER /* msvc */
# pragma warning(disable : 4786)
# ifndef INLINE
# define INLINE __inline
# endif
#else  /* gcc */
# ifndef INLINE
# define INLINE inline
# endif
#endif

#include <stdint.h>

#ifdef __cplusplus 
extern "C" { 
#endif 

struct bit_flag_t {
	uint64_t _flag;
};

extern INLINE void				bit_flag_set( struct bit_flag_t *bf, uint64_t flag );
extern INLINE void				bit_flag_clear(struct bit_flag_t *bf, uint64_t flag);
extern INLINE int				bit_flag_test(struct bit_flag_t *bf, uint64_t flag);
extern INLINE void				bit_flag_reset(struct bit_flag_t *bf, uint64_t flag);

#ifdef __cplusplus 
}
#endif 

/*EOF*/