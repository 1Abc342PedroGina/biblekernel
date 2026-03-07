/*
 * Bible Task - Ambiente de execução para threads
 * Inspirada na struct task do XNU
 */


# include <bible/task.h>
# include <bible/vm.h>
# include <bible/ipc.h>
# include <bible/interrupt.h>
# include <bible/exception.h>
# include <proc/activity.h>
# include <bible/kernl_object.h>
# include <bible/ktypes.h>
# include <proc/thread.h>

/*
 * Flags da task (protegidas por task_lock)
 */
#define BK_TASK_FLAG_NONE               0x00000000
#define BK_TASK_FLAG_64BIT_ADDR          0x00000001  /* Task com endereçamento 64-bit */
#define BK_TASK_FLAG_64BIT_DATA          0x00000002  /* Task com dados 64-bit */
#define BK_TASK_FLAG_CPU_WARNING         0x00000004  /* Threads em zona de aviso de CPU */
#define BK_TASK_FLAG_WAKE_WARNING        0x00000008  /* Em zona de aviso de wakeups */
#define BK_TASK_FLAG_GPU_DENIED          0x00000010  /* Acesso à GPU negado */
#define BK_TASK_FLAG_PENDING_CORPSE      0x00000020  /* Corpse não reportado */
#define BK_TASK_FLAG_CORPSE_FORK         0x00000040  /* Task é um corpse forked */
#define BK_TASK_FLAG_DARKWAKE_MODE       0x00000080  /* Task em modo darkwake */
#define BK_TASK_FLAG_NO_SMT               0x00000100  /* Threads não devem usar SMT */
#define BK_TASK_FLAG_COALITION_MEMBER    0x00000200  /* Membro de coalizão */
#define BK_TASK_FLAG_NO_CORPSE_FORK      0x00000400  /* Não criar corpse */
#define BK_TASK_FLAG_HAS_PROC             0x00000800  /* Task aponta para um proc */
#define BK_TASK_FLAG_GAME_MODE            0x00001000  /* Modo game ativo */
#define BK_TASK_FLAG_CARPLAY_MODE         0x00002000  /* Modo CarPlay ativo */
#define BK_TASK_FLAG_TELEMETRY           0x00004000  /* Participante de telemetria */

/*
 * Flags protegidas por proc_lock
 */
#define BK_TASK_PROC_FLAG_NONE           0x00000000
#define BK_TASK_PROC_FLAG_DID_EXEC       0x00000001  /* Task executou exec */
#define BK_TASK_PROC_FLAG_EXEC_COPY      0x00000002  /* Task é cópia de exec */

/*
 * Tipos de coalizão
 */
#define BK_COALITION_NUM_TYPES            3
#define BK_COALITION_TYPE_LEADER          0
#define BK_COALITION_TYPE_MEMBER          1
#define BK_COALITION_TYPE_EXT             2

/*
 * Portos de task
 */
#define BK_TASK_SELF_PORT_COUNT           4
#define BK_TASK_PORT_REGISTER_MAX          8
#define BK_TASK_PORT_DYLD_MAX              8

/*
 * Ações de exceção
 */
#define BK_EXC_TYPES_COUNT                12

/*
 * Comportamento de guarda de exceção
 */
typedef enum {
    BK_EXC_GUARD_NONE           = 0,
    BK_EXC_GUARD_HARD            = 1,
    BK_EXC_GUARD_SOFT            = 2,
    BK_EXC_GUARD_FATAL           = 3,
} BK_EXC_GUARD_BEHAVIOR;

/*
 * Configuração de segurança
 */
struct __bk_task_security_config {
    BK_BOOLEAN          tsc_hardened;           /* Task hardened */
    BK_BOOLEAN          tsc_restricted;         /* Task restricted */
    BK_UINT32           tsc_flags;               /* Flags de segurança */
};

/*
 * Estatísticas de QoS de CPU
 */
struct __bk_cpu_time_qos_stats {
    BK_UINT64           ctqs_user_time;          /* Tempo user por QoS */
    BK_UINT64           ctqs_system_time;        /* Tempo system por QoS */
    BK_UINT32           ctqs_run_count;          /* Contagem de execuções */
};

/*
 * Ação de exceção
 */
struct __bk_exception_action {
    BK_ID               ea_port;                 /* Porto para exceção */
    BK_UINT32           ea_mask;                  /* Máscara de exceções */
    BK_UINT32           ea_behavior;              /* Comportamento */
    BK_UINT32           ea_flavor;                /* Flavor da exceção */
};

/*
 * Ação de exceção hardened
 */
struct __bk_hardened_exception_action {
    BK_ID               hea_port;                 /* Porto para exceção */
    BK_UINT32           hea_flags;                 /* Flags */
};

/*
 * Estatísticas de writes
 */
struct __bk_task_writes_counters {
    BK_UINT64           twc_bytes;                 /* Bytes escritos */
    BK_UINT32           twc_operations;            /* Operações de escrita */
    BK_UINT32           twc_files;                 /* Arquivos escritos */
};

/*
 * Estrutura principal da task
 */
struct task {
    /*
     * Identificação e sincronização
     */
    BK_ID                   task_id;                 /* ID único da task */
    BK_UINT32               task_magic;              /* Magic number para debug */
#define BK_TASK_MAGIC        0x5441534B               /* "TASK" */
    
    BK_SPINLOCK             task_lock;                /* Lock principal */
    BK_REFCOUNT             task_refcnt;              /* Contagem de referências */
    
    BK_BOOLEAN              task_active;              /* Task não terminada */
    BK_BOOLEAN              task_ipc_active;          /* IPC permitido */
    BK_BOOLEAN              task_halting;             /* Task sendo parada */
    BK_BOOLEAN              task_pidsuspended;        /* PID suspendido */
    
    /*
     * Timers virtuais
     */
    BK_UINT32               task_vtimers;              /* Timers virtuais */
    BK_UINT32               task_load_tag;             /* Tag para logging */
    
    /*
     * Identificadores únicos
     */
    BK_UINT64               task_unique_id;            /* ID único global */
    
    /*
     * Espaço de endereço
     */
    BK_VM_ADDRSPACE         *task_addrspace;           /* Espaço de endereço virtual */
    
    /*
     * Listas
     */
    BK_LIST_ENTRY(__bk_task) task_global_link;         /* Link na lista global */
    BK_LIST_ENTRY(__bk_task) task_corpse_link;         /* Link na lista de corpses */
    
    /*
     * Threads na task
     */
    BK_LIST_HEAD(__bk_thread_list) task_threads;       /* Lista de threads */
    BK_UINT32               task_thread_count;          /* Número total de threads */
    BK_UINT32               task_active_thread_count;   /* Threads ativas */
    BK_INT                  task_suspend_count;         /* Contagem de suspensão */
    
    /*
     * Escalonamento
     */
    BK_INT                  task_priority;              /* Prioridade base para threads */
    BK_INT                  task_max_priority;          /* Prioridade máxima */
    BK_INT                  task_importance;            /* Offset de prioridade (nice) */
    
    /*
     * Paradas (user-level)
     */
    BK_INT                  task_user_stop_count;       /* Stops pendentes do usuário */
    BK_INT                  task_legacy_stop_count;     /* Stops legacy pendentes */
    
    /*
     * Estatísticas
     */
    BK_UINT64               task_total_runnable_time;   /* Tempo total runnable */
    
    BK_UINT64               task_faults;                /* Contador de faults */
    BK_UINT64               task_pageins;               /* Pageins */
    BK_UINT64               task_cow_faults;            /* Copy-on-write faults */
    BK_UINT64               task_messages_sent;         /* Mensagens enviadas */
    BK_UINT64               task_messages_received;     /* Mensagens recebidas */
    BK_UINT32               task_decompressions;        /* Decompressões */
    BK_UINT32               task_syscalls_mach;         /* System calls Mach */
    BK_UINT32               task_syscalls_unix;         /* System calls Unix */
    BK_UINT32               task_context_switches;      /* Context switches */
    BK_UINT32               task_processor_switches;    /* Processor switches */
    BK_UINT32               task_pset_switches;         /* Pset switches */
    
    /*
     * Estruturas IPC
     */
    BK_SPINLOCK             task_ipc_lock;              /* Lock para IPC */
    
    BK_ID                   task_ports[BK_TASK_SELF_PORT_COUNT];  /* Portos da task */
    BK_ID                   task_settable_self;         /* Send right */
    
    struct __bk_exception_action task_exc_actions[BK_EXC_TYPES_COUNT];
    struct __bk_hardened_exception_action task_hardened_exc_action;
    
    BK_ID                   task_host_port;              /* Send right para host */
    BK_ID                   task_bootstrap_port;         /* Send right para bootstrap */
    BK_ID                   task_debug_control_port;     /* Para debug */
    BK_ID                   task_access_port;            /* Porto de acesso */
    BK_ID                   task_resume_port;            /* Receive right para resume */
    BK_ID                   task_registered_ports[BK_TASK_PORT_REGISTER_MAX];
    BK_ID                   *task_dyld_notify_ports;     /* Array de ports para dyld */
    BK_ID                   task_resource_notify_port;   /* Porto para notificação de recursos */
    BK_ID                   task_ipc_space;              /* Espaço IPC */
    
    /*
     * Sincronizadores
     */
    BK_LIST_HEAD(__bk_semaphore_list) task_semaphore_list;  /* Semáforos owned */
    BK_INT                  task_semaphores_owned;          /* Número de semáforos */
    
    /*
     * Flags privilegiadas
     */
    BK_UINT32               task_priv_flags;
#define BK_TASK_PRIV_VM_BACKING_STORE   0x0001
    
    /*
     * Flags gerais
     */
    BK_UINT32               task_flags;                    /* Protegido por task_lock */
    BK_UINT32               task_proc_flags;               /* Protegido por proc_lock */
    
    /*
     * Informações de imagem
     */
    BK_VM_OFFSET            task_all_image_info_addr;      /* dyld __all_image_info */
    BK_VM_SIZE              task_all_image_info_size;      /* Localização e tamanho */
    BK_VM_OFFSET            task_mach_header_addr;         /* Endereço do mach_header */
    
    /*
     * GPU role
     */
    BK_UINT32               task_gpu_role;                  /* Role da GPU */
    
    /*
     * Estados de suspensão
     */
    BK_BOOLEAN              task_pidsuspended;             /* pid_suspend chamado */
    BK_BOOLEAN              task_frozen;                    /* Congelada */
    BK_BOOLEAN              task_changing_freeze_state;    /* Congelando/descongelando */
    BK_BOOLEAN              task_is_large_corpse;          /* Corpse grande */
    
    /*
     * Limites de CPU
     */
    BK_UINT16               task_policy_ru_cpu;
    BK_UINT16               task_policy_ru_cpu_ext;
    BK_UINT16               task_applied_ru_cpu;
    BK_UINT16               task_applied_ru_cpu_ext;
    
    BK_UINT8                task_rusage_cpu_flags;
    BK_UINT8                task_rusage_cpu_percentage;     /* Limite CPU task-wide */
    BK_UINT8                task_rusage_cpu_perthr_percentage; /* Limite CPU por thread */
    
    BK_UINT64               task_rusage_cpu_interval;       /* Intervalo limite CPU */
    BK_UINT64               task_rusage_cpu_perthr_interval; /* Intervalo por thread */
    BK_UINT64               task_rusage_cpu_deadline;       /* Deadline */
    
    void                    *task_rusage_cpu_callt;         /* Thread call para limite */
    
    /*
     * Wait flags
     */
    BK_UINT8                task_return_wait_flags;
#define BK_TASK_WAIT_NONE            0
#define BK_TASK_WAIT_RETURN          0x01   /* Aguardando fork/posix_spawn/exec */
#define BK_TASK_WAIT_RETURN_WAITER   0x02   /* Aguardando flag ser limpa */
#define BK_TASK_WAIT_EXEC_COMPLETE   0x04   /* Thread deve chamar exec complete */
    
#if MACH_ASSERT
    BK_INT8                 task_suspends_outstanding;      /* Suspends em excesso */
#endif
    
    /*
     * Shared region
     */
    void                    *task_shared_region;            /* Região compartilhada */
    BK_BOOLEAN              task_shared_region_auth_remapped; /* Seções autenticadas */
    char                    *task_shared_region_id;         /* ID da região */
    BK_UINT32               task_shared_region_slide;       /* Slide da região */
    BK_UUID                 task_shared_region_uuid;        /* UUID da região */
    
    /*
     * Bank context
     */
    void                    *task_bank_context;             /* Contexto bank */
    
    /*
     * Importância (IPC)
     */
    void                    *task_imp_base;                 /* Base da cadeia de importância */
    
    /*
     * Estatísticas de módulos externos
     */
    BK_UINT64               task_extmod_statistics[8];      /* Estatísticas extmod */
    
    /*
     * Políticas
     */
    struct {
        BK_UINT32           rp_qos;                          /* QoS requested */
        BK_UINT32           rp_flags;                        /* Flags de política */
    } task_requested_policy;
    
    struct {
        BK_UINT32           ep_qos;                          /* QoS efetivo */
        BK_UINT32           ep_flags;                        /* Flags efetivas */
    } task_effective_policy;
    
    /*
     * Notificações de memória
     */
    BK_UINT32               task_low_mem_notified_warn       :1;
    BK_UINT32               task_low_mem_notified_critical   :1;
    BK_UINT32               task_purged_memory_warn          :1;
    BK_UINT32               task_purged_memory_critical      :1;
    BK_UINT32               task_low_mem_privileged_listener :1;
    BK_UINT32               task_mem_notify_reserved         :27;
    
    /*
     * Limites de memória
     */
    BK_UINT32               task_memlimit_flags;             /* Flags de limite */
    
    /*
     * Estatísticas de I/O
     */
    void                    *task_io_stats;                  /* Estatísticas de I/O */
    
    struct __bk_task_writes_counters task_writes_internal;   /* Writes internos */
    struct __bk_task_writes_counters task_writes_external;   /* Writes externos */
    
    /*
     * Estatísticas de QoS de CPU
     */
    struct __bk_cpu_time_qos_stats task_cpu_time_eqos_stats; /* Estatísticas EQoS */
    struct __bk_cpu_time_qos_stats task_cpu_time_rqos_stats; /* Estatísticas RQoS */
    
    /*
     * Estatísticas agregadas de threads terminadas
     */
    BK_UINT32               task_timer_wakeups_bin_1;        /* Wakeups timer bin1 */
    BK_UINT32               task_timer_wakeups_bin_2;        /* Wakeups timer bin2 */
    BK_UINT64               task_gpu_time_ns;                /* Tempo GPU em ns */
    
    /*
     * Objetos VM
     */
    BK_UINT8                task_can_transfer_memory_ownership;
    BK_UINT8                task_objects_disowning;
    BK_UINT8                task_objects_disowned;
    
    BK_INT                  task_volatile_objects;           /* Objetos voláteis */
    BK_INT                  task_nonvolatile_objects;        /* Objetos não voláteis */
    BK_INT                  task_owned_objects;              /* Objetos owned */
    
    BK_LIST_HEAD(__bk_vm_object_list) task_vm_objects;       /* Lista de objetos VM */
    BK_SPINLOCK             task_vm_objects_lock;            /* Lock da lista */
    
    /*
     * Limites
     */
    BK_UINT32               task_thread_limit;               /* Limite de threads */
    BK_UINT32               task_region_footprint      :1;    /* Footprint de região */
    BK_UINT32               task_region_info_flags     :1;    /* Flags de info */
    BK_UINT32               task_has_crossed_thread_limit :1; /* Cruzou limite */
    BK_UINT32               task_rr_in_flight          :1;    /* t_rr_synchronize em voo */
    BK_UINT32               task_jetsam_realtime_audio :1;    /* Áudio tempo real */
    BK_UINT32               task_legacy_footprint      :1;    /* Footprint legacy */
    BK_UINT32               task_extra_footprint_limit :1;    /* Limite extra */
    BK_UINT32               task_ios13extended_footprint_limit :1; /* Limite iOS13 */
    
    /*
     * Coalizões
     */
    void                    *task_coalition[BK_COALITION_NUM_TYPES];
    BK_LIST_ENTRY(__bk_task) task_coalition_links[BK_COALITION_NUM_TYPES];
    BK_UINT64               task_dispatchqueue_offset;        /* Offset da dispatch queue */
    
    /*
     * Flags de desenvolvimento
     */
#if BK_DEVELOPMENT || BK_DEBUG
    BK_BOOLEAN              task_unnested;                    /* Unnested */
    BK_INT                  task_disconnected_count;          /* Contador de disconnected */
#endif
    
    /*
     * Memória isolada
     */
#if BK_CONFIG_SECLUDED_MEMORY
    BK_UINT8                task_can_use_secluded_mem;        /* Pode usar memória isolada */
    BK_UINT8                task_could_use_secluded_mem;      /* Poderia usar */
    BK_UINT8                task_could_also_use_secluded_mem; /* Também poderia */
    BK_UINT8                task_suppressed_secluded;         /* Suprimido */
#endif
    
    /*
     * Comportamento de guarda de exceção
     */
    BK_EXC_GUARD_BEHAVIOR   task_exc_guard_behavior;          /* Comportamento */
    
    /*
     * Clientes I/O
     */
    BK_LIST_HEAD(__bk_io_user_client_list) task_io_user_clients; /* Clientes I/O */
    
    /*
     * Congelamento
     */
#if BK_CONFIG_FREEZE
    BK_LIST_HEAD(__bk_frozen_cseg_list) task_frozen_cseg_queue; /* CSEGs congelados */
#endif
    
    /*
     * Doação de páginas
     */
    BK_BOOLEAN              task_donates_own_pages;           /* Doa próprias páginas */
    
    /*
     * Metadados de deferred reclaim
     */
#if BK_CONFIG_DEFERRED_RECLAIM
    void                    *task_deferred_reclamation_metadata; /* Metadados */
#endif
    
    /*
     * Informação de code-signing
     */
    BK_UINT64               task_cs_auxiliary_info;           /* Info auxiliar */
    
    /*
     * Configuração de segurança
     */
    struct __bk_task_security_config task_security_config;    /* Configuração */
    
    /*
     * Alocações grandes (telemetria)
     */
    BK_VM_SIZE              task_large_allocation_size;       /* Tamanho de alocação grande */
    
    /*
     * Corpse
     */
    void                    *task_corpse_info;                /* Informação do corpse */
    BK_UINT64               task_crashed_thread_id;           /* Thread que crashou */
    BK_LIST_ENTRY(__bk_task) task_corpse_list;                /* Lista de corpses */
    
    /*
     * Dados privados
     */
    void                    *task_private;                    /* Dados privados */
    BK_UINT32               task_private_size;                /* Tamanho dos dados privados */
    
    /*
     * Callbacks
     */
    void                    (*task_on_terminate)(struct task *);
    void                    (*task_on_suspend)(struct task *);
    void                    (*task_on_resume)(struct task *);
    void                    *task_callback_data;
};

/*
 * Cabeças de lista para tasks
 */
BK_LIST_HEAD(__bk_task_list, task);
BK_TAILQ_HEAD(__bk_task_queue, task);

/*
 * Estatísticas globais de tasks
 */
struct __bk_task_global_stats {
    BK_UINT64               tgs_total_tasks;
    BK_UINT64               tgs_active_tasks;
    BK_UINT64               tgs_suspended_tasks;
    BK_UINT64               tgs_frozen_tasks;
    BK_UINT64               tgs_corpse_tasks;
    BK_UINT64               tgs_total_threads;
    BK_UINT64               tgs_total_semaphores;
    BK_UINT64               tgs_total_messages;
    BK_UINT64               tgs_total_faults;
    BK_UINT64               tgs_total_pageins;
};

/*
 * Macros úteis
 */
#define BK_TASK_IS_64BIT_ADDR(task) \
    (((task)->task_flags & BK_TASK_FLAG_64BIT_ADDR) != 0)
#define BK_TASK_SET_64BIT_ADDR(task) \
    ((task)->task_flags |= BK_TASK_FLAG_64BIT_ADDR)
#define BK_TASK_CLEAR_64BIT_ADDR(task) \
    ((task)->task_flags &= ~BK_TASK_FLAG_64BIT_ADDR)

#define BK_TASK_IS_64BIT_DATA(task) \
    (((task)->task_flags & BK_TASK_FLAG_64BIT_DATA) != 0)
#define BK_TASK_SET_64BIT_DATA(task) \
    ((task)->task_flags |= BK_TASK_FLAG_64BIT_DATA)

#define BK_TASK_HAS_PENDING_CORPSE(task) \
    (((task)->task_flags & BK_TASK_FLAG_PENDING_CORPSE) != 0)
#define BK_TASK_SET_PENDING_CORPSE(task) \
    ((task)->task_flags |= BK_TASK_FLAG_PENDING_CORPSE)

#define BK_TASK_IS_CORPSE_FORK(task) \
    (((task)->task_flags & BK_TASK_FLAG_CORPSE_FORK) != 0)

#define BK_TASK_IS_COALITION_MEMBER(task) \
    (((task)->task_flags & BK_TASK_FLAG_COALITION_MEMBER) != 0)
#define BK_TASK_SET_COALITION_MEMBER(task) \
    ((task)->task_flags |= BK_TASK_FLAG_COALITION_MEMBER)

#define BK_TASK_HAS_PROC(task) \
    (((task)->task_flags & BK_TASK_FLAG_HAS_PROC) != 0)
#define BK_TASK_SET_HAS_PROC(task) \
    ((task)->task_flags |= BK_TASK_FLAG_HAS_PROC)

#define BK_TASK_DID_EXEC(task) \
    (((task)->task_proc_flags & BK_TASK_PROC_FLAG_DID_EXEC) != 0)
#define BK_TASK_IS_EXEC_COPY(task) \
    (((task)->task_proc_flags & BK_TASK_PROC_FLAG_EXEC_COPY) != 0)

#define BK_TASK_IS_ACTIVE(task)        ((task)->task_active)
#define BK_TASK_IS_FROZEN(task)         ((task)->task_frozen)
#define BK_TASK_IS_SUSPENDED(task)      ((task)->task_suspend_count > 0)
