#include "bit_flag.h"

/**------------------------------------------------------------------------------
 *
 */
void
bit_flag_set( struct bit_flag_t *bf, uint64_t flag )
{
	bf->_flag |= flag;
}

/**------------------------------------------------------------------------------
 *
 */
void
bit_flag_clear( struct bit_flag_t *bf, uint64_t flag )
{
	bf->_flag &= ~flag;
}

/**------------------------------------------------------------------------------
 *
 */
int
bit_flag_test( struct bit_flag_t *bf, uint64_t flag )
{
	return (bf->_flag & flag)==flag;
}

/**------------------------------------------------------------------------------
 *
 */
void
bit_flag_reset( struct bit_flag_t *bf, uint64_t flag )
{
	bf->_flag = flag;
}


/** -- EOF -- **/