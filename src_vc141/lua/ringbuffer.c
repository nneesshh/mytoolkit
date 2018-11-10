#include "ringbuffer.h"

#define ELEMENT_SIZE sizeof(uintptr_t)

static inline bool
ringbuffer_check_validity(const struct ringbuffer_t *buffer)
{
	return buffer != NULL && buffer->data != NULL
		&& buffer->head < buffer->capacity && buffer->tail < buffer->capacity
		&& buffer->count <= buffer->capacity;
}

struct ringbuffer_t *
	ringbuffer_create(const uint16_t capacity)
{
	struct ringbuffer_t *buffer = (struct ringbuffer_t *)calloc(1, sizeof(struct ringbuffer_t));
	buffer->data = (uintptr_t *)malloc(ELEMENT_SIZE * capacity);
	buffer->capacity = capacity;
	buffer->head = buffer->tail = buffer->count = 0;
	return buffer;
}

void
ringbuffer_destroy(struct ringbuffer_t *buffer)
{
	free(buffer->data);
	free(buffer);
}

inline bool
ringbuffer_push(struct ringbuffer_t *buffer, uintptr_t val)
{
	if (!ringbuffer_check_validity(buffer))
		return false;

	if (RINGBUFFER_FULL(buffer)) {
		uint16_t capacity = buffer->capacity;
		uintptr_t *data = buffer->data;

		buffer->data = (uintptr_t *)malloc(ELEMENT_SIZE * capacity * 2);
		buffer->capacity = capacity * 2;

		memcpy(buffer->data, data + ELEMENT_SIZE * buffer->head, ELEMENT_SIZE * (buffer->capacity - buffer->head));
		if (buffer->tail > 0) memcpy(buffer->data, data, ELEMENT_SIZE * buffer->tail);

		buffer->head = 0;
		buffer->tail = capacity;

		free(data);
	}

	buffer->data[buffer->tail] = val;
	buffer->tail = (buffer->tail + 1 >= buffer->capacity) ? 0 : buffer->tail + 1;
	++buffer->count;
	return true;
}

inline uintptr_t
ringbuffer_pop(struct ringbuffer_t *buffer, bool *out_b)
{
	uintptr_t result = 0;
	if (RINGBUFFER_EMPTY(buffer)) {
		*out_b = false;
	}
	else {
		result = buffer->data[buffer->head];
		buffer->head = (buffer->head + 1 >= buffer->capacity) ? 0 : buffer->head + 1;
		--buffer->count;
		*out_b = true;
	}
	return result;
}

inline void
ringbuffer_reset(struct ringbuffer_t *buffer)
{
	buffer->count = buffer->tail = buffer->head = 0;
}