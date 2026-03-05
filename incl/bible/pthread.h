#ifndef _BIBLE__PTHREADTYPES_H_
#define _BIBLE__PTHREADTYPES_H_

#include <bible/cdefs.h>
#include <bible/types.h>

/* OS ARQUIVOS C DEFINE QUE
 * AS POSIX THREADS São apenas compatiblidade
 * Desde que atenda as syscall não-Unix
 */
struct __bk_pthread;
struct __bk_pthread_attr;
struct __bk_pthread_cond;
struct __bk_pthread_cond_attr;
struct __bk_pthread_mutex;
struct __bk_pthread_mutex_attr;
struct __bk_pthread_once;
struct __bk_pthread_rwlock;
struct __bk_pthread_rwlockattr;
struct __bk_pthread_barrier;
struct __bk_pthread_barrier_attr;
struct __bk_pthread_spinlock;

/*
 * Definições de tipos de dados de sistema primitivos requeridos por P1003.1c.
 *
 * Note que P1003.1c especifica que não há operadores de comparação
 * ou atribuição definidos para os tipos pthread_attr_t, pthread_cond_t,
 * pthread_condattr_t, pthread_mutex_t, pthread_mutexattr_t.
 */
#ifndef _BK_PTHREAD_T_DECLARED
typedef struct	__bk_pthread			*__bk_pthread_t;
#define	_BK_PTHREAD_T_DECLARED
#endif

typedef struct	__bk_pthread_attr		*__bk_pthread_attr_t;
typedef struct	__bk_pthread_mutex		*__bk_pthread_mutex_t;
typedef struct	__bk_pthread_mutex_attr	*__bk_pthread_mutexattr_t;
typedef struct	__bk_pthread_cond		*__bk_pthread_cond_t;
typedef struct	__bk_pthread_cond_attr		*__bk_pthread_condattr_t;
typedef __BK_I32				__bk_pthread_key_t;
typedef struct	__bk_pthread_once		__bk_pthread_once_t;
typedef struct	__bk_pthread_rwlock		*__bk_pthread_rwlock_t;
typedef struct	__bk_pthread_rwlockattr	*__bk_pthread_rwlockattr_t;
typedef struct	__bk_pthread_barrier		*__bk_pthread_barrier_t;
typedef struct	__bk_pthread_barrierattr	*__bk_pthread_barrierattr_t;
typedef struct	__bk_pthread_spinlock		*__bk_pthread_spinlock_t;

/*
 * Definições de tipo adicionais:
 *
 * Note que P1003.1c reserva os prefixos pthread_ e PTHREAD_ para
 * uso em símbolos de cabeçalho.
 */
typedef void	*__bk_pthread_addr_t;
typedef void	*(*__bk_pthread_startroutine_t)(void *);

/*
 * Definições de once (execução única).
 */
struct __bk_pthread_once {
	__BK_I32		state;
	__bk_pthread_mutex_t	mutex;
};

/* Compatibilidade POSIX (opcional) */
#ifdef _BK_POSIX_COMPAT
typedef __bk_pthread_t			pthread_t;
typedef __bk_pthread_attr_t		pthread_attr_t;
typedef __bk_pthread_mutex_t		pthread_mutex_t;
typedef __bk_pthread_mutexattr_t	pthread_mutexattr_t;
typedef __bk_pthread_cond_t		pthread_cond_t;
typedef __bk_pthread_condattr_t	pthread_condattr_t;
typedef __bk_pthread_key_t		pthread_key_t;
typedef __bk_pthread_once_t		pthread_once_t;
typedef __bk_pthread_rwlock_t		pthread_rwlock_t;
typedef __bk_pthread_rwlockattr_t	pthread_rwlockattr_t;
typedef __bk_pthread_barrier_t		pthread_barrier_t;
typedef __bk_pthread_barrierattr_t	pthread_barrierattr_t;
typedef __bk_pthread_spinlock_t	pthread_spinlock_t;
typedef __bk_pthread_addr_t		pthread_addr_t;
typedef __bk_pthread_startroutine_t	pthread_startroutine_t;
#endif /* _BK_POSIX_COMPAT */

#endif /* ! _BIBLE__PTHREADTYPES_H_ */