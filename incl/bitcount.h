#ifndef _BIBLE_BITCOUNT_H_
#define _BIBLE_BITCOUNT_H_

#include <bible/cdefs.h>
#include <bible/types.h>



#ifdef __BK_HAS_POPCNT__

/* Usa instruções POPCNT do processador */
static __BK_ALWAYS_INLINE BK_UI32
bk_bitcount32(BK_UI32 x)
{
	BK_UI32 count;
	__asm__ volatile ("popcnt %1, %0" : "=r" (count) : "r" (x));
	return count;
}

static __BK_ALWAYS_INLINE BK_UI64
bk_bitcount64(BK_UL3264 x)
{
	BK_UL3264 count;
	__asm__ volatile ("popcnt %1, %0" : "=r" (count) : "r" (x));
	return count;
}

#else /* !__BK_HAS_POPCNT__ */

/*
 * Implementação de contagem de bits usando SWAR (SIMD Within A Register)
 * Algoritmo de Hamming weight otimizado
 */
static __BK_ALWAYS_INLINE BK_UI32
bk_bitcount32(BK_UI32 x)
{
	/* Método SWAR - contagem paralela de bits */
	x = x - ((x >> 1) & 0x55555555);
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	x = (x + (x >> 4)) & 0x0F0F0F0F;
	x = (x * 0x01010101) >> 24;
	return x;
}

static __BK_ALWAYS_INLINE BK_UI64
bk_bitcount64(BK_UL3264 x)
{
	/* Método SWAR para 64 bits */
	x = x - ((x >> 1) & 0x5555555555555555ULL);
	x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
	x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
	x = (x * 0x0101010101010101ULL) >> 56;
	return x;
}

#endif /* __BK_HAS_POPCNT__ */

/* Versão genérica para qualquer tipo inteiro */
#define BK_BITCOUNT(x) _Generic((x), \
	BK_UI8: bk_bitcount32((BK_UI32)(x)), \
	BK_UI16: bk_bitcount32((BK_UI32)(x)), \
	BK_UI32: bk_bitcount32(x), \
	BK_UL3264: bk_bitcount64(x), \
	default: bk_bitcount32((BK_UI32)(x)) \
)

/*
 * Contagem de zeros à esquerda (clz) e à direita (ctz)
 */

/* Contagem de zeros à esquerda (Count Leading Zeros) */
static __BK_ALWAYS_INLINE BK_UI32
bk_clz32(BK_UI32 x)
{
	if (x == 0) return 32;
	
#ifdef __BK_HAS_BMI__
	BK_UI32 result;
	__asm__ volatile ("lzcnt %1, %0" : "=r" (result) : "r" (x));
	return result;
#else
	/* De Bruijn sequence method */
	static const BK_UI8 debruijn32[32] = {
		31, 22, 30, 21, 18, 10, 29,  2, 20, 17, 15, 13, 9,  6, 28, 1,
		23, 26, 19, 16, 14, 12,  8,  5, 25, 0, 24, 27,  3, 11,  4, 7
	};
	
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	
	return debruijn32[x * 0x076BE629 >> 27];
#endif
}

static __BK_ALWAYS_INLINE BK_UI32
bk_clz64(BK_UL3264 x)
{
	if (x == 0) return 64;
	
#ifdef __BK_HAS_BMI__
	BK_UL3264 result;
	__asm__ volatile ("lzcnt %1, %0" : "=r" (result) : "r" (x));
	return (BK_UI32)result;
#else
	BK_UI32 hi = (BK_UI32)(x >> 32);
	if (hi != 0) {
		return bk_clz32(hi);
	} else {
		return 32 + bk_clz32((BK_UI32)x);
	}
#endif
}

/* Contagem de zeros à direita (Count Trailing Zeros) */
static __BK_ALWAYS_INLINE BK_UI32
bk_ctz32(BK_UI32 x)
{
	if (x == 0) return 32;
	
#ifdef __BK_HAS_BMI__
	BK_UI32 result;
	__asm__ volatile ("tzcnt %1, %0" : "=r" (result) : "r" (x));
	return result;
#else
	/* De Bruijn sequence method */
	static const BK_UI8 debruijn32[32] = {
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};
	
	return debruijn32[((x & -x) * 0x077CB531U) >> 27];
#endif
}

static __BK_ALWAYS_INLINE BK_UI32
bk_ctz64(BK_UL3264 x)
{
	if (x == 0) return 64;
	
#ifdef __BK_HAS_BMI__
	BK_UL3264 result;
	__asm__ volatile ("tzcnt %1, %0" : "=r" (result) : "r" (x));
	return (BK_UI32)result;
#else
	BK_UI32 lo = (BK_UI32)x;
	if (lo != 0) {
		return bk_ctz32(lo);
	} else {
		return 32 + bk_ctz32((BK_UI32)(x >> 32));
	}
#endif
}

/*
 * Encontrar primeiro bit definido (ffs - find first set)
 * Retorna posição baseada em 1 (0 se nenhum bit definido)
 */
static __BK_ALWAYS_INLINE BK_I32
bk_ffs32(BK_UI32 x)
{
	if (x == 0) return 0;
	return bk_ctz32(x) + 1;
}

static __BK_ALWAYS_INLINE BK_I32
bk_ffs64(BK_UL3264 x)
{
	if (x == 0) return 0;
	return bk_ctz64(x) + 1;
}

/*
 * Macros úteis para manipulação de bits
 */
#define BK_BIT(n)			(1UL << (n))
#define BK_BITMASK(n)		(BK_BIT(n) - 1)
#define BK_TESTBIT(val, bit)	(((val) >> (bit)) & 1)
#define BK_SETBIT(val, bit)	((val) |= BK_BIT(bit))
#define BK_CLEARBIT(val, bit)	((val) &= ~BK_BIT(bit))
#define BK_TOGGLEBIT(val, bit)	((val) ^= BK_BIT(bit))

/* Extrair faixa de bits */
#define BK_EXTRACTBITS(val, start, length) \
	(((val) >> (start)) & BK_BITMASK(length))

/* Inserir faixa de bits */
#define BK_INSERTBITS(val, bits, start, length) \
	((val) = ((val) & ~(BK_BITMASK(length) << (start))) | \
		(((bits) & BK_BITMASK(length)) << (start)))

/*
 * Funções para rotação de bits
 */
static __BK_ALWAYS_INLINE BK_UI32
bk_rotl32(BK_UI32 x, BK_UI32 shift)
{
	shift &= 31;
	return (x << shift) | (x >> (32 - shift));
}

static __BK_ALWAYS_INLINE BK_UI32
bk_rotr32(BK_UI32 x, BK_UI32 shift)
{
	shift &= 31;
	return (x >> shift) | (x << (32 - shift));
}

static __BK_ALWAYS_INLINE BK_UL3264
bk_rotl64(BK_UL3264 x, BK_UI32 shift)
{
	shift &= 63;
	return (x << shift) | (x >> (64 - shift));
}

static __BK_ALWAYS_INLINE BK_UL3264
bk_rotr64(BK_UL3264 x, BK_UI32 shift)
{
	shift &= 63;
	return (x >> shift) | (x << (64 - shift));
}

/*
 * Compatibilidade com APIs POSIX
 */
#ifndef _BK_NO_COMPAT

#define	kern_bitcount32		bk_bitcount32
#define	kern_bitcount64		bk_bitcount64
#define	kern_clz32			bk_clz32
#define	kern_clz64			bk_clz64
#define	kern_ctz32			bk_ctz32
#define	kern_ctz64			bk_ctz64
#define	posix_ffs32			bk_ffs32
#define	posix_ffs64			bk_ffs64
#define	posix_rotl32			bk_rotl32
#define	posix_rotr32			bk_rotr32
#define	posix_rotl64			bk_rotl64
#define	posix_rotr64			bk_rotr64

#endif /* !_BK_NO_COMPAT */

#endif /* !_BIBLE_BITCOUNT_H_ */