#ifndef _BIBLE_REGSISTER_H_
#define _BIBLE_REGSISTER_H_

/*
 * regsister.h - Sistema de Registros do BibleKernel
 * 
 * "Tudo é Registro" - Filosofia central do BibleKernel
 * 
 * Um registro é a unidade fundamental de todo recurso no sistema:
 * - Processos são registros
 * - Tasks são registros
 * - Threads são registros
 * - Atividades são registros
 * - Arquivos (quando existem) são registros
 * - Capacidades são registros
 * - Mensagens de log são registros
 * - Dispositivos são registros
 * - Conexões de rede são registros
 * - E tudo mais no sistema
 */

#include <bible/cdefs.h>
#include <bible/ktypes.h>
#include <bible/types.h>
#include <bible/queue.h>

typedef __uintcap_kt		BK_REGISTER_HANDLE;

typedef enum {
	BK_SPIN_UNLOCKED	= 0x00000000,	/* Lock disponível */
	BK_SPIN_LOCKED		= 0x00000001,	/* Lock adquirido */
	BK_SPIN_CONTENDED	= 0x00000002,	/* Com contenção (vários esperando) */
	BK_SPIN_IRQ_SAFE	= 0x80000000,	/* Flag: salva IRQ state */
} BK_SPIN_STATE;

/* Estrutura principal do spinlock */
typedef struct {
	/* Estado atômico do lock */
	volatile BK_UINT32	s_lock;		/* Estado atual do lock */
	
	/* Identificação e debug */
	BK_UINT32		s_cpu;		/* CPU que detém o lock */
	BK_UINT32		s_depth;	/* Profundidade de aquisição (para recursive) */
	BK_REGISTER_HANDLE	s_owner;	/* Handle do dono (thread/atividade) */
	
	/* Estatísticas de contenção */
	BK_UINT64		s_acquires;	/* Total de aquisições */
	BK_UINT64		s_contentions;	/* Total de contenções */
	BK_UINT64		s_spins;	/* Total de spins */
	BK_UINT64		s_time_held;	/* Tempo total mantido */
	
	/* Informação do local de aquisição (debug) */
#ifdef _BK_SPINLOCK_DEBUG
	const char		*s_func;	/* Função que adquiriu */
	const char		*s_file;	/* Arquivo que adquiriu */
	int			s_line;		/* Linha que adquiriu */
#endif
	
	/* Padding para cache line alignment */
	char			_pad[32 - sizeof(BK_UINT32) * 8];
} BK_SPINLOCK;

typedef enum __BK_ENUM_U8 {
	BK_FALSE	= 0,	/* Falso (0) */
	BK_TRUE		= 1,	/* Verdadeiro (1) */
	
	/* Valores especiais para casos de erro */
	BK_BOOL_ERROR	= 0xFF	/* Erro na avaliação booleana */
} BK_BOOL;

#define BK_BOOL_FROM(expr)	((expr) ? BK_TRUE : BK_FALSE)

/* Operadores lógicos com BK_BOOL */
#define BK_BOOL_AND(a, b)	BK_BOOL_FROM((a) && (b))
#define BK_BOOL_OR(a, b)	BK_BOOL_FROM((a) || (b))
#define BK_BOOL_NOT(a)		BK_BOOL_FROM(!(a))
#define BK_BOOL_XOR(a, b)	BK_BOOL_FROM((a) != (b))

/* Verificação de validade */
#define BK_BOOL_IS_VALID(b)	(((b) == BK_TRUE) || ((b) == BK_FALSE))
#define BK_BOOL_IS_TRUE(b)	((b) == BK_TRUE)
#define BK_BOOL_IS_FALSE(b)	((b) == BK_FALSE)
#define BK_BOOL_IS_ERROR(b)	((b) == BK_BOOL_ERROR)

/* Para compatibilidade com código C padrão */
#define bool	BK_BOOL
#define true	BK_TRUE
#define false	BK_FALSE

/*
 * ============================================================================
 * TIPOS FUNDAMENTAIS DE REGISTROS
 * ============================================================================
 */

/* Handle universal para qualquer registro no sistema */
typedef BK_REGISTER_HANDLE	BK_REGISTER;	/* Alias para simplicidade */

/* Handle inválido */
#define BK_REGISTER_NULL	((BK_REGISTER_HANDLE)0)
#define BK_REGISTER_INVALID	((BK_REGISTER_HANDLE)-1)

/* Tipos de registro no sistema */
typedef enum __BK_ENUM_UINT16 {
	/* Tipos fundamentais (0x0000-0x00FF) */
	BK_REGISTER_TYPE_NULL		= 0x0000,
	BK_REGISTER_TYPE_CAPABILITY	= 0x0001,	/* Capacidade */
	BK_REGISTER_TYPE_CONTEXT	= 0x0002,	/* Contexto de execução */
	BK_REGISTER_TYPE_MEMORY_REGION	= 0x0003,	/* Região de memória */
	BK_REGISTER_TYPE_DEVICE		= 0x0004,	/* Dispositivo */
	
	/* Hierarquia de execução (0x0100-0x01FF) */
	BK_REGISTER_TYPE_PROCESS	= 0x0100,	/* Processo (container) */
	BK_REGISTER_TYPE_TASK		= 0x0101,	/* Task (região de recursos) */
	BK_REGISTER_TYPE_THREAD		= 0x0102,	/* Thread (unidade execução) */
	BK_REGISTER_TYPE_ACTIVITY	= 0x0103,	/* Atividade (função atômica) */
	
	/* Sistema de arquivos (0x0200-0x02FF) */
	BK_REGISTER_TYPE_FILE		= 0x0200,	/* Arquivo */
	BK_REGISTER_TYPE_DIRECTORY	= 0x0201,	/* Diretório */
	BK_REGISTER_TYPE_SYMLINK	= 0x0202,	/* Link simbólico */
	BK_REGISTER_TYPE_MOUNT		= 0x0203,	/* Ponto de montagem */
	BK_REGISTER_TYPE_FS		= 0x0204,	/* Sistema de arquivos */
	
	/* IPC e Comunicação (0x0300-0x03FF) */
	BK_REGISTER_TYPE_PIPE		= 0x0300,	/* Pipe */
	BK_REGISTER_TYPE_SOCKET		= 0x0301,	/* Socket */
	BK_REGISTER_TYPE_MQUEUE		= 0x0302,	/* Fila de mensagens */
	BK_REGISTER_TYPE_SHM		= 0x0303,	/* Memória compartilhada */
	BK_REGISTER_TYPE_SEMAPHORE	= 0x0304,	/* Semáforo */
	BK_REGISTER_TYPE_MUTEX		= 0x0305,	/* Mutex */
	BK_REGISTER_TYPE_CONDVAR	= 0x0306,	/* Variável de condição */
	BK_REGISTER_TYPE_RWLOCK		= 0x0307,	/* Read-Write lock */
	
	/* Sistema de log (0x0400-0x04FF) */
	BK_REGISTER_TYPE_LOG		= 0x0400,	/* Entrada de log */
	BK_REGISTER_TYPE_STATISTIC	= 0x0401,	/* Estatística */
	BK_REGISTER_TYPE_PROFILE	= 0x0402,	/* Perfil de execução */
	
	/* Rede (0x0500-0x05FF) */
	BK_REGISTER_TYPE_CONNECTION	= 0x0500,	/* Conexão de rede */
	BK_REGISTER_TYPE_ENDPOINT	= 0x0501,	/* Endpoint */
	BK_REGISTER_TYPE_INTERFACE	= 0x0502,	/* Interface de rede */
	BK_REGISTER_TYPE_ADDRESS	= 0x0503,	/* Endereço IP */
	
	/* Dispositivos (0x0600-0x06FF) */
	BK_REGISTER_TYPE_DRIVER		= 0x0600,	/* Driver */
	BK_REGISTER_TYPE_INTERRUPT	= 0x0601,	/* Linha de IRQ */
	BK_REGISTER_TYPE_DMA		= 0x0602,	/* Canal DMA */
	BK_REGISTER_TYPE_PORT		= 0x0603,	/* Porta de I/O */
	
	/* Usuários e grupos (0x0700-0x07FF) */
	BK_REGISTER_TYPE_USER		= 0x0700,	/* Usuário */
	BK_REGISTER_TYPE_GROUP		= 0x0701,	/* Grupo */
	BK_REGISTER_TYPE_SESSION	= 0x0702,	/* Sessão */
	
	/* Timers e tempo (0x0800-0x08FF) */
	BK_REGISTER_TYPE_TIMER		= 0x0800,	/* Timer */
	BK_REGISTER_TYPE_TIMEOUT	= 0x0801,	/* Timeout */
	BK_REGISTER_TYPE_CLOCK		= 0x0802,	/* Clock */
	
	/* Tipos definidos pelo usuário (0xF000-0xFFFF) */
	BK_REGISTER_TYPE_USER_BASE	= 0xF000,
} BK_REGISTER_TYPE;

/* Máscara para tipo de registro */
#define BK_REGISTER_TYPE_MASK		0xFFFF
#define BK_REGISTER_TYPE(handle)	\
	((BK_REGISTER_TYPE)((handle) >> 48) & BK_REGISTER_TYPE_MASK)

/*
 * ============================================================================
 * CABEÇALHO BASE DE REGISTRO
 * ============================================================================
 */

/* Todo registro começa com este cabeçalho */
struct bk_register_header {
	/* Identificação */
	BK_REGISTER_TYPE	r_type;		/* Tipo do registro */
	BK_ID			r_id;		/* ID único no tipo */
	
	/* Capacidades */
	BK_REGISTER_HANDLE	r_capabilities;	/* Handle para capacidades */
	
	/* Controle de acesso */
	BK_UID			r_owner;	/* Dono do registro */
	BK_GID			r_group;	/* Grupo do registro */
	BK_UINT16		r_permissions;	/* Permissões (rwx para owner/group/other) */
	
	/* Contagem de referências */
	BK_UINT32		r_refcount;	/* Número de handles ativos */
	
	/* Estado do registro */
	BK_UINT16		r_state;	/* Estado específico do tipo */
	BK_UINT16		r_flags;	/* Flags gerais */
	
	/* Tempos */
	BK_TIME			r_created;	/* Momento de criação */
	BK_TIME			r_accessed;	/* Último acesso */
	BK_TIME			r_modified;	/* Última modificação */
	
	/* Lock do registro */
	BK_SPINLOCK		r_lock;
	
	/* Listas para o sistema de registros */
	BK_TAILQ_ENTRY(bk_register_header) r_global_link;	/* Lista global */
	BK_TAILQ_ENTRY(bk_register_header) r_type_link;	/* Lista por tipo */
	BK_LIST_ENTRY(bk_register_header) r_owner_link;	/* Lista por dono */
};

typedef struct bk_register_header BK_REGISTER_HEADER;

/*
 * ============================================================================
 * MACROS PARA DECLARAÇÃO DE REGISTROS
 * ============================================================================
 */

/* Declara um novo tipo de registro */
#define BK_REGISTER_DECLARE(name, parent) \
	struct bk_register_##name { \
		BK_REGISTER_HEADER header; \
		parent parent_data; \
		/* Campos específicos abaixo */ \
		/* ... */

/* Obtém o cabeçalho a partir de um handle */
#define BK_REGISTER_FROM_HANDLE(handle, type) \
	((struct bk_register_##type *)((handle) & ~BK_REGISTER_TYPE_MASK))

/* Obtém o handle a partir de um registro */
#define BK_REGISTER_TO_HANDLE(reg, type) \
	((BK_REGISTER_HANDLE)((uintptr_t)(reg) | \
	 (((BK_REGISTER_TYPE_##type) & BK_REGISTER_TYPE_MASK) << 48)))

/*
 * ============================================================================
 * FUNÇÕES DO SISTEMA DE REGISTROS
 * ============================================================================
 */

/* Inicialização do sistema */
BK_I32 bk_register_system_init(void);
void bk_register_system_shutdown(void);

/* Criação e destruição de registros */
BK_REGISTER_HANDLE bk_register_create(BK_REGISTER_TYPE type, 
				      BK_UID owner, 
				      BK_GID group,
				      BK_UINT16 permissions);
BK_I32 bk_register_destroy(BK_REGISTER_HANDLE handle);

/* Gerenciamento de referências */
BK_REGISTER_HANDLE bk_register_duplicate(BK_REGISTER_HANDLE handle);
BK_I32 bk_register_close(BK_REGISTER_HANDLE handle);
BK_UINT32 bk_register_get_refcount(BK_REGISTER_HANDLE handle);

/* Acesso a dados do registro */
BK_REGISTER_HEADER *bk_register_get_header(BK_REGISTER_HANDLE handle);
void *bk_register_get_data(BK_REGISTER_HANDLE handle);

/* Verificação de tipo */
BK_BOOL bk_register_is_type(BK_REGISTER_HANDLE handle, BK_REGISTER_TYPE type);
BK_REGISTER_TYPE bk_register_get_type(BK_REGISTER_HANDLE handle);

/* Controle de acesso */
BK_BOOL bk_register_check_access(BK_REGISTER_HANDLE handle,
				 BK_UID uid, BK_GID gid,
				 BK_UINT8 access_wanted);
BK_I32 bk_register_set_permissions(BK_REGISTER_HANDLE handle,
				   BK_UINT16 permissions);
BK_I32 bk_register_set_owner(BK_REGISTER_HANDLE handle,
			     BK_UID uid, BK_GID gid);

/* Busca de registros */
BK_REGISTER_HANDLE bk_register_find_by_id(BK_REGISTER_TYPE type, BK_ID id);
BK_REGISTER_HANDLE bk_register_find_first(BK_REGISTER_TYPE type);
BK_REGISTER_HANDLE bk_register_find_next(BK_REGISTER_HANDLE handle);

/* Iteração sobre registros */
#define BK_REGISTER_FOREACH(var, type) \
	for (BK_REGISTER_HANDLE var = bk_register_find_first(type); \
	     var != BK_REGISTER_NULL; \
	     var = bk_register_find_next(var))

#define BK_REGISTER_FOREACH_SAFE(var, tmp, type) \
	for (BK_REGISTER_HANDLE var = bk_register_find_first(type), \
	     tmp = bk_register_find_next(var); \
	     var != BK_REGISTER_NULL; \
	     var = tmp, tmp = bk_register_find_next(tmp))

/*
 * ============================================================================
 * CAPACIDADES DE REGISTRO
 * ============================================================================
 */

/* Tipos de capacidades */
typedef enum {
	BK_CAP_NONE		= 0x00000000,
	
	/* Capacidades básicas */
	BK_CAP_READ		= 0x00000001,	/* Ler o registro */
	BK_CAP_WRITE		= 0x00000002,	/* Escrever no registro */
	BK_CAP_EXECUTE		= 0x00000004,	/* Executar o registro */
	BK_CAP_DELETE		= 0x00000008,	/* Deletar o registro */
	
	/* Capacidades de gerenciamento */
	BK_CAP_SET_PERMS	= 0x00000010,	/* Mudar permissões */
	BK_CAP_SET_OWNER	= 0x00000020,	/* Mudar dono/grupo */
	BK_CAP_DUPLICATE	= 0x00000040,	/* Duplicar handle */
	
	/* Capacidades específicas por tipo */
	BK_CAP_PROCESS_FORK	= 0x00010000,	/* Fork de processo */
	BK_CAP_PROCESS_KILL	= 0x00020000,	/* Matar processo */
	BK_CAP_PROCESS_DEBUG	= 0x00040000,	/* Debug de processo */
	
	BK_CAP_TASK_CREATE	= 0x00100000,	/* Criar task */
	BK_CAP_TASK_DESTROY	= 0x00200000,	/* Destruir task */
	
	BK_CAP_THREAD_CREATE	= 0x01000000,	/* Criar thread */
	BK_CAP_THREAD_SUSPEND	= 0x02000000,	/* Suspender thread */
	BK_CAP_THREAD_RESUME	= 0x04000000,	/* Continuar thread */
	
	BK_CAP_ACTIVITY_SCHED	= 0x10000000,	/* Escalonar atividade */
	BK_CAP_ACTIVITY_CANCEL	= 0x20000000,	/* Cancelar atividade */
	
	/* Capacidades compostas */
	BK_CAP_ALL		= 0xFFFFFFFF,
} BK_CAPABILITY_TYPE;

/* Estrutura de capacidades */
struct bk_capability {
	BK_REGISTER_HEADER	header;
	
	BK_CAPABILITY_TYPE	caps;		/* Bits de capacidades */
	BK_REGISTER_HANDLE	c_target;	/* Registro alvo */
	BK_REGISTER_HANDLE	c_grantor;	/* Quem concedeu */
	BK_TIME			c_expires;	/* Expiração (0 = nunca) */
};

typedef struct bk_capability BK_CAPABILITY;

/* Funções de capacidade */
BK_REGISTER_HANDLE bk_cap_create(BK_REGISTER_HANDLE target,
				 BK_CAPABILITY_TYPE caps,
				 BK_TIME expires);
BK_BOOL bk_cap_check(BK_REGISTER_HANDLE handle,
		     BK_CAPABILITY_TYPE needed);
BK_I32 bk_cap_grant(BK_REGISTER_HANDLE from,
		    BK_REGISTER_HANDLE to,
		    BK_CAPABILITY_TYPE caps);
BK_I32 bk_cap_revoke(BK_REGISTER_HANDLE cap_handle);
BK_I32 bk_cap_transfer(BK_REGISTER_HANDLE cap_handle,
		       BK_REGISTER_HANDLE new_owner);

/*
 * ============================================================================
 * REGISTROS ESPECÍFICOS DO SISTEMA
 * ============================================================================
 */

/* Registro de Processo (container na memória) */
struct bk_process_register {
	BK_REGISTER_HEADER	header;
	
	/* Identificação */
	BK_PID			pid;
	BK_PID			ppid;
	BK_PID			pgid;
	BK_PID			sid;
	
	/* Tasks gerenciadas */
	BK_TAILQ_HEAD(, bk_task_register) tasks;
	BK_UINT32		task_count;
	
	/* Espaço de endereço (registro separado) */
	BK_REGISTER_HANDLE	addrspace;
	
	/* Limites de recursos */
	BK_RLIM			rlimits[16];
	
	/* Credenciais */
	BK_UID			uid;
	BK_UID			euid;
	BK_UID			suid;
	BK_GID			gid;
	BK_GID			egid;
	BK_GID			sgid;
	
	/* Estatísticas */
	BK_TIME			user_time;
	BK_TIME			system_time;
	BK_VM_SIZE		peak_memory;
	
	/* Flags */
	BK_UINT32		flags;
#define BK_PROCESS_FLAG_SYSTEM		0x0001
#define BK_PROCESS_FLAG_SESSION_LEADER	0x0002
#define BK_PROCESS_FLAG_GROUP_LEADER	0x0004
};

/* Registro de Task (região de recursos) */
struct bk_task_register {
	BK_REGISTER_HEADER	header;
	
	/* Identificação */
	BK_ID			task_id;
	BK_REGISTER_HANDLE	process;	/* Processo dono */
	
	/* Recursos gerenciados */
	BK_UINT64		resource_mask;	/* Quais recursos gerencia */
#define BK_RESOURCE_NETWORK	0x00000001
#define BK_RESOURCE_FILESYSTEM	0x00000002
#define BK_RESOURCE_GRAPHICS	0x00000004
#define BK_RESOURCE_AUDIO	0x00000008
#define BK_RESOURCE_INPUT	0x00000010
#define BK_RESOURCE_IPC		0x00000020
	
	/* Threads gerenciadas */
	BK_TAILQ_HEAD(, bk_thread_register) threads;
	BK_UINT32		thread_count;
	
	/* Região de memória da task */
	BK_REGISTER_HANDLE	memory_region;
	
	/* Working directory e ambiente */
	BK_REGISTER_HANDLE	cwd;
	BK_REGISTER_HANDLE	env;
};

/* Registro de Thread (unidade de execução) */
struct bk_thread_register {
	BK_REGISTER_HEADER	header;
	
	/* Identificação */
	BK_LWPID		tid;
	BK_REGISTER_HANDLE	task;		/* Task dona */
	BK_REGISTER_HANDLE	process;	/* Processo dono */
	
	/* Estado */
	BK_UINT8		state;
	BK_UINT8		priority;
	
	/* Contexto de execução */
	BK_REGISTER_HANDLE	context;	/* Registradores */
	BK_REGISTER_HANDLE	stack;		/* Pilha */
	BK_REGISTER_HANDLE	kstack;		/* Pilha do kernel */
	
	/* Atividades */
	BK_TAILQ_HEAD(, bk_activity_register) activities;
	BK_REGISTER_HANDLE	current_activity;
	BK_UINT32		activity_count;
	
	/* TLS */
	BK_REGISTER_HANDLE	tls;
	
	/* Escalonamento */
	BK_UINT32		cpu_affinity;
	BK_UINT64		quantum;
	BK_TIME			last_run;
};

/* Registro de Atividade (função atômica) */
struct bk_activity_register {
	BK_REGISTER_HEADER	header;
	
	/* Identificação */
	BK_ID			activity_id;
	BK_UINT8		type;
	BK_UINT8		state;
	
	/* Execução */
	BK_REGISTER_HANDLE	thread;		/* Thread executando */
	BK_REGISTER_HANDLE	handler_cap;	/* Capacidade do handler */
	BK_REGISTER_HANDLE	data;		/* Dados da atividade */
	
	/* Resultado */
	BK_I32			result;
	BK_REGISTER_HANDLE	error;
	
	/* Estatísticas */
	BK_TIME			created;
	BK_TIME			started;
	BK_TIME			completed;
	BK_UINT64		cpu_cycles;
};

/* Registro de Arquivo (quando usado) */
struct bk_file_register {
	BK_REGISTER_HEADER	header;
	
	/* Nome e localização */
	BK_REGISTER_HANDLE	name;		/* String com nome */
	BK_REGISTER_HANDLE	parent;		/* Diretório pai */
	
	/* Dados */
	BK_REGISTER_HANDLE	data;		/* Conteúdo do arquivo */
	BK_UINT64		size;
	
	/* Posição (para arquivos abertos) */
	BK_UINT64		offset;
	BK_UINT32		open_flags;
	
	/* Device (se for dispositivo) */
	BK_DEV			dev;
};

/* Registro de Log */
struct bk_log_register {
	BK_REGISTER_HEADER	header;
	
	/* Classificação */
	BK_UINT8		subsystem;
	BK_UINT8		level;
	BK_UINT32		error_code;
	
	/* Contexto */
	BK_REGISTER_HANDLE	process;
	BK_REGISTER_HANDLE	thread;
	BK_REGISTER_HANDLE	activity;
	
	/* Mensagem */
	char			message[256];
};

/*
 * ============================================================================
 * FUNÇÕES AUXILIARES
 * ============================================================================
 */

/* Estatísticas do sistema de registros */
struct bk_register_stats {
	BK_UINT64	total_registers;
	BK_UINT64	total_capabilities;
	BK_UINT64	handles_active;
	BK_UINT64	memory_used;
	BK_UINT64	lookups;
	BK_UINT64	cache_hits;
};

BK_I32 bk_register_get_stats(struct bk_register_stats *stats);

/* Debug */
#ifdef _BK_REGSISTER_DEBUG
void bk_register_dump_all(void);
void bk_register_dump_type(BK_REGISTER_TYPE type);
void bk_register_dump_handle(BK_REGISTER_HANDLE handle);
void bk_register_dump_capabilities(BK_REGISTER_HANDLE handle);
#endif

/*
 * ============================================================================
 * MACROS DE CONVENIÊNCIA
 * ============================================================================
 */

/* Acesso rápido a registros específicos */
#define BK_PROCESS_REG(p)	((struct bk_process_register *)p)
#define BK_TASK_REG(t)		((struct bk_task_register *)t)
#define BK_THREAD_REG(t)	((struct bk_thread_register *)t)
#define BK_ACTIVITY_REG(a)	((struct bk_activity_register *)a)
#define BK_FILE_REG(f)		((struct bk_file_register *)f)
#define BK_CAP_REG(c)		((struct bk_capability *)c)

/* Verificações rápidas */
#define BK_IS_PROCESS(h)	(bk_register_get_type(h) == BK_REGISTER_TYPE_PROCESS)
#define BK_IS_TASK(h)		(bk_register_get_type(h) == BK_REGISTER_TYPE_TASK)
#define BK_IS_THREAD(h)		(bk_register_get_type(h) == BK_REGISTER_TYPE_THREAD)
#define BK_IS_ACTIVITY(h)	(bk_register_get_type(h) == BK_REGISTER_TYPE_ACTIVITY)
#define BK_IS_FILE(h)		(bk_register_get_type(h) == BK_REGISTER_TYPE_FILE)
#define BK_IS_CAPABILITY(h)	(bk_register_get_type(h) == BK_REGISTER_TYPE_CAPABILITY)

/* Atalhos para criação */
#define BK_CREATE_PROCESS(owner, perms) \
	bk_register_create(BK_REGISTER_TYPE_PROCESS, owner, perms)

#define BK_CREATE_TASK(owner, perms) \
	bk_register_create(BK_REGISTER_TYPE_TASK, owner, perms)

#define BK_CREATE_THREAD(owner, perms) \
	bk_register_create(BK_REGISTER_TYPE_THREAD, owner, perms)

#define BK_CREATE_ACTIVITY(owner, perms) \
	bk_register_create(BK_REGISTER_TYPE_ACTIVITY, owner, perms)

#define BK_CREATE_FILE(owner, perms) \
	bk_register_create(BK_REGISTER_TYPE_FILE, owner, perms)

#endif /* !_BIBLE_REGSISTER_H_ */