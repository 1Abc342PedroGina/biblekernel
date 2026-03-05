#ifndef _BIBLE__STDINT_H_
#define _BIBLE__STDINT_H_

#include <bible/types.h>


/* Tipos inteiros com sinal */
#ifndef _BK_INT8_T_DECLARED
typedef	__BK_C8		BK_C8;
#define	_BK_INT8_T_DECLARED
#endif

#ifndef _BK_INT16_T_DECLARED
typedef	__BK_S16	BK_S16;
#define	_BK_INT16_T_DECLARED
#endif

#ifndef _BK_INT32_T_DECLARED
typedef	__BK_I32	BK_I32;
#define	_BK_INT32_T_DECLARED
#endif

#ifndef _BK_INT64_T_DECLARED
typedef	__BK_L3264	BK_L3264;
#define	_BK_INT64_T_DECLARED
#endif

#ifndef _BK_INTMAX_T_DECLARED
typedef	__BK_L3264	BK_INTMAX;
#define	_BK_INTMAX_T_DECLARED
#endif

/* Tipos inteiros sem sinal */
#ifndef _BK_UINT8_T_DECLARED
typedef	__BK_UI8	BK_UI8;
#define	_BK_UINT8_T_DECLARED
#endif

#ifndef _BK_UINT16_T_DECLARED
typedef	__BK_UI16	BK_UI16;
#define	_BK_UINT16_T_DECLARED
#endif

#ifndef _BK_UINT32_T_DECLARED
typedef	__BK_UI32	BK_UI32;
#define	_BK_UINT32_T_DECLARED
#endif

#ifndef _BK_UINT64_T_DECLARED
typedef	__BK_UL3264	BK_UL3264;
#define	_BK_UINT64_T_DECLARED
#endif

#ifndef _BK_UINTMAX_T_DECLARED
typedef	__BK_ULL64	BK_UINTMAX64;
#define	_BK_UINTMAX_T_DECLARED
#endif

/* Tipos de ponteiro */
#ifndef _BK_INTPTR_T_DECLARED
typedef	__intptr_kt	BK_INTPTR;
#define	_BK_INTPTR_T_DECLARED
#endif

#ifndef _BK_UINTPTR_T_DECLARED
typedef	__uintptr_kt	BK_UINTPTR;
#define	_BK_UINTPTR_T_DECLARED
#endif

/* Tipos de ponteiro 64-bit fixos */
#ifndef _BK_INT64PTR_T_DECLARED
typedef	__int64ptr_kt	BK_INT64PTR;
#define	_BK_INT64PTR_T_DECLARED
#endif

#ifndef _BK_UINT64PTR_T_DECLARED
typedef	__uint64ptr_kt	BK_UINT64PTR;
#define	_BK_UINT64PTR_T_DECLARED
#endif

/* Compatibilidade com stdint.h padrão */
#ifndef _BK_STDINT_COMPAT

/* Tipos inteiros com sinal - padrão C */
#ifndef _INT8_T_DECLARED
typedef	BK_C8		int8_t;
#define	_INT8_T_DECLARED
#endif

#ifndef _INT16_T_DECLARED
typedef	BK_S16		int16_t;
#define	_INT16_T_DECLARED
#endif

#ifndef _INT32_T_DECLARED
typedef	BK_I32		int32_t;
#define	_INT32_T_DECLARED
#endif

#ifndef _INT64_T_DECLARED
typedef	BK_L3264	int64_t;
#define	_INT64_T_DECLARED
#endif

/* Tipos inteiros sem sinal - padrão C */
#ifndef _UINT8_T_DECLARED
typedef	BK_UI8		uint8_t;
#define	_UINT8_T_DECLARED
#endif

#ifndef _UINT16_T_DECLARED
typedef	BK_UI16		uint16_t;
#define	_UINT16_T_DECLARED
#endif

#ifndef _UINT32_T_DECLARED
typedef	BK_UI32		uint32_t;
#define	_UINT32_T_DECLARED
#endif

#ifndef _UINT64_T_DECLARED
typedef	BK_UL3264	uint64_t;
#define	_UINT64_T_DECLARED
#endif

/* Tipos de ponteiro - padrão C */
#ifndef _INTPTR_T_DECLARED
typedef	BK_INTPTR	intptr_t;
#define	_INTPTR_T_DECLARED
#endif

#ifndef _UINTPTR_T_DECLARED
typedef	BK_UINTPTR	uintptr_t;
#define	_UINTPTR_T_DECLARED
#endif

/* Tipos máximo - padrão C */
#ifndef _INTMAX_T_DECLARED
typedef	BK_INTMAX	intmax_t;
#define	_INTMAX_T_DECLARED
#endif

#ifndef _UINTMAX_T_DECLARED
typedef	BK_UINTMAX64	uintmax_t;
#define	_UINTMAX_T_DECLARED
#endif

#endif /* !_BK_STDINT_COMPAT */

#endif /* !_BIBLE__STDINT_H_ */