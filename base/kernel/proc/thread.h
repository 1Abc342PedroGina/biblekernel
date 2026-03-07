# ifndef _PROC_THREAD_H
# define _PROC_THREAD_H

# include <bible/task.h>
# include <bible/vm.h>
# include <bible/ipc.h>
# include <bible/interrupt.h>
# include <bible/exception.h>
# include <bible/kernl_object.h>
# include <bible/ktypes.h>

#define BK_THREAD_STATE_NEW             0x0001  /* Thread criada */
#define BK_THREAD_STATE_READY            0x0002  /* Pronta para execução */
#define BK_THREAD_STATE_RUNNING          0x0004  /* Em execução */
#define BK_THREAD_STATE_BLOCKED          0x0008  /* Bloqueada */
#define BK_THREAD_STATE_SLEEPING         0x0010  /* Dormindo */
#define BK_THREAD_STATE_SUSPENDED        0x0020  /* Suspensa */
#define BK_THREAD_STATE_TERMINATED       0x0040  /* Terminada */
#define BK_THREAD_STATE_ZOMBIE           0x0080  /* Zombie */
#define BK_THREAD_STATE_DEAD             0x0100  /* Morta */
#define BK_THREAD_STATE_IDLE              0x0200  /* Idle */
#define BK_THREAD_STATE_STOPPED          0x0400  /* Parada */
#define BK_THREAD_STATE_TRACED            0x0800  /* Tracejada */

/*
 * Flags da thread
 */
#define BK_THREAD_FLAG_SYSTEM            0x00000001  /* Thread do sistema */
#define BK_THREAD_FLAG_USER              0x00000002  /* Thread do usuário */
#define BK_THREAD_FLAG_KERNEL            0x00000004  /* Thread do kernel */
#define BK_THREAD_FLAG_DAEMON            0x00000008  /* Thread daemon */
#define BK_THREAD_FLAG_IDLE              0x00000010  /* Thread idle */
#define BK_THREAD_FLAG_DETACHED          0x00000020  /* Thread detached */
#define BK_THREAD_FLAG_SIGNALED          0x00000040  /* Sinalizada */
#define BK_THREAD_FLAG_STOPPED           0x00000080  /* Parada */
#define BK_THREAD_FLAG_CONTINUED         0x00000100  /* Continuada */
#define BK_THREAD_FLAG_EXITING           0x00000200  /* Em saída */
#define BK_THREAD_FLAG_EXITCODE          0x00000400  /* Código de saída definido */
#define BK_THREAD_FLAG_FORK              0x00000800  /* Em fork */
#define BK_THREAD_FLAG_EXEC              0x00001000  /* Em exec */
#define BK_THREAD_FLAG_VFORK             0x00002000  /* Em vfork */
#define BK_THREAD_FLAG_PTRACED           0x00004000  /* Sendo tracejada */
#define BK_THREAD_FLAG_TRACING           0x00008000  /* Tracejando */
#define BK_THREAD_FLAG_NO_PREEMPT        0x00010000  /* Não preemptível */
#define BK_THREAD_FLAG_PREEMPT           0x00020000  /* Preemptível */
#define BK_THREAD_FLAG_MIGRATE           0x00040000  /* Migrando */
#define BK_THREAD_FLAG_BOUND             0x00080000  /* Bound a CPU */
#define BK_THREAD_FLAG_CRITICAL          0x00100000  /* Crítica */
#define BK_THREAD_FLAG_REALTIME          0x00200000  /* Tempo real */
#define BK_THREAD_FLAG_ISO               0x00400000  /* Isolada */
#define BK_THREAD_FLAG_BOOSTED           0x00800000  /* Prioridade boostada */
#define BK_THREAD_FLAG_THROTTLED         0x01000000  /* Throttled */
#define BK_THREAD_FLAG_FAILSAFE          0x02000000  /* Failsafe ativo */
#define BK_THREAD_FLAG_WAITQ_PROMOTED    0x04000000  /* Promovida por waitq */
#define BK_THREAD_FLAG_INHERIT_PRIO      0x08000000  /* Herdar prioridade */

/*
 * Políticas de escalonamento
 */
#define BK_SCHED_POLICY_NORMAL           0
#define BK_SCHED_POLICY_FIFO             1
#define BK_SCHED_POLICY_RR               2
#define BK_SCHED_POLICY_BATCH            3
#define BK_SCHED_POLICY_IDLE             4
#define BK_SCHED_POLICY_DEADLINE         5
#define BK_SCHED_POLICY_EXT              6

/*
 * Modos de escalonamento
 */
#define BK_SCHED_MODE_TIMESHARE          0
#define BK_SCHED_MODE_REALTIME           1
#define BK_SCHED_MODE_FIXED              2

/*
 * Razões de bloqueio
 */
#define BK_THREAD_BLOCK_NONE             0
#define BK_THREAD_BLOCK_MUTEX             1
#define BK_THREAD_BLOCK_SEMAPHORE         2
#define BK_THREAD_BLOCK_CONDVAR           3
#define BK_THREAD_BLOCK_RWLOCK            4
#define BK_THREAD_BLOCK_QUEUE             5
#define BK_THREAD_BLOCK_PIPE              6
#define BK_THREAD_BLOCK_EVENT             7
#define BK_THREAD_BLOCK_IPC               8
#define BK_THREAD_BLOCK_IO                9
#define BK_THREAD_BLOCK_SLEEP             10
#define BK_THREAD_BLOCK_PAGEIN            11
#define BK_THREAD_BLOCK_PAGEOUT           12
#define BK_THREAD_BLOCK_WAIT              13
#define BK_THREAD_BLOCK_JOIN              14

/*
 * Sinais e notificações
 */
#define BK_SIG_MAX                        32
#define BK_SIG_BLOCK                      0
#define BK_SIG_UNBLOCK                    1
#define BK_SIG_SETMASK                     2

/*
 * Estrutura de contexto de hardware
 */
struct __bk_thread_context {
    BK_UL3264           ctx_regs[32];           /* Registradores gerais */
    BK_UL3264           ctx_pc;                  /* Program Counter */
    BK_UL3264           ctx_sp;                  /* Stack Pointer */
    BK_UL3264           ctx_fp;                  /* Frame Pointer */
    BK_UL3264           ctx_lr;                  /* Link Register */
    BK_UL3264           ctx_flags;               /* Flags do processador */
    BK_UL3264           ctx_fpu[64];             /* Estado da FPU/SIMD */
    BK_UL3264           ctx_vector[32];          /* Registradores vetoriais */
    BK_UL3264           ctx_tls;                 /* Thread Local Storage */
    BK_UL3264           ctx_syscall_num;         /* Número da syscall */
    BK_UL3264           ctx_syscall_args[6];     /* Argumentos da syscall */
    BK_UL3264           ctx_syscall_result;      /* Resultado da syscall */
    BK_UINT32           ctx_fpu_flags;           /* Flags da FPU */
    BK_UINT32           ctx_debug_regs[8];       /* Registradores de debug */
};

/*
 * Entidade de escalonamento (sched_entity)
 */
struct __bk_sched_entity {
    BK_UINT64           se_exec_start;           /* Início da execução */
    BK_UINT64           se_sum_exec_runtime;     /* Tempo total de execução */
    BK_UINT64           se_prev_sum_exec_runtime; /* Tempo anterior */
    BK_UINT64           se_last_update_time;     /* Última atualização */
    BK_UINT64           se_nr_migrations;        /* Número de migrações */
    
    BK_UINT32           se_depth;                 /* Profundidade na árvore */
    BK_UINT32           se_parent;                /* Entidade pai */
    
    struct __bk_sched_entity *se_parent_entity;
    struct __bk_sched_entity *se_prev_entity;
    struct __bk_sched_entity *se_next_entity;
    
    BK_LIST_ENTRY(__bk_sched_entity) se_group_node;
    BK_RB_NODE(__bk_sched_entity) se_rb_node;
    
    BK_UINT64           se_vruntime;              /* Virtual runtime */
    BK_UINT64           se_deadline;              /* Deadline */
    BK_UINT64           se_min_vruntime;          /* Mínimo vruntime */
    
    BK_UINT32           se_weight;                /* Peso para CFS */
    BK_UINT32           se_inv_weight;            /* Peso inverso */
    
    BK_UINT32           se_avg_per_period;        /* Média por período */
    BK_UINT32           se_avg_runnable;          /* Média runnable */
    BK_UINT32           se_avg_util;              /* Média de utilização */
    
    BK_BOOLEAN          se_on_rq;                 /* Na runqueue */
    BK_BOOLEAN          se_preempt;                /* Preemptível */
};

/*
 * Entidade de tempo real (sched_rt_entity)
 */
struct __bk_sched_rt_entity {
    BK_LIST_ENTRY(__bk_sched_rt_entity) rt_runq_node;
    BK_UINT32           rt_time;                   /* Tempo em RT */
    BK_UINT32           rt_max_time;               /* Tempo máximo */
    BK_UINT32           rt_period;                 /* Período */
    BK_UINT32           rt_runtime;                /* Runtime */
    BK_UINT32           rt_deadline;               /* Deadline */
    BK_UINT32           rt_priority;               /* Prioridade RT */
    BK_UINT32           rt_nr_cpus_allowed;        /* CPUs permitidas */
    
    BK_BOOLEAN          rt_throttled;              /* Throttled */
    BK_BOOLEAN          rt_queued;                  /* Na fila RT */
};

/*
 * Entidade de deadline (sched_dl_entity)
 */
struct __bk_sched_dl_entity {
    BK_UINT64           dl_runtime;                /* Runtime */
    BK_UINT64           dl_deadline;               /* Deadline */
    BK_UINT64           dl_period;                  /* Período */
    BK_UINT64           dl_budget;                  /* Budget */
    BK_UINT64           dl_remaining;               /* Budget restante */
    
    BK_UINT64           dl_density;                 /* Densidade */
    BK_UINT64           dl_vruntime;                /* Virtual runtime */
    BK_UINT64           dl_deadline_abs;           /* Deadline absoluto */
    
    BK_UINT32           dl_nr_cpus_allowed;        /* CPUs permitidas */
    BK_UINT32           dl_flags;                   /* Flags do deadline */
    
    BK_BOOLEAN          dl_throttled;               /* Throttled */
    BK_BOOLEAN          dl_boosted;                 /* Boosted */
    BK_BOOLEAN          dl_yielded;                 /* Yielded */
    
    BK_RB_NODE(__bk_sched_dl_entity) dl_rb_node;
};

/*
 * Classe de escalonamento
 */
struct __bk_sched_class {
    const char          *sc_name;                   /* Nome da classe */
    BK_UINT32           sc_id;                       /* ID da classe */
    
    void                (*sc_enqueue_task)(struct __bk_thread *thread);
    void                (*sc_dequeue_task)(struct __bk_thread *thread);
    void                (*sc_yield_task)(struct __bk_thread *thread);
    void                (*sc_check_preempt)(struct __bk_thread *thread);
    struct __bk_thread  *(*sc_pick_next_task)(void);
    void                (*sc_task_tick)(struct __bk_thread *thread);
    void                (*sc_task_fork)(struct __bk_thread *thread);
    void                (*sc_task_dead)(struct __bk_thread *thread);
    void                (*sc_set_priority)(struct __bk_thread *thread, BK_UINT8 prio);
    void                (*sc_get_priority)(struct __bk_thread *thread);
};

/*
 * Estrutura de estatísticas de escalonamento
 */
struct __bk_sched_stats {
    BK_UINT64           ss_wait_time;               /* Tempo de espera */
    BK_UINT64           ss_exec_time;               /* Tempo de execução */
    BK_UINT64           ss_block_time;              /* Tempo bloqueado */
    BK_UINT64           ss_sleep_time;              /* Tempo dormindo */
    BK_UINT64           ss_iowait_time;             /* Tempo em I/O */
    
    BK_UINT32           ss_nr_switches;             /* Número de switches */
    BK_UINT32           ss_nr_preemptions;          /* Número de preempções */
    BK_UINT32           ss_nr_migrations;           /* Número de migrações */
    BK_UINT32           ss_nr_wakeups;               /* Número de wakeups */
    BK_UINT32           ss_nr_yields;                /* Número de yields */
    
    BK_UINT64           ss_last_update;              /* Última atualização */
};

/*
 * Estrutura de informações de NUMA
 */
struct __bk_numa_info {
    BK_UINT32           ni_preferred_nid;           /* Nó preferido */
    BK_UINT32           ni_scan_seq;                 /* Sequência de scan */
    BK_UINT32           ni_scan_period;              /* Período de scan */
    BK_UINT64           ni_node_stamp;               /* Timestamp do nó */
    BK_UINT64           ni_faults[4];                /* Faults por tipo */
    BK_UINT64           ni_pages_migrated;           /* Páginas migradas */
    BK_UINT32           ni_migrate_retry;            /* Retry de migração */
};

/*
 * Bible Thread - Menor unidade de execução gerenciada pelo kernel
 * Estrutura principal que utiliza as estruturas auxiliares já definidas
 */

/*
 * Estados da thread
 */
#define BK_THREAD_STATE_NEW             0x0001  /* Thread criada */
#define BK_THREAD_STATE_READY           0x0002  /* Pronta para execução */
#define BK_THREAD_STATE_RUNNING         0x0004  /* Em execução */
#define BK_THREAD_STATE_BLOCKED         0x0008  /* Bloqueada */
#define BK_THREAD_STATE_SLEEPING        0x0010  /* Dormindo */
#define BK_THREAD_STATE_SUSPENDED       0x0020  /* Suspensa */
#define BK_THREAD_STATE_TERMINATED      0x0040  /* Terminada */
#define BK_THREAD_STATE_ZOMBIE          0x0080  /* Zombie */
#define BK_THREAD_STATE_DEAD            0x0100  /* Morta */
#define BK_THREAD_STATE_IDLE            0x0200  /* Idle */
#define BK_THREAD_STATE_STOPPED         0x0400  /* Parada */
#define BK_THREAD_STATE_TRACED          0x0800  /* Tracejada */

/*
 * Flags da thread
 */
#define BK_THREAD_FLAG_SYSTEM           0x00000001  /* Thread do sistema */
#define BK_THREAD_FLAG_USER             0x00000002  /* Thread do usuário */
#define BK_THREAD_FLAG_KERNEL           0x00000004  /* Thread do kernel */
#define BK_THREAD_FLAG_DAEMON           0x00000008  /* Thread daemon */
#define BK_THREAD_FLAG_IDLE             0x00000010  /* Thread idle */
#define BK_THREAD_FLAG_DETACHED         0x00000020  /* Thread detached */
#define BK_THREAD_FLAG_SIGNALED         0x00000040  /* Sinalizada */
#define BK_THREAD_FLAG_STOPPED          0x00000080  /* Parada */
#define BK_THREAD_FLAG_CONTINUED        0x00000100  /* Continuada */
#define BK_THREAD_FLAG_EXITING          0x00000200  /* Em saída */
#define BK_THREAD_FLAG_EXEC             0x00000400  /* Em exec */
#define BK_THREAD_FLAG_VFORK            0x00000800  /* Em vfork */
#define BK_THREAD_FLAG_PTRACED          0x00001000  /* Sendo tracejada */
#define BK_THREAD_FLAG_TRACING          0x00002000  /* Tracejando */
#define BK_THREAD_FLAG_NO_PREEMPT       0x00004000  /* Não preemptível */
#define BK_THREAD_FLAG_PREEMPT          0x00008000  /* Preemptível */
#define BK_THREAD_FLAG_MIGRATE          0x00010000  /* Migrando */
#define BK_THREAD_FLAG_BOUND            0x00020000  /* Bound a CPU */
#define BK_THREAD_FLAG_CRITICAL         0x00040000  /* Crítica */
#define BK_THREAD_FLAG_REALTIME         0x00080000  /* Tempo real */
#define BK_THREAD_FLAG_BOOSTED          0x00100000  /* Prioridade boostada */
#define BK_THREAD_FLAG_THROTTLED        0x00200000  /* Throttled */

/*
 * Políticas de escalonamento
 */
#define BK_SCHED_POLICY_NORMAL          0
#define BK_SCHED_POLICY_FIFO            1
#define BK_SCHED_POLICY_RR              2
#define BK_SCHED_POLICY_BATCH           3
#define BK_SCHED_POLICY_IDLE            4
#define BK_SCHED_POLICY_DEADLINE        5

/*
 * Sinais (máximo)
 */
#define BK_SIG_MAX                       32

/*
 * Estrutura principal da thread
 */
struct thread {
    /*
     * Identificação e estado básico
     */
    BK_ID                   thr_id;                 /* Thread ID (LWP ID) */
    BK_UINT32               thr_magic;              /* Magic number para debug */
#define BK_THREAD_MAGIC      0x54485244              /* "THRD" */
    
    BK_UINT16               thr_state;              /* Estado atual */
    BK_UINT16               thr_saved_state;        /* Estado salvo */
    BK_UINT32               thr_flags;              /* Flags da thread */
    BK_UINT32               thr_ptrace_flags;       /* Flags de ptrace */
    
    /*
     * Pilha e memória
     */
    void                    *thr_stack;             /* Pilha da thread */
    BK_SIZE                 thr_stack_size;         /* Tamanho da pilha */
    void                    *thr_kernel_stack;      /* Pilha do kernel */
    BK_SIZE                 thr_kernel_stack_size;  /* Tamanho da pilha do kernel */
    BK_REFCOUNT             thr_stack_refcnt;       /* Contagem de referências da pilha */
    
    /*
     * Contagem de referências
     */
    BK_REFCOUNT             thr_usage;              /* Contagem de referências */
    BK_UINT32               thr_refcount;           /* Contador de referências alternativo */
    
    /*
     * Contexto de hardware
     */
    struct __bk_thread_context  thr_hw_context;     /* Contexto de hardware */
    
    /*
     * Escalonamento - prioridades
     */
    BK_UINT8                thr_priority;           /* Prioridade atual */
    BK_UINT8                thr_static_priority;    /* Prioridade estática */
    BK_UINT8                thr_normal_priority;    /* Prioridade normal */
    BK_UINT8                thr_rt_priority;        /* Prioridade de tempo real */
    BK_UINT8                thr_effective_priority; /* Prioridade efetiva */
    
    BK_INT                  thr_on_cpu;             /* CPU atual (-1 se não executando) */
    BK_INT                  thr_on_rq;              /* Na runqueue */
    BK_INT                  thr_recent_used_cpu;    /* CPU usada recentemente */
    BK_INT                  thr_wake_cpu;           /* CPU para wakeup */
    
    /*
     * Wakeup e relacionamentos
     */
    struct thread      *thr_last_wakee;        /* Última thread que acordou esta */
    BK_UINT32               thr_wakee_flips;        /* Flips de wakee */
    BK_UINT64               thr_wakee_flip_decay_ts; /* Timestamp de decay */
    
    struct __bk_call_single_node thr_wake_entry;    /* Entrada de wakeup */
    
    /*
     * Entidades de escalonamento
     */
    struct __bk_sched_entity    thr_sched_entity;    /* Entidade CFS */
    struct __bk_sched_rt_entity thr_sched_rt_entity; /* Entidade RT */
    struct __bk_sched_dl_entity thr_sched_dl_entity; /* Entidade Deadline */
    
    const struct __bk_sched_class *thr_sched_class; /* Classe de escalonamento */
    
    /*
     * Política e afinidade
     */
    BK_UINT32               thr_policy;             /* Política de escalonamento */
    BK_UINT32               thr_nr_cpus_allowed;    /* Número de CPUs permitidas */
    BK_CPUMASK              thr_cpus_allowed;       /* Máscara de CPUs permitidas */
    BK_CPUMASK              *thr_cpus_ptr;          /* Ponteiro para máscara de CPUs */
    BK_CPUMASK              *thr_user_cpus_ptr;     /* Máscara de usuário */
    void                    *thr_migration_pending; /* Migração pendente */
    BK_UINT16               thr_migration_disabled; /* Migração desabilitada */
    BK_UINT16               thr_migration_flags;    /* Flags de migração */
    
    BK_UINT64               thr_max_allowed_capacity; /* Capacidade máxima permitida */
    
    /*
     * Estatísticas de escalonamento
     */
    struct __bk_sched_stats thr_sched_stats;        /* Estatísticas de escalonamento */
    
    /*
     * Listas de threads
     */
    BK_LIST_ENTRY(thread)  thr_task_link;      /* Link na task */
    BK_LIST_ENTRY(thread)  thr_global_link;    /* Link na lista global */
    BK_LIST_ENTRY(thread)  thr_cpu_link;       /* Link na CPU */
    BK_LIST_ENTRY(thread)  thr_sched_link;     /* Link no escalonador */
    BK_RB_NODE(thread)     thr_pushable_node;  /* Nó para pushable */
    
    /*
     * Relacionamentos
     */
    struct __bk_task            *thr_task;          /* Task proprietária */
    struct __bk_process         *thr_process;       /* Processo proprietário */
    struct thread          *thr_group_leader;  /* Líder do grupo */
    
    BK_LIST_ENTRY(thread)  thr_group_link;     /* Link no grupo */
    
    /*
     * Espaço de endereço
     */
    BK_VM_ADDRSPACE             *thr_addrspace;     /* Espaço de endereço */
    
    /*
     * Exit e status
     */
    BK_INT                  thr_exit_state;         /* Estado de saída */
    BK_INT                  thr_exit_code;          /* Código de saída */
    BK_INT                  thr_exit_signal;        /* Sinal de saída */
    BK_INT                  thr_pdeath_signal;      /* Sinal quando pai morre */
    
    /*
     * Sinais
     */
    BK_SIGSET               thr_sig_pending;        /* Sinais pendentes */
    BK_SIGSET               thr_sig_blocked;        /* Sinais bloqueados */
    BK_SIGSET               thr_sig_real_blocked;   /* Sinais realmente bloqueados */
    BK_SIGSET               thr_sig_saved;          /* Sinais salvos */
    
    void                    (*thr_sig_handlers[BK_SIG_MAX])(BK_INT); /* Handlers */
    
    BK_UL3264               thr_sas_ss_sp;          /* Stack para signal handler */
    BK_SIZE                 thr_sas_ss_size;        /* Tamanho da stack */
    BK_UINT32               thr_sas_ss_flags;       /* Flags da stack */
    
    /*
     * Credenciais
     */
    BK_UID                  thr_uid;                /* User ID */
    BK_UID                  thr_euid;               /* Effective UID */
    BK_UID                  thr_suid;               /* Saved UID */
    BK_GID                  thr_gid;                /* Group ID */
    BK_GID                  thr_egid;               /* Effective GID */
    BK_GID                  thr_sgid;               /* Saved GID */
    
    /*
     * Nome e identificação
     */
    char                    thr_comm[BK_TASK_COMM_LEN]; /* Nome do comando */
    
    /*
     * Timers e tempo
     */
    BK_TIME                 thr_utime;              /* User time */
    BK_TIME                 thr_stime;              /* System time */
    BK_TIME                 thr_start_time;         /* Start time */
    BK_TIME                 thr_start_boottime;     /* Start time (boot) */
    
    BK_UINT64               thr_timer_slack_ns;     /* Timer slack */
    BK_UINT64               thr_default_timer_slack_ns; /* Default timer slack */
    
    /*
     * Page faults
     */
    BK_UL3264               thr_min_flt;            /* Minor faults */
    BK_UL3264               thr_maj_flt;            /* Major faults */
    
    /*
     * Context switch counts
     */
    BK_UL3264               thr_nvcsw;              /* Voluntary switches */
    BK_UL3264               thr_nivcsw;             /* Involuntary switches */
    
    /*
     * Locks e sincronização
     */
    BK_SPINLOCK             thr_alloc_lock;         /* Lock de alocação */
    BK_SPINLOCK             thr_pi_lock;            /* Lock de PI */
    
    struct __bk_wake_q_node thr_wake_q;             /* Wake queue node */
    
    /*
     * PI (Priority Inheritance) data
     */
    struct __bk_mutex       *thr_blocked_on;        /* Mutex bloqueado */
    BK_UINT32               thr_pi_state;           /* Estado PI */
    BK_UINT32               thr_pi_flags;           /* Flags PI */
    
    /*
     * Wait queue
     */
    void                    *thr_waitq;             /* Wait queue (tipo opaco) */
    BK_UINT64               thr_wait_event;         /* Evento de espera */
    BK_UINT32               thr_block_reason;       /* Razão do bloqueio */
    
    /*
     * NUMA
     */
    struct __bk_numa_info   thr_numa;               /* Informações NUMA */
    
    /*
     * RSEQ (Restartable Sequences)
     */
    struct __bk_rseq        thr_rseq;               /* RSEQ data */
    
    /*
     * Atividades
     */
    struct activity    *thr_current_activity;  /* Atividade atual */
    BK_ACTIVITY_LIST        thr_activity_queue;     /* Fila de atividades */
    BK_UINT32               thr_activity_count;     /* Contador de atividades */
    
    /*
     * Dados do usuário e callbacks
     */
    void                    *thr_private;           /* Dados privados */
    BK_UINT32               thr_private_size;       /* Tamanho dos dados privados */
    
    void                    (*thr_on_start)(struct thread *);
    void                    (*thr_on_exit)(struct thread *);
    void                    (*thr_on_block)(struct thread *, BK_UINT32 reason);
    void                    (*thr_on_wake)(struct thread *);
    void                    (*thr_on_migrate)(struct thread *, BK_INT from_cpu, BK_INT to_cpu);
    
    void                    *thr_callback_data;     /* Dados para callbacks */
    
    /*
     * Task work
     */
    struct __bk_callback_head *thr_task_works;      /* Task works */
    
    /*
     * Restart block
     */
    struct __bk_restart_block thr_restart_block;    /* Restart block */
};

/*
 * Cabeças de lista para threads
 */
BK_LIST_HEAD(__bk_thread_list, thread);
BK_TAILQ_HEAD(__bk_thread_queue, thread);
BK_RB_HEAD(__bk_thread_rb, thread);

/*
 * Estrutura para estatísticas globais de threads
 */
struct __bk_thread_stats {
    BK_UINT64               ts_total_threads;
    BK_UINT64               ts_running_threads;
    BK_UINT64               ts_blocked_threads;
    BK_UINT64               ts_sleeping_threads;
    BK_UINT64               ts_zombie_threads;
    BK_UINT64               ts_total_switches;
    BK_UINT64               ts_total_migrations;
    BK_UINT64               ts_total_preemptions;
    BK_UINT64               ts_total_wakeups;
    BK_UINT64               ts_total_cpu_time;
};

/*
 * Macros úteis
 */
#define BK_THREAD_IS_RUNNING(thr)   ((thr)->thr_state & BK_THREAD_STATE_RUNNING)
#define BK_THREAD_IS_BLOCKED(thr)   ((thr)->thr_state & BK_THREAD_STATE_BLOCKED)
#define BK_THREAD_IS_SLEEPING(thr)  ((thr)->thr_state & BK_THREAD_STATE_SLEEPING)
#define BK_THREAD_IS_TERMINATED(thr) ((thr)->thr_state & BK_THREAD_STATE_TERMINATED)

#define BK_THREAD_HAS_FLAG(thr, flag)  (((thr)->thr_flags & (flag)) != 0)
#define BK_THREAD_SET_FLAG(thr, flag)  ((thr)->thr_flags |= (flag))
#define BK_THREAD_CLEAR_FLAG(thr, flag) ((thr)->thr_flags &= ~(flag))

#define BK_THREAD_PRIO(thr)            ((thr)->thr_priority)
#define BK_THREAD_STATIC_PRIO(thr)     ((thr)->thr_static_priority)

#endif
