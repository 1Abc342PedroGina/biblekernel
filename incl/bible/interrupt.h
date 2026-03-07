#ifndef _BIBLE_INTERRUPT_H_
#define _BIBLE_INTERRUPT_H_

/*
 * interrupt.h - Sistema de Interrupções do BibleKernel
 * 
 * "Interrupções são Registros" - Extensão da filosofia central
 * 
 * Interrupções são registros especiais que representam:
 * - Interrupções de hardware (IRQ, FIQ, NMI)
 * - Interrupções de software (syscalls, int3, int n)
 * - Exceções de CPU (faults, traps, aborts)
 * - IPIs (Inter-Processor Interrupts)
 * - Eventos assíncronos de dispositivos
 */

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/queue.h>
#include <bible/regsister.h>
#include <bible/task.h>
#include <bible/exception.h>
#include <bible/ipc.h>
#include <bible/vm.h>
#include <bible/pmap.h>

/*
 * ============================================================================
 * NÍVEIS DE INTERRUPÇÃO
 * ============================================================================
 */

typedef enum {
	BK_IRQL_PASSIVE		= 0,	/* Nível mais baixo, threads normais */
	BK_IRQL_APC		= 1,	/* Chamada de procedimento assíncrono */
	BK_IRQL_DPC		= 2,	/* Chamada de procedimento adiado */
	BK_IRQL_SOFTWARE	= 3,	/* Interrupções de software (int n) */
	BK_IRQL_CLOCK		= 4,	/* Interrupção de clock/timer */
	BK_IRQL_DEVICE_LOW	= 5,	/* Dispositivos de baixa prioridade */
	BK_IRQL_DEVICE_MEDIUM	= 6,	/* Dispositivos de média prioridade */
	BK_IRQL_DEVICE_HIGH	= 7,	/* Dispositivos de alta prioridade */
	BK_IRQL_IPI		= 8,	/* Inter-processor interrupt */
	BK_IRQL_PROFILING	= 9,	/* Profiling */
	BK_IRQL_ERROR		= 10,	/* Erro de hardware (machine check) */
	BK_IRQL_NMI		= 11,	/* Non-maskable interrupt */
	BK_IRQL_HIGHEST		= 15,	/* Nível mais alto (debug, crashes) */
} BK_IRQL;

/*
 * ============================================================================
 * TIPOS DE INTERRUPÇÃO
 * ============================================================================
 */

typedef enum __BK_ENUM_INTERRUPT_UINT16{
	/* Interrupções de hardware (0x2000-0x20FF) */
	BK_INTERRUPT_TYPE_HW_BASE		= 0x2000,
	
	BK_INTERRUPT_IRQ0			= 0x2000,	/* Programmable Interval Timer */
	BK_INTERRUPT_IRQ1			= 0x2001,	/* Keyboard */
	BK_INTERRUPT_IRQ2			= 0x2002,	/* Cascade for 8259A */
	BK_INTERRUPT_IRQ3			= 0x2003,	/* Serial port 2 */
	BK_INTERRUPT_IRQ4			= 0x2004,	/* Serial port 1 */
	BK_INTERRUPT_IRQ5			= 0x2005,	/* Parallel port 2 or sound card */
	BK_INTERRUPT_IRQ6			= 0x2006,	/* Floppy disk */
	BK_INTERRUPT_IRQ7			= 0x2007,	/* Parallel port 1 */
	BK_INTERRUPT_IRQ8			= 0x2008,	/* RTC (Real Time Clock) */
	BK_INTERRUPT_IRQ9			= 0x2009,	/* ACPI */
	BK_INTERRUPT_IRQ10			= 0x200A,	/* Available */
	BK_INTERRUPT_IRQ11			= 0x200B,	/* Available */
	BK_INTERRUPT_IRQ12			= 0x200C,	/* Mouse (PS/2) */
	BK_INTERRUPT_IRQ13			= 0x200D,	/* Math coprocessor */
	BK_INTERRUPT_IRQ14			= 0x200E,	/* Primary ATA channel */
	BK_INTERRUPT_IRQ15			= 0x200F,	/* Secondary ATA channel */
	
	BK_INTERRUPT_MSI			= 0x2010,	/* Message Signaled Interrupt */
	BK_INTERRUPT_MSIX			= 0x2011,	/* MSI-X */
	
	/* Interrupções de software (0x2100-0x21FF) */
	BK_INTERRUPT_TYPE_SW_BASE		= 0x2100,
	
	BK_INTERRUPT_SYSCALL			= 0x2100,	/* Chamada de sistema */
	BK_INTERRUPT_SOFTIRQ_0			= 0x2101,	/* Software IRQ 0 */
	BK_INTERRUPT_SOFTIRQ_1			= 0x2102,	/* Software IRQ 1 */
	BK_INTERRUPT_SOFTIRQ_2			= 0x2103,	/* Software IRQ 2 */
	BK_INTERRUPT_SOFTIRQ_3			= 0x2104,	/* Software IRQ 3 */
	BK_INTERRUPT_SOFTIRQ_4			= 0x2105,	/* Software IRQ 4 */
	BK_INTERRUPT_SOFTIRQ_5			= 0x2106,	/* Software IRQ 5 */
	BK_INTERRUPT_SOFTIRQ_6			= 0x2107,	/* Software IRQ 6 */
	BK_INTERRUPT_SOFTIRQ_7			= 0x2108,	/* Software IRQ 7 */
	
	BK_INTERRUPT_INT3			= 0x2110,	/* Breakpoint */
	BK_INTERRUPT_INT1			= 0x2111,	/* Debug trap */
	BK_INTERRUPT_BOUND			= 0x2112,	/* BOUND instruction */
	
	/* IPI (Inter-Processor Interrupts) (0x2200-0x22FF) */
	BK_INTERRUPT_TYPE_IPI_BASE		= 0x2200,
	
	BK_INTERRUPT_IPI_RESCHEDULE		= 0x2200,	/* Reschedule on other CPU */
	BK_INTERRUPT_IPI_CALL_FUNCTION		= 0x2201,	/* Call function on other CPU */
	BK_INTERRUPT_IPI_TLB_SHOOTDOWN		= 0x2202,	/* TLB flush on other CPUs */
	BK_INTERRUPT_IPI_STOP			= 0x2203,	/* Stop CPU */
	BK_INTERRUPT_IPI_PANIC			= 0x2204,	/* Panic on other CPU */
	BK_INTERRUPT_IPI_DEBUG			= 0x2205,	/* Debug interrupt */
	BK_INTERRUPT_IPI_PROFILING		= 0x2206,	/* Profiling sample */
	
	/* Interrupções de CPU (exceções) (0x2300-0x23FF) */
	BK_INTERRUPT_TYPE_CPU_BASE		= 0x2300,
	
	BK_INTERRUPT_CPU_DIVIDE_ERROR		= 0x2300,	/* Divisão por zero */
	BK_INTERRUPT_CPU_DEBUG			= 0x2301,	/* Debug exception */
	BK_INTERRUPT_CPU_NMI			= 0x2302,	/* Non-maskable interrupt */
	BK_INTERRUPT_CPU_BREAKPOINT		= 0x2303,	/* Breakpoint */
	BK_INTERRUPT_CPU_OVERFLOW		= 0x2304,	/* Overflow */
	BK_INTERRUPT_CPU_BOUND_RANGE		= 0x2305,	/* Bound range exceeded */
	BK_INTERRUPT_CPU_UNDEFINED_OPCODE	= 0x2306,	/* Invalid opcode */
	BK_INTERRUPT_CPU_DEVICE_NOT_AVAILABLE	= 0x2307,	/* Device not available */
	BK_INTERRUPT_CPU_DOUBLE_FAULT		= 0x2308,	/* Double fault */
	BK_INTERRUPT_CPU_COPROCESSOR_SEGMENT	= 0x2309,	/* Coprocessor segment overrun */
	BK_INTERRUPT_CPU_INVALID_TSS		= 0x230A,	/* Invalid TSS */
	BK_INTERRUPT_CPU_SEGMENT_NOT_PRESENT	= 0x230B,	/* Segment not present */
	BK_INTERRUPT_CPU_STACK_FAULT		= 0x230C,	/* Stack fault */
	BK_INTERRUPT_CPU_GENERAL_PROTECTION	= 0x230D,	/* General protection fault */
	BK_INTERRUPT_CPU_PAGE_FAULT		= 0x230E,	/* Page fault */
	BK_INTERRUPT_CPU_FPU_ERROR		= 0x2310,	/* FPU error */
	BK_INTERRUPT_CPU_ALIGNMENT_CHECK	= 0x2311,	/* Alignment check */
	BK_INTERRUPT_CPU_MACHINE_CHECK		= 0x2312,	/* Machine check */
	BK_INTERRUPT_CPU_SIMD			= 0x2313,	/* SIMD exception */
	BK_INTERRUPT_CPU_VIRTUALIZATION		= 0x2314,	/* Virtualization exception */
	BK_INTERRUPT_CPU_HYPERVISOR		= 0x2315,	/* Hypervisor exception */
	
	/* Interrupções de dispositivos específicos (0x2400-0x24FF) */
	BK_INTERRUPT_TYPE_DEVICE_BASE		= 0x2400,
	
	BK_INTERRUPT_DEVICE_TIMER		= 0x2400,	/* Timer/Hardclock */
	BK_INTERRUPT_DEVICE_KEYBOARD		= 0x2401,	/* Teclado */
	BK_INTERRUPT_DEVICE_MOUSE		= 0x2402,	/* Mouse */
	BK_INTERRUPT_DEVICE_DISK			= 0x2403,	/* Disco */
	BK_INTERRUPT_DEVICE_NETWORK		= 0x2404,	/* Placa de rede */
	BK_INTERRUPT_DEVICE_USB			= 0x2405,	/* USB controller */
	BK_INTERRUPT_DEVICE_AUDIO		= 0x2406,	/* Áudio */
	BK_INTERRUPT_DEVICE_VIDEO		= 0x2407,	/* Vídeo/GPU */
	BK_INTERRUPT_DEVICE_ACPI		= 0x2408,	/* ACPI */
	BK_INTERRUPT_DEVICE_PCI			= 0x2409,	/* PCI bus */
	BK_INTERRUPT_DEVICE_DMA			= 0x240A,	/* DMA controller */
	
	/* Interrupções de sistema (0x2500-0x25FF) */
	BK_INTERRUPT_TYPE_SYSTEM_BASE		= 0x2500,
	
	BK_INTERRUPT_SYSTEM_CLOCK		= 0x2500,	/* System clock tick */
	BK_INTERRUPT_SYSTEM_SCHEDULER		= 0x2501,	/* Scheduler tick */
	BK_INTERRUPT_SYSTEM_APC			= 0x2502,	/* APC delivery */
	BK_INTERRUPT_SYSTEM_DPC			= 0x2503,	/* DPC delivery */
	BK_INTERRUPT_SYSTEM_POWER		= 0x2504,	/* Power management */
	BK_INTERRUPT_SYSTEM_SHUTDOWN		= 0x2505,	/* Shutdown */
	BK_INTERRUPT_SYSTEM_REBOOT		= 0x2506,	/* Reboot */
	BK_INTERRUPT_SYSTEM_PANIC		= 0x2507,	/* Kernel panic */
	
	/* Interrupções de virtualização (0x2600-0x26FF) */
	BK_INTERRUPT_TYPE_VIRT_BASE		= 0x2600,
	
	BK_INTERRUPT_VIRT_VMENTRY		= 0x2600,	/* VM entry */
	BK_INTERRUPT_VIRT_VMEXIT		= 0x2601,	/* VM exit */
	BK_INTERRUPT_VIRT_HYPERCALL		= 0x2602,	/* Hypercall */
} BK_INTERRUPT_TYPE;

/*
 * ============================================================================
 * FLAGS DE INTERRUPÇÃO
 * ============================================================================
 */

#define BK_INTERRUPT_FLAG_EDGE		0x0001	/* Edge-triggered */
#define BK_INTERRUPT_FLAG_LEVEL		0x0002	/* Level-triggered */
#define BK_INTERRUPT_FLAG_MASKABLE	0x0004	/* Pode ser mascarada */
#define BK_INTERRUPT_FLAG_NMI		0x0008	/* Non-maskable */
#define BK_INTERRUPT_FLAG_SHARED	0x0010	/* Compartilhada */
#define BK_INTERRUPT_FLAG_EXCLUSIVE	0x0020	/* Exclusiva */
#define BK_INTERRUPT_FLAG_HARDWARE	0x0040	/* Interrupção de hardware */
#define BK_INTERRUPT_FLAG_SOFTWARE	0x0080	/* Interrupção de software */
#define BK_INTERRUPT_FLAG_WAKEUP	0x0100	/* Wake-up from sleep */
#define BK_INTERRUPT_FLAG_PERCPU	0x0200	/* Per-CPU interrupt */
#define BK_INTERRUPT_FLAG_REENTRANT	0x0400	/* Reentrante */
#define BK_INTERRUPT_FLAG_EOI		0x0800	/* Requer EOI */
#define BK_INTERRUPT_FLAG_EOI_BEFORE	0x1000	/* EOI antes do handler */
#define BK_INTERRUPT_FLAG_EOI_AFTER	0x2000	/* EOI depois do handler */

/*
 * ============================================================================
 * REGISTRO DE INTERRUPÇÃO
 * ============================================================================
 */

struct bk_interrupt_register {
	BK_REGISTER_HEADER		header;
	
	/* Identificação */
	BK_INTERRUPT_TYPE		int_type;
	BK_UINT32			int_number;	/* Número da IRQ/vector */
	BK_UINT32			int_cpu;	/* CPU alvo (para IPI/per-CPU) */
	
	/* Características */
	BK_IRQL				irql;		/* Nível de IRQL */
	BK_UINT32			flags;		/* Flags (BK_INTERRUPT_FLAG_*) */
	
	/* Contagem de ocorrências */
	BK_UINT64			count;		/* Total de ocorrências */
	BK_UINT64			last_occurrence;	/* Timestamp da última */
	BK_UINT64			occurrence_rate;	/* Taxa (por segundo) */
	
	/* Handler */
	BK_REGISTER_HANDLE		handler;	/* Handler registrado */
	void				(*handler_func)(struct bk_interrupt_register *,
							void *);
	void				*handler_arg;
	
	/* Estado atual */
	BK_BOOLEAN			masked;		/* Está mascarada? */
	BK_BOOLEAN			pending;	/* Pendente? */
	BK_UINT32			active_cpus;	/* CPUs onde está ativa */
	
	/* Dispositivo associado */
	BK_REGISTER_HANDLE		device;		/* Dispositivo que gerou */
	
	/* Estatísticas */
	BK_UINT64			handled_count;	/* Quantas foram tratadas */
	BK_UINT64			spurious_count;	/* Spurious interrupts */
	BK_UINT64			late_count;	/* Chegaram tarde */
	BK_UINT64			missed_deadline;	/* Perderam deadline */
	
	/* Listas */
	BK_LIST_ENTRY(bk_interrupt_register) int_list;	/* Lista global */
	BK_LIST_ENTRY(bk_interrupt_register) cpu_list;	/* Lista por CPU */
};

typedef struct bk_interrupt_register BK_INTERRUPT_REGISTER;

/*
 * ============================================================================
 * CONTROLLER DE INTERRUPÇÃO
 * ============================================================================
 */

struct bk_interrupt_controller {
	BK_REGISTER_HEADER		header;
	
	/* Identificação */
	char				name[32];
	BK_UINT32			version;
	
	/* Capabilidades */
	BK_UINT32			max_interrupts;
	BK_UINT32			supported_flags;
	BK_BOOLEAN			supports_msi;
	BK_BOOLEAN			supports_msix;
	
	/* Operações */
	BK_I32	(*enable)(struct bk_interrupt_controller *ctrl,
			  BK_UINT32 int_number);
	BK_I32	(*disable)(struct bk_interrupt_controller *ctrl,
			   BK_UINT32 int_number);
	BK_I32	(*mask)(struct bk_interrupt_controller *ctrl,
			BK_UINT32 int_number);
	BK_I32	(*unmask)(struct bk_interrupt_controller *ctrl,
			  BK_UINT32 int_number);
	BK_I32	(*eoi)(struct bk_interrupt_controller *ctrl,
		       BK_UINT32 int_number);
	BK_I32	(*configure)(struct bk_interrupt_controller *ctrl,
			    BK_UINT32 int_number,
			    BK_UINT32 flags);
	BK_I32	(*get_pending)(struct bk_interrupt_controller *ctrl,
			       BK_UINT32 int_number,
			       BK_BOOLEAN *pending);
	BK_I32	(*set_affinity)(struct bk_interrupt_controller *ctrl,
				BK_UINT32 int_number,
				BK_UINT32 cpu_mask);
	
	/* MSI/MSI-X */
	BK_I32	(*allocate_msi)(struct bk_interrupt_controller *ctrl,
				BK_UINT32 count,
				BK_UINT32 *int_numbers);
	BK_I32	(*free_msi)(struct bk_interrupt_controller *ctrl,
			   BK_UINT32 count,
			   BK_UINT32 *int_numbers);
	
	/* Dados privados do controller */
	void				*private;
};

typedef struct bk_interrupt_controller BK_INTERRUPT_CONTROLLER;

/*
 * ============================================================================
 * CONTEXTO DE INTERRUPÇÃO
 * ============================================================================
 */

struct bk_interrupt_context {
	/* Estado salvo */
	BK_UINT64			regs[16];	/* Registradores */
	BK_UINT64			rip;		/* Instruction pointer */
	BK_UINT64			rsp;		/* Stack pointer */
	BK_UINT64			rflags;		/* Flags */
	BK_UINT16			cs, ds, es, ss;	/* Segmentos */
	
	/* Informação da interrupção */
	BK_INTERRUPT_REGISTER		*interrupt;
	BK_UINT32			error_code;	/* Código de erro (se houver) */
	BK_UINT64			address;	/* Endereço do fault */
	
	/* Pilha de interrupção */
	void				*int_stack;	/* Pilha específica */
	BK_SIZE				int_stack_size;
	
	/* Aninhamento */
	BK_UINT32			nesting_level;	/* Nível de aninhamento */
	BK_IRQL				previous_irql;	/* IRQL anterior */
	
	/* CPU atual */
	BK_UINT32			cpu_id;
	
	/* Timestamp */
	BK_TIME				entry_time;
	BK_TIME				exit_time;
};

typedef struct bk_interrupt_context BK_INTERRUPT_CONTEXT;

/*
 * ============================================================================
 * DESCRITOR DE INTERRUPÇÃO (IDT/IVT)
 * ============================================================================
 */

typedef enum {
	BK_GATE_INTERRUPT	= 0x0E,		/* Interrupt gate */
	BK_GATE_TRAP		= 0x0F,		/* Trap gate */
	BK_GATE_TASK		= 0x05,		/* Task gate */
	BK_GATE_CALL		= 0x0C,		/* Call gate */
} BK_GATE_TYPE;

struct bk_interrupt_descriptor {
	BK_UINT16		offset_low;	/* Offset bits 0-15 */
	BK_UINT16		selector;	/* Segment selector */
	BK_UINT8		ist;		/* Interrupt Stack Table offset */
	BK_UINT8		type_attr;	/* Type and attributes */
	BK_UINT16		offset_mid;	/* Offset bits 16-31 */
	BK_UINT32		offset_high;	/* Offset bits 32-63 */
	BK_UINT32		reserved;	/* Reserved */
} __attribute__((packed));

typedef struct bk_interrupt_descriptor BK_INTERRUPT_DESCRIPTOR;

/*
 * ============================================================================
 * HANDLER DE INTERRUPÇÃO (também é registro!)
 * ============================================================================
 */

struct bk_interrupt_handler {
	BK_REGISTER_HEADER		header;
	
	/* O que este handler trata */
	BK_INTERRUPT_TYPE		handled_type;
	BK_UINT32			handled_number;	/* Número específico (ou -1) */
	BK_UINT32			handled_cpu;	/* CPU específica (ou -1) */
	
	/* Prioridade */
	BK_UINT8			priority;	/* Prioridade do handler */
	BK_IRQL				min_irql;	/* IRQL mínimo necessário */
	
	/* Callback */
	BK_I32	(*handler)(BK_INTERRUPT_REGISTER *interrupt,
			   BK_INTERRUPT_CONTEXT *context,
			   struct bk_interrupt_handler *self,
			   void *arg);
	void				*arg;
	
	/* Fast path (executado com interrupções desligadas) */
	BK_BOOLEAN			fast_path;
	BK_I32	(*fast_handler)(BK_INTERRUPT_REGISTER *interrupt,
				BK_INTERRUPT_CONTEXT *context);
	
	/* Chain */
	struct bk_interrupt_handler		*next;		/* Próximo na cadeia */
	BK_BOOLEAN			chain_end;	/* Se for o último, chama default */
	
	/* Estatísticas */
	BK_UINT64			calls;
	BK_UINT64			execution_time;	/* Tempo total de execução */
	BK_UINT64			max_execution_time;
};

typedef struct bk_interrupt_handler BK_INTERRUPT_HANDLER;

/*
 * ============================================================================
 * DPC (Deferred Procedure Call)
 * ============================================================================
 */

struct bk_dpc {
	BK_REGISTER_HEADER		header;
	
	/* Prioridade */
	BK_UINT8			priority;
	
	/* Callback */
	void	(*handler)(struct bk_dpc *dpc, void *arg);
	void				*arg;
	
	/* Estado */
	BK_BOOLEAN			queued;
	BK_BOOLEAN			running;
	
	/* CPU alvo */
	BK_UINT32			target_cpu;
	
	/* Lista */
	BK_LIST_ENTRY(bk_dpc)		dpc_list;
};

typedef struct bk_dpc BK_DPC;

/*
 * ============================================================================
 * APC (Asynchronous Procedure Call)
 * ============================================================================
 */

struct bk_apc {
	BK_REGISTER_HEADER		header;
	
	/* Tipo */
	BK_BOOLEAN			kernel_mode;	/* APC em modo kernel */
	BK_BOOLEAN			user_mode;	/* APC em modo usuário */
	
	/* Thread alvo */
	BK_REGISTER_HANDLE		target_thread;
	
	/* Callbacks */
	void	(*kernel_handler)(struct bk_apc *apc, void *arg);
	void	(*user_handler)(struct bk_apc *apc, void *arg);
	void				*arg;
	
	/* Estado */
	BK_BOOLEAN			pending;
	BK_BOOLEAN			inserted;
	
	/* Lista */
	BK_LIST_ENTRY(bk_apc)		apc_list;
};

typedef struct bk_apc BK_APC;

/*
 * ============================================================================
 * FUNÇÕES DO SISTEMA DE INTERRUPÇÕES
 * ============================================================================
 */

/* Inicialização */
BK_I32 bk_interrupt_system_init(void);
void bk_interrupt_system_shutdown(void);
BK_I32 bk_interrupt_cpu_init(BK_UINT32 cpu_id);

/* Registro de interrupções */
BK_INTERRUPT_REGISTER *bk_interrupt_register(BK_INTERRUPT_TYPE type,
					     BK_UINT32 int_number,
					     BK_IRQL irql,
					     BK_UINT32 flags);
BK_I32 bk_interrupt_unregister(BK_INTERRUPT_REGISTER *interrupt);

/* Handlers */
BK_I32 bk_interrupt_attach_handler(BK_INTERRUPT_REGISTER *interrupt,
				   BK_INTERRUPT_HANDLER *handler);
BK_I32 bk_interrupt_detach_handler(BK_INTERRUPT_REGISTER *interrupt,
				   BK_INTERRUPT_HANDLER *handler);
BK_I32 bk_interrupt_set_default_handler(BK_INTERRUPT_REGISTER *interrupt,
					BK_INTERRUPT_HANDLER *handler);

/* Controle de interrupções */
BK_I32 bk_interrupt_enable(BK_INTERRUPT_REGISTER *interrupt);
BK_I32 bk_interrupt_disable(BK_INTERRUPT_REGISTER *interrupt);
BK_I32 bk_interrupt_mask(BK_INTERRUPT_REGISTER *interrupt);
BK_I32 bk_interrupt_unmask(BK_INTERRUPT_REGISTER *interrupt);
BK_I32 bk_interrupt_eoi(BK_INTERRUPT_REGISTER *interrupt);
BK_I32 bk_interrupt_set_affinity(BK_INTERRUPT_REGISTER *interrupt,
				 BK_UINT32 cpu_mask);

/* Controle de IRQL */
BK_IRQL bk_interrupt_raise_irql(BK_IRQL new_irql);
BK_IRQL bk_interrupt_lower_irql(BK_IRQL new_irql);
BK_IRQL bk_interrupt_get_irql(void);
BK_BOOLEAN bk_interrupt_irql_is_at_least(BK_IRQL irql);

/* Interrupções de software */
BK_I32 bk_interrupt_softirq_raise(BK_UINT32 softirq_number);
BK_I32 bk_interrupt_softirq_schedule(BK_UINT32 softirq_number, BK_UINT32 cpu);

/* IPI */
BK_I32 bk_interrupt_ipi_send(BK_UINT32 target_cpu, BK_INTERRUPT_TYPE ipi_type);
BK_I32 bk_interrupt_ipi_send_all(BK_INTERRUPT_TYPE ipi_type);
BK_I32 bk_interrupt_ipi_send_except(BK_INTERRUPT_TYPE ipi_type,
				    BK_UINT32 except_cpu);

/* DPC */
BK_DPC *bk_dpc_create(void (*handler)(BK_DPC *, void *), void *arg,
		      BK_UINT8 priority);
BK_I32 bk_dpc_destroy(BK_DPC *dpc);
BK_I32 bk_dpc_queue(BK_DPC *dpc, BK_UINT32 target_cpu);
BK_I32 bk_dpc_cancel(BK_DPC *dpc);
BK_BOOLEAN bk_dpc_is_queued(BK_DPC *dpc);

/* APC */
BK_APC *bk_apc_create_kernel(void (*handler)(BK_APC *, void *), void *arg,
			     BK_REGISTER_HANDLE target_thread);
BK_APC *bk_apc_create_user(void (*handler)(BK_APC *, void *), void *arg,
			   BK_REGISTER_HANDLE target_thread);
BK_I32 bk_apc_destroy(BK_APC *apc);
BK_I32 bk_apc_queue(BK_APC *apc);
BK_I32 bk_apc_cancel(BK_APC *apc);

/* Controllers */
BK_I32 bk_interrupt_controller_register(BK_INTERRUPT_CONTROLLER *ctrl);
BK_I32 bk_interrupt_controller_unregister(BK_INTERRUPT_CONTROLLER *ctrl);
BK_INTERRUPT_CONTROLLER *bk_interrupt_get_controller(BK_UINT32 int_number);

/* Contexto */
BK_INTERRUPT_CONTEXT *bk_interrupt_get_current_context(void);
BK_BOOLEAN bk_interrupt_in_interrupt(void);
BK_UINT32 bk_interrupt_get_nesting_level(void);

/* Estatísticas */
struct bk_interrupt_stats {
	BK_UINT64	total_interrupts;
	BK_UINT64	total_handled;
	BK_UINT64	total_spurious;
	BK_UINT64	total_late;
	BK_UINT64	by_type[256];
	BK_UINT64	by_cpu[32];
	BK_UINT64	execution_time_total;
	BK_UINT64	execution_time_max;
	BK_UINT64	interrupt_rate;		/* Interrupções por segundo */
};

BK_I32 bk_interrupt_get_stats(struct bk_interrupt_stats *stats);
BK_I32 bk_interrupt_get_cpu_stats(BK_UINT32 cpu_id,
				  struct bk_interrupt_stats *stats);
void bk_interrupt_reset_stats(void);

/* Habilitação/desabilitação global */
static __BK_ALWAYS_INLINE void
bk_interrupt_global_disable(void)
{
	__asm__ volatile("cli" ::: "memory");
}

static __BK_ALWAYS_INLINE void
bk_interrupt_global_enable(void)
{
	__asm__ volatile("sti" ::: "memory");
}

static __BK_ALWAYS_INLINE BK_BOOLEAN
bk_interrupt_global_state(void)
{
	BK_UINT64 flags;
	__asm__ volatile("pushfq; popq %0" : "=r"(flags));
	return (flags & (1 << 9)) != 0;	/* IF bit */
}

/* Debug */
#ifdef _BK_INTERRUPT_DEBUG
void bk_interrupt_dump(BK_INTERRUPT_REGISTER *interrupt);
void bk_interrupt_dump_all(void);
void bk_interrupt_dump_handlers(BK_INTERRUPT_REGISTER *interrupt);
void bk_interrupt_dump_idt(void);
void bk_interrupt_dump_stats(void);
void bk_interrupt_dump_pending(void);
#endif

/*
 * ============================================================================
 * MACROS DE CONVENIÊNCIA
 * ============================================================================
 */

#define BK_IN_INTERRUPT()	bk_interrupt_in_interrupt()
#define BK_GET_IRQL()		bk_interrupt_get_irql()
#define BK_IRQL_AT_LEAST(irql)	bk_interrupt_irql_is_at_least(irql)

#define BK_RAISE_IRQL(irql) \
	do { \
		BK_IRQL __old_irql = bk_interrupt_raise_irql(irql); \
		/* Uso: BK_RAISE_IRQL(BK_IRQL_DEVICE_HIGH) { código } */ \
		do

#define BK_LOWER_IRQL(irql) \
		while (0); \
		bk_interrupt_lower_irql(__old_irql); \
	} while (0)

#define BK_SPIN_LOCK_IRQL(lock, irql) \
	do { \
		BK_IRQL __old_irql = bk_interrupt_raise_irql(irql); \
		bk_spinlock_acquire(lock)

#define BK_SPIN_UNLOCK_IRQL(lock) \
		bk_spinlock_release(lock); \
		bk_interrupt_lower_irql(__old_irql); \
	} while (0)

#define BK_DISABLE_INTERRUPTS() \
	do { \
		BK_BOOLEAN __was_enabled = bk_interrupt_global_state(); \
		bk_interrupt_global_disable()

#define BK_RESTORE_INTERRUPTS() \
		if (__was_enabled) \
			bk_interrupt_global_enable(); \
	} while (0)

/*
 * ============================================================================
 * HANDLERS PRÉ-DEFINIDOS
 * ============================================================================
 */

/* Handler que ignora a interrupção */
extern BK_INTERRUPT_HANDLER bk_interrupt_ignore_handler;

/* Handler que conta a interrupção */
extern BK_INTERRUPT_HANDLER bk_interrupt_count_handler;

/* Handler que causa panic (para interrupções inesperadas) */
extern BK_INTERRUPT_HANDLER bk_interrupt_panic_handler;

/* Handler para spurious interrupts */
extern BK_INTERRUPT_HANDLER bk_interrupt_spurious_handler;

/* Handler padrão para syscalls */
extern BK_INTERRUPT_HANDLER bk_interrupt_syscall_handler;

/* Handler para timer */
extern BK_INTERRUPT_HANDLER bk_interrupt_timer_handler;


#define BK_INTERRUPT_ANY_CPU		((BK_UINT32)-1)
#define BK_INTERRUPT_ANY_NUMBER		((BK_UINT32)-1)
#define BK_INTERRUPT_NO_VECTOR		0

#define BK_SPURIOUS_INTERRUPT		0xFF	/* Vetor de spurious do APIC */

/* Limites */
#define BK_MAX_INTERRUPTS		256	/* IDT size */
#define BK_MAX_SOFTIRQS			32
#define BK_MAX_DPC_PRIORITIES		16

/* Timeouts */
#define BK_INTERRUPT_NO_TIMEOUT		((BK_TIME)-1)
#define BK_INTERRUPT_IMMEDIATE_TIMEOUT	0

#endif /* !_BIBLE_INTERRUPT_H_ */
