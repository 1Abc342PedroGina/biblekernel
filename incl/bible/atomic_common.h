#ifndef _BIBLE_ATOMIC_COMMON_H_
#define _BIBLE_ATOMIC_COMMON_H_

/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2024 Bible System Developers
 *
 * atomic_common.h - Operações atômicas para kernel mode (Ring 0)
 * 
 * Este cabeçalho fornece operações atômicas portáteis usando built-ins
 * do compilador GCC/Clang, adequadas para uso no kernel sem dependências
 * de userland.
 */

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/ktypes.h>

__BK_BEGIN_DECLS

/* ============================================================================
 * TIPOS ATÔMICOS
 * ============================================================================ */

typedef volatile struct {
    BK_I32 counter;
} bk_atomic_t;

typedef volatile struct {
    BK_UI32 counter;
} bk_atomic32_t;

typedef volatile struct {
    BK_UI64 counter;
} bk_atomic64_t;

typedef volatile struct {
    BK_L3264 counter;
} bk_atomic_long_t;

/* ============================================================================
 * BARREIRAS DE MEMÓRIA
 * ============================================================================ */

/**
 * bk_mb() - Barreira de memória completa
 * Garante que todas as operações de memória antes da barreira
 * sejam completadas antes das operações depois da barreira.
 */
static __BK_ALWAYS_INLINE void
bk_mb(void)
{
    __sync_synchronize();
}

/**
 * bk_rmb() - Barreira de leitura
 * Garante que todas as leituras antes da barreira sejam completadas
 * antes de qualquer leitura depois da barreira.
 */
static __BK_ALWAYS_INLINE void
bk_rmb(void)
{
    __sync_synchronize();
}

/**
 * bk_wmb() - Barreira de escrita
 * Garante que todas as escritas antes da barreira sejam completadas
 * antes de qualquer escrita depois da barreira.
 */
static __BK_ALWAYS_INLINE void
bk_wmb(void)
{
    __sync_synchronize();
}

/**
 * bk_barrier() - Barreira de otimização do compilador
 * Impede o compilador de reordenar operações de memória.
 */
static __BK_ALWAYS_INLINE void
bk_barrier(void)
{
    __asm__ volatile ("" : : : "memory");
}

/* ============================================================================
 * OPERAÇÕES ATÔMICAS BÁSICAS (BK_I32)
 * ============================================================================ */

/**
 * bk_atomic_read() - Lê o valor de uma variável atômica
 * @v: Ponteiro para a variável atômica
 * 
 * Retorna: O valor atual
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_read(const bk_atomic_t *v)
{
    return v->counter;
}

/**
 * bk_atomic_set() - Define o valor de uma variável atômica
 * @v: Ponteiro para a variável atômica
 * @i: Novo valor
 */
static __BK_ALWAYS_INLINE void
bk_atomic_set(bk_atomic_t *v, BK_I32 i)
{
    v->counter = i;
    bk_barrier();
}

/**
 * bk_atomic_add() - Adiciona um valor atomicamente
 * @i: Valor a adicionar
 * @v: Ponteiro para a variável atômica
 * 
 * Retorna: O valor anterior
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_add(BK_I32 i, bk_atomic_t *v)
{
    return __sync_fetch_and_add(&v->counter, i);
}

/**
 * bk_atomic_sub() - Subtrai um valor atomicamente
 * @i: Valor a subtrair
 * @v: Ponteiro para a variável atômica
 * 
 * Retorna: O valor anterior
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_sub(BK_I32 i, bk_atomic_t *v)
{
    return __sync_fetch_and_sub(&v->counter, i);
}

/**
 * bk_atomic_inc() - Incrementa atomicamente
 * @v: Ponteiro para a variável atômica
 * 
 * Retorna: O valor anterior
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_inc(bk_atomic_t *v)
{
    return __sync_fetch_and_add(&v->counter, 1);
}

/**
 * bk_atomic_dec() - Decrementa atomicamente
 * @v: Ponteiro para a variável atômica
 * 
 * Retorna: O valor anterior
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_dec(bk_atomic_t *v)
{
    return __sync_fetch_and_sub(&v->counter, 1);
}

/**
 * bk_atomic_inc_return() - Incrementa e retorna o novo valor
 * @v: Ponteiro para a variável atômica
 * 
 * Retorna: O valor após o incremento
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_inc_return(bk_atomic_t *v)
{
    return __sync_add_and_fetch(&v->counter, 1);
}

/**
 * bk_atomic_dec_return() - Decrementa e retorna o novo valor
 * @v: Ponteiro para a variável atômica
 * 
 * Retorna: O valor após o decremento
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_dec_return(bk_atomic_t *v)
{
    return __sync_sub_and_fetch(&v->counter, 1);
}

/**
 * bk_atomic_add_return() - Adiciona e retorna o novo valor
 * @i: Valor a adicionar
 * @v: Ponteiro para a variável atômica
 * 
 * Retorna: O valor após a adição
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_add_return(BK_I32 i, bk_atomic_t *v)
{
    return __sync_add_and_fetch(&v->counter, i);
}

/**
 * bk_atomic_sub_return() - Subtrai e retorna o novo valor
 * @i: Valor a subtrair
 * @v: Ponteiro para a variável atômica
 * 
 * Retorna: O valor após a subtração
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_sub_return(BK_I32 i, bk_atomic_t *v)
{
    return __sync_sub_and_fetch(&v->counter, i);
}

/**
 * bk_atomic_and() - Operação AND bit a bit atômica
 * @i: Máscara para AND
 * @v: Ponteiro para a variável atômica
 * 
 * Retorna: O valor anterior
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_and(BK_I32 i, bk_atomic_t *v)
{
    return __sync_fetch_and_and(&v->counter, i);
}

/**
 * bk_atomic_or() - Operação OR bit a bit atômica
 * @i: Máscara para OR
 * @v: Ponteiro para a variável atômica
 * 
 * Retorna: O valor anterior
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_or(BK_I32 i, bk_atomic_t *v)
{
    return __sync_fetch_and_or(&v->counter, i);
}

/**
 * bk_atomic_xor() - Operação XOR bit a bit atômica
 * @i: Máscara para XOR
 * @v: Ponteiro para a variável atômica
 * 
 * Retorna: O valor anterior
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_xor(BK_I32 i, bk_atomic_t *v)
{
    return __sync_fetch_and_xor(&v->counter, i);
}

/**
 * bk_atomic_cmpxchg() - Compare and exchange
 * @v: Ponteiro para a variável atômica
 * @old: Valor esperado
 * @new: Novo valor
 * 
 * Retorna: O valor anterior. Se retornar == old, a troca foi bem-sucedida.
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_cmpxchg(bk_atomic_t *v, BK_I32 old, BK_I32 new)
{
    return __sync_val_compare_and_swap(&v->counter, old, new);
}

/**
 * bk_atomic_cmpxchg_bool() - Compare and exchange (versão booleana)
 * @v: Ponteiro para a variável atômica
 * @old: Valor esperado
 * @new: Novo valor
 * 
 * Retorna: 1 se a troca foi bem-sucedida, 0 caso contrário.
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_cmpxchg_bool(bk_atomic_t *v, BK_I32 old, BK_I32 new)
{
    return __sync_bool_compare_and_swap(&v->counter, old, new);
}

/**
 * bk_atomic_xchg() - Exchange atômico
 * @v: Ponteiro para a variável atômica
 * @new: Novo valor
 * 
 * Retorna: O valor anterior
 */
static __BK_ALWAYS_INLINE BK_I32
bk_atomic_xchg(bk_atomic_t *v, BK_I32 new)
{
    return __sync_lock_test_and_set(&v->counter, new);
}

/**
 * bk_atomic_clear() - Limpa uma variável atômica (para spinlocks)
 * @v: Ponteiro para a variável atômica
 */
static __BK_ALWAYS_INLINE void
bk_atomic_clear(bk_atomic_t *v)
{
    __sync_lock_release(&v->counter);
}

/* ============================================================================
 * OPERAÇÕES ATÔMICAS PARA BK_UI32
 * ============================================================================ */

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_read(const bk_atomic32_t *v)
{
    return v->counter;
}

static __BK_ALWAYS_INLINE void
bk_atomic32_set(bk_atomic32_t *v, BK_UI32 i)
{
    v->counter = i;
    bk_barrier();
}

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_add(BK_UI32 i, bk_atomic32_t *v)
{
    return __sync_fetch_and_add(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_sub(BK_UI32 i, bk_atomic32_t *v)
{
    return __sync_fetch_and_sub(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_inc(bk_atomic32_t *v)
{
    return __sync_fetch_and_add(&v->counter, 1);
}

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_dec(bk_atomic32_t *v)
{
    return __sync_fetch_and_sub(&v->counter, 1);
}

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_inc_return(bk_atomic32_t *v)
{
    return __sync_add_and_fetch(&v->counter, 1);
}

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_dec_return(bk_atomic32_t *v)
{
    return __sync_sub_and_fetch(&v->counter, 1);
}

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_add_return(BK_UI32 i, bk_atomic32_t *v)
{
    return __sync_add_and_fetch(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_sub_return(BK_UI32 i, bk_atomic32_t *v)
{
    return __sync_sub_and_fetch(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_and(BK_UI32 i, bk_atomic32_t *v)
{
    return __sync_fetch_and_and(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_or(BK_UI32 i, bk_atomic32_t *v)
{
    return __sync_fetch_and_or(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_xor(BK_UI32 i, bk_atomic32_t *v)
{
    return __sync_fetch_and_xor(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_cmpxchg(bk_atomic32_t *v, BK_UI32 old, BK_UI32 new)
{
    return __sync_val_compare_and_swap(&v->counter, old, new);
}

static __BK_ALWAYS_INLINE BK_I32
bk_atomic32_cmpxchg_bool(bk_atomic32_t *v, BK_UI32 old, BK_UI32 new)
{
    return __sync_bool_compare_and_swap(&v->counter, old, new);
}

static __BK_ALWAYS_INLINE BK_UI32
bk_atomic32_xchg(bk_atomic32_t *v, BK_UI32 new)
{
    return __sync_lock_test_and_set(&v->counter, new);
}

static __BK_ALWAYS_INLINE void
bk_atomic32_clear(bk_atomic32_t *v)
{
    __sync_lock_release(&v->counter);
}

/* ============================================================================
 * OPERAÇÕES ATÔMICAS PARA BK_UI64
 * ============================================================================ */

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_read(const bk_atomic64_t *v)
{
    return v->counter;
}

static __BK_ALWAYS_INLINE void
bk_atomic64_set(bk_atomic64_t *v, BK_UI64 i)
{
    v->counter = i;
    bk_barrier();
}

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_add(BK_UI64 i, bk_atomic64_t *v)
{
    return __sync_fetch_and_add(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_sub(BK_UI64 i, bk_atomic64_t *v)
{
    return __sync_fetch_and_sub(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_inc(bk_atomic64_t *v)
{
    return __sync_fetch_and_add(&v->counter, 1);
}

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_dec(bk_atomic64_t *v)
{
    return __sync_fetch_and_sub(&v->counter, 1);
}

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_inc_return(bk_atomic64_t *v)
{
    return __sync_add_and_fetch(&v->counter, 1);
}

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_dec_return(bk_atomic64_t *v)
{
    return __sync_sub_and_fetch(&v->counter, 1);
}

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_add_return(BK_UI64 i, bk_atomic64_t *v)
{
    return __sync_add_and_fetch(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_sub_return(BK_UI64 i, bk_atomic64_t *v)
{
    return __sync_sub_and_fetch(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_and(BK_UI64 i, bk_atomic64_t *v)
{
    return __sync_fetch_and_and(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_or(BK_UI64 i, bk_atomic64_t *v)
{
    return __sync_fetch_and_or(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_xor(BK_UI64 i, bk_atomic64_t *v)
{
    return __sync_fetch_and_xor(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_cmpxchg(bk_atomic64_t *v, BK_UI64 old, BK_UI64 new)
{
    return __sync_val_compare_and_swap(&v->counter, old, new);
}

static __BK_ALWAYS_INLINE BK_I32
bk_atomic64_cmpxchg_bool(bk_atomic64_t *v, BK_UI64 old, BK_UI64 new)
{
    return __sync_bool_compare_and_swap(&v->counter, old, new);
}

static __BK_ALWAYS_INLINE BK_UI64
bk_atomic64_xchg(bk_atomic64_t *v, BK_UI64 new)
{
    return __sync_lock_test_and_set(&v->counter, new);
}

static __BK_ALWAYS_INLINE void
bk_atomic64_clear(bk_atomic64_t *v)
{
    __sync_lock_release(&v->counter);
}

/* ============================================================================
 * OPERAÇÕES ATÔMICAS PARA BK_LONG
 * ============================================================================ */

static __BK_ALWAYS_INLINE BK_L3264
bk_atomic_long_read(const bk_atomic_long_t *v)
{
    return v->counter;
}

static __BK_ALWAYS_INLINE void
bk_atomic_long_set(bk_atomic_long_t *v, BK_L3264 i)
{
    v->counter = i;
    bk_barrier();
}

static __BK_ALWAYS_INLINE BK_L3264
bk_atomic_long_add(BK_L3264 i, bk_atomic_long_t *v)
{
    return __sync_fetch_and_add(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_L3264
bk_atomic_long_sub(BK_L3264 i, bk_atomic_long_t *v)
{
    return __sync_fetch_and_sub(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_L3264
bk_atomic_long_inc(bk_atomic_long_t *v)
{
    return __sync_fetch_and_add(&v->counter, 1);
}

static __BK_ALWAYS_INLINE BK_L3264
bk_atomic_long_dec(bk_atomic_long_t *v)
{
    return __sync_fetch_and_sub(&v->counter, 1);
}

static __BK_ALWAYS_INLINE BK_L3264
bk_atomic_long_inc_return(bk_atomic_long_t *v)
{
    return __sync_add_and_fetch(&v->counter, 1);
}

static __BK_ALWAYS_INLINE BK_L3264
bk_atomic_long_dec_return(bk_atomic_long_t *v)
{
    return __sync_sub_and_fetch(&v->counter, 1);
}

static __BK_ALWAYS_INLINE BK_L3264
bk_atomic_long_add_return(BK_L3264 i, bk_atomic_long_t *v)
{
    return __sync_add_and_fetch(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_L3264
bk_atomic_long_sub_return(BK_L3264 i, bk_atomic_long_t *v)
{
    return __sync_sub_and_fetch(&v->counter, i);
}

static __BK_ALWAYS_INLINE BK_L3264
bk_atomic_long_cmpxchg(bk_atomic_long_t *v, BK_L3264 old, BK_L3264 new)
{
    return __sync_val_compare_and_swap(&v->counter, old, new);
}

static __BK_ALWAYS_INLINE BK_I32
bk_atomic_long_cmpxchg_bool(bk_atomic_long_t *v, BK_L3264 old, BK_L3264 new)
{
    return __sync_bool_compare_and_swap(&v->counter, old, new);
}

static __BK_ALWAYS_INLINE BK_L3264
bk_atomic_long_xchg(bk_atomic_long_t *v, BK_L3264 new)
{
    return __sync_lock_test_and_set(&v->counter, new);
}

static __BK_ALWAYS_INLINE void
bk_atomic_long_clear(bk_atomic_long_t *v)
{
    __sync_lock_release(&v->counter);
}

/* ============================================================================
 * OPERAÇÕES DE TESTE E DEFINIÇÃO DE BITS
 * ============================================================================ */

/**
 * bk_test_and_set_bit() - Testa e define um bit atomicamente
 * @nr: Número do bit (0-31)
 * @addr: Endereço da palavra
 * 
 * Retorna: O valor anterior do bit
 */
static __BK_ALWAYS_INLINE BK_I32
bk_test_and_set_bit(BK_I32 nr, volatile void *addr)
{
    BK_I32 mask = 1 << (nr & 0x1f);
    volatile BK_I32 *p = (volatile BK_I32 *)addr + (nr >> 5);
    BK_I32 old = __sync_fetch_and_or(p, mask);
    return (old & mask) != 0;
}

/**
 * bk_test_and_clear_bit() - Testa e limpa um bit atomicamente
 * @nr: Número do bit (0-31)
 * @addr: Endereço da palavra
 * 
 * Retorna: O valor anterior do bit
 */
static __BK_ALWAYS_INLINE BK_I32
bk_test_and_clear_bit(BK_I32 nr, volatile void *addr)
{
    BK_I32 mask = 1 << (nr & 0x1f);
    volatile BK_I32 *p = (volatile BK_I32 *)addr + (nr >> 5);
    BK_I32 old = __sync_fetch_and_and(p, ~mask);
    return (old & mask) != 0;
}

/**
 * bk_test_and_change_bit() - Testa e inverte um bit atomicamente
 * @nr: Número do bit (0-31)
 * @addr: Endereço da palavra
 * 
 * Retorna: O valor anterior do bit
 */
static __BK_ALWAYS_INLINE BK_I32
bk_test_and_change_bit(BK_I32 nr, volatile void *addr)
{
    BK_I32 mask = 1 << (nr & 0x1f);
    volatile BK_I32 *p = (volatile BK_I32 *)addr + (nr >> 5);
    BK_I32 old = __sync_fetch_and_xor(p, mask);
    return (old & mask) != 0;
}

/**
 * bk_set_bit() - Define um bit atomicamente
 * @nr: Número do bit
 * @addr: Endereço da palavra
 */
static __BK_ALWAYS_INLINE void
bk_set_bit(BK_I32 nr, volatile void *addr)
{
    bk_test_and_set_bit(nr, addr);
}

/**
 * bk_clear_bit() - Limpa um bit atomicamente
 * @nr: Número do bit
 * @addr: Endereço da palavra
 */
static __BK_ALWAYS_INLINE void
bk_clear_bit(BK_I32 nr, volatile void *addr)
{
    bk_test_and_clear_bit(nr, addr);
}

/**
 * bk_change_bit() - Inverte um bit atomicamente
 * @nr: Número do bit
 * @addr: Endereço da palavra
 */
static __BK_ALWAYS_INLINE void
bk_change_bit(BK_I32 nr, volatile void *addr)
{
    bk_test_and_change_bit(nr, addr);
}

/**
 * bk_test_bit() - Testa o valor de um bit (não atômico)
 * @nr: Número do bit
 * @addr: Endereço da palavra
 * 
 * Retorna: O valor do bit (0 ou 1)
 */
static __BK_ALWAYS_INLINE BK_I32
bk_test_bit(BK_I32 nr, const volatile void *addr)
{
    const volatile BK_I32 *p = (const volatile BK_I32 *)addr;
    return (p[nr >> 5] >> (nr & 0x1f)) & 1;
}

/* ============================================================================
 * OPERAÇÕES PARA VARIÁVEIS DE 64 BITS
 * ============================================================================ */

/**
 * bk_test_and_set_bit64() - Testa e define um bit em palavra de 64 bits
 * @nr: Número do bit (0-63)
 * @addr: Endereço da palavra
 * 
 * Retorna: O valor anterior do bit
 */
static __BK_ALWAYS_INLINE BK_I32
bk_test_and_set_bit64(BK_I32 nr, volatile void *addr)
{
    BK_UI64 mask = 1ULL << (nr & 0x3f);
    volatile BK_UI64 *p = (volatile BK_UI64 *)addr + (nr >> 6);
    BK_UI64 old = __sync_fetch_and_or(p, mask);
    return (old & mask) != 0;
}

/**
 * bk_test_and_clear_bit64() - Testa e limpa um bit em palavra de 64 bits
 * @nr: Número do bit (0-63)
 * @addr: Endereço da palavra
 * 
 * Retorna: O valor anterior do bit
 */
static __BK_ALWAYS_INLINE BK_I32
bk_test_and_clear_bit64(BK_I32 nr, volatile void *addr)
{
    BK_UI64 mask = 1ULL << (nr & 0x3f);
    volatile BK_UI64 *p = (volatile BK_UI64 *)addr + (nr >> 6);
    BK_UI64 old = __sync_fetch_and_and(p, ~mask);
    return (old & mask) != 0;
}

/* ============================================================================
 * OPERAÇÕES PARA SPINLOCKS (BASEADAS EM ATOMIC)
 * ============================================================================ */

/**
 * bk_spin_trylock() - Tenta adquirir um spinlock
 * @lock: Ponteiro para a variável do lock (0 = livre, 1 = ocupado)
 * 
 * Retorna: 1 se adquiriu, 0 se já estava ocupado
 */
static __BK_ALWAYS_INLINE BK_I32
bk_spin_trylock(bk_atomic_t *lock)
{
    return bk_atomic_cmpxchg_bool(lock, 0, 1);
}

/**
 * bk_spin_lock() - Adquire um spinlock (busy-wait)
 * @lock: Ponteiro para a variável do lock
 */
static __BK_ALWAYS_INLINE void
bk_spin_lock(bk_atomic_t *lock)
{
    while (__sync_lock_test_and_set(&lock->counter, 1)) {
        /* Spin */
        while (lock->counter) {
            bk_barrier();
        }
    }
}

/**
 * bk_spin_unlock() - Libera um spinlock
 * @lock: Ponteiro para a variável do lock
 */
static __BK_ALWAYS_INLINE void
bk_spin_unlock(bk_atomic_t *lock)
{
    __sync_lock_release(&lock->counter);
}

/**
 * bk_spin_lock_irq() - Adquire spinlock e desabilita interrupções
 * @lock: Ponteiro para a variável do lock
 * @flags: Variável para salvar o estado das interrupções
 * 
 * NOTA: Esta função depende de funções específicas da arquitetura
 * para manipular interrupções. Deve ser implementada em um header
 * arquitetura-específico.
 */
#define bk_spin_lock_irq(lock, flags) do { \
    bk_local_irq_save(flags); \
    bk_spin_lock(lock); \
} while (0)

/**
 * bk_spin_unlock_irq() - Libera spinlock e restaura interrupções
 * @lock: Ponteiro para a variável do lock
 * @flags: Estado das interrupções salvo anteriormente
 */
#define bk_spin_unlock_irq(lock, flags) do { \
    bk_spin_unlock(lock); \
    bk_local_irq_restore(flags); \
} while (0)

/* ============================================================================
 * MACROS PARA INICIALIZAÇÃO
 * ============================================================================ */

#define BK_ATOMIC_INIT(i)   { (i) }
#define BK_ATOMIC32_INIT(i) { (i) }
#define BK_ATOMIC64_INIT(i) { (i) }
#define BK_ATOMIC_LONG_INIT(i) { (i) }

#define BK_ATOMIC(name)     bk_atomic_t name = BK_ATOMIC_INIT(0)
#define BK_ATOMIC32(name)   bk_atomic32_t name = BK_ATOMIC32_INIT(0)
#define BK_ATOMIC64(name)   bk_atomic64_t name = BK_ATOMIC64_INIT(0)
#define BK_ATOMIC_LONG(name) bk_atomic_long_t name = BK_ATOMIC_LONG_INIT(0)

__BK_END_DECLS

#endif /* !_BIBLE_ATOMIC_COMMON_H_ */
