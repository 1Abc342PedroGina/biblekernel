# ifndef PROC_PROC_H
# define PROC_PROC_H
# include <bible/task.h>
# include <bible/vm.h>
# include <bible/ipc.h>
# include <bible/interrupt.h>
# include <bible/exception.h>
# include <proc/activity.h>
# include <bible/kernl_object.h>
# include <bible/ktypes.h>
# include <proc/thread.h>
# include <bible/pmap.h>

/*
 * ============================================================================
 * ESTRUTURAS AUXILIARES
 * ============================================================================
 */

/*
 * KPROCESS (Process Control Block) - Estrutura base do processo
 */
struct __bk_kprocess {
    BK_SPINLOCK             kp_lock;                 /* Lock do processo */
    BK_UINT16               kp_state;                 /* Estado do processo */
    BK_UINT16               kp_thread_state;          /* Estado das threads */
    BK_UINT8                kp_base_priority;         /* Prioridade base */
    BK_UINT8                kp_quantum;                /* Quantum */
    BK_UINT8                kp_auto_boost;             /* Auto boost */
    BK_UINT8                kp_disable_boost;          /* Desabilitar boost */
    BK_UINT32               kp_thread_count;           /* Número de threads */
    BK_UINT32               kp_ready_thread_count;     /* Threads prontas */
    BK_UINT32               kp_ideal_node;             /* Nó ideal */
    BK_UL3264               kp_affinity_mask;          /* Máscara de afinidade */
    BK_UL3264               kp_user_affinity_mask;     /* Afinidade do usuário */
    void                    *kp_stack;                  /* Pilha do kernel */
    BK_LIST_ENTRY(__bk_kprocess) kp_process_list;      /* Lista de processos */
};

/*
 * EX_PUSH_LOCK - Lock de exclusão com push
 */
struct __bk_ex_push_lock {
    BK_UL3264               pl_value;                  /* Valor do lock */
    BK_SPINLOCK             pl_spinlock;                /* Spinlock interno */
};

/*
 * EX_RUNDOWN_REF - Referência de rundown
 */
struct __bk_ex_rundown_ref {
    BK_UL3264               rr_count;                  /* Contagem de referências */
    BK_SPINLOCK             rr_lock;                    /* Lock */
};

/*
 * EX_FAST_REF - Referência rápida com contagem embutida
 */
struct __bk_ex_fast_ref {
    BK_UL3264               fr_value;                   /* Valor com contagem */
#define BK_EX_FAST_REF_COUNT_MASK   0x000000000000000FULL
#define BK_EX_FAST_REF_PTR_MASK     0xFFFFFFFFFFFFFFF0ULL
};

/*
 * LARGE_INTEGER - Inteiro grande (64 bits)
 */
union __bk_large_integer {
    struct {
        BK_UINT32           li_low_part;                /* Parte baixa */
        BK_UINT32           li_high_part;               /* Parte alta */
    };
    BK_UINT64               li_quad_part;               /* Valor de 64 bits */
};

/*
 * LIST_ENTRY - Entrada de lista duplamente encadeada
 */
struct __bk_list_entry {
    struct __bk_list_entry  *le_flink;                  /* Próximo */
    struct __bk_list_entry  *le_blink;                  /* Anterior */
};

/*
 * RTL_AVL_TREE - Árvore AVL
 */
struct __bk_rtl_avl_tree {
    struct __bk_avl_node    *at_root;                    /* Raiz da árvore */
};

/*
 * MMSUPPORT_FULL - Suporte completo de memória
 */
struct __bk_mmsupport_full {
    BK_VM_SIZE              ms_working_set_size;        /* Tamanho do working set */
    BK_VM_SIZE              ms_working_set_peak;        /* Pico do working set */
    BK_VM_SIZE              ms_minimum_working_set;     /* Working set mínimo */
    BK_VM_SIZE              ms_maximum_working_set;     /* Working set máximo */
    BK_UINT64               ms_page_fault_count;        /* Contagem de page faults */
    BK_UINT64               ms_page_fault_peak;         /* Pico de page faults */
    BK_SPINLOCK             ms_lock;                     /* Lock */
};

/*
 * ALPC_PROCESS_CONTEXT - Contexto ALPC
 */
struct __bk_alpc_process_context {
    BK_ID                   ac_port;                     /* Porto ALPC */
    BK_LIST_ENTRY           ac_connection_list;          /* Lista de conexões */
    BK_SPINLOCK             ac_lock;                     /* Lock */
};

/*
 * PO_PROCESS_ENERGY_CONTEXT - Contexto de energia
 */
struct __bk_po_process_energy_context {
    BK_UINT64               ec_energy_consumed;          /* Energia consumida */
    BK_UINT64               ec_last_update_time;         /* Última atualização */
    BK_UINT32               ec_flags;                     /* Flags */
};

/*
 * PS_PROTECTION - Proteção do processo
 */
struct __bk_ps_protection {
    BK_UINT8                pp_level;                     /* Nível de proteção */
    BK_UINT8                pp_type;                      /* Tipo de proteção */
    BK_UINT8                pp_audit;                     /* Audit */
    BK_UINT8                pp_reserved;                  /* Reservado */
};

/*
 * PROCESS_DISK_COUNTERS - Contadores de disco
 */
struct __bk_process_disk_counters {
    BK_UINT64               dc_bytes_read;                /* Bytes lidos */
    BK_UINT64               dc_bytes_written;             /* Bytes escritos */
    BK_UINT64               dc_read_ops;                  /* Operações de leitura */
    BK_UINT64               dc_write_ops;                 /* Operações de escrita */
    BK_UINT64               dc_flush_ops;                 /* Operações de flush */
};

/*
 * WNF_STATE_NAME - Nome de estado WNF
 */
struct __bk_wnf_state_name {
    BK_UINT64               wsn_name;                     /* Nome */
    BK_UINT32               wsn_scope;                    /* Escopo */
    BK_UINT32               wsn_flags;                     /* Flags */
};

/*
 * PS_PROCESS_WAKE_INFORMATION - Informação de wake
 */
struct __bk_ps_process_wake_information {
    BK_UINT64               pwi_wake_count;                /* Contagem de wakes */
    BK_UINT64               pwi_last_wake_time;            /* Último wake */
    BK_LIST_ENTRY           pwi_wake_list;                 /* Lista de wakes */
};

/*
 * PS_DYNAMIC_ENFORCED_ADDRESS_RANGES - Intervalos de endereços dinâmicos
 */
struct __bk_ps_dynamic_enforced_address_ranges {
    BK_VM_OFFSET            de_start;                      /* Início */
    BK_VM_OFFSET            de_end;                        /* Fim */
    BK_UINT32               de_flags;                       /* Flags */
};

/*
 * PS_SYSCALL_PROVIDER - Provedor de syscalls
 */
struct __bk_ps_syscall_provider {
    BK_ID                   sp_id;                          /* ID do provedor */
    BK_UINT32               sp_flags;                       /* Flags */
    void                    *sp_dispatch_table;             /* Tabela de dispatch */
};

/*
 * PSP_SYSCALL_PROVIDER_DISPATCH_CONTEXT - Contexto de dispatch
 */
struct __bk_psp_syscall_provider_dispatch_context {
    BK_UINT32               dc_magic;                       /* Magic */
    void                    *dc_dispatch;                    /* Função de dispatch */
    void                    *dc_context;                     /* Contexto */
};

/*
 * PROCESS_NETWORK_COUNTERS - Contadores de rede
 */
struct __bk_process_network_counters {
    BK_UINT64               nc_bytes_sent;                  /* Bytes enviados */
    BK_UINT64               nc_bytes_received;              /* Bytes recebidos */
    BK_UINT64               nc_packets_sent;                /* Pacotes enviados */
    BK_UINT64               nc_packets_received;            /* Pacotes recebidos */
};

/*
 * PROCESS_EXECUTION - Execução do processo
 */
union __bk_process_execution {
    struct {
        BK_UINT64           pe_state;                        /* Estado */
        BK_UINT64           pe_flags;                        /* Flags */
    };
    BK_UINT8                pe_data[16];                     /* Dados brutos */
};

/*
 * ============================================================================
 * ESTRUTURA PRINCIPAL DO PROCESSO
 * ============================================================================
 */

struct __bk_process {
    /*
     * Process Control Block (0x0 - 0x1c8)
     */
    struct __bk_kprocess    proc_pcb;                        /* KPROCESS */
    
    /*
     * Locks e proteções (0x1c8 - 0x1d0)
     */
    struct __bk_ex_push_lock proc_lock;                       /* ProcessLock */
    
    /*
     * Identificação (0x1d0 - 0x1e8)
     */
    BK_ID                   proc_unique_id;                  /* UniqueProcessId */
    struct __bk_list_entry  proc_active_links;                /* ActiveProcessLinks */
    struct __bk_ex_rundown_ref proc_rundown_protect;          /* RundownProtect */
    
    /*
     * Flags2 (0x1f0)
     */
    union {
        BK_UINT32           proc_flags2;
        struct {
            BK_UINT32       proc_job_not_active       :1;     /* JobNotReallyActive */
            BK_UINT32       proc_accounting_folded    :1;     /* AccountingFolded */
            BK_UINT32       proc_new_reported         :1;     /* NewProcessReported */
            BK_UINT32       proc_exit_reported        :1;     /* ExitProcessReported */
            BK_UINT32       proc_report_commit        :1;     /* ReportCommitChanges */
            BK_UINT32       proc_last_report_memory   :1;     /* LastReportMemory */
            BK_UINT32       proc_force_wake_charge    :1;     /* ForceWakeCharge */
            BK_UINT32       proc_cross_session        :1;     /* CrossSessionCreate */
            BK_UINT32       proc_needs_handle_rundown :1;     /* NeedsHandleRundown */
            BK_UINT32       proc_ref_trace_enabled    :1;     /* RefTraceEnabled */
            BK_UINT32       proc_pico_created         :1;     /* PicoCreated */
            BK_UINT32       proc_empty_job_evaluated  :1;     /* EmptyJobEvaluated */
            BK_UINT32       proc_default_page_priority :3;    /* DefaultPagePriority */
            BK_UINT32       proc_primary_token_frozen :1;     /* PrimaryTokenFrozen */
            BK_UINT32       proc_verifier_target      :1;     /* ProcessVerifierTarget */
            BK_UINT32       proc_restrict_thread_context :1;  /* RestrictSetThreadContext */
            BK_UINT32       proc_affinity_permanent    :1;    /* AffinityPermanent */
            BK_UINT32       proc_affinity_update       :1;    /* AffinityUpdateEnable */
            BK_UINT32       proc_propagate_node        :1;    /* PropagateNode */
            BK_UINT32       proc_explicit_affinity     :1;    /* ExplicitAffinity */
            BK_UINT32       proc_flags2_available1     :2;    /* Flags2Available1 */
            BK_UINT32       proc_enable_read_vm_log    :1;    /* EnableReadVmLogging */
            BK_UINT32       proc_enable_write_vm_log   :1;    /* EnableWriteVmLogging */
            BK_UINT32       proc_fatal_access_request  :1;    /* FatalAccessTerminationRequested */
            BK_UINT32       proc_disable_system_cpu_set :1;   /* DisableSystemAllowedCpuSet */
            BK_UINT32       proc_flags2_available2     :3;    /* Flags2Available2 */
            BK_UINT32       proc_in_private            :1;    /* InPrivate */
        };
    };
    
    /*
     * Flags (0x1f4)
     */
    union {
        BK_UINT32           proc_flags;
        struct {
            BK_UINT32       proc_create_reported       :1;    /* CreateReported */
            BK_UINT32       proc_no_debug_inherit      :1;    /* NoDebugInherit */
            BK_UINT32       proc_exiting               :1;    /* ProcessExiting */
            BK_UINT32       proc_delete                :1;    /* ProcessDelete */
            BK_UINT32       proc_manage_exec_writes    :1;    /* ManageExecutableMemoryWrites */
            BK_UINT32       proc_vm_deleted            :1;    /* VmDeleted */
            BK_UINT32       proc_outswap_enabled       :1;    /* OutswapEnabled */
            BK_UINT32       proc_outswapped            :1;    /* Outswapped */
            BK_UINT32       proc_failfast_on_commit    :1;    /* FailFastOnCommitFail */
            BK_UINT32       proc_wow64_4gb_space       :1;    /* Wow64VaSpace4Gb */
            BK_UINT32       proc_address_space_init    :2;    /* AddressSpaceInitialized */
            BK_UINT32       proc_set_timer_resolution  :1;    /* SetTimerResolution */
            BK_UINT32       proc_break_on_termination  :1;    /* BreakOnTermination */
            BK_UINT32       proc_deprioritize_views    :1;    /* DeprioritizeViews */
            BK_UINT32       proc_write_watch           :1;    /* WriteWatch */
            BK_UINT32       proc_in_session            :1;    /* ProcessInSession */
            BK_UINT32       proc_override_address_space :1;   /* OverrideAddressSpace */
            BK_UINT32       proc_has_address_space     :1;    /* HasAddressSpace */
            BK_UINT32       proc_launch_prefetched     :1;    /* LaunchPrefetched */
            BK_UINT32       proc_reserved              :1;    /* Reserved */
            BK_UINT32       proc_vm_top_down           :1;    /* VmTopDown */
            BK_UINT32       proc_image_notify_done     :1;    /* ImageNotifyDone */
            BK_UINT32       proc_pde_update_needed     :1;    /* PdeUpdateNeeded */
            BK_UINT32       proc_vdm_allowed           :1;    /* VdmAllowed */
            BK_UINT32       proc_rundown               :1;    /* ProcessRundown */
            BK_UINT32       proc_inserted              :1;    /* ProcessInserted */
            BK_UINT32       proc_default_io_priority   :3;    /* DefaultIoPriority */
            BK_UINT32       proc_self_delete           :1;    /* ProcessSelfDelete */
            BK_UINT32       proc_timer_resolution_link :1;    /* SetTimerResolutionLink */
        };
    };
    
    /*
     * Tempos e quotas (0x1f8 - 0x230)
     */
    union __bk_large_integer proc_create_time;                /* CreateTime */
    BK_UINT64               proc_quota_usage[2];              /* ProcessQuotaUsage */
    BK_UINT64               proc_quota_peak[2];               /* ProcessQuotaPeak */
    BK_UINT64               proc_peak_virtual_size;           /* PeakVirtualSize */
    BK_UINT64               proc_virtual_size;                /* VirtualSize */
    
    /*
     * Links de sessão (0x230 - 0x240)
     */
    struct __bk_list_entry  proc_session_links;                /* SessionProcessLinks */
    
    /*
     * Porto de exceção (0x240 - 0x248)
     */
    union {
        void                *proc_exception_port_data;         /* ExceptionPortData */
        BK_UINT64           proc_exception_port_value;         /* ExceptionPortValue */
        struct {
            BK_UINT64       proc_exception_port_state :3;      /* ExceptionPortState */
        };
    };
    
    /*
     * Token e memória (0x248 - 0x258)
     */
    struct __bk_ex_fast_ref proc_token;                        /* Token */
    BK_UINT64               proc_mm_reserved;                  /* MmReserved */
    struct __bk_ex_push_lock proc_address_creation_lock;       /* AddressCreationLock */
    struct __bk_ex_push_lock proc_page_table_commit_lock;      /* PageTableCommitmentLock */
    
    /*
     * Threads e forks (0x268 - 0x280)
     */
    struct __bk_thread      *proc_rotate_in_progress;          /* RotateInProgress */
    struct __bk_thread      *proc_fork_in_progress;            /* ForkInProgress */
    struct __bk_job         *proc_commit_charge_job;           /* CommitChargeJob */
    
    /*
     * Árvores e contagens (0x280 - 0x298)
     */
    struct __bk_rtl_avl_tree proc_clone_root;                   /* CloneRoot */
    BK_UINT64               proc_number_private_pages;         /* NumberOfPrivatePages */
    BK_UINT64               proc_number_locked_pages;          /* NumberOfLockedPages */
    
    /*
     * Objetos Windows (0x298 - 0x2b8)
     */
    void                    *proc_win32_process;               /* Win32Process */
    struct __bk_job         *proc_job;                          /* Job */
    void                    *proc_section_object;               /* SectionObject */
    void                    *proc_section_base_address;         /* SectionBaseAddress */
    BK_UINT32               proc_cookie;                        /* Cookie */
    
    /*
     * Working set e janela (0x2c0 - 0x2d0)
     */
    void                    *proc_working_set_watch;            /* WorkingSetWatch */
    void                    *proc_win32_window_station;         /* Win32WindowStation */
    BK_ID                   proc_inherited_from_pid;            /* InheritedFromUniqueProcessId */
    
    /*
     * Owner e PEB (0x2d8 - 0x2e8)
     */
    BK_UINT64               proc_owner_process_id;              /* OwnerProcessId */
    struct __bk_peb         *proc_peb;                          /* Peb */
    void                    *proc_session;                      /* Session */
    
    /*
     * Quota e tabelas (0x2f0 - 0x308)
     */
    void                    *proc_spare1;                        /* Spare1 */
    struct __bk_process_quota_block *proc_quota_block;          /* QuotaBlock */
    struct __bk_handle_table *proc_object_table;                 /* ObjectTable */
    void                    *proc_debug_port;                    /* DebugPort */
    
    /*
     * WoW64 e device map (0x310 - 0x320)
     */
    struct __bk_wow64_process *proc_wow64_process;               /* WoW64Process */
    struct __bk_ex_fast_ref  proc_device_map;                    /* DeviceMap */
    void                     *proc_etw_data_source;              /* EtwDataSource */
    
    /*
     * Page directory e imagem (0x328 - 0x348)
     */
    BK_UINT64                proc_page_directory_pte;            /* PageDirectoryPte */
    struct __bk_file_object  *proc_image_file_pointer;           /* ImageFilePointer */
    BK_UINT8                 proc_image_file_name[15];           /* ImageFileName */
    BK_UINT8                 proc_priority_class;                /* PriorityClass */
    void                     *proc_security_port;                /* SecurityPort */
    
    /*
     * Auditoria e jobs (0x350 - 0x368)
     */
    struct {
        BK_ID               sap_process_id;                      /* ProcessId */
        void                *sap_image_name;                     /* ImageFileName */
    } proc_se_audit_process_creation_info;                        /* SeAuditProcessCreationInfo */
    
    struct __bk_list_entry   proc_job_links;                      /* JobLinks */
    void                    *proc_highest_user_address;           /* HighestUserAddress */
    
    /*
     * Threads (0x370 - 0x390)
     */
    struct __bk_list_entry   proc_thread_list_head;               /* ThreadListHead */
    BK_UINT32                proc_active_threads;                 /* ActiveThreads */
    BK_UINT32                proc_image_path_hash;                /* ImagePathHash */
    BK_UINT32                proc_default_hard_error_processing;  /* DefaultHardErrorProcessing */
    BK_INT                   proc_last_thread_exit_status;        /* LastThreadExitStatus */
    
    /*
     * Prefetch e páginas (0x390 - 0x3a0)
     */
    struct __bk_ex_fast_ref  proc_prefetch_trace;                 /* PrefetchTrace */
    void                     *proc_locked_pages_list;             /* LockedPagesList */
    
    /*
     * Contadores de operações (0x3a0 - 0x3d0)
     */
    union __bk_large_integer proc_read_operation_count;           /* ReadOperationCount */
    union __bk_large_integer proc_write_operation_count;          /* WriteOperationCount */
    union __bk_large_integer proc_other_operation_count;          /* OtherOperationCount */
    union __bk_large_integer proc_read_transfer_count;            /* ReadTransferCount */
    union __bk_large_integer proc_write_transfer_count;           /* WriteTransferCount */
    union __bk_large_integer proc_other_transfer_count;           /* OtherTransferCount */
    
    /*
     * Commit charge (0x3d0 - 0x400)
     */
    BK_UINT64                proc_commit_charge_limit;            /* CommitChargeLimit */
    BK_UINT64                proc_commit_charge;                  /* CommitCharge */
    BK_UINT64                proc_commit_charge_peak;             /* CommitChargePeak */
    
    /*
     * Suporte de memória (0x400 - 0x540)
     */
    struct __bk_mmsupport_full proc_vm;                           /* Vm */
    
    /*
     * Links de memória (0x540 - 0x550)
     */
    struct __bk_list_entry   proc_mm_process_links;               /* MmProcessLinks */
    
    /*
     * Páginas modificadas e status (0x550 - 0x558)
     */
    BK_UINT32                proc_modified_page_count;            /* ModifiedPageCount */
    BK_INT                   proc_exit_status;                    /* ExitStatus */
    
    /*
     * VAD (Virtual Address Descriptor) (0x558 - 0x580)
     */
    struct __bk_rtl_avl_tree proc_vad_root;                       /* VadRoot */
    void                     *proc_vad_hint;                      /* VadHint */
    BK_UINT64                proc_vad_count;                      /* VadCount */
    BK_UINT64                proc_vad_physical_pages;             /* VadPhysicalPages */
    BK_UINT64                proc_vad_physical_pages_limit;       /* VadPhysicalPagesLimit */
    
    /*
     * ALPC (0x580 - 0x5a0)
     */
    struct __bk_alpc_process_context proc_alpc_context;           /* AlpcContext */
    
    /*
     * Timer resolution (0x5a0 - 0x5c0)
     */
    struct __bk_list_entry   proc_timer_resolution_link;          /* TimerResolutionLink */
    void                     *proc_timer_resolution_stack_record; /* TimerResolutionStackRecord */
    BK_UINT32                proc_requested_timer_resolution;     /* RequestedTimerResolution */
    BK_UINT32                proc_smallest_timer_resolution;      /* SmallestTimerResolution */
    
    /*
     * Exit time e função invertida (0x5c0 - 0x5d0)
     */
    union __bk_large_integer proc_exit_time;                      /* ExitTime */
    void                     *proc_inverted_function_table;       /* InvertedFunctionTable */
    struct __bk_ex_push_lock proc_inverted_function_table_lock;   /* InvertedFunctionTableLock */
    
    /*
     * Watermarks e VADs grandes (0x5d8 - 0x5e0)
     */
    BK_UINT32                proc_active_threads_high_watermark;  /* ActiveThreadsHighWatermark */
    BK_UINT32                proc_large_private_vad_count;        /* LargePrivateVadCount */
    
    /*
     * Thread list lock e WNF (0x5e0 - 0x5f0)
     */
    struct __bk_ex_push_lock proc_thread_list_lock;                /* ThreadListLock */
    void                     *proc_wnf_context;                    /* WnfContext */
    
    /*
     * Server silo e assinaturas (0x5f0 - 0x5fb)
     */
    struct __bk_job          *proc_server_silo;                    /* ServerSilo */
    BK_UINT8                 proc_signature_level;                 /* SignatureLevel */
    BK_UINT8                 proc_section_signature_level;         /* SectionSignatureLevel */
    struct __bk_ps_protection proc_protection;                     /* Protection */
    
    BK_UINT8                 proc_hang_count              :3;      /* HangCount */
    BK_UINT8                 proc_ghost_count             :3;      /* GhostCount */
    BK_UINT8                 proc_prefilter_exception     :1;      /* PrefilterException */
    BK_UINT8                 proc_spare_byte              :1;      /* Spare */
    
    /*
     * Flags3 (0x5fc)
     */
    union {
        BK_UINT32           proc_flags3;
        struct {
            BK_UINT32       proc_minimal                  :1;      /* Minimal */
            BK_UINT32       proc_replacing_page_root      :1;      /* ReplacingPageRoot */
            BK_UINT32       proc_crashed                  :1;      /* Crashed */
            BK_UINT32       proc_job_vads_tracked         :1;      /* JobVadsAreTracked */
            BK_UINT32       proc_vad_tracking_disabled    :1;      /* VadTrackingDisabled */
            BK_UINT32       proc_auxiliary_process        :1;      /* AuxiliaryProcess */
            BK_UINT32       proc_subsystem_process        :1;      /* SubsystemProcess */
            BK_UINT32       proc_indirect_cpu_sets        :1;      /* IndirectCpuSets */
            BK_UINT32       proc_relinquished_commit      :1;      /* RelinquishedCommit */
            BK_UINT32       proc_high_graphics_priority   :1;      /* HighGraphicsPriority */
            BK_UINT32       proc_commit_fail_logged       :1;      /* CommitFailLogged */
            BK_UINT32       proc_reserve_fail_logged      :1;      /* ReserveFailLogged */
            BK_UINT32       proc_system_process           :1;      /* SystemProcess */
            BK_UINT32       proc_images_at_base           :1;      /* AllImagesAtBasePristineBase */
            BK_UINT32       proc_address_policy_frozen    :1;      /* AddressPolicyFrozen */
            BK_UINT32       proc_first_resume             :1;      /* ProcessFirstResume */
            BK_UINT32       proc_foreground_external      :1;      /* ForegroundExternal */
            BK_UINT32       proc_foreground_system        :1;      /* ForegroundSystem */
            BK_UINT32       proc_high_memory_priority     :1;      /* HighMemoryPriority */
            BK_UINT32       proc_enable_suspend_resume_log :1;     /* EnableProcessSuspendResumeLogging */
            BK_UINT32       proc_enable_thread_suspend_resume_log :1; /* EnableThreadSuspendResumeLogging */
            BK_UINT32       proc_security_domain_changed  :1;      /* SecurityDomainChanged */
            BK_UINT32       proc_security_freeze_complete :1;      /* SecurityFreezeComplete */
            BK_UINT32       proc_vm_processor_host        :1;      /* VmProcessorHost */
            BK_UINT32       proc_vm_processor_host_transition :1; /* VmProcessorHostTransition */
            BK_UINT32       proc_alt_syscall              :1;      /* AltSyscall */
            BK_UINT32       proc_timer_resolution_ignore  :1;      /* TimerResolutionIgnore */
            BK_UINT32       proc_disallow_user_terminate  :1;      /* DisallowUserTerminate */
            BK_UINT32       proc_enable_remote_exec_protect_log :1; /* EnableProcessRemoteExecProtectVmLogging */
            BK_UINT32       proc_enable_local_exec_protect_log :1;  /* EnableProcessLocalExecProtectVmLogging */
            BK_UINT32       proc_memory_compression_process :1;    /* MemoryCompressionProcess */
            BK_UINT32       proc_enable_impersonation_log :1;      /* EnableProcessImpersonationLogging */
        };
    };
    
    /*
     * SVM e dispositivos (0x600 - 0x630)
     */
    BK_INT                   proc_device_asid;                      /* DeviceAsid */
    void                     *proc_svm_data;                        /* SvmData */
    struct __bk_ex_push_lock proc_svm_process_lock;                  /* SvmProcessLock */
    BK_UINT64                proc_svm_lock;                         /* SvmLock */
    struct __bk_list_entry   proc_svm_device_list_head;             /* SvmProcessDeviceListHead */
    
    /*
     * Freeze e contadores de disco (0x630 - 0x640)
     */
    BK_UINT64                proc_last_freeze_interrupt_time;       /* LastFreezeInterruptTime */
    struct __bk_process_disk_counters *proc_disk_counters;          /* DiskCounters */
    
    /*
     * Pico e enclaves (0x640 - 0x660)
     */
    void                     *proc_pico_context;                    /* PicoContext */
    void                     *proc_enclave_table;                    /* EnclaveTable */
    BK_UINT64                proc_enclave_number;                    /* EnclaveNumber */
    struct __bk_ex_push_lock proc_enclave_lock;                      /* EnclaveLock */
    BK_UINT32                proc_high_priority_faults_allowed;     /* HighPriorityFaultsAllowed */
    
    /*
     * Energia e contexto VM (0x668 - 0x678)
     */
    struct __bk_po_process_energy_context *proc_energy_context;      /* EnergyContext */
    void                     *proc_vm_context;                       /* VmContext */
    
    /*
     * Tempos e sequência (0x678 - 0x6a8)
     */
    BK_UINT64                proc_sequence_number;                   /* SequenceNumber */
    BK_UINT64                proc_create_interrupt_time;             /* CreateInterruptTime */
    BK_UINT64                proc_create_unbiased_interrupt_time;    /* CreateUnbiasedInterruptTime */
    BK_UINT64                proc_total_unbiased_frozen_time;        /* TotalUnbiasedFrozenTime */
    BK_UINT64                proc_last_app_state_update_time;        /* LastAppStateUpdateTime */
    
    BK_UINT64                proc_last_app_state_uptime      :61;    /* LastAppStateUptime */
    BK_UINT64                proc_last_app_state              :3;     /* LastAppState */
    
    /*
     * Commit compartilhado (0x6a8 - 0x6c0)
     */
    BK_UINT64                proc_shared_commit_charge;              /* SharedCommitCharge */
    struct __bk_ex_push_lock proc_shared_commit_lock;                 /* SharedCommitLock */
    struct __bk_list_entry   proc_shared_commit_links;                /* SharedCommitLinks */
    
    /*
     * CPU sets (0x6c8 - 0x6d8)
     */
    union {
        struct {
            BK_UINT64       proc_allowed_cpu_sets;                    /* AllowedCpuSets */
            BK_UINT64       proc_default_cpu_sets;                    /* DefaultCpuSets */
        };
        struct {
            BK_UINT64       *proc_allowed_cpu_sets_indirect;          /* AllowedCpuSetsIndirect */
            BK_UINT64       *proc_default_cpu_sets_indirect;          /* DefaultCpuSetsIndirect */
        };
    };
    
    /*
     * Disk I/O e DXG (0x6d8 - 0x6e8)
     */
    void                     *proc_disk_io_attribution;               /* DiskIoAttribution */
    void                     *proc_dxg_process;                       /* DxgProcess */
    BK_UINT32                proc_win32k_filter_set;                   /* Win32KFilterSet */
    
    /*
     * Machine e identidade (0x6ec - 0x6f0)
     */
    BK_UINT16                proc_machine;                            /* Machine */
    BK_UINT8                 proc_mm_slab_identity;                   /* MmSlabIdentity */
    BK_UINT8                 proc_spare0;                             /* Spare0 */
    
    /*
     * Timers (0x6f0 - 0x720)
     */
    BK_UINT64                proc_process_timer_delay;                /* ProcessTimerDelay */
    BK_UINT32                proc_ktimer_sets;                        /* KTimerSets */
    BK_UINT32                proc_ktimer2_sets;                       /* KTimer2Sets */
    BK_UINT32                proc_thread_timer_sets;                  /* ThreadTimerSets */
    BK_UINT64                proc_virtual_timer_list_lock;            /* VirtualTimerListLock */
    struct __bk_list_entry   proc_virtual_timer_list_head;             /* VirtualTimerListHead */
    
    /*
     * Wake information (0x720 - 0x750)
     */
    union {
        struct __bk_wnf_state_name           proc_wake_channel;        /* WakeChannel */
        struct __bk_ps_process_wake_information proc_wake_info;        /* WakeInfo */
    };
    
    /*
     * Mitigation Flags (0x750 - 0x758)
     */
    union {
        BK_UINT32           proc_mitigation_flags;
        struct {
            BK_UINT32       proc_cfg_enabled                   :1;     /* ControlFlowGuardEnabled */
            BK_UINT32       proc_cfg_export_suppression        :1;     /* ControlFlowGuardExportSuppressionEnabled */
            BK_UINT32       proc_cfg_strict                    :1;     /* ControlFlowGuardStrict */
            BK_UINT32       proc_disallow_stripped_images      :1;     /* DisallowStrippedImages */
            BK_UINT32       proc_force_relocate_images         :1;     /* ForceRelocateImages */
            BK_UINT32       proc_high_entropy_aslr             :1;     /* HighEntropyASLREnabled */
            BK_UINT32       proc_stack_randomization_disabled  :1;     /* StackRandomizationDisabled */
            BK_UINT32       proc_extension_point_disable       :1;     /* ExtensionPointDisable */
            BK_UINT32       proc_disable_dynamic_code          :1;     /* DisableDynamicCode */
            BK_UINT32       proc_disable_dynamic_code_optout   :1;     /* DisableDynamicCodeAllowOptOut */
            BK_UINT32       proc_disable_dynamic_code_remote   :1;     /* DisableDynamicCodeAllowRemoteDowngrade */
            BK_UINT32       proc_audit_disable_dynamic_code    :1;     /* AuditDisableDynamicCode */
            BK_UINT32       proc_disallow_win32k_syscalls      :1;     /* DisallowWin32kSystemCalls */
            BK_UINT32       proc_audit_disallow_win32k         :1;     /* AuditDisallowWin32kSystemCalls */
            BK_UINT32       proc_enable_filtered_win32k        :1;     /* EnableFilteredWin32kAPIs */
            BK_UINT32       proc_audit_filtered_win32k         :1;     /* AuditFilteredWin32kAPIs */
            BK_UINT32       proc_disable_non_system_fonts      :1;     /* DisableNonSystemFonts */
            BK_UINT32       proc_audit_non_system_fonts        :1;     /* AuditNonSystemFontLoading */
            BK_UINT32       proc_prefer_system32_images        :1;     /* PreferSystem32Images */
            BK_UINT32       proc_prohibit_remote_image_map     :1;     /* ProhibitRemoteImageMap */
            BK_UINT32       proc_audit_prohibit_remote_map     :1;     /* AuditProhibitRemoteImageMap */
            BK_UINT32       proc_prohibit_lowil_image_map      :1;     /* ProhibitLowILImageMap */
            BK_UINT32       proc_audit_prohibit_lowil_map      :1;     /* AuditProhibitLowILImageMap */
            BK_UINT32       proc_signature_mitigation_optin    :1;     /* SignatureMitigationOptIn */
            BK_UINT32       proc_audit_block_non_ms            :1;     /* AuditBlockNonMicrosoftBinaries */
            BK_UINT32       proc_audit_block_non_ms_store      :1;     /* AuditBlockNonMicrosoftBinariesAllowStore */
            BK_UINT32       proc_loader_integrity_continuity   :1;     /* LoaderIntegrityContinuityEnabled */
            BK_UINT32       proc_audit_loader_integrity        :1;     /* AuditLoaderIntegrityContinuity */
            BK_UINT32       proc_enable_module_tampering_protect :1;  /* EnableModuleTamperingProtection */
            BK_UINT32       proc_module_tampering_no_inherit   :1;     /* EnableModuleTamperingProtectionNoInherit */
            BK_UINT32       proc_restrict_indirect_branch      :1;     /* RestrictIndirectBranchPrediction */
            BK_UINT32       proc_isolate_security_domain       :1;     /* IsolateSecurityDomain */
        } proc_mitigation_flags_values;
    };
    
    /*
     * Mitigation Flags 2 (0x754 - 0x758)
     */
    union {
        BK_UINT32           proc_mitigation_flags2;
        struct {
            BK_UINT32       proc_enable_export_filter          :1;     /* EnableExportAddressFilter */
            BK_UINT32       proc_audit_export_filter           :1;     /* AuditExportAddressFilter */
            BK_UINT32       proc_enable_export_filter_plus     :1;     /* EnableExportAddressFilterPlus */
            BK_UINT32       proc_audit_export_filter_plus      :1;     /* AuditExportAddressFilterPlus */
            BK_UINT32       proc_enable_rop_stack_pivot        :1;     /* EnableRopStackPivot */
            BK_UINT32       proc_audit_rop_stack_pivot         :1;     /* AuditRopStackPivot */
            BK_UINT32       proc_enable_rop_caller_check       :1;     /* EnableRopCallerCheck */
            BK_UINT32       proc_audit_rop_caller_check        :1;     /* AuditRopCallerCheck */
            BK_UINT32       proc_enable_rop_sim_exec           :1;     /* EnableRopSimExec */
            BK_UINT32       proc_audit_rop_sim_exec            :1;     /* AuditRopSimExec */
            BK_UINT32       proc_enable_import_filter          :1;     /* EnableImportAddressFilter */
            BK_UINT32       proc_audit_import_filter           :1;     /* AuditImportAddressFilter */
            BK_UINT32       proc_disable_page_combine          :1;     /* DisablePageCombine */
            BK_UINT32       proc_speculative_store_bypass_disable :1; /* SpeculativeStoreBypassDisable */
            BK_UINT32       proc_cet_user_shadow_stacks        :1;     /* CetUserShadowStacks */
            BK_UINT32       proc_audit_cet_shadow_stacks       :1;     /* AuditCetUserShadowStacks */
            BK_UINT32       proc_audit_cet_shadow_stacks_logged :1;   /* AuditCetUserShadowStacksLogged */
            BK_UINT32       proc_cet_set_context_ip_validation :1;    /* UserCetSetContextIpValidation */
            BK_UINT32       proc_audit_cet_set_context_ip      :1;     /* AuditUserCetSetContextIpValidation */
            BK_UINT32       proc_audit_cet_set_context_logged  :1;     /* AuditUserCetSetContextIpValidationLogged */
            BK_UINT32       proc_cet_shadow_stacks_strict      :1;     /* CetUserShadowStacksStrictMode */
            BK_UINT32       proc_block_non_cet_binaries        :1;     /* BlockNonCetBinaries */
            BK_UINT32       proc_block_non_cet_non_ehcont      :1;     /* BlockNonCetBinariesNonEhcont */
            BK_UINT32       proc_audit_block_non_cet           :1;     /* AuditBlockNonCetBinaries */
            BK_UINT32       proc_audit_block_non_cet_logged    :1;     /* AuditBlockNonCetBinariesLogged */
            BK_UINT32       proc_xfg_deprecated                :1;     /* XtendedControlFlowGuard_Deprecated */
            BK_UINT32       proc_audit_xfg_deprecated          :1;     /* AuditXtendedControlFlowGuard_Deprecated */
            BK_UINT32       proc_pointer_auth_user_ip          :1;     /* PointerAuthUserIp */
            BK_UINT32       proc_audit_pointer_auth_user_ip    :1;     /* AuditPointerAuthUserIp */
            BK_UINT32       proc_audit_pointer_auth_user_logged :1;    /* AuditPointerAuthUserIpLogged */
            BK_UINT32       proc_cet_dynamic_apis_outproc_only :1;     /* CetDynamicApisOutOfProcOnly */
            BK_UINT32       proc_cet_set_context_relaxed       :1;     /* UserCetSetContextIpValidationRelaxedMode */
        } proc_mitigation_flags2_values;
    };
    
    /*
     * Partição e domínios (0x758 - 0x770)
     */
    void                     *proc_partition_object;                 /* PartitionObject */
    BK_UINT64                proc_security_domain;                   /* SecurityDomain */
    BK_UINT64                proc_parent_security_domain;            /* ParentSecurityDomain */
    
    /*
     * Sampling e hot patch (0x770 - 0x780)
     */
    void                     *proc_coverage_sampler_context;         /* CoverageSamplerContext */
    void                     *proc_mm_hot_patch_context;             /* MmHotPatchContext */
    
    /*
     * Dynamic EH (0x780 - 0x7a0)
     */
    struct __bk_rtl_avl_tree proc_dynamic_eh_continuation_targets;   /* DynamicEHContinuationTargetsTree */
    struct __bk_ex_push_lock proc_dynamic_eh_continuation_lock;      /* DynamicEHContinuationTargetsLock */
    struct __bk_ps_dynamic_enforced_address_ranges proc_dynamic_enforced_cet_ranges; /* DynamicEnforcedCetCompatibleRanges */
    
    /*
     * Componentes e page combine (0x7a0 - 0x7b0)
     */
    BK_UINT32                proc_disabled_component_flags;          /* DisabledComponentFlags */
    BK_INT                   proc_page_combine_sequence;             /* PageCombineSequence */
    struct __bk_ex_push_lock proc_enable_optional_xstate_lock;       /* EnableOptionalXStateFeaturesLock */
    
    /*
     * Redirecionamento e syscalls (0x7b0 - 0x7d0)
     */
    BK_UINT32                *proc_path_redirection_hashes;          /* PathRedirectionHashes */
    struct __bk_ps_syscall_provider *proc_syscall_provider;          /* SyscallProvider */
    struct __bk_list_entry   proc_syscall_provider_links;            /* SyscallProviderProcessLinks */
    struct __bk_psp_syscall_provider_dispatch_context proc_syscall_dispatch_context; /* SyscallProviderDispatchContext */
    
    /*
     * Mitigation Flags 3 (0x7d8 - 0x7dc)
     */
    union {
        BK_UINT32           proc_mitigation_flags3;
        struct {
            BK_UINT32       proc_restrict_core_sharing        :1;    /* RestrictCoreSharing */
            BK_UINT32       proc_disallow_fsctl_syscalls      :1;    /* DisallowFsctlSystemCalls */
            BK_UINT32       proc_audit_disallow_fsctl         :1;    /* AuditDisallowFsctlSystemCalls */
            BK_UINT32       proc_mitigation_flags3_spare      :29;   /* Spare */
        } proc_mitigation_flags3_values;
    };
    
    /*
     * Flags4 (0x7dc - 0x7e0)
     */
    union {
        BK_UINT32           proc_flags4;
        struct {
            BK_UINT32       proc_thread_was_active            :1;    /* ThreadWasActive */
            BK_UINT32       proc_minimal_terminate            :1;    /* MinimalTerminate */
            BK_UINT32       proc_image_expansion_disable      :1;    /* ImageExpansionDisable */
            BK_UINT32       proc_session_first_process        :1;    /* SessionFirstProcess */
            BK_UINT32       proc_flags4_spare                 :28;   /* Spare */
        };
    };
    
    /*
     * Syscall usage (0x7e0 - 0x7e4)
     */
    union {
        BK_UINT32           proc_syscall_usage;
        struct {
            BK_UINT32       proc_sys_system_module_info       :1;    /* SystemModuleInformation */
            BK_UINT32       proc_sys_system_module_info_ex    :1;    /* SystemModuleInformationEx */
            BK_UINT32       proc_sys_locks_info               :1;    /* SystemLocksInformation */
            BK_UINT32       proc_sys_stacktrace_info          :1;    /* SystemStackTraceInformation */
            BK_UINT32       proc_sys_handle_info              :1;    /* SystemHandleInformation */
            BK_UINT32       proc_sys_extended_handle_info     :1;    /* SystemExtendedHandleInformation */
            BK_UINT32       proc_sys_object_info              :1;    /* SystemObjectInformation */
            BK_UINT32       proc_sys_bigpool_info             :1;    /* SystemBigPoolInformation */
            BK_UINT32       proc_sys_extended_process_info    :1;    /* SystemExtendedProcessInformation */
            BK_UINT32       proc_sys_session_process_info     :1;    /* SystemSessionProcessInformation */
            BK_UINT32       proc_sys_memory_topology_info     :1;    /* SystemMemoryTopologyInformation */
            BK_UINT32       proc_sys_memory_channel_info      :1;    /* SystemMemoryChannelInformation */
            BK_UINT32       proc_sys_unused                   :1;    /* SystemUnused */
            BK_UINT32       proc_sys_platform_binary_info     :1;    /* SystemPlatformBinaryInformation */
            BK_UINT32       proc_sys_firmware_table_info      :1;    /* SystemFirmwareTableInformation */
            BK_UINT32       proc_sys_boot_metadata_info       :1;    /* SystemBootMetadataInformation */
            BK_UINT32       proc_sys_whea_ipmi_info           :1;    /* SystemWheaIpmiHardwareInformation */
            BK_UINT32       proc_sys_superfetch_prefetch      :1;    /* SystemSuperfetchPrefetch */
            BK_UINT32       proc_sys_superfetch_pfn_query     :1;    /* SystemSuperfetchPfnQuery */
            BK_UINT32       proc_sys_superfetch_priv_source   :1;    /* SystemSuperfetchPrivSourceQuery */
            BK_UINT32       proc_sys_superfetch_memory_list   :1;    /* SystemSuperfetchMemoryListQuery */
            BK_UINT32       proc_sys_superfetch_memory_ranges :1;    /* SystemSuperfetchMemoryRangesQuery */
            BK_UINT32       proc_sys_superfetch_pfn_priority  :1;    /* SystemSuperfetchPfnSetPriority */
            BK_UINT32       proc_sys_superfetch_move_pages    :1;    /* SystemSuperfetchMovePages */
            BK_UINT32       proc_sys_superfetch_pfn_heat      :1;    /* SystemSuperfetchPfnSetPageHeat */
            BK_UINT32       proc_sys_dbg_triage_dump          :1;    /* SysDbgGetTriageDump */
            BK_UINT32       proc_sys_dbg_live_kernel_dump     :1;    /* SysDbgGetLiveKernelDump */
            BK_UINT32       proc_syscall_usage_spare          :5;    /* Spare */
        } proc_syscall_usage_values;
    };
    
    /*
     * Supervisor (0x7e4 - 0x7f0)
     */
    BK_INT                   proc_supervisor_device_asid;            /* SupervisorDeviceAsid */
    void                     *proc_supervisor_svm_data;              /* SupervisorSvmData */
    
    /*
     * Network counters (0x7f0 - 0x7f8)
     */
    struct __bk_process_network_counters *proc_network_counters;     /* NetworkCounters */
    
    /*
     * Execution (0x7f8 - 0x800)
     */
    union __bk_process_execution proc_execution;                      /* Execution */
    
    /*
     * Thread index e freeze (0x800 - 0x818)
     */
    void                     *proc_thread_index_table;                /* ThreadIndexTable */
    struct __bk_list_entry   proc_freeze_work_links;                  /* FreezeWorkLinks */
    
    /*
     * Padding para alinhamento (0x818 - 0x840)
     */
    BK_UINT8                 proc_padding[0x28];                      /* Padding */
};

/*
 * Cabeças de lista para processos
 */
BK_LIST_HEAD(__bk_process_list, __bk_process);
BK_TAILQ_HEAD(__bk_process_queue, __bk_process);

/*
 * Macros úteis
 */
#define BK_PROCESS_IS_64BIT(proc) \
    (((proc)->proc_flags & BK_TASK_FLAG_64BIT_ADDR) != 0)

#define BK_PROCESS_IS_SYSTEM(proc) \
    (((proc)->proc_flags3 & (1 << 12)) != 0)  /* proc_system_process */

#define BK_PROCESS_IS_CRASHED(proc) \
    (((proc)->proc_flags3 & (1 << 2)) != 0)   /* proc_crashed */

#define BK_PROCESS_HAS_JOB(proc) \
    ((proc)->proc_job != NULL)

#define BK_PROCESS_HAS_DEBUG_PORT(proc) \
    ((proc)->proc_debug_port != NULL)

#define BK_PROCESS_IS_EXITING(proc) \
    (((proc)->proc_flags & (1 << 2)) != 0)    /* proc_exiting */

#define BK_PROCESS_ACTIVE_THREADS(proc) \
    ((proc)->proc_active_threads)

#define BK_PROCESS_GET_PEB(proc) \
    ((proc)->proc_peb)

#define BK_PROCESS_GET_ID(proc) \
    ((proc)->proc_unique_id)

#define BK_PROCESS_GET_PARENT_ID(proc) \
    ((proc)->proc_inherited_from_pid)

#endif
