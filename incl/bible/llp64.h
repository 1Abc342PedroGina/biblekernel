#ifndef _BIBLE_LLP64_H_
#define _BIBLE_LLP64_H_

/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2024 Bible System Developers
 *
 * llp64.h - Modelo de dados LLP64 (apenas definições base)
 * 
 * Filosofia LLP64:
 * - int: 32 bits
 * - long: 32 bits
 * - long long: 64 bits
 * - pointer: 64 bits
 */

#include <bible/cdefs.h>

/* ============================================================================
 * TIPOS FUNDAMENTAIS LLP64
 * ============================================================================ */

/* Tipos de 8 bits */
typedef signed char         __llp64_int8;
typedef unsigned char       __llp64_uint8;

/* Tipos de 16 bits */
typedef short               __llp64_int16;
typedef unsigned short      __llp64_uint16;

/* Tipos de 32 bits */
typedef int                 __llp64_int32;
typedef unsigned int        __llp64_uint32;

/* Tipos de 64 bits */
typedef long long           __llp64_int64;
typedef unsigned long long  __llp64_uint64;

/* Tipos para ponteiros (sempre 64 bits) */
typedef __llp64_uint64      __llp64_uintptr;
typedef __llp64_int64       __llp64_intptr;

/* Tipos de tamanho (baseados em ponteiro) */
typedef __llp64_uint64      __llp64_size;
typedef __llp64_int64       __llp64_ssize;
typedef __llp64_int64       __llp64_ptrdiff;

#endif /* !_BIBLE_LLP64_H_ */
