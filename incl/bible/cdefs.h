
#ifndef _BIBLEKERNEL_CDEFS_H
#define _BIBLEKERNEL_CDEFS_H 1

/* ================================================================
   Definições de compatibilidade de compilador
   ================================================================ */

#ifdef __cplusplus
#define __BK_BEGIN_DECLS   extern "C" {
#define __BK_END_DECLS     }
#else
#define __BK_BEGIN_DECLS
#define __BK_END_DECLS
#endif

/* Verificação de compilador suportado */
#if !defined(__GNUC__) && !defined(__clang__)
#error "BibleKernel requer GCC ou Clang"
#endif

/* Macros de verificação de features do compilador */
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif
#ifndef __has_feature
#define __has_feature(x) 0
#endif

/* ================================================================
   Macros fundamentais do BibleKernel
   ================================================================ */

/* Concatenação de símbolos */
#define __BK_CONCAT(x, y) x ## y
#define __BK_STRING(x) #x

/* Qualificadores para compatibilidade */
#define __bk_const        const
#define __bk_volatile     volatile
#define __bk_restrict     restrict
#define __bk_inline       inline

/* ================================================================
   Atributos específicos do BibleKernel
   ================================================================ */

/* Modos de operação do kernel híbrido */
#define __BK_MICRO        __attribute__((section(".micro.text")))
#define __BK_NANO         __attribute__((section(".nano.text")))
#define __BK_EXO          __attribute__((section(".exo.text")))
#define __BK_MONO         __attribute__((section(".mono.text")))

/* Tipos de drivers/subsistemas */
#define __BK_DRIVER       __attribute__((section(".driver.text")))
#define __BK_SUBSYSTEM    __attribute__((section(".subsystem.text")))
#define __BK_SERVICES     __attribute__((section(".services.text")))

/* Prioridades de inicialização */
#define __BK_INIT(level)  __attribute__((init_priority(level)))
#define __BK_EARLY_INIT   __BK_INIT(100)
#define __BK_CORE_INIT    __BK_INIT(200)
#define __BK_LATE_INIT    __BK_INIT(300)
#define __BK_POST_INIT    __BK_INIT(400)

/* Atributos de função */
#define __bk_noreturn     __attribute__((noreturn))
#define __bk_pure         __attribute__((pure))
#define __bk_constfn      __attribute__((const))
#define __bk_used         __attribute__((used))
#define __bk_unused       __attribute__((unused))
#define __bk_weak         __attribute__((weak))
#define __bk_alias(alias) __attribute__((alias(#alias)))

/* Otimizações */
#define __bk_always_inline    __attribute__((always_inline))
#define __bk_noinline         __attribute__((noinline))
#define __bk_flatten          __attribute__((flatten))
#define __bk_hot              __attribute__((hot))
#define __bk_cold             __attribute__((cold))
#define __bk_optimize(level)  __attribute__((optimize(level)))

/* Segurança e verificação */
#define __bk_nonnull(...)     __attribute__((nonnull(__VA_ARGS__)))
#define __bk_returns_nonnull  __attribute__((returns_nonnull))
#define __bk_warn_unused      __attribute__((warn_unused_result))
#define __bk_sentinel         __attribute__((sentinel))
#define __bk_format(type, idx, first) __attribute__((format(type, idx, first)))

/* Alinhamento e empacotamento */
#define __bk_packed           __attribute__((packed))
#define __bk_aligned(x)       __attribute__((aligned(x)))
#define __BK_CACHELINE        __bk_aligned(64)
#define __BK_PAGEALIGN        __bk_aligned(4096)

/* ================================================================
   Macros para arquitetura híbrida
   ================================================================ */

/* Seleção de componente baseado em modo */
#define __BK_MICRO_ONLY(...) \
    __attribute__((target("micro-mode"))) __VA_ARGS__
    
#define __BK_NANO_ONLY(...) \
    __attribute__((target("nano-mode"))) __VA_ARGS__
    
#define __BK_EXO_ONLY(...) \
    __attribute__((target("exo-mode"))) __VA_ARGS__
    
#define __BK_MONO_ONLY(...) \
    __attribute__((target("mono-mode"))) __VA_ARGS__

/* Macros para transições entre modos */
#define __BK_ENTER_MICRO()   __asm__ volatile("enter_micro_mode")
#define __BK_ENTER_NANO()    __asm__ volatile("enter_nano_mode")
#define __BK_ENTER_EXO()     __asm__ volatile("enter_exo_mode")
#define __BK_ENTER_MONO()    __asm__ volatile("enter_mono_mode")

/* ================================================================
   Barreiras e sincronização
   ================================================================ */

/* Barreiras de memória */
#define __bk_membar()        __asm__ volatile("" ::: "memory")
#define __bk_mfence()        __asm__ volatile("mfence" ::: "memory")
#define __bk_sfence()        __asm__ volatile("sfence" ::: "memory")
#define __bk_lfence()        __asm__ volatile("lfence" ::: "memory")

/* Barreiras específicas do kernel híbrido */
#define __BK_HYBRID_BARRIER() \
    do { \
        __bk_membar(); \
        __asm__ volatile("hybrid_barrier" ::: "memory"); \
    } while(0)

/* ================================================================
   Macros de depuração e teste
   ================================================================ */

/* Anotações para debug */
#define __bk_debug           __attribute__((section(".debug.text")))
#define __bk_test            __attribute__((section(".test.text")))
#define __bk_profile         __attribute__((section(".profile.text")))

/* Macros para verificação */
#define __bk_assert_check    __attribute__((no_sanitize("undefined")))
#define __bk_sanitize_addr   __attribute__((no_sanitize("address")))

/* ================================================================
   Macros de otimização de caminho
   ================================================================ */

#define __bk_likely(x)       __builtin_expect(!!(x), 1)
#define __bk_unlikely(x)     __builtin_expect(!!(x), 0)

#define __bk_prefetch(addr)  __builtin_prefetch(addr)
#define __bk_prefetch_r(addr) __builtin_prefetch(addr, 0)
#define __bk_prefetch_w(addr) __builtin_prefetch(addr, 1)

/* ================================================================
   Macros utilitárias
   ================================================================ */

/* Tamanho de array */
#define __BK_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Container of */
#define __BK_CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/* Mínimo e máximo */
#define __BK_MIN(a, b) ((a) < (b) ? (a) : (b))
#define __BK_MAX(a, b) ((a) > (b) ? (a) : (b))

/* Alinhamento */
#define __BK_ALIGN_UP(x, align)   (((x) + (align) - 1) & ~((align) - 1))
#define __BK_ALIGN_DOWN(x, align) ((x) & ~((align) - 1))

/* Bits */
#define __BK_BIT(n)          (1ULL << (n))
#define __BK_MASK(n)         (__BK_BIT(n) - 1)
#define __BK_SET_BIT(var, bit)   ((var) |= __BK_BIT(bit))
#define __BK_CLEAR_BIT(var, bit) ((var) &= ~__BK_BIT(bit))
#define __BK_TEST_BIT(var, bit)  ((var) & __BK_BIT(bit))

/* ================================================================
   Macros para o modelo híbrido
   ================================================================ */

/* Definição de camadas */
#define __BK_LAYER_MICRO     0x01
#define __BK_LAYER_NANO      0x02
#define __BK_LAYER_EXO       0x04
#define __BK_LAYER_MONO      0x08

/* Anotações de camada */
#define __BK_IN_MICRO        __attribute__((layer("micro")))
#define __BK_IN_NANO         __attribute__((layer("nano")))
#define __BK_IN_EXO          __attribute__((layer("exo")))
#define __BK_IN_MONO         __attribute__((layer("mono")))

/* Transições entre camadas */
#define __BK_CALL_MICRO(func, ...) \
    __bk_enter_micro(); \
    func(__VA_ARGS__); \
    __bk_leave_micro()

#define __BK_CALL_NANO(func, ...) \
    __bk_enter_nano(); \
    func(__VA_ARGS__); \
    __bk_leave_nano()

/* ================================================================
   Macros de segurança e verificação
   ================================================================ */

/* Verificação de ponteiros */
#define __BK_CHECK_PTR(ptr) \
    ({ \
        typeof(ptr) __ptr = (ptr); \
        (__ptr && !__builtin_address_sanitizer(__ptr)); \
    })

/* Verificação de limites */
#define __BK_CHECK_BOUNDS(ptr, size) \
    ({ \
        typeof(ptr) __ptr = (ptr); \
        typeof(size) __size = (size); \
        (__ptr && __size && __builtin_object_size(__ptr, 0) >= __size); \
    })

/* ================================================================
   Macros de compatibilidade
   ================================================================ */

/* Para código que pode ser usado em diferentes modos */
#define __BK_HYBRID_COMPAT   __attribute__((target("generic")))
#define __BK_MODE_AGNOSTIC   __attribute__((no_mode_specific))

/* Para drivers/subsistemas portáveis */
#define __BK_PORTABLE        __attribute__((section(".portable.text")))

/* ================================================================
   Macros de logging e trace
   ================================================================ */

/* Níveis de log */
#define __BK_LOG_DEBUG       __attribute__((section(".log.debug")))
#define __BK_LOG_INFO        __attribute__((section(".log.info")))
#define __BK_LOG_WARN        __attribute__((section(".log.warn")))
#define __BK_LOG_ERROR       __attribute__((section(".log.error")))
#define __BK_LOG_FATAL       __attribute__((section(".log.fatal")))

/* Tracepoints */
#define __BK_TRACEPOINT(name) \
    __attribute__((section(".trace." #name)))

/* ================================================================
   Macros finais
   ================================================================ */

/* Marcação de código experimental */
#define __BK_EXPERIMENTAL    __attribute__((deprecated("Experimental API")))
#define __BK_UNSTABLE        __attribute__((section(".unstable.text")))

/* Marcação de código legado */
#define __BK_LEGACY          __attribute__((deprecated("Legacy API")))
#define __BK_OBSOLETE        __attribute__((section(".obsolete.text")))

/* Versão e identificação */
#define __BK_VERSION(major, minor, patch) \
    __attribute__((version(major * 10000 + minor * 100 + patch)))

/* Identificação do módulo */
#define __BK_MODULE(name) \
    static const char __bk_module_name[] __bk_used = name

/* ================================================================
   Macros de depuração condicional
   ================================================================ */

#ifdef BK_DEBUG
#define __BK_DEBUG_ONLY(...) __VA_ARGS__
#define __BK_DEBUG_CODE(code) code
#else
#define __BK_DEBUG_ONLY(...)
#define __BK_DEBUG_CODE(code)
#endif

#ifdef BK_TEST
#define __BK_TEST_ONLY(...) __VA_ARGS__
#define __BK_TEST_CODE(code) code
#else
#define __BK_TEST_ONLY(...)
#define __BK_TEST_CODE(code)
#endif

/* ================================================================
   Fim do header
   ================================================================ */

#endif /* _BIBLEKERNEL_CDEFS_H */