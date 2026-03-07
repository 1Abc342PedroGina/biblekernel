#ifndef _BIBLE_KPRINTN_H
#define _BIBLE_KPRINTN_H

#include <bible/types.h>
#include <bible/ktypes.h>
#include <bible/task.h>
#include <bible/kernl_object.h>
#include <bible/ipc.h>
#include <bible/exception.h>
#include <bible/interrupt.h>
#include <bible/bofs.h>
#include <bible/ctfs.h>
#include <bible/vm.h>
#include <bible/pmap.h>

/*
 * KPrintn - Kernel Print Number System
 * 
 * Formato: [SUBSYSTEM]_[LEVEL]:0x[VALUE]
 * Exemplo: PROC_SYS_ERROR:0x1000
 *          MEM_SYS_WARNING:0x2000
 *          FS_SYS_INFO:0x3000
 */

/* Níveis de severidade */
#define KPRINTN_LEVEL_EMERG     0
#define KPRINTN_LEVEL_ALERT     1
#define KPRINTN_LEVEL_CRIT      2
#define KPRINTN_LEVEL_ERROR     3
#define KPRINTN_LEVEL_WARNING   4
#define KPRINTN_LEVEL_NOTICE    5
#define KPRINTN_LEVEL_INFO       6
#define KPRINTN_LEVEL_DEBUG      7

/* Subsystems */
#define KPRINTN_SUBSYS_PROC     0x01  /* Gerenciamento de Processos */
#define KPRINTN_SUBSYS_MEM      0x02  /* Gerenciamento de Memória */
#define KPRINTN_SUBSYS_FS       0x03  /* Sistema de Arquivos */
#define KPRINTN_SUBSYS_NET      0x04  /* Networking */
#define KPRINTN_SUBSYS_DEV      0x05  /* Device Drivers */
#define KPRINTN_SUBSYS_SCHED    0x06  /* Escalonador */
#define KPRINTN_SUBSYS_IPC      0x07  /* Comunicação entre Processos */
#define KPRINTN_SUBSYS_TIMER    0x08  /* Timers */
#define KPRINTN_SUBSYS_SEC      0x09  /* Segurança */
#define KPRINTN_SUBSYS_MOD      0x0A  /* Módulos do Kernel */

/* String mapping para subsystems */
#define KPRINTN_PROC_SYS        "PROC_SYS"
#define KPRINTN_MEM_SYS         "MEM_SYS"
#define KPRINTN_FS_SYS          "FS_SYS"
#define KPRINTN_NET_SYS         "NET_SYS"
#define KPRINTN_DEV_SYS         "DEV_SYS"
#define KPRINTN_SCHED_SYS       "SCHED_SYS"
#define KPRINTN_IPC_SYS         "IPC_SYS"
#define KPRINTN_TIMER_SYS       "TIMER_SYS"
#define KPRINTN_SEC_SYS         "SEC_SYS"
#define KPRINTN_MOD_SYS         "MOD_SYS"

/* String mapping para níveis */
#define KPRINTN_LEVEL_STR_EMERG    "EMERG"
#define KPRINTN_LEVEL_STR_ALERT    "ALERT"
#define KPRINTN_LEVEL_STR_CRIT     "CRIT"
#define KPRINTN_LEVEL_STR_ERROR    "ERROR"
#define KPRINTN_LEVEL_STR_WARNING  "WARNING"
#define KPRINTN_LEVEL_STR_NOTICE   "NOTICE"
#define KPRINTN_LEVEL_STR_INFO     "INFO"
#define KPRINTN_LEVEL_STR_DEBUG    "DEBUG"

/* Códigos de erro comuns (valores de exemplo) */
#define KPRINTN_ERR_PROC_NONE      0x0000  /* Sem erro */
#define KPRINTN_ERR_PROC_CREATE    0x1000  /* Erro ao criar processo */
#define KPRINTN_ERR_PROC_KILL      0x1001  /* Erro ao matar processo */
#define KPRINTN_ERR_PROC_SCHED     0x1002  /* Erro no escalonamento */
#define KPRINTN_ERR_PROC_LIMIT     0x1003  /* Limite de processos atingido */

#define KPRINTN_ERR_MEM_ALLOC      0x2000  /* Erro de alocação de memória */
#define KPRINTN_ERR_MEM_FREE       0x2001  /* Erro ao liberar memória */
#define KPRINTN_ERR_MEM_OVERFLOW   0x2002  /* Estouro de memória */
#define KPRINTN_ERR_MEM_PROTECT    0x2003  /* Erro de proteção de memória */

#define KPRINTN_ERR_FS_MOUNT       0x3000  /* Erro ao montar filesystem */
#define KPRINTN_ERR_FS_OPEN        0x3001  /* Erro ao abrir arquivo */
#define KPRINTN_ERR_FS_READ        0x3002  /* Erro ao ler arquivo */
#define KPRINTN_ERR_FS_WRITE       0x3003  /* Erro ao escrever arquivo */
#define KPRINTN_ERR_FS_PERM        0x3004  /* Erro de permissão */

#define KPRINTN_ERR_NET_SOCKET     0x4000  /* Erro ao criar socket */
#define KPRINTN_ERR_NET_CONNECT    0x4001  /* Erro de conexão */
#define KPRINTN_ERR_NET_BIND       0x4002  /* Erro ao bind */
#define KPRINTN_ERR_NET_TIMEOUT    0x4003  /* Timeout de rede */

#define KPRINTN_ERR_DEV_NOTFOUND   0x5000  /* Dispositivo não encontrado */
#define KPRINTN_ERR_DEV_BUSY       0x5001  /* Dispositivo ocupado */
#define KPRINTN_ERR_DEV_IO         0x5002  /* Erro de I/O */
#define KPRINTN_ERR_DEV_INIT       0x5003  /* Erro de inicialização */

/* Flags de formatação */
#define KPRINTN_FLAG_NONE          0x0000
#define KPRINTN_FLAG_TIMESTAMP     0x0001
#define KPRINTN_FLAG_PID           0x0002
#define KPRINTN_FLAG_CPU           0x0004
#define KPRINTN_FLAG_FUNCTION      0x0008
#define KPRINTN_FLAG_LINE          0x0010

/* Estrutura para mensagem de log */
struct kprintn_msg {
    __BK_UI8 subsystem;           /* Subsystem ID */
    __BK_UI8 level;                /* Severity level */
    __BK_UI32 error_code;          /* Error code */
    __BK_UI32 flags;               /* Format flags */
    BK_UINT64 timestamp;           /* Timestamp */
    __BK_UI32 pid;                 /* Process ID */
    __BK_UI32 cpu_id;              /* CPU ID */
    const char *function;          /* Function name */
    __BK_UI32 line;                /* Line number */
    const char *file;              /* File name */
};

#ifndef __bible_printf
#define __bible_printf(x, y) __attribute__((format(printf, x, y)))
#endif

/* Inicialização do sistema de log */
int kprintn_init(void);
void kprintn_exit(void);

/* Função principal de logging - versão variádica */
__bible_printf(5, 6)
void _kprintn(__BK_UI8 subsystem, __BK_UI8 level, __BK_UI32 error_code, 
              __BK_UI32 flags, const char *fmt, ...);

/* Macros para facilitar o uso */
#define kprintn(subsys, level, code, fmt, ...) \
    _kprintn(subsys, level, code, KPRINTN_FLAG_TIMESTAMP | KPRINTN_FLAG_PID, \
             fmt, ##__VA_ARGS__)

#define kprintn_debug(subsys, code, fmt, ...) \
    _kprintn(subsys, KPRINTN_LEVEL_DEBUG, code, \
             KPRINTN_FLAG_TIMESTAMP | KPRINTN_FLAG_PID | KPRINTN_FLAG_FUNCTION | KPRINTN_FLAG_LINE, \
             fmt " [%s:%d]", ##__VA_ARGS__, __func__, __LINE__)

/* Macros específicas por subsistema */
#define proc_sys_error(code, fmt, ...) \
    _kprintn(KPRINTN_SUBSYS_PROC, KPRINTN_LEVEL_ERROR, code, \
             KPRINTN_FLAG_TIMESTAMP | KPRINTN_FLAG_PID, \
             fmt, ##__VA_ARGS__)

#define proc_sys_warning(code, fmt, ...) \
    _kprintn(KPRINTN_SUBSYS_PROC, KPRINTN_LEVEL_WARNING, code, \
             KPRINTN_FLAG_TIMESTAMP | KPRINTN_FLAG_PID, \
             fmt, ##__VA_ARGS__)

#define proc_sys_info(code, fmt, ...) \
    _kprintn(KPRINTN_SUBSYS_PROC, KPRINTN_LEVEL_INFO, code, \
             KPRINTN_FLAG_TIMESTAMP | KPRINTN_FLAG_PID, \
             fmt, ##__VA_ARGS__)

#define mem_sys_error(code, fmt, ...) \
    _kprintn(KPRINTN_SUBSYS_MEM, KPRINTN_LEVEL_ERROR, code, \
             KPRINTN_FLAG_TIMESTAMP | KPRINTN_FLAG_PID, \
             fmt, ##__VA_ARGS__)

#define mem_sys_warning(code, fmt, ...) \
    _kprintn(KPRINTN_SUBSYS_MEM, KPRINTN_LEVEL_WARNING, code, \
             KPRINTN_FLAG_TIMESTAMP | KPRINTN_FLAG_PID, \
             fmt, ##__VA_ARGS__)

#define fs_sys_error(code, fmt, ...) \
    _kprintn(KPRINTN_SUBSYS_FS, KPRINTN_LEVEL_ERROR, code, \
             KPRINTN_FLAG_TIMESTAMP | KPRINTN_FLAG_PID, \
             fmt, ##__VA_ARGS__)

#define fs_sys_warning(code, fmt, ...) \
    _kprintn(KPRINTN_SUBSYS_FS, KPRINTN_LEVEL_WARNING, code, \
             KPRINTN_FLAG_TIMESTAMP | KPRINTN_FLAG_PID, \
             fmt, ##__VA_ARGS__)

#define net_sys_error(code, fmt, ...) \
    _kprintn(KPRINTN_SUBSYS_NET, KPRINTN_LEVEL_ERROR, code, \
             KPRINTN_FLAG_TIMESTAMP | KPRINTN_FLAG_PID, \
             fmt, ##__VA_ARGS__)

#define dev_sys_error(code, fmt, ...) \
    _kprintn(KPRINTN_SUBSYS_DEV, KPRINTN_LEVEL_ERROR, code, \
             KPRINTN_FLAG_TIMESTAMP | KPRINTN_FLAG_PID, \
             fmt, ##__VA_ARGS__)

/* Funções auxiliares */
const char *kprintn_subsystem_str(__BK_UI8 subsystem);
const char *kprintn_level_str(__BK_UI8 level);
void kprintn_set_log_level(__BK_UI8 min_level);
void kprintn_set_output_driver(void (*output_func)(const char *msg));

#ifdef DEBUG_KPRINTN
#define kprintn_assert(cond, code, msg) \
    do { \
        if (!(cond)) { \
            kprintn(KPRINTN_SUBSYS_PROC, KPRINTN_LEVEL_ERROR, code, \
                   "Assertion failed: %s - %s [%s:%d]", \
                   #cond, msg, __func__, __LINE__); \
        } \
    } while (0)
#else
#define kprintn_assert(cond, code, msg) do { } while (0)
#endif

#endif /* _BIBLE_KPRINTN_H */
