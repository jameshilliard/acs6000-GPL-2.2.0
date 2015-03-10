/* -*- linux-c -*- */
/*****************************************************************
 * File: libcyc_ll.c
 *
 * Copyright (C) 2003-2004 Cyclades Corporation
 *
 * www.cyclades.com
 *
 * Description: libcyc_ll
 *
 ****************************************************************/

#include <cyc.h>

/* Treat the linked list as a stack and push data onto it */
int libcyc_push(libcyc_list **stack, void *data)
{
	if ((data == NULL) || (stack == NULL)) return(ERROR);

	if (*stack == NULL) {
		/* We have a brand-new list */
		*stack=malloc(sizeof(libcyc_list));
		(*stack)->data=data;
		(*stack)->next=NULL;
		(*stack)->prev=NULL;
	} else {
		libcyc_list *tmp = malloc(sizeof(libcyc_list));
		tmp->data=data;
		tmp->prev=NULL;
		tmp->next=*stack;
		*stack=tmp;
	}

	return(0);
}

/* Treat the linked list as a stack and pop data from it */
void *libcyc_pop(libcyc_list **stack)
{
	void *data=NULL;
	libcyc_list *tmp=NULL;

	if (*stack == NULL) return(NULL);

	if ((*stack)->next) {
		if ((*stack)->data) data=(*stack)->data;
		tmp=*stack;
		*stack=(*stack)->next;
		free(tmp);
		tmp=NULL;
	} else {
		if ((*stack)->data) data=(*stack)->data;
		free(*stack);
		*stack=NULL;
	}

	return(data);
}

/* Return a pointer to the item on the top of the stack; don't pop anything */
void *libcyc_top(libcyc_list **stack)
{
	if (*stack == NULL) return(NULL);

	return((*stack)->data);
}

/* Append data to the end of the linked list */
libcyc_list *libcyc_ll_append(libcyc_list *list, void *data)
{
	if (data == NULL) return(NULL);

	if (list == NULL) {
		/* We have a brand-new list */
		list=malloc(sizeof(libcyc_list));

		list->data=data;
		list->next=NULL;
		list->prev=NULL;
		list->active = list;
		list->last = list; /* only element: 1st == last */

		return(list);
	} else if (list && list->last) {
		/* We know where the end of the list is; just add
		 * the new data */
		list->last->next = malloc(sizeof(libcyc_list));

		list->last->next->data = data;
		list->last->next->active = NULL;
		list->last->next->prev = list->last;
		list->last->next->next = NULL;
		list->last->next->last = NULL;

		/* Point to new last element */
		list->last = list->last->next;	

		return(list);
	}

	return(NULL);
}

/* Remove the specified data from the list; return a pointer
   to the start of the new list */
libcyc_list *libcyc_ll_remove(libcyc_list *list, void *data)
{
	libcyc_list *tmp=NULL;

	if ((list == NULL) || (data == NULL)) return(NULL);

	for (tmp=list; tmp; tmp=tmp->next) {
		if ((tmp->data) && (tmp->data == data)) {
			/* We've found our element to remove */
			if ((tmp->prev == NULL) && tmp->next) {
				/* We're at the beginning of the list;
				   reassign the next element as the first */
				libcyc_list *t = tmp->next;

				t->prev = NULL;
				if (tmp->active == tmp) {
					t->active = t;
				}
				free(tmp);
				tmp = NULL;
				return(t);
			} else if (tmp->prev &&
				   (tmp->next == NULL)) {
				/* We're at the end of the list;
				   reassign the second to last as
				   the last */
				libcyc_list *t = tmp->prev;
				t->next=NULL;
				list->last = t;
				if (list->active == tmp) {
					list->active = t;
				}
				free(tmp);
				tmp = NULL;
				return(list);
			} else if (tmp->prev && tmp->next) {
				/* We're in the middle; lose the
				   specified link and reconnect
				   the ends. */
				tmp->prev->next=tmp->next;
				tmp->next->prev=tmp->prev;
				if (list->active == tmp) {
					list->active = tmp->next;
				}
				free(tmp);
				tmp = NULL;
				return(list);
			} else {
				/* Only one link in the list, so
				   tmp == list; same as libcyc_ll_free() */
				libcyc_ll_free(list);
				return(NULL);
			}
		}
	}

	return(NULL);
}

/* Free memory used by the list; data must be freed by the user program */
void libcyc_ll_free(libcyc_list *list)
{
	libcyc_list *tmp=NULL;

	if (list == NULL) return;

	for (tmp=list; tmp; tmp=tmp->next) {
		if (tmp->next == NULL) {
			/* We're at the end; free the link that
			   we're on and the previous one too.*/
			if (tmp->prev) {
				free(tmp->prev);
				tmp->prev = NULL;
			}
			free(tmp);
			tmp = NULL;
			return;
		} else if (tmp->prev && tmp->next) {
			/* We're in the middle; free the previous
			   link */
			free(tmp->prev);
			tmp->prev = NULL;
		}
	}
}


/* libcyc_list_iterate takes a linked list argument and
returns the data of each successive link */
void *libcyc_ll_iterate(libcyc_list *list)
{
	void *data = NULL;

	if (list == NULL) return(NULL);

	/* Fetch our data */
	if (list->active == list) { /* First element is active */
		data = list->data;
	} else if (list->active == NULL) {
		/* No active node: last call reached end of list */
		list->active = list; /* reset active */
		return(NULL);
	} else { /* Another element is active */
		data = list->active->data;
	}

	/* Update active pointer */
	if (list->active->next) { /* If it exists, move pointer to next */
		list->active = list->active->next;
	} else { /* We were at the end; no active node */
		list->active = NULL;
	}

	return(data);
}

void libcyc_ll_reset(libcyc_list *list)
{
	if (list) {
		list->active = list;
	}
}

/* Returns the size of the list (e.g. number of links) */
int libcyc_ll_get_size(libcyc_list *list)
{
	libcyc_list *tmp=NULL;
	int size = 0;

	if (list == NULL) return(0);

	for (tmp=list; tmp; tmp=tmp->next) size++;

	return(size);
}
