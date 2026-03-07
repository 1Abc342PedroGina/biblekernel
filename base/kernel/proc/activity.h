# ifndef _PROC_ACTIVITY_H
# define _PROC_ACTIVITY_H

# include <bible/task.h>
# include <bible/vm.h>
# include <bible/ipc.h>
# include <bible/interrupt.h>
# include <bible/exception.h>
# include <bible/kernl_object.h>
# include <bible/ktypes.h>

/*
 * Estados da atividade
 */
#define BK_ACTIVITY_STATE_READY         0x0001  /* Pronta para execução */
#define BK_ACTIVITY_STATE_RUNNING        0x0002  /* Em execução */
#define BK_ACTIVITY_STATE_BLOCKED        0x0004  /* Bloqueada */
#define BK_ACTIVITY_STATE_SUSPENDED      0x0008  /* Suspensa */
#define BK_ACTIVITY_STATE_COMPLETED      0x0010  /* Completada */
#define BK_ACTIVITY_STATE_FAILED         0x0020  /* Falhou */
#define BK_ACTIVITY_STATE_CANCELLED      0x0040  /* Cancelada */
#define BK_ACTIVITY_STATE_WAITING        0x0080  /* Esperando evento */
#define BK_ACTIVITY_STATE_TIMEDOUT       0x0100  /* Timeout expirado */
#define BK_ACTIVITY_STATE_MIGRATING      0x0200  /* Migrando entre threads */
#define BK_ACTIVITY_STATE_DEFERRED       0x0400  /* Adiada */

/*
 * Flags da atividade
 */
#define BK_ACTIVITY_FLAG_SYSTEM          0x00000001  /* Atividade do sistema */
#define BK_ACTIVITY_FLAG_USER            0x00000002  /* Atividade do usuário */
#define BK_ACTIVITY_FLAG_CRITICAL        0x00000004  /* Atividade crítica */
#define BK_ACTIVITY_FLAG_REALTIME        0x00000008  /* Tempo real */
#define BK_ACTIVITY_FLAG_IDLE             0x00000010  /* Idle */
#define BK_ACTIVITY_FLAG_BACKGROUND      0x00000020  /* Background */
#define BK_ACTIVITY_FLAG_FOREGROUND      0x00000040  /* Foreground */
#define BK_ACTIVITY_FLAG_DETACHED         0x00000080  /* Desanexada */
#define BK_ACTIVITY_FLAG_PERSISTENT      0x00000100  /* Persistente */
#define BK_ACTIVITY_FLAG_NO_PREEMPT      0x00000200  /* Não preemptível */
#define BK_ACTIVITY_FLAG_NO_MIGRATE      0x00000400  /* Não migrar */
#define BK_ACTIVITY_FLAG_INHERIT_PRIO    0x00000800  /* Herdar prioridade */
#define BK_ACTIVITY_FLAG_BOOSTED          0x00001000  /* Prioridade boostada */
#define BK_ACTIVITY_FLAG_THROTTLED        0x00002000  /* Acelerada */
#define BK_ACTIVITY_FLAG_WAITQ_PROMOTED  0x00004000  /* Promovida por waitq */
#define BK_ACTIVITY_FLAG_OVERCOMMIT      0x00008000  /* Overcommit permitido */

/*
 * Tipos de atividade
 */
#define BK_ACTIVITY_TYPE_GENERIC          0
#define BK_ACTIVITY_TYPE_COMPUTATION      1
#define BK_ACTIVITY_TYPE_IO                2
#define BK_ACTIVITY_TYPE_SYSCALL          3
#define BK_ACTIVITY_TYPE_SIGNAL            4
#define BK_ACTIVITY_TYPE_INTERRUPT        5
#define BK_ACTIVITY_TYPE_TIMER            6
#define BK_ACTIVITY_TYPE_IPC              7
#define BK_ACTIVITY_TYPE_USER             8

/*
 * Razões de bloqueio
 */
#define BK_ACTIVITY_BLOCK_NONE            0
#define BK_ACTIVITY_BLOCK_MUTEX            1
#define BK_ACTIVITY_BLOCK_SEMAPHORE        2
#define BK_ACTIVITY_BLOCK_CONDVAR          3
#define BK_ACTIVITY_BLOCK_RWLOCK           4
#define BK_ACTIVITY_BLOCK_QUEUE            5
#define BK_ACTIVITY_BLOCK_PIPE             6
#define BK_ACTIVITY_BLOCK_EVENT            7
#define BK_ACTIVITY_BLOCK_IPC              8
#define BK_ACTIVITY_BLOCK_IO                9
#define BK_ACTIVITY_BLOCK_SLEEP            10
#define BK_ACTIVITY_BLOCK_PAGEIN           11
#define BK_ACTIVITY_BLOCK_PAGEOUT          12

/*
 * Resultados de wait
 */
#define BK_ACTIVITY_WAIT_SUCCESS          0
#define BK_ACTIVITY_WAIT_TIMEOUT          1
#define BK_ACTIVITY_WAIT_INTERRUPTED      2
#define BK_ACTIVITY_WAIT_ERROR            3

/*
 * Prioridades
 */
#define BK_ACTIVITY_PRIO_IDLE              0
#define BK_ACTIVITY_PRIO_LOW               32
#define BK_ACTIVITY_PRIO_NORMAL            64
#define BK_ACTIVITY_PRIO_HIGH              96
#define BK_ACTIVITY_PRIO_REALTIME          127
#define BK_ACTIVITY_PRIO_DEFAULT           BK_ACTIVITY_PRIO_NORMAL

/*
 * Estrutura principal da atividade
 */
struct __bk_activity {
    /*
     * Identificação e estado básico
     */
    BK_ID                   act_id;                 /* ID único da atividade */
    BK_UINT32               act_magic;              /* Magic number para debug */
#define BK_ACTIVITY_MAGIC    0xAC71V17A              /* "ACTIVITY" */
    
    BK_UINT8                act_type;               /* Tipo de atividade */
    BK_UINT16               act_state;              /* Estado atual (flags) */
    BK_UINT32               act_flags;              /* Flags da atividade */
    BK_UINT8                act_priority;           /* Prioridade atual */
    BK_UINT8                act_base_priority;      /* Prioridade base */
    BK_UINT8                act_max_priority;       /* Prioridade máxima */
    BK_UINT8                act_boost_priority;     /* Prioridade boostada */
    
    /*
     * Controle de execução
     */
    void                    (*act_handler)(void *); /* Função handler */
    void                    *act_data;               /* Dados da atividade */
    BK_SIZE                 act_data_size;           /* Tamanho dos dados */
    
    BK_VM_OFFSET            act_stack;               /* Pilha da atividade */
    BK_SIZE                 act_stack_size;          /* Tamanho da pilha */
    BK_VM_OFFSET            act_context;             /* Contexto salvo */
    void                    *act_continuation;       /* Continuação após block */
    void                    *act_parameter;          /* Parâmetro para continuação */
    
    /*
     * Relacionamentos
     */
    struct __bk_thread      *act_thread;             /* Thread executando */
    struct __bk_task        *act_task;               /* Task proprietária */
    struct __bk_process     *act_process;            /* Processo proprietário */
    BK_ID                   act_creator;             /* ID do criador */
    BK_ID                   act_parent;              /* ID da atividade pai */
    
    /*
     * Filas e listas (usando macros do queue.h)
     */
    BK_LIST_ENTRY(__bk_activity)   act_task_link;    /* Link na task */
    BK_LIST_ENTRY(__bk_activity)   act_thread_link;  /* Link na thread */
    BK_TAILQ_ENTRY(__bk_activity)  act_sched_link;   /* Link no escalonador */
    BK_TAILQ_ENTRY(__bk_activity)  act_wait_link;    /* Link em wait queue */
    
    /*
     * Wait queue e bloqueio
     */
    void                    *act_waitq;              /* Wait queue (tipo opaco) */
    BK_UINT64               act_wait_event;          /* Evento de espera */
    BK_UINT8                act_block_reason;        /* Razão do bloqueio */
    BK_UINT8                act_wait_result;         /* Resultado do wait */
    BK_BOOLEAN              act_wake_active;         /* Wake em andamento */
    BK_BOOLEAN              act_timedwait_active;    /* Timedwait ativo */
    
    /*
     * Timers e tempo
     */
    BK_TIME                 act_creation_time;       /* Tempo de criação */
    BK_TIME                 act_start_time;          /* Tempo de início */
    BK_TIME                 act_completion_time;     /* Tempo de conclusão */
    BK_TIME                 act_block_time;          /* Tempo de bloqueio */
    BK_TIME                 act_wakeup_time;         /* Tempo de wakeup */
    BK_TIME                 act_expiry_time;         /* Tempo de expiração */
    BK_TIME                 act_deadline;            /* Deadline */
    
    BK_UINT64               act_cpu_cycles;          /* Ciclos consumidos */
    BK_UINT64               act_cpu_time;            /* Tempo de CPU */
    BK_UINT64               act_wait_time;           /* Tempo em wait */
    BK_UINT64               act_last_run_time;       /* Último tempo de execução */
    
    void                    *act_timer;               /* Timer associado */
    BK_BOOLEAN              act_timer_active;        /* Timer ativo */
    BK_BOOLEAN              act_timer_armed;         /* Timer armado */
    
    /*
     * Resultado e erro
     */
    BK_I32                  act_result;              /* Resultado da atividade */
    BK_I32                  act_error;               /* Código de erro */
    
    /*
     * Estatísticas e contadores
     */
    BK_UINT32               act_switch_count;        /* Número de context switches */
    BK_UINT32               act_migration_count;     /* Número de migrações */
    BK_UINT32               act_preempt_count;       /* Número de preempções */
    BK_UINT32               act_wakeup_count;        /* Número de wakeups */
    BK_UINT32               act_block_count;         /* Número de bloqueios */
    BK_UINT32               act_yield_count;         /* Número de yields */
    
    BK_UINT64               act_cycles_total;        /* Ciclos totais */
    BK_UINT64               act_cycles_user;         /* Ciclos em user */
    BK_UINT64               act_cycles_system;       /* Ciclos em kernel */
    BK_UINT64               act_cycles_io;           /* Ciclos em I/O */
    
    /*
     * Sincronização
     */
    BK_SPINLOCK             act_lock;                /* Lock principal */
    BK_SPINLOCK             act_sched_lock;          /* Lock do escalonador */
    BK_SPINLOCK             act_wake_lock;           /* Lock para wake */
    
    BK_SEMAPHORE            act_completion_sem;      /* Semáforo de conclusão */
    BK_MUTEX                act_mutex;               /* Mutex interno */
    BK_CONDVAR              act_cond;                /* Condition variable */
    
    /*
     * Herança e boosting
     */
    struct __bk_activity    *act_inheritor;          /* Herdeiro de prioridade */
    BK_UINT8                act_inheritor_prio;      /* Prioridade do herdeiro */
    BK_UINT32               act_inheritor_flags;     /* Flags do herdeiro */
    
    BK_UINT32               act_boost_refcount;      /* Contador de boosts */
    BK_UINT32               act_boost_stack[8];      /* Stack de boosts */
    BK_UINT8                act_boost_current;       /* Boost atual */
    
    /*
     * Wait queue específico
     */
    union {
        /* Para wait em mutex */
        struct {
            BK_MUTEX        *wq_mutex;
        } mutex_wait;
        
        /* Para wait em semáforo */
        struct {
            BK_SEMAPHORE    *wq_sem;
        } sem_wait;
        
        /* Para wait em condvar */
        struct {
            BK_CONDVAR      *wq_cond;
        } cond_wait;
        
        /* Para wait em queue */
        struct {
            struct __bk_queue *wq_queue;
            BK_UINT32       wq_flags;
        } queue_wait;
        
        /* Para wait em IPC */
        struct {
            BK_ID           wq_ipc_obj;
            BK_SIZE         wq_msg_size;
        } ipc_wait;
        
        /* Para wait em I/O */
        struct {
            BK_ID           wq_fd;
            BK_UINT32       wq_events;
        } io_wait;
    } act_wait_data;
    
    /*
     * Dados para continuação
     */
    union {
        struct {
            void            *cont_data;
            BK_SIZE         cont_size;
            BK_UINT32       cont_flags;
        } generic;
        
        struct {
            BK_ID           msg_id;
            void            *msg_buffer;
            BK_SIZE         msg_size;
        } ipc;
        
        struct {
            BK_ID           fd;
            void            *buffer;
            BK_SIZE         count;
            BK_OFFSET       offset;
        } io;
        
        struct {
            BK_UINT64       key;
            void            *value;
        } hash;
        
        struct {
            BK_ID           timer_id;
            BK_TIME         expiry;
        } timer;
    } act_continuation_data;
    
    /*
     * Preempção e yield
     */
    BK_UINT32               act_preempt_remaining;   /* Quantum restante */
    BK_UINT32               act_yield_remaining;     /* Yields restantes */
    BK_UINT32               act_slice_remaining;     /* Slice restante */
    
    BK_BOOLEAN              act_preempt_requested;   /* Preempção solicitada */
    BK_BOOLEAN              act_yield_requested;     /* Yield solicitado */
    BK_BOOLEAN              act_ast_pending;         /* AST pendente */
    
    BK_UINT32               act_ast_flags;           /* Flags de AST */
#define BK_ACTIVITY_AST_PREEMPT         0x0001
#define BK_ACTIVITY_AST_YIELD           0x0002
#define BK_ACTIVITY_AST_SIGNAL          0x0004
#define BK_ACTIVITY_AST_TIMER            0x0008
#define BK_ACTIVITY_AST_URGENT          0x0010
#define BK_ACTIVITY_AST_INTERRUPT       0x0020
    
    /*
     * Callbacks e hooks
     */
    void                    (*act_on_start)(struct __bk_activity *);
    void                    (*act_on_complete)(struct __bk_activity *);
    void                    (*act_on_block)(struct __bk_activity *, BK_UINT8 reason);
    void                    (*act_on_wake)(struct __bk_activity *);
    void                    (*act_on_migrate)(struct __bk_activity *, struct __bk_thread *from, struct __bk_thread *to);
    void                    (*act_on_preempt)(struct __bk_activity *);
    void                    (*act_on_yield)(struct __bk_activity *);
    void                    (*act_on_timeout)(struct __bk_activity *);
    
    void                    *act_callback_data;      /* Dados para callbacks */
    
    /*
     * Contagem de referências
     */
    BK_REFCOUNT             act_refcnt;              /* Contador de referências */
    BK_UINT32               act_refcnt_debug;        /* Debug counter */
    
    /*
     * Limites e restrições
     */
    BK_RLIM                 act_cpu_limit;           /* Limite de CPU */
    BK_RLIM                 act_time_limit;          /* Limite de tempo */
    BK_RLIM                 act_memory_limit;        /* Limite de memória */
    BK_RLIM                 act_stack_limit;         /* Limite de pilha */
    
    BK_TIME                 act_start_limit;         /* Tempo limite de início */
    BK_TIME                 act_duration_limit;      /* Duração limite */
    
    /*
     * Depuração e tracing
     */
#ifdef BK_ACTIVITY_DEBUG
    BK_UINT32               act_debug_flags;
    BK_UINT64               act_debug_data[16];
    void                    *act_debug_stack[32];    /* Stack trace */
    BK_UINT32               act_debug_stack_depth;
    BK_ID                   act_debug_owner;         /* Dono do debug */
#endif
    
    /*
     * Dados privados e extensões
     */
    void                    *act_private;            /* Dados privados */
    BK_UINT32               act_private_size;        /* Tamanho dos dados privados */
    
    /*
     * Lista de recursos associados
     */
    BK_LIST_HEAD(__bk_activity_resources) act_resources;
    
    /*
     * Vetor de extensões
     */
    BK_UINT32               act_extension_count;
    void                    **act_extensions;
};

typedef struct __bk_activity BK_ACTIVITY;

/*
 * Cabeças de lista para atividades
 */
BK_LIST_HEAD(__bk_activity_list, __bk_activity);
BK_TAILQ_HEAD(__bk_activity_queue, __bk_activity);

/*
 * Estatísticas da atividade
 */
struct __bk_activity_stats {
    BK_UINT64               as_total_activities;
    BK_UINT64               as_active_activities;
    BK_UINT64               as_completed_activities;
    BK_UINT64               as_failed_activities;
    BK_UINT64               as_cancelled_activities;
    BK_UINT64               as_total_cpu_time;
    BK_UINT64               as_total_wait_time;
    BK_UINT64               as_total_switches;
    BK_UINT64               as_total_migrations;
    BK_UINT64               as_total_preempts;
    BK_UINT64               as_total_yields;
};

/*
 * Macros úteis
 */
#define BK_ACTIVITY_IS_READY(act)       ((act)->act_state & BK_ACTIVITY_STATE_READY)
#define BK_ACTIVITY_IS_RUNNING(act)     ((act)->act_state & BK_ACTIVITY_STATE_RUNNING)
#define BK_ACTIVITY_IS_BLOCKED(act)     ((act)->act_state & BK_ACTIVITY_STATE_BLOCKED)
#define BK_ACTIVITY_IS_COMPLETED(act)   ((act)->act_state & BK_ACTIVITY_STATE_COMPLETED)
#define BK_ACTIVITY_IS_FAILED(act)      ((act)->act_state & BK_ACTIVITY_STATE_FAILED)

#define BK_ACTIVITY_HAS_TIMER(act)       ((act)->act_timer != NULL)
#define BK_ACTIVITY_HAS_BOOST(act)       ((act)->act_boost_refcount > 0)
#define BK_ACTIVITY_IS_CRITICAL(act)    ((act)->act_flags & BK_ACTIVITY_FLAG_CRITICAL)
#define BK_ACTIVITY_IS_REALTIME(act)    ((act)->act_flags & BK_ACTIVITY_FLAG_REALTIME)

#define BK_ACTIVITY_PRIO(act)            ((act)->act_priority)
#define BK_ACTIVITY_BASE_PRIO(act)       ((act)->act_base_priority)
#define BK_ACTIVITY_EFFECTIVE_PRIO(act)  (BK_ACTIVITY_HAS_BOOST(act) ? \
                                          (act)->act_boost_priority : (act)->act_priority)

/*
 * Funções principais
 */
BK_ACTIVITY *bk_activity_create(BK_UINT8 type, void (*handler)(void *), 
                                void *data, BK_SIZE data_size, BK_UINT8 priority);
BK_I32 bk_activity_destroy(BK_ACTIVITY *act);
BK_I32 bk_activity_retain(BK_ACTIVITY *act);
BK_I32 bk_activity_release(BK_ACTIVITY *act);

BK_I32 bk_activity_start(BK_ACTIVITY *act, struct __bk_thread *thread);
BK_I32 bk_activity_cancel(BK_ACTIVITY *act);
BK_I32 bk_activity_wait(BK_ACTIVITY *act, BK_TIME timeout);
BK_I32 bk_activity_yield(BK_ACTIVITY *act);
BK_I32 bk_activity_suspend(BK_ACTIVITY *act);
BK_I32 bk_activity_resume(BK_ACTIVITY *act);

BK_I32 bk_activity_set_priority(BK_ACTIVITY *act, BK_UINT8 priority);
BK_I32 bk_activity_boost_priority(BK_ACTIVITY *act, BK_UINT8 boost);
BK_I32 bk_activity_unboost_priority(BK_ACTIVITY *act);
BK_UINT8 bk_activity_get_priority(BK_ACTIVITY *act);

BK_I32 bk_activity_set_deadline(BK_ACTIVITY *act, BK_TIME deadline);
BK_I32 bk_activity_set_timeout(BK_ACTIVITY *act, BK_TIME timeout);
BK_I32 bk_activity_clear_timeout(BK_ACTIVITY *act);

BK_I32 bk_activity_get_result(BK_ACTIVITY *act);
BK_I32 bk_activity_get_error(BK_ACTIVITY *act);
BK_UINT16 bk_activity_get_state(BK_ACTIVITY *act);

BK_BOOLEAN bk_activity_is_completed(BK_ACTIVITY *act);
BK_BOOLEAN bk_activity_is_running(BK_ACTIVITY *act);
BK_BOOLEAN bk_activity_is_blocked(BK_ACTIVITY *act);

void bk_activity_get_stats(BK_ACTIVITY *act, struct __bk_activity_stats *stats);
