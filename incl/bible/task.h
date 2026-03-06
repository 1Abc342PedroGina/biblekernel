#ifndef _BIBLE_TASK_H_
#define _BIBLE_TASK_H_

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/queue.h>
#include <bible/pthread.h>
#include <bible/vm.h>
#include <bible/pmap.h>


/* Semáforo */
typedef struct __bk_semaphore {
    _INT_            sem_count;      /* Valor atual do semáforo */
    _INT_             sem_max;        /* Valor máximo */
    BK_SPINLOCK     sem_lock;       /* Lock interno */
    void            *sem_waiters;   /* Fila de threads esperando */
} BK_SEMAPHORE;

/* Mutex */
typedef struct __bk_mutex {
    BK_UINT32       mutex_owner;    /* Thread ID do proprietário */
    _INT_             mutex_count;    /* Contador de locks aninhados */
    BK_SPINLOCK     mutex_lock;     /* Lock interno */
    void            *mutex_waiters; /* Fila de threads esperando */
} BK_MUTEX;

/* Variável de condição */
typedef struct __bk_condvar {
    BK_SPINLOCK     cond_lock;      /* Lock interno */
    void            *cond_waiters;  /* Fila de threads esperando */
} BK_CONDVAR;

/* Signal set */
typedef BK_UINT32   BK_SIGSET;

/* Ação de sinal */
struct __bk_sigaction {
    void    (*sa_handler)(int);              /* Handler do sinal */
    BK_SIGSET   sa_mask;                      /* Máscara durante handler */
    _INT_         sa_flags;                      /* Flags */
};

/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2024 Bible System Developers
 * Todos os direitos reservados.
 *
 * Sistema de Gerenciamento de Tarefas do Bible Kernel
 * Arquitetura Microkernel: Atividades → Threads → Task → Process
 */

/*
 * Hierarquia do sistema de tarefas:
 * 
 * 1. Bible_Activity (Atividade): 
 *    - Unidade atômica de trabalho
 *    - Executada por uma thread
 *    - Contém contexto específico da atividade
 * 
 * 2. Bible_Thread (Thread):
 *    - Menor unidade de execução gerenciada pelo kernel
 *    - Executa atividades
 *    - Possui contexto de hardware e pilha
 * 
 * 3. Bible_Task (Task):
 *    - Ambiente de execução para threads
 *    - Gerencia recursos compartilhados entre threads
 *    - Contém espaço de endereço e descritores de arquivo
 * 
 * 4. Bible_Process (Process):
 *    - Atividades + Threads + Task + Gerenciamento próprio
 *    - Entidade completa de execução
 *    - Gerencia seu próprio ciclo de vida
 */

/*
 * ============================================================================
 * BIBLE ACTIVITY (Atividade)
 * ============================================================================
 */

/* Estados de uma atividade */
typedef enum {
	BK_ACTIVITY_READY	= 0,	/* Pronta para execução */
	BK_ACTIVITY_RUNNING	= 1,	/* Em execução */
	BK_ACTIVITY_BLOCKED	= 2,	/* Bloqueada (esperando recurso) */
	BK_ACTIVITY_SUSPENDED	= 3,	/* Suspensa */
	BK_ACTIVITY_COMPLETED	= 4,	/* Completada com sucesso */
	BK_ACTIVITY_FAILED	= 5,	/* Falhou */
	BK_ACTIVITY_CANCELLED	= 6,	/* Cancelada */
	BK_ACTIVITY_WAITING	= 7,	/* Esperando evento */
} BK_ACTIVITY_STATE;

/* Tipos de atividade */
typedef enum {
	BK_ACTIVITY_TYPE_GENERIC	= 0,	/* Atividade genérica */
	BK_ACTIVITY_TYPE_COMPUTATION	= 1,	/* Computação */
	BK_ACTIVITY_TYPE_IO		= 2,	/* Operação de I/O */
	BK_ACTIVITY_TYPE_SYSCALL	= 3,	/* Chamada de sistema */
	BK_ACTIVITY_TYPE_SIGNAL		= 4,	/* Manipulação de sinal */
	BK_ACTIVITY_TYPE_INTERRUPT	= 5,	/* Manipulação de interrupção */
	BK_ACTIVITY_TYPE_TIMER		= 6,	/* Atividade de timer */
	BK_ACTIVITY_TYPE_IPC		= 7,	/* Comunicação interprocesso */
	BK_ACTIVITY_TYPE_USER		= 8,	/* Atividade definida pelo usuário */
} BK_ACTIVITY_TYPE;

/* Prioridades de atividade */
#define BK_ACTIVITY_PRIORITY_IDLE		0
#define BK_ACTIVITY_PRIORITY_LOW		32
#define BK_ACTIVITY_PRIORITY_NORMAL		64
#define BK_ACTIVITY_PRIORITY_HIGH		96
#define BK_ACTIVITY_PRIORITY_REALTIME		127
#define BK_ACTIVITY_PRIORITY_DEFAULT		BK_ACTIVITY_PRIORITY_NORMAL

/* Estrutura de uma atividade */
struct __bk_activity {
	BK_ID		act_id;			/* ID único da atividade */
	BK_ACTIVITY_TYPE	act_type;	/* Tipo de atividade */
	BK_ACTIVITY_STATE	act_state;	/* Estado atual */
	BK_UINT8		act_priority;	/* Prioridade (0-127) */
	
	/* Callback e dados da atividade */
	void		(*act_handler)(void *);	/* Handler da atividade */
	void		*act_data;		/* Dados da atividade */
	BK_SIZE		act_data_size;		/* Tamanho dos dados */
	
	/* Contexto de execução */
	BK_VM_OFFSET	act_stack;		/* Pilha da atividade */
	BK_SIZE		act_stack_size;		/* Tamanho da pilha */
	BK_VM_OFFSET	act_context;		/* Contexto salvo */
	
	/* Metadados */
	BK_TIME		act_creation_time;	/* Tempo de criação */
	BK_TIME		act_start_time;		/* Tempo de início */
	BK_TIME		act_completion_time;	/* Tempo de conclusão */
	BK_UI64		act_cpu_cycles;		/* Ciclos de CPU consumidos */
	
	/* Thread e task associadas */
	struct __bk_thread	*act_thread;	/* Thread executando */
	struct __bk_task	*act_task;	/* Task proprietária */
	
	/* Listas */
	BK_LIST_ENTRY(__bk_activity) act_task_link;	/* Link na task */
	BK_LIST_ENTRY(__bk_activity) act_thread_link;	/* Link na thread */
	BK_LIST_ENTRY(__bk_activity) act_sched_link;	/* Link no escalonador */
	
	/* Sincronização */
	BK_SPINLOCK	act_lock;		/* Lock da atividade */
	BK_SEMAPHORE	act_completion_sem;	/* Semáforo de conclusão */
	
	/* Resultado */
	BK_I32		act_result;		/* Resultado da atividade */
	BK_I32		act_error;		/* Código de erro (se falhou) */
	
	/* Flags */
	BK_UINT32	act_flags;
#define BK_ACTIVITY_FLAG_DETACHED	0x0001	/* Atividade detached */
#define BK_ACTIVITY_FLAG_PERSISTENT	0x0002	/* Atividade persistente */
#define BK_ACTIVITY_FLAG_SYSTEM	0x0004	/* Atividade do sistema */
#define BK_ACTIVITY_FLAG_USER		0x0008	/* Atividade do usuário */
#define BK_ACTIVITY_FLAG_CRITICAL	0x0010	/* Atividade crítica */
};

typedef struct __bk_activity BK_ACTIVITY;

/* Lista de atividades */
BK_LIST_HEAD(__bk_activity_list, __bk_activity);
typedef struct __bk_activity_list BK_ACTIVITY_LIST;

/*
 * ============================================================================
 * BIBLE THREAD (Thread)
 * ============================================================================
 */

/* Estados de uma thread */
typedef enum {
	BK_THREAD_NEW		= 0,	/* Thread criada mas não inicializada */
	BK_THREAD_READY		= 1,	/* Pronta para execução */
	BK_THREAD_RUNNING	= 2,	/* Em execução */
	BK_THREAD_BLOCKED	= 3,	/* Bloqueada */
	BK_THREAD_SLEEPING	= 4,	/* Dormindo */
	BK_THREAD_SUSPENDED	= 5,	/* Suspensa */
	BK_THREAD_TERMINATED	= 6,	/* Terminada */
	BK_THREAD_ZOMBIE	= 7,	/* Thread zombie (aguardando join) */
} BK_THREAD_STATE;

/* Tipos de thread */
typedef enum {
	BK_THREAD_TYPE_KERNEL	= 0,	/* Thread do kernel */
	BK_THREAD_TYPE_USER	= 1,	/* Thread do usuário */
	BK_THREAD_TYPE_DAEMON	= 2,	/* Thread daemon */
	BK_THREAD_TYPE_WORKER	= 3,	/* Thread worker */
	BK_THREAD_TYPE_IDLE	= 4,	/* Thread idle */
} BK_THREAD_TYPE;

/* Contexto de hardware da thread (architecture specific) */
struct __bk_thread_context {
	BK_UL3264	ctx_regs[16];	/* Registradores gerais */
	BK_UL3264	ctx_pc;		/* Program Counter */
	BK_UL3264	ctx_sp;		/* Stack Pointer */
	BK_UL3264	ctx_fp;		/* Frame Pointer */
	BK_UL3264	ctx_flags;	/* Flags do processador */
	BK_UL3264	ctx_fpu[32];	/* Estado da FPU */
	BK_UL3264	ctx_vector[16];	/* Registradores vetoriais (SIMD) */
};

typedef struct __bk_thread_context BK_THREAD_CONTEXT;

/* Estrutura de uma thread */
struct __bk_thread {
	BK_LWPID	thr_id;		/* Thread ID (LWP ID) */
	BK_THREAD_TYPE	thr_type;	/* Tipo de thread */
	BK_THREAD_STATE	thr_state;	/* Estado atual */
	BK_UINT8	thr_priority;	/* Prioridade (0-255) */
	
	/* Contexto de execução */
	BK_THREAD_CONTEXT	thr_context;	/* Contexto de hardware */
	BK_VM_OFFSET		thr_stack;	/* Endereço da pilha */
	BK_SIZE			thr_stack_size;	/* Tamanho da pilha */
	BK_VM_OFFSET		thr_kstack;	/* Pilha do kernel */
	BK_SIZE			thr_kstack_size;	/* Tamanho da pilha do kernel */
	
	/* Atividade atual */
	BK_ACTIVITY		*thr_current_activity;	/* Atividade em execução */
	BK_ACTIVITY_LIST	thr_activity_queue;	/* Fila de atividades */
	BK_UI32			thr_activity_count;	/* Contador de atividades */
	
	/* Task proprietária */
	struct __bk_task	*thr_task;		/* Task a que pertence */
	
	/* Informações de escalonamento */
	BK_TIME		thr_creation_time;	/* Tempo de criação */
	BK_TIME		thr_last_run_time;	/* Última vez que executou */
	BK_UI64		thr_total_runtime;	/* Tempo total de execução */
	BK_UI64		thr_quantum;		/* Quantum restante */
	BK_UI32		thr_cpu_affinity;	/* Afinidade de CPU */
	
	/* Thread Local Storage (TLS) */
	BK_VM_OFFSET	thr_tls;		/* Área TLS */
	BK_SIZE		thr_tls_size;		/* Tamanho da área TLS */
	
	/* Sinais */
	BK_SIGSET	thr_sigpending;		/* Sinais pendentes */
	BK_SIGSET	thr_sigmask;		/* Máscara de sinais */
	void		(*thr_sighandlers[32])(BK_I32);	/* Handlers de sinal */
	
	/* Listas */
	BK_LIST_ENTRY(__bk_thread) thr_task_link;	/* Link na task */
	BK_LIST_ENTRY(__bk_thread) thr_sched_link;	/* Link no escalonador */
	BK_LIST_ENTRY(__bk_thread) thr_cpu_link;	/* Link na CPU */
	
	/* Sincronização */
	BK_SPINLOCK	thr_lock;		/* Lock da thread */
	BK_MUTEX	thr_join_mutex;		/* Mutex para join */
	BK_CONDVAR	thr_join_cond;		/* Variável de condição para join */
	
	/* Retorno e erro */
	BK_I32		thr_exit_code;		/* Código de saída */
	BK_I32		thr_error;		/* Código de erro */
	
	/* Flags */
	BK_UINT32	thr_flags;
#define BK_THREAD_FLAG_DETACHED	0x0001	/* Thread detached */
#define BK_THREAD_FLAG_SYSTEM	0x0002	/* Thread do sistema */
#define BK_THREAD_FLAG_SIGNALED	0x0004	/* Thread sinalizada */
#define BK_THREAD_FLAG_STOPPED	0x0008	/* Thread parada */
#define BK_THREAD_FLAG_CONTINUED	0x0010	/* Thread continuada */
#define BK_THREAD_FLAG_KERNEL	0x0020	/* Executando no kernel */
#define BK_THREAD_FLAG_USER	0x0040	/* Executando no usuário */
};

typedef struct __bk_thread BK_THREAD;


/* Lista de threads */
BK_LIST_HEAD(__bk_thread_list, __bk_thread);
typedef struct __bk_thread_list BK_THREAD_LIST;

/*
 * ============================================================================
 * BIBLE TASK (Task)
 * ============================================================================
 */

/* Estados de uma task */
typedef enum {
	BK_TASK_NEW		= 0,	/* Task criada mas não inicializada */
	BK_TASK_READY		= 1,	/* Pronta para execução */
	BK_TASK_RUNNING		= 2,	/* Em execução */
	BK_TASK_BLOCKED		= 3,	/* Bloqueada */
	BK_TASK_SUSPENDED	= 4,	/* Suspensa */
	BK_TASK_TERMINATED	= 5,	/* Terminada */
	BK_TASK_ZOMBIE		= 6,	/* Task zombie */
} BK_TASK_STATE;

/* Estrutura de uma task */
struct __bk_task {
	BK_ID		task_id;		/* ID único da task */
	BK_TASK_STATE	task_state;		/* Estado atual */
	BK_UINT8	task_priority;		/* Prioridade base */
	
	/* Espaço de endereço */
	BK_VM_ADDRSPACE	*task_addrspace;	/* Espaço de endereço virtual */
	
	/* Threads da task */
	BK_THREAD_LIST	task_threads;		/* Lista de threads */
	BK_THREAD	*task_main_thread;	/* Thread principal */
	BK_UI32		task_thread_count;	/* Número de threads */
	
	/* Atividades da task */
	BK_ACTIVITY_LIST	task_activities;	/* Lista de atividades */
	BK_UI32			task_activity_count;	/* Contador de atividades */
	
	/* Descritores de arquivo */
	struct __bk_fd_table	*task_fdtable;		/* Tabela de descritores */
	BK_UI32			task_max_fds;		/* Máximo de descritores */
	
	/* Recursos do sistema */
	BK_RLIM		task_rlimits[16];	/* Limites de recursos */
	BK_TIME		task_cpu_time;		/* Tempo total de CPU */
	BK_VM_SIZE	task_memory_usage;	/* Uso de memória */
	
	/* Informações do processo proprietário */
	struct __bk_process	*task_process;		/* Processo proprietário */
	
	/* Working Directory e ambiente */
	char		*task_cwd;		/* Diretório atual de trabalho */
	char		**task_envp;		/* Variáveis de ambiente */
	
	/* Sinais */
	BK_SIGSET	task_sigpending;	/* Sinais pendentes */
	BK_SIGSET	task_sigmask;		/* Máscara de sinais */
	void		(*task_sighandlers[32])(BK_I32);	/* Handlers */
	
	/* Listas */
	BK_LIST_ENTRY(__bk_task) task_proc_link;	/* Link no processo */
	BK_LIST_ENTRY(__bk_task) task_sched_link;	/* Link no escalonador */
	
	/* Sincronização */
	BK_SPINLOCK	task_lock;		/* Lock da task */
	BK_SEMAPHORE	task_completion_sem;	/* Semáforo de conclusão */
	
	/* IPC */
	struct __bk_ipc_space	*task_ipcspace;		/* Espaço IPC */
	
	/* Capabilities */
	BK_CAP_RIGHTS	*task_capabilities;	/* Capabilities da task */
	
	/* Metadados */
	BK_TIME		task_creation_time;	/* Tempo de criação */
	BK_TIME		task_start_time;	/* Tempo de início */
	BK_TIME		task_termination_time;	/* Tempo de término */
	
	/* Flags */
	BK_UINT32	task_flags;
#define BK_TASK_FLAG_SYSTEM	0x0001	/* Task do sistema */
#define BK_TASK_FLAG_USER	0x0002	/* Task do usuário */
#define BK_TASK_FLAG_DAEMON	0x0004	/* Task daemon */
#define BK_TASK_FLAG_PRIVILEGED	0x0008	/* Task privilegiada */
#define BK_TASK_FLAG_FOREGROUND	0x0010	/* Task em foreground */
#define BK_TASK_FLAG_BACKGROUND	0x0020	/* Task em background */
};

typedef struct __bk_task BK_TASK;

/* Lista de tasks */
BK_LIST_HEAD(__bk_task_list, __bk_task);
typedef struct __bk_task_list BK_TASK_LIST;

/*
 * ============================================================================
 * BIBLE PROCESS (Process)
 * ============================================================================
 */

/* Estados de um processo */
typedef enum {
	BK_PROCESS_NEW		= 0,	/* Processo criado */
	BK_PROCESS_READY	= 1,	/* Pronto para execução */
	BK_PROCESS_RUNNING	= 2,	/* Em execução */
	BK_PROCESS_BLOCKED	= 3,	/* Bloqueado */
	BK_PROCESS_SLEEPING	= 4,	/* Dormindo */
	BK_PROCESS_STOPPED	= 5,	/* Parado */
	BK_PROCESS_PADECIDO	= 6,	/* Processo zombie */
	BK_PROCESS_DEAD		= 7,	/* Processo morto */
} BK_PROCESS_STATE;

/* Estrutura de um processo */
struct __bk_process {
	BK_PID		proc_pid;		/* Process ID */
	BK_PID		proc_ppid;		/* Parent Process ID */
	BK_PID		proc_pgid;		/* Process Group ID */
	BK_PID		proc_sid;		/* Session ID */
	
	BK_PROCESS_STATE	proc_state;	/* Estado atual */
	BK_UINT8		proc_priority;	/* Prioridade base */
	
	/* Tasks do processo */
	BK_TASK_LIST	proc_tasks;		/* Lista de tasks */
	BK_TASK		*proc_main_task;	/* Task principal */
	BK_UI32		proc_task_count;	/* Número de tasks */
	
	/* Threads do processo (todas as threads de todas as tasks) */
	BK_THREAD_LIST	proc_all_threads;	/* Todas as threads */
	BK_UI32		proc_thread_count;	/* Número total de threads */
	
	/* Atividades do processo (todas as atividades) */
	BK_ACTIVITY_LIST	proc_all_activities;	/* Todas as atividades */
	BK_UI32			proc_activity_count;	/* Número total de atividades */
	
	/* Espaço de endereço principal */
	BK_VM_ADDRSPACE	*proc_addrspace;	/* Espaço de endereço principal */
	
	/* Gerenciamento de memória */
	BK_VM_SIZE	proc_vm_size;		/* Tamanho total da memória virtual */
	BK_PM_SIZE	proc_pm_size;		/* Tamanho total da memória física */
	BK_VM_OFFSET	proc_heap_start;	/* Início do heap */
	BK_VM_OFFSET	proc_heap_end;		/* Fim do heap */
	BK_VM_OFFSET	proc_stack_start;	/* Início da stack */
	BK_VM_OFFSET	proc_stack_end;		/* Fim da stack */
	
        struct __bk_process_list {
    struct __bk_process *plh_first;
    };
	/* Recursos do sistema */
	struct __bk_proc_resources {
		BK_RLIM		pr_cputime;	/* Limite de tempo de CPU */
		BK_RLIM		pr_filesize;	/* Limite de tamanho de arquivo */
		BK_RLIM		pr_datasize;	/* Limite de tamanho de dados */
		BK_RLIM		pr_stacksize;	/* Limite de tamanho de stack */
		BK_RLIM		pr_coredumpsize;	/* Limite de core dump */
		BK_RLIM		pr_memoryuse;	/* Limite de uso de memória */
		BK_RLIM		pr_vmemoryuse;	/* Limite de memória virtual */
		BK_RLIM		pr_locks;	/* Limite de locks */
		BK_RLIM		pr_sigpending;	/* Limite de sinais pendentes */
		BK_RLIM		pr_msgqueue;	/* Limite de fila de mensagens */
		BK_RLIM		pr_nice;	/* Limite de nice value */
		BK_RLIM		pr_rtprio;	/* Limite de prioridade em tempo real */
	} proc_rlimits;
	
	/* Informações do executável */
	char		*proc_exe_path;		/* Caminho do executável */
	char		*proc_argv0;		/* Argumento 0 (nome) */
	char		**proc_argv;		/* Argumentos */
	char		**proc_envp;		/* Variáveis de ambiente */
	
	/* Working Directory */
	char		*proc_cwd;		/* Diretório atual de trabalho */
	
	/* Usuário e grupo */
	BK_UID		proc_uid;		/* User ID */
	BK_UID		proc_euid;		/* Effective User ID */
	BK_UID		proc_suid;		/* Saved User ID */
	BK_GID		proc_gid;		/* Group ID */
	BK_GID		proc_egid;		/* Effective Group ID */
	BK_GID		proc_sgid;		/* Saved Group ID */
	
	/* Sinais */
	BK_SIGSET	proc_sigpending;	/* Sinais pendentes */
	BK_SIGSET	proc_sigmask;		/* Máscara de sinais */
	struct __bk_sigaction	proc_sigactions[32];	/* Ações de sinal */
	
	/* IPC */
	struct __bk_ipc_space	*proc_ipcspace;		/* Espaço IPC */
	
	/* Timers */
	struct __bk_timer	*proc_timers;		/* Timers do processo */
	
	/* File System */
	struct __bk_fs_context	*proc_fsctx;		/* Contexto do filesystem */
	
	/* Capabilities */
	BK_CAP_RIGHTS	*proc_capabilities;	/* Capabilities do processo */
	
	/* Listas */
	BK_LIST_ENTRY(__bk_process) proc_list_link;	/* Link na lista global */
	BK_LIST_ENTRY(__bk_process) proc_parent_link;	/* Link nos filhos do pai */
	BK_LIST_ENTRY(__bk_process) proc_child_link;	/* Link na lista de filhos */
	
	/* Relacionamento pai-filho */
	struct __bk_process	*proc_parent;		/* Processo pai */
	struct __bk_process_list		proc_children;		/* Processos filhos */
	BK_UI32			proc_child_count;	/* Número de filhos */
	
	/* Sincronização */
	BK_SPINLOCK	proc_lock;		/* Lock do processo */
	BK_MUTEX	proc_wait_mutex;	/* Mutex para wait */
	BK_CONDVAR	proc_wait_cond;		/* Variável de condição para wait */
	
	/* Estatísticas */
	BK_TIME		proc_creation_time;	/* Tempo de criação */
	BK_TIME		proc_start_time;	/* Tempo de início */
	BK_TIME		proc_user_time;		/* Tempo de usuário */
	BK_TIME		proc_system_time;	/* Tempo de sistema */
	BK_VM_SIZE	proc_peak_memory;	/* Pico de uso de memória */
	BK_UI64		proc_page_faults;	/* Número de page faults */
	BK_UI64		proc_swaps;		/* Número de swaps */
	
	/* Saída */
	BK_I32		proc_exit_status;	/* Status de saída */
	BK_I32		proc_term_signal;	/* Sinal de término */
	
	/* Flags */
	BK_UINT32	proc_flags;
#define BK_PROCESS_FLAG_SYSTEM		0x0001	/* Processo do sistema */
#define BK_PROCESS_FLAG_USER		0x0002	/* Processo do usuário */
#define BK_PROCESS_FLAG_DAEMON		0x0004	/* Processo daemon */
#define BK_PROCESS_FLAG_SESSION_LEADER	0x0008	/* Líder de sessão */
#define BK_PROCESS_FLAG_PROCESS_GROUP_LEADER	0x0010	/* Líder de grupo */
#define BK_PROCESS_FLAG_TRACED		0x0020	/* Processo sendo traçado */
#define BK_PROCESS_FLAG_STOPPED		0x0040	/* Processo parado */
#define BK_PROCESS_FLAG_CONTINUED	0x0080	/* Processo continuado */
#define BK_PROCESS_FLAG_PRIVILEGED	0x0100	/* Processo privilegiado */
#define BK_PROCESS_FLAG_NO_NEW_PRIVS	0x0200	/* Sem novos privilégios */
#define BK_PROCESS_FLAG_SECCOMP		0x0400	/* Seccomp ativo */
};

typedef struct __bk_process BK_PROCESS;

typedef struct __bk_process_list BK_PROCESS_LIST;

/*
 * ============================================================================
 * FUNÇÕES DO SISTEMA DE TAREFAS
 * ============================================================================
 */

/*
 * Funções para Activity
 */
BK_ACTIVITY *bk_activity_create(BK_ACTIVITY_TYPE type, 
			       void (*handler)(void *), 
			       void *data, BK_SIZE data_size,
			       BK_UINT8 priority);
BK_I32 bk_activity_destroy(BK_ACTIVITY *activity);
BK_I32 bk_activity_schedule(BK_ACTIVITY *activity, BK_THREAD *thread);
BK_I32 bk_activity_cancel(BK_ACTIVITY *activity);
BK_I32 bk_activity_wait(BK_ACTIVITY *activity, BK_TIME timeout);
BK_I32 bk_activity_get_result(BK_ACTIVITY *activity);
BK_ACTIVITY_STATE bk_activity_get_state(BK_ACTIVITY *activity);

/*
 * Funções para Thread
 */
BK_THREAD *bk_thread_create(BK_THREAD_TYPE type, BK_UINT8 priority,
			   BK_SIZE stack_size);
BK_I32 bk_thread_destroy(BK_THREAD *thread);
BK_I32 bk_thread_start(BK_THREAD *thread, 
		      void (*entry)(void *), void *arg);
BK_I32 bk_thread_join(BK_THREAD *thread, BK_TIME timeout);
BK_I32 bk_thread_suspend(BK_THREAD *thread);
BK_I32 bk_thread_resume(BK_THREAD *thread);
BK_I32 bk_thread_set_priority(BK_THREAD *thread, BK_UINT8 priority);
BK_I32 bk_thread_set_affinity(BK_THREAD *thread, BK_UI32 cpu_mask);
BK_THREAD *bk_thread_self(void);
BK_LWPID bk_thread_get_id(BK_THREAD *thread);

/*
 * Funções para Task
 */
BK_TASK *bk_task_create(BK_TASK_STATE initial_state, BK_UINT8 priority);
BK_I32 bk_task_destroy(BK_TASK *task);
BK_I32 bk_task_add_thread(BK_TASK *task, BK_THREAD *thread);
BK_I32 bk_task_remove_thread(BK_TASK *task, BK_THREAD *thread);
BK_I32 bk_task_add_activity(BK_TASK *task, BK_ACTIVITY *activity);
BK_I32 bk_task_remove_activity(BK_TASK *task, BK_ACTIVITY *activity);
BK_I32 bk_task_set_addrspace(BK_TASK *task, BK_VM_ADDRSPACE *addrspace);
BK_I32 bk_task_suspend(BK_TASK *task);
BK_I32 bk_task_resume(BK_TASK *task);
BK_TASK *bk_task_get_current(void);

/*
 * Funções para Process
 */
BK_PROCESS *bk_process_create(BK_PID ppid, const char *exe_path,
			     char *const argv[], char *const envp[]);
BK_I32 bk_process_destroy(BK_PROCESS *process);
BK_I32 bk_process_add_task(BK_PROCESS *process, BK_TASK *task);
BK_I32 bk_process_remove_task(BK_PROCESS *process, BK_TASK *task);
BK_I32 bk_process_fork(BK_PROCESS *parent, BK_PROCESS **child);
BK_I32 bk_process_exec(BK_PROCESS *process, const char *path,
		      char *const argv[], char *const envp[]);
BK_I32 bk_process_wait(BK_PROCESS *process, BK_I32 *status, BK_I32 options);
BK_I32 bk_process_kill(BK_PROCESS *process, BK_I32 sig);
BK_I32 bk_process_suspend(BK_PROCESS *process);
BK_I32 bk_process_resume(BK_PROCESS *process);
BK_PROCESS *bk_process_get_current(void);
BK_PID bk_process_get_pid(BK_PROCESS *process);

/*
 * Funções de gerenciamento do sistema
 */
BK_I32 bk_task_system_init(void);
void bk_task_system_shutdown(void);
BK_I32 bk_task_system_get_stats(struct __bk_task_system_stats *stats);

/*
 * Estrutura para estatísticas do sistema de tarefas
 */
struct __bk_task_system_stats {
	BK_UI32	tss_total_processes;
	BK_UI32	tss_total_tasks;
	BK_UI32	tss_total_threads;
	BK_UI32	tss_total_activities;
	BK_UI32	tss_running_processes;
	BK_UI32	tss_running_tasks;
	BK_UI32	tss_running_threads;
	BK_UI32	tss_running_activities;
	BK_UI32	tss_blocked_processes;
	BK_UI32	tss_blocked_tasks;
	BK_UI32	tss_blocked_threads;
	BK_UI32	tss_blocked_activities;
	BK_TIME	tss_total_uptime;
	BK_UI64	tss_context_switches;
	BK_UI64	tss_system_calls;
};

/*
 * Macros úteis
 */
#define BK_CURRENT_PROCESS()		bk_process_get_current()
#define BK_CURRENT_TASK()		bk_task_get_current()
#define BK_CURRENT_THREAD()		bk_thread_self()
#define BK_CURRENT_ACTIVITY()		(BK_CURRENT_THREAD() ? \
					 BK_CURRENT_THREAD()->thr_current_activity : NULL)

#define BK_IS_SYSTEM_PROCESS(p)		((p)->proc_flags & BK_PROCESS_FLAG_SYSTEM)
#define BK_IS_USER_PROCESS(p)		((p)->proc_flags & BK_PROCESS_FLAG_USER)
#define BK_IS_PRIVILEGED_PROCESS(p)	((p)->proc_flags & BK_PROCESS_FLAG_PRIVILEGED)

#define BK_IS_SYSTEM_THREAD(t)		((t)->thr_flags & BK_THREAD_FLAG_SYSTEM)
#define BK_IS_USER_THREAD(t)		((t)->thr_flags & BK_THREAD_FLAG_USER)

/*
 * Funções de debug
 */
#ifdef _BK_TASK_DEBUG
void bk_task_dump_process(BK_PROCESS *proc);
void bk_task_dump_task(BK_TASK *task);
void bk_task_dump_thread(BK_THREAD *thread);
void bk_task_dump_activity(BK_ACTIVITY *activity);
void bk_task_dump_system_stats(void);
#endif

#endif /* !_BIBLE_TASK_H_ */
