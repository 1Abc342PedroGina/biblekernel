#ifndef _BIBLE_EXCEPTION_H_
#define _BIBLE_EXCEPTION_H_

/*
 * exception.h - Sistema de Exceções e Tratamento de Erros do BibleKernel
 * 
 * "Até exceções são registros" - Extensão da filosofia central
 * 
 * Exceções são registros especiais que representam:
 * - Condições de erro do sistema
 * - Falhas de hardware (page faults, divisão por zero)
 * - Violações de segurança (acesso negado, capability insuficiente)
 * - Timeouts e condições assíncronas
 * - Exceções de usuário (throw/catch em linguagens)
 */

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/queue.h>
#include <bible/regsister.h>
#include <bible/task.h>

/*
 * ============================================================================
 * TIPOS DE EXCEÇÃO
 * ============================================================================
 */

/* Hierarquia de exceções (usando 16 bits como outros registros) */
typedef enum __BK_ENUM_EXCEPTIONS_UINT16 {
	/* Exceções de hardware (0x1000-0x10FF) */
	BK_EXCEPTION_CLASS_HW_BASE		= 0x1000,
	
	BK_EXCEPTION_DIVIDE_ERROR		= 0x1001,	/* Divisão por zero */
	BK_EXCEPTION_DEBUG			= 0x1002,	/* Trap de debug */
	BK_EXCEPTION_BREAKPOINT			= 0x1003,	/* Breakpoint */
	BK_EXCEPTION_OVERFLOW			= 0x1004,	/* Overflow */
	BK_EXCEPTION_BOUND_RANGE			= 0x1005,	/* Violação de bound */
	BK_EXCEPTION_UNDEFINED_OPCODE		= 0x1006,	/* Instrução inválida */
	BK_EXCEPTION_DEVICE_NOT_AVAILABLE	= 0x1007,	/* FPU/device não disponível */
	BK_EXCEPTION_DOUBLE_FAULT			= 0x1008,	/* Double fault */
	BK_EXCEPTION_INVALID_TSS			= 0x1009,	/* TSS inválido */
	BK_EXCEPTION_SEGMENT_NOT_PRESENT	= 0x100A,	/* Segmento não presente */
	BK_EXCEPTION_STACK_SEGMENT_FAULT	= 0x100B,	/* Falha de stack */
	BK_EXCEPTION_GENERAL_PROTECTION		= 0x100C,	/* GPF */
	BK_EXCEPTION_PAGE_FAULT			= 0x100D,	/* Page fault */
	BK_EXCEPTION_FPU_ERROR			= 0x100E,	/* Erro de FPU */
	BK_EXCEPTION_ALIGNMENT_CHECK		= 0x100F,	/* Erro de alinhamento */
	BK_EXCEPTION_MACHINE_CHECK		= 0x1010,	/* Machine check */
	BK_EXCEPTION_SIMD_ERROR			= 0x1011,	/* Erro SIMD/AVX */
	BK_EXCEPTION_VIRTUALIZATION		= 0x1012,	/* Exceção de virtualização */
	BK_EXCEPTION_HYPERVISOR			= 0x1013,	/* Exceção de hypervisor */
	
	/* Exceções de memória (0x1100-0x11FF) */
	BK_EXCEPTION_CLASS_MEMORY_BASE		= 0x1100,
	
	BK_EXCEPTION_OUT_OF_MEMORY		= 0x1101,	/* Sem memória */
	BK_EXCEPTION_INVALID_ADDRESS		= 0x1102,	/* Endereço inválido */
	BK_EXCEPTION_ACCESS_VIOLATION		= 0x1103,	/* Violação de acesso */
	BK_EXCEPTION_WRITE_PROTECTED		= 0x1104,	/* Escrita em página protegida */
	BK_EXCEPTION_EXECUTE_PROTECTED		= 0x1105,	/* Execução em página NX */
	BK_EXCEPTION_COW_FAULT			= 0x1106,	/* Copy-on-write fault */
	BK_EXCEPTION_SWAP_FAULT			= 0x1107,	/* Falha de swap */
	BK_EXCEPTION_STACK_OVERFLOW		= 0x1108,	/* Estouro de pilha */
	BK_EXCEPTION_STACK_UNDERFLOW		= 0x1109,	/* Underflow de pilha */
	BK_EXCEPTION_HEAP_CORRUPTION		= 0x110A,	/* Heap corrompido */
	BK_EXCEPTION_MEMORY_LIMIT_EXCEEDED	= 0x110B,	/* Limite de memória excedido */
	
	/* Exceções de processo/task/thread (0x1200-0x12FF) */
	BK_EXCEPTION_CLASS_EXECUTION_BASE	= 0x1200,
	
	BK_EXCEPTION_INVALID_PROCESS		= 0x1201,	/* Processo inválido */
	BK_EXCEPTION_INVALID_TASK		= 0x1202,	/* Task inválida */
	BK_EXCEPTION_INVALID_THREAD		= 0x1203,	/* Thread inválida */
	BK_EXCEPTION_INVALID_ACTIVITY		= 0x1204,	/* Atividade inválida */
	BK_EXCEPTION_THREAD_TERMINATED		= 0x1205,	/* Thread terminada */
	BK_EXCEPTION_PROCESS_TERMINATED		= 0x1206,	/* Processo terminado */
	BK_EXCEPTION_SCHEDULER_ERROR		= 0x1207,	/* Erro do escalonador */
	BK_EXCEPTION_DEADLOCK			= 0x1208,	/* Deadlock detectado */
	BK_EXCEPTION_RACE_CONDITION		= 0x1209,	/* Condição de corrida */
	
	/* Exceções de IPC e comunicação (0x1300-0x13FF) */
	BK_EXCEPTION_CLASS_IPC_BASE		= 0x1300,
	
	BK_EXCEPTION_IPC_ERROR			= 0x1301,	/* Erro genérico de IPC */
	BK_EXCEPTION_IPC_TIMEOUT		= 0x1302,	/* Timeout de IPC */
	BK_EXCEPTION_IPC_NO_RECEIVER		= 0x1303,	/* Sem receptor */
	BK_EXCEPTION_IPC_NO_SENDER		= 0x1304,	/* Sem remetente */
	BK_EXCEPTION_IPC_QUEUE_FULL		= 0x1305,	/* Fila cheia */
	BK_EXCEPTION_IPC_QUEUE_EMPTY		= 0x1306,	/* Fila vazia */
	BK_EXCEPTION_PIPE_BROKEN		= 0x1307,	/* Pipe quebrado */
	BK_EXCEPTION_CONNECTION_RESET		= 0x1308,	/* Conexão resetada */
	BK_EXCEPTION_CONNECTION_REFUSED		= 0x1309,	/* Conexão recusada */
	BK_EXCEPTION_CONNECTION_TIMEOUT		= 0x130A,	/* Timeout de conexão */
	BK_EXCEPTION_SHUTDOWN			= 0x130B,	/* Shutdown */
	
	/* Exceções de sistema de arquivos (0x1400-0x14FF) */
	BK_EXCEPTION_CLASS_FILESYSTEM_BASE	= 0x1400,
	
	BK_EXCEPTION_FILE_NOT_FOUND		= 0x1401,	/* Arquivo não encontrado */
	BK_EXCEPTION_PATH_NOT_FOUND		= 0x1402,	/* Caminho não encontrado */
	BK_EXCEPTION_IS_DIRECTORY		= 0x1403,	/* É um diretório */
	BK_EXCEPTION_NOT_DIRECTORY		= 0x1404,	/* Não é um diretório */
	BK_EXCEPTION_DIRECTORY_NOT_EMPTY	= 0x1405,	/* Diretório não vazio */
	BK_EXCEPTION_FILE_EXISTS		= 0x1406,	/* Arquivo já existe */
	BK_EXCEPTION_NO_SPACE			= 0x1407,	/* Sem espaço em disco */
	BK_EXCEPTION_READ_ONLY			= 0x1408,	/* Sistema de arquivos read-only */
	BK_EXCEPTION_IO_ERROR			= 0x1409,	/* Erro de I/O */
	BK_EXCEPTION_BAD_FD			= 0x140A,	/* File descriptor inválido */
	BK_EXCEPTION_TOO_MANY_OPEN_FILES	= 0x140B,	/* Muitos arquivos abertos */
	BK_EXCEPTION_IS_TEMPORARY		= 0x140C,	/* É arquivo temporário */
	BK_EXCEPTION_TEMPORARY_EXPIRED		= 0x140D,	/* Arquivo temporário expirou */
	
	/* Exceções de segurança (0x1500-0x15FF) */
	BK_EXCEPTION_CLASS_SECURITY_BASE	= 0x1500,
	
	BK_EXCEPTION_ACCESS_DENIED		= 0x1501,	/* Acesso negado */
	BK_EXCEPTION_PERMISSION_DENIED		= 0x1502,	/* Permissão negada */
	BK_EXCEPTION_CAPABILITY_MISSING		= 0x1503,	/* Capability ausente */
	BK_EXCEPTION_CAPABILITY_EXPIRED		= 0x1504,	/* Capability expirada */
	BK_EXCEPTION_CAPABILITY_REVOKED		= 0x1505,	/* Capability revogada */
	BK_EXCEPTION_INSUFFICIENT_PRIVILEGE	= 0x1506,	/* Privilégio insuficiente */
	BK_EXCEPTION_SECURITY_VIOLATION		= 0x1507,	/* Violação de segurança */
	BK_EXCEPTION_INTEGRITY_CHECK_FAILED	= 0x1508,	/* Falha de integridade */
	BK_EXCEPTION_AUTHENTICATION_FAILED	= 0x1509,	/* Autenticação falhou */
	BK_EXCEPTION_UNAUTHORIZED		= 0x150A,	/* Não autorizado */
	BK_EXCEPTION_SANDBOX_VIOLATION		= 0x150B,	/* Violação de sandbox */
	
	/* Exceções de registro (0x1600-0x16FF) */
	BK_EXCEPTION_CLASS_REGISTER_BASE	= 0x1600,
	
	BK_EXCEPTION_REGISTER_NOT_FOUND		= 0x1601,	/* Registro não encontrado */
	BK_EXCEPTION_REGISTER_INVALID_HANDLE	= 0x1602,	/* Handle inválido */
	BK_EXCEPTION_REGISTER_WRONG_TYPE	= 0x1603,	/* Tipo de registro errado */
	BK_EXCEPTION_REGISTER_CORRUPTED		= 0x1604,	/* Registro corrompido */
	BK_EXCEPTION_REGISTER_LOCKED		= 0x1605,	/* Registro bloqueado */
	BK_EXCEPTION_REGISTER_REFCOUNT_ZERO	= 0x1606,	/* Refcount já zero */
	BK_EXCEPTION_REGISTER_MAX_REACHED	= 0x1607,	/* Máximo de registros */
	BK_EXCEPTION_REGISTER_TYPE_FULL		= 0x1608,	/* Tipo de registro cheio */
	
	/* Exceções de sistema (0x1700-0x17FF) */
	BK_EXCEPTION_CLASS_SYSTEM_BASE		= 0x1700,
	
	BK_EXCEPTION_NOT_IMPLEMENTED		= 0x1701,	/* Não implementado */
	BK_EXCEPTION_NOT_SUPPORTED		= 0x1702,	/* Não suportado */
	BK_EXCEPTION_OPERATION_CANCELLED	= 0x1703,	/* Operação cancelada */
	BK_EXCEPTION_OPERATION_IN_PROGRESS	= 0x1704,	/* Operação em andamento */
	BK_EXCEPTION_ALREADY_EXISTS		= 0x1705,	/* Já existe */
	BK_EXCEPTION_BUSY			= 0x1706,	/* Recurso ocupado */
	BK_EXCEPTION_TIMEOUT			= 0x1707,	/* Timeout */
	BK_EXCEPTION_WOULD_BLOCK		= 0x1708,	/* Operação bloquearia */
	BK_EXCEPTION_INTERRUPTED		= 0x1709,	/* Interrompido */
	BK_EXCEPTION_AGAIN			= 0x170A,	/* Tente novamente */
	BK_EXCEPTION_NO_SYS			= 0x170B,	/* Função não implementada */
	BK_EXCEPTION_INVALID_ARGUMENT		= 0x170C,	/* Argumento inválido */
	BK_EXCEPTION_RANGE			= 0x170D,	/* Valor fora de faixa */
	BK_EXCEPTION_KOVERFLOW			= 0x170E,	/* Overflow */
	BK_EXCEPTION_UNDERFLOW			= 0x170F,	/* Underflow */
	
	/* Exceções de usuário (0xF000-0xFFFF) */
	BK_EXCEPTION_CLASS_USER_BASE		= 0xF000,
	
	BK_EXCEPTION_USER			= 0xF001,	/* Exceção de usuário genérica */
	BK_EXCEPTION_USER_DEFINED_1		= 0xF100,
	BK_EXCEPTION_USER_DEFINED_2		= 0xF200,
	BK_EXCEPTION_USER_DEFINED_3		= 0xF300,
	BK_EXCEPTION_USER_DEFINED_4		= 0xF400,
	BK_EXCEPTION_USER_DEFINED_5		= 0xF500,
} BK_EXCEPTION_CLASS;

/*
 * ============================================================================
 * GRAVIDADE DA EXCEÇÃO
 * ============================================================================
 */

typedef enum {
	BK_EXCEPTION_SEVERITY_DEBUG		= 0,	/* Apenas debug, ignorável */
	BK_EXCEPTION_SEVERITY_INFO		= 1,	/* Informativo */
	BK_EXCEPTION_SEVERITY_WARNING		= 2,	/* Aviso */
	BK_EXCEPTION_SEVERITY_ERROR		= 3,	/* Erro recuperável */
	BK_EXCEPTION_SEVERITY_FATAL		= 4,	/* Erro fatal (processo termina) */
	BK_EXCEPTION_SEVERITY_PANIC		= 5,	/* Pânico do kernel */
	BK_EXCEPTION_SEVERITY_CATASTROPHIC	= 6,	/* Catastrófico (reinicialização) */
} BK_EXCEPTION_SEVERITY;

/*
 * ============================================================================
 * AÇÕES DE TRATAMENTO
 * ============================================================================
 */

typedef enum {
	BK_EXCEPTION_ACTION_IGNORE		= 0,	/* Ignorar exceção */
	BK_EXCEPTION_ACTION_RETRY		= 1,	/* Tentar novamente */
	BK_EXCEPTION_ACTION_TERMINATE		= 2,	/* Terminar processo/thread */
	BK_EXCEPTION_ACTION_COREDUMP		= 3,	/* Gerar core dump */
	BK_EXCEPTION_ACTION_SIGNAL		= 4,	/* Enviar sinal */
	BK_EXCEPTION_ACTION_HANDLER		= 5,	/* Chamar handler registrado */
	BK_EXCEPTION_ACTION_CASCADE		= 6,	/* Propagar para entidade superior */
	BK_EXCEPTION_ACTION_RESUME		= 7,	/* Retomar execução */
} BK_EXCEPTION_ACTION;

/*
 * ============================================================================
 * REGISTRO DE EXCEÇÃO
 * ============================================================================
 */

/* Exceção como registro (tudo é registro!) */
struct bk_exception_register {
	BK_REGISTER_HEADER		header;
	
	/* Classificação */
	BK_EXCEPTION_CLASS		exception_class;
	BK_EXCEPTION_SEVERITY		severity;
	BK_UINT32			code;		/* Código específico */
	
	/* Contexto da exceção */
	BK_UINT64			address;	/* Endereço onde ocorreu */
	BK_UINT64			instruction;	/* Instrução que causou */
	BK_UINT64			fault_info;	/* Informação adicional */
	
	/* Estado do sistema no momento */
	BK_REGISTER_HANDLE		process;	/* Processo afetado */
	BK_REGISTER_HANDLE		task;		/* Task afetada */
	BK_REGISTER_HANDLE		thread;		/* Thread afetada */
	BK_REGISTER_HANDLE		activity;	/* Atividade afetada */
	
	/* Dados da exceção */
	BK_REGISTER_HANDLE		data;		/* Dados adicionais */
	char				message[256];	/* Mensagem descritiva */
	
	/* Handler */
	BK_REGISTER_HANDLE		handler;	/* Handler registrado */
	void				(*handler_func)(struct bk_exception_register *);
	void				*handler_arg;
	
	/* Resolução */
	BK_EXCEPTION_ACTION		taken_action;	/* Ação tomada */
	BK_I32				result;		/* Resultado do tratamento */
	
	/* Estatísticas */
	BK_UINT32			occurrence_count;	/* Número de ocorrências */
	BK_TIME				first_occurrence;	/* Primeira ocorrência */
	BK_TIME				last_occurrence;	/* Última ocorrência */
};

typedef struct bk_exception_register BK_EXCEPTION_REGISTER;

/*
 * ============================================================================
 * FRAME DE EXCEÇÃO (para propagação)
 * ============================================================================
 */

struct bk_exception_frame {
	BK_EXCEPTION_REGISTER		*exception;
	
	/* Encadeamento */
	struct bk_exception_frame	*previous;	/* Frame anterior */
	struct bk_exception_frame	*next;		/* Próximo frame */
	
	/* Localização */
	BK_REGISTER_HANDLE		process;
	BK_REGISTER_HANDLE		task;
	BK_REGISTER_HANDLE		thread;
	BK_REGISTER_HANDLE		activity;
	
	/* Ponto de retorno */
	BK_UINT64			return_address;
	BK_UINT64			stack_pointer;
};

typedef struct bk_exception_frame BK_EXCEPTION_FRAME;

/*
 * ============================================================================
 * HANDLER DE EXCEÇÃO (também é registro!)
 * ============================================================================
 */

struct bk_exception_handler {
	BK_REGISTER_HEADER		header;
	
	/* O que este handler trata */
	BK_EXCEPTION_CLASS		handled_class;
	BK_EXCEPTION_SEVERITY		max_severity;
	BK_UINT32			handled_codes[8];	/* Códigos específicos */
	
	/* Ação padrão */
	BK_EXCEPTION_ACTION		default_action;
	
	/* Callback */
	BK_EXCEPTION_ACTION		(*handler)(
						struct bk_exception_register *exc,
						struct bk_exception_handler *self,
						void *arg);
	void				*arg;
	
	/* Escopo */
	BK_REGISTER_HANDLE		scope_process;
	BK_REGISTER_HANDLE		scope_thread;
	BK_BOOLEAN			global;		/* Handler global? */
	
	/* Estatísticas */
	BK_UINT32			calls;
	BK_UINT32			handled;
	BK_UINT32			unhandled;
};

typedef struct bk_exception_handler BK_EXCEPTION_HANDLER;

/*
 * ============================================================================
 * FUNÇÕES DO SISTEMA DE EXCEÇÕES
 * ============================================================================
 */

/* Inicialização */
BK_I32 bk_exception_system_init(void);
void bk_exception_system_shutdown(void);

/* Lançamento de exceções */
BK_I32 bk_exception_throw(BK_EXCEPTION_CLASS class,
			  BK_EXCEPTION_SEVERITY severity,
			  BK_UINT32 code,
			  const char *message,
			  BK_REGISTER_HANDLE process,
			  BK_REGISTER_HANDLE thread);

BK_I32 bk_exception_throw_with_data(BK_EXCEPTION_CLASS class,
				    BK_EXCEPTION_SEVERITY severity,
				    BK_UINT32 code,
				    const char *message,
				    BK_REGISTER_HANDLE data,
				    BK_REGISTER_HANDLE process,
				    BK_REGISTER_HANDLE thread);

BK_I32 bk_exception_throw_address(BK_EXCEPTION_CLASS class,
				  BK_EXCEPTION_SEVERITY severity,
				  BK_UINT64 address,
				  BK_UINT64 instruction,
				  BK_UINT64 fault_info,
				  BK_REGISTER_HANDLE process,
				  BK_REGISTER_HANDLE thread);

/* Tratamento */
BK_EXCEPTION_ACTION bk_exception_handle(BK_EXCEPTION_REGISTER *exception);
BK_I32 bk_exception_register_handler(BK_EXCEPTION_HANDLER *handler);
BK_I32 bk_exception_unregister_handler(BK_EXCEPTION_HANDLER *handler);

/* Frames de exceção */
BK_EXCEPTION_FRAME *bk_exception_push_frame(BK_EXCEPTION_REGISTER *exception);
BK_EXCEPTION_FRAME *bk_exception_pop_frame(void);
BK_EXCEPTION_FRAME *bk_exception_current_frame(void);

/* Handlers padrão */
void bk_exception_default_handler(BK_EXCEPTION_REGISTER *exception);
void bk_exception_fatal_handler(BK_EXCEPTION_REGISTER *exception);
void bk_exception_debug_handler(BK_EXCEPTION_REGISTER *exception);

/* Verificações */
BK_BOOLEAN bk_exception_check_capability(BK_REGISTER_HANDLE handle,
					 BK_CAPABILITY_TYPE needed);
BK_BOOLEAN bk_exception_check_memory(BK_VM_OFFSET addr, BK_SIZE size,
				     BK_UINT32 prot);
BK_BOOLEAN bk_exception_check_pointer(const void *ptr);

/* Conversões */
const char *bk_exception_class_name(BK_EXCEPTION_CLASS class);
const char *bk_exception_severity_name(BK_EXCEPTION_SEVERITY severity);
const char *bk_exception_action_name(BK_EXCEPTION_ACTION action);
BK_I32 bk_exception_to_errno(BK_EXCEPTION_CLASS class);
BK_EXCEPTION_CLASS bk_exception_from_errno(BK_I32 errno);

/* Estatísticas */
struct bk_exception_stats {
	BK_UINT64	total_exceptions;
	BK_UINT64	total_handled;
	BK_UINT64	total_unhandled;
	BK_UINT64	total_fatal;
	BK_UINT64	by_class[256];		/* Contagem por classe */
	BK_UINT64	by_severity[8];		/* Contagem por severidade */
	BK_TIME		last_exception_time;
	BK_EXCEPTION_CLASS	last_exception_class;
};

BK_I32 bk_exception_get_stats(struct bk_exception_stats *stats);
void bk_exception_reset_stats(void);

/* Debug */
#ifdef _BK_EXCEPTION_DEBUG
void bk_exception_dump(BK_EXCEPTION_REGISTER *exception);
void bk_exception_dump_current(void);
void bk_exception_dump_stats(void);
void bk_exception_dump_handlers(void);
#endif

/*
 * ============================================================================
 * MACROS DE CONVENIÊNCIA
 * ============================================================================
 */

#define BK_TRY \
	BK_EXCEPTION_FRAME *__frame = bk_exception_push_frame(NULL); \
	if (__BK_LIKELY(__frame != NULL))

#define BK_CATCH(class) \
	if (__BK_UNLIKELY(__frame->exception && \
	     __frame->exception->exception_class == (class)))

#define BK_CATCH_ALL \
	if (__BK_UNLIKELY(__frame->exception))

#define BK_FINALLY \
	if (1)

#define BK_END_TRY \
	bk_exception_pop_frame();

#define BK_THROW(class, severity, code, msg) \
	bk_exception_throw((class), (severity), (code), (msg), \
			   BK_CURRENT_PROCESS(), BK_CURRENT_THREAD())

#define BK_THROW_DATA(class, severity, code, msg, data) \
	bk_exception_throw_with_data((class), (severity), (code), (msg), \
				     (data), BK_CURRENT_PROCESS(), \
				     BK_CURRENT_THREAD())

#define BK_RETHROW \
	if (__frame->exception) \
		bk_exception_throw(__frame->exception->exception_class, \
				   __frame->exception->severity, \
				   __frame->exception->code, \
				   __frame->exception->message, \
				   __frame->exception->process, \
				   __frame->exception->thread)

#define BK_ASSERT(cond) \
	do { \
		if (__BK_UNLIKELY(!(cond))) { \
			bk_exception_throw(BK_EXCEPTION_INVALID_ARGUMENT, \
					   BK_EXCEPTION_SEVERITY_ERROR, \
					   0, "Assertion failed: " #cond, \
					   BK_CURRENT_PROCESS(), \
					   BK_CURRENT_THREAD()); \
		} \
	} while (0)

#define BK_ASSERT_MSG(cond, msg) \
	do { \
		if (__BK_UNLIKELY(!(cond))) { \
			bk_exception_throw(BK_EXCEPTION_INVALID_ARGUMENT, \
					   BK_EXCEPTION_SEVERITY_ERROR, \
					   0, (msg), \
					   BK_CURRENT_PROCESS(), \
					   BK_CURRENT_THREAD()); \
		} \
	} while (0)

#define BK_CHECK(expr) \
	do { \
		BK_I32 __result = (expr); \
		if (__BK_UNLIKELY(__result < 0)) { \
			bk_exception_throw(bk_exception_from_errno(-__result), \
					   BK_EXCEPTION_SEVERITY_ERROR, \
					   -__result, NULL, \
					   BK_CURRENT_PROCESS(), \
					   BK_CURRENT_THREAD()); \
		} \
	} while (0)

#define BK_CHECK_NOT_NULL(ptr) \
	do { \
		if (__BK_UNLIKELY((ptr) == NULL)) { \
			bk_exception_throw(BK_EXCEPTION_INVALID_ADDRESS, \
					   BK_EXCEPTION_SEVERITY_ERROR, \
					   0, "NULL pointer dereference", \
					   BK_CURRENT_PROCESS(), \
					   BK_CURRENT_THREAD()); \
		} \
	} while (0)

#define BK_CHECK_CAP(handle, cap) \
	do { \
		if (__BK_UNLIKELY(!bk_exception_check_capability((handle), (cap)))) { \
			bk_exception_throw(BK_EXCEPTION_CAPABILITY_MISSING, \
					   BK_EXCEPTION_SEVERITY_ERROR, \
					   (cap), "Capability missing", \
					   BK_CURRENT_PROCESS(), \
					   BK_CURRENT_THREAD()); \
		} \
	} while (0)

/*
 * ============================================================================
 * HANDLERS PRÉ-DEFINIDOS
 * ============================================================================
 */

/* Handler que ignora a exceção */
extern BK_EXCEPTION_HANDLER bk_exception_ignore_handler;

/* Handler que loga a exceção e continua */
extern BK_EXCEPTION_HANDLER bk_exception_log_handler;

/* Handler que termina a thread */
extern BK_EXCEPTION_HANDLER bk_exception_terminate_handler;

/* Handler que gera core dump */
extern BK_EXCEPTION_HANDLER bk_exception_coredump_handler;

/* Handler que envia sinal */
extern BK_EXCEPTION_HANDLER bk_exception_signal_handler;

/*
 * ============================================================================
 * CONSTANTES ÚTEIS
 * ============================================================================
 */

/* Códigos especiais */
#define BK_EXCEPTION_CODE_UNSPECIFIED	0
#define BK_EXCEPTION_CODE_GENERIC	1
#define BK_EXCEPTION_CODE_USER_BASE	0x1000

/* Tempo para timeouts */
#define BK_EXCEPTION_NO_TIMEOUT		((BK_TIME)-1)
#define BK_EXCEPTION_IMMEDIATE_TIMEOUT	0

#endif /* !_BIBLE_EXCEPTION_H_ */
