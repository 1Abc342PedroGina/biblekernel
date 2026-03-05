#ifndef _BIBLE_QUEUE_H_
#define _BIBLE_QUEUE_H_

#include <bible/cdefs.h>
#include <bible/types.h>


/*
 * LISTA ENCADEADA SIMPLES (SINGLY-LINKED LIST)
 */

/*
 * Definições de macros para lista encadeada simples
 */
#define BK_SLIST_HEAD(name, type)					\
struct name {								\
	struct type *slh_first;	/* primeiro elemento */		\
}

#define BK_SLIST_HEAD_INITIALIZER(head)				\
	{ NULL }

#define BK_SLIST_ENTRY(type)						\
struct {								\
	struct type *sle_next;	/* próximo elemento */		\
}

/*
 * Operações de lista encadeada simples
 */

/* Inicialização */
#define BK_SLIST_INIT(head) do {					\
	(head)->slh_first = NULL;					\
} while (0)

/* Verifica se a lista está vazia */
#define BK_SLIST_EMPTY(head)	((head)->slh_first == NULL)

/* Obtém o primeiro elemento */
#define BK_SLIST_FIRST(head)	((head)->slh_first)

/* Obtém o próximo elemento */
#define BK_SLIST_NEXT(elm, field)	((elm)->field.sle_next)

/*
 * Percorre a lista
 */
#define BK_SLIST_FOREACH(var, head, field)				\
	for ((var) = BK_SLIST_FIRST((head));				\
	    (var);							\
	    (var) = BK_SLIST_NEXT((var), field))

/*
 * Percorre a lista de forma segura (permite remoção durante a iteração)
 */
#define BK_SLIST_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = BK_SLIST_FIRST((head));				\
	    (var) && ((tvar) = BK_SLIST_NEXT((var), field), 1);		\
	    (var) = (tvar))

/*
 * Insere no início da lista
 */
#define BK_SLIST_INSERT_HEAD(head, elm, field) do {			\
	(elm)->field.sle_next = (head)->slh_first;			\
	(head)->slh_first = (elm);					\
} while (0)

/*
 * Insere após um elemento específico
 */
#define BK_SLIST_INSERT_AFTER(slistelm, elm, field) do {		\
	(elm)->field.sle_next = (slistelm)->field.sle_next;		\
	(slistelm)->field.sle_next = (elm);				\
} while (0)

/*
 * Remove o primeiro elemento da lista
 */
#define BK_SLIST_REMOVE_HEAD(head, field) do {				\
	(head)->slh_first = (head)->slh_first->field.sle_next;		\
} while (0)

/*
 * Remove um elemento específico da lista
 */
#define BK_SLIST_REMOVE(head, elm, type, field) do {			\
	if ((head)->slh_first == (elm)) {				\
		BK_SLIST_REMOVE_HEAD((head), field);			\
	} else {							\
		struct type *curelm = (head)->slh_first;		\
		while (curelm->field.sle_next != (elm))			\
			curelm = curelm->field.sle_next;		\
		curelm->field.sle_next =				\
		    curelm->field.sle_next->field.sle_next;		\
	}								\
} while (0)

/*
 * LISTA ENCADEADA SIMPLES COM CAUDA (SINGLY-LINKED TAIL QUEUE)
 */

/*
 * Definições de macros para lista encadeada simples com cauda
 */
#define BK_STAILQ_HEAD(name, type)					\
struct name {								\
	struct type *stqh_first;	/* primeiro elemento */		\
	struct type **stqh_last;	/* ponteiro para o último próximo */	\
}

#define BK_STAILQ_HEAD_INITIALIZER(head)				\
	{ NULL, &(head).stqh_first }

#define BK_STAILQ_ENTRY(type)						\
struct {								\
	struct type *stqe_next;	/* próximo elemento */		\
}

/*
 * Operações de lista encadeada simples com cauda
 */

/* Inicialização */
#define BK_STAILQ_INIT(head) do {					\
	(head)->stqh_first = NULL;					\
	(head)->stqh_last = &(head)->stqh_first;			\
} while (0)

/* Verifica se a lista está vazia */
#define BK_STAILQ_EMPTY(head)	((head)->stqh_first == NULL)

/* Obtém o primeiro elemento */
#define BK_STAILQ_FIRST(head)	((head)->stqh_first)

/* Obtém o próximo elemento */
#define BK_STAILQ_NEXT(elm, field)	((elm)->field.stqe_next)

/* Obtém o último elemento */
#define BK_STAILQ_LAST(head, type, field)				\
	(BK_STAILQ_EMPTY(head) ? NULL :					\
	    __containerof((head)->stqh_last, struct type, field.stqe_next))

/*
 * Percorre a lista
 */
#define BK_STAILQ_FOREACH(var, head, field)				\
	for ((var) = BK_STAILQ_FIRST((head));				\
	    (var);							\
	    (var) = BK_STAILQ_NEXT((var), field))

/*
 * Percorre a lista de forma segura
 */
#define BK_STAILQ_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = BK_STAILQ_FIRST((head));				\
	    (var) && ((tvar) = BK_STAILQ_NEXT((var), field), 1);	\
	    (var) = (tvar))

/*
 * Insere no início da lista
 */
#define BK_STAILQ_INSERT_HEAD(head, elm, field) do {			\
	if (((elm)->field.stqe_next = (head)->stqh_first) == NULL)	\
		(head)->stqh_last = &(elm)->field.stqe_next;		\
	(head)->stqh_first = (elm);					\
} while (0)

/*
 * Insere no final da lista
 */
#define BK_STAILQ_INSERT_TAIL(head, elm, field) do {			\
	(elm)->field.stqe_next = NULL;					\
	*(head)->stqh_last = (elm);					\
	(head)->stqh_last = &(elm)->field.stqe_next;			\
} while (0)

/*
 * Insere após um elemento específico
 */
#define BK_STAILQ_INSERT_AFTER(head, listelm, elm, field) do {		\
	if (((elm)->field.stqe_next = (listelm)->field.stqe_next) == NULL) \
		(head)->stqh_last = &(elm)->field.stqe_next;		\
	(listelm)->field.stqe_next = (elm);				\
} while (0)

/*
 * Remove o primeiro elemento da lista
 */
#define BK_STAILQ_REMOVE_HEAD(head, field) do {			\
	if (((head)->stqh_first = (head)->stqh_first->field.stqe_next) == NULL) \
		(head)->stqh_last = &(head)->stqh_first;		\
} while (0)

/*
 * Remove um elemento específico da lista
 */
#define BK_STAILQ_REMOVE(head, elm, type, field) do {			\
	if ((head)->stqh_first == (elm)) {				\
		BK_STAILQ_REMOVE_HEAD((head), field);			\
	} else {							\
		struct type *curelm = (head)->stqh_first;		\
		while (curelm->field.stqe_next != (elm))		\
			curelm = curelm->field.stqe_next;		\
		if ((curelm->field.stqe_next =				\
			curelm->field.stqe_next->field.stqe_next) == NULL) \
			(head)->stqh_last = &(curelm->field.stqe_next); \
	}								\
} while (0)

/*
 * LISTA DUPLAMENTE ENCADEADA (DOUBLY-LINKED LIST)
 */

/*
 * Definições de macros para lista duplamente encadeada
 */
#define BK_LIST_HEAD(name, type)					\
struct name {								\
	struct type *lh_first;	/* primeiro elemento */		\
}

#define BK_LIST_HEAD_INITIALIZER(head)				\
	{ NULL }

#define BK_LIST_ENTRY(type)						\
struct {								\
	struct type *le_next;	/* próximo elemento */		\
	struct type **le_prev;	/* ponteiro para o ponteiro anterior */ \
}

/*
 * Operações de lista duplamente encadeada
 */

/* Inicialização */
#define BK_LIST_INIT(head) do {						\
	(head)->lh_first = NULL;					\
} while (0)

/* Verifica se a lista está vazia */
#define BK_LIST_EMPTY(head)	((head)->lh_first == NULL)

/* Obtém o primeiro elemento */
#define BK_LIST_FIRST(head)	((head)->lh_first)

/* Obtém o próximo elemento */
#define BK_LIST_NEXT(elm, field)	((elm)->field.le_next)

/*
 * Percorre a lista
 */
#define BK_LIST_FOREACH(var, head, field)				\
	for ((var) = BK_LIST_FIRST((head));				\
	    (var);							\
	    (var) = BK_LIST_NEXT((var), field))

/*
 * Percorre a lista de forma segura
 */
#define BK_LIST_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = BK_LIST_FIRST((head));				\
	    (var) && ((tvar) = BK_LIST_NEXT((var), field), 1);		\
	    (var) = (tvar))

/*
 * Insere no início da lista
 */
#define BK_LIST_INSERT_HEAD(head, elm, field) do {			\
	if (((elm)->field.le_next = (head)->lh_first) != NULL)		\
		(head)->lh_first->field.le_prev = &(elm)->field.le_next; \
	(head)->lh_first = (elm);					\
	(elm)->field.le_prev = &(head)->lh_first;			\
} while (0)

/*
 * Insere antes de um elemento específico
 */
#define BK_LIST_INSERT_BEFORE(listelm, elm, field) do {		\
	(elm)->field.le_prev = (listelm)->field.le_prev;		\
	(elm)->field.le_next = (listelm);				\
	*(listelm)->field.le_prev = (elm);				\
	(listelm)->field.le_prev = &(elm)->field.le_next;		\
} while (0)

/*
 * Insere após um elemento específico
 */
#define BK_LIST_INSERT_AFTER(listelm, elm, field) do {			\
	if (((elm)->field.le_next = (listelm)->field.le_next) != NULL) \
		(listelm)->field.le_next->field.le_prev =		\
		    &(elm)->field.le_next;				\
	(listelm)->field.le_next = (elm);				\
	(elm)->field.le_prev = &(listelm)->field.le_next;		\
} while (0)

/*
 * Remove um elemento específico da lista
 */
#define BK_LIST_REMOVE(elm, field) do {				\
	if ((elm)->field.le_next != NULL)				\
		(elm)->field.le_next->field.le_prev =			\
		    (elm)->field.le_prev;				\
	*(elm)->field.le_prev = (elm)->field.le_next;			\
} while (0)

/*
 * LISTA DUPLAMENTE ENCADEADA COM CAUDA (DOUBLY-LINKED TAIL QUEUE)
 */

/*
 * Definições de macros para lista duplamente encadeada com cauda
 */
#define BK_TAILQ_HEAD(name, type)					\
struct name {								\
	struct type *tqh_first;	/* primeiro elemento */		\
	struct type **tqh_last;	/* ponteiro para o último elemento */	\
}

#define BK_TAILQ_HEAD_INITIALIZER(head)				\
	{ NULL, &(head).tqh_first }

#define BK_TAILQ_ENTRY(type)						\
struct {								\
	struct type *tqe_next;	/* próximo elemento */		\
	struct type **tqe_prev;	/* ponteiro para o elemento anterior */ \
}

/*
 * Operações de lista duplamente encadeada com cauda
 */

/* Inicialização */
#define BK_TAILQ_INIT(head) do {					\
	(head)->tqh_first = NULL;					\
	(head)->tqh_last = &(head)->tqh_first;				\
} while (0)

/* Verifica se a lista está vazia */
#define BK_TAILQ_EMPTY(head)	((head)->tqh_first == NULL)

/* Obtém o primeiro elemento */
#define BK_TAILQ_FIRST(head)	((head)->tqh_first)

/* Obtém o próximo elemento */
#define BK_TAILQ_NEXT(elm, field)	((elm)->field.tqe_next)

/* Obtém o último elemento */
#define BK_TAILQ_LAST(head, headname)				\
	(*(((struct headname *)((head)->tqh_last))->tqh_last))

/* Obtém o elemento anterior */
#define BK_TAILQ_PREV(elm, headname, field)				\
	(*(((struct headname *)((elm)->field.tqe_prev))->tqh_last))

/*
 * Percorre a lista para frente
 */
#define BK_TAILQ_FOREACH(var, head, field)				\
	for ((var) = BK_TAILQ_FIRST((head));				\
	    (var);							\
	    (var) = BK_TAILQ_NEXT((var), field))

/*
 * Percorre a lista para trás
 */
#define BK_TAILQ_FOREACH_REVERSE(var, head, headname, field)		\
	for ((var) = BK_TAILQ_LAST((head), headname);			\
	    (var);							\
	    (var) = BK_TAILQ_PREV((var), headname, field))

/*
 * Percorre a lista para frente de forma segura
 */
#define BK_TAILQ_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = BK_TAILQ_FIRST((head));				\
	    (var) && ((tvar) = BK_TAILQ_NEXT((var), field), 1);	\
	    (var) = (tvar))

/*
 * Percorre a lista para trás de forma segura
 */
#define BK_TAILQ_FOREACH_REVERSE_SAFE(var, head, headname, field, tvar) \
	for ((var) = BK_TAILQ_LAST((head), headname);			\
	    (var) && ((tvar) = BK_TAILQ_PREV((var), headname, field), 1); \
	    (var) = (tvar))

/*
 * Insere no início da lista
 */
#define BK_TAILQ_INSERT_HEAD(head, elm, field) do {			\
	if (((elm)->field.tqe_next = (head)->tqh_first) != NULL)	\
		(head)->tqh_first->field.tqe_prev =			\
		    &(elm)->field.tqe_next;				\
	else								\
		(head)->tqh_last = &(elm)->field.tqe_next;		\
	(head)->tqh_first = (elm);					\
	(elm)->field.tqe_prev = &(head)->tqh_first;			\
} while (0)

/*
 * Insere no final da lista
 */
#define BK_TAILQ_INSERT_TAIL(head, elm, field) do {			\
	(elm)->field.tqe_next = NULL;					\
	(elm)->field.tqe_prev = (head)->tqh_last;			\
	*(head)->tqh_last = (elm);					\
	(head)->tqh_last = &(elm)->field.tqe_next;			\
} while (0)

/*
 * Insere antes de um elemento específico
 */
#define BK_TAILQ_INSERT_BEFORE(listelm, elm, field) do {		\
	(elm)->field.tqe_prev = (listelm)->field.tqe_prev;		\
	(elm)->field.tqe_next = (listelm);				\
	*(listelm)->field.tqe_prev = (elm);				\
	(listelm)->field.tqe_prev = &(elm)->field.tqe_next;		\
} while (0)

/*
 * Insere após um elemento específico
 */
#define BK_TAILQ_INSERT_AFTER(head, listelm, elm, field) do {		\
	if (((elm)->field.tqe_next = (listelm)->field.tqe_next) != NULL) \
		(elm)->field.tqe_next->field.tqe_prev =			\
		    &(elm)->field.tqe_next;				\
	else								\
		(head)->tqh_last = &(elm)->field.tqe_next;		\
	(listelm)->field.tqe_next = (elm);				\
	(elm)->field.tqe_prev = &(listelm)->field.tqe_next;		\
} while (0)

/*
 * Remove um elemento específico da lista
 */
#define BK_TAILQ_REMOVE(head, elm, field) do {				\
	if (((elm)->field.tqe_next) != NULL)				\
		(elm)->field.tqe_next->field.tqe_prev =			\
		    (elm)->field.tqe_prev;				\
	else								\
		(head)->tqh_last = (elm)->field.tqe_prev;		\
	*(elm)->field.tqe_prev = (elm)->field.tqe_next;		\
} while (0)

/*
 * Concatena duas listas
 */
#define BK_TAILQ_CONCAT(head1, head2, field) do {			\
	if (!BK_TAILQ_EMPTY(head2)) {					\
		*(head1)->tqh_last = (head2)->tqh_first;		\
		(head2)->tqh_first->field.tqe_prev = (head1)->tqh_last; \
		(head1)->tqh_last = (head2)->tqh_last;			\
		BK_TAILQ_INIT((head2));					\
	}								\
} while (0)

/*
 * FILA (CIRCULAR QUEUE)
 */

/*
 * Definições de macros para fila circular
 */
#define BK_CIRCLEQ_HEAD(name, type)					\
struct name {								\
	struct type *cqh_first;	/* primeiro elemento */		\
	struct type *cqh_last;	/* último elemento */		\
}

#define BK_CIRCLEQ_HEAD_INITIALIZER(head)				\
	{ (void *)&head, (void *)&head }

#define BK_CIRCLEQ_ENTRY(type)						\
struct {								\
	struct type *cqe_next;	/* próximo elemento */		\
	struct type *cqe_prev;	/* elemento anterior */		\
}

/*
 * Operações de fila circular
 */

/* Inicialização */
#define BK_CIRCLEQ_INIT(head) do {					\
	(head)->cqh_first = (void *)(head);				\
	(head)->cqh_last = (void *)(head);				\
} while (0)

/* Verifica se a fila está vazia */
#define BK_CIRCLEQ_EMPTY(head)						\
	((head)->cqh_first == (void *)(head))

/* Obtém o primeiro elemento */
#define BK_CIRCLEQ_FIRST(head)	((head)->cqh_first)

/* Obtém o último elemento */
#define BK_CIRCLEQ_LAST(head)	((head)->cqh_last)

/* Obtém o próximo elemento */
#define BK_CIRCLEQ_NEXT(elm, field)	((elm)->field.cqe_next)

/* Obtém o elemento anterior */
#define BK_CIRCLEQ_PREV(elm, field)	((elm)->field.cqe_prev)

/*
 * Percorre a fila para frente
 */
#define BK_CIRCLEQ_FOREACH(var, head, field)				\
	for ((var) = BK_CIRCLEQ_FIRST((head));				\
	    (var) != (void *)(head);					\
	    (var) = BK_CIRCLEQ_NEXT((var), field))

/*
 * Percorre a fila para trás
 */
#define BK_CIRCLEQ_FOREACH_REVERSE(var, head, field)			\
	for ((var) = BK_CIRCLEQ_LAST((head));				\
	    (var) != (void *)(head);					\
	    (var) = BK_CIRCLEQ_PREV((var), field))

/*
 * Insere no início da fila
 */
#define BK_CIRCLEQ_INSERT_HEAD(head, elm, field) do {			\
	(elm)->field.cqe_next = (head)->cqh_first;			\
	(elm)->field.cqe_prev = (void *)(head);				\
	if ((head)->cqh_last == (void *)(head))				\
		(head)->cqh_last = (elm);				\
	else								\
		(head)->cqh_first->field.cqe_prev = (elm);		\
	(head)->cqh_first = (elm);					\
} while (0)

/*
 * Insere no final da fila
 */
#define BK_CIRCLEQ_INSERT_TAIL(head, elm, field) do {			\
	(elm)->field.cqe_next = (void *)(head);				\
	(elm)->field.cqe_prev = (head)->cqh_last;			\
	if ((head)->cqh_first == (void *)(head))			\
		(head)->cqh_first = (elm);				\
	else								\
		(head)->cqh_last->field.cqe_next = (elm);		\
	(head)->cqh_last = (elm);					\
} while (0)

/*
 * Insere antes de um elemento específico
 */
#define BK_CIRCLEQ_INSERT_BEFORE(head, listelm, elm, field) do {	\
	(elm)->field.cqe_next = (listelm);				\
	(elm)->field.cqe_prev = (listelm)->field.cqe_prev;		\
	(listelm)->field.cqe_prev->field.cqe_next = (elm);		\
	(listelm)->field.cqe_prev = (elm);				\
	if ((listelm) == (head)->cqh_first)				\
		(head)->cqh_first = (elm);				\
} while (0)

/*
 * Insere após um elemento específico
 */
#define BK_CIRCLEQ_INSERT_AFTER(head, listelm, elm, field) do {		\
	(elm)->field.cqe_next = (listelm)->field.cqe_next;		\
	(elm)->field.cqe_prev = (listelm);				\
	(listelm)->field.cqe_next->field.cqe_prev = (elm);		\
	(listelm)->field.cqe_next = (elm);				\
	if ((listelm) == (head)->cqh_last)				\
		(head)->cqh_last = (elm);				\
} while (0)

/*
 * Remove um elemento específico da fila
 */
#define BK_CIRCLEQ_REMOVE(head, elm, field) do {			\
	(elm)->field.cqe_prev->field.cqe_next = (elm)->field.cqe_next; \
	(elm)->field.cqe_next->field.cqe_prev = (elm)->field.cqe_prev; \
	if ((elm) == (head)->cqh_last)					\
		(head)->cqh_last = (elm)->field.cqe_prev;		\
	if ((elm) == (head)->cqh_first)					\
		(head)->cqh_first = (elm)->field.cqe_next;		\
} while (0)

/*
 * Helper macro para containerof
 */
#ifndef __containerof
#define __containerof(ptr, type, field)				\
	((type *)((char *)(ptr) - __BK_OFFSETOF(type, field)))
#endif

/*
 * Compatibilidade com APIs BSD
 */
#ifndef _BK_NO_COMPAT

#define SLIST_HEAD		BK_SLIST_HEAD
#define SLIST_HEAD_INITIALIZER	BK_SLIST_HEAD_INITIALIZER
#define SLIST_ENTRY		BK_SLIST_ENTRY
#define SLIST_INIT		BK_SLIST_INIT
#define SLIST_EMPTY		BK_SLIST_EMPTY
#define SLIST_FIRST		BK_SLIST_FIRST
#define SLIST_NEXT		BK_SLIST_NEXT
#define SLIST_FOREACH		BK_SLIST_FOREACH
#define SLIST_FOREACH_SAFE	BK_SLIST_FOREACH_SAFE
#define SLIST_INSERT_HEAD	BK_SLIST_INSERT_HEAD
#define SLIST_INSERT_AFTER	BK_SLIST_INSERT_AFTER
#define SLIST_REMOVE_HEAD	BK_SLIST_REMOVE_HEAD
#define SLIST_REMOVE		BK_SLIST_REMOVE

#define STAILQ_HEAD		BK_STAILQ_HEAD
#define STAILQ_HEAD_INITIALIZER	BK_STAILQ_HEAD_INITIALIZER
#define STAILQ_ENTRY		BK_STAILQ_ENTRY
#define STAILQ_INIT		BK_STAILQ_INIT
#define STAILQ_EMPTY		BK_STAILQ_EMPTY
#define STAILQ_FIRST		BK_STAILQ_FIRST
#define STAILQ_NEXT		BK_STAILQ_NEXT
#define STAILQ_LAST		BK_STAILQ_LAST
#define STAILQ_FOREACH		BK_STAILQ_FOREACH
#define STAILQ_FOREACH_SAFE	BK_STAILQ_FOREACH_SAFE
#define STAILQ_INSERT_HEAD	BK_STAILQ_INSERT_HEAD
#define STAILQ_INSERT_TAIL	BK_STAILQ_INSERT_TAIL
#define STAILQ_INSERT_AFTER	BK_STAILQ_INSERT_AFTER
#define STAILQ_REMOVE_HEAD	BK_STAILQ_REMOVE_HEAD
#define STAILQ_REMOVE		BK_STAILQ_REMOVE

#define LIST_HEAD		BK_LIST_HEAD
#define LIST_HEAD_INITIALIZER	BK_LIST_HEAD_INITIALIZER
#define LIST_ENTRY		BK_LIST_ENTRY
#define LIST_INIT		BK_LIST_INIT
#define LIST_EMPTY		BK_LIST_EMPTY
#define LIST_FIRST		BK_LIST_FIRST
#define LIST_NEXT		BK_LIST_NEXT
#define LIST_FOREACH		BK_LIST_FOREACH
#define LIST_FOREACH_SAFE	BK_LIST_FOREACH_SAFE
#define LIST_INSERT_HEAD	BK_LIST_INSERT_HEAD
#define LIST_INSERT_BEFORE	BK_LIST_INSERT_BEFORE
#define LIST_INSERT_AFTER	BK_LIST_INSERT_AFTER
#define LIST_REMOVE		BK_LIST_REMOVE

#define TAILQ_HEAD		BK_TAILQ_HEAD
#define TAILQ_HEAD_INITIALIZER	BK_TAILQ_HEAD_INITIALIZER
#define TAILQ_ENTRY		BK_TAILQ_ENTRY
#define TAILQ_INIT		BK_TAILQ_INIT
#define TAILQ_EMPTY		BK_TAILQ_EMPTY
#define TAILQ_FIRST		BK_TAILQ_FIRST
#define TAILQ_NEXT		BK_TAILQ_NEXT
#define TAILQ_LAST		BK_TAILQ_LAST
#define TAILQ_PREV		BK_TAILQ_PREV
#define TAILQ_FOREACH		BK_TAILQ_FOREACH
#define TAILQ_FOREACH_REVERSE	BK_TAILQ_FOREACH_REVERSE
#define TAILQ_FOREACH_SAFE	BK_TAILQ_FOREACH_SAFE
#define TAILQ_FOREACH_REVERSE_SAFE	BK_TAILQ_FOREACH_REVERSE_SAFE
#define TAILQ_INSERT_HEAD	BK_TAILQ_INSERT_HEAD
#define TAILQ_INSERT_TAIL	BK_TAILQ_INSERT_TAIL
#define TAILQ_INSERT_BEFORE	BK_TAILQ_INSERT_BEFORE
#define TAILQ_INSERT_AFTER	BK_TAILQ_INSERT_AFTER
#define TAILQ_REMOVE		BK_TAILQ_REMOVE
#define TAILQ_CONCAT		BK_TAILQ_CONCAT

#define CIRCLEQ_HEAD		BK_CIRCLEQ_HEAD
#define CIRCLEQ_HEAD_INITIALIZER	BK_CIRCLEQ_HEAD_INITIALIZER
#define CIRCLEQ_ENTRY		BK_CIRCLEQ_ENTRY
#define CIRCLEQ_INIT		BK_CIRCLEQ_INIT
#define CIRCLEQ_EMPTY		BK_CIRCLEQ_EMPTY
#define CIRCLEQ_FIRST		BK_CIRCLEQ_FIRST
#define CIRCLEQ_LAST		BK_CIRCLEQ_LAST
#define CIRCLEQ_NEXT		BK_CIRCLEQ_NEXT
#define CIRCLEQ_PREV		BK_CIRCLEQ_PREV
#define CIRCLEQ_FOREACH		BK_CIRCLEQ_FOREACH
#define CIRCLEQ_FOREACH_REVERSE	BK_CIRCLEQ_FOREACH_REVERSE
#define CIRCLEQ_INSERT_HEAD	BK_CIRCLEQ_INSERT_HEAD
#define CIRCLEQ_INSERT_TAIL	BK_CIRCLEQ_INSERT_TAIL
#define CIRCLEQ_INSERT_BEFORE	BK_CIRCLEQ_INSERT_BEFORE
#define CIRCLEQ_INSERT_AFTER	BK_CIRCLEQ_INSERT_AFTER
#define CIRCLEQ_REMOVE		BK_CIRCLEQ_REMOVE

#endif /* !_BK_NO_COMPAT */

#endif /* !_BIBLE_QUEUE_H_ */