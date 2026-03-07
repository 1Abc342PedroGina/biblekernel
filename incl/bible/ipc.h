#ifndef _BIBLE_IPC_H_
#define _BIBLE_IPC_H_

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/ktypes.h>
#include <bible/queue.h>
#include <bible/kernl_object.h>
#include <bible/vm.h>
#include <bible/pmap.h>
#include <bible/regsister.h>
#include <bible/task.h>

/* Código de retorno para operações bem-sucedidas */
#define BK_SUCCESS          0
/* Código de erro para mensagem muito grande em operações IPC */
#define BK_IPC_EMSGSIZE     40  /* Tamanho da mensagem excede o limite */
/* Tamanho máximo permitido para mensagens IPC */
#define BK_IPC_MAX_MSG_SIZE     65536  /* 64 KB */
/* Código de erro para argumento inválido em operações IPC */
#define BK_IPC_EINVAL       22  /* Argumento inválido */

typedef struct __bk_hashtable {
    BK_UINT32   ht_size;        /* Número de buckets */
    BK_UINT32   ht_count;       /* Número de elementos */
    BK_LIST_HEAD(, __bk_hash_entry) *ht_buckets;  /* Array de buckets */
    BK_SPINLOCK ht_lock;        /* Lock para concorrência */
} BK_HASHTABLE;

/*
 * Entrada da tabela hash
 */
struct __bk_hash_entry {
    BK_UINT64       he_key;     /* Chave */
    void            *he_value;  /* Valor associado */
    BK_LIST_ENTRY(__bk_hash_entry) he_link;  /* Link na lista do bucket */
};

typedef struct __bk_hash_entry BK_HASH_ENTRY;


typedef struct __bk_ipc_entry_list BK_IPC_ENTRY_LIST;
/*
 * Sistema de Eventos do Kernel
 */

/* Tipos de evento */
typedef enum {
    BK_EVENT_TYPE_NONE         = 0,  /* Tipo inválido */
    BK_EVENT_TYPE_SIGNAL       = 1,  /* Sinal */
    BK_EVENT_TYPE_TIMER        = 2,  /* Timer expirado */
    BK_EVENT_TYPE_IPC          = 3,  /* Mensagem IPC recebida */
    BK_EVENT_TYPE_IO           = 4,  /* Operação de I/O completada */
    BK_EVENT_TYPE_PROCESS      = 5,  /* Evento de processo (fork, exec, exit) */
    BK_EVENT_TYPE_THREAD       = 6,  /* Evento de thread */
    BK_EVENT_TYPE_USER         = 7,  /* Evento definido pelo usuário */
} BK_EVENT_TYPE;

/* Estados de um evento */
typedef enum {
    BK_EVENT_PENDING    = 0,  /* Evento pendente */
    BK_EVENT_SENT       = 1,  /* Evento enviado */
    BK_EVENT_RECEIVED   = 2,  /* Evento recebido */
    BK_EVENT_PROCESSED  = 3,  /* Evento processado */
    BK_EVENT_CANCELLED  = 4,  /* Evento cancelado */
    BK_EVENT_TIMEDOUT   = 5,  /* Evento expirou */
} BK_EVENT_STATE;

/* Prioridades de evento */
#define BK_EVENT_PRIORITY_LOW       0
#define BK_EVENT_PRIORITY_NORMAL    1
#define BK_EVENT_PRIORITY_HIGH      2
#define BK_EVENT_PRIORITY_CRITICAL  3

/*
 * Estrutura principal de evento
 */
struct __bk_event {
    BK_ID           ev_id;              /* ID único do evento */
    BK_EVENT_TYPE   ev_type;            /* Tipo do evento */
    BK_EVENT_STATE  ev_state;           /* Estado atual */
    BK_UINT8        ev_priority;        /* Prioridade */
    
    /* Origem e destino */
    BK_ID           ev_source;          /* Objeto de origem */
    BK_ID           ev_target;          /* Objeto de destino */
    
    /* Dados do evento */
    union {
        struct {
            BK_I32  sig_num;            /* Número do sinal */
        } signal;
        
        struct {
            BK_ID   timer_id;            /* ID do timer */
            BK_TIME expiry_time;         /* Tempo de expiração */
        } timer;
        
        struct {
            BK_ID   msg_id;              /* ID da mensagem */
            BK_SIZE msg_size;             /* Tamanho da mensagem */
            void    *msg_data;           /* Dados da mensagem */
        } ipc;
        
        struct {
            BK_I32  fd;                   /* File descriptor */
            BK_I32  result;               /* Resultado da operação */
        } io;
        
        struct {
            BK_PID  pid;                  /* PID do processo */
            BK_I32  status;                /* Status do processo */
        } process;
        
        struct {
            BK_LWPID tid;                  /* Thread ID */
        } thread;
        
        struct {
            BK_UINT32 user_data1;          /* Dados definidos pelo usuário */
            BK_UINT32 user_data2;
            void    *user_ptr;
        } user;
    } ev_data;
    
    /* Temporização */
    BK_TIME         ev_creation_time;   /* Tempo de criação */
    BK_TIME         ev_expiry_time;      /* Tempo de expiração (0 = sem expiração) */
    BK_TIME         ev_delivery_time;    /* Tempo de entrega */
    
    /* Controle */
    BK_UINT32       ev_flags;
#define BK_EVENT_FLAG_SYSTEM        0x0001  /* Evento do sistema */
#define BK_EVENT_FLAG_USER          0x0002  /* Evento do usuário */
#define BK_EVENT_FLAG_BROADCAST     0x0004  /* Evento broadcast */
#define BK_EVENT_FLAG_PRIVATE       0x0008  /* Evento privado */
#define BK_EVENT_FLAG_PERSISTENT    0x0010  /* Evento persistente */
    
    /* Callbacks */
    void            (*ev_handler)(struct __bk_event *ev, void *context);
    void            *ev_handler_context;
    void            (*ev_cleanup)(struct __bk_event *ev);
    
    /* Listas */
    BK_LIST_ENTRY(__bk_event) ev_list_link;      /* Link em listas gerais */
    BK_LIST_ENTRY(__bk_event) ev_queue_link;     /* Link em filas de evento */
    BK_TAILQ_ENTRY(__bk_event) ev_target_link;   /* Link no objeto alvo */
    
    /* Sincronização */
    BK_SPINLOCK     ev_lock;
    BK_SEMAPHORE    ev_completion_sem;
    
    /* Resultado */
    BK_I32          ev_result;
    BK_I32          ev_error;
};

typedef struct __bk_event BK_EVENT;

/* Listas de eventos */
BK_LIST_HEAD(__bk_event_list, __bk_event);
typedef struct __bk_event_list BK_EVENT_LIST;

BK_TAILQ_HEAD(__bk_event_queue, __bk_event);
typedef struct __bk_event_queue BK_EVENT_QUEUE;

/*
 * Funções para manipulação de eventos
 */

/* Criação/destruição */
BK_EVENT *bk_event_create(BK_EVENT_TYPE type, BK_UINT8 priority);
void bk_event_destroy(BK_EVENT *ev);
BK_I32 bk_event_init(BK_EVENT *ev, BK_EVENT_TYPE type, BK_UINT8 priority);
void bk_event_fini(BK_EVENT *ev);

/* Envio e recebimento */
BK_I32 bk_event_send(BK_EVENT *ev, BK_ID target, BK_UINT32 flags);
BK_I32 bk_event_send_sync(BK_EVENT *ev, BK_ID target, BK_TIME timeout);
BK_EVENT *bk_event_receive(BK_ID source, BK_EVENT_TYPE type, BK_TIME timeout);
BK_I32 bk_event_receive_into(BK_EVENT *ev, BK_ID source, BK_EVENT_TYPE type, BK_TIME timeout);

/* Cancelamento e timeout */
BK_I32 bk_event_cancel(BK_EVENT *ev);
BK_I32 bk_event_set_timeout(BK_EVENT *ev, BK_TIME timeout);
BK_I32 bk_event_clear_timeout(BK_EVENT *ev);

/* Status e informações */
BK_EVENT_STATE bk_event_get_state(BK_EVENT *ev);
BK_ID bk_event_get_id(BK_EVENT *ev);
BK_EVENT_TYPE bk_event_get_type(BK_EVENT *ev);
BK_I32 bk_event_get_result(BK_EVENT *ev);
BK_I32 bk_event_get_error(BK_EVENT *ev);

/* Callbacks */
void bk_event_set_handler(BK_EVENT *ev, void (*handler)(BK_EVENT *, void *), void *context);
void bk_event_set_cleanup(BK_EVENT *ev, void (*cleanup)(BK_EVENT *));

/* Espera por eventos */
BK_I32 bk_event_wait(BK_EVENT *ev, BK_TIME timeout);
BK_I32 bk_event_wait_any(BK_EVENT **ev, BK_UINT32 count, BK_TIME timeout);

/* Macros úteis */
#define BK_EVENT_IS_PENDING(ev)     ((ev)->ev_state == BK_EVENT_PENDING)
#define BK_EVENT_IS_SENT(ev)        ((ev)->ev_state == BK_EVENT_SENT)
#define BK_EVENT_IS_RECEIVED(ev)    ((ev)->ev_state == BK_EVENT_RECEIVED)
#define BK_EVENT_IS_PROCESSED(ev)   ((ev)->ev_state == BK_EVENT_PROCESSED)
#define BK_EVENT_IS_CANCELLED(ev)   ((ev)->ev_state == BK_EVENT_CANCELLED)
#define BK_EVENT_HAS_TIMEDOUT(ev)   ((ev)->ev_state == BK_EVENT_TIMEDOUT)

#define BK_EVENT_SET_DATA(ev, type, field, value) \
    do { (ev)->ev_data.type.field = (value); } while(0)

#define BK_EVENT_GET_DATA(ev, type, field) \
    ((ev)->ev_data.type.field)

/*
 * Eventos predefinidos do sistema
 */
extern BK_EVENT *bk_event_null;        /* Evento nulo */
extern BK_EVENT *bk_event_quit;        /* Evento de saída */
extern BK_EVENT *bk_event_interrupt;   /* Evento de interrupção */

/*
 * Inicialização do sistema de eventos
 */
BK_I32 bk_event_system_init(void);
void bk_event_system_shutdown(void);

/*
 * Funções para manipulação da hashtable (declarações)
 */
BK_HASHTABLE *bk_hashtable_create(BK_UINT32 size);
void bk_hashtable_destroy(BK_HASHTABLE *ht);
BK_I32 bk_hashtable_insert(BK_HASHTABLE *ht, BK_UINT64 key, void *value);
void *bk_hashtable_lookup(BK_HASHTABLE *ht, BK_UINT64 key);
BK_I32 bk_hashtable_remove(BK_HASHTABLE *ht, BK_UINT64 key);
void bk_hashtable_clear(BK_HASHTABLE *ht);
BK_UINT32 bk_hashtable_count(BK_HASHTABLE *ht);

/*
 * Modelos de kernel suportados
 */
#define BK_IPC_MODEL_MICROKERNEL	0	/* IPC como serviço do kernel */
#define BK_IPC_MODEL_MONOLITHIC		1	/* IPC como syscall */
#define BK_IPC_MODEL_EXOKERNEL		2	/* IPC direto entre apps */
#define BK_IPC_MODEL_NANOKERNEL		3	/* IPC mínimo e otimizado */

#ifndef BK_IPC_MODEL
#define BK_IPC_MODEL	BK_IPC_MODEL_MICROKERNEL	/* Modelo padrão */
#endif

/*
 * Tipos de IPC
 */
typedef enum {
	BK_IPC_TYPE_MESSAGE	= 0,	/* Mensagens */
	BK_IPC_TYPE_SHARED_MEMORY = 1,	/* Memória compartilhada */
	BK_IPC_TYPE_PIPE	= 2,	/* Pipes */
	BK_IPC_TYPE_FIFO	= 3,	/* FIFOs (named pipes) */
	BK_IPC_TYPE_SOCKET	= 4,	/* Sockets */
	BK_IPC_TYPE_SEMAPHORE	= 5,	/* Semáforos */
	BK_IPC_TYPE_MUTEX	= 6,	/* Mutexes */
	BK_IPC_TYPE_CONDVAR	= 7,	/* Variáveis de condição */
	BK_IPC_TYPE_EVENT	= 8,	/* Eventos */
	BK_IPC_TYPE_MAILBOX	= 9,	/* Mailboxes */
	BK_IPC_TYPE_PORT	= 10,	/* Portas */
	BK_IPC_TYPE_CHANNEL	= 11,	/* Canais */
	BK_IPC_TYPE_QUEUE	= 12,	/* Filas de mensagens */
	BK_IPC_TYPE_RENDEZVOUS	= 13,	/* Ponto de encontro */
	BK_IPC_TYPE_BARRIER	= 14,	/* Barreiras */
	BK_IPC_TYPE_RPC		= 15,	/* Chamada de procedimento remoto */
} BK_IPC_TYPE;

/*
 * Permissões de IPC
 */
#define BK_IPC_PERM_READ	0x01	/* Permissão de leitura */
#define BK_IPC_PERM_WRITE	0x02	/* Permissão de escrita */
#define BK_IPC_PERM_EXECUTE	0x04	/* Permissão de execução */
#define BK_IPC_PERM_OWNER	0x08	/* Permissão de proprietário */
#define BK_IPC_PERM_GROUP	0x10	/* Permissão de grupo */
#define BK_IPC_PERM_OTHER	0x20	/* Permissão para outros */
#define BK_IPC_PERM_TRANSFER	0x40	/* Permissão para transferir */

/*
 * Flags de IPC
 */
#define BK_IPC_FLAG_CREATE	0x0001	/* Criar se não existir */
#define BK_IPC_FLAG_EXCLUSIVE	0x0002	/* Criação exclusiva */
#define BK_IPC_FLAG_NONBLOCK	0x0004	/* Operação não bloqueante */
#define BK_IPC_FLAG_PERSISTENT	0x0008	/* IPC persistente */
#define BK_IPC_FLAG_PRIVATE	0x0010	/* IPC privado */
#define BK_IPC_FLAG_SHARED	0x0020	/* IPC compartilhado */
#define BK_IPC_FLAG_SECURE	0x0040	/* IPC seguro (criptografado) */
#define BK_IPC_FLAG_FASTPATH	0x0080	/* Caminho rápido (bypass kernel) */
#define BK_IPC_FLAG_BUFFERED	0x0100	/* IPC com buffer */
#define BK_IPC_FLAG_STREAM	0x0200	/* IPC em stream */
#define BK_IPC_FLAG_DATAGRAM	0x0400	/* IPC em datagrama */
#define BK_IPC_FLAG_RELIABLE	0x0800	/* Entrega confiável */
#define BK_IPC_FLAG_ORDERED	0x1000	/* Entrega ordenada */

/*
 * ============================================================================
 * ESTRUTURAS COMUNS DE IPC
 * ============================================================================
 */

/* Identificador de IPC */
typedef BK_ID BK_IPC_ID;

/* Handle de IPC */
typedef struct __bk_ipc_handle *BK_IPC_HANDLE;

/* Endpoint de IPC (para conexões) */
typedef struct __bk_ipc_endpoint *BK_IPC_ENDPOINT;

/* Mensagem de IPC */
struct __bk_ipc_message {
	BK_IPC_ID	msg_id;		/* ID da mensagem */
	BK_SIZE		msg_size;	/* Tamanho dos dados */
	void		*msg_data;	/* Dados da mensagem */
	BK_IPC_ID	msg_sender;	/* ID do remetente */
	BK_IPC_ID	msg_receiver;	/* ID do destinatário */
	BK_UINT32	msg_flags;	/* Flags da mensagem */
	BK_TIME		msg_timestamp;	/* Timestamp */
	BK_UINT32	msg_seq;	/* Número de sequência */
	BK_UINT32	msg_type;	/* Tipo da mensagem */
};

typedef struct __bk_ipc_message BK_IPC_MESSAGE;

/* Cabeçalho de mensagem (para transferência otimizada) */
struct __bk_ipc_msg_header {
	BK_UINT32	mh_magic;	/* Número mágico */
	BK_UINT32	mh_version;	/* Versão do protocolo */
	BK_UINT32	mh_type;	/* Tipo da mensagem */
	BK_UINT32	mh_flags;	/* Flags */
	BK_SIZE		mh_size;	/* Tamanho dos dados */
	BK_IPC_ID	mh_sender;	/* Remetente */
	BK_IPC_ID	mh_receiver;	/* Destinatário */
	BK_UINT32	mh_seq;		/* Número de sequência */
	BK_UINT32	mh_crc32;	/* Checksum */
};

typedef struct __bk_ipc_msg_header BK_IPC_MSG_HEADER;

/* Região de memória compartilhada */
struct __bk_ipc_shm {
	BK_IPC_ID	shm_id;		/* ID da região */
	BK_SIZE		shm_size;	/* Tamanho da região */
	BK_VM_OFFSET	shm_addr;	/* Endereço virtual */
	BK_PM_PADDR	shm_phys;	/* Endereço físico (se mapeado) */
	BK_UINT32	shm_refcount;	/* Contador de referências */
	BK_UINT32	shm_perms;	/* Permissões */
	BK_UINT32	shm_flags;	/* Flags */
	BK_SPINLOCK	shm_lock;	/* Lock da região */
};

typedef struct __bk_ipc_shm BK_IPC_SHM;

/* Pipe/FIFO */
struct __bk_ipc_pipe {
	BK_IPC_ID	pipe_id;	/* ID do pipe */
	BK_SIZE		pipe_size;	/* Tamanho do buffer */
	void		*pipe_buffer;	/* Buffer do pipe */
	BK_SIZE		pipe_read_pos;	/* Posição de leitura */
	BK_SIZE		pipe_write_pos;	/* Posição de escrita */
	BK_UI32		pipe_refcount[2];	/* Referências (leitura/escrita) */
	BK_SEMAPHORE	pipe_sem_empty;	/* Semáforo para vazio */
	BK_SEMAPHORE	pipe_sem_full;	/* Semáforo para cheio */
	BK_MUTEX	pipe_mutex;	/* Mutex para acesso */
};

typedef struct __bk_ipc_pipe BK_IPC_PIPE;

/* Semáforo de IPC */
struct __bk_ipc_semaphore {
	BK_IPC_ID	sem_id;		/* ID do semáforo */
	BK_I32		sem_value;	/* Valor atual */
	BK_I32		sem_max;	/* Valor máximo */
	BK_UINT32	sem_flags;	/* Flags */
	BK_PROCESS_LIST sem_waiters;	/* Processos esperando */
	BK_SPINLOCK	sem_lock;	/* Lock do semáforo */
};

typedef struct __bk_ipc_semaphore BK_IPC_SEMAPHORE;

/* Mutex de IPC */
struct __bk_ipc_mutex {
	BK_IPC_ID	mutex_id;	/* ID do mutex */
	BK_BOOLEAN	mutex_locked;	/* Estado do lock */
	BK_THREAD	*mutex_owner;	/* Thread proprietária */
	BK_UINT32	mutex_recursion;	/* Contador de recursão */
	BK_UINT32	mutex_flags;	/* Flags */
	BK_THREAD_LIST	mutex_waiters;	/* Threads esperando */
	BK_SPINLOCK	mutex_lock;	/* Lock interno */
};

typedef struct __bk_ipc_mutex BK_IPC_MUTEX;

/* Variável de condição de IPC */
struct __bk_ipc_condvar {
	BK_IPC_ID	cond_id;	/* ID da variável de condição */
	BK_IPC_MUTEX	*cond_mutex;	/* Mutex associado */
	BK_UINT32	cond_waiters;	/* Número de waiters */
	BK_THREAD_LIST	cond_wait_list;	/* Lista de threads esperando */
	BK_SPINLOCK	cond_lock;	/* Lock da variável */
};

typedef struct __bk_ipc_condvar BK_IPC_CONDVAR;

/* Evento de IPC */
struct __bk_ipc_event {
	BK_IPC_ID	event_id;	/* ID do evento */
	BK_BOOLEAN	event_signaled;	/* Estado do sinal */
	BK_UINT32	event_flags;	/* Flags */
#define BK_IPC_EVENT_AUTO_RESET	0x01	/* Reset automático */
#define BK_IPC_EVENT_MANUAL_RESET	0x02	/* Reset manual */
	BK_THREAD_LIST	event_waiters;	/* Threads esperando */
	BK_SPINLOCK	event_lock;	/* Lock do evento */
};

typedef struct __bk_ipc_event BK_IPC_EVENT;

/* Mailbox de IPC */
struct __bk_ipc_mailbox {
	BK_IPC_ID	mbx_id;		/* ID da mailbox */
	BK_SIZE		mbx_capacity;	/* Capacidade máxima */
	BK_IPC_MESSAGE	*mbx_messages;	/* Array de mensagens */
	BK_SIZE		mbx_count;	/* Número de mensagens */
	BK_SIZE		mbx_read_idx;	/* Índice de leitura */
	BK_SIZE		mbx_write_idx;	/* Índice de escrita */
	BK_SEMAPHORE	mbx_sem_empty;	/* Semáforo para vazio */
	BK_SEMAPHORE	mbx_sem_full;	/* Semáforo para cheio */
	BK_MUTEX	mbx_mutex;	/* Mutex para acesso */
};

typedef struct __bk_ipc_mailbox BK_IPC_MAILBOX;

/*
 * Sistema de Pipes do Kernel
 */

/* Direções do pipe */
typedef enum {
    BK_PIPE_READ        = 0x01,    /* Extremidade de leitura */
    BK_PIPE_WRITE       = 0x02,    /* Extremidade de escrita */
    BK_PIPE_BOTH        = 0x03,    /* Ambas extremidades */
} BK_PIPE_DIR;

/* Flags de criação do pipe */
#define BK_PIPE_FLAG_NONBLOCK   0x0001  /* Operações não bloqueantes */
#define BK_PIPE_FLAG_DIRECT      0x0002  /* Pipe direto (sem buffer) */
#define BK_PIPE_FLAG_SYSTEM      0x0004  /* Pipe do sistema */
#define BK_PIPE_FLAG_USER        0x0008  /* Pipe do usuário */

/* Flags de operação do pipe */
#define BK_PIPE_O_NONBLOCK   0x0001  /* Não bloqueante */
#define BK_PIPE_O_SYNC       0x0002  /* Escrita síncrona */
#define BK_PIPE_O_ASYNC      0x0004  /* Escrita assíncrona */

/* Estados do pipe */
typedef enum {
    BK_PIPE_STATE_OPEN      = 0,    /* Pipe aberto */
    BK_PIPE_STATE_CLOSED    = 1,    /* Pipe fechado */
    BK_PIPE_STATE_BROKEN    = 2,    /* Pipe quebrado (sem leitores/escritores) */
    BK_PIPE_STATE_ERROR     = 3,    /* Estado de erro */
} BK_PIPE_STATE;

/*
 * Estrutura principal do pipe
 */
struct __bk_pipe {
    BK_ID           pipe_id;            /* ID único do pipe */
    BK_PIPE_STATE   pipe_state;         /* Estado atual */
    BK_UINT32       pipe_flags;         /* Flags de criação */
    BK_SIZE         pipe_buffer_size;    /* Tamanho do buffer */
    BK_SIZE         pipe_buffer_used;    /* Espaço usado no buffer */
    
    /* Buffer circular */
    BK_UINT8        *pipe_buffer;        /* Buffer de dados */
    BK_SIZE         pipe_read_pos;       /* Posição de leitura */
    BK_SIZE         pipe_write_pos;      /* Posição de escrita */
    
    /* Extremidades */
    BK_ID           pipe_read_fd;        /* File descriptor de leitura */
    BK_ID           pipe_write_fd;       /* File descriptor de escrita */
    
    /* Leitores e escritores */
    BK_UINT32       pipe_readers;        /* Número de leitores */
    BK_UINT32       pipe_writers;        /* Número de escritores */
    
    /* Processo proprietário */
    BK_PID          pipe_owner_pid;      /* PID do processo dono */
    
    /* Listas */
    BK_LIST_ENTRY(__bk_pipe) pipe_list_link;    /* Link na lista global */
    
    /* Sincronização */
    BK_SPINLOCK     pipe_lock;            /* Lock principal */
    BK_SEMAPHORE    pipe_read_sem;        /* Semáforo para leitura */
    BK_SEMAPHORE    pipe_write_sem;       /* Semáforo para escrita */
    BK_MUTEX        pipe_mutex;           /* Mutex para acesso ao buffer */
    BK_CONDVAR      pipe_read_cond;       /* Condição para leitura */
    BK_CONDVAR      pipe_write_cond;      /* Condição para escrita */
    
    /* Eventos */
    BK_EVENT        *pipe_read_event;      /* Evento de leitura disponível */
    BK_EVENT        *pipe_write_event;     /* Evento de escrita disponível */
    BK_EVENT        *pipe_hangup_event;    /* Evento de hangup */
    
    /* Estatísticas */
    BK_UINT64       pipe_bytes_read;       /* Total de bytes lidos */
    BK_UINT64       pipe_bytes_written;    /* Total de bytes escritos */
    BK_UINT32       pipe_read_ops;         /* Número de operações de leitura */
    BK_UINT32       pipe_write_ops;        /* Número de operações de escrita */
    BK_UINT32       pipe_read_blocked;     /* Leitores bloqueados */
    BK_UINT32       pipe_write_blocked;    /* Escritores bloqueados */
    
    /* Timeout */
    BK_TIME         pipe_read_timeout;     /* Timeout para leitura */
    BK_TIME         pipe_write_timeout;    /* Timeout para escrita */
    
    /* Dados do usuário */
    void            *pipe_private;         /* Dados privados */
};

typedef struct __bk_pipe BK_PIPE;

/* Lista de pipes */
BK_LIST_HEAD(__bk_pipe_list, __bk_pipe);
typedef struct __bk_pipe_list BK_PIPE_LIST;

/*
 * Funções para criação e destruição
 */

/* Cria um novo pipe */
BK_I32 bk_pipe_create(BK_PIPE **pipe, BK_UINT32 flags, BK_SIZE buffer_size);

/* Cria um pipe com descritores de arquivo */
BK_I32 bk_pipe_open(BK_ID *read_fd, BK_ID *write_fd, BK_UINT32 flags);

/* Fecha uma extremidade do pipe */
BK_I32 bk_pipe_close(BK_PIPE *pipe, BK_PIPE_DIR dir);

/* Fecha o pipe completamente */
BK_I32 bk_pipe_destroy(BK_PIPE *pipe);

/*
 * Operações de leitura e escrita
 */

/* Lê dados do pipe */
BK_SSIZE bk_pipe_read(BK_PIPE *pipe, void *buffer, BK_SIZE size, BK_UINT32 flags);

/* Lê dados do pipe com timeout */
BK_SSIZE bk_pipe_read_timeout(BK_PIPE *pipe, void *buffer, BK_SIZE size, BK_TIME timeout);

/* Escreve dados no pipe */
BK_SSIZE bk_pipe_write(BK_PIPE *pipe, const void *buffer, BK_SIZE size, BK_UINT32 flags);

/* Escreve dados no pipe com timeout */
BK_SSIZE bk_pipe_write_timeout(BK_PIPE *pipe, const void *buffer, BK_SIZE size, BK_TIME timeout);

/* Lê dados sem remover do buffer (peek) */
BK_SSIZE bk_pipe_peek(BK_PIPE *pipe, void *buffer, BK_SIZE size);

/*
 * Operações de controle
 */

/* Configura flags do pipe */
BK_I32 bk_pipe_set_flags(BK_PIPE *pipe, BK_UINT32 flags);

/* Obtém flags do pipe */
BK_UINT32 bk_pipe_get_flags(BK_PIPE *pipe);

/* Configura timeouts */
BK_I32 bk_pipe_set_timeout(BK_PIPE *pipe, BK_TIME read_timeout, BK_TIME write_timeout);

/* Limpa o buffer do pipe */
BK_I32 bk_pipe_clear(BK_PIPE *pipe);

/* Obtém espaço disponível para leitura */
BK_SIZE bk_pipe_available_read(BK_PIPE *pipe);

/* Obtém espaço disponível para escrita */
BK_SIZE bk_pipe_available_write(BK_PIPE *pipe);

/*
 * Operações de bloqueio
 */

/* Bloqueia até que dados estejam disponíveis para leitura */
BK_I32 bk_pipe_wait_read(BK_PIPE *pipe, BK_TIME timeout);

/* Bloqueia até que espaço esteja disponível para escrita */
BK_I32 bk_pipe_wait_write(BK_PIPE *pipe, BK_TIME timeout);

/*
 * Informações e estatísticas
 */

/* Obtém estado do pipe */
BK_PIPE_STATE bk_pipe_get_state(BK_PIPE *pipe);

/* Obtém ID do pipe */
BK_ID bk_pipe_get_id(BK_PIPE *pipe);

/* Obtém estatísticas do pipe */
void bk_pipe_get_stats(BK_PIPE *pipe, struct __bk_pipe_stats *stats);

/* Estrutura de estatísticas do pipe */
struct __bk_pipe_stats {
    BK_SIZE         ps_buffer_size;
    BK_SIZE         ps_buffer_used;
    BK_UINT32       ps_readers;
    BK_UINT32       ps_writers;
    BK_UINT64       ps_bytes_read;
    BK_UINT64       ps_bytes_written;
    BK_UINT32       ps_read_ops;
    BK_UINT32       ps_write_ops;
    BK_UINT32       ps_read_blocked;
    BK_UINT32       ps_write_blocked;
    BK_PIPE_STATE   ps_state;
};

/*
 * Funções de gerenciamento do sistema
 */

/* Inicializa o sistema de pipes */
BK_I32 bk_pipe_system_init(void);

/* Finaliza o sistema de pipes */
void bk_pipe_system_shutdown(void);

/* Obtém estatísticas globais do sistema de pipes */
void bk_pipe_system_get_stats(struct __bk_pipe_system_stats *stats);

/* Estrutura de estatísticas globais */
struct __bk_pipe_system_stats {
    BK_UINT32       pss_total_pipes;
    BK_UINT32       pss_active_pipes;
    BK_UINT32       pss_closed_pipes;
    BK_UINT64       pss_total_bytes_read;
    BK_UINT64       pss_total_bytes_written;
    BK_UINT32       pss_total_read_ops;
    BK_UINT32       pss_total_write_ops;
    BK_SIZE         pss_total_buffer_size;
    BK_SIZE         pss_total_buffer_used;
};

/*
 * Macros úteis
 */
#define BK_PIPE_IS_READABLE(pipe)   (bk_pipe_available_read(pipe) > 0)
#define BK_PIPE_IS_WRITABLE(pipe)   (bk_pipe_available_write(pipe) > 0)
#define BK_PIPE_IS_OPEN(pipe)       ((pipe)->pipe_state == BK_PIPE_STATE_OPEN)
#define BK_PIPE_IS_CLOSED(pipe)     ((pipe)->pipe_state == BK_PIPE_STATE_CLOSED)
#define BK_PIPE_IS_BROKEN(pipe)     ((pipe)->pipe_state == BK_PIPE_STATE_BROKEN)

#define BK_PIPE_HAS_READERS(pipe)   ((pipe)->pipe_readers > 0)
#define BK_PIPE_HAS_WRITERS(pipe)   ((pipe)->pipe_writers > 0)

/*
 * Compatibilidade com POSIX
 */
#ifndef _BK_NO_COMPAT
#define pipe(fds)               bk_pipe_open(&(fds)[0], &(fds)[1], 0)
#define pipe2(fds, flags)       bk_pipe_open(&(fds)[0], &(fds)[1], flags)
#endif /* !_BK_NO_COMPAT */

/*
 * Sistema de Filas (Queues) do Kernel
 */

/* Tipos de fila */
typedef enum {
    BK_QUEUE_TYPE_FIFO      = 0,    /* First In, First Out */
    BK_QUEUE_TYPE_LIFO      = 1,    /* Last In, First Out (pilha) */
    BK_QUEUE_TYPE_PRIORITY  = 2,    /* Fila de prioridade */
    BK_QUEUE_TYPE_CIRCULAR  = 3,    /* Fila circular */
    BK_QUEUE_TYPE_MSG       = 4,    /* Fila de mensagens */
} BK_QUEUE_TYPE;

/* Modos de operação */
typedef enum {
    BK_QUEUE_MODE_BLOCKING  = 0,    /* Operações bloqueantes */
    BK_QUEUE_MODE_NONBLOCK  = 1,    /* Operações não bloqueantes */
} BK_QUEUE_MODE;

/* Estados da fila */
typedef enum {
    BK_QUEUE_STATE_READY    = 0,    /* Fila pronta para uso */
    BK_QUEUE_STATE_EMPTY    = 1,    /* Fila vazia */
    BK_QUEUE_STATE_FULL     = 2,    /* Fila cheia */
    BK_QUEUE_STATE_CLOSED   = 3,    /* Fila fechada */
} BK_QUEUE_STATE;

/*
 * Estrutura de um elemento da fila
 */
struct __bk_queue_item {
    void                    *qi_data;       /* Ponteiro para os dados */
    BK_SIZE                 qi_size;        /* Tamanho dos dados */
    BK_UINT32               qi_priority;    /* Prioridade */
    BK_ID                   qi_sender;      /* ID do remetente */
    BK_TIME                 qi_timestamp;   /* Timestamp de inserção */
    
    /* Próximo elemento na fila */
    struct __bk_queue_item *qi_next;
    
    /* Anterior elemento na fila (para LIFO e filas duplamente encadeadas) */
    struct __bk_queue_item *qi_prev;
};

typedef struct __bk_queue_item BK_QUEUE_ITEM;

/*
 * Estrutura principal da fila
 */
struct __bk_queue {
    BK_ID               q_id;               /* ID único da fila */
    BK_QUEUE_TYPE       q_type;             /* Tipo da fila */
    BK_QUEUE_MODE       q_mode;             /* Modo de operação */
    BK_QUEUE_STATE      q_state;            /* Estado atual */
    BK_UINT32           q_flags;            /* Flags de criação */
    
    /* Capacidade e tamanho */
    BK_UINT32           q_max_items;        /* Número máximo de itens */
    BK_UINT32           q_current_items;    /* Número atual de itens */
    BK_SIZE             q_max_data_size;    /* Tamanho máximo dos dados */
    BK_SIZE             q_current_data_size; /* Tamanho total dos dados */
    
    /* Ponteiros para início e fim da fila */
    BK_QUEUE_ITEM       *q_head;             /* Primeiro item */
    BK_QUEUE_ITEM       *q_tail;             /* Último item */
    
    /* Processo proprietário */
    BK_PID              q_owner_pid;        /* PID do processo dono */
    
    /* Sincronização */
    BK_SPINLOCK         q_lock;              /* Lock principal */
    BK_SEMAPHORE        q_items_sem;         /* Semáforo de itens disponíveis */
    BK_SEMAPHORE        q_space_sem;         /* Semáforo de espaço disponível */
    
    /* Estatísticas */
    BK_UINT64           q_items_enqueued;    /* Total de itens enfileirados */
    BK_UINT64           q_items_dequeued;    /* Total de itens desenfileirados */
    BK_UINT64           q_bytes_enqueued;    /* Total de bytes enfileirados */
    BK_UINT64           q_bytes_dequeued;    /* Total de bytes desenfileirados */
    BK_UINT32           q_peak_items;        /* Pico de itens */
    
    /* Timeouts */
    BK_TIME             q_read_timeout;      /* Timeout padrão para leitura */
    BK_TIME             q_write_timeout;     /* Timeout padrão para escrita */
    
    /* Dados do usuário */
    void               *q_private;           /* Dados privados */
};

typedef struct __bk_queue BK_QUEUE;

/*
 * Funções básicas
 */

/* Cria uma nova fila */
BK_QUEUE *bk_queue_create(BK_QUEUE_TYPE type, BK_UINT32 max_items, BK_UINT32 flags);

/* Destrói uma fila */
BK_I32 bk_queue_destroy(BK_QUEUE *q);

/* Enfileira um item */
BK_I32 bk_queue_enqueue(BK_QUEUE *q, void *data, BK_SIZE size, BK_UINT32 flags);

/* Desenfileira um item */
BK_I32 bk_queue_dequeue(BK_QUEUE *q, void **data, BK_SIZE *size, BK_UINT32 flags);

/* Verifica se a fila está vazia */
static __BK_ALWAYS_INLINE BK_BOOLEAN
bk_queue_is_empty(BK_QUEUE *q)
{
    return (q->q_current_items == 0);
}

/* Verifica se a fila está cheia */
static __BK_ALWAYS_INLINE BK_BOOLEAN
bk_queue_is_full(BK_QUEUE *q)
{
    return (q->q_current_items >= q->q_max_items);
}

/* Obtém número de itens na fila */
static __BK_ALWAYS_INLINE BK_UINT32
bk_queue_get_count(BK_QUEUE *q)
{
    return q->q_current_items;
}

/* Porta de IPC (para comunicação cliente-servidor) */
struct __bk_ipc_port {
	BK_IPC_ID	port_id;	/* ID da porta */
	BK_UINT32	port_type;	/* Tipo da porta */
	BK_PROCESS	*port_owner;	/* Processo proprietário */
	BK_IPC_ENDPOINT	port_endpoint;	/* Endpoint da porta */
	BK_UINT32	port_perms;	/* Permissões */
	BK_UINT32	port_flags;	/* Flags */
	BK_QUEUE	port_msg_queue;	/* Fila de mensagens */
	BK_SPINLOCK	port_lock;	/* Lock da porta */
};

typedef struct __bk_ipc_port BK_IPC_PORT;

/* Canal de IPC (para comunicação bidirecional) */
struct __bk_ipc_channel {
	BK_IPC_ID	chan_id;	/* ID do canal */
	BK_IPC_ENDPOINT	chan_endpoint1;	/* Endpoint 1 */
	BK_IPC_ENDPOINT	chan_endpoint2;	/* Endpoint 2 */
	BK_PIPE		chan_pipe_in;	/* Pipe de entrada */
	BK_PIPE		chan_pipe_out;	/* Pipe de saída */
	BK_UINT32	chan_flags;	/* Flags */
	BK_SPINLOCK	chan_lock;	/* Lock do canal */
};

typedef struct __bk_ipc_channel BK_IPC_CHANNEL;

/* Fila de mensagens de IPC */
struct __bk_ipc_queue {
	BK_IPC_ID	queue_id;	/* ID da fila */
	BK_SIZE		queue_max_msgs;	/* Máximo de mensagens */
	BK_SIZE		queue_max_size;	/* Tamanho máximo por mensagem */
	BK_SIZE		queue_cur_msgs;	/* Mensagens atuais */
	BK_SIZE		queue_cur_bytes;	/* Bytes atuais */
	BK_IPC_MESSAGE	*queue_messages;	/* Array de mensagens */
	BK_UINT32	queue_perms;	/* Permissões */
	BK_UINT32	queue_flags;	/* Flags */
	BK_PROCESS_LIST	queue_readers;	/* Processos lendo */
	BK_PROCESS_LIST	queue_writers;	/* Processos escrevendo */
	BK_SPINLOCK	queue_lock;	/* Lock da fila */
};

typedef struct __bk_ipc_queue BK_IPC_QUEUE;

/* Ponto de encontro (Rendezvous) */
struct __bk_ipc_rendezvous {
	BK_IPC_ID	rv_id;		/* ID do rendezvous */
	BK_UINT32	rv_participants;	/* Número de participantes */
	BK_UINT32	rv_required;	/* Participantes necessários */
	BK_UINT32	rv_arrived;	/* Participantes que chegaram */
	void		*rv_data;	/* Dados compartilhados */
	BK_SIZE		rv_data_size;	/* Tamanho dos dados */
	BK_THREAD_LIST	rv_waiters;	/* Threads esperando */
	BK_SPINLOCK	rv_lock;	/* Lock do rendezvous */
};

typedef struct __bk_ipc_rendezvous BK_IPC_RENDEZVOUS;

/* Barreira de IPC */
struct __bk_ipc_barrier {
	BK_IPC_ID	barrier_id;	/* ID da barreira */
	BK_UINT32	barrier_count;	/* Número de threads necessário */
	BK_UINT32	barrier_waiting;	/* Threads esperando */
	BK_UINT32	barrier_generation;	/* Geração da barreira */
	BK_THREAD_LIST	barrier_waiters;	/* Threads na barreira */
	BK_SPINLOCK	barrier_lock;	/* Lock da barreira */
};

typedef struct __bk_ipc_barrier BK_IPC_BARRIER;

/* Chamada de Procedimento Remoto (RPC) */
struct __bk_ipc_rpc {
	BK_IPC_ID	rpc_id;		/* ID da chamada RPC */
	BK_UINT32	rpc_proc_id;	/* ID do procedimento */
	BK_IPC_ID	rpc_client;	/* ID do cliente */
	BK_IPC_ID	rpc_server;	/* ID do servidor */
	void		*rpc_args;	/* Argumentos */
	BK_SIZE		rpc_args_size;	/* Tamanho dos argumentos */
	void		*rpc_result;	/* Resultado */
	BK_SIZE		rpc_result_size;	/* Tamanho do resultado */
	BK_UINT32	rpc_status;	/* Status da chamada */
	BK_EVENT	rpc_completion;	/* Evento de conclusão */
	BK_SPINLOCK	rpc_lock;	/* Lock da RPC */
};

typedef struct __bk_ipc_rpc BK_IPC_RPC;

/*
 * ============================================================================
 * ESPAÇO DE IPC (IPC SPACE)
 * ============================================================================
 */

/* Entrada na tabela de IPC */
struct __bk_ipc_entry {
	BK_IPC_ID	ie_id;		/* ID do objeto IPC */
	BK_IPC_TYPE	ie_type;	/* Tipo do objeto */
	void		*ie_object;	/* Ponteiro para o objeto */
	BK_UINT32	ie_perms;	/* Permissões */
	BK_UINT32	ie_flags;	/* Flags */
	BK_TIME		ie_creation;	/* Tempo de criação */
	BK_REFCOUNT	ie_refcount;	/* Contador de referências */
	BK_LIST_ENTRY(__bk_ipc_entry) ie_link;	/* Link na tabela */
};

typedef struct __bk_ipc_entry BK_IPC_ENTRY;

/* Tabela de IPC (por processo) */
BK_LIST_HEAD(__bk_ipc_entry_list, __bk_ipc_entry);

struct __bk_ipc_space {
	BK_PROCESS	*ips_owner;	/* Processo proprietário */
	BK_IPC_ENTRY_LIST ips_entries;	/* Lista de entradas IPC */
	BK_UI32		ips_count;	/* Número de entradas */
	BK_UI32		ips_max;	/* Máximo de entradas */
	BK_HASHTABLE	*ips_hashtable;	/* Hash table para busca rápida */
	BK_SPINLOCK	ips_lock;	/* Lock do espaço IPC */
};

typedef struct __bk_ipc_space BK_IPC_SPACE;

/*
 * ============================================================================
 * FUNÇÕES DE IPC (MODELO MICROKERNEL)
 * ============================================================================
 */

#if BK_IPC_MODEL == BK_IPC_MODEL_MICROKERNEL

/*
 * No modelo Microkernel, o IPC é o mecanismo fundamental de comunicação
 * entre serviços do usuário. Todas as operações são feitas através de
 * mensagens enviadas ao kernel.
 */

/* Criação de objetos IPC */
BK_IPC_ID bk_ipc_create(BK_IPC_TYPE type, BK_SIZE size, BK_UINT32 perms, BK_UINT32 flags);
BK_I32 bk_ipc_destroy(BK_IPC_ID ipc_id);

/* Operações de mensagem */
BK_I32 bk_ipc_send(BK_IPC_ID dest, const void *data, BK_SIZE size, BK_UINT32 flags);
BK_I32 bk_ipc_receive(BK_IPC_ID src, void *buffer, BK_SIZE size, BK_UINT32 flags);
BK_I32 bk_ipc_call(BK_IPC_ID dest, const void *in_data, BK_SIZE in_size,
		  void *out_buffer, BK_SIZE out_size, BK_UINT32 flags);

/* Memória compartilhada */
BK_IPC_ID bk_ipc_shm_create(BK_SIZE size, BK_UINT32 perms, BK_UINT32 flags);
BK_I32 bk_ipc_shm_attach(BK_IPC_ID shm_id, void **addr, BK_UINT32 flags);
BK_I32 bk_ipc_shm_detach(BK_IPC_ID shm_id);
BK_I32 bk_ipc_shm_destroy(BK_IPC_ID shm_id);

/* Portas (para servidores) */
BK_IPC_ID bk_ipc_port_create(BK_UINT32 perms, BK_UINT32 flags);
BK_I32 bk_ipc_port_listen(BK_IPC_ID port_id);
BK_I32 bk_ipc_port_accept(BK_IPC_ID port_id, BK_IPC_ID *client_id);
BK_I32 bk_ipc_port_connect(BK_IPC_ID port_id);
BK_I32 bk_ipc_port_close(BK_IPC_ID port_id);

#endif /* BK_IPC_MODEL_MICROKERNEL */

/*
 * ============================================================================
 * FUNÇÕES DE IPC (MODELO MONOLÍTICO)
 * ============================================================================
 */

#if BK_IPC_MODEL == BK_IPC_MODEL_MONOLITHIC

/*
 * No modelo Monolítico, o IPC é implementado como chamadas de sistema
 * regulares. O kernel gerencia todos os recursos IPC diretamente.
 */

/* Syscalls de IPC */
BK_I32 bk_ipc_syscall(BK_UINT32 syscall_num, ...);

/* Funções específicas (encapsulam syscalls) */
BK_IPC_ID bk_ipc_msgget(BK_KEY key, BK_UINT32 flags);
BK_I32 bk_ipc_msgsnd(BK_IPC_ID msqid, const void *msgp, BK_SIZE msgsz, BK_UINT32 flags);
BK_I32 bk_ipc_msgrcv(BK_IPC_ID msqid, void *msgp, BK_SIZE msgsz, BK_I32 msgtyp, BK_UINT32 flags);
BK_I32 bk_ipc_msgctl(BK_IPC_ID msqid, BK_I32 cmd, struct __bk_ipc_msqid_ds *buf);

BK_IPC_ID bk_ipc_semget(BK_KEY key, BK_UINT32 nsems, BK_UINT32 flags);
BK_I32 bk_ipc_semop(BK_IPC_ID semid, struct __bk_sembuf *sops, BK_SIZE nsops);
BK_I32 bk_ipc_semctl(BK_IPC_ID semid, BK_UINT32 semnum, BK_UINT32 cmd, ...);

BK_IPC_ID bk_ipc_shmget(BK_KEY key, BK_SIZE size, BK_UINT32 flags);
void *bk_ipc_shmat(BK_IPC_ID shmid, const void *shmaddr, BK_UINT32 flags);
BK_I32 bk_ipc_shmdt(const void *shmaddr);
BK_I32 bk_ipc_shmctl(BK_IPC_ID shmid, BK_UINT32 cmd, struct __bk_ipc_shmid_ds *buf);

#endif /* BK_IPC_MODEL_MONOLITHIC */

/*
 * ============================================================================
 * FUNÇÕES DE IPC (MODELO EXOKERNEL)
 * ============================================================================
 */

#if BK_IPC_MODEL == BK_IPC_MODEL_EXOKERNEL

/*
 * No modelo Exokernel, o IPC é direto entre aplicações com mínimo
 * envolvimento do kernel. O kernel apenas aloca recursos e controla acesso.
 */

/* Alocação de recursos IPC */
BK_IPC_HANDLE bk_ipc_allocate(BK_IPC_TYPE type, BK_SIZE size, BK_UINT32 flags);
BK_I32 bk_ipc_deallocate(BK_IPC_HANDLE handle);

/* Configuração direta (bypass kernel quando possível) */
BK_I32 bk_ipc_setup_direct(BK_IPC_HANDLE handle1, BK_IPC_HANDLE handle2,
			  BK_UINT32 flags);
BK_I32 bk_ipc_teardown_direct(BK_IPC_HANDLE handle);

/* Transferência direta (DMA, shared memory) */
BK_I32 bk_ipc_transfer_direct(BK_IPC_HANDLE src, BK_IPC_HANDLE dst,
			     void *data, BK_SIZE size, BK_UINT32 flags);

/* Controle de acesso */
BK_I32 bk_ipc_grant_access(BK_IPC_HANDLE handle, BK_PROCESS *proc,
			  BK_UINT32 perms);
BK_I32 bk_ipc_revoke_access(BK_IPC_HANDLE handle, BK_PROCESS *proc);

#endif /* BK_IPC_MODEL_EXOKERNEL */

/*
 * ============================================================================
 * FUNÇÕES DE IPC (MODELO NANOKERNEL)
 * ============================================================================
 */

#if BK_IPC_MODEL == BK_IPC_MODEL_NANOKERNEL

/*
 * No modelo Nanokernel, o IPC é mínimo e otimizado para desempenho.
 * Operações são atômicas e não bloqueantes quando possível.
 */

/* IPC rápido (usando hardware quando disponível) */
BK_I32 bk_ipc_fast_send(BK_IPC_ID dest, BK_UINT32 data0, BK_UINT32 data1);
BK_I32 bk_ipc_fast_receive(BK_IPC_ID src, BK_UINT32 *data0, BK_UINT32 *data1);

/* IPC atômico (sem bloqueio do kernel) */
BK_I32 bk_ipc_atomic_send(BK_IPC_ID dest, const void *data, BK_SIZE size);
BK_I32 bk_ipc_atomic_receive(BK_IPC_ID src, void *buffer, BK_SIZE size);

/* IPC em lote (múltiplas operações) */
BK_I32 bk_ipc_batch_send(BK_IPC_ID *dests, const void **datas,
			BK_SIZE *sizes, BK_UINT32 count);
BK_I32 bk_ipc_batch_receive(BK_IPC_ID *srcs, void **buffers,
			   BK_SIZE *sizes, BK_UINT32 count);

#endif /* BK_IPC_MODEL_NANOKERNEL */

/*
 * ============================================================================
 * FUNÇÕES COMUNS A TODOS OS MODELOS
 * ============================================================================
 */

/* Inicialização do sistema IPC */
BK_I32 bk_ipc_system_init(void);
void bk_ipc_system_shutdown(void);

/* Gerenciamento de espaço IPC */
BK_IPC_SPACE *bk_ipc_space_create(BK_PROCESS *proc);
BK_I32 bk_ipc_space_destroy(BK_IPC_SPACE *space);
BK_I32 bk_ipc_space_add(BK_IPC_SPACE *space, BK_IPC_ENTRY *entry);
BK_I32 bk_ipc_space_remove(BK_IPC_SPACE *space, BK_IPC_ID ipc_id);
BK_IPC_ENTRY *bk_ipc_space_lookup(BK_IPC_SPACE *space, BK_IPC_ID ipc_id);

/* Operações genéricas (implementadas para todos os modelos) */
BK_I32 bk_ipc_wait(BK_IPC_ID ipc_id, BK_TIME timeout);
BK_I32 bk_ipc_notify(BK_IPC_ID ipc_id, BK_UINT32 event);
BK_I32 bk_ipc_poll(BK_IPC_ID *ipc_ids, BK_I32 *events, BK_UINT32 count, BK_TIME timeout);

/* Sincronização */
BK_I32 bk_ipc_sync(BK_IPC_ID ipc_id, BK_UINT32 flags);
BK_I32 bk_ipc_barrier_wait(BK_IPC_ID barrier_id);
BK_I32 bk_ipc_barrier_init(BK_IPC_ID barrier_id, BK_UINT32 count);
BK_I32 bk_ipc_barrier_destroy(BK_IPC_ID barrier_id);

/* RPC (Remote Procedure Call) */
BK_I32 bk_ipc_rpc_call(BK_IPC_ID server, BK_UINT32 proc_id,
		      const void *args, BK_SIZE args_size,
		      void *result, BK_SIZE result_size,
		      BK_TIME timeout);
BK_I32 bk_ipc_rpc_register(BK_IPC_ID server, BK_UINT32 proc_id,
			  void (*handler)(void *, void *));
BK_I32 bk_ipc_rpc_unregister(BK_IPC_ID server, BK_UINT32 proc_id);

/* Segurança de IPC */
BK_I32 bk_ipc_set_perms(BK_IPC_ID ipc_id, BK_UINT32 perms);
BK_I32 bk_ipc_get_perms(BK_IPC_ID ipc_id, BK_UINT32 *perms);
BK_I32 bk_ipc_check_perms(BK_IPC_ID ipc_id, BK_UINT32 required_perms);
BK_I32 bk_ipc_seal(BK_IPC_ID ipc_id);	/* Torna IPC imutável */

/* Estatísticas e monitoramento */
struct __bk_ipc_stats {
	BK_UI64		is_total_messages;
	BK_UI64		is_total_bytes;
	BK_UI64		is_active_objects;
	BK_UI64		is_peak_objects;
	BK_UI64		is_failed_sends;
	BK_UI64		is_failed_receives;
	BK_TIME		is_total_wait_time;
	BK_UI64		is_context_switches;
};

BK_I32 bk_ipc_get_stats(struct __bk_ipc_stats *stats);
BK_I32 bk_ipc_reset_stats(void);

/* Debug e diagnóstico */
#ifdef _BK_IPC_DEBUG
void bk_ipc_dump_object(BK_IPC_ID ipc_id);
void bk_ipc_dump_space(BK_IPC_SPACE *space);
void bk_ipc_dump_system_stats(void);
BK_I32 bk_ipc_validate_object(BK_IPC_ID ipc_id);
#endif

/*
 * ============================================================================
 * MACROS E FUNÇÕES DE UTILIDADE
 * ============================================================================
 */

/* Macros para diferentes modelos */
#if BK_IPC_MODEL == BK_IPC_MODEL_MICROKERNEL
#define BK_IPC_SEND		bk_ipc_send
#define BK_IPC_RECEIVE		bk_ipc_receive
#define BK_IPC_CALL		bk_ipc_call
#elif BK_IPC_MODEL == BK_IPC_MODEL_MONOLITHIC
#define BK_IPC_SEND		bk_ipc_msgsnd
#define BK_IPC_RECEIVE		bk_ipc_msgrcv
#define BK_IPC_CALL		bk_ipc_syscall
#elif BK_IPC_MODEL == BK_IPC_MODEL_EXOKERNEL
#define BK_IPC_SEND		bk_ipc_transfer_direct
#define BK_IPC_RECEIVE		bk_ipc_transfer_direct
#define BK_IPC_CALL		bk_ipc_setup_direct
#elif BK_IPC_MODEL == BK_IPC_MODEL_NANOKERNEL
#define BK_IPC_SEND		bk_ipc_fast_send
#define BK_IPC_RECEIVE		bk_ipc_fast_receive
#define BK_IPC_CALL		bk_ipc_atomic_send
#endif

/* Macros para operações comuns */
#define BK_IPC_CREATE(type, size, perms, flags)	\
	bk_ipc_create((type), (size), (perms), (flags))

#define BK_IPC_DESTROY(id)		bk_ipc_destroy((id))

#define BK_IPC_SEND_TO(dest, data, size, flags)	\
	BK_IPC_SEND((dest), (data), (size), (flags))

#define BK_IPC_RECEIVE_FROM(src, buf, size, flags)	\
	BK_IPC_RECEIVE((src), (buf), (size), (flags))

#define BK_IPC_CALL_SERVER(srv, in, insz, out, outsz, flags)	\
	BK_IPC_CALL((srv), (in), (insz), (out), (outsz), (flags))

/* Macros para sincronização */
#define BK_IPC_LOCK(mutex)		do { \
	BK_I32 __ret = bk_ipc_wait((mutex), BK_TIME_INFINITE); \
	if (__ret != BK_SUCCESS) { /* handle error */ } \
} while (0)

#define BK_IPC_UNLOCK(mutex)		bk_ipc_notify((mutex), 0)

#define BK_IPC_WAIT(cond, mutex)	do { \
	BK_IPC_UNLOCK(mutex); \
	bk_ipc_wait((cond), BK_TIME_INFINITE); \
	BK_IPC_LOCK(mutex); \
} while (0)

#define BK_IPC_SIGNAL(cond)		bk_ipc_notify((cond), 1)

/* Macros para RPC */
#define BK_IPC_RPC_DECLARE(proc_id, ret_type, name, ...)	\
	ret_type name(__VA_ARGS__)

#define BK_IPC_RPC_CALL(server, proc_id, ...)	\
	bk_ipc_rpc_call((server), (proc_id), __VA_ARGS__)

#define BK_IPC_RPC_REGISTER(server, proc_id, handler)	\
	bk_ipc_rpc_register((server), (proc_id), (handler))

/* Funções inline de utilidade */
static __BK_ALWAYS_INLINE BK_BOOLEAN
bk_ipc_id_valid(BK_IPC_ID id)
{
	return (id != 0 && id != BK_ID_INVALID);
}

static __BK_ALWAYS_INLINE BK_UINT32
bk_ipc_calculate_crc32(const void *data, BK_SIZE size)
{
	/* Implementação CRC32 otimizada */
	const BK_UI8 *bytes = (const BK_UI8 *)data;
	BK_UINT32 crc = 0xFFFFFFFF;
	
	for (BK_SIZE i = 0; i < size; i++) {
		crc ^= bytes[i];
		for (BK_UI32 j = 0; j < 8; j++) {
			crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
		}
	}
	
	return ~crc;
}

static __BK_ALWAYS_INLINE BK_I32
bk_ipc_validate_message(const BK_IPC_MESSAGE *msg)
{
	if (!msg || !msg->msg_data || msg->msg_size == 0)
		return BK_IPC_EINVAL;
	
	if (msg->msg_size > BK_IPC_MAX_MSG_SIZE)
		return BK_IPC_EMSGSIZE;
	
	return BK_SUCCESS;
}

/*
 * ============================================================================
 * CONSTANTES E DEFINIÇÕES
 * ============================================================================
 */

/* Tamanhos máximos */
#define BK_IPC_MAX_MSG_SIZE		(64 * 1024)	/* 64KB */
#define BK_IPC_MAX_SHM_SIZE		(256 * 1024 * 1024)	/* 256MB */
#define BK_IPC_MAX_PIPE_SIZE		(16 * 1024)	/* 16KB */
#define BK_IPC_MAX_MAILBOX_MSGS		1024
#define BK_IPC_MAX_QUEUE_MSGS		8192

/* Timeouts especiais */
#define BK_IPC_TIMEOUT_IMMEDIATE	0
#define BK_IPC_TIMEOUT_INFINITE		((BK_TIME)-1)

/* Códigos de erro IPC */
#define BK_IPC_SUCCESS			0
#define BK_IPC_EINVAL			1	/* Argumento inválido */
#define BK_IPC_ENOMEM			2	/* Sem memória */
#define BK_IPC_ENOSPC			3	/* Sem espaço */
#define BK_IPC_EEXIST			4	/* Já existe */
#define BK_IPC_ENOENT			5	/* Não existe */
#define BK_IPC_EACCES			6	/* Acesso negado */
#define BK_IPC_EPERM			7	/* Permissão negada */
#define BK_IPC_EBUSY			8	/* Recurso ocupado */
#define BK_IPC_EAGAIN			9	/* Tente novamente */
#define BK_IPC_EMSGSIZE			10	/* Tamanho de mensagem inválido */
#define BK_IPC_ENOMSG			11	/* Sem mensagem */
#define BK_IPC_ETIMEDOUT		12	/* Timeout */
#define BK_IPC_ECONNREFUSED		13	/* Conexão recusada */
#define BK_IPC_ECONNRESET		14	/* Conexão resetada */
#define BK_IPC_EISCONN			15	/* Já conectado */
#define BK_IPC_ENOTCONN			16	/* Não conectado */
#define BK_IPC_ESHUTDOWN		17	/* Shutdown */
#define BK_IPC_EPIPE			18	/* Pipe quebrado */

/* Constantes mágicas */
#define BK_IPC_MAGIC_NUMBER		0x42494F53	/* "BIOS" */
#define BK_IPC_VERSION_CURRENT		1

#endif /* !_BIBLE_IPC_H_ */
