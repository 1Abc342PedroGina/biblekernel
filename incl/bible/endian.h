#ifndef _BIBLE_ENDIAN_H_
#define _BIBLE_ENDIAN_H_

#include <bible/cdefs.h>
#include <bible/types.h>

/*
 * SPDX-License-Identifier: MIT License
 *
 * Copyright (c) 2024 Bible System Developers
 * Todos os direitos reservados.
 *
 * Definições de ordem de bytes para o sistema Bible
 */

/*
 * Define a ordem de palavras de 32-bit em palavras de 64-bit.
 */
#define	_BK_QUAD_HIGHWORD 1
#define	_BK_QUAD_LOWWORD  0

/*
 * Definições para ordem de bytes, de acordo com a significância do byte
 * do endereço baixo para o alto.
 */
#define	BK_LITTLE_ENDIAN	1234	/* LSB primeiro: i386, vax, x86_64 */
#define	BK_BIG_ENDIAN		4321	/* MSB primeiro: 68000, ibm, net, powerpc */
#define	BK_PDP_ENDIAN		3412	/* LSB primeiro na palavra, MSW primeiro no long */

/*
 * Ordem de bytes padrão do sistema Bible
 * Por padrão assume little-endian (arquiteturas x86/ARM modernas)
 */
#ifndef BK_BYTE_ORDER
#ifdef __BIG_ENDIAN__
#define	BK_BYTE_ORDER	BK_BIG_ENDIAN
#elif defined(__LITTLE_ENDIAN__)
#define	BK_BYTE_ORDER	BK_LITTLE_ENDIAN
#elif defined(__PDP_ENDIAN__)
#define	BK_BYTE_ORDER	BK_PDP_ENDIAN
#else
#define	BK_BYTE_ORDER	BK_LITTLE_ENDIAN	/* Padrão moderno */
#endif
#endif

/*
 * Definições compatíveis com POSIX
 */
#ifndef _BYTE_ORDER
#define	_BYTE_ORDER	BK_BYTE_ORDER
#define	_LITTLE_ENDIAN	BK_LITTLE_ENDIAN
#define	_BIG_ENDIAN	BK_BIG_ENDIAN
#define	_PDP_ENDIAN	BK_PDP_ENDIAN
#endif

/*
 * Macros de troca de bytes otimizadas para Bible
 */
#ifdef __BK_ASM_SUPPORT__

/* Troca de palavras (16-bit) */
#define __bk_byte_swap_word(x) \
({ \
	register BK_UI16 X = (x); \
	__asm__ volatile ("xchgb %%al, %%ah" \
		: "=a" (X) \
		: "0" (X)); \
	X; \
})

/* Troca de longs (32-bit) */
#define __bk_byte_swap_long(x) \
({ \
	register BK_UI32 X = (x); \
	__asm__ volatile ("bswap %0" \
		: "=r" (X) \
		: "0" (X)); \
	X; \
})

/* Troca de quad words (64-bit) */
#define __bk_byte_swap_quad(x) \
({ \
	register BK_UL3264 X = (x); \
	__asm__ volatile ("bswap %0" \
		: "=r" (X) \
		: "0" (X)); \
	__asm__ volatile ("ror $32, %0" : "+r" (X)); \
	X; \
})

#else /* !__BK_ASM_SUPPORT__ */

/* Implementações em C puro */
static __BK_ALWAYS_INLINE BK_UI16 __bk_byte_swap_word(BK_UI16 x) {
	return (x << 8) | (x >> 8);
}

static __BK_ALWAYS_INLINE BK_UI32 __bk_byte_swap_long(BK_UI32 x) {
	return ((x & 0xFF000000) >> 24) |
	       ((x & 0x00FF0000) >> 8) |
	       ((x & 0x0000FF00) << 8) |
	       ((x & 0x000000FF) << 24);
}

static __BK_ALWAYS_INLINE BK_UL3264 __bk_byte_swap_quad(BK_UL3264 x) {
	return ((x & 0xFF00000000000000ULL) >> 56) |
	       ((x & 0x00FF000000000000ULL) >> 40) |
	       ((x & 0x0000FF0000000000ULL) >> 24) |
	       ((x & 0x000000FF00000000ULL) >> 8) |
	       ((x & 0x00000000FF000000ULL) << 8) |
	       ((x & 0x0000000000FF0000ULL) << 24) |
	       ((x & 0x000000000000FF00ULL) << 40) |
	       ((x & 0x00000000000000FFULL) << 56);
}

#endif /* __BK_ASM_SUPPORT__ */

/*
 * Macros para conversão de representação de números de rede/forma externa.
 */
#if BK_BYTE_ORDER == BK_BIG_ENDIAN

/* Sistema big-endian nativo - sem conversão necessária */
#define	bk_ntohl(x)	((BK_UI32)(x))
#define	bk_ntohs(x)	((BK_UI16)(x))
#define	bk_htonl(x)	((BK_UI32)(x))
#define	bk_htons(x)	((BK_UI16)(x))

#define	bk_ntohll(x)	((BK_UL3264)(x))
#define	bk_htonll(x)	((BK_UL3264)(x))

#define	BK_NTOHL(x)	((void)0)	/* NOP para big-endian */
#define	BK_NTOHS(x)	((void)0)
#define	BK_HTONL(x)	((void)0)
#define	BK_HTONS(x)	((void)0)

#else /* Little-endian ou PDP-endian */

/* Sistema little-endian - precisa de conversão */
#define	bk_ntohl(x)	__bk_byte_swap_long((BK_UI32)(x))
#define	bk_ntohs(x)	__bk_byte_swap_word((BK_UI16)(x))
#define	bk_htonl(x)	__bk_byte_swap_long((BK_UI32)(x))
#define	bk_htons(x)	__bk_byte_swap_word((BK_UI16)(x))

#define	bk_ntohll(x)	__bk_byte_swap_quad((BK_UL3264)(x))
#define	bk_htonll(x)	__bk_byte_swap_quad((BK_UL3264)(x))

/* Macros que modificam a variável in-place */
#define	BK_NTOHL(x)	do { (x) = bk_ntohl(x); } while(0)
#define	BK_NTOHS(x)	do { (x) = bk_ntohs(x); } while(0)
#define	BK_HTONL(x)	do { (x) = bk_htonl(x); } while(0)
#define	BK_HTONS(x)	do { (x) = bk_htons(x); } while(0)

#endif /* BK_BYTE_ORDER */

/*
 * Funções de leitura/escrita independentes de endianness
 */

/* Leitura little-endian */
static __BK_ALWAYS_INLINE BK_UI16 bk_le16toh(const void *p) {
	const BK_UI8 *bytes = (const BK_UI8 *)p;
	return bytes[0] | (bytes[1] << 8);
}

static __BK_ALWAYS_INLINE BK_UI32 bk_le32toh(const void *p) {
	const BK_UI8 *bytes = (const BK_UI8 *)p;
	return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

/* Leitura big-endian */
static __BK_ALWAYS_INLINE BK_UI16 bk_be16toh(const void *p) {
	const BK_UI8 *bytes = (const BK_UI8 *)p;
	return (bytes[0] << 8) | bytes[1];
}

static __BK_ALWAYS_INLINE BK_UI32 bk_be32toh(const void *p) {
	const BK_UI8 *bytes = (const BK_UI8 *)p;
	return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

/* Escrita little-endian */
static __BK_ALWAYS_INLINE void bk_htole16(void *p, BK_UI16 value) {
	BK_UI8 *bytes = (BK_UI8 *)p;
	bytes[0] = value & 0xFF;
	bytes[1] = (value >> 8) & 0xFF;
}

static __BK_ALWAYS_INLINE void bk_htole32(void *p, BK_UI32 value) {
	BK_UI8 *bytes = (BK_UI8 *)p;
	bytes[0] = value & 0xFF;
	bytes[1] = (value >> 8) & 0xFF;
	bytes[2] = (value >> 16) & 0xFF;
	bytes[3] = (value >> 24) & 0xFF;
}

/* Escrita big-endian */
static __BK_ALWAYS_INLINE void bk_htobe16(void *p, BK_UI16 value) {
	BK_UI8 *bytes = (BK_UI8 *)p;
	bytes[0] = (value >> 8) & 0xFF;
	bytes[1] = value & 0xFF;
}

static __BK_ALWAYS_INLINE void bk_htobe32(void *p, BK_UI32 value) {
	BK_UI8 *bytes = (BK_UI8 *)p;
	bytes[0] = (value >> 24) & 0xFF;
	bytes[1] = (value >> 16) & 0xFF;
	bytes[2] = (value >> 8) & 0xFF;
	bytes[3] = value & 0xFF;
}

/*
 * Compatibilidade com APIs POSIX/BSD
 */
#ifndef _BK_NO_COMPAT

#define	ntohl		bk_ntohl
#define	ntohs		bk_ntohs
#define	htonl		bk_htonl
#define	htons		bk_htons

#define	NTOHL(x)	BK_NTOHL(x)
#define	NTOHS(x)	BK_NTOHS(x)
#define	HTONL(x)	BK_HTONL(x)
#define	HTONS(x)	BK_HTONS(x)

#define	LITTLE_ENDIAN	BK_LITTLE_ENDIAN
#define	BIG_ENDIAN	BK_BIG_ENDIAN
#define	PDP_ENDIAN	BK_PDP_ENDIAN
#define	BYTE_ORDER	BK_BYTE_ORDER

#endif /* !_BK_NO_COMPAT */

/*
 * Macros para detecção de endianness em tempo de compilação
 */
#define BK_IS_LITTLE_ENDIAN	(BK_BYTE_ORDER == BK_LITTLE_ENDIAN)
#define BK_IS_BIG_ENDIAN	(BK_BYTE_ORDER == BK_BIG_ENDIAN)
#define BK_IS_PDP_ENDIAN	(BK_BYTE_ORDER == BK_PDP_ENDIAN)

/*
 * Funções para troca de endianness genérica
 */
static __BK_ALWAYS_INLINE BK_UI64 bk_bswap64(BK_UI64 x) {
	return __bk_byte_swap_quad(x);
}

static __BK_ALWAYS_INLINE BK_UI32 bk_bswap32(BK_UI32 x) {
	return __bk_byte_swap_long(x);
}

static __BK_ALWAYS_INLINE BK_UI16 bk_bswap16(BK_UI16 x) {
	return __bk_byte_swap_word(x);
}


#endif /* !_BIBLE_ENDIAN_H_ */
