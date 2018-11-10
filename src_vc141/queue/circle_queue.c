#include "circle_queue.h"

#include <stdlib.h>
#include <memory.h>

/* */
static unsigned int 
_nextpow2(unsigned int num)
{
	--num;
	num |= num >> 1;
	num |= num >> 2;
	num |= num >> 4;
	num |= num >> 8;
	num |= num >> 16;
	return ++num;
}

/* */
static struct cq_element_t *
_cq_alloc_entry(struct cq_entry *entry, unsigned int size)
{
	unsigned int i;

	entry->size = size;
	entry->elements = malloc(sizeof(struct cq_element_t) * size);

	// link free list
	for (i=0; i<size; ++i)
	{
		memset(&(entry->elements[i]), 0, sizeof(struct cq_element_t));

		if (i!=size-1)
		{
			entry->elements[i].next = &(entry->elements[i+1]);
		}
	}
	return entry->elements;
}

/* */
static int 
_cq_grow(struct circqueue *cq)
{
	unsigned int new_size = cq->queue_size << 1;
	unsigned int i;
	for (i=0; i<CQ_ENTRY_DEPTH; ++i)
	{
		if (0==cq->entries[i].size)
			break;
	}

	if (i>=CQ_ENTRY_DEPTH)
		return(-1);
	
	//
	cq->queue_size += new_size;
	
	//
	cq->free_list = _cq_alloc_entry(&(cq->entries[i]), new_size);
	cq->free_count = new_size;
	return(0);
}

/**------------------------------------------------------------------------------
 *
 */
struct circqueue *
circqueue_new(unsigned int size)
{
	struct circqueue *cq;

	if (!(cq = calloc(1, sizeof(struct circqueue)))) 
		return(NULL);
 
	//
	memset(cq, 0, sizeof *cq);

	//
	if (!size || !(cq->queue_size=_nextpow2(size))) 
	{
		cq->queue_size = CQ_UNBOUNDED_QUEUE_SIZE;
	}

	//
	cq->free_list = _cq_alloc_entry(&(cq->entries[0]), cq->queue_size);
	cq->free_count = cq->queue_size;
 	return(cq);
}

/**------------------------------------------------------------------------------
 *
 */
void 
circqueue_destroy(struct circqueue *cq)
{
	unsigned int i;
	for (i=0; i<CQ_ENTRY_DEPTH; ++i)
	{
		free(cq->entries[i].elements);
	}
	free(cq);
}

/**------------------------------------------------------------------------------
 *
 */
int 
circqueue_push(struct circqueue *cq, void *obj)
{
	struct cq_element_t *item = cq->free_list;
	if (NULL==item)
		return 0;

	cq->free_list = item->next;
	--cq->free_count;
	item->obj = obj;

	if (cq->pending_tail)
	{
		item->next = cq->pending_tail->next;
		cq->pending_tail->next = item;
		cq->pending_tail = item;
	}
	else
	{
		item->next = item;
		cq->pending_tail = item;
	}
	return 1;
}

/**------------------------------------------------------------------------------
 *
 */
void *
circqueue_pop(struct circqueue *cq)
{
	void *obj = NULL;
	if (cq->pending_tail)
	{
		struct cq_element_t *first = cq->pending_tail->next;
		obj = first->obj;

		cq->pending_tail->next = (first==cq->pending_tail) ? NULL : first->next;
		cq->pending_tail = (NULL==cq->pending_tail->next) ? NULL : cq->pending_tail;

		// recycle
		first->next = cq->free_list;
		cq->free_list = first;
		++cq->free_count;
	}
	return(obj);
}

/** -- EOF -- **/