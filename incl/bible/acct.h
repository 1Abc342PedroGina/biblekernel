#ifndef _BIBLE_ACCT_H_
#define _BIBLE_ACCT_H_

/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2024 Bible System Developers
 *
 * acct.h - Sistema de Contabilidade de Processos (Process Accounting)
 * 
 * Este cabeçalho fornece estruturas e funções para contabilizar o uso de
 * recursos por processos, threads e atividades no sistema.
 * 
 * O sistema de contabilidade registra:
 * - Tempo de CPU (user/system)
 * - Uso de memória
 * - Operações de I/O
 * - Chamadas de sistema
 * - Context switches
 * - Page faults
 * - Sinais recebidos/enviados
 * - Mensagens IPC
 */

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/ktypes.h>
#include <bible/task.h>
#include <bible/regsister.h>
#include <bible/queue.h>
#include <bible/ipc.h>

__BK_BEGIN_DECLS

/* ============================================================================
 * CONSTANTES DE CONTABILIDADE
 * ============================================================================ */

/* Versão do formato de contabilidade */
#define BK_ACCT_VERSION         0x20240314

/* Tamanho máximo do nome do comando */
#define BK_ACCT_COMM_LEN        16

/* Flags de controle de contabilidade */
#define BK_ACCT_ENABLE          0x00000001  /* Contabilidade ativada */
#define BK_ACCT_FLUSH           0x00000002  /* Força escrita imediata */
#define BK_ACCT_COMPRESS        0x00000004  /* Comprime registros */
#define BK_ACCT_VERIFY          0x00000008  /* Verifica integridade */
#define BK_ACCT_ENCRYPT         0x00000010  /* Criptografa registros */
#define BK_ACCT_PER_PROCESS     0x00000020  /* Contabilidade por processo */
#define BK_ACCT_PER_THREAD      0x00000040  /* Contabilidade por thread */
#define BK_ACCT_PER_ACTIVITY    0x00000080  /* Contabilidade por atividade */
#define BK_ACCT_PER_TASK        0x00000100  /* Contabilidade por task */

/* Tipos de registro de contabilidade */
typedef enum {
    BK_ACCT_TYPE_PROCESS    = 1,    /* Registro de processo */
    BK_ACCT_TYPE_THREAD     = 2,    /* Registro de thread */
    BK_ACCT_TYPE_ACTIVITY   = 3,    /* Registro de atividade */
    BK_ACCT_TYPE_TASK       = 4,    /* Registro de task */
    BK_ACCT_TYPE_IPC        = 5,    /* Registro de operações IPC */
    BK_ACCT_TYPE_IO         = 6,    /* Registro de operações I/O */
    BK_ACCT_TYPE_SIGNAL     = 7,    /* Registro de sinais */
    BK_ACCT_TYPE_MEMORY     = 8,    /* Registro de uso de memória */
    BK_ACCT_TYPE_NETWORK    = 9,    /* Registro de operações de rede */
    BK_ACCT_TYPE_SYSCALL    = 10,   /* Registro de chamadas de sistema */
    BK_ACCT_TYPE_SCHED      = 11,   /* Registro de escalonamento */
    BK_ACCT_TYPE_POWER      = 12,   /* Registro de consumo de energia */
} BK_ACCT_TYPE;

/* Flags de operação */
#define BK_ACCT_O_RDONLY    0x0001  /* Apenas leitura */
#define BK_ACCT_O_WRONLY    0x0002  /* Apenas escrita */
#define BK_ACCT_O_RDWR      0x0003  /* Leitura e escrita */
#define BK_ACCT_O_APPEND    0x0004  /* Append ao final */
#define BK_ACCT_O_TRUNC     0x0008  /* Trunca arquivo existente */
#define BK_ACCT_O_CREAT     0x0010  /* Cria se não existir */
#define BK_ACCT_O_EXCL      0x0020  /* Criação exclusiva */
#define BK_ACCT_O_SYNC      0x0040  /* Escrita síncrona */
#define BK_ACCT_O_DSYNC     0x0080  /* Escrita síncrona de dados */
#define BK_ACCT_O_RSYNC     0x0100  /* Leitura síncrona */

/* ============================================================================
 * ESTRUTURAS DE TEMPO PARA CONTABILIDADE
 * ============================================================================ */

/* Estrutura de tempo para contabilidade (compatível com ipc.h) */
struct bk_acct_timeval {
    BK_TIME     tv_sec;      /* segundos */
    BK_SUSECONDS tv_usec;    /* microssegundos */
};

typedef struct bk_acct_timeval BK_ACCT_TIMEVAL;

struct bk_acct_timespec {
    BK_TIME     tv_sec;      /* segundos */
    BK_L3264    tv_nsec;     /* nanossegundos */
};

typedef struct bk_acct_timespec BK_ACCT_TIMESPEC;

/* ============================================================================
 * ESTRUTURAS DE CONTABILIDADE
 * ============================================================================ */

/* Cabeçalho de registro de contabilidade */
struct bk_acct_header {
    BK_UINT32       ah_magic;           /* Número mágico */
    BK_UINT32       ah_version;         /* Versão do formato */
    BK_UINT32       ah_size;            /* Tamanho total do registro */
    BK_ACCT_TYPE    ah_type;            /* Tipo de registro */
    BK_UINT32       ah_flags;           /* Flags do registro */
    BK_TIME         ah_timestamp;       /* Timestamp de criação */
    BK_UINT32       ah_crc32;           /* Checksum CRC32 */
    BK_UINT32       ah_sequence;        /* Número de sequência */
};

typedef struct bk_acct_header BK_ACCT_HEADER;

/* Estatísticas de CPU */
struct bk_acct_cpu_stats {
    BK_ACCT_TIMEVAL  cs_user_time;       /* Tempo em modo usuário */
    BK_ACCT_TIMEVAL  cs_system_time;     /* Tempo em modo kernel */
    BK_ACCT_TIMEVAL  cs_child_user;      /* Tempo usuário dos filhos */
    BK_ACCT_TIMEVAL  cs_child_system;    /* Tempo kernel dos filhos */
    BK_UINT64        cs_voluntary_switches;   /* Switches voluntários */
    BK_UINT64        cs_involuntary_switches; /* Switches involuntários */
    BK_UINT32        cs_priority;         /* Prioridade média */
    BK_UINT32        cs_nice;             /* Nice value médio */
    BK_UINT64        cs_cycles;           /* Ciclos de CPU gastos */
};

typedef struct bk_acct_cpu_stats BK_ACCT_CPU_STATS;

/* Estatísticas de memória */
struct bk_acct_mem_stats {
    BK_VM_SIZE       ms_vsize;            /* Tamanho virtual total */
    BK_VM_SIZE       ms_rss;              /* Resident Set Size */
    BK_VM_SIZE       ms_peak_vsize;       /* Pico de memória virtual */
    BK_VM_SIZE       ms_peak_rss;         /* Pico de RSS */
    BK_VM_SIZE       ms_stack;            /* Tamanho da pilha */
    BK_VM_SIZE       ms_heap;             /* Tamanho do heap */
    BK_VM_SIZE       ms_text;             /* Tamanho do código */
    BK_VM_SIZE       ms_data;             /* Tamanho dos dados */
    BK_VM_SIZE       ms_shared;           /* Memória compartilhada */
    BK_VM_SIZE       ms_locked;           /* Memória bloqueada */
    BK_UINT64        ms_page_faults;      /* Page faults totais */
    BK_UINT64        ms_minor_faults;     /* Page faults menores */
    BK_UINT64        ms_major_faults;     /* Page faults maiores */
    BK_UINT64        ms_swapins;          /* Páginas trazidas do swap */
    BK_UINT64        ms_swapouts;         /* Páginas enviadas ao swap */
};

typedef struct bk_acct_mem_stats BK_ACCT_MEM_STATS;

/* Estatísticas de I/O */
struct bk_acct_io_stats {
    BK_UINT64        io_read_bytes;       /* Bytes lidos */
    BK_UINT64        io_write_bytes;      /* Bytes escritos */
    BK_UINT64        io_read_ops;         /* Operações de leitura */
    BK_UINT64        io_write_ops;        /* Operações de escrita */
    BK_UINT64        io_cancelled_bytes;  /* Bytes cancelados */
    BK_UINT64        io_errors;           /* Erros de I/O */
    BK_ACCT_TIMEVAL  io_read_time;        /* Tempo gasto em leituras */
    BK_ACCT_TIMEVAL  io_write_time;       /* Tempo gasto em escritas */
    BK_ACCT_TIMEVAL  io_busy_time;        /* Tempo com I/O ocupado */
};

typedef struct bk_acct_io_stats BK_ACCT_IO_STATS;

/* Estatísticas de IPC (baseado em ipc.h) */
struct bk_acct_ipc_stats {
    BK_UINT64        ipc_msgs_sent;       /* Mensagens enviadas */
    BK_UINT64        ipc_msgs_received;   /* Mensagens recebidas */
    BK_UINT64        ipc_bytes_sent;      /* Bytes enviados */
    BK_UINT64        ipc_bytes_received;  /* Bytes recebidos */
    BK_UINT64        ipc_ops;             /* Operações IPC totais */
    BK_UINT64        ipc_sync_ops;        /* Operações síncronas */
    BK_UINT64        ipc_async_ops;       /* Operações assíncronas */
    BK_UINT64        ipc_errors;          /* Erros em IPC */
    BK_UINT64        ipc_timeouts;        /* Timeouts em IPC */
    BK_ACCT_TIMEVAL  ipc_wait_time;       /* Tempo esperando IPC */
    
    /* Por tipo de IPC (baseado em BK_IPC_TYPE) */
    BK_UINT64        ipc_msg_count;       /* Mensagens (tipo 0) */
    BK_UINT64        ipc_shm_count;       /* Shared memory (tipo 1) */
    BK_UINT64        ipc_pipe_count;      /* Pipes (tipo 2) */
    BK_UINT64        ipc_fifo_count;      /* FIFOs (tipo 3) */
    BK_UINT64        ipc_socket_count;    /* Sockets (tipo 4) */
    BK_UINT64        ipc_sem_count;       /* Semáforos (tipo 5) */
    BK_UINT64        ipc_mutex_count;     /* Mutexes (tipo 6) */
    BK_UINT64        ipc_cond_count;      /* Cond vars (tipo 7) */
    BK_UINT64        ipc_event_count;     /* Eventos (tipo 8) */
    BK_UINT64        ipc_mailbox_count;   /* Mailboxes (tipo 9) */
    BK_UINT64        ipc_port_count;      /* Portas (tipo 10) */
    BK_UINT64        ipc_channel_count;   /* Canais (tipo 11) */
    BK_UINT64        ipc_queue_count;     /* Filas (tipo 12) */
    BK_UINT64        ipc_rv_count;        /* Rendezvous (tipo 13) */
    BK_UINT64        ipc_barrier_count;   /* Barreiras (tipo 14) */
    BK_UINT64        ipc_rpc_count;       /* RPCs (tipo 15) */
};

typedef struct bk_acct_ipc_stats BK_ACCT_IPC_STATS;

/* Estatísticas de sinais */
struct bk_acct_signal_stats {
    BK_UINT64        sig_received;        /* Sinais recebidos */
    BK_UINT64        sig_sent;            /* Sinais enviados */
    BK_UINT64        sig_pending;         /* Sinais pendentes */
    BK_UINT64        sig_ignored;         /* Sinais ignorados */
    BK_UINT64        sig_handled;         /* Sinais tratados */
    BK_UINT64        sig_queued;          /* Sinais enfileirados */
    BK_UINT64        sig_rt_received;     /* Sinais em tempo real */
    BK_UINT64        sig_rt_sent;         /* Sinais RT enviados */
};

typedef struct bk_acct_signal_stats BK_ACCT_SIGNAL_STATS;

/* Estatísticas de chamadas de sistema */
struct bk_acct_syscall_stats {
    BK_UINT64        sc_total;            /* Total de syscalls */
    BK_UINT64        sc_by_number[512];   /* Contagem por número */
    BK_ACCT_TIMEVAL  sc_total_time;       /* Tempo total em syscalls */
    BK_UINT64        sc_errors;           /* Syscalls com erro */
    BK_UINT64        sc_restarts;         /* Syscalls reiniciadas */
};

typedef struct bk_acct_syscall_stats BK_ACCT_SYSCALL_STATS;

/* Estatísticas de rede (baseadas em ipc.h) */
struct bk_acct_network_stats {
    BK_UINT64        net_bytes_sent;      /* Bytes enviados */
    BK_UINT64        net_bytes_received;  /* Bytes recebidos */
    BK_UINT64        net_packets_sent;    /* Pacotes enviados */
    BK_UINT64        net_packets_received; /* Pacotes recebidos */
    BK_UINT64        net_connections;     /* Conexões estabelecidas */
    BK_UINT64        net_errors;          /* Erros de rede */
    BK_UINT64        net_dropped;         /* Pacotes descartados */
    BK_UINT64        net_retransmits;     /* Retransmissões */
};

typedef struct bk_acct_network_stats BK_ACCT_NETWORK_STATS;

/* Estatísticas de energia */
struct bk_acct_power_stats {
    BK_UINT64        ps_energy_uj;        /* Energia em microjoules */
    BK_UINT64        ps_power_mw;         /* Potência média em mW */
    BK_UINT64        ps_cpu_energy;       /* Energia da CPU */
    BK_UINT64        ps_mem_energy;       /* Energia da memória */
    BK_UINT64        ps_io_energy;        /* Energia de I/O */
    BK_UINT64        ps_network_energy;   /* Energia de rede */
    BK_UINT32        ps_cpu_freq_mhz;     /* Frequência média da CPU */
    BK_UINT32        ps_c_temp_c;         /* Temperatura da CPU */
};

typedef struct bk_acct_power_stats BK_ACCT_POWER_STATS;

/* ============================================================================
 * REGISTROS COMPLETOS DE CONTABILIDADE
 * ============================================================================ */

/* Registro de processo (BK_ACCT_TYPE_PROCESS) */
struct bk_acct_process_record {
    BK_ACCT_HEADER   pr_header;
    
    /* Identificação */
    BK_PID           pr_pid;              /* PID do processo */
    BK_PID           pr_ppid;             /* PID do pai */
    BK_PID           pr_pgid;             /* Grupo de processos */
    BK_PID           pr_sid;              /* Sessão */
    
    /* Credenciais */
    BK_UID           pr_uid;              /* User ID */
    BK_UID           pr_euid;             /* Effective UID */
    BK_GID           pr_gid;              /* Group ID */
    BK_GID           pr_egid;             /* Effective GID */
    
    /* Comando */
    char             pr_comm[BK_ACCT_COMM_LEN];  /* Nome do comando */
    char             pr_path[256];        /* Caminho do executável */
    
    /* Estatísticas */
    BK_ACCT_CPU_STATS    pr_cpu;
    BK_ACCT_MEM_STATS    pr_mem;
    BK_ACCT_IO_STATS     pr_io;
    BK_ACCT_IPC_STATS    pr_ipc;
    BK_ACCT_SIGNAL_STATS pr_signal;
    BK_ACCT_SYSCALL_STATS pr_syscall;
    BK_ACCT_NETWORK_STATS pr_network;
    BK_ACCT_POWER_STATS   pr_power;
    
    /* Tempos */
    BK_TIME          pr_start_time;       /* Tempo de início */
    BK_TIME          pr_end_time;         /* Tempo de término */
    BK_UINT64        pr_duration_ms;      /* Duração em ms */
    
    /* Status de saída */
    BK_I32           pr_exit_status;      /* Status de saída */
    BK_I32           pr_term_signal;      /* Sinal de término */
    BK_UINT32        pr_flags;            /* Flags do processo */
    
    /* Contagem */
    BK_UINT32        pr_threads_created;  /* Threads criadas */
    BK_UINT32        pr_threads_terminated; /* Threads terminadas */
    BK_UINT32        pr_activities;       /* Atividades executadas */
    BK_UINT32        pr_tasks;            /* Tasks criadas */
};

typedef struct bk_acct_process_record BK_ACCT_PROCESS_RECORD;

/* Registro de thread (BK_ACCT_TYPE_THREAD) */
struct bk_acct_thread_record {
    BK_ACCT_HEADER   tr_header;
    
    /* Identificação */
    BK_LWPID         tr_tid;              /* Thread ID */
    BK_PID           tr_pid;              /* PID do processo */
    BK_UINT32        tr_thread_num;       /* Número da thread no processo */
    
    /* Estado */
    BK_UINT8         tr_priority;         /* Prioridade média */
    BK_UINT8         tr_policy;           /* Política de escalonamento */
    BK_UINT32        tr_cpu_affinity;     /* Afinidade de CPU */
    
    /* Estatísticas */
    BK_ACCT_CPU_STATS    tr_cpu;
    BK_ACCT_MEM_STATS    tr_mem;
    BK_ACCT_IPC_STATS    tr_ipc;
    BK_ACCT_SIGNAL_STATS tr_signal;
    
    /* Tempos */
    BK_TIME          tr_start_time;       /* Tempo de início */
    BK_TIME          tr_end_time;         /* Tempo de término */
    
    /* Atividades */
    BK_UINT32        tr_activities_executed;  /* Atividades executadas */
    BK_UINT32        tr_activities_completed; /* Atividades completadas */
    BK_UINT32        tr_activities_failed;    /* Atividades com falha */
    
    /* Resultado */
    BK_I32           tr_exit_code;         /* Código de saída */
};

typedef struct bk_acct_thread_record BK_ACCT_THREAD_RECORD;

/* Registro de atividade (BK_ACCT_TYPE_ACTIVITY) */
struct bk_acct_activity_record {
    BK_ACCT_HEADER   ar_header;
    
    /* Identificação */
    BK_ID            ar_activity_id;      /* ID da atividade */
    BK_LWPID         ar_tid;              /* Thread que executou */
    BK_PID           ar_pid;              /* Processo proprietário */
    BK_UINT8         ar_type;             /* Tipo de atividade */
    
    /* Handler */
    void             *ar_handler;          /* Ponteiro do handler */
    BK_SIZE          ar_data_size;         /* Tamanho dos dados */
    
    /* Tempos */
    BK_TIME          ar_creation_time;     /* Tempo de criação */
    BK_TIME          ar_start_time;        /* Tempo de início */
    BK_TIME          ar_completion_time;   /* Tempo de conclusão */
    BK_UINT64        ar_execution_time_ns; /* Tempo de execução em ns */
    
    /* Resultado */
    BK_I32           ar_result;            /* Resultado */
    BK_I32           ar_error;             /* Código de erro */
    
    /* Estado */
    BK_UINT8         ar_state;             /* Estado final */
    BK_UINT8         ar_priority;          /* Prioridade */
};

typedef struct bk_acct_activity_record BK_ACCT_ACTIVITY_RECORD;

/* Registro de IPC (BK_ACCT_TYPE_IPC) - baseado em ipc.h */
struct bk_acct_ipc_record {
    BK_ACCT_HEADER   ir_header;
    
    /* Identificação */
    BK_IPC_ID        ir_ipc_id;            /* ID do objeto IPC */
    BK_IPC_TYPE      ir_ipc_type;          /* Tipo de IPC */
    BK_PID           ir_owner_pid;         /* PID do proprietário */
    
    /* Operação */
    BK_UINT32        ir_operation;         /* Tipo de operação */
#define BK_ACCT_IPC_OP_CREATE   1
#define BK_ACCT_IPC_OP_DESTROY  2
#define BK_ACCT_IPC_OP_SEND     3
#define BK_ACCT_IPC_OP_RECEIVE  4
#define BK_ACCT_IPC_OP_CALL     5
#define BK_ACCT_IPC_OP_CONNECT  6
#define BK_ACCT_IPC_OP_ACCEPT   7
#define BK_ACCT_IPC_OP_CLOSE    8
    
    /* Dados da operação */
    BK_SIZE          ir_data_size;         /* Tamanho dos dados */
    BK_UINT32        ir_flags;             /* Flags da operação */
    BK_UINT32        ir_result;            /* Resultado */
    
    /* Tempo */
    BK_ACCT_TIMEVAL  ir_duration;          /* Duração da operação */
    
    /* Partes envolvidas */
    BK_PID           ir_sender_pid;        /* PID do remetente */
    BK_PID           ir_receiver_pid;      /* PID do destinatário */
    BK_LWPID         ir_sender_tid;        /* Thread remetente */
    BK_LWPID         ir_receiver_tid;      /* Thread destinatária */
};

typedef struct bk_acct_ipc_record BK_ACCT_IPC_RECORD;

/* ============================================================================
 * ESTRUTURAS DE GERENCIAMENTO
 * ============================================================================ */

/* Contexto de contabilidade */
struct bk_acct_context {
    BK_UINT32        ac_flags;             /* Flags de operação */
    BK_UINT32        ac_buffer_size;       /* Tamanho do buffer */
    void             *ac_buffer;           /* Buffer de registros */
    BK_SIZE          ac_buffer_used;       /* Espaço usado no buffer */
    BK_UINT64        ac_records_written;   /* Registros escritos */
    BK_UINT64        ac_records_read;      /* Registros lidos */
    
    /* Arquivo de saída (via IPC) */
    BK_IPC_ID        ac_output_ipc;        /* IPC ID para saída */
    BK_PIPE          *ac_output_pipe;       /* Pipe para saída */
    
    /* Estatísticas */
    BK_ACCT_PROCESS_RECORD ac_total;        /* Totais do sistema */
    
    /* Sincronização */
    BK_SPINLOCK      ac_lock;
    
    /* Filtros */
    BK_UINT32        ac_filter_types;      /* Tipos a registrar */
    BK_PID           ac_filter_pid;        /* Filtrar por PID (0 = todos) */
};

typedef struct bk_acct_context BK_ACCT_CONTEXT;

/* ============================================================================
 * FUNÇÕES DE CONTABILIDADE
 * ============================================================================ */

/* Inicialização */
BK_I32 bk_acct_init(void);
void bk_acct_shutdown(void);

/* Criação de contexto */
BK_ACCT_CONTEXT *bk_acct_context_create(BK_UINT32 flags);
BK_I32 bk_acct_context_destroy(BK_ACCT_CONTEXT *ctx);

/* Controle de contabilidade */
BK_I32 bk_acct_enable(BK_ACCT_CONTEXT *ctx);
BK_I32 bk_acct_disable(BK_ACCT_CONTEXT *ctx);
BK_I32 bk_acct_set_filter(BK_ACCT_CONTEXT *ctx, BK_UINT32 types, BK_PID pid);

/* Escrita de registros */
BK_I32 bk_acct_write_process(BK_ACCT_CONTEXT *ctx, struct bk_process *proc);
BK_I32 bk_acct_write_thread(BK_ACCT_CONTEXT *ctx, struct bk_thread *thread);
BK_I32 bk_acct_write_activity(BK_ACCT_CONTEXT *ctx, struct bk_activity *act);
BK_I32 bk_acct_write_ipc(BK_ACCT_CONTEXT *ctx, BK_IPC_ID ipc_id, BK_UINT32 op);

/* Leitura de registros */
BK_SSIZE bk_acct_read(BK_ACCT_CONTEXT *ctx, void *buffer, BK_SIZE size);
BK_I32 bk_acct_read_next(BK_ACCT_CONTEXT *ctx, BK_ACCT_HEADER *hdr, void **data);

/* Flush de buffers */
BK_I32 bk_acct_flush(BK_ACCT_CONTEXT *ctx);

/* Estatísticas globais */
BK_I32 bk_acct_get_total_stats(BK_ACCT_PROCESS_RECORD *stats);
BK_I32 bk_acct_reset_total_stats(void);

/* ============================================================================
 * FUNÇÕES DE UTILIDADE
 * ============================================================================ */

/* Cálculo de CRC32 (baseado em ipc.h) */
static __BK_ALWAYS_INLINE BK_UINT32
bk_acct_crc32(const void *data, BK_SIZE size)
{
    const BK_UI8 *bytes = (const BK_UI8 *)data;
    BK_UINT32 crc = 0xFFFFFFFF;
    
    for (BK_SIZE i = 0; i < size; i++) {
        crc ^= bytes[i];
        for (BK_UINT32 j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    
    return ~crc;
}

/* Validação de registro */
static __BK_ALWAYS_INLINE BK_BOOLEAN
bk_acct_validate_record(const BK_ACCT_HEADER *hdr)
{
    if (!hdr || hdr->ah_magic != BK_IPC_MAGIC_NUMBER)
        return BK_FALSE;
    
    if (hdr->ah_version != BK_ACCT_VERSION)
        return BK_FALSE;
    
    BK_UINT32 calc_crc = bk_acct_crc32((const BK_UI8 *)hdr + sizeof(BK_UINT32),
                                       hdr->ah_size - sizeof(BK_UINT32));
    
    return (calc_crc == hdr->ah_crc32);
}

/* Converte timeval para timespec */
static __BK_ALWAYS_INLINE void
bk_acct_timeval_to_timespec(const BK_ACCT_TIMEVAL *tv, BK_ACCT_TIMESPEC *ts)
{
    ts->tv_sec = tv->tv_sec;
    ts->tv_nsec = tv->tv_usec * 1000;
}

/* Converte timespec para timeval */
static __BK_ALWAYS_INLINE void
bk_acct_timespec_to_timeval(const BK_ACCT_TIMESPEC *ts, BK_ACCT_TIMEVAL *tv)
{
    tv->tv_sec = ts->tv_sec;
    tv->tv_usec = ts->tv_nsec / 1000;
}

/* ============================================================================
 * MACROS DE DEBUG
 * ============================================================================ */

#ifdef _BK_ACCT_DEBUG

#define BK_ACCT_DEBUG_PRINT(fmt, ...) \
    do { \
        bk_printf("[ACCT] " fmt "\n", ##__VA_ARGS__); \
    } while (0)

#define BK_ACCT_DUMP_PROCESS(rec) \
    do { \
        BK_ACCT_DEBUG_PRINT("Process PID=%d: CPU=%llu us, MEM=%llu KB", \
            (rec)->pr_pid, \
            (rec)->pr_cpu.cs_user_time.tv_sec * 1000000 + (rec)->pr_cpu.cs_user_time.tv_usec, \
            (rec)->pr_mem.ms_rss / 1024); \
    } while (0)

#else

#define BK_ACCT_DEBUG_PRINT(fmt, ...) do { } while (0)
#define BK_ACCT_DUMP_PROCESS(rec) do { } while (0)

#endif /* _BK_ACCT_DEBUG */

/* ============================================================================
 * COMPATIBILIDADE COM POSIX
 * ============================================================================ */

#ifndef _BK_NO_COMPAT

/* Compatibilidade com acct() do POSIX */
#define acct(filename)          bk_acct_enable_filename(filename)

#endif /* !_BK_NO_COMPAT */

__BK_END_DECLS

#endif /* !_BIBLE_ACCT_H_ */
