#ifndef _BIBLE_IPC_H_
#define _BIBLE_IPC_H_

#include <bible/cdefs.h>
#include <bible/types.h>
#include <bible/queue.h>
#include <bible/task.h>


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