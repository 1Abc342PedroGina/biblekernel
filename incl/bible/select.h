#ifndef _BIBLE_SELECT_H_
#define _BIBLE_SELECT_H_

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/ktypes.h>
#include <bible/task.h>


/*
 * SPDX-License-Identifier: MIT License
 *
 * Copyright (c) 2024 Bible System Developers
 * Todos os direitos reservados.
 *
 * Definições para operações de seleção de I/O
 */

/*
 * O tipo fd_set representa um conjunto de descritores de arquivo.
 * Implementado como um array de bits.
 */
#ifndef _BK_FD_SETSIZE
#define _BK_FD_SETSIZE	1024	/* Número máximo de descritores */
#endif

/* Número de palavras necessárias para representar _BK_FD_SETSIZE bits */
#define _BK_NFDBITS		(sizeof(BK_UL3264) * 8)
#define _BK_HOWMANY(x, y)	(((x) + ((y) - 1)) / (y))
#define _BK_FDSETWORDS	_BK_HOWMANY(_BK_FD_SETSIZE, _BK_NFDBITS)

typedef struct __bk_fd_set {
	BK_UL3264	fds_bits[_BK_FDSETWORDS];
} __bk_fd_set;

typedef __bk_fd_set BK_FD_SET;

/*
 * Macros para manipulação de fd_set
 */
#define BK_FD_ZERO(set) do { \
	BK_SIZE __i; \
	for (__i = 0; __i < _BK_FDSETWORDS; __i++) \
		(set)->fds_bits[__i] = 0; \
} while (0)

#define BK_FD_SET(fd, set) \
	((set)->fds_bits[(fd) / _BK_NFDBITS] |= (1ULL << ((fd) % _BK_NFDBITS)))

#define BK_FD_CLR(fd, set) \
	((set)->fds_bits[(fd) / _BK_NFDBITS] &= ~(1ULL << ((fd) % _BK_NFDBITS)))

#define BK_FD_ISSET(fd, set) \
	(((set)->fds_bits[(fd) / _BK_NFDBITS] & (1ULL << ((fd) % _BK_NFDBITS))) != 0)

/*
 * Estrutura para timeout em operações select/poll
 */
struct __bk_timeval {
	BK_TIME		tv_sec;		/* segundos */
	BK_SUSECONDS	tv_usec;	/* microssegundos */
};

typedef struct __bk_timeval BK_TIMEVAL;

struct __bk_timespec {
	BK_TIME		tv_sec;		/* segundos */
	BK_L3264	tv_nsec;	/* nanosegundos */
};

typedef struct __bk_timespec BK_TIMESPEC;

/*
 * Valores de retorno para funções de seleção
 */
#define BK_SELECT_READ		0x01	/* Descritor pronto para leitura */
#define BK_SELECT_WRITE		0x02	/* Descritor pronto para escrita */
#define BK_SELECT_ERROR		0x04	/* Condição de erro no descritor */
#define BK_SELECT_HUP		0x08	/* Hang-up no descritor */
#define BK_SELECT_INVALID	0x10	/* Descritor inválido */

/*
 * Protótipos das funções principais
 */
#ifndef _BK_KERNEL

__BK_BEGIN_DECLS

/*
 * bk_select() - Monitora múltiplos descritores de arquivo
 * 
 * nfds: Maior descritor + 1
 * readfds: Conjunto de descritores para leitura
 * writefds: Conjunto de descritores para escrita
 * exceptfds: Conjunto de descritores para exceções
 * timeout: Timeout ou NULL para bloqueio infinito
 * 
 * Retorna: Número de descritores prontos, 0 no timeout, -1 em erro
 */
BK_I32 bk_select(BK_I32 nfds, BK_FD_SET *readfds, BK_FD_SET *writefds,
		 BK_FD_SET *exceptfds, BK_TIMEVAL *timeout);

/*
 * bk_pselect() - select() com mask de sinais
 */
BK_I32 bk_pselect(BK_I32 nfds, BK_FD_SET *readfds, BK_FD_SET *writefds,
		  BK_FD_SET *exceptfds, const BK_TIMESPEC *timeout,
		  const BK_SIGSET *sigmask);

/*
 * bk_poll() - Poll de descritores de arquivo
 * 
 * fds: Array de estruturas pollfd
 * nfds: Número de estruturas no array
 * timeout: Timeout em milissegundos (-1 para bloqueio infinito)
 * 
 * Retorna: Número de descritores com eventos, 0 no timeout, -1 em erro
 */
BK_I32 bk_poll(struct __bk_pollfd *fds, BK_UI32 nfds, BK_I32 timeout);

__BK_END_DECLS

#endif /* !_BK_KERNEL */

/*
 * Estrutura para poll()
 */
struct __bk_pollfd {
	BK_I32	fd;		/* Descritor de arquivo */
	__BK_S16	events;		/* Eventos solicitados */
	__BK_S16	revents;	/* Eventos ocorridos */
};

typedef struct __bk_pollfd BK_POLLFD;

/* Eventos para poll() */
#define BK_POLLIN		0x001	/* Dados disponíveis para leitura */
#define BK_POLLPRI		0x002	/* Dados urgentes disponíveis */
#define BK_POLLOUT		0x004	/* Escrita possível sem bloqueio */
#define BK_POLLRDNORM		0x040	/* Dados normais disponíveis */
#define BK_POLLRDBAND		0x080	/* Dados de prioridade disponíveis */
#define BK_POLLWRNORM		0x100	/* Escrita de dados normais possível */
#define BK_POLLWRBAND		0x200	/* Escrita de dados de prioridade possível */
#define BK_POLLERR		0x008	/* Condição de erro */
#define BK_POLLHUP		0x010	/* Hang-up */
#define BK_POLLNVAL		0x020	/* Descritor inválido */

/*
 * Macros úteis para manipulação de fd_set com arrays de bits
 */
static __BK_ALWAYS_INLINE void
BK_FD_COPY(const BK_FD_SET *src, BK_FD_SET *dst)
{
	do { \
    BK_SIZE __i; \
    for (__i = 0; __i < _BK_FDSETWORDS; __i++) \
        (dst)->fds_bits[__i] = (src)->fds_bits[__i]; \
} while (0);

}

static __BK_ALWAYS_INLINE BK_BOOLEAN
BK_FD_EQUAL(const BK_FD_SET *a, const BK_FD_SET *b)
{
	for (BK_UI32 i = 0; i < _BK_FDSETWORDS; i++) {
		if (a->fds_bits[i] != b->fds_bits[i])
			return BK_FALSE;
	}
	return BK_TRUE;
}

static __BK_ALWAYS_INLINE void
BK_FD_OR(BK_FD_SET *dst, const BK_FD_SET *src)
{
	for (BK_UI32 i = 0; i < _BK_FDSETWORDS; i++) {
		dst->fds_bits[i] |= src->fds_bits[i];
	}
}

static __BK_ALWAYS_INLINE void
BK_FD_AND(BK_FD_SET *dst, const BK_FD_SET *src)
{
	for (BK_UI32 i = 0; i < _BK_FDSETWORDS; i++) {
		dst->fds_bits[i] &= src->fds_bits[i];
	}
}

static __BK_ALWAYS_INLINE void
BK_FD_XOR(BK_FD_SET *dst, const BK_FD_SET *src)
{
	for (BK_UI32 i = 0; i < _BK_FDSETWORDS; i++) {
		dst->fds_bits[i] ^= src->fds_bits[i];
	}
}

/*
 * Função para encontrar o próximo fd definido no conjunto
 * Começa a busca a partir de 'start' (inclusive)
 * Retorna -1 se não encontrar mais fds
 */
static __BK_ALWAYS_INLINE BK_I32
BK_FD_NEXT(const BK_FD_SET *set, BK_I32 start)
{
	BK_I32 word = start / _BK_NFDBITS;
	BK_I32 bit = start % _BK_NFDBITS;
	
	if (word >= _BK_FDSETWORDS)
		return -1;
	
	/* Máscara para bits a partir de 'start' */
	BK_UL3264 mask = ~((1ULL << bit) - 1);
	
	/* Verifica a palavra atual */
	BK_UL3264 word_bits = set->fds_bits[word] & mask;
	if (word_bits != 0) {
		return word * _BK_NFDBITS + bk_ctz64(word_bits);
	}
	
	/* Verifica palavras seguintes */
	for (word++; word < _BK_FDSETWORDS; word++) {
		if (set->fds_bits[word] != 0) {
			return word * _BK_NFDBITS + bk_ctz64(set->fds_bits[word]);
		}
	}
	
	return -1;
}

/*
 * Função para contar o número de fds definidos no conjunto
 */
static __BK_ALWAYS_INLINE BK_UI32
BK_FD_COUNT(const BK_FD_SET *set)
{
	BK_UI32 count = 0;
	for (BK_UI32 i = 0; i < _BK_FDSETWORDS; i++) {
		count += bk_bitcount64(set->fds_bits[i]);
	}
	return count;
}

/*
 * Compatibilidade com APIs POSIX/BSD
 */
#ifndef _BK_NO_COMPAT

#define	fd_set			BK_FD_SET
#define	FD_ZERO			BK_FD_ZERO
#define	FD_SET			BK_FD_SET
#define	FD_CLR			BK_FD_CLR
#define	FD_ISSET		BK_FD_ISSET

#define	timeval			BK_TIMEVAL
#define	timespec		BK_TIMESPEC

#define	pollfd			BK_POLLFD
#define	POLLIN			BK_POLLIN
#define	POLLPRI			BK_POLLPRI
#define	POLLOUT			BK_POLLOUT
#define	POLLERR			BK_POLLERR
#define	POLLHUP			BK_POLLHUP
#define	POLLNVAL		BK_POLLNVAL

#define	select			bk_select
#define	pselect			bk_pselect
#define	poll			bk_poll

#endif /* !_BK_NO_COMPAT */

#endif /* !_BIBLE_SELECT_H_ */
