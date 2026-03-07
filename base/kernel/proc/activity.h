# ifndef PROC_ACTIVITY_H
# define PROC_ACTIVITY_H
# include <bible/task.h>
# include <bible/vm.h>
# include <bible/ipc.h>
# include <bible/interrupt.h>
# include <bible/exception.h>
# include <bible/kernl_object.h>
# include <bible/ktypes.h>

  /* Razões de bloqueio */
typedef enum {
	BK_ACT_BLOCK_NONE		= 0,
	BK_ACT_BLOCK_SLEEP		= 1,	/* Dormindo */
	BK_ACT_BLOCK_WAIT		= 2,	/* Esperando */
	BK_ACT_BLOCK_IPC_RECEIVE	= 3,	/* Recebendo IPC */
	BK_ACT_BLOCK_IPC_SEND		= 4,	/* Enviando IPC */
	BK_ACT_BLOCK_SEMAPHORE		= 5,	/* Semáforo */
	BK_ACT_BLOCK_MUTEX		= 6,	/* Mutex */
	BK_ACT_BLOCK_CONDVAR		= 7,	/* Variável de condição */
	BK_ACT_BLOCK_IO			= 8,	/* Operação de I/O */
	BK_ACT_BLOCK_PAGEFAULT		= 9,	/* Page fault */
	BK_ACT_BLOCK_SUSPENDED		= 10,	/* Suspensa */
	BK_ACT_BLOCK_TERMINATING	= 11,	/* Terminando */
} BK_ACT_BLOCK_REASON;

/* Evento de wait (64 bits para compatibilidade) */
typedef __BK_UL3264 BK_EVENT64;

/* Timer para deadlines */
typedef struct __bk_act_timer {
	BK_TIME		at_deadline;		/* Deadline absoluta */
	BK_TIME		at_interval;		/* Intervalo se periódico */
	__BK_UI32	at_flags;		/* Flags do timer */
	void		(*at_handler)(void *);	/* Handler do timer */
	void		*at_data;		/* Dados do handler */
	struct __bk_act_timer *at_next;		/* Próximo timer na lista */
} BK_ACT_TIMER;

/* Estatísticas de CPU */
typedef struct __bk_act_cpu_stats {
	__BK_UL3264		acs_total_cycles;	/* Ciclos totais */
	__BK_UL3264		acs_user_cycles;	/* Ciclos em modo usuário */
	__BK_UL3264		acs_kernel_cycles;	/* Ciclos em modo kernel */
	__BK_UL3264		acs_last_run_time;	/* Último momento de execução */
	__BK_UL3264		acs_last_runnable_time;	/* Último momento runnable */
	BK_UI32		acs_context_switches;	/* Total de context switches */
	BK_UI32		acs_processor_switches;	/* Trocas de processador */
	BK_UI32		acs_pset_switches;	/* Trocas de conjunto */
} BK_ACT_CPU_STATS;

/*
 * Estrutura principal da atividade
 * Inspirada na thread do XNU com adaptações para o Bible Kernel
 */
struct activity {
	/*
	 * Magic number para debug - verifica integridade da estrutura
	 */
	__BK_UL3264			act_magic;		/* BK_ACTIVITY_MAGIC */
	
	/*
	 * Links para filas (run queue, wait queue, etc.)
	 * União para permitir diferentes tipos de encadeamento
	 */
	union {
		BK_LIST_ENTRY(__bk_activity)	act_runq_link;	/* Fila de run */
		BK_LIST_ENTRY(__bk_activity)	act_waitq_link;	/* Fila de wait */
		BK_LIST_ENTRY(__bk_activity)	act_task_link;	/* Link na task */
		BK_LIST_ENTRY(__bk_activity)	act_thread_link;	/* Link na thread */
		BK_LIST_ENTRY(__bk_activity)	act_sched_link;	/* Link no escalonador */
	};
	
	/*
	 * Evento de wait - usado para esperar por eventos
	 */
	BK_EVENT64		act_wait_event;		/* Evento aguardado */
	
	/*
	 * Wait queue atual - fila onde esta atividade está enfileirada
	 */
	void			*act_waitq;		/* Wait queue atual */
	
	/*
	 * Herdeiro de prioridade - quem herda prioridade durante PI
	 */
	struct activity	*act_inheritor;		/* Herdeiro de prioridade */
	
	/*
	 * Estado atual e flags
	 */
	BK_ACTIVITY_STATE	act_state;		/* Estado atual */
	BK_ACT_BLOCK_REASON	act_block_reason;	/* Razão do bloqueio */
	__BK_UI16		act_options;		/* Opções (BK_ACT_FLAG_*) */
	__BK_UI16		act_sched_flags;	/* Flags de escalonamento */
	
	/*
	 * Wait ativo e flags de wake
	 */
__BK_UI32		act_wake_active:1,	/* Wake ativo */
				act_at_safe_point:1,	/* Em ponto seguro */
				act_timer_armed:1,	/* Timer armado */
				act_pinned:1,		/* Pinado no processador */
				:0;
	
	/*
	 * Resultado do wait e continuação
	 */
	 __BK_I32			act_wait_result;	/* Resultado do wait */
	void			(*act_continuation)(void *);	/* Continuação */
	void			*act_continuation_data;	/* Dados da continuação */
	
	/*
	 * Dados da atividade
	 */
	void			*act_data;		/* Dados da atividade */
	BK_SIZE			act_data_size;		/* Tamanho dos dados */
	
	/*
	 * Pilha e contexto
	 */
	BK_VM_OFFSET		act_stack;		/* Pilha atual */
	BK_VM_OFFSET		act_reserved_stack;	/* Pilha reservada */
	BK_SIZE			act_stack_size;		/* Tamanho da pilha */
	BK_VM_OFFSET		act_context;		/* Contexto salvo */
	
	/*
	 * Estado específico da máquina (arquitetura)
	 */
	struct __bk_act_machine {
		BK_UL3264	act_regs[32];		/* Registradores */
		BK_UL3264	act_pc;			/* Program Counter */
		BK_UL3264	act_sp;			/* Stack Pointer */
		BK_UL3264	act_fp;			/* Frame Pointer */
		BK_UL3264	act_flags;		/* Flags do processador */
		BK_UL3264	act_fpu[64];		/* Estado FPU/SIMD */
	} act_machine;
	
	/*
	 * Informações de escalonamento
	 */
	__BK_UI8		act_base_pri;		/* Prioridade base */
	__BK_UI8		act_effective_pri;	/* Prioridade efetiva */
	__BK_UI8		act_sched_pri;		/* Prioridade escalonada */
	__BK_UI8		act_max_priority;	/* Prioridade máxima */
	__BK_UI8		act_saved_pri;		/* Prioridade salva */
	__BK_UI8		act_promotion_count;	/* Contador de promoções */
	
	__BK_UI32		act_quantum;		/* Quantum restante */
	__BK_UI32		act_quantum_initial;	/* Quantum inicial */
	
	/*
	 * Afinidade de processador
	 */
	__BK_UI32		act_cpu_affinity;	/* Máscara de afinidade */
	__BK_UI32		act_last_processor;	/* Último processador */
	__BK_UI32		act_bound_processor;	/* Processador bound */
	
	/*
	 * Temporizadores e deadlines
	 */
	BK_ACT_TIMER		*act_timers;		/* Timers ativos */
	BK_TIME			act_deadline;		/* Deadline absoluta */
	BK_TIME			act_wake_time;		/* Tempo para acordar */
	
	/*
	 * Estatísticas de CPU e tempo
	 */
	BK_ACT_CPU_STATS	act_cpu_stats;		/* Estatísticas de CPU */
	
	__BK_UL3264			act_creation_time;	/* Tempo de criação */
	__BK_UL3264			act_start_time;		/* Tempo de início */
	__BK_UL3264			act_completion_time;	/* Tempo de conclusão */
	__BK_UL3264			act_last_run_time;	/* Última execução */
	__BK_UL3264			act_total_runtime;	/* Tempo total de execução */
	
	/*
	 * Dados de IPC e mensagens
	 */
	struct {
		BK_QUEUE_HEAD	act_msg_queue;		/* Fila de mensagens */
		__BK_UI32	act_msg_count;		/* Contador de mensagens */
		void		*act_reply_port;	/* Porto de resposta */
	} act_ipc;
	
	/*
	 * Voucher e herança de importância
	 */
	void			*act_voucher;		/* Voucher atual */
	__BK_UI32		act_importance;		/* Importância */
	
	/*
	 * Estado para continuação/bloqueio
	 */
	union {
		/* Estado para recepção IPC */
		struct {
			void		*act_recv_buffer;	/* Buffer de recepção */
			BK_SIZE		act_recv_size;		/* Tamanho do buffer */
			__BK_UI32	act_recv_options;	/* Opções de recepção */
			BK_I32		act_recv_result;	/* Resultado da recepção */
		} act_ipc_receive;
		
		/* Estado para semáforo */
		struct {
			struct __bk_semaphore	*act_sem;	/* Semáforo */
			BK_I32			act_sem_result;	/* Resultado */
		} act_semaphore;
		
		/* Estado para I/O */
		struct {
			void		*act_io_buffer;		/* Buffer de I/O */
			BK_SIZE		act_io_size;		/* Tamanho */
			BK_OFFSET	act_io_offset;		/* Offset */
			BK_I32		act_io_result;		/* Resultado */
		} act_io;
	} act_saved_state;
	
	/*
	 * Contadores de suspensão
	 */
	BK_I32			act_suspend_count;	/* Suspensões do kernel */
	BK_I32			act_user_suspend_count;	/* Suspensões do usuário */
	
	/*
	 * Thread e task associadas
	 */
	struct __bk_thread	*act_thread;		/* Thread executando */
	struct __bk_task	*act_task;		/* Task proprietária */
	
	/*
	 * Referência e contagem
	 */
	__BK_UI32		act_ref_count;		/* Contagem de referências */
	
	/*
	 * Locks de sincronização
	 */
	BK_SPINLOCK		act_lock;		/* Lock principal */
	BK_SPINLOCK		act_wake_lock;		/* Lock para wake */
	BK_SPINLOCK		act_sched_lock;		/* Lock de escalonamento */
	
	/*
	 * Wait queue específica para timed waits
	 */
	void			*act_timer_waitq;	/* Wait queue do timer */
	
	/*
	 * Resultado e erro
	 */
	BK_I32			act_result;		/* Resultado da atividade */
	BK_I32			act_error;		/* Código de erro */
	
	/*
	 * Metadados adicionais
	 */
	__BK_UI32		act_decompressions;	/* Contador de decompressões */
	__BK_UI32		act_page_faults;	/* Page faults */
	
	/*
	 * Ponteiro para dados específicos do usuário
	 */
	void			*act_user_data;		/* Dados do usuário */
	
	/*
	 * Lista de timers ativos
	 */
	BK_LIST_HEAD(, __bk_act_timer)	act_timer_list;	/* Lista de timers */
	
	/*
	 * Flags de AST (Asynchronous System Trap)
	 */
	__BK_UI32		act_ast_pending;	/* ASTs pendentes */
# define BK_AST_NONE		0x00000000
# define BK_AST_SIGNAL		0x00000001	/* Sinal pendente */
# define BK_AST_SUSPEND		0x00000002	/* Suspender */
# define BK_AST_RESUME		0x00000004	/* Resumir */
# define BK_AST_TERMINATE	0x00000008	/* Terminar */
# define BK_AST_TIMER		0x00000010	/* Timer expirou */
# define BK_AST_URGENT		0x00000020	/* AST urgente */
# define BK_AST_PREEMPT		0x00000040	/* Preempção */
# define BK_AST_QUANTUM		0x00000080	/* Quantum expirou */
};

static inline BK_BOOL
bk_activity_validate(BK_ACTIVITY *act)
{
	return act != NULL && act->act_magic == BK_ACTIVITY_MAGIC;
}

static inline BK_BOOL
bk_activity_is_runnable(BK_ACTIVITY *act)
{
	return act->act_state == BK_ACTIVITY_READY || 
	       act->act_state == BK_ACTIVITY_RUNNING;
}

static inline BK_BOOL
bk_activity_is_blocked(BK_ACTIVITY *act)
{
	return act->act_state == BK_ACTIVITY_BLOCKED ||
	       act->act_state == BK_ACTIVITY_WAITING;
}

static inline BK_BOOL
bk_activity_is_suspended(BK_ACTIVITY *act)
{
	return act->act_suspend_count > 0 || 
	       act->act_user_suspend_count > 0 ||
	       act->act_state == BK_ACTIVITY_SUSPENDED;
}

static inline void
bk_activity_set_priority(BK_ACTIVITY *act, __BK_UI8 priority)
{
	act->act_base_pri = priority;
	act->act_effective_pri = priority;
	act->act_sched_pri = priority;
}

static inline __BK_UI8
bk_activity_get_effective_priority(BK_ACTIVITY *act)
{
	/* Se promovida, retorna prioridade efetiva, senão base */
	if (act->act_sched_flags & BK_ACT_SFLAG_PROMOTED) {
		return act->act_sched_pri;
	}
	return act->act_base_pri;
}

/*
 * Inicializador de atividade (para uso interno)
 */
# define BK_ACTIVITY_INITIALIZER(handler, data, priority) { \
	.act_magic = BK_ACTIVITY_MAGIC, \
	.act_state = BK_ACTIVITY_READY, \
	.act_block_reason = BK_ACT_BLOCK_NONE, \
	.act_base_pri = (priority), \
	.act_effective_pri = (priority), \
	.act_sched_pri = (priority), \
	.act_handler = (handler), \
	.act_data = (data), \
	.act_ref_count = 1, \
	.act_creation_time = 0, \
	.act_quantum_initial = 10, \
	.act_quantum = 10, \
}

#endif /* !PROC_ACTIVITY_H */
