#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <stdint.h>
#include <stdbool.h>

struct ringbuffer_t {
	uintptr_t *data;
	uint16_t  capacity;

	uint16_t  head;
	uint16_t  tail;
	uint16_t  count;
};

#define RINGBUFFER_EMPTY(buffer) ((buffer)->count == 0)
#define RINGBUFFER_FULL(buffer) (((buffer)->capacity - (buffer)->count) == 0)
#define RINGBUFFER_COUNT(buffer) (buffer)->count
#define RINGBUFFER_CAPACITY(buffer) (buffer)->capacity
#define RINGBUFFER_FREE_SIZE(buffer) ((buffer)->capacity - (buffer)->count)

extern struct ringbuffer_t * ringbuffer_create(const uint16_t capacity);
extern void ringbuffer_destroy(struct ringbuffer_t *buffer);
extern bool ringbuffer_push(struct ringbuffer_t *buffer, uintptr_t val);
extern uintptr_t ringbuffer_pop(struct ringbuffer_t *buffer, bool *out_b);
extern void ringbuffer_reset(struct ringbuffer_t *buffer);

#endif /* RINGBUFFER_H_ */