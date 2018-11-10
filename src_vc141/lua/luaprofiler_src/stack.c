/*
** LuaProfiler
** Copyright Kepler Project 2005-2007 (http://www.keplerproject.org/luaprofiler)
** $Id: stack.c,v 1.4 2007-08-22 19:23:53 carregal Exp $
*/

/*****************************************************************************
stack.c:
Simple stack manipulation
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

static luaprofiler_logentry_t *s_log_free_list = NULL;

static luaprofiler_logentry_t *
logentry_alloc(luaprofiler_logentry_t **pp_free_list) {
	luaprofiler_logentry_t *entry;

	entry = (*pp_free_list);
	if (NULL != entry)
		(*pp_free_list) = entry->next;
	else
		entry = (luaprofiler_logentry_t *)malloc(sizeof(luaprofiler_logentry_t));
	return entry;
}

static void
logentry_free(luaprofiler_logentry_t **pp_free_list, luaprofiler_logentry_t *entry) {
	entry->next = (*pp_free_list);
	(*pp_free_list) = entry;
}

static void
logentry_real_free(luaprofiler_logentry_t **pp_free_list) {
	luaprofiler_logentry_t *tmp;
	while((*pp_free_list)) {
		tmp = (*pp_free_list);
		(*pp_free_list) = tmp->next;
		free(tmp);
	}
}

void lprofS_push(lprofP_STATE* S, lprofS_STACK *p, lprofS_STACK_RECORD r) {
	lprofS_STACK q;

	q = (lprofS_STACK)logentry_alloc(&s_log_free_list);
	*q = r;
	q->next = *p;
	*p = q;
}

lprofS_STACK_RECORD lprofS_pop(lprofP_STATE* S, lprofS_STACK *p) {
	lprofS_STACK_RECORD r;
	lprofS_STACK q;

	r = **p;
	q = *p;
	*p = (*p)->next;
	logentry_free(&S->log_list, q);
	return r;
}
