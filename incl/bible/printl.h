#ifndef _BIBLE_PRINTL_H
#define _BIBLE_PRINTL_H

#include <bible/kprintn.h>
#include <bible/types.h>

/*
 * printl - Kernel Print Log
 * 
 * Inspirado no printk do Linux e printf do XNU/BSD
 * Formato: printl(SUBSISTEMA_SYS_NIVEL "Mensagem formatada %d", args...)
 * 
 * Exemplos:
 *   printl(PROC_SYS_ERROR "Falha no processo %d", pid);
 *   printl(MEM_SYS_WARNING "Pouca memória: %lu bytes", free_mem);
 *   printl(FS_SYS_INFO "Montando %s em %s", dev, mountpoint);
 */

/* Macros para subsistema + nível */
#define PROC_SYS_EMERG     "PROC_SYS_EMERG"
#define PROC_SYS_ALERT     "PROC_SYS_ALERT"
#define PROC_SYS_CRIT      "PROC_SYS_CRIT"
#define PROC_SYS_ERROR     "PROC_SYS_ERROR"
#define PROC_SYS_WARNING   "PROC_SYS_WARNING"
#define PROC_SYS_NOTICE    "PROC_SYS_NOTICE"
#define PROC_SYS_INFO      "PROC_SYS_INFO"
#define PROC_SYS_DEBUG     "PROC_SYS_DEBUG"

#define MEM_SYS_EMERG      "MEM_SYS_EMERG"
#define MEM_SYS_ALERT      "MEM_SYS_ALERT"
#define MEM_SYS_CRIT       "MEM_SYS_CRIT"
#define MEM_SYS_ERROR      "MEM_SYS_ERROR"
#define MEM_SYS_WARNING    "MEM_SYS_WARNING"
#define MEM_SYS_NOTICE     "MEM_SYS_NOTICE"
#define MEM_SYS_INFO       "MEM_SYS_INFO"
#define MEM_SYS_DEBUG      "MEM_SYS_DEBUG"

#define FS_SYS_EMERG       "FS_SYS_EMERG"
#define FS_SYS_ALERT       "FS_SYS_ALERT"
#define FS_SYS_CRIT        "FS_SYS_CRIT"
#define FS_SYS_ERROR       "FS_SYS_ERROR"
#define FS_SYS_WARNING     "FS_SYS_WARNING"
#define FS_SYS_NOTICE      "FS_SYS_NOTICE"
#define FS_SYS_INFO        "FS_SYS_INFO"
#define FS_SYS_DEBUG       "FS_SYS_DEBUG"

#define NET_SYS_EMERG      "NET_SYS_EMERG"
#define NET_SYS_ALERT      "NET_SYS_ALERT"
#define NET_SYS_CRIT       "NET_SYS_CRIT"
#define NET_SYS_ERROR      "NET_SYS_ERROR"
#define NET_SYS_WARNING    "NET_SYS_WARNING"
#define NET_SYS_NOTICE     "NET_SYS_NOTICE"
#define NET_SYS_INFO       "NET_SYS_INFO"
#define NET_SYS_DEBUG      "NET_SYS_DEBUG"

#define DEV_SYS_EMERG      "DEV_SYS_EMERG"
#define DEV_SYS_ALERT      "DEV_SYS_ALERT"
#define DEV_SYS_CRIT       "DEV_SYS_CRIT"
#define DEV_SYS_ERROR      "DEV_SYS_ERROR"
#define DEV_SYS_WARNING    "DEV_SYS_WARNING"
#define DEV_SYS_NOTICE     "DEV_SYS_NOTICE"
#define DEV_SYS_INFO       "DEV_SYS_INFO"
#define DEV_SYS_DEBUG      "DEV_SYS_DEBUG"

#define IPC_SYS_EMERG      "IPC_SYS_EMERG"
#define IPC_SYS_ALERT      "IPC_SYS_ALERT"
#define IPC_SYS_CRIT       "IPC_SYS_CRIT"
#define IPC_SYS_ERROR      "IPC_SYS_ERROR"
#define IPC_SYS_WARNING    "IPC_SYS_WARNING"
#define IPC_SYS_NOTICE     "IPC_SYS_NOTICE"
#define IPC_SYS_INFO       "IPC_SYS_INFO"
#define IPC_SYS_DEBUG      "IPC_SYS_DEBUG"

#define KSPC_SYS_EMERG      "KSPC_SYS_EMERG"
#define KSPC_SYS_ALERT      "KSPC_SYS_ALERT"
#define KSPC_SYS_CRIT       "KSPC_SYS_CRIT"
#define KSPC_SYS_ERROR      "KSPC_SYS_ERROR"
#define KSPC_SYS_WARNING    "KSPC_SYS_WARNING"
#define KSPC_SYS_NOTICE     "KSPC_SYS_NOTICE"
#define KSPC_SYS_INFO       "KSPC_SYS_INFO"
#define KSPC_SYS_DEBUG      "KSPC_SYS_DEBUG"
/*
 * printl - Função principal de log em modo kernel
 * @format: String no formato "SUBSISTEMA_NIVEL Mensagem %d %s"
 * 
 * Exemplo: printl(PROC_SYS_ERROR "Falha no pid %d", current->pid);
 */
void printl(const char *format, ...);

/*
 * printl_once - Printa apenas uma vez
 */
#define printl_once(format, ...) \
    do { \
        static __bk_bool __printed = 0; \
        if (!__printed) { \
            __printed = 1; \
            printl(format, ##__VA_ARGS__); \
        } \
    } while (0)

#endif /* _BIBLE_PRINTL_H */