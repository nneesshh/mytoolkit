#pragma once

#ifdef __cplusplus 
extern "C" { 
#endif 

/************************************************************************/
/* circle queue                                                         */
/************************************************************************/
#define CQ_ENTRY_DEPTH				8
#define CQ_UNBOUNDED_QUEUE_SIZE		1024 

struct cq_element_t
{
	void					*obj;
	struct cq_element_t		*next;
};

struct cq_entry
{
	unsigned int			size;
	struct cq_element_t		*elements;
};

struct circqueue
{
	unsigned int			queue_size;

	struct cq_entry			entries[CQ_ENTRY_DEPTH];
	struct cq_element_t		*pending_tail;
	struct cq_element_t		*free_list;
	unsigned int			free_count;
};

#define CQ_LENGTH(q)		((q)->queue_size-(q)->free_count) 
#define CQ_IS_EMPTY(q)		(!CQ_LENGTH(q)) 
#define CQ_IS_FULL(q)		(NULL==(q)->free_list) 

/**------------------------------------------------------------------------------
 *
 */
extern struct circqueue *		circqueue_new(unsigned int size);
extern void						circqueue_destroy(struct circqueue *cq);
extern int						circqueue_push(struct circqueue *cq, void *obj);
extern void *					circqueue_pop(struct circqueue *cq);

#ifdef __cplusplus 
} 
#endif 

/*EOF*/